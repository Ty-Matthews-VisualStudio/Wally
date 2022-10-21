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
//   BMP Support
////////////////////////////////////////////////////////////////////////////////////

BOOL CImageHelper::DecodeBMP( int iFlags /* = 0 */ )
{
	RGBQUAD *RGBQuadArray				= NULL;	
	
	long iLength = GetFileLength();
	
	unsigned char *p_PixelLine			= NULL;
	unsigned char *p_PixelData			= NULL;
	unsigned char *p_DecodedLine		= NULL;
			
	int j				= 0;
	int x				= 0;
	int iTemp			= 0;
	int iWidth			= 0;
	int iHeight			= 0;
	int iSize			= 0;
	int iBPP			= 0;
	int iPaddedWidth	= 0;
	int iColorsUsed		= 0;
//	div_t x_result;
//	div_t y_result;
	
	if (iLength < (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO)))
	{
		SetErrorCode (IH_BMP_MALFORMED);		
		return FALSE;
	}	 

	// Point the structs at the raw data array
	m_pBmfHeader = (BITMAPFILEHEADER *) m_pbyEncodedData;
	m_pBmfInfo = (BITMAPINFO *) (m_pbyEncodedData + sizeof (BITMAPFILEHEADER));
	
	if (m_pBmfHeader->bfType != BMH)
	{
		SetErrorCode (IH_BMP_MALFORMED);
		return FALSE;
	}	
	
	if (m_pBmfInfo->bmiHeader.biCompression != BI_RGB)
	{
		SetErrorCode (IH_BMP_UNSUPPORTED_COMPRESSION);
		return FALSE;
	}

	iBPP = m_pBmfInfo->bmiHeader.biBitCount;
	iColorsUsed = m_pBmfInfo->bmiHeader.biClrUsed;
	if (iColorsUsed == 0)
	{
		iColorsUsed = 256;
	}
	
	iWidth = m_pBmfInfo->bmiHeader.biWidth;
	iHeight = m_pBmfInfo->bmiHeader.biHeight;	

// Neal - test of ANY SIZE image
//
//	x_result = div(iWidth, 16);
//	y_result = div(iHeight, 16);
//
//	// Make sure it's of valid size
//	if ((x_result.rem != 0) || (y_result.rem != 0))
//	{
//		SetErrorCode (IH_NOT_DIV16);		
//		return FALSE;
//	}	
	
	SetImageWidth (iWidth);
	SetImageHeight (iHeight);

	switch( iBPP )
	{
	case 8 :	
		{
			SetColorDepth (IH_8BIT);
		}
		break;

	case 24 :
		{
			SetColorDepth (IH_24BIT);
		}
		break;

	default:
		SetErrorCode (IH_BMP_UNSUPPORTED_VERSION);		
		return FALSE;
		break;
	}

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	iSize = iWidth * iHeight * (iBPP / 8);
	
	m_pbyDecodedData = new BYTE[iSize];
	if (! m_pbyDecodedData)
	{
		SetErrorCode( IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (m_pbyDecodedData, 0, iSize);	
	
	switch( iBPP )
	{
	case 8 :	
		{
			p_PixelData = (m_pbyEncodedData + sizeof (BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * (iColorsUsed - 1)));

			int iDataSize = iLength - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + sizeof(RGBQUAD) * (iColorsUsed - 1));

			if (iSize > iDataSize)
			{	
				SetErrorCode (IH_BMP_MALFORMED);
				return FALSE;
			}	
					
			RGBQuadArray = (RGBQUAD *) (m_pBmfInfo->bmiColors);
			
			for (j = 0; j < iColorsUsed; j++) 
			{	
				m_byPalette[j * 3] = RGBQuadArray[j].rgbRed;			
				m_byPalette[j * 3 + 1] = RGBQuadArray[j].rgbGreen;
				m_byPalette[j * 3 + 2] = RGBQuadArray[j].rgbBlue;
			}
			
			iPaddedWidth = PadDWORD( iWidth);
		
			//  Bitmaps are stored bottom-up; we need to flip them around
			for (j = (iHeight - 1); j >= 0; j--)
			{				
				p_DecodedLine = m_pbyDecodedData + (j * iWidth);		
				p_PixelLine   = p_PixelData + ((iHeight- j - 1) * iPaddedWidth);
				memcpy (p_DecodedLine, p_PixelLine, iWidth);			
			}
		}
		break;

	case 24 :
		{
			p_PixelData = m_pbyEncodedData + m_pBmfHeader->bfOffBits;
			//p_PixelData = (m_pbyEncodedData + sizeof (BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

			if ((int) (iSize) > (int) (iLength - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))))
			{	
				SetErrorCode (IH_BMP_MALFORMED);
				return FALSE;
			}			

			iWidth *= 3;
			iPaddedWidth = PadDWORD( iWidth);
		
			//  Bitmaps are stored bottom-up; we need to flip them around
			for (j = (iHeight - 1); j >= 0; j--)
			{	
				p_DecodedLine = m_pbyDecodedData + (j * iWidth);		
				p_PixelLine   = p_PixelData + ((iHeight- j - 1) * iPaddedWidth);			
				
				memcpy( p_DecodedLine, p_PixelLine, iWidth );

				// 24-bit Bitmaps store their color data BGR, flip it to RGB so that any function that
				// retrieves this data has a common method for interpreting it.
				for (x = 0; x < iWidth; x += 3)
				{
					iTemp = p_DecodedLine[x + 2];
					p_DecodedLine[x + 2] = p_DecodedLine[x];
					p_DecodedLine[x] = iTemp;
				}
			}
		}
		break;

	default:
		SetErrorCode (IH_BMP_UNSUPPORTED_VERSION);		
		return FALSE;
		break;
	}
	return TRUE;
}


