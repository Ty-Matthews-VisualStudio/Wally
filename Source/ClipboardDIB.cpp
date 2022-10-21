/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// ClipboardDIB.cpp: implementation of the CClipboardDIB class.
//
// Created by Ty Matthews, 3-15-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyPal.h"
#include "ClipboardDIB.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClipboardDIB::CClipboardDIB()
{

}

CClipboardDIB::~CClipboardDIB()
{
	GlobalFree ((HGLOBAL) m_pData.p_ClipboardData);	
	
	if (p_BitsArray)	
		delete [] p_BitsArray;			// neal - must use array delete to delete arrays!
	
	if (m_ClipboardPalette)
		delete [] m_ClipboardPalette;	// neal - must use array delete to delete arrays!

	p_BitsArray        = NULL;
	m_ClipboardPalette = NULL;
}

bool CClipboardDIB::InitFromClipboard(CWnd* p_Wnd)
{
	if (p_Wnd->OpenClipboard())
	{
		// GetClipboardFile		
		HGLOBAL hg = ::GetClipboardData(CF_DIB);		
		if (hg == NULL)
		{
			AfxMessageBox ("Error reading from clipboard");
		}

		// Determine the data size
		int iDataSize = GlobalSize(hg); 
		
		// Alloc memory
		m_pData.p_ClipboardData = (unsigned char *)GlobalAlloc(GMEM_FIXED, iDataSize); 

		// Grab the data					
		CopyMemory(m_pData.p_ClipboardData, GlobalLock(hg), iDataSize); 

		// Point the DIB structure to the beginning of the array
		m_pData.DIB = (LPCDDIBType)m_pData.p_ClipboardData;

		// Get out as fast as we can from the clipboard
		GlobalUnlock(hg);
		CloseClipboard();	

		// These will be used to walk the array
		unsigned char *BitsLine;
		unsigned char *DataLine;

		// Allocate the Palette
		WORD iColors = GetColorsUsed();
		m_ClipboardPalette = new BYTE[iColors * 4];

		long Width  = GetWidth();
		long Height = GetHeight();

		int j, k;
		int iSize;
		unsigned char *DataOffset; 
		int BMHeaderSize = sizeof (CDDIBType);

		if (GetColorDepth() == 8)	
		{
			DataOffset = m_pData.p_ClipboardData + BMHeaderSize + (iColors * 4);
			iSize = Width * Height;	
			p_BitsArray = new BYTE[iSize];
			for (j = (Height - 1), k = 0; j >= 0; j--, k++)
			{
				DataLine = DataOffset + (PadDWORD((int)Width) * j);
				BitsLine = p_BitsArray + (Width * k);
				memcpy (BitsLine, DataLine, Width);
			}			
		}		
	
		if (GetColorDepth() == 24)
		{
			DataOffset = m_pData.p_ClipboardData + BMHeaderSize;
			iSize = Width * Height * 3;
			p_BitsArray = new BYTE[iSize];			
			for (j = (Height - 1), k = 0; j >= 0; j--, k++)
			{			
				DataLine = DataOffset + (j * PadDWORD((int)Width * 3));
				BitsLine = p_BitsArray + (Width * k * 3);				
				memcpy (BitsLine, DataLine, Width * 3);
			}				
	
		}			

		return true;
	}
	else
		return false;
}

void CClipboardDIB::GetDIBPalette( BYTE* p_Palette)
{	
	WORD wColors = GetColorsUsed();

	if ((wColors < 0) || (wColors > 256))
	{
		MessageBeep( MB_ICONEXCLAMATION);
		return;
	}

	if (p_Palette != NULL)
		CopyMemory (p_Palette, (m_pData.p_ClipboardData + 0x28), wColors * 4);
}


void CClipboardDIB::WriteToClipboard( CWnd* p_Wnd, CWallyPalette* pPal, BYTE* pbyBits, int iWidth, int iHeight)
{
	BITMAPINFO *bmfInfo;
	RGBQUAD *RGBQuadArray;
	unsigned char *p_RawData;
	unsigned char *PixelData;
	unsigned char *MipLine;
	unsigned char *PixelLine;
	int iDataSize;	
	int iSize = iWidth * iHeight;
	int j;

	iDataSize = sizeof( BITMAPINFO) + (sizeof(RGBQUAD) * 255) + iSize;

	// Build the buffer
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, iDataSize); 
	p_RawData = (BYTE* )GlobalLock( hData);
	
	// Set the various pointers to areas within the buffer
	bmfInfo      = (BITMAPINFO *) p_RawData;
	RGBQuadArray = (RGBQUAD *) (&bmfInfo->bmiColors[0]);	
	PixelData    = p_RawData + sizeof( BITMAPINFO) + (sizeof( RGBQUAD) * 255);

	bmfInfo->bmiHeader.biSize          = sizeof( BITMAPINFOHEADER);
	bmfInfo->bmiHeader.biWidth         = iWidth;
	bmfInfo->bmiHeader.biHeight        = iHeight;
	bmfInfo->bmiHeader.biPlanes        = 1;
	bmfInfo->bmiHeader.biBitCount      = 8;
	bmfInfo->bmiHeader.biCompression   = BI_RGB;
	bmfInfo->bmiHeader.biSizeImage     = 0;
	bmfInfo->bmiHeader.biXPelsPerMeter = 0;
	bmfInfo->bmiHeader.biYPelsPerMeter = 0;
	bmfInfo->bmiHeader.biClrUsed       = 0;
	bmfInfo->bmiHeader.biClrImportant  = 0;	

	for (j = 0; j < 256; j++)
	{
		RGBQuadArray[j].rgbRed      = pPal->GetR( j);
		RGBQuadArray[j].rgbGreen    = pPal->GetG( j);
		RGBQuadArray[j].rgbBlue     = pPal->GetB( j);
		RGBQuadArray[j].rgbReserved = 0;
	} 

	//  Bitmaps are stored bottom-up; we need to flip them around
	for (j = (iHeight - 1); j >= 0; j--)
	{		
		MipLine   = pbyBits + (j * iWidth);
		PixelLine = PixelData + ((iHeight - j - 1) * iWidth);
		memcpy( PixelLine, MipLine, iWidth);
	}	
	GlobalUnlock( hData);
	
	if (::OpenClipboard(p_Wnd->GetSafeHwnd()))
	{
		::EmptyClipboard();		
		::SetClipboardData( CF_DIB, hData);								
		CloseClipboard();			
	}
	else
		AfxMessageBox( CG_IDS_CANNOT_OPEN_CLIPBOARD); 
}