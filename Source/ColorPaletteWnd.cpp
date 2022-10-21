// ColorPaletteWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "resource.h"
#include "ColorPaletteWnd.h"
#include "WndList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWallyPalette g_CurrentPalette;

/////////////////////////////////////////////////////////////////////////////
// CColorSwatchWnd

BEGIN_MESSAGE_MAP(CColorSwatchWnd, CWnd)
	//{{AFX_MSG_MAP(CColorSwatchWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSwatchWnd message handlers

void CColorSwatchWnd::OnPaint() 
{
	CPaintDC PaintDC( this); // device context for painting

	CWallyDoc* pDoc = NULL;

	if (m_pColorPaletteToolbar)
	{
		pDoc = m_pColorPaletteToolbar->m_pDoc;
	}
	else
	{
		static BOOL bOneTime = TRUE;

		if (bOneTime)
		{
			ASSERT( FALSE);
			bOneTime = FALSE;
		}
	}

	// clear background
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( PaintDC.m_hDC, hBrush);

	CRect rClient;
	GetClientRect( &rClient);

	::PatBlt( PaintDC.m_hDC, rClient.left, rClient.top, rClient.right, rClient.bottom, PATCOPY);

	int iWidth  = rClient.Width() - 1;
	int iHeight = rClient.Height() - 1;

	// neal - make 'em square

	int iMin     = min( iWidth, iHeight);
	int iDeltaX  = iMin * 70 / 100;
	int iDeltaY  = iDeltaX;
	int iOffsetX = ((iWidth  - iMin) / 2) + (iMin - iDeltaX);
	int iOffsetY = ((iHeight - iMin) / 2) + (iMin - iDeltaY);

	m_rRight.SetRect( iOffsetX, iOffsetY, iOffsetX+iDeltaX+1, iOffsetY+iDeltaY+1);
	iOffsetX = ((iWidth  - iMin) / 2);
	iOffsetY = ((iHeight - iMin) / 2);
	m_rLeft.SetRect( iOffsetX, iOffsetY, iOffsetX+iDeltaX+1, iOffsetY+iDeltaY+1);

	if (UsesColors( g_iCurrentTool))
	{
		int      r, g, b, iColor;
		COLORREF rgb;

		// Neal - apply gamma dynamically (bugfix)
		//
		double dfLambda = 1.0 / g_dfGamma;

		// Neal - this can happen when last image is closed
		if (pDoc && IsBadReadPtr( pDoc, sizeof( CWallyDoc)))
			pDoc = NULL;

		if (pDoc && (pDoc->GetOriginalBitDepth() <= 8))
		{
			iColor = GetIValue( g_irgbColorRight);
			r      = g_CurrentPalette.GetR( iColor);
			g      = g_CurrentPalette.GetG( iColor);
			b      = g_CurrentPalette.GetB( iColor);

			if (g_bGammaCorrectPaletteToolbar)
			{
				r = (int )(exp( dfLambda * log( r / 255.0)) * 255.0);
				g = (int )(exp( dfLambda * log( g / 255.0)) * 255.0);
				b = (int )(exp( dfLambda * log( b / 255.0)) * 255.0);
			}
			rgb = RGB( r, g, b);
		}
		else
		{
			rgb = g_irgbColorRight & 0x00FFFFFF;
		}

		hBrush = CreateSolidBrush( rgb);

		SelectObject( PaintDC.m_hDC, hBrush);
		Rectangle( PaintDC.m_hDC, m_rRight.left, m_rRight.top, m_rRight.right, m_rRight.bottom);

		SelectObject( PaintDC.m_hDC, hOldBrush);
		DeleteObject( hBrush);

		if (g_iCurrentTool == EDIT_MODE_SELECTION)
		{
			FillRect( PaintDC.m_hDC, &m_rLeft, GetSysColorBrush( COLOR_BTNFACE));
			DrawEdge( PaintDC.m_hDC, &m_rLeft, BDR_SUNKENOUTER, BF_RECT);
		}
		else
		{
			if (pDoc && (pDoc->GetOriginalBitDepth() <= 8))
			{
				iColor = GetIValue( g_irgbColorLeft);
				r      = g_CurrentPalette.GetR( iColor);
				g      = g_CurrentPalette.GetG( iColor);
				b      = g_CurrentPalette.GetB( iColor);

				if (g_bGammaCorrectPaletteToolbar)
				{
					r = (int )(exp( dfLambda * log( r / 255.0)) * 255.0);
					g = (int )(exp( dfLambda * log( g / 255.0)) * 255.0);
					b = (int )(exp( dfLambda * log( b / 255.0)) * 255.0);
				}
				rgb    = RGB( r, g, b);
			}
			else
			{
				rgb = g_irgbColorLeft & 0x00FFFFFF;
			}
			hBrush = CreateSolidBrush( rgb);

			SelectObject( PaintDC.m_hDC, hBrush);
			Rectangle( PaintDC.m_hDC, m_rLeft.left, m_rLeft.top, m_rLeft.right, m_rLeft.bottom);
			DeleteObject( hBrush);
		}
	}
	else
	{
		DrawEdge( PaintDC.m_hDC, &m_rRight, EDGE_ETCHED, BF_RECT);
		FillRect( PaintDC.m_hDC, &m_rLeft, GetSysColorBrush( COLOR_BTNFACE));
		DrawEdge( PaintDC.m_hDC, &m_rLeft, BDR_SUNKENOUTER, BF_RECT);
	}
	SelectObject( PaintDC.m_hDC, hOldBrush);

	// Do not call CWnd::OnPaint() for painting messages
}

