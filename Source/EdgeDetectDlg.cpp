// EdgeDetectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "WallyPal.h"
#include "Remip.h"
#include "filter.h"
#include "EdgeDetectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEdgeDetectDlg dialog


CEdgeDetectDlg::CEdgeDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEdgeDetectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEdgeDetectDlg)
	m_bMonochrome = FALSE;
	m_iEdgeDetectAmount = -1;
	m_iBackgroundColor = -1;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CEdgeDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEdgeDetectDlg)
	DDX_Check(pDX, IDC_CHECK_MONOCHROME, m_bMonochrome);
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iEdgeDetectAmount);
	DDX_Radio(pDX, IDC_RADIO_BLACK, m_iBackgroundColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEdgeDetectDlg, CDialog)
	//{{AFX_MSG_MAP(CEdgeDetectDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEdgeDetectDlg message handlers

void CEdgeDetectDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);

	int r, g, b;
	COLORREF rgbBackgroundColor = 0;

	CWallyPalette* pPal = m_pLayer->GetDoc()->GetPalette();

	if (IsDlgButtonChecked( IDC_RADIO_BLACK))
	{
		r = g = b = 0;
	}
	else if (IsDlgButtonChecked( IDC_RADIO_WHITE))
	{
		r = g = b = 255;
	}
	else if (IsDlgButtonChecked( IDC_RADIO_LEFT))
	{
//		r = pPal->GetR( g_iColorIndexLeft);
//		g = pPal->GetG( g_iColorIndexLeft);
//		b = pPal->GetB( g_iColorIndexLeft);
		rgbBackgroundColor = g_irgbColorLeft & 0x00FFFFFF;
	}
	else if (IsDlgButtonChecked( IDC_RADIO_RIGHT))
	{
//		r = pPal->GetR( g_iColorIndexRight);
//		g = pPal->GetG( g_iColorIndexRight);
//		b = pPal->GetB( g_iColorIndexRight);
		rgbBackgroundColor = g_irgbColorRight & 0x00FFFFFF;
	}
//	rgbBackgroundColor = RGB( r, g, b);

	g_Filter.DoEdgeDetect( m_pLayer, iAmount, rgbBackgroundColor,
			IsDlgButtonChecked( IDC_CHECK_MONOCHROME));

	m_bPreviewApplied = TRUE;
}
