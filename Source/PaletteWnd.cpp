/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// PaletteWnd.cpp : interface of the CPaletteWnd class
//
// Created by Ty Matthews, 12-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "PaletteWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int WM_PALETTEWND_CUSTOM = RegisterWindowMessage ("WM_PALETTEWND_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CPaletteWnd

CPaletteWnd::CPaletteWnd()
{
	m_iColumns	= 16;
	m_iRows		= 16;
	
	m_iSelectedIndexLeft	= 0;
	m_iSelectedIndexRight	= 0;

	for (int j = 0; j < 256; j++)
	{
		memset (m_byPalette + (j * 3), j, 3);
	}
}

CPaletteWnd::~CPaletteWnd()
{
}


BEGIN_MESSAGE_MAP(CPaletteWnd, CWnd)
	//{{AFX_MSG_MAP(CPaletteWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPaletteWnd message handlers

void CPaletteWnd::SetPalette(unsigned char *pPal)
{
	memcpy (m_byPalette, pPal, 768);
}

unsigned char *CPaletteWnd::GetPalette()
{
	return m_byPalette;
}

void CPaletteWnd::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting

	CRect rcItem;
	GetClientRect( &rcItem);

	CDC DC;
	DC.CreateCompatibleDC( &PaintDC);
	CBitmap Bmp;	
	Bmp.CreateCompatibleBitmap( &PaintDC, rcItem.Width(), rcItem.Height());
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, 10000, 10000, PATCOPY);
	
	int iColor = 0;

	CPen PenEdge( PS_SOLID, 1, GetSysColor( COLOR_BTNFACE));
	DC.SelectObject( PenEdge);

	int h, w, iX, iY;
	int iOffset = 0;

	for (h = 0; h < m_iRows; h++)
	{
		for (w = 0; w < m_iColumns; w++)
		{	
			int r = m_byPalette[iColor * 3];
			int g = m_byPalette[iColor * 3 + 1];
			int b = m_byPalette[iColor * 3 + 2];

			if (iColor == m_iSelectedIndexRight)
			{
				SelectObject( DC, hOldBrush);
				DeleteObject( hBrush);
				hBrush = CreateSolidBrush( RGB( 255, 255, 0));
				SelectObject( DC, hBrush);
				iX = w * (PALETTE_THUMBNAIL_WIDTH + 3);
				iY = h * (PALETTE_THUMBNAIL_HEIGHT + 3);
				DC.Rectangle (iX, iY, iX + PALETTE_HIGHLIGHT_WIDTH + 2, iY + PALETTE_HIGHLIGHT_HEIGHT + 2);
			}

			if (iColor == m_iSelectedIndexLeft)
			{
				SelectObject( DC, hOldBrush);
				DeleteObject( hBrush);
				hBrush = CreateSolidBrush( RGB( 255, 0, 0));
				SelectObject( DC, hBrush);
				iX = w * (PALETTE_THUMBNAIL_WIDTH + 3);
				iY = h * (PALETTE_THUMBNAIL_HEIGHT + 3);
				DC.Rectangle (iX, iY, iX + PALETTE_HIGHLIGHT_WIDTH + 2, iY + PALETTE_HIGHLIGHT_HEIGHT + 2);
			}			

			SelectObject( DC, hOldBrush);
			DeleteObject( hBrush);
			hBrush = CreateSolidBrush( RGB( r, g, b));
			SelectObject( DC, hBrush);
			
			iX = 2 + (w * (PALETTE_THUMBNAIL_WIDTH + 3));
			iY = 2 + (h * (PALETTE_THUMBNAIL_HEIGHT + 3));
			DC.Rectangle (iX, iY, iX + PALETTE_THUMBNAIL_WIDTH + 2, iY + PALETTE_THUMBNAIL_HEIGHT + 2);
			iColor++;
		}		
	}
	
	DC.SelectStockObject( NULL_BRUSH);
	CRect Rect;
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);

	PaintDC.BitBlt( 0, 0, 10000, 10000, &DC, 0, 0, SRCCOPY);

	DC.DeleteDC();	
}