void CColorSwatchWnd::OnLButtonDown( UINT /*uFlags*/, CPoint ptPos)
{
	if (m_rLeft.PtInRect( ptPos))
		DisplayColorPicker( TRUE);
	else if (m_rRight.PtInRect( ptPos))
		DisplayColorPicker( FALSE);
}

void CColorSwatchWnd::OnRButtonDown( UINT uFlags, CPoint ptPos)
{
	OnLButtonDown( uFlags, ptPos);
}

void CColorSwatchWnd::DisplayColorPicker( BOOL bLeftColor)
{
	CWallyDoc* pDoc = NULL;

	if (m_pColorPaletteToolbar)
	{
		pDoc = m_pColorPaletteToolbar->m_pDoc;
	}
	else
	{
		ASSERT( FALSE);
	}

	if (pDoc && IsBadReadPtr( pDoc, sizeof( CWallyDoc)))
		pDoc = NULL;

	if (pDoc && (pDoc->GetOriginalBitDepth() <= 8))
	{
		AfxMessageBox( "You can only use the color picker when\n"
						"editing true-color (24 bit) images.");
		return;
	}

	COLORREF rgbColor = (bLeftColor) ? g_irgbColorLeft : g_irgbColorRight;
	rgbColor &= rgbColor & 0x00FFFFFF;

	CColorDialog dlgColor( rgbColor, CC_FULLOPEN);		
	
	if (dlgColor.DoModal() == IDOK)
	{	
		rgbColor = dlgColor.GetColor();
		int r    = GetRValue( rgbColor);
		int g    = GetGValue( rgbColor);
		int b    = GetBValue( rgbColor);

		rgbColor = IRGB( g_CurrentPalette.FindNearestColor( r, g, b, FALSE), r, g, b);

		if (bLeftColor)
			g_irgbColorLeft = rgbColor;
		else
			g_irgbColorRight = rgbColor;

		if (m_pColorPaletteToolbar)
			m_pColorPaletteToolbar->Update( pDoc, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CColorPaletteWnd

CColorPaletteWnd::CColorPaletteWnd()
{
	m_iColumns = 8;
	m_iRows    = 32;

	m_iSelectedIndex      = -1;
	m_bUpdateMasterColors = TRUE;

	// neal - defaults to standard Quake 2 palette
	g_CurrentPalette.SetPalette( quake2_pal, 256);
}

CColorPaletteWnd::~CColorPaletteWnd()
{
}

BEGIN_MESSAGE_MAP(CColorPaletteWnd, CWnd)
	//{{AFX_MSG_MAP(CColorPaletteWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CColorPaletteWnd message handlers

void CColorPaletteWnd::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting

	CRect rClient;
	GetClientRect( &rClient);

	CDC DC;
	DC.CreateCompatibleDC( &PaintDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( &PaintDC, rClient.Width(), rClient.Height());
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, 10000, 10000, PATCOPY);

	if (m_iColumns < 0)
	{
		ASSERT( FALSE);
		m_iColumns = 1;
	}
	if (m_iRows < 0)
	{
		ASSERT( FALSE);
		m_iRows = 1;
	}

	int i, j, iX, iY, iColor;
	int iWidth   = rClient.Width() - 1;
	int iHeight  = rClient.Height() - 1;
	int iDeltaX  = iWidth  / m_iColumns;
	int iDeltaY  = iHeight / m_iRows;
	int iOffsetX = (iWidth  - (iDeltaX * m_iColumns)) / 2;
	int iOffsetY = (iHeight - (iDeltaY * m_iRows))    / 2;

	iWidth  = iDeltaX * m_iColumns;
	iHeight = iDeltaY * m_iRows;

	iColor = 0;

	CPen PenEdge( PS_SOLID, 1, GetSysColor( COLOR_BTNFACE));
	DC.SelectObject( PenEdge);

	for (j = 0; j < iHeight; j += iDeltaY)
	{
		iY = j + iOffsetY;

		for (i = 0; i < iWidth; i += iDeltaX)
		{
			iX = i + iOffsetX;

			int r = g_CurrentPalette.GetR( iColor);
			int g = g_CurrentPalette.GetG( iColor);
			int b = g_CurrentPalette.GetB( iColor);

			// neal - just apply gamma dynamically (bugfix)

			double dfLambda = 1.0 / g_dfGamma;

			if (g_bGammaCorrectPaletteToolbar)
			{
				r = (int )(exp( dfLambda * log( r / 255.0)) * 255.0);
				g = (int )(exp( dfLambda * log( g / 255.0)) * 255.0);
				b = (int )(exp( dfLambda * log( b / 255.0)) * 255.0);
			}

			SelectObject( DC, hOldBrush);
			DeleteObject( hBrush);
			hBrush = CreateSolidBrush( RGB( r, g, b));
			SelectObject( DC, hBrush);

			DC.Rectangle( iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);

			iColor++;
		}
	}

	DC.SelectStockObject( NULL_BRUSH);
	CRect Rect;

	if (m_iColumns < 0)
	{
		ASSERT( FALSE);
		m_iColumns = 1;
	}
	if (m_iRows < 0)
	{
		ASSERT( FALSE);
		m_iRows = 1;
	}

	if (m_bUpdateMasterColors)
	{
//		iColor = g_iColorIndexRight;
		iColor = GetIValue( g_irgbColorRight);

		DC.SelectStockObject( BLACK_PEN);
		Rect.SetRect( iOffsetX, iOffsetY, iOffsetX+iDeltaX, iOffsetY+iDeltaY);
		Rect.OffsetRect( (iColor % m_iColumns) * iDeltaX, (iColor / m_iColumns) * iDeltaY);
		DC.Rectangle( Rect);
		DC.SelectStockObject( WHITE_PEN);
		Rect.InflateRect( 1, 1);
		DC.Rectangle( Rect);

		// neal - we want left button to be on "top"
//		iColor = g_iColorIndexLeft;
		iColor = GetIValue( g_irgbColorLeft);

		DC.SelectStockObject( BLACK_PEN);
		Rect.SetRect( iOffsetX, iOffsetY, iOffsetX+iDeltaX, iOffsetY+iDeltaY);
		Rect.OffsetRect( (iColor % m_iColumns) * iDeltaX, (iColor / m_iColumns) * iDeltaY);
		DC.Rectangle( Rect);
		DC.SelectStockObject( WHITE_PEN);
		Rect.InflateRect( 1, 1);
		DC.Rectangle( Rect);
	}
	else
	{
		iColor = m_iSelectedIndex;

		DC.SelectStockObject( BLACK_PEN);
		Rect.SetRect( iOffsetX, iOffsetY, iOffsetX+iDeltaX, iOffsetY+iDeltaY);
		Rect.OffsetRect( (iColor % m_iColumns) * iDeltaX, (iColor / m_iColumns) * iDeltaY);
		DC.Rectangle( Rect);
		DC.SelectStockObject( WHITE_PEN);
		Rect.InflateRect( 1, 1);
		DC.Rectangle( Rect);
	}

	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);

	PaintDC.BitBlt( 0, 0, 10000, 10000, &DC, 0, 0, SRCCOPY);

	DC.DeleteDC();

	// Do not call CWnd::OnPaint() for painting messages
}

int CColorPaletteWnd::CalcZone( CPoint ptPos)
{
	CRect rClient;
	GetClientRect( &rClient);

	if (m_iColumns < 0)
	{
		ASSERT( FALSE);
		m_iColumns = 1;
	}
	if (m_iRows < 0)
	{
		ASSERT( FALSE);
		m_iRows = 1;
	}

	int iWidth   = rClient.Width() - 1;
	int iHeight  = rClient.Height() - 1;
	int iDeltaX  = iWidth  / m_iColumns;
	int iDeltaY  = iHeight / m_iRows;
	int iOffsetX = (iWidth  - (iDeltaX * m_iColumns)) / 2;
	int iOffsetY = (iHeight - (iDeltaY * m_iRows))    / 2;

	iDeltaX = max( 1, iDeltaX);
	iDeltaY = max( 1, iDeltaY);

	iWidth  = iDeltaX * m_iColumns;
	iHeight = iDeltaY * m_iRows;

	int iX = (ptPos.x - iOffsetX) / iDeltaX;
	int iY = (ptPos.y - iOffsetY) / iDeltaY;

	if ((iX < m_iColumns) && (iY < m_iRows))
	{
		int iColor = (iY * m_iColumns) + iX;

		return iColor;
	}
	return -1;
}

void CColorPaletteWnd::Update( int iColor)
{
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->InvalidateRect( NULL, FALSE);

		if (pParent->GetDlgItem( IDC_EDIT_INDEX))
			pParent->SetDlgItemInt( IDC_EDIT_INDEX, iColor);

		if (g_dfGamma <= 0.0)
		{
			ASSERT( FALSE);
			g_dfGamma = 0.0001;
		}

		if (iColor == UPDATE_GAMMA)
		{
//			double dfLambda = 1.0 / g_dfGamma;
//
//			for (int iColor = 0; iColor < 256; iColor++)
//			{
//				int r = g_CurrentPalette.GetR( iColor);
//				int g = g_CurrentPalette.GetG( iColor);
//				int b = g_CurrentPalette.GetB( iColor);
//
//				if (g_bGammaCorrectPaletteToolbar)
//				{
//					r = (int )(exp( dfLambda * log( r / 255.0)) * 255.0);
//					g = (int )(exp( dfLambda * log( g / 255.0)) * 255.0);
//					b = (int )(exp( dfLambda * log( b / 255.0)) * 255.0);
//				}
//				g_CurrentPalette.SetRGB( iColor, r, g, b);
			}
//		}
//		else if (iColor == 255)
//		{
//			if (pParent->GetDlgItem( IDC_EDIT_RED))
//				pParent->SetDlgItemText( IDC_EDIT_RED, "- - -");
//
//			if (pParent->GetDlgItem( IDC_EDIT_GREEN))
//				pParent->SetDlgItemText( IDC_EDIT_GREEN, "- - -");
//
//			if (pParent->GetDlgItem( IDC_EDIT_BLUE))
//				pParent->SetDlgItemText( IDC_EDIT_BLUE, "- - -");
//		}
		else if ((iColor >= 0) && (iColor <= 255))
		{
			if (pParent->GetDlgItem( IDC_EDIT_RED))
				pParent->SetDlgItemInt( IDC_EDIT_RED, g_CurrentPalette.GetR( iColor));

			if (pParent->GetDlgItem( IDC_EDIT_GREEN))
				pParent->SetDlgItemInt( IDC_EDIT_GREEN, g_CurrentPalette.GetG( iColor));

			if (pParent->GetDlgItem( IDC_EDIT_BLUE))
				pParent->SetDlgItemInt( IDC_EDIT_BLUE, g_CurrentPalette.GetB( iColor));
		}
		else
		{
			ASSERT( (iColor >= 0) && (iColor <= 255));
		}
	}

	// Need to update the decal bits
	g_bUpdateDecalPreviewWnd = TRUE;

	// Mark those wnds affected
	g_InvalidateList.InvalidateAll(INVALIDATE_PALETTE_CHANGE);	
}