BOOL CImageHelper::EncodeBMP()
{
	unsigned char *p_RawData		= NULL;
	unsigned char *p_PixelData		= NULL;	
	unsigned char *p_PixelLine		= NULL;	
	unsigned char *pbyPackedData		= NULL;
	unsigned char *p_DecodedLine	= NULL;
	RGBQUAD *RGBQuadArray			= NULL;	
	int iDataSize		= 0;	
	int j				= 0;

	int iDepth = GetColorDepth();
	int iWidth = GetImageWidth();
	int iHeight = GetImageHeight();	
	int iPaddedWidth	= 0;	
	int iSize			= 0;

	switch (iDepth)
	{
	case IH_8BIT:
		iPaddedWidth = PadDWORD( iWidth);
		iSize = iPaddedWidth * iHeight;
		iDataSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * 255) + iSize;
		break;

	case IH_24BIT:
		// Bitmaps are stored BGR, not RGB, so flip them
		int iHold;
		iSize = iWidth * iHeight;
		for (j = 0; j < iSize; j++)
		{
			iHold = m_pbyDecodedData[j * 3];
			m_pbyDecodedData[j * 3] = m_pbyDecodedData[j * 3 + 2];
			m_pbyDecodedData[j * 3 + 2] = iHold;
		}
		
		iPaddedWidth = PadDWORD( iWidth * 3);
		iSize = iPaddedWidth * iHeight;
		iDataSize = sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFO) - sizeof(RGBQUAD)) + iSize;
		break;

	default:
		ASSERT (FALSE);		// Unhandled color depth!
		break;
	}
	
	
	// Build the buffer
	pbyPackedData = new BYTE[iDataSize];

	if (!pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyPackedData, 0, iDataSize);

	// Set the various pointers to areas within the buffer
	m_pBmfHeader = (BITMAPFILEHEADER *) pbyPackedData;
	m_pBmfInfo = (BITMAPINFO *) (pbyPackedData + (sizeof(BITMAPFILEHEADER)));

	switch (iDepth)
	{
	case IH_8BIT:
		RGBQuadArray = (RGBQUAD *) (&m_pBmfInfo->bmiColors[0]);
		for (j = 0; j < 256; j++)
		{
			RGBQuadArray[j].rgbRed   = m_byPalette[j * 3];
			RGBQuadArray[j].rgbGreen = m_byPalette[j * 3 + 1];
			RGBQuadArray[j].rgbBlue  = m_byPalette[j * 3 + 2];
			RGBQuadArray[j].rgbReserved = 0;
		}
		p_PixelData = pbyPackedData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * 255);
		m_pBmfInfo->bmiHeader.biBitCount = 8;
		m_pBmfHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * 255);
		
		//  Bitmaps are stored bottom-up; we need to flip it around
		for (j = (iHeight - 1); j >= 0; j--)
		{
			p_DecodedLine = m_pbyDecodedData + (j * iWidth);		
			p_PixelLine = p_PixelData + (iHeight - j - 1) * iPaddedWidth;
			memcpy (p_PixelLine, p_DecodedLine, iWidth);
		}	
		break;

	case IH_24BIT:
		p_PixelData = pbyPackedData + sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFO) - sizeof(RGBQUAD));
		m_pBmfInfo->bmiHeader.biBitCount = 24;
		m_pBmfHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + (sizeof(BITMAPINFO) - sizeof(RGBQUAD));

		//  Bitmaps are stored bottom-up; we need to flip it around
		for (j = (iHeight - 1); j >= 0; j--)
		{
			p_DecodedLine = m_pbyDecodedData + (j * iWidth * 3);
			p_PixelLine = p_PixelData + ((iHeight - j - 1) * iPaddedWidth);
			memcpy (p_PixelLine, p_DecodedLine, iWidth * 3);
		}	
		break;

	}

	HDC ScreenDC = CreateCompatibleDC(NULL);

	// Get the number of pixels per inch
	double dfXPels = (GetDeviceCaps (ScreenDC, LOGPIXELSX)) * 1.0;
	double dfYPels = (GetDeviceCaps (ScreenDC, LOGPIXELSY)) * 1.0;
	
	DeleteDC (ScreenDC);

	// Convert to number of pixels per meter
	dfXPels /= 2.54;
	dfXPels *= 100.0;
	dfYPels /= 2.54;
	dfYPels *= 100.0;

	m_pBmfHeader->bfType = BMH;
	m_pBmfHeader->bfSize = iDataSize;
	m_pBmfHeader->bfReserved1 = 0;
	m_pBmfHeader->bfReserved2 = 0;

	m_pBmfInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmfInfo->bmiHeader.biWidth = iWidth;
	m_pBmfInfo->bmiHeader.biHeight = iHeight;
	m_pBmfInfo->bmiHeader.biPlanes = 1;	
	m_pBmfInfo->bmiHeader.biCompression = BI_RGB;
	m_pBmfInfo->bmiHeader.biSizeImage = iPaddedWidth * iHeight;
	m_pBmfInfo->bmiHeader.biXPelsPerMeter = (int)dfXPels;
	m_pBmfInfo->bmiHeader.biYPelsPerMeter = (int)dfYPels;
	m_pBmfInfo->bmiHeader.biClrUsed = 0;
	m_pBmfInfo->bmiHeader.biClrImportant = 0;	

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iDataSize);
	}
	else
	{
		FILE *fp;
		fp = fopen (m_strFileName, "wb");

		if (!fp)
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
			fwrite (pbyPackedData, iDataSize, 1, fp);	
			fclose(fp);
		}
	}

	if (pbyPackedData)
	{
		delete [] pbyPackedData;
		pbyPackedData = NULL;
	}	

	return TRUE;

}