void CPaletteWnd::Update()
{
	InvalidateRect (NULL, false);
}

int CPaletteWnd::CalcZone( CPoint ptPos)
{
	CRect rcItem;
	GetClientRect( &rcItem);
	div_t x, y;
	int iX = ptPos.x - 2;
	int iY = ptPos.y - 2;	

	iX = min (iX, (PALETTE_THUMBNAIL_WIDTH + 2) * (m_iColumns + 1));
	iX = max (iX, 0);
	iY = min (iY, (PALETTE_THUMBNAIL_HEIGHT + 2) * (m_iRows + 1));
	iY = max (iY, 0);

	x = div (iX, PALETTE_THUMBNAIL_WIDTH + 3);
	iX = x.quot;

	y = div (iY, PALETTE_THUMBNAIL_HEIGHT + 3);
	iY = y.quot;

	int iColor = (iY * m_iColumns) + iX;
	return iColor;	
}

void CPaletteWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	this->SetFocus ();
	m_iSelectedIndexLeft = CalcZone (point);
	Update();

	CWnd::OnLButtonDown(nFlags, point);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{			
		pParent->SendMessage( WM_PALETTEWND_CUSTOM, PALETTEWND_LBUTTONDOWN, 0);
	}	
}

void CPaletteWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{		
	this->SetFocus ();
	m_iSelectedIndexLeft = CalcZone (point);
	Update();
	
	CWnd::OnLButtonDblClk(nFlags, point);

	int r = m_byPalette[m_iSelectedIndexLeft * 3 + 0];
	int g = m_byPalette[m_iSelectedIndexLeft * 3 + 1];
	int b = m_byPalette[m_iSelectedIndexLeft * 3 + 2];

	COLORREF rgbColor (RGB (r, g, b));
	
	CColorDialog dlgColor (rgbColor, CC_FULLOPEN);		
	
	if (dlgColor.DoModal() == IDOK)
	{	
		rgbColor = dlgColor.GetColor();
		m_byPalette[m_iSelectedIndexLeft * 3 + 0] = GetRValue (rgbColor);
		m_byPalette[m_iSelectedIndexLeft * 3 + 1] = GetGValue (rgbColor);
		m_byPalette[m_iSelectedIndexLeft * 3 + 2] = GetBValue (rgbColor);
		Update();
	}
	
	CWnd* pParent = GetParent();
	if (pParent)
	{			
		pParent->SendMessage( WM_PALETTEWND_CUSTOM, PALETTEWND_LBUTTONDBLCLK, 0);
	}	
}

void CPaletteWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{	
	this->SetFocus ();
	m_iSelectedIndexRight = CalcZone (point);
	Update();
	
	CWnd::OnRButtonDblClk(nFlags, point);

	int r = m_byPalette[m_iSelectedIndexRight * 3 + 0];
	int g = m_byPalette[m_iSelectedIndexRight * 3 + 1];
	int b = m_byPalette[m_iSelectedIndexRight * 3 + 2];

	COLORREF rgbColor (RGB (r, g, b));
	
	CColorDialog dlgColor (rgbColor, CC_FULLOPEN);		
	
	if (dlgColor.DoModal() == IDOK)
	{	
		rgbColor = dlgColor.GetColor();
		m_byPalette[m_iSelectedIndexRight * 3 + 0] = GetRValue (rgbColor);
		m_byPalette[m_iSelectedIndexRight * 3 + 1] = GetGValue (rgbColor);
		m_byPalette[m_iSelectedIndexRight * 3 + 2] = GetBValue (rgbColor);
		Update();
	}
	
	CWnd* pParent = GetParent();
	if (pParent)
	{			
		pParent->SendMessage( WM_PALETTEWND_CUSTOM, PALETTEWND_RBUTTONDBLCLK, 0);
	}	
}

void CPaletteWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	this->SetFocus ();
	m_iSelectedIndexRight = CalcZone (point);
	Update();

	CWnd::OnRButtonDown(nFlags, point);
	
	CWnd* pParent = GetParent();	
	if (pParent)
	{			
		pParent->SendMessage( WM_PALETTEWND_CUSTOM, PALETTEWND_RBUTTONDOWN, 0);
	}	
}