void CColorPaletteWnd::UpdateStatusBar( CPoint ptPos)
{
	int            iColor = CalcZone( ptPos);
	CWallyPalette* pPal   = &g_CurrentPalette;
	char           sbText[80];

	sprintf( sbText, "RGB:%3d,%3d,%3d   Color Index:%3d",
			pPal->GetR( iColor), pPal->GetG( iColor), pPal->GetB( iColor), iColor);

	m_wndStatusBar.SetPaneText( 0, sbText, true);
}

void CColorPaletteWnd::OnMouseMove( UINT uFlags, CPoint ptPos)
{
	UpdateStatusBar( ptPos);

	CWnd::OnMouseMove( uFlags, ptPos);
}

void CColorPaletteWnd::OnLButtonDown( UINT /*uFlags*/, CPoint ptPos)
{
	int iColor = CalcZone( ptPos);

	if (iColor != -1)
	{
		if (m_bUpdateMasterColors)
		{
//			g_iColorIndexLeft = iColor;
			g_irgbColorLeft   = IRGB( iColor, g_CurrentPalette.GetR( iColor), 
					g_CurrentPalette.GetG( iColor), g_CurrentPalette.GetB( iColor));
		}
		else
		{
			m_iSelectedIndex = iColor;
		}

		Update( iColor);
	}
	UpdateStatusBar( ptPos);	
}

