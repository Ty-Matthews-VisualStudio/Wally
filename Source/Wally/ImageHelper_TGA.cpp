/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  ImageHelper.cpp : implementation of the CImageHelper class
//
//  Created by Ty Matthews, 5-8-2001
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
//   TGA Support
////////////////////////////////////////////////////////////////////////////////////

BOOL CImageHelper::DecodeTGA( int iFlags /* = 0 */ )
{
	struct TGA_Header *tgaHeader;	
	tgaHeader = (struct TGA_Header *)m_pbyEncodedData;
	BOOL bCompressed	= FALSE;
	BOOL bGreyScale		= FALSE;
	
	switch (tgaHeader->ImageType)
	{
	case 0:
		SetErrorCode (IH_TGA_MALFORMED);
		return FALSE;
		break;

	case 1:
		if (tgaHeader->ColorMapType != 1)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = FALSE;
		break;

	case 2:
		if (tgaHeader->ColorMapType != 0)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = FALSE;
		break;

	case 3:
		if (tgaHeader->ColorMapType != 0)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = FALSE;
		bGreyScale  = TRUE;
		break;

	case 9:
		if (tgaHeader->ColorMapType != 1)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = TRUE;
		break;

	case 10:
		if (tgaHeader->ColorMapType != 0)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = TRUE;
		break;

	case 11:
		if (tgaHeader->ColorMapType != 0)
		{
			SetErrorCode (IH_TGA_MALFORMED);
			return FALSE;
		}
		bCompressed = TRUE;
		bGreyScale  = TRUE;
		break;

	default:
		SetErrorCode (IH_TGA_MALFORMED);
		return FALSE;
		break;
	}
		
	unsigned short *iFirstEntryIndex	= (unsigned short *)(m_pbyEncodedData + 3);
	unsigned short *iColorMapLength		= (unsigned short *)(m_pbyEncodedData + 5);
	unsigned char  *cColorMapEntrySize	= m_pbyEncodedData + 7;
	
	int iWidth = tgaHeader->Width;
	int iHeight = tgaHeader->Height;
	int iSize = 0;
	int r, g, b;

	/*
	div_t x_result = div(iWidth, 16);
	div_t y_result = div(iHeight, 16);

	// Make sure it's of valid size
	if ((x_result.rem != 0) || (y_result.rem != 0))
	{
		SetErrorCode (IH_NOT_DIV16);
		return FALSE;
	}
	*/
	
	SetImageWidth (iWidth);
	SetImageHeight (iHeight);

	switch (tgaHeader->PixelDepth)
	{
	case 8:
		SetColorDepth(IH_8BIT);
		iSize = iWidth * iHeight;
		break;

	case 16:
		// We'll convert 16 to 24-bit
		SetColorDepth(IH_24BIT);
		iSize = iWidth * iHeight * 3;		// Neal - *not* DWORD padded
		break;

	case 24:
		SetColorDepth(IH_24BIT);
		iSize = iWidth * iHeight * 3;		// *not* DWORD padded
		break;
	
	case 32:
		// TODO:  Do something with the alpha channel?
		SetColorDepth(IH_32BIT);
		iSize = iWidth * iHeight * 4;
		break;
	
	default:
		SetErrorCode (IH_TGA_UNSUPPORTED_BIT_DEPTH);
		return FALSE;
		break;
	}

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}
	
	BYTE *pbyTempDecodedData = new BYTE[iSize];
	if (!pbyTempDecodedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);			
		return FALSE;
	}
	memset (pbyTempDecodedData, 0, iSize);
	
	BYTE* pbyData = NULL;	
	BYTE* pbyPixel = pbyTempDecodedData;
	BYTE dataByte;
	BOOL bRLEPacket = FALSE;
	int x = 0;
	int y = 0;
	int z = 0;
	int iRunLength = 0;
	int iOffset = 0;
	int iColorMapSize = 0;

	switch (tgaHeader->PixelDepth)
	{
	case 8:
		{
			iOffset = sizeof (struct TGA_Header) + tgaHeader->ID_Length;
			DWORD *dwPaletteEntry;
			char cTemp;

			if (!bGreyScale)
			{
				switch (*cColorMapEntrySize)
				{			
				case 32:			
					iColorMapSize = (*iColorMapLength) * 4;
					for (x = (*iFirstEntryIndex), y = 0; x < (*iColorMapLength); x++, y++)
					{
						//memcpy (m_byPalette + (y * 3), m_pbyEncodedData + iOffset + (x * 4), 3);
						m_byPalette[ (y * 3) + 0 ] = m_pbyEncodedData[ iOffset + (x * 4) + 0 ];
						m_byPalette[ (y * 3) + 1 ] = m_pbyEncodedData[ iOffset + (x * 4) + 1 ];
						m_byPalette[ (y * 3) + 2 ] = m_pbyEncodedData[ iOffset + (x * 4) + 2 ];
					}

					for (x = 0; x < (*iColorMapLength); x++)
					{
						cTemp = m_byPalette[x * 3 + 0];
						m_byPalette[x * 3 + 0] = m_byPalette[x * 3 + 2];
						m_byPalette[x * 3 + 2] = cTemp;
					}
					break;
					
				case 24:
					iColorMapSize = (*iColorMapLength) * 3;
					memcpy (m_byPalette, m_pbyEncodedData + iOffset + ((*iFirstEntryIndex) * 3), (iColorMapSize - (*iFirstEntryIndex) * 3));
					
					for (x = 0; x < (*iColorMapLength); x++)
					{
						cTemp = m_byPalette[x * 3 + 0];
						m_byPalette[x * 3 + 0] = m_byPalette[x * 3 + 2];
						m_byPalette[x * 3 + 2] = cTemp;
					}

					break;

				case 15:
				case 16:
					iColorMapSize = (*iColorMapLength) * 2;
					dwPaletteEntry = (DWORD *)(m_pbyEncodedData + iOffset + ((*iFirstEntryIndex) * 2));
					
					for (x = (*iFirstEntryIndex), y = 0; x < (*iColorMapLength); x++, y++, dwPaletteEntry += 2)
					{
						b = (((*dwPaletteEntry) & 0x7c00) >> 10) * 8;
						g = (((*dwPaletteEntry) & 0x03e0) >> 5) * 8;
						r = ((*dwPaletteEntry) & 0x001f) * 8;

						m_byPalette[y * 3 + 0] = r;
						m_byPalette[y * 3 + 1] = g;
						m_byPalette[y * 3 + 2] = b;
					}
					break;

				default:
					SetErrorCode (IH_TGA_UNSUPPORTED_PALETTE);
					if (pbyTempDecodedData)
					{
						delete [] pbyTempDecodedData;
						pbyTempDecodedData = NULL;
					}
					return FALSE;
					break;
				}
			}
			else
			{
				for (x = 0; x < 256; x++)
				{
					memset (m_byPalette + (x * 3), x, 3);
				}
			}

			iOffset = sizeof (struct TGA_Header) + tgaHeader->ID_Length + iColorMapSize;
			pbyData = m_pbyEncodedData + iOffset;

			if (bCompressed)
			{
				for (y = 0; y < iHeight; y++, pbyPixel += iWidth)
				{
					for (x = 0; x < iWidth;)
					{
						dataByte = *pbyData;

						bRLEPacket = ((dataByte >> 7) == 1);
						
						dataByte = *pbyData;
						iRunLength = (dataByte & 0x7f) + 1;

						if (iRunLength > iWidth)
						{
							SetErrorCode (IH_TGA_MALFORMED);
							if (pbyTempDecodedData)
							{
								delete [] pbyTempDecodedData;
								pbyTempDecodedData = NULL;
							}
							return FALSE;
						}			
						
						pbyData++;

						for (z = 0; z < iRunLength; z++)
						{
							pbyPixel[x++] = *pbyData;						
							if (!bRLEPacket)
							{
								pbyData++;
							}

							if (x > iWidth)
							{
								SetErrorCode (IH_TGA_MALFORMED);
								if (pbyTempDecodedData)
								{
									delete [] pbyTempDecodedData;
									pbyTempDecodedData = NULL;
								}
								return FALSE;
							}			
						}
						
						if (bRLEPacket)
						{
							pbyData++;
						}
					}
				}
			}
			else
			{	
				// Not compressed
				memcpy (pbyTempDecodedData, pbyData, iSize);
			}
			
			m_pbyDecodedData = new BYTE[iSize];
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);			
				return FALSE;
			}
			memset (m_pbyDecodedData, 0, iSize);
			
			BYTE* pbyPixelLine   = NULL;
			BYTE* pbyDecodedLine = m_pbyDecodedData;
			
			//  Targa files are stored bottom-up; we need to flip them around
			for (y = (iHeight - 1); y >= 0; y--)
			{	
				pbyDecodedLine = m_pbyDecodedData + (y * iWidth);
				pbyPixelLine = pbyTempDecodedData + ((iHeight - y - 1) * iWidth);
				memcpy (pbyDecodedLine, pbyPixelLine, iWidth);
			}
			
		}
		break;

	case 16:
		{
			pbyData = m_pbyEncodedData + sizeof (struct TGA_Header) + tgaHeader->ID_Length;
			DWORD *dwPixel;

			if (bCompressed)
			{
				for (y = 0; y < iHeight; y++, pbyPixel += iWidth * 3)
				{
					for (x = 0; x < iWidth; )
					{
						dataByte = *pbyData;

						bRLEPacket = ((dataByte >> 7) == 1);
						
						dataByte = *pbyData;
						iRunLength = (dataByte & 0x7f) + 1;

						if (iRunLength > iWidth)
						{
							SetErrorCode (IH_TGA_MALFORMED);
							if (pbyTempDecodedData)
							{
								delete [] pbyTempDecodedData;
								pbyTempDecodedData = NULL;
							}
							return FALSE;
						}			
						
						pbyData++;

						for (z = 0; z < iRunLength; z++)
						{
							dwPixel = (DWORD *)pbyData;
							r = (((*dwPixel) & 0x7c00) >> 10) * 8;
							g = (((*dwPixel) & 0x03e0) >> 5) * 8;
							b = ((*dwPixel) & 0x001f) * 8;
							
							pbyPixel[x * 3 + 0] = r;
							pbyPixel[x * 3 + 1] = g;
							pbyPixel[x * 3 + 2] = b;						
							x++;

							if (!bRLEPacket)
							{
								pbyData += 2;
							}

							if (x > (iWidth * 3))
							{
								SetErrorCode (IH_TGA_MALFORMED);
								if (pbyTempDecodedData)
								{
									delete [] pbyTempDecodedData;
									pbyTempDecodedData = NULL;
								}
								return FALSE;
							}			
						}
						if (bRLEPacket)
						{
							pbyData += 2;
						}
					}
				}
			}
			else
			{
				// Not compressed
				for (x = 0; x < (iWidth * iHeight * 2); x += 2, pbyData += 2, pbyPixel += 3)
				{
					dwPixel = (DWORD *)pbyData;
					r = (((*dwPixel) & 0x7c00) >> 10) * 8;
					g = (((*dwPixel) & 0x03e0) >> 5) * 8;
					b = ((*dwPixel) & 0x001f) * 8;

					pbyPixel[0] = r;
					pbyPixel[1] = g;
					pbyPixel[2] = b;
				}

			}

			m_pbyDecodedData = new BYTE[iSize];
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);			
				return FALSE;
			}

			memcpy( m_pbyDecodedData, pbyTempDecodedData, iSize );

