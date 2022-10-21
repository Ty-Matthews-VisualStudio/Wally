/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// Created by Ty Matthews, 3-23-1998
//
// DibSection.cpp: implementation of the CDibSection class.
//
// This class is a modified version of one I saw in the book "Cutting-Edge
// 3D Game Programming with C++"  (John DeGoes, Coriolis Group Books, 1996)
// Chapter 14, pages 627-630.
//
// I've removed some of the code from his class that I didn't need, and
// added some additional functionality.  Thanks to John for sharing his 
// code!
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wingdi.h"
#include "DibSection.h"
#include "Wally.h"
#include "Layer.h"
#include "Tool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CTool g_Tool;

CDibSection::CDibSection()
{
	m_BMHan		= NULL;
	m_byBits	= NULL;
	m_iWidth	= 0;
	m_iHeight	= 0;
	m_iBitCount = 0;
	
	m_bWasInit	= FALSE;
	memset (m_byOriginalPalette, 0, 768);
}

CDibSection::~CDibSection() 
{		
	if (m_BMHan)
	{
		DeleteObject (m_BMHan);
		m_byBits = NULL;
	}		
}

CString CDibSection::GetError()
{
	CString strError("");

	switch (GetErrorCode())
	{
	case DIB_ERROR_OUT_OF_MEMORY:
		{
			strError = "Out of memory";
		}
		break;

	case DIB_ERROR_CREATE:
		{
			strError.Format ("Failed to create DibSection\n%s", GetErrorString());
		}
		break;

	case DIB_ERROR_CLIPBOARD:
		{
			strError = "Failed to write to clipboard";
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	return strError;
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	ReInit
//	Action:	Deletes the existing Bitmap and recreates it with new data
BOOL CDibSection::ReInit( int iWidth, int iHeight, int iBitCount, BYTE* pbyPal /* = NULL */, bool bUseGamma /* = TRUE */)
{
	//delete BMInfo;
	if (m_BMHan)
	{
		DeleteObject( m_BMHan);
		m_BMHan = NULL;
		m_byBits  = NULL;
	}
	return Init( iWidth, iHeight, iBitCount, pbyPal, bUseGamma);
}

void CDibSection::RebuildPalette( BYTE* pbyPal, BOOL bSaveAsOriginalPalette)
{
	if (GetBitCount() != 8)
	{
		return;
	}

	if (bSaveAsOriginalPalette)
	{
		memcpy( m_byOriginalPalette, pbyPal, (int )m_BMInfo.bmiHeader.biClrUsed * 3);
	}

	//double dfGamma = 1.49;		// 0.8, 2.2 ???
	//double dfGamma = 1.0;

	double dfLambda  = 1.0 / g_dfGamma;
	BOOL   bUseGamma = (m_bUseGamma && (g_dfGamma != 1.0));

	int c = 0;
	int k = 0;

	for (long n = 0; n < (int )m_BMInfo.bmiHeader.biClrUsed; n++)
	{
		if (m_bUseGamma)
		{
			// with gamma correction

			c = pbyPal[n * 3];
			k = (int )(exp( dfLambda * log( c / 255.0)) * 255.0);
			m_BMInfo.bmiColors[n].rgbRed = k;

			c = pbyPal[n * 3 + 1];
			k = (int )(exp( dfLambda * log( c / 255.0)) * 255.0);
			m_BMInfo.bmiColors[n].rgbGreen = k;

			c = pbyPal[n * 3 + 2];
			k = (int )(exp( dfLambda * log( c / 255.0)) * 255.0);
			m_BMInfo.bmiColors[n].rgbBlue = k;
		}
		else
		{
			// no gamma correction

			m_BMInfo.bmiColors[n].rgbRed   = pbyPal[n * 3];
			m_BMInfo.bmiColors[n].rgbGreen = pbyPal[n * 3 + 1];
			m_BMInfo.bmiColors[n].rgbBlue  = pbyPal[n * 3 + 2];
		}

		m_BMInfo.bmiColors[n].rgbReserved = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	Init
//	Action:	Creates the CDibSection bitmap, by filling in the BitmapInfo struct,
//			building the bmiColors palette, and setting the resultant bits to 0
BOOL CDibSection::Init( int iWidth, int iHeight, int iBitCount, BYTE* pbyPal /* = NULL */, bool bUseGamma /* = TRUE */)
{
	m_bWasInit = TRUE;
	
	HWND hWndActive = GetActiveWindow();
	HDC  hScreenDC  = GetDC( hWndActive);
	int  iColors    = (iBitCount == 8) ? 256 : 0;
	
	// neal - don't memleak
	if (m_BMHan)
	{
		DeleteObject (m_BMHan);
		m_BMHan = NULL;
		m_byBits  = NULL;
	}

	SetBitCount (iBitCount);
	SetWidth (iWidth);
	SetHeight (iHeight);
		
	m_BMInfo.bmiHeader.biSize          = sizeof( BITMAPINFOHEADER);
	m_BMInfo.bmiHeader.biWidth         = GetWidth();
	m_BMInfo.bmiHeader.biHeight        = -abs( GetHeight());
	m_BMInfo.bmiHeader.biPlanes        = 1;
	m_BMInfo.bmiHeader.biBitCount      = GetBitCount();
	m_BMInfo.bmiHeader.biCompression   = BI_RGB;
	m_BMInfo.bmiHeader.biSizeImage     = NULL;
	m_BMInfo.bmiHeader.biXPelsPerMeter = NULL;
	m_BMInfo.bmiHeader.biYPelsPerMeter = NULL;
	m_BMInfo.bmiHeader.biClrUsed       = iColors;
	m_BMInfo.bmiHeader.biClrImportant  = iColors;

	m_bUseGamma = bUseGamma;

	if (iColors)
	{		
		RebuildPalette( pbyPal, TRUE);
	}

	m_BMHan = CreateDIBSection ( hScreenDC, (BITMAPINFO *)&m_BMInfo, DIB_RGB_COLORS, (VOID **)&m_byBits, NULL, NULL);
	DWORD dwError = GetLastError();

	ReleaseDC( hWndActive, hScreenDC);
	
	if (m_byBits)
	{
		ClearBits (0);		
	}
	else
	{
		CString strError("");
		strError.Format ("CreateDIBSection error:  %lu", dwError);
		
		SetErrorString (strError);
		SetErrorCode (DIB_ERROR_CREATE);
		return FALSE;
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// Name:	RebuildDibSection
// Action:	Rebuilds the DIBSection only; preserving the BMFinfo struct.  This
//			is called by FlipWidthHeight, and others.  The objective here is to
//			clean out the BMHan and recreate it, but without having to respecify
//			any of the BMFinfo (useful when Undoing texture rotation)
BOOL CDibSection::RebuildDibSection( BYTE* pbyPal /* = NULL */)
{
	HWND hWndActive = GetActiveWindow();
	HDC  hScreenDC  = GetDC( hWndActive);
	
	// neal - don't memleak
	if (m_BMHan)
	{	
		DeleteObject (m_BMHan);
		m_BMHan = NULL;
		m_byBits = NULL;
	}

	m_BMInfo.bmiHeader.biWidth  = GetWidth();
	m_BMInfo.bmiHeader.biHeight = -abs (GetHeight());

	if (pbyPal)
	{		
		RebuildPalette( pbyPal, TRUE);
	}


	m_BMHan = CreateDIBSection ( hScreenDC, (BITMAPINFO *)&m_BMInfo, DIB_RGB_COLORS, (VOID **)&m_byBits, NULL, NULL);
	DWORD dwError = GetLastError();

	ReleaseDC( hWndActive, hScreenDC);
	if (m_byBits)
	{
		ClearBits (0);		
	}
	else
	{
		CString strError("");
		strError.Format ("CreateDIBSection error:  %lu", dwError);
		
		SetErrorString (strError);
		SetErrorCode (DIB_ERROR_CREATE);
		return FALSE;
	}

	return TRUE;
}

inline void CDibSection::InternalStretchBlt( HDC hDC, int X, int Y, double dfZoom, HDC hContextDC)
{
// Neal - does this fix the ComCtl32 page fault bug?  NOPE???
//#define MAX_STRETCH 512
#define MAX_STRETCH 1024

	if ((dfZoom == 0.0) || (GetWidth() <= 0) || (GetHeight() <= 0))
	{
		ASSERT( FALSE);
		MessageBeep( MB_ICONEXCLAMATION);
		return;
	}

	CRect rClip;
	if (NULLREGION == GetClipBox( hDC, &rClip))
	{
		return;
	} 

	int i, j;
	i = max( GetWidth(), GetHeight());

	int iSectionW = GetWidth();
	int iSectionH = GetHeight();

	if (dfZoom == 1.0)
	{
		BitBlt( hDC, X, Y, GetWidth(), GetHeight(), 
				hContextDC, 0, 0, SRCCOPY);
	}
//	else if ((dfZoom < 0.0) || ((int )(i * dfZoom) <= (2 * MAX_STRETCH)))
	else if ((dfZoom < 0.0) || ((int )(i * dfZoom) <= MAX_STRETCH))
	{
		SetStretchBltMode( hDC, COLORONCOLOR);

		StretchBlt( hDC, X, Y, 
				(int )(iSectionW * dfZoom), (int )(iSectionH * dfZoom), 
				hContextDC, 0, 0, iSectionW, iSectionH, SRCCOPY);
	}
	else
	{
		// neal - fix for StretchBlt failure when too wide
		//        (Windows can't build a code segment larger than 4K?)

		// this is also a major (up to 16 X) speed up for
		// large textures at high zoom levels
		// even better (up to 16*9 X) speed up for tiled textures

		// NO LONGER assumes: height and width are both divisible by several powers-of-two

		int iZoom = (int )dfZoom;		// we now know it's not negative

		ASSERT( iZoom >= 1);
		iZoom = max( 1, iZoom);

		while ((iSectionH * iZoom) > MAX_STRETCH)
		{
			iSectionH /= 2;
		}
		while ((iSectionW * iZoom) > MAX_STRETCH)
		{
			iSectionW /= 2;
		}

//		int iSection = MAX_STRETCH / iZoom;

		// Neal - test of ANY SIZE image

//		ASSERT( (m_iOriginalWidth  % iSectionW) == 0);
//		ASSERT( (m_iOriginalHeight % iSectionH) == 0);

//		for (j = 0; j < m_iOriginalHeight; j += iSection)
		for (j = 0; j < GetHeight(); j += iSectionH)
		{
//			if ((j*iZoom <= rClip.bottom) && (((j+iSection)*iZoom) >= rClip.top))
			if ((j*iZoom <= rClip.bottom) && (((j+iSectionH)*iZoom) >= rClip.top))
			{
//				for (i = 0; i < m_iOriginalWidth; i += iSection)
				for (i = 0; i < GetWidth(); i += iSectionW)
				{
//					if ((i*iZoom <= rClip.right) && (((i+iSection)*iZoom) >= rClip.left))
					if ((i*iZoom <= rClip.right) && (((i+iSectionW)*iZoom) >= rClip.left))
					{
//						StretchBlt( hDC, X + i*iZoom, Y + j*iZoom, 
//								iSection * iZoom, iSection * iZoom, 
//								hContextDC, i, j, iSection, iSection, SRCCOPY);
						StretchBlt( hDC, X + i*iZoom, Y + j*iZoom, 
								iSectionW * iZoom, iSectionH * iZoom, 
								hContextDC, i, j, iSectionW, iSectionH, SRCCOPY);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	Show
//	Action:	Displays the bitmap to the screen, using the CDC passed in as the owner,
//			the X,Y location on that CDC, and the Zoom(stretch) value.
void CDibSection::Show( CDC* pActiveWindow, int X, int Y, double dfZoom)
{
	CDC ContextDC;
	ContextDC.CreateCompatibleDC( pActiveWindow);

	TRY
	{
		ContextDC.SelectObject( m_BMHan);
	
		InternalStretchBlt( pActiveWindow->m_hDC, X, Y, dfZoom, ContextDC.m_hDC);
	}
	CATCH_ALL( e)
	{
		MessageBeep( MB_ICONEXCLAMATION);
	}
	END_CATCH_ALL

	ContextDC.DeleteDC();
}

/*
void CDibSection::Show (CPaintDC* ActiveWindow, int X, int Y, int Zoom)
{
	CDC Context;
	Context.CreateCompatibleDC (ActiveWindow);
	HBITMAP p_OldBM = (HBITMAP)Context.SelectObject (BMHan);	 
	
	StretchBlt( ActiveWindow->m_hDC, X, Y, 
			m_iOriginalWidth * Zoom, m_iOriginalHeight * Zoom, 
			Context.m_hDC, 0, 0, m_iOriginalWidth, m_iOriginalHeight, SRCCOPY);
	
	Context.SelectObject (p_OldBM);
	Context.DeleteDC();
}
*/

/////////////////////////////////////////////////////////////////////////////////
//  Name:	ShowTiled
//	Action:	This function is pretty much a custom thing for Wally, but can be used
//			elsewhere if the situation calls for it.  It displays the bitmap into
//			a tiled array (currently hard-coded to 3x3, the NumTiles is not used, 
//			although there for future enhancements.)
void CDibSection::ShowTiled (CDC* pActiveWindow, int X, int Y, double dfZoom, int NumTiles)
{
	CDC ContextDC;
	ContextDC.CreateCompatibleDC( pActiveWindow);
	//HBITMAP p_OldBM = (HBITMAP)Context.SelectObject (BMHan);

	TRY
	{
		ContextDC.SelectObject( m_BMHan);

		int ModWidth  = (int )(GetWidth()	* dfZoom);
		int ModHeight = (int )(GetHeight()	* dfZoom);

		HDC hDC = pActiveWindow->m_hDC;

		InternalStretchBlt( hDC, X,                Y,                 dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth,     Y,                 dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth * 2, Y,                 dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X,                Y + ModHeight,     dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth,     Y + ModHeight,     dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth * 2, Y + ModHeight,     dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X,                Y + ModHeight * 2, dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth,     Y + ModHeight * 2, dfZoom, ContextDC.m_hDC);
		InternalStretchBlt( hDC, X + ModWidth * 2, Y + ModHeight * 2, dfZoom, ContextDC.m_hDC);
	}
	CATCH_ALL( e)
	{
		MessageBeep( MB_ICONEXCLAMATION);
	}
	END_CATCH_ALL

	ContextDC.DeleteDC();
}
/*
/////////////////////////////////////////////////////////////////////////////////
//  Name:	Stretch
//	Action:	This function is similar to Show(), except that it takes as a parameter
//			an actual CRect, defining the borders that image should be stretched across.
//			Zoom value is unapplicable here, as the CRect gives us this information
void CDibSection::Stretch (CDC* ActiveWindow, CRect m_Rect, int Width, int Height)
{
	CDC Context;
	Context.CreateCompatibleDC (ActiveWindow);
	Context.SelectObject (BMHan);
	CPoint tl = m_Rect.TopLeft();
	CPoint br = m_Rect.BottomRight();
	ActiveWindow->StretchBlt (tl.x, tl.y, br.x - tl.x, br.y - tl.y, &Context, 0, 0, Width, Height, SRCCOPY );	
	Context.DeleteDC();
}

void CDibSection::Stretch (CPaintDC* ActiveWindow, CRect m_Rect, int Width, int Height)
{
	CDC Context;
	Context.CreateCompatibleDC (ActiveWindow);
	Context.SelectObject (BMHan);
	CPoint tl = m_Rect.TopLeft();
	CPoint br = m_Rect.BottomRight();
	ActiveWindow->StretchBlt (tl.x, tl.y, br.x - tl.x, br.y - tl.y, &Context, 0, 0, Width, Height, SRCCOPY );	
	Context.DeleteDC();
}
*/
void CDibSection::ShowRestricted (CDC* ActiveWindow, CRect rcClient)
{
	CDC Context;
	Context.CreateCompatibleDC (ActiveWindow);
	HBITMAP pOldBM = (HBITMAP)Context.SelectObject (m_BMHan);

	CPoint tl = rcClient.TopLeft();
	CPoint br = rcClient.BottomRight();
	int XRatio;
	int YRatio;

	// Neal - fix wrong-color pixels bug
	int iOldStretchMode = ActiveWindow->SetStretchBltMode( COLORONCOLOR);

	if (GetWidth() == GetHeight())
	{
		// It's square, no need to find the ratio
		ActiveWindow->StretchBlt (tl.x, tl.y, br.x - tl.x, br.y - tl.y, &Context, 0, 0, GetWidth(), GetHeight(), SRCCOPY);
	}
	else
	{
		float fSquish = (float)0.0;		
		float fWidth = (float)GetWidth();
		float fHeight = (float)GetHeight();
		float fClientWidth = (float)br.x - tl.x;
		float fClientHeight = (float)br.y - tl.y;		

		// Wider than taller?
		if (fWidth > fHeight)
		{			
			fSquish = (fClientWidth / fWidth);				
			XRatio = (int) (fSquish * fWidth);			
			YRatio = (int) (fSquish * fHeight);			
		}
		else
		{	
			fSquish = (fClientHeight / fHeight);							
			YRatio = (int) (fSquish * fHeight);			
			XRatio = (int) (fSquish * fWidth);			
		}		

		ActiveWindow->StretchBlt ( tl.x, tl.y, XRatio, YRatio, &Context, 0, 0, GetWidth(), GetHeight(), SRCCOPY);
	}
	ActiveWindow->SetStretchBltMode( iOldStretchMode);		// Neal

	Context.SelectObject (pOldBM);
	Context.DeleteDC();
}

////////////////////////////////////////////////////////////////////////////
//  Name:		AddDIB
//  Action:		Adds a DIB to this existing DIB, placing it at the X,Y coords
//				passed in.
//  Assumes:	The data being passed in is in the same format (8-bit or 24-bit)
//				as the current DIB being added to.
////////////////////////////////////////////////////////////////////////////
void CDibSection::AddDIB( int X, int Y, int iWidth, int iHeight, BYTE *pbyAddBits)
{
	int j = Y;
	int k = 0;

	for (; k < iHeight; j++, k++)
	{
		memcpy ((GetBits() + (GetByteWidth() * j) + X), (pbyAddBits + (k * iWidth * GetByteCount())), iWidth * GetByteCount());
	}
}

void CDibSection::AddLine (int iLineNumber, int iNumBits, BYTE *pbyBits)
{
	memcpy ((GetBits() + (GetByteWidth() * iLineNumber)), (pbyBits), iNumBits);
}

void CDibSection::ClearBits (int iIndex)
{		
	memset (GetBits(), iIndex, GetByteWidth() * GetHeight());
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	InitFromClipboard
//	Action:	Builds the DIB based on data currently on the clipboard
//
BOOL CDibSection::InitFromClipboard(CWnd *pWnd)
{
	if (!::IsClipboardFormatAvailable (CF_DIB))
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (!::OpenClipboard(NULL))
	{
		return FALSE;
	}

	// GetClipboardFile		
	HGLOBAL hg = ::GetClipboardData(CF_DIB);		
	if (hg == NULL)
	{
		return FALSE;
	}

	// Determine the data size
	DWORD dwDataSize = GlobalSize(hg); 
		
	// Build a buffer
	LPBYTE pbyTempBuffer = (LPBYTE)GlobalAlloc(GMEM_FIXED, dwDataSize);
	
	// Grab the data
	CopyMemory(pbyTempBuffer, GlobalLock(hg), dwDataSize); 

	// Get out as fast as we can from the clipboard
	GlobalUnlock(hg);
	::CloseClipboard();

	// Point a DIB structure to the beginning of the array
	ClipboardDataType uClipData;
	uClipData.lpDIB = (LPDIBType)pbyTempBuffer;
	LPDIBType lpDIB = uClipData.lpDIB;

	LPBYTE pbyDataOffset	= NULL; 
	LPBYTE pbyClipboardPalette = NULL;		// RGBx	
	BYTE byPalette[768];					// RGB
	
	UINT iColors = lpDIB->bmColorsUsed;
	UINT iWidth = lpDIB->bmWidth;
	UINT iHeight = lpDIB->bmHeight;	
	UINT BMHeaderSize = sizeof (DIBType);
	UINT j;

	switch (lpDIB->bmBitsPixel)
	{
	case 8:
		{
			ZeroMemory (byPalette, 768);

			pbyDataOffset = uClipData.pbyClipboardData + BMHeaderSize + (iColors * 4);			
			pbyClipboardPalette = (LPBYTE)(uClipData.pbyClipboardData + BMHeaderSize);

			for (j = 0; j < iColors; j++)
			{
				byPalette[j * 3 + 0] = pbyClipboardPalette[j * 4 + 2];
				byPalette[j * 3 + 1] = pbyClipboardPalette[j * 4 + 1];
				byPalette[j * 3 + 2] = pbyClipboardPalette[j * 4 + 0];				
			}

			if (!Init (iWidth, iHeight, 8, byPalette))
			{
				::GlobalFree ((HGLOBAL)pbyTempBuffer);
				return FALSE;
			}

			SetRawBits (pbyDataOffset, DIB_FLAG_WITH_PADDING);
		}
		break;

	case 24:
		{
			pbyDataOffset = uClipData.pbyClipboardData + BMHeaderSize;

			if (!Init (iWidth, iHeight, 24, NULL))
			{
				::GlobalFree ((HGLOBAL)pbyTempBuffer);
				return FALSE;
			}

			SetRawBits (pbyDataOffset, DIB_FLAG_WITH_PADDING);
		}
		break;

	default:
		{
			ASSERT (FALSE);
		}
		break;
	}			// switch (lpDIB->bmBitsPixel)

	FlipBits();

	// Cleanup
	::GlobalFree ((HGLOBAL)pbyTempBuffer);

	return TRUE;		
}


/////////////////////////////////////////////////////////////////////////////////
//  Name:	WriteToClipboard
//	Action:	As the name implies, this will copy the DIB to the clipboard, giving 
//			ownership then to the CWnd passed in.  BPP is used for the
//			bits per pixel value; the values 8BIT or 24BIT can be passed 
BOOL CDibSection::WriteToClipboard (CWnd *pWnd)
{
	// neal - TODO: use non-kludge method 
	// (some day, pass gamma instead of changing global)

	double dfHold = g_dfGamma;
	g_dfGamma = 1.0;

	RebuildPalette( m_byOriginalPalette, FALSE);

	BITMAPINFO	*pbmInfo		= NULL;
	BYTE		*pbyRawData		= NULL;
	BYTE		*pbyPixelData	= NULL;
	BYTE		*pbyBitsLine	= NULL;
	BYTE		*pbyPixelLine	= NULL;

	int			iSize		= GetByteWidth() * GetHeight();
	int         iDataSize	= sizeof( DIB_BITMAPINFO) + iSize;
	BOOL		bReturn			= TRUE;

	// Build the buffer
	HGLOBAL hData = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_DDESHARE, iDataSize);
	pbyRawData = (BYTE *)GlobalLock(hData);
	
	// Set the various pointers to areas within the buffer
	pbmInfo = (BITMAPINFO *)pbyRawData;
	
	if (GetBitCount() == 24)		// true color has no palette
	{
		pbyPixelData = pbyRawData + sizeof( BITMAPINFOHEADER);
	}
	else
	{
		pbyPixelData = pbyRawData + sizeof( DIB_BITMAPINFO);
	}	
	
	memcpy (pbmInfo, &m_BMInfo, sizeof( DIB_BITMAPINFO));

	// Change the biHeight member to be always positive, because the clipboard
	// doesn't care too much for them otherwise
	pbmInfo->bmiHeader.biHeight = abs (GetHeight());
	
	//  Bitmaps are stored bottom-up; we need to flip them around
	GetFlippedBits (pbyPixelData, DIB_FLAG_WITH_PADDING);
	
	GlobalUnlock(hData);

	if (::OpenClipboard(pWnd->GetSafeHwnd()))
	{
		::EmptyClipboard();		
		::SetClipboardData(CF_DIB, hData);								
		CloseClipboard();			
	}
	else
	{
		SetErrorCode (DIB_ERROR_CLIPBOARD);
		bReturn = FALSE;
	}
	
	g_dfGamma = dfHold;	
	RebuildPalette( m_byOriginalPalette, FALSE);

	return bReturn;
}


/////////////////////////////////////////////////////////////////////////////////
//  Name:	WriteToClipboardTiled
//	Action:	Very similar to WriteToClipboard, the only difference being the image
//			is copied Count*Count times, to form a tiled array.  This too is pretty
//			much a custom thing for Wally.
BOOL CDibSection::WriteToClipboardTiled( CWnd* pWnd, int iCountX, int iCountY)
{
	double dfHold = g_dfGamma;
	g_dfGamma = 1.0;

	RebuildPalette( m_byOriginalPalette, FALSE);
	
	BITMAPINFO* pbmInfo  = NULL;
	BYTE*       pbyRawData = NULL;
	BYTE*       pbyPixelData = NULL;
	BYTE*       pbyMipLine   = NULL;
	BYTE*       pbyPixelLine = NULL;
	
	int		iDataSize;	
	int		iTiledWidth				= PadDWORD( abs( GetWidth()) * iCountX * GetByteCount());
	int		iTiledHeight			= abs( GetHeight())  * iCountY;
	int		iTiledOriginalWidth		= abs( GetWidth())   * iCountX;
	int		iTiledOriginalHeight	= abs( GetHeight())  * iCountY;
	
	int		iSize = iTiledWidth * iTiledHeight;
	int		j, h, w;
	int		iRow;
	BOOL	bReturn = TRUE;

	iDataSize = sizeof( DIB_BITMAPINFO) + iSize;

	// Build the buffer
	HGLOBAL hData = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, iDataSize); 
	pbyRawData = (BYTE *)GlobalLock( hData);
	memset( pbyRawData, iDataSize, 0);
	
	// Set the various pointers to areas within the buffer
	pbmInfo = (BITMAPINFO *)pbyRawData;
	
	if (GetBitCount() == 24)		// true color has no palette
	{
		pbyPixelData = pbyRawData + sizeof( BITMAPINFOHEADER);
	}
	else
	{
		pbyPixelData = pbyRawData + sizeof( DIB_BITMAPINFO);
	}	

	memcpy( pbmInfo, &m_BMInfo, sizeof( DIB_BITMAPINFO));

	pbmInfo->bmiHeader.biHeight = iTiledOriginalHeight;
	pbmInfo->bmiHeader.biWidth  = iTiledOriginalWidth;

	//  Bitmaps are stored bottom-up; we need to flip them around		
	int x = 0;
	int y = 0;
	
	int iByteCount = GetByteCount();
	int iByteWidth = GetByteWidth();
	int iHeight = GetHeight();
	int iWidth = GetWidth();

	BYTE *pbyBits = GetBits();

	for (h = 0; h < iCountY; h++)
	{
		for (w = 0; w < iCountX; w++)
		{
			for (j = (iHeight - 1), iRow = 0; j >= 0; j--, iRow++)
			{
				pbyMipLine = pbyBits + (j * iByteWidth);
				
				x = (w * iWidth * iByteCount);
				y = (iTiledWidth * iRow) + (iTiledWidth * iHeight * h);
				
				pbyPixelLine = pbyPixelData + x + y;

				memcpy (pbyPixelLine, pbyMipLine, iWidth * iByteCount);
			}
		}
	}

	// Unlock her
	GlobalUnlock(hData);

	// Open the board and pass over the data handle
	if (::OpenClipboard( pWnd->GetSafeHwnd()))
	{
		::EmptyClipboard();		
		::SetClipboardData( CF_DIB, hData);								
		CloseClipboard();			
	}
	else
	{		
		SetErrorCode (DIB_ERROR_CLIPBOARD);
		bReturn = FALSE;
	}
	
	g_dfGamma = dfHold;	
	RebuildPalette( m_byOriginalPalette, FALSE);

	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	MirrorBits
//	Action:	Takes the valuable bits of this DIB and mirrors them on
//			the vertical axis:
//
//  Source:
//		0  1  2  3  4  5  (6)  (7)
//		8  9  10 11 12 13 (14) (15)
//		16 17 18 19 20 21 (22) (23)
//
//	Destination:
//		5  4  3  2  1  0  (6)  (7)
//		13 12 11 10 9  8  (14) (15)
//		21 20 19 18 17 16 (22) (23)

void CDibSection::MirrorBits()
{
	int i, j, k, w, h, iByteWidth, iByteCount;
	BYTE *pbyPixelData = NULL;	
	BYTE *pbyNonPaddedData = NULL;

	w = GetWidth();
	h = GetHeight();

	int iDataSize = w * h * GetByteCount();

	pbyPixelData		= new BYTE[iDataSize];	
	pbyNonPaddedData	= new BYTE[iDataSize];

	memset (pbyPixelData, 255, iDataSize);
	memset (pbyNonPaddedData, 255, iDataSize);

	// Copy over the non-padded m_byBits, as this image might not be DWORD aligned...
	// affecting what might get flipped (avoid the junk bits)
	GetRawBits (pbyNonPaddedData, 0);

	// This isn't padded:
	iByteCount = GetByteCount();
	iByteWidth = iByteCount * GetWidth();
	
	for (j = 0; j < h; j++)
	{
		// k is used to start from the right side decrementing to 0
		// i is used to start from zero, incrementing to the width
		for (k = ((w - 1) * iByteCount), i = 0; k >= 0; k -= iByteCount, i += iByteCount)
		{
			memcpy (
				pbyPixelData		+ (i + (j * w * iByteCount)),
				pbyNonPaddedData	+ (k + (j * w * iByteCount)),
				iByteCount);			
		}
	}
		
	SetRawBits (pbyPixelData, 0);
	
	if (pbyPixelData)
	{
		delete []pbyPixelData;
		pbyPixelData = NULL;
	}

	if (pbyNonPaddedData)
	{
		delete []pbyNonPaddedData;
		pbyNonPaddedData = NULL;
	}	
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	FlipBits
//	Action:	Takes the valuable bits of this DIB and flips them on the 
//			horizonal axis:
//
//  Source:
//		0  1  2  3  4  5  (6)  (7)
//		8  9  10 11 12 13 (14) (15)
//		16 17 18 19 20 21 (22) (23)
//
//	Destination:
//		16 17 18 19 20 21 (22) (23)
//		8  9  10 11 12 13 (14) (15)
//		0  1  2  3  4  5  (6)  (7)

void CDibSection::FlipBits()
{
	// Copy over the non-padded bits, as this image might not be DWORD aligned...
	// affecting what might get flipped (avoid the junk bits)
	int w = GetWidth();
	int h = GetHeight();
	int iByteCount = GetByteCount();
	
	int iDataSize = w * h * iByteCount;

	BYTE *pbyPixelData	  = new BYTE[iDataSize];	
	GetFlippedBits (pbyPixelData);
	SetRawBits (pbyPixelData, 0);

	if (pbyPixelData)
	{
		delete []pbyPixelData;
		pbyPixelData = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////
//  Name:	RotateBits
//	Action:	Takes the valuable bits of this DIB and rotates them based
//			on the value of DestinationAngle.  Destination can be one
//			of these three settings:
//
/*			ROTATETO90		(0)
			ROTATETO180		(1)
			ROTATETO270		(2)
 
    This is how each value would affect the rotation:
	

	Source:	
			1  -  2  -  3
			4  -  5  -  6
			7  -  8  -  9

	RotateTo90:
			7  -  4  -  1
			8  -  5  -  2     --> Original top
			9  -  6  -  3

	RotateTo180:
			9  -  8  -  7
			6  -  5  -  4
			3  -  2  -  1

	RotateTo270:
			3  -  6  -  9
			2  -  5  -  8
			1  -  4  -  7

	In all cases, the direction of the rotation (left or right) is 
	irrelevant; we only care about the final destination direction 
	for top
*/

void CDibSection::RotateBits(int iDestinationAngle)
{	
	int i, j, k, m, w, h, iOffset;
	BYTE *pbyPixelData		= NULL;
	BYTE *pbyNonPaddedData	= NULL;

	BYTE *pbyBits = GetBits();

	int iByteCount = GetByteCount();
	int iByteWidth = GetByteWidth();
	
	w = GetWidth();
	h = GetHeight();
	int iDataSize = w * h * iByteCount;
		
	switch (iDestinationAngle)
	{
	// Easiest solution here
	case ROTATETO180:
		{		
			FlipBits();
			MirrorBits();	
			
			return;
		}
		break;

	case ROTATETO90:
		{
			pbyPixelData		= new BYTE[iDataSize];
			pbyNonPaddedData	= new BYTE[iDataSize];

			// Copy over the non-padded bits
			GetRawBits (pbyNonPaddedData, 0);

			// Start at the bottom-left pixel (iDataSize - w) and loop 
			// through until the bottom-right (iDataSize - 1) is hit.  m
			// is used to set the actual offset
			for (j = (iDataSize - (w * iByteCount)), m = 0; j < iDataSize; j += iByteCount, m++)
			{
				iOffset = (m * h * iByteCount);

				// Start at one of the bottom row pixels, loop through until
				// the top row is hit, and decrement m_iOriginalWidth on
				// each pass, picking off the pixel data
				for (k = j, i = 0; i < h; k -= (w * iByteCount), i++)
				{
					memcpy (pbyPixelData + (iOffset + (i * iByteCount)), pbyNonPaddedData + k, iByteCount);
				}
			}
		}
		
		break;

	case ROTATETO270:
		{
			pbyPixelData		= new BYTE[iDataSize];
			pbyNonPaddedData	= new BYTE[iDataSize];

			// Copy over the non-padded bits
			GetRawBits (pbyNonPaddedData, 0);

			// Start at the top-right pixel (m_iOriginalWidth - 1) and loop 
			// through until the top-left pixel (0) is hit.  m is used to 
			// set the actual offset
			for (j = ((w - 1) * iByteCount), m = 0; j >= 0; j -= iByteCount, m++)
			{
				iOffset = (m * h * iByteCount);

				// Start at one of the top row pixels, loop through until
				// the bottom row is hit, and increment m_iOriginalWidth on
				// each pass, picking of the pixel data
				for (k = j, i = 0; i < h; k += (w * iByteCount), i++)
				{
					memcpy (pbyPixelData + (iOffset + (i * iByteCount)), pbyNonPaddedData + k, iByteCount);
				}
			}
		}

		break;

	default:
		ASSERT (false);		// Whoops!!  Some kinda goofy angle was passed in
		break;

	}		

	// Time to rebuild the bitmap if its not square
	FlipWidthHeight();
		
	// Copy over the rotated data
	SetRawBits (pbyPixelData, 0);

	if (pbyPixelData)
	{
		delete []pbyPixelData;
		pbyPixelData = NULL;
	}

	if (pbyNonPaddedData)
	{
		delete []pbyNonPaddedData;
		pbyNonPaddedData = NULL;
	}	
}

/*
void CDibSection::ShiftBits (int iDirection, int iFactor)
{
	int j, w, h;
	
	BYTE *pbyPixelData		= NULL;
	BYTE *pbyNonPaddedData	= NULL;
	BYTE *pbyPixelLine		= NULL;
	BYTE *pbyNonPaddedLine	= NULL;

	BYTE *pbyBits = GetBits();

	int iByteCount = GetByteCount();
	int iByteWidth = GetByteWidth();
		
	w = GetWidth();
	h = GetHeight();
	int iDataSize = w * h * iByteCount;
	
	pbyPixelData		= new BYTE[iDataSize];	
	pbyNonPaddedData	= new BYTE[iDataSize * 2];	

		
	switch (iDirection)
	{
	case (SHIFT_UP):
	
		// Copy over the non-padded bits
		GetRawBits (pbyNonPaddedData, 0);
		GetRawBits (pbyNonPaddedData + iDataSize, 0);

		for (j = 0; j < h; j++)
		{
			pbyPixelLine = pbyPixelData + (j * w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + ((j + iFactor) * w * iByteCount);
			memcpy (pbyPixelLine, pbyNonPaddedLine, w * iByteCount);
		}		
		break;

	case (SHIFT_DOWN):
		
		// Copy over the non-padded bits
		GetRawBits (pbyNonPaddedData, 0);
		GetRawBits (pbyNonPaddedData + iDataSize, 0);

		for (j = (h - 1); j >= 0; j--)
		{
			pbyPixelLine = pbyPixelData + (j * w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + iDataSize + ((j - iFactor) * w * iByteCount);
			memcpy (pbyPixelLine, pbyNonPaddedLine, w * iByteCount);
		}		
		break;

	case (SHIFT_LEFT):
		
		// Copy over the non-padded bits
		for (j = 0; j < h; j++)
		{
			pbyPixelLine = pbyBits + (j * iByteWidth);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount);
			memcpy (pbyNonPaddedLine, pbyPixelLine, w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount) + w * iByteCount;
			memcpy (pbyNonPaddedLine, pbyPixelLine, w * iByteCount);
		}
		
		for (j = 0; j < h; j++)
		{
			pbyPixelLine = pbyPixelData + (j * w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount) + (iFactor * iByteCount);
			memcpy (pbyPixelLine, pbyNonPaddedLine, w * iByteCount);
		}
		break;

	case (SHIFT_RIGHT):
		
		// Copy over the non-padded bits
		for (j = 0; j < h; j++)
		{
			pbyPixelLine = pbyBits + (j * iByteWidth);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount);
			memcpy (pbyNonPaddedLine, pbyPixelLine, w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount) + (w * iByteCount);
			memcpy (pbyNonPaddedLine, pbyPixelLine, w * iByteCount);
		}
		
		for (j = 0; j < h; j++)
		{
			pbyPixelLine = pbyPixelData + (j * w * iByteCount);
			pbyNonPaddedLine = pbyNonPaddedData + (j * w * 2 * iByteCount) + ((w - iFactor) * iByteCount);
			memcpy (pbyPixelLine, pbyNonPaddedLine, w * iByteCount);
		}
		break;


	default:
		ASSERT (false);
		break;
	}

	// Copy over the rotated data
	SetRawBits (pbyPixelData, 0);
		
	if (pbyPixelData)
	{
		delete []pbyPixelData;	// neal - must use array delete to delete arrays!
		pbyPixelData = NULL;
	}

	if (pbyNonPaddedData)
	{	
		delete []pbyNonPaddedData;
		pbyNonPaddedData = NULL;
	}
}
*/

////////////////////////////////////////////////////////////////////////////
//  Name:	FlipWidthHeight
//  Action:	To flip the width/height values of the bitmap, and recreate it
void CDibSection::FlipWidthHeight ()
{
	// Only bother if we're actually different width from height
	if (GetWidth() != GetHeight())
	{
		int iHold = GetWidth();
		SetWidth (GetHeight());
		SetHeight (iHold);
	
		m_BMInfo.bmiHeader.biWidth  = GetWidth();
		m_BMInfo.bmiHeader.biHeight = -abs (GetHeight());
		
		RebuildDibSection( m_byOriginalPalette);
	}	
}

/////////////////////////////////////////////////////////////////////////////
// Name:		SetRawLiquidBits
// Action:		Makes a wavy liquid texture
//
// Parameter:	iFrame: Frame number (must be between 0 and 15)
/////////////////////////////////////////////////////////////////////////////
void CDibSection::SetRawLiquidBits (unsigned char *Data, int iFrame)
{
	ASSERT (Data);  // Hey now, don't pass in a NULL value... block must exist

	ASSERT( (iFrame >= 0) && (iFrame <= 31));
	if ((iFrame < 0) || (iFrame > 31))
		iFrame = 0;

	int i, j;

//	static const int iMultValues[8]  = { 0, 1, 2, 1, 0, -1, -2, -1 };
//	static const int iMultValues[16] = { 0,  2,  4,  6,  8,  6,  4,  2,
//				                         0, -2, -4, -6, -8, -6, -4, -2 };
	static const int iMultValues[32] = 
		{ 0,  20,  30,  40,  50,  60,  70,  75,  80,  75,  70,  60,  50,  40,  30,  20, 
		  0, -20, -30, -40, -50, -60, -70, -75, -80, -75, -70, -60, -50, -40, -30, -20 };

	static bool   bOnce = TRUE;
	static double dfCosTable[64];
	int           iCosTable[64];

	if (bOnce)
	{
		bOnce = false;

		double pi = 3.1415926535;

		for (j = 0; j < 64; j++)
			dfCosTable[j] = cos( (j * 2.0 * pi) / 63.0);
	}

	//int iMult = iMultValues[iFrame];

	for (j = 0; j < 64; j++)
		iCosTable[j] = (int )(dfCosTable[j] * iMultValues[iFrame] / 10.0);

	ASSERT( m_byBits != NULL);

// Neal - no longer assumes power-of-two
//
//	int iMaskX = (m_iOriginalWidth-1);		// assumes power-of-two
//	int iMaskY = (m_iOriginalHeight-1);

	int iWidth     = GetWidth();
	int iHeight    = GetHeight();
	int iByteWidth = GetByteWidth();
	int iBitCount  = GetBitCount();

	for (j = 0; j < iHeight; j++)
	{
		//int iOffsetX = (int )( cos( ((j & 63) * 2.0 * pi) / 63.0) * (double )iMult );
		int iOffsetX = iCosTable[j&63];

		for (i = 0; i < iWidth; i++)
		{
			//int iOffsetY = 0;
			//int iOffsetY = (int )( cos( ((i & 63) * 2.0 * pi) / 63.0) * (double )iMult );
			int iOffsetY = iCosTable[i&63];

			// Neal - handle 24 bit
			if (iBitCount == 8)
			{
				m_byBits[j * iByteWidth + i] = 
//						Data[( ((j+iOffsetY) & iMaskY) * m_iOriginalWidth + 
//							   ((i+iOffsetX) & iMaskX) )];

						// Neal - "+m_iOriginal..." is to fix negative mod problem

						Data[( ((j+iOffsetY+ iHeight) % iHeight) * iWidth + 
							   ((i+iOffsetX+ iWidth) % iWidth) )];
			}
			else if ((iBitCount == 24) || ( iBitCount == 32))
			{
				int iMult = iBitCount / 8;

				// Neal - "+m_iOriginal..." is to fix negative mod problem

				int iOrigOffset = ((j+iOffsetY + iHeight) % iHeight) * iWidth + 
							   (((i+iOffsetX) % iWidth)*iMult);

				memcpy( &m_byBits[j * iByteWidth * iMult + i], &Data[iOrigOffset], iMult);
			}
			else
			{
				ASSERT( FALSE);		// What format is it?

				int iSize = iWidth * iHeight * iBitCount / 8;
				ZeroMemory( m_byBits, iSize);
			}

		}
	}
}

void CDibSection::SetRawBits (BYTE *pbyData, int iFlags /* = DIB_FLAG_FLIP_REDBLUE */)
{
	ASSERT (pbyData);  // Hey now, don't pass in a NULL value... block must exist

	BYTE *pbyBits = GetBits();
	ASSERT( pbyBits != NULL);

	int iWidth  = GetWidth();
	int iHeight = GetHeight();

	int iByteWidth = GetByteWidth();
	int iByteCount = GetByteCount();

	if (IsBadReadPtr( pbyData, iByteWidth * iHeight))
	{
		ASSERT( FALSE);
		return;
	}
	if (IsBadWritePtr( pbyBits, iWidth * iHeight))
	{
		ASSERT( FALSE);
		return;
	}

	// Neal - handle 24 bit

	if (GetBitCount() <= 8)
	{
		if ( (iByteWidth == iWidth) || (iFlags & DIB_FLAG_WITH_PADDING) )
		{
			memcpy (pbyBits, pbyData, iWidth * iHeight);
		}
		else
		{
			for (int j = 0; j < iHeight; j++)
			{
				BYTE* pbyBitsLine = pbyBits + (j * iByteWidth);
				BYTE* pbyDataLine = pbyData + (j * iWidth);

				memcpy (pbyBitsLine, pbyDataLine, iWidth);
			}
		}		
	}
	else
	{
		int iOrigOffset = 0;
		int iOffset = 0;
		
		for (int h = 0; h < iHeight; h++)
		{
			for (int w = 0; w < iWidth; w++)
			{
				iOffset	=	h * iByteWidth;
				iOffset	+=	w * iByteCount;

				if (iFlags & DIB_FLAG_WITH_PADDING)
				{
					iOrigOffset =	h * iByteWidth;					
				}
				else
				{
					iOrigOffset =	h * iWidth * iByteCount;					
				}

				iOrigOffset +=	w * iByteCount;

				if (m_iBitCount == 32)
				{
					BYTE byAlpha = pbyData[iOrigOffset+3];

					// Neal - TEMP CODE - show Alpha in browse view
					// TODO: this will need to be UPGRADED to allow alpha channel editing

					if (iFlags & DIB_FLAG_FLIP_REDBLUE)
					{
						pbyBits[iOffset]   = (pbyData[iOrigOffset+2] * byAlpha) / 255;
						pbyBits[iOffset+1] = (pbyData[iOrigOffset+1] * byAlpha) / 255;
						pbyBits[iOffset+2] = (pbyData[iOrigOffset+0] * byAlpha) / 255;
					}
					else
					{
						pbyBits[iOffset + 0] = (pbyData[iOrigOffset + 0] * byAlpha) / 255;
						pbyBits[iOffset + 1] = (pbyData[iOrigOffset + 1] * byAlpha) / 255;
						pbyBits[iOffset + 2] = (pbyData[iOrigOffset + 2] * byAlpha) / 255;
					}

					// Neal - END OF TEMP CODE - show Alpha in browse view

					pbyBits[iOffset+3] = byAlpha;
				}
				else
				{
					ASSERT( m_iBitCount == 24);

					if (iFlags & DIB_FLAG_FLIP_REDBLUE)
					{
						// Neal - swap red & blue (so screen looks right)
						pbyBits[iOffset+0] = pbyData[iOrigOffset+2];
						pbyBits[iOffset+1] = pbyData[iOrigOffset+1];
						pbyBits[iOffset+2] = pbyData[iOrigOffset+0];
					}
					else
					{
						memcpy (pbyBits + iOffset, pbyData + iOrigOffset, iByteCount);
					}
				}
			}
		}
	}
}

// Neal - always assumes DIB_FLAG_FLIP_REDBLUE
void CDibSection::SetBitsFromLayer( CLayer* pLayer, RECT *pRect, bool bOnlyToolDirtyArea, bool bTiled)
{
	// Neal - bTiled was added to fix paste cutout (while tiled) doesn't wrap bug

	ASSERT (pLayer);  // Hey now, don't pass in a NULL value... block must exist

	int w, h;

	BYTE *pbyBits = GetBits();
	ASSERT( pbyBits != NULL);

	int iWidth  = GetWidth();
	int iHeight = GetHeight();

	CRect Rect;
	if (pRect == NULL)
	{
		Rect.top    = 0;
		Rect.left   = 0;
		Rect.right  = iWidth;
		Rect.bottom = iHeight;
	}
	else
	{
		Rect = *pRect;
	}

	int iByteWidth = GetByteWidth();
	int iByteCount = GetByteCount();

	// Neal - handle 24 bit

	if (GetBitCount() <= 8)
	{
		for (h = Rect.top; h < Rect.bottom; h++)
		{
			int iY = h;
			if (bTiled)
			{
				if (iY < 0)
					iY += 1000 * iHeight;	// negative modulus problem work-around
				iY %= iHeight;
			}

			if ((iY >= 0) && (iY < pLayer->GetHeight()))
			{
				int iOffset = iY * iByteWidth + Rect.left;

				for (w = Rect.left; w < Rect.right; w++)
				{
					int iX = w;
					if (bTiled)
					{
						if (iX < 0)
							iX += 1000 * iWidth;	// negative modulus problem work-around
						iX %= iWidth;

						iOffset = iY * iByteWidth + iX;
					}

					if ((iX >= 0) && (iX < pLayer->GetWidth()))
					{
						int iIndex = (iY * iWidth) + iX;

						if ((! bOnlyToolDirtyArea) || (! g_Tool.m_pbySpotMarker) || 
									(g_Tool.m_pbySpotMarker[iIndex] != 0))
						{
							COLOR_IRGB irgbPixel = pLayer->GetPixel( iX, iY);

							// Neal - swap red & blue (so screen looks right)

							pbyBits[iOffset] = GetIValue( irgbPixel);
						}
					}
					iOffset++;
				}
			}
		}
	}
	else
	{		
		ASSERT( GetBitCount() == 24);

		for (h = Rect.top; h < Rect.bottom; h++)
		{
			int iY = h;
			if (bTiled)
			{
				if (iY < 0)
					iY += 1000 * iHeight;	// negative modulus problem work-around
				iY %= iHeight;
			}

			if ((iY >= 0) && (iY < pLayer->GetHeight()))
			{
				int iOffset = iY * iByteWidth + 3*Rect.left;

				for (w = Rect.left; w < Rect.right; w++)
				{
					int iX = w;
					if (bTiled)
					{
						if (iX < 0)
							iX += 1000 * iWidth;	// negative modulus problem work-around
						iX %= iWidth;

						iOffset = iY * iByteWidth + 3*iX;
					}

					if ((iX >= 0) && (iX < pLayer->GetWidth()))
					{
						int iIndex = (iY * iWidth) + iX;

						if ((! bOnlyToolDirtyArea) || (! g_Tool.m_pbySpotMarker) || 
									(g_Tool.m_pbySpotMarker[iIndex] != 0))
						{
							COLOR_IRGB irgbPixel = pLayer->GetPixel( iX, iY);

							// Neal - swap red & blue (so screen looks right)

							pbyBits[iOffset]   = GetBValue( irgbPixel);
							pbyBits[iOffset+1] = GetGValue( irgbPixel);
							pbyBits[iOffset+2] = GetRValue( irgbPixel);
						}
					}
					iOffset += 3;
				}
			}
		}
	}
}

void CDibSection::GetFlippedBits (BYTE *pbyData, int iFlags /* = 0 */)
{
	ASSERT (pbyData);  // Hey now, don't pass in a NULL value... block must already be created
	
	BYTE *pbyBitsLine		= NULL;
	BYTE *pbyPixelLine		= NULL;

	int iByteCount = GetByteCount();
	int iByteWidth = GetByteWidth();
	int iHeight = GetHeight();
	int iWidth = GetWidth();

	BYTE *pbyBits = GetBits();

	if (iFlags & DIB_FLAG_WITH_PADDING)
	{
		for (int j = (iHeight - 1); j >= 0; j--)
		{		
			pbyBitsLine = pbyBits + (j * iByteWidth);
			pbyPixelLine = pbyData + ((iHeight - j - 1) * iByteWidth);
			CopyMemory( pbyPixelLine, pbyBitsLine, iByteWidth);
		}
	}
	else
	{
		for (int j = (iHeight - 1); j >= 0; j--)
		{		
			pbyBitsLine = pbyBits + (j * iByteWidth);
			pbyPixelLine = pbyData + ((iHeight - j - 1) * iWidth * iByteCount);
			CopyMemory( pbyPixelLine, pbyBitsLine, iWidth * iByteCount);			
		}
	}
}

void CDibSection::GetRawBits (BYTE *pbyData, int iFlags /* = DIB_FLAG_FLIP_REDBLUE */)
{
	ASSERT (pbyData);  // Hey now, don't pass in a NULL value... block must already be created
	int j, k;
	BYTE *pbyBitsLine = NULL;
	BYTE *pbyDataLine = NULL;
	BYTE *pbyBits = GetBits();

	int iWidth = GetWidth();
	int iHeight = GetHeight();
	int iByteWidth = GetByteWidth();
	int iByteCount = GetByteCount();
	
	switch (GetBitCount())
	{
	case 8:
		{
			for (j = 0; j < iHeight; j++)
			{			
				pbyBitsLine = pbyBits + (j * iByteWidth);
				pbyDataLine = pbyData + (j * iWidth * iByteCount);
				memcpy (pbyDataLine, pbyBitsLine, iWidth * iByteCount);
			}
		}
		break;

	case 24:
	case 32:
		{
			int iOffset = 0;
			int iOrigOffset = 0;

			for (j = 0; j < iHeight; j++)
			{
				if (iFlags & DIB_FLAG_FLIP_REDBLUE)
				{
					for (k = 0; k < iWidth; k++)
					{
						iOffset = (j * iByteWidth) + k * iByteCount;
						iOrigOffset = (j * iWidth * iByteCount) + k * iByteCount;
						
						// Swap red & blue
						pbyData[iOrigOffset + 0] = pbyBits[iOffset + 2];
						pbyData[iOrigOffset + 1] = pbyBits[iOffset + 1];
						pbyData[iOrigOffset + 2] = pbyBits[iOffset + 0];
					}
				}
				else
				{
					pbyBitsLine = pbyBits + (j * iByteWidth);
					pbyDataLine = pbyData + (j * iWidth * iByteCount);
					memcpy (pbyDataLine, pbyBitsLine, iWidth * iByteCount);
				}
			}		
		}
		break;

	default:
		ASSERT (FALSE);
		break;

	}		

	pbyBitsLine = NULL;
	pbyDataLine = NULL;
}

COLOR_IRGB CDibSection::GetPixelAtIndex( UINT iIndex)
{	
	UINT iWidth = GetWidth();
	UINT iX = 0;
	UINT iY = 0;
	div_t dt;

	dt = div(iIndex, iWidth);
	iX = dt.rem;
	iY = dt.quot;

	return GetPixelAtXY( iX, iY);
}

COLOR_IRGB CDibSection::GetPixelAtXY( UINT iX, UINT iY)
{
	COLOR_IRGB iRGBPixel;
	
	UINT iByteWidth = GetByteWidth();
	UINT iByteCount = GetByteCount();
	
	UINT iOffset = (iY * iByteWidth) + (iX * iByteCount);
	LPBYTE pbyData = GetBits();

	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;

	switch (GetBitCount())
	{
	case 8:
		{
			LPBYTE pbyPalette = GetPalette();
			UINT iIndex = pbyData[iOffset];
			r = pbyPalette[iIndex * 3 + 0];
			g = pbyPalette[iIndex * 3 + 1];
			b = pbyPalette[iIndex * 3 + 2];
			
			iRGBPixel = IRGB ( iIndex, r, g, b);
		}
		break;

	case 24:
		{
			b = pbyData[iOffset + 0];
			g = pbyData[iOffset + 1];
			r = pbyData[iOffset + 2];

			iRGBPixel = IRGB ( 0, r, g, b);
		}
		break;

	default:
		ASSERT( FALSE);
		break;
	}

	return iRGBPixel;
}
