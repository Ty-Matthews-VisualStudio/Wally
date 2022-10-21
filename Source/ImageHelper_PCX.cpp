/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  ImageHelper_PCX.cpp : implementation of the CImageHelper PCX functions
//
//  Created by Ty Matthews, 6-28-2001
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "Wally.h"
#include "TextureFlags.h"
#include "WallyPal.h"
#include "ColorOpt.h"
#include "WADList.h"
#include "RegistryHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////
//   PCX Support
////////////////////////////////////////////////////////////////////////////////////

BOOL CImageHelper::DecodePCX( int iFlags /* = 0 */ )
{
	BYTE* p_RawData     = NULL;	
	BYTE* p_TempPalette = NULL;
	BYTE* p_Pixel       = NULL;
	
	CString szErrorMessage;
	int iSize		= 0;
	int dataByte	= 0;
	int y, x, j;
	int iRunLength	= 0;		
	int iLength = GetFileLength();
	int iDepth		= 0;

	m_pPcxHeader = (PCXHEADER *)m_pbyEncodedData;
	p_RawData = &m_pPcxHeader->byData;
	
	// Check for valid PCX type

	if (m_pPcxHeader->byManufacturer != 0x0a
			|| m_pPcxHeader->byVersion != 5
			|| m_pPcxHeader->byEncoding != 1
			|| m_pPcxHeader->byBitsPerPixelPlane != 8)
	{
		SetErrorCode( IH_PCX_INCOMPATIBLE);	
		return (FALSE);
	}

	int iWidth  = (m_pPcxHeader->wXmax) - (m_pPcxHeader->wXmin) + 1;		
	int iHeight = (m_pPcxHeader->wYmax) - (m_pPcxHeader->wYmin) + 1;

//	int iPaddedWidth = PadWORD( iWidth);	// WORD padded ?
//	int iPaddedWidth = PadDWORD( iWidth);	// DWORD padded ???
	int iPaddedWidth = m_pPcxHeader->wBytesPerLine;

	SetImageWidth( iWidth);
	SetImageHeight( iHeight);
	
	if (m_pPcxHeader->byBitsPerPixelPlane == 8) 
	{
		switch (m_pPcxHeader->byNumPlanes)
		{
			case 1:	// Image is 8-bit 256 colors
				SetColorDepth( IH_8BIT);
				memcpy( m_byPalette, m_pbyEncodedData + iLength - 768, 768);			
				//iSize = iWidth * iHeight;
				iSize = iPaddedWidth * iHeight;
				break;
			
			case 3:	// Image is 24-bit 16m colors
				SetColorDepth( IH_24BIT);			
				//iSize = iWidth * iHeight * 3;
				iSize = iPaddedWidth * iHeight * 3;
				break;

			default : 
				SetErrorCode( IH_PCX_BAD_COLOR_PLANES);			
				return (FALSE);
		}
	}
	else
	{
		ASSERT( FALSE);		// Should not ever get here; something happen to the "check
							// for valid PCX type" condition?
		SetErrorCode( IH_PCX_INCOMPATIBLE);		
		return (FALSE);
	}

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	iDepth = GetColorDepth();	
	
	m_pbyDecodedData = new BYTE[iSize];

	if (! m_pbyDecodedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);			
		return (FALSE);
	}
	memset (m_pbyDecodedData, 0, iSize);

	p_Pixel = m_pbyDecodedData;
	
	// Decode the PCX data

	iRunLength = 0;
		
	switch (iDepth)
	{
	case IH_8BIT :	
		for (y = 0; y < iHeight; y++, p_Pixel += iWidth)
		{
			for (x = 0; x < iPaddedWidth; )
			{
				if (iRunLength <= 0)
				{
					dataByte = *p_RawData++;

					if((dataByte & 0xC0) == 0xC0)
					{
						iRunLength = dataByte & 0x3F;
						dataByte = *p_RawData++;
					}
					else
						iRunLength = 1;
				}

				while (iRunLength-- > 0)
				{
					// Neal - some pcx files wrap the runlength
					// data from one line to the next

//					if (x >= iWidth)
//					{						
//						SetErrorCode (IH_PCX_MALFORMED);
//						return FALSE;
//					}

					if (x < iWidth)
						p_Pixel[x] = dataByte;
					else if (x == iPaddedWidth)
					{
						break;
					}
					x++;
				}
			}
		}
		break;

	case IH_24BIT :
		
		unsigned char *ScanLine[3];
		ScanLine[RED]   = new BYTE[iWidth];
		ScanLine[GREEN] = new BYTE[iWidth];
		ScanLine[BLUE]  = new BYTE[iWidth];

		for (y = 0; y < iHeight; y++, p_Pixel += (iWidth * 3))
		{
			for (j = 0; j < 3; j++)  // Read in the three scan lines
			{
				for (x = 0; x < iPaddedWidth;)
				{
					if (iRunLength <= 0)
					{
						dataByte = *p_RawData++;

						if((dataByte & 0xC0) == 0xC0)
						{
							iRunLength = dataByte & 0x3F;
							dataByte = *p_RawData++;
						}
						else
							iRunLength = 1;
					}

					while (iRunLength-- > 0)
					{
						// Neal - some pcx files wrap the runlength
						// data from one line to the next

//						if (x >= iWidth)
//						{
//							for (j = 0; j < 3; j++)
//							{
//								if (ScanLine[j])
//									delete [] ScanLine[j];
//							}
//							SetErrorCode (IH_PCX_MALFORMED);
//							return FALSE;
//						}

						if (x < iWidth)
							ScanLine[j][x] = dataByte;
						else if (x == iPaddedWidth)
						{
							break;
						}
						x++;
					}
				}		
			}

			for (x = 0; x < iWidth; x++)
			{
				p_Pixel[x * 3]	   = ScanLine[RED][x];
				p_Pixel[x * 3 + 1] = ScanLine[GREEN][x];
				p_Pixel[x * 3 + 2] = ScanLine[BLUE][x];
			}

		}

		for (j = 0; j < 3; j++)
		{
			if (ScanLine[j])
				delete [] ScanLine[j];
		}
		break;

	default:
		ASSERT( FALSE);		// Unsupported color depth... missed implementation?
		SetErrorCode (IH_PCX_INCOMPATIBLE);		
		return (FALSE);
		break;
	}

	p_Pixel = NULL;	

	if (p_RawData - (BYTE *)m_pPcxHeader > iLength)
	{
		p_RawData    = NULL;
		m_pPcxHeader = NULL;
		SetErrorCode( IH_PCX_MALFORMED);		
		return (FALSE);
	}	

	p_RawData = NULL;
	return (TRUE);
}