#if 0
			//memset (m_pbyDecodedData, 0, iSize);			
			
			BYTE* pbyPixelLine   = NULL;
			BYTE* pbyDecodedLine = m_pbyDecodedData;			

			iWidth *= 3; 

			//  Targa files are stored bottom-up; we need to flip them around
			for (y = (iHeight - 1); y >= 0; y--)
			{	
				pbyDecodedLine = m_pbyDecodedData + (y * iWidth);
				pbyPixelLine = pbyTempDecodedData + ((iHeight - y - 1) * iWidth);				
				memcpy (pbyDecodedLine, pbyPixelLine, iWidth);
			}
#endif
		}
		break;

	case 24:
		{
			pbyData = m_pbyEncodedData + sizeof (struct TGA_Header) + tgaHeader->ID_Length;

			if (bCompressed)
			{
				for (y = 0; y < iHeight; y++)
				{
					for (x = 0; x < (iWidth * 3); )
					{
						dataByte = *pbyData;

						bRLEPacket = ((dataByte >> 7) == 1);
						
						dataByte = *pbyData;
						iRunLength = (dataByte & 0x7f) + 1;

						if (iRunLength > iWidth)
						{
							SetErrorCode (IH_TGA_MALFORMED);
							if (pbyTempDecodedData)
							{
								delete [] pbyTempDecodedData;
								pbyTempDecodedData = NULL;
							}
							return FALSE;
						}			
						
						pbyData++;

						for (z = 0; z < iRunLength; z++)
						{
							//memcpy (pbyPixel, pbyData, 3);
							pbyPixel[0] = pbyData[0];
							pbyPixel[1] = pbyData[1];
							pbyPixel[2] = pbyData[2];

							pbyPixel += 3;
							x += 3;
							if (!bRLEPacket)
							{
								pbyData += 3;					
							}

							if (x > (iWidth * 3))
							{
								SetErrorCode (IH_TGA_MALFORMED);
								if (pbyTempDecodedData)
								{
									delete [] pbyTempDecodedData;
									pbyTempDecodedData = NULL;
								}
								return FALSE;
							}			
						}
						if (bRLEPacket)
						{
							pbyData += 3;
						}
					}
				}
			}
			else
			{
				// Not compressed
				memcpy (pbyTempDecodedData, pbyData, iSize);
			}

			m_pbyDecodedData = new unsigned char[iSize];
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);			
				return FALSE;
			}
			memset (m_pbyDecodedData, 0, iSize);
			
			unsigned char *pbyPixelLine = NULL;
			unsigned char *pbyDecodedLine = m_pbyDecodedData;
			char cTemp;

			iWidth *= 3; 

			//  Targa files are stored bottom-up; we need to flip them around
			for (y = (iHeight - 1); y >= 0; y--)
			{	
				pbyDecodedLine = m_pbyDecodedData + (y * iWidth);
				pbyPixelLine = pbyTempDecodedData + ((iHeight - y - 1) * iWidth);
				
				// 24-bit Targa files store their color data BGR, flip it to RGB so that any function that
				// retrieves this data has a common method for interpreting it.
				for (x = 0; x < iWidth; x += 3)
				{
					cTemp = pbyPixelLine[x + 2];
					pbyPixelLine[x + 2] = pbyPixelLine[x];
					pbyPixelLine[x] = cTemp;
				}
				memcpy (pbyDecodedLine, pbyPixelLine, iWidth);
			}
		}
		break;
