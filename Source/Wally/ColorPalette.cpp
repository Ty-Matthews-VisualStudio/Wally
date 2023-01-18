/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ColorPalette.cpp : implementation of the CColorPalette class
//
// Created by Neal White III, 6-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
//#include "WallyPal.h"
#include "ColorPalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPalette dialog


CColorPalette::CColorPalette(CWnd* pParent /*=NULL*/) : m_pDoc(NULL)
{
	//{{AFX_DATA_INIT(CColorPalette)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CColorPalette::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPalette)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPalette, CDialogBar)
	//{{AFX_MSG_MAP(CColorPalette)
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPalette message handlers

void CColorPalette::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
/*  UINT   CtlType;
    UINT   CtlID;
    UINT   itemID;
    UINT   itemAction;
    UINT   itemState;
    HWND   hwndItem;
    HDC    hDC;
    RECT   rcItem;
    DWORD  itemData;
*/
	ASSERT( FALSE);
/*
	// clear background
//	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( lpDrawItemStruct->hDC, hBrush);
	::PatBlt( lpDrawItemStruct->hDC, 0, 0, 10000, 10000, PATCOPY);

	#define NUM_COLUMNS 8
	#define NUM_ROWS    32

	if (lpDrawItemStruct->CtlID == IDC_BTN_PALETTE)
	{
	}
	else if (lpDrawItemStruct->CtlID == IDC_BTN_UI_AREA)
	{
		int iWidth  = (lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left - 1);
		int iHeight = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - 1);

		// neal - make 'em square

		int   iMin     = min( iWidth, iHeight);
		int   iDeltaX  = iMin * 70 / 100;
		int   iDeltaY  = iDeltaX;
		int   iOffsetX = ((iWidth  - iMin) / 2) + (iMin - iDeltaX);
		int   iOffsetY = ((iHeight - iMin) / 2) + (iMin - iDeltaY);
		int   iX       = iOffsetX;
		int   iY       = iOffsetY;
		CRect rRect;

		if (UsesColors( g_iCurrentTool))
		{
			int      r, g, b, iColor;
			COLORREF rgb;

			// Neal - apply gamma dynamically (bugfix)
			//
			double dfLambda = 1.0 / g_dfGamma;

			// Neal - this can happen when last image is closed
			if (m_pDoc && IsBadReadPtr( m_pDoc, sizeof( CWallyDoc)))
				m_pDoc = NULL;

			if (m_pDoc && (m_pDoc->GetOriginalBitDepth() <= 8))
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
				rgb    = RGB( r, g, b);
			}
			else
			{
				rgb = g_irgbColorRight & 0x00FFFFFF;
			}
			hBrush = CreateSolidBrush( rgb);

			SelectObject( lpDrawItemStruct->hDC, hBrush);
			Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);

			SelectObject( lpDrawItemStruct->hDC, hOldBrush);
			DeleteObject( hBrush);

			iX = ((iWidth  - iMin) / 2);
			iY = ((iHeight - iMin) / 2);

			if (g_iCurrentTool == EDIT_MODE_SELECTION)
			{
				hBrush = GetSysColorBrush( COLOR_BTNFACE);

				Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
				rRect.SetRect( iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
				DrawEdge( lpDrawItemStruct->hDC, &rRect, BDR_SUNKENOUTER, BF_RECT);
			}
			else
			{
				if (m_pDoc && (m_pDoc->GetOriginalBitDepth() <= 8))
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

				SelectObject( lpDrawItemStruct->hDC, hBrush);
				Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
				DeleteObject( hBrush);
			}
		}
		else
		{
			//SelectObject( lpDrawItemStruct->hDC, GetStockObject( NULL_BRUSH));
			//Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);

			rRect.SetRect( iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
			DrawEdge( lpDrawItemStruct->hDC, &rRect, EDGE_ETCHED, BF_RECT);

			iX = ((iWidth  - iMin) / 2);
			iY = ((iHeight - iMin) / 2);

			Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
			rRect.SetRect( iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
			DrawEdge( lpDrawItemStruct->hDC, &rRect, BDR_SUNKENOUTER, BF_RECT);
		}
	}
	SelectObject( lpDrawItemStruct->hDC, hOldBrush);
*/
}

void CColorPalette::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	static BOOL bFirstTime = TRUE;

	if (bFirstTime)
	{
		bFirstTime = FALSE;

		CWnd* pWnd = GetDlgItem( IDC_BTN_PALETTE);

		if (pWnd)
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			pWnd->GetWindowRect( &Rect);
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
//			pWnd->ShowWindow( SW_HIDE);
			pWnd->DestroyWindow();

			m_wndColorPalette.Create( NULL, NULL, 
				WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);

			SetWindowLong( m_wndColorPalette.m_hWnd, GWL_ID, IDC_BTN_PALETTE);
		}

		pWnd = GetDlgItem( IDC_BTN_UI_AREA);

		if (pWnd)
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			pWnd->GetWindowRect( &Rect);
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
//			pWnd->ShowWindow( SW_HIDE);
			pWnd->DestroyWindow();

			m_wndColorSwatch.Create( NULL, NULL, 
				WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);

			SetWindowLong( m_wndColorSwatch.m_hWnd, GWL_ID, IDC_BTN_UI_AREA);
			m_wndColorSwatch.m_pColorPaletteToolbar = this;
		}
		Update( m_pDoc, TRUE);
	}
	
	// Do not call CDialogBar::OnPaint() for painting messages
}
/*
void CColorPalette::OnSelChangeSize()
{
	if (ToolHasWidth( g_iCurrentTool))
	{
		CComboBox* pCombo  = (CComboBox* )GetDlgItem( ID_COMBO_SIZE);
		int        iCurSel = pCombo->GetCurSel();

		if (g_iCurrentTool == EDIT_MODE_SELECTION)
		{
			g_bPasteInvisibleBackground = IndexToBrushSize( iCurSel);
		}
		else if (g_iCurrentTool == EDIT_MODE_RIVETS)
		{
			g_iRivetDistance = IndexToBrushSize( iCurSel);
		}
		else
		{
			g_iDrawingWidth = IndexToBrushSize( iCurSel);
		}
	}
}

void CColorPalette::OnSelChangeShape()
{
	CComboBox* pCombo = (CComboBox* )GetDlgItem( ID_COMBO_SHAPE);
	WPARAM     wID    = IndexToBrushShapeId( pCombo->GetCurSel());

	AfxGetMainWnd()->SendMessage( WM_COMMAND, wID, 0);
}

void CColorPalette::OnSelChangeAmount()
{
	CComboBox* pCombo = (CComboBox* )GetDlgItem( ID_COMBO_AMOUNT);
	WPARAM     wID    = IndexToToolAmountId( pCombo->GetCurSel());

	AfxGetMainWnd()->SendMessage( WM_COMMAND, wID, 0);
}
*/

void CColorPalette::Update( CWallyDoc* pDoc, BOOL /*bToolSwitch*/)
{
	m_pDoc = pDoc;

	m_wndColorPalette.Update( UPDATE_GAMMA);
	m_wndColorPalette.InvalidateRect( NULL, FALSE);

	CWnd* pWnd = GetDlgItem( IDC_BTN_UI_AREA);
	if (pWnd)
		pWnd->InvalidateRect( NULL, FALSE);
	else
	{
		ASSERT( FALSE);
	}
}