BOOL CImageHelper::EncodePCX()
{
	int	  i, j, k, l;
	BYTE* pack					= NULL;	
	BYTE* pbyPackedData	= NULL;
	BYTE  p_ThisByte;
	BYTE  p_LastByte;
		
	int iWidth	= GetImageWidth();
	int	iHeight = GetImageHeight();
	int	iDataSize					= 0;	
	int iPackedLength				= 0;
	int iRunLength					= 0;
	int iOffset						= 0;
	int iDepth = GetColorDepth();

	CMemBuffer mbData;

	iDataSize = iWidth * iHeight * 2 + 1000;

	iDataSize *= (iDepth == IH_24BIT) ? 3 : 1;
	
	pbyPackedData = mbData.GetBuffer( iDataSize );
	if (! pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	//memset (pbyPackedData, 0, iDataSize);
	
	m_pPcxHeader = (PCXHEADER *)pbyPackedData;

	WORD wBytesPerLine = (iWidth * GetNumBitsPerPixelPlane()) / 8;
	wBytesPerLine      = (wBytesPerLine + 1) & ~1;		// WORD padded

	m_pPcxHeader->byManufacturer      = 0x0A;	// ZSoft PCX file id
	m_pPcxHeader->byVersion           = 5;		// 256 color (with palette)
 	m_pPcxHeader->byEncoding          = 1;		// standard run-length encoding
	m_pPcxHeader->byBitsPerPixelPlane = 8;		// 256 color
	m_pPcxHeader->wXmin = 0;
	m_pPcxHeader->wYmin = 0;
	m_pPcxHeader->wXmax = (short )(iWidth-1);
	m_pPcxHeader->wYmax = (short )(iHeight-1);
	m_pPcxHeader->wHdpi = 92;				// just use screen res
	m_pPcxHeader->wVdpi = 92;
	m_pPcxHeader->wBytesPerLine = (short)iWidth;
	m_pPcxHeader->wPalInfo      = 1;		// not a grey scale image

	//int iPaddedWidth = PadWORD( iWidth);	// WORD padded

	switch (iDepth)
	{
	case IH_8BIT:
		m_pPcxHeader->byNumPlanes = 1;
		
		// pack the image
		pack = &m_pPcxHeader->byData;
	
		for (i = 0; i < iHeight; i++)
		{		
			p_LastByte = *(m_pbyDecodedData);
			iRunLength = 1;

			for (j = 1; j < iWidth; j++)
//			for (j = 1; j < iPaddedWidth; j++)
			{
				if (j < iWidth)
					p_ThisByte = *(++m_pbyDecodedData);
				// else, just repeat previous pixel (for WORD padding)
				
				if (p_ThisByte == p_LastByte)
				{
					iRunLength++;				
				
					if (iRunLength == 63)
					{
						*pack++ = 0xC0 | iRunLength;
						*pack++ = p_LastByte;
						iRunLength = 0;					
					}				
				}
				else
				{
					if (iRunLength)
					{				
						// If there's only one byte, and it's less than 192, just move it in
						// as no coded runlength
						if ((iRunLength == 1) && ((p_LastByte & 0xC0) != 0xC0))
						{
							*pack++ = p_LastByte;
						}
						else
						{				
							*pack++ = 0xC0 | iRunLength;
							*pack++ = p_LastByte;						
						}
					}
					p_LastByte = p_ThisByte;
					iRunLength = 1;
				}
			
			}
			if (iRunLength)
			{				
				// If there's only one byte, and it's less than 192, just move it in
				// as no coded runlength

				if ((iRunLength == 1) && ((p_LastByte & 0xc0) != 0xc0))
				{
					*pack++ = p_LastByte;
				}
				else
				{				
					*pack++ = 0xc0 | iRunLength;
					*pack++ = p_LastByte;						
				}	
			}	
			m_pbyDecodedData++;
		}

		// write the palette
		*pack++ = 0x0C;	// palette ID byte
		
		memcpy( pack, m_byPalette, 768 );
		pack += 768;		
		break;

		
	case IH_24BIT:		
		m_pPcxHeader->byNumPlanes = 3;	

		// pack the image
		pack = &m_pPcxHeader->byData;
		BYTE* ScanLine[3];
		ScanLine[RED]   = new BYTE[iWidth];		
		ScanLine[GREEN] = new BYTE[iWidth];
		ScanLine[BLUE]  = new BYTE[iWidth];
		
		for (i = 0; i < iHeight; i++)
		{				
			// Grab all three color-lines

			for (j = 0, l = 0; j < (iWidth * 3); j += 3, l++)
			{
				iOffset = (i * (iWidth * 3)) + j;
				ScanLine[RED][l]   = *(m_pbyDecodedData + iOffset);
				ScanLine[GREEN][l] = *(m_pbyDecodedData + iOffset + 1);
				ScanLine[BLUE][l]  = *(m_pbyDecodedData + iOffset + 2);
			}
		
			for (k = 0; k < 3; k++)
			{				
				iRunLength = 1;
				p_LastByte = ScanLine[k][0];

				for (j = 1; j < iWidth; j++)
//				for (j = 1; j < iPaddedWidth; j++)
				{
					if (j < iWidth)
						p_ThisByte = ScanLine[k][j];
					// else, just repeat previous pixel (for WORD padding)
					
					if (p_ThisByte == p_LastByte)
					{
						iRunLength++;				
				
						if (iRunLength == 63)
						{
							*pack++ = 0xC0 | iRunLength;
							*pack++ = p_LastByte;
							iRunLength = 0;					
						}				
					}
					else
					{
						if (iRunLength)
						{				
							// If there's only one byte, and it's less than 192, just move it in
							// as no coded runlength
							if ((iRunLength == 1) && ((p_LastByte & 0xC0) != 0xC0))
							{
								*pack++ = p_LastByte;
							}
							else
							{				
								*pack++ = 0xC0 | iRunLength;
								*pack++ = p_LastByte;						
							}
						}
						p_LastByte = p_ThisByte;
						iRunLength = 1;
					}
				}
				if (iRunLength)
				{				
					// If there's only one byte, and it's less than 192, just move it in
					// as no coded runlength

					if ((iRunLength == 1) && ((p_LastByte & 0xC0) != 0xC0))
					{
						*pack++ = p_LastByte;
					}
					else
					{				
						*pack++ = 0xC0 | iRunLength;
						*pack++ = p_LastByte;						
					}
				}	
			}
		}
		for (j = 0; j < 3; j++)
		{
			if (ScanLine[j])
				delete [] ScanLine[j];	// neal - must use array delete to delete arrays!
		}
		break;

	default:
		ASSERT(FALSE);		// Unhandled color depth
		break;
	}	
			
	// write output file 
	iPackedLength = pack - pbyPackedData;
	mbData.SetDataSize( iPackedLength );

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iPackedLength);
	}
	else
	{	
		try
		{			
			mbData.WriteToFile( m_strFileName );
		}
		catch( CWallyException we )
		{
			SetErrorCode( IH_ERROR_WRITING_FILE );
			return FALSE;
		}		
	}	
	
	return TRUE;
}