/*
	case 32:
		{
			pbyData = m_pbyEncodedData + sizeof (struct TGA_Header) + tgaHeader->ID_Length;

			if (bCompressed)
			{
				for (y = 0; y < iHeight; y++)
				{
					for (x = 0; x < (iWidth * 3); )
					{
						dataByte = *pbyData;

						bRLEPacket = ((dataByte >> 7) == 1);
						
						dataByte = *pbyData;
						iRunLength = (dataByte & 0x7f) + 1;

						if (iRunLength > iWidth)
						{
							SetErrorCode (IH_TGA_MALFORMED);
							if (pbyTempDecodedData)
							{
								delete [] pbyTempDecodedData;
								pbyTempDecodedData = NULL;
							}
							return FALSE;
						}			
						
						pbyData++;

						for (z = 0; z < iRunLength; z++)
						{
							memcpy (pbyPixel, pbyData, 3);
							pbyPixel += 3;
							x += 3;
							if (!bRLEPacket)
							{
								pbyData += 4;
							}

							if (x > (iWidth * 3))
							{
								SetErrorCode (IH_TGA_MALFORMED);
								if (pbyTempDecodedData)
								{
									delete [] pbyTempDecodedData;
									pbyTempDecodedData = NULL;
								}
								return FALSE;
							}			
						}
						if (bRLEPacket)
						{
							pbyData += 4;
						}
					}
				}
			}
			else
			{
				// Not compressed
				
				int iTempSize = iWidth * iHeight;
				int i32Offset = 0;
				int i24Offset = 0;
				int j = 0;				

				for (j = 0; j < iTempSize; j++)
				{
					i32Offset = j * 4;
					i24Offset = j * 3;
					memcpy (pbyTempDecodedData + i24Offset, pbyData + i32Offset, 3);
				}				
			}

			m_pbyDecodedData = new unsigned char[iSize];
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);			
				return FALSE;
			}
			memset (m_pbyDecodedData, 0, iSize);
			
			unsigned char *pbyPixelLine = NULL;
			unsigned char *pbyDecodedLine = m_pbyDecodedData;
			char cTemp;

			iWidth *= 3; 

			//  Targa files are stored bottom-up; we need to flip them around
			for (y = (iHeight - 1); y >= 0; y--)
			{	
				pbyDecodedLine = m_pbyDecodedData + (y * iWidth);
				pbyPixelLine = pbyTempDecodedData + ((iHeight - y - 1) * iWidth);
				
				// 24-bit Targa files store their color data BGR, flip it to RGB so that any function that
				// retrieves this data has a common method for interpreting it.
				for (x = 0; x < iWidth; x += 3)
				{
					cTemp = pbyPixelLine[x + 2];
					pbyPixelLine[x + 2] = pbyPixelLine[x];
					pbyPixelLine[x] = cTemp;
				}
				memcpy (pbyDecodedLine, pbyPixelLine, iWidth);
			}
		}
*/
	case 32:
		{
			pbyData = m_pbyEncodedData + sizeof (struct TGA_Header) + tgaHeader->ID_Length;

#ifdef _DEBUG
			// Neal - only show a limited number of pixels
			// (otherwise, OutputDebug buffer overflows)

			int iMask = 0;		// show every pixel
			int iMax = max( iWidth, iHeight);

			if (iMax > 128)
				iMask = 15;		// only show every sizteenth pixel
			else if (iMax > 64)
				iMask = 7;		// only show every eighth pixel
			else if (iMax > 32)
				iMask = 3;		// only show every forth pixel
			else if (iMax > 16)
				iMask = 1;		// only show every other pixel
#endif
			if (bCompressed)
			{
				for (y = 0; y < iHeight; y++)
				{
					for (x = 0; x < (iWidth * 4); )
					{
						dataByte = *pbyData;

						bRLEPacket = ((dataByte >> 7) == 1);
						
						dataByte = *pbyData;
						iRunLength = (dataByte & 0x7f) + 1;

						if (iRunLength > iWidth)
						{
							SetErrorCode (IH_TGA_MALFORMED);
							if (pbyTempDecodedData)
							{
								delete [] pbyTempDecodedData;
								pbyTempDecodedData = NULL;
							}
							return FALSE;
						}			
						
						pbyData++;

						for (z = 0; z < iRunLength; z++)
						{
							//memcpy (pbyPixel, pbyData, 4);
							pbyPixel[0] = pbyData[0];
							pbyPixel[1] = pbyData[1];
							pbyPixel[2] = pbyData[2];
							pbyPixel[3] = pbyData[3];

							pbyPixel += 4;
							x += 4;
							if (!bRLEPacket)
							{
								pbyData += 4;
							}

							if (x > (iWidth * 4))
							{
								SetErrorCode (IH_TGA_MALFORMED);
								if (pbyTempDecodedData)
								{
									delete [] pbyTempDecodedData;
									pbyTempDecodedData = NULL;
								}
								return FALSE;
							}			
						}
						if (bRLEPacket)
						{
							pbyData += 4;
						}
					}
				}
			}
			else
			{
				// Not compressed
				
				memcpy (pbyTempDecodedData, pbyData, 4 * iWidth * iHeight);
			}

			m_pbyDecodedData = new unsigned char[iSize];
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);			
				return FALSE;
			}
			memset (m_pbyDecodedData, 0, iSize);
			
			unsigned char *pbyPixelLine = NULL;
			unsigned char *pbyDecodedLine = m_pbyDecodedData;
			char cTemp;

			iWidth *= 4;

			//  Targa files are stored bottom-up; we need to flip them around
			for (y = (iHeight - 1); y >= 0; y--)
			{	
				pbyDecodedLine = m_pbyDecodedData + (y * iWidth);
				pbyPixelLine = pbyTempDecodedData + ((iHeight - y - 1) * iWidth);
				
				// 32-bit Targa files store their color data BGR, flip it to RGB so that any function that
				// retrieves this data has a common method for interpreting it.
				for (x = 0; x < iWidth; x += 4)
				{
					cTemp = pbyPixelLine[x + 2];
					pbyPixelLine[x + 2] = pbyPixelLine[x];
					pbyPixelLine[x] = cTemp;

#ifdef _DEBUG
					// Neal - only show a limited number of pixels
					// (otherwise, OutbutDebug buffer overflows)

					if (((y & iMask) == 0) && (((x/4) & iMask) == 0))
					{
						static int iOffset = 3;		// alpha channel

						int a = pbyPixelLine[x + iOffset];
						if (a == 0)
							OutputDebugString( ".");
						else if (a < 64)
							OutputDebugString( "!");
						else if (a < 128)
							OutputDebugString( "+");
						else if (a < 192)
							OutputDebugString( "*");
						else
							OutputDebugString( "@");
					}
#endif
				}
				memcpy (pbyDecodedLine, pbyPixelLine, iWidth);

#ifdef _DEBUG
				if ((y & iMask) == 0)	// only show every Xth line
				{
					OutputDebugString( "\n");
				}
#endif
			}
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	if (pbyTempDecodedData)
	{
		delete [] pbyTempDecodedData;
		pbyTempDecodedData = NULL;
	}

	SetErrorCode (IH_SUCCESS);
	return TRUE;

}

