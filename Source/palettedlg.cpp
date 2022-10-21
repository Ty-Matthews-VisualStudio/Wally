// PaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyPal.h"
#include "PaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPalettePickerDlg dialog


CPalettePickerDlg::CPalettePickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPalettePickerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPalettePickerDlg)
	m_iIndex = 0xFFFF;
	//}}AFX_DATA_INIT
}


void CPalettePickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPalettePickerDlg)
	DDX_Text(pDX, IDC_EDIT_INDEX, m_iIndex);
	DDV_MinMaxUInt(pDX, m_iIndex, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPalettePickerDlg, CDialog)
	//{{AFX_MSG_MAP(CPalettePickerDlg)
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPalettePickerDlg message handlers

BOOL CPalettePickerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		CWnd* pWnd = GetDlgItem( IDC_BTN_PALETTE);

	if (pWnd)
	{
		CRect Rect, rDlg;
		GetClientRect( &rDlg);
		ClientToScreen( rDlg);
		pWnd->GetWindowRect( &Rect);
		Rect.OffsetRect( -rDlg.left, -rDlg.top);
		pWnd->ShowWindow( SW_HIDE);

		m_wndColorPalette.m_iColumns = 16;
		m_wndColorPalette.m_iRows    = 16;

		m_wndColorPalette.m_iSelectedIndex      = m_iIndex;

		m_wndColorPalette.m_bUpdateMasterColors = FALSE;

		m_wndColorPalette.Create( NULL, NULL, 
			WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);

		m_wndColorPalette.Update( m_iIndex);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPalettePickerDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// clear background

//	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( lpDrawItemStruct->hDC, hBrush);
	::PatBlt( lpDrawItemStruct->hDC, 0, 0, 10000, 10000, PATCOPY);

	if (lpDrawItemStruct->CtlID == IDC_BTN_PALETTE)
	{
	}
	else if (lpDrawItemStruct->CtlID == IDC_BTN_UI_AREA)
	{
		int iWidth   = (lpDrawItemStruct->rcItem.right  - lpDrawItemStruct->rcItem.left - 1);
		int iHeight  = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top - 1);

		int iOffsetX = iWidth  / 4;
		int iOffsetY = iHeight / 4;

//		int r = g_CurrentPalette.GetR( g_iColorIndexRight);
//		int g = g_CurrentPalette.GetG( g_iColorIndexRight);
//		int b = g_CurrentPalette.GetB( g_iColorIndexRight);

		SelectObject( lpDrawItemStruct->hDC, hOldBrush);
//		DeleteObject( hBrush);
//		hBrush = CreateSolidBrush( RGB( r, g, b));
		hBrush = CreateSolidBrush( g_irgbColorRight & 0x00FFFFFF);

		if (UsesColors( g_iCurrentTool))
			SelectObject( lpDrawItemStruct->hDC, hBrush);
		else
			SelectObject( lpDrawItemStruct->hDC, GetStockObject( NULL_BRUSH));

		int iDeltaX = iWidth  - iOffsetX;
		int iDeltaY = iHeight - iOffsetY;

//		iDeltaX = min( iDeltaX, iDeltaY);
//		iDeltaY = iDeltaX;

		int iX = iOffsetX;
		int iY = iOffsetY;
		Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);

//		r = g_CurrentPalette.GetR( g_iColorIndexLeft);
//		g = g_CurrentPalette.GetG( g_iColorIndexLeft);
//		b = g_CurrentPalette.GetB( g_iColorIndexLeft);

		SelectObject( lpDrawItemStruct->hDC, hOldBrush);
		DeleteObject( hBrush);
//		hBrush = CreateSolidBrush( RGB( r, g, b));
		hBrush = CreateSolidBrush( g_irgbColorLeft & 0x00FFFFFF);

		if (UsesColors( g_iCurrentTool))
			SelectObject( lpDrawItemStruct->hDC, hBrush);
		else
			SelectObject( lpDrawItemStruct->hDC, GetStockObject( NULL_BRUSH));

		iX = iY = 0;
		Rectangle( lpDrawItemStruct->hDC, iX, iY, iX+iDeltaX+1, iY+iDeltaY+1);
	}

	SelectObject( lpDrawItemStruct->hDC, hOldBrush);
	DeleteObject( hBrush);
}