int CPaletteWnd::GetSelectedIndex(int iButton)
{
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));
	return (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);
}

COLORREF CPaletteWnd::GetSelectedIndexColor(int iButton)
{
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));

	int iIndex = (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);

	int r = m_byPalette[iIndex * 3 + 0];
	int g = m_byPalette[iIndex * 3 + 1];
	int b = m_byPalette[iIndex * 3 + 2];

	COLORREF rgbReturn (RGB (r, g, b));	
	return rgbReturn;

}

void CPaletteWnd::SetSelectedIndex(int iIndex, int iButton)
{
	ASSERT ((iIndex >= 0) && (iIndex <= 255));
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));

	int iIndexCurrent = (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);

	if (iIndex != iIndexCurrent)
	{
		switch (iButton)
		{
		case BUTTON_LEFT:
			m_iSelectedIndexLeft = iIndex;
			break;

		case BUTTON_RIGHT:
			m_iSelectedIndexRight = iIndex;
			break;

		default:
			ASSERT (false);
			break;
		}
		
		Update();
	}
}


void CPaletteWnd::SetPaletteIndex (int iColorType, int iColor, int iButton)
{
	// This uses the currently selected index to update the color value
	ASSERT ((iColorType >= COLOR_RED) && (iColorType <= COLOR_BLUE));
	ASSERT ((iColor >= 0) && (iColor <= 255));
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));

	int iIndex = (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);

	int iOldColor = m_byPalette[iIndex * 3 + iColorType];

	if (iColor != iOldColor)
	{
		m_byPalette[iIndex * 3 + iColorType] = iColor;
		Update();
	}
}



void CPaletteWnd::Blend() 
{
	float rl = 0;
	float gl = 0;
	float bl = 0;
	float rh = 0;
	float gh = 0;
	float bh = 0;

	float rAdj = 0.0;
	float gAdj = 0.0;
	float bAdj = 0.0;

	float fSeparation = 0.0;
	int iSeparation = 0;

	float r = 0.0;
	float g = 0.0;
	float b = 0.0;	
	
	int iIndexLeft	= 0;
	int iIndexRight	= 0;
	int iIndexLow	= 0;
	int iIndexHigh	= 0;	

	int j = 0;
	int k = 0;

	iIndexLeft = GetSelectedIndex (BUTTON_LEFT);
	iIndexRight = GetSelectedIndex (BUTTON_RIGHT);	

	iSeparation = (abs(iIndexRight - iIndexLeft));
	if ((iSeparation < 2) || (iIndexLeft == iIndexRight))
	{
		// There's nothing to blend!
		return;
	}

	fSeparation = (float)(iSeparation);

	iIndexLow	= min (iIndexRight, iIndexLeft);
	iIndexHigh	= max (iIndexRight, iIndexLeft);
	
	rl = (float)(m_byPalette[iIndexLow * 3]);
	gl = (float)(m_byPalette[iIndexLow * 3 + 1]);
	bl = (float)(m_byPalette[iIndexLow * 3 + 2]);	
	
	rh = (float)(m_byPalette[iIndexHigh * 3]);
	gh = (float)(m_byPalette[iIndexHigh * 3 + 1]);
	bh = (float)(m_byPalette[iIndexHigh * 3 + 2]);

	// Determine the adjustment amounts
	rAdj = (float)((rh - rl) / fSeparation);

	gAdj = (float)((gh - gl) / fSeparation);	

	bAdj = (float)((bh - bl) / fSeparation);	

	r = rl;
	g = gl;
	b = bl;

	for (j = (iIndexLow + 1); j < iIndexHigh; j++)
	{
		r += rAdj;
		g += gAdj;
		b += bAdj;
		m_byPalette[j * 3]		= (int)r;
		m_byPalette[j * 3 + 1]	= (int)g;
		m_byPalette[j * 3 + 2]	= (int)b;
	}	
	Update();
}