void CColorPaletteWnd::OnLButtonDblClk( UINT uFlags, CPoint ptPos)
{
	OnLButtonDown( uFlags, ptPos);

	CWnd* pParent = GetParent();
	if (pParent)
		pParent->PostMessage( WM_COMMAND, IDOK, 0);
}

void CColorPaletteWnd::OnRButtonDown( UINT /*uFlags*/, CPoint ptPos)
{
	int iColor = CalcZone( ptPos);

	if (iColor != -1)
	{
		if (m_bUpdateMasterColors)
		{
//			g_iColorIndexRight = iColor;
			g_irgbColorRight   = IRGB( iColor, g_CurrentPalette.GetR( iColor), 
					g_CurrentPalette.GetG( iColor), g_CurrentPalette.GetB( iColor));
		}
		else
			m_iSelectedIndex = iColor;

		Update( iColor);
	}
	UpdateStatusBar( ptPos);	
}

void CColorPaletteWnd::OnRButtonDblClk( UINT uFlags, CPoint ptPos)
{
	OnRButtonDown( uFlags, ptPos);

	CWnd* pParent = GetParent();
	if (pParent)
		pParent->PostMessage( WM_COMMAND, IDOK, 0);
}

BOOL CColorPaletteWnd::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message) 
{
	::SetCursor( AfxGetApp()->LoadCursor( IDC_DROPPER_CURSOR));

	return TRUE;
}
