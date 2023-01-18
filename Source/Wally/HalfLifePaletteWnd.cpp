// HalfLifePaletteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "HalfLifePaletteWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteWnd

CHalfLifePaletteWnd::CHalfLifePaletteWnd()
{
	m_iColumns	= 16;
	m_iRows		= 16;
	
	m_iSelectedIndexLeft	= 0;
	m_iSelectedIndexRight	= 0;

	for (int j = 0; j < 256; j++)
	{
		memset (m_pbyPalette + (j * 3), j, 3);
	}
}

CHalfLifePaletteWnd::~CHalfLifePaletteWnd()
{
}


BEGIN_MESSAGE_MAP(CHalfLifePaletteWnd, CWnd)
	//{{AFX_MSG_MAP(CHalfLifePaletteWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteWnd message handlers

void CHalfLifePaletteWnd::SetPalette(unsigned char *pPal)
{
	memcpy (m_pbyPalette, pPal, 768);
}

unsigned char *CHalfLifePaletteWnd::GetPalette()
{
	return m_pbyPalette;
}

void CHalfLifePaletteWnd::OnPaint() 
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
			int r = m_pbyPalette[iColor * 3];
			int g = m_pbyPalette[iColor * 3 + 1];
			int b = m_pbyPalette[iColor * 3 + 2];

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

void CHalfLifePaletteWnd::Update()
{
	InvalidateRect (NULL, false);
}

int CHalfLifePaletteWnd::CalcZone( CPoint ptPos)
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

void CHalfLifePaletteWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	this->SetFocus ();
	m_iSelectedIndexLeft = CalcZone (point);
	Update();

	CWnd::OnLButtonDown(nFlags, point);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage( WM_COMMAND, IDC_PALETTE, 0);
	}
}

void CHalfLifePaletteWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{		
	this->SetFocus ();
	m_iSelectedIndexLeft = CalcZone (point);
	Update();
	
	CWnd::OnLButtonDblClk(nFlags, point);

	int r = m_pbyPalette[m_iSelectedIndexLeft * 3 + 0];
	int g = m_pbyPalette[m_iSelectedIndexLeft * 3 + 1];
	int b = m_pbyPalette[m_iSelectedIndexLeft * 3 + 2];

	COLORREF rgbColor (RGB (r, g, b));
	
	CColorDialog dlgColor (rgbColor, CC_FULLOPEN);		
	
	if (dlgColor.DoModal() == IDOK)
	{	
		rgbColor = dlgColor.GetColor();
		m_pbyPalette[m_iSelectedIndexLeft * 3 + 0] = GetRValue (rgbColor);
		m_pbyPalette[m_iSelectedIndexLeft * 3 + 1] = GetGValue (rgbColor);
		m_pbyPalette[m_iSelectedIndexLeft * 3 + 2] = GetBValue (rgbColor);
		Update();
	}
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage( WM_COMMAND, IDC_PALETTE, 0);
	}
}

void CHalfLifePaletteWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{	
	this->SetFocus ();
	m_iSelectedIndexRight = CalcZone (point);
	Update();
	
	CWnd::OnRButtonDblClk(nFlags, point);

	int r = m_pbyPalette[m_iSelectedIndexRight * 3 + 0];
	int g = m_pbyPalette[m_iSelectedIndexRight * 3 + 1];
	int b = m_pbyPalette[m_iSelectedIndexRight * 3 + 2];

	COLORREF rgbColor (RGB (r, g, b));
	
	CColorDialog dlgColor (rgbColor, CC_FULLOPEN);		
	
	if (dlgColor.DoModal() == IDOK)
	{	
		rgbColor = dlgColor.GetColor();
		m_pbyPalette[m_iSelectedIndexRight * 3 + 0] = GetRValue (rgbColor);
		m_pbyPalette[m_iSelectedIndexRight * 3 + 1] = GetGValue (rgbColor);
		m_pbyPalette[m_iSelectedIndexRight * 3 + 2] = GetBValue (rgbColor);
		Update();
	}
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage( WM_COMMAND, IDC_PALETTE, 0);
	}
}

void CHalfLifePaletteWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	this->SetFocus ();
	m_iSelectedIndexRight = CalcZone (point);
	Update();

	CWnd::OnRButtonDown(nFlags, point);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->PostMessage( WM_COMMAND, IDC_PALETTE, 0);
	}	
	
}

int CHalfLifePaletteWnd::GetSelectedIndex(int iButton)
{
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));
	return (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);
}

COLORREF CHalfLifePaletteWnd::GetSelectedIndexColor(int iButton)
{
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));

	int iIndex = (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);

	int r = m_pbyPalette[iIndex * 3 + 0];
	int g = m_pbyPalette[iIndex * 3 + 1];
	int b = m_pbyPalette[iIndex * 3 + 2];

	COLORREF rgbReturn (RGB (r, g, b));	
	return rgbReturn;

}

void CHalfLifePaletteWnd::SetSelectedIndex(int iIndex, int iButton)
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


void CHalfLifePaletteWnd::SetPaletteIndex (int iColorType, int iColor, int iButton)
{
	// This uses the currently selected index to update the color value
	ASSERT ((iColorType >= COLOR_RED) && (iColorType <= COLOR_BLUE));
	ASSERT ((iColor >= 0) && (iColor <= 255));
	ASSERT ((iButton >= BUTTON_LEFT) && (iButton <= BUTTON_RIGHT));

	int iIndex = (iButton == BUTTON_LEFT ? m_iSelectedIndexLeft : m_iSelectedIndexRight);

	int iOldColor = m_pbyPalette[iIndex * 3 + iColorType];

	if (iColor != iOldColor)
	{
		m_pbyPalette[iIndex * 3 + iColorType] = iColor;
		Update();
	}
}