BOOL CImageHelper::EncodeTGA ()
{	
	unsigned char *pbyRawData		= NULL;
	unsigned char *pbyPixelData		= NULL;	
	unsigned char *pbyPixelLine		= NULL;	
	unsigned char *pbyPackedData	= NULL;
	unsigned char *pbyDecodedLine	= NULL;
	unsigned char *pbyTempData		= NULL;

	struct TGA_Header *tgaHeader;
	FILE *fp;

	int iDataSize		= 0;	
	int iSize			= 0;
	int j				= 0;
	int k				= 0;
	int x				= 0;
	int y				= 0;
	int iOffset			= 0;

	int iDepth = GetColorDepth();
	int iWidth = GetImageWidth();
	int iHeight = GetImageHeight();	
	int iHeaderSize = sizeof(struct TGA_Header);
	iSize = iWidth * iHeight * (iDepth == IH_8BIT ? 1 : 3);

	WORD* iFirstEntryIndex   = NULL;
	WORD* iColorMapLength    = NULL;
	BYTE* cColorMapEntrySize = NULL;

	pbyTempData = new BYTE[iSize];
	if (!pbyTempData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyTempData, 0, iDataSize);

	//  Targa files are stored bottom-up; we need to flip them around
	for (y = (iHeight - 1); y >= 0; y--)
	{	
		pbyDecodedLine = m_pbyDecodedData + (y * iWidth * (iDepth == IH_8BIT ? 1 : 3));
		pbyPixelLine = pbyTempData + ((iHeight - y - 1) * (iWidth * (iDepth == IH_8BIT ? 1 : 3)));
				
		memcpy (pbyPixelLine, pbyDecodedLine, iWidth * (iDepth == IH_8BIT ? 1 : 3));
	}

	pbyPixelData = pbyTempData;

	switch (iDepth)
	{
	case IH_8BIT:
		{
			// Flip R->B
			BYTE Temp;
			for (j = 0; j < 256; j++)
			{
				Temp = m_byPalette[j * 3 + 0];
				m_byPalette[j * 3 + 0] = m_byPalette[j * 3 + 2];
				m_byPalette[j * 3 + 2] = Temp;
			}

			iSize = iWidth * iHeight;		
			iDataSize = iHeaderSize + 768 + iSize * 2 + 1000;

			// Build the buffer
			pbyPackedData = new BYTE[iDataSize];

			if (!pbyPackedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				return FALSE;
			}
			memset (pbyPackedData, 0, iDataSize);

			tgaHeader = (struct TGA_Header *)pbyPackedData;

			tgaHeader->ID_Length = 0;
			tgaHeader->ColorMapType = 1;
			tgaHeader->ImageType = 9;

			iFirstEntryIndex	= (WORD *)(pbyPackedData + 3);
			iColorMapLength		= (WORD *)(pbyPackedData + 5);
			cColorMapEntrySize	= pbyPackedData + 7;

			*iFirstEntryIndex = 0;
			*iColorMapLength = 256;
			*cColorMapEntrySize = 24;

			tgaHeader->XOrigin = 0;
			tgaHeader->YOrigin = 0;
			tgaHeader->Width = iWidth;
			tgaHeader->Height = iHeight;
			tgaHeader->PixelDepth = 8;
			tgaHeader->ImageDescriptor = 0;

			// Move in the palette			
			memcpy (pbyPackedData + iHeaderSize, m_byPalette, 768);
		
			// Time to start encoding!			
			pbyRawData = pbyPackedData + iHeaderSize + 768;
			unsigned char LastByte;
			unsigned char ThisByte;
			int iRunLength = 0;
	
			for (k = 0; k < iHeight; k++)
			{
				LastByte = (*pbyPixelData);
				iRunLength = 1;

				for (j = 1; j < iWidth; j++)
				{
					ThisByte = *(++pbyPixelData);
					
					if (ThisByte == LastByte)
					{
						iRunLength++;				
					
						if (iRunLength == 128)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);
							*pbyRawData++ = LastByte;
							iRunLength = 0;					
						}				
					}
					else
					{
						if (iRunLength)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);							
							*pbyRawData++ = LastByte;
						}
						LastByte = ThisByte;
						iRunLength = 1;
					}
				
				}
				
				if (iRunLength)
				{				
					*pbyRawData++ = 0x80 | (iRunLength - 1);
					*pbyRawData++ = LastByte;
				}	
				pbyPixelData++;
			}
			
			int iPackedLength = pbyRawData - pbyPackedData;

			if (m_bUseArchive)
			{
				m_parFile->Write (pbyPackedData, iPackedLength);
			}
			else
			{
				errno_t err = fopen_s (&fp, m_strFileName, "wb");

				if (err != 0)
				{
					SetErrorCode (IH_ERROR_WRITING_FILE);
					if (pbyPackedData)
					{
						delete [] pbyPackedData;
						pbyPackedData = NULL;
					}
					return FALSE;		
				}
				else
				{
					fwrite (pbyPackedData, iPackedLength, 1, fp);	
					fclose(fp);
				}
			}

			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}						
		}
		break;

	case IH_24BIT:
		{
			iSize = iWidth * iHeight * 3;
			iDataSize = iHeaderSize + iSize * 2 + 1000;

			// Build the buffer
			pbyPackedData = new unsigned char[iDataSize];

			if (!pbyPackedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				return FALSE;
			}
			memset (pbyPackedData, 0, iDataSize);

			tgaHeader = (struct TGA_Header *)pbyPackedData;

			tgaHeader->ID_Length = 0;
			tgaHeader->ColorMapType = 0;
			tgaHeader->ImageType = 10;
			memset (tgaHeader->ColorMapSpec, 0, 5);
			tgaHeader->XOrigin = 0;
			tgaHeader->YOrigin = 0;
			tgaHeader->Width = iWidth;
			tgaHeader->Height = iHeight;
			tgaHeader->PixelDepth = 24;
			tgaHeader->ImageDescriptor = 0;
			
			// Time to start encoding!			
			pbyRawData = pbyPackedData + iHeaderSize;
			char LastRGB[3];
			char ThisRGB[3];
			int iRunLength = 0;
	
			for (k = 0; k < iHeight; k++)
			{		
				//memcpy (LastRGB, pbyPixelData, 3);
				LastRGB[0] = pbyPixelData[0];
				LastRGB[1] = pbyPixelData[1];
				LastRGB[2] = pbyPixelData[2];

				iRunLength = 1;

				for (j = 1; j < iWidth; j++)
				{
					pbyPixelData += 3;
					//memcpy (ThisRGB, pbyPixelData, 3);
					ThisRGB[0] = pbyPixelData[0];
					ThisRGB[1] = pbyPixelData[1];
					ThisRGB[2] = pbyPixelData[2];
					
					if ((ThisRGB[0] == LastRGB[0]) && (ThisRGB[1] == LastRGB[1]) && (ThisRGB[2] == LastRGB[2]))
					{
						iRunLength++;				
					
						if (iRunLength == 128)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);

							pbyRawData[0] = LastRGB[2];
							pbyRawData[1] = LastRGB[1];
							pbyRawData[2] = LastRGB[0];							
							pbyRawData += 3;
							
							iRunLength = 0;					
						}				
					}
					else
					{
						if (iRunLength)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);
							pbyRawData[0] = LastRGB[2];
							pbyRawData[1] = LastRGB[1];
							pbyRawData[2] = LastRGB[0];
							pbyRawData += 3;							
						}
						//memcpy (LastRGB, ThisRGB, 3);
						LastRGB[0] = ThisRGB[0];
						LastRGB[1] = ThisRGB[1];
						LastRGB[2] = ThisRGB[2];
						iRunLength = 1;
					}
				
				}
				
				if (iRunLength)
				{				
					*pbyRawData++ = 0x80 | (iRunLength - 1);
					pbyRawData[0] = LastRGB[2];
					pbyRawData[1] = LastRGB[1];
					pbyRawData[2] = LastRGB[0];
					pbyRawData += 3;
				}	
				pbyPixelData += 3;
			}
			
			int iPackedLength = pbyRawData - pbyPackedData;

			if (m_bUseArchive)
			{
				m_parFile->Write (pbyPackedData, iPackedLength);
			}
			else
			{
				errno_t err = fopen_s (&fp, m_strFileName, "wb");

				if (err != 0)
				{
					SetErrorCode (IH_ERROR_WRITING_FILE);
					if (pbyPackedData)
					{
						delete [] pbyPackedData;
						pbyPackedData = NULL;
					}
					return FALSE;		
				}
				else
				{
					fwrite (pbyPackedData, iPackedLength, 1, fp);	
					fclose(fp);
				}
			}

			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}			
		}
		break;

	case IH_32BIT:
		{
			iSize = iWidth * iHeight * 4;
			iDataSize = iHeaderSize + iSize * 2 + 1000;

			// Build the buffer
			pbyPackedData = new BYTE[iDataSize];

			if (!pbyPackedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				return FALSE;
			}
			memset (pbyPackedData, 0, iDataSize);

			tgaHeader = (struct TGA_Header *)pbyPackedData;

			tgaHeader->ID_Length       = 0;
			tgaHeader->ColorMapType    = 0;
			tgaHeader->ImageType       = 10;
			memset (tgaHeader->ColorMapSpec, 0, 5);
			tgaHeader->XOrigin         = 0;
			tgaHeader->YOrigin         = 0;
			tgaHeader->Width           = iWidth;
			tgaHeader->Height          = iHeight;
			tgaHeader->PixelDepth      = 32;
			tgaHeader->ImageDescriptor = 0;
			
			// Time to start encoding!			
			pbyRawData = pbyPackedData + iHeaderSize;
			BYTE LastRGB[4];
			BYTE ThisRGB[4];
			int iRunLength = 0;
	
			for (k = 0; k < iHeight; k++)
			{		
				//memcpy (LastRGB, pbyPixelData, 3);
				LastRGB[0] = pbyPixelData[0];
				LastRGB[1] = pbyPixelData[1];
				LastRGB[2] = pbyPixelData[2];

				iRunLength = 1;

				for (j = 1; j < iWidth; j++)
				{
					pbyPixelData += 4;
					//memcpy (ThisRGB, pbyPixelData, 4);
					ThisRGB[0] = pbyPixelData[0];
					ThisRGB[1] = pbyPixelData[1];
					ThisRGB[2] = pbyPixelData[2];
					ThisRGB[3] = pbyPixelData[3];
					
					if ((ThisRGB[0] == LastRGB[0]) && (ThisRGB[1] == LastRGB[1]) &&
							(ThisRGB[2] == LastRGB[2]) && (ThisRGB[3] == LastRGB[3]))
					{
						iRunLength++;				
					
						if (iRunLength == 128)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);

							pbyRawData[0] = LastRGB[3];
							pbyRawData[1] = LastRGB[2];
							pbyRawData[2] = LastRGB[1];
							pbyRawData[3] = LastRGB[0];
							pbyRawData += 4;
							
							iRunLength = 0;					
						}				
					}
					else
					{
						if (iRunLength)
						{
							*pbyRawData++ = 0x80 | (iRunLength - 1);
							pbyRawData[0] = LastRGB[3];
							pbyRawData[1] = LastRGB[2];
							pbyRawData[2] = LastRGB[1];
							pbyRawData[3] = LastRGB[0];
							pbyRawData += 4;
						}
						//memcpy (LastRGB, ThisRGB, 4);
						LastRGB[0] = ThisRGB[0];
						LastRGB[1] = ThisRGB[1];
						LastRGB[2] = ThisRGB[2];
						LastRGB[3] = ThisRGB[3];

						iRunLength = 1;
					}
				
				}
				
				if (iRunLength)
				{				
					*pbyRawData++ = 0x80 | (iRunLength - 1);
					pbyRawData[0] = LastRGB[3];
					pbyRawData[1] = LastRGB[2];
					pbyRawData[2] = LastRGB[1];
					pbyRawData[3] = LastRGB[0];
					pbyRawData += 4;
				}	
				pbyPixelData += 4;
			}
			
			int iPackedLength = pbyRawData - pbyPackedData;

			if (m_bUseArchive)
			{
				m_parFile->Write (pbyPackedData, iPackedLength);
			}
			else
			{					
				errno_t err = fopen_s (&fp, m_strFileName, "wb");

				if (err != 0 )
				{
					SetErrorCode (IH_ERROR_WRITING_FILE);
					if (pbyPackedData)
					{
						delete [] pbyPackedData;
						pbyPackedData = NULL;
					}
					return FALSE;		
				}
				else
				{
					fwrite (pbyPackedData, iPackedLength, 1, fp);	
					fclose(fp);
				}
			}
			
			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}						
		}
		
		break;

	default:
		ASSERT (FALSE);		// Unhandled color depth!
		break;
	}


	if (pbyTempData)
	{
		delete [] pbyTempData;
		pbyTempData = NULL;
	}

	return TRUE;
}
