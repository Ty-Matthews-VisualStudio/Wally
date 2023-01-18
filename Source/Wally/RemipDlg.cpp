// RemipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "RemipDlg.h"
#include "Remip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemipDlg dialog


CRemipDlg::CRemipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemipDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRemipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemipDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemipDlg, CDialog)
	//{{AFX_MSG_MAP(CRemipDlg)
	ON_BN_CLICKED(IDC_BTN_USE_RECOMMENDED, OnBtnUseRecommended)
	ON_BN_CLICKED(IDC_BTN_USE_QUAKE, OnBtnUseQuake)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemipDlg message handlers

void CRemipDlg::OnOK() 
{
	g_bRebuildSubMipsOnSave = IsDlgButtonChecked( IDC_CHECK_REBUILD_SUBMIPS);
	g_bAutoRemip            = IsDlgButtonChecked( IDC_CHECK_AUTO_REMIP);
	g_bAutoRemipAfterEdit   = IsDlgButtonChecked( IDC_CHECK_AUTO_REMIP2);
	g_bWeightedAverage      = IsDlgButtonChecked( IDC_CHECK_USE_WEIGHTED);

	g_bErrorDiffusion       = IsDlgButtonChecked( IDC_RADIO_ERROR_DIFUSSION);
	g_bNoise                = IsDlgButtonChecked( IDC_RADIO_WHITE_NOISE);

	if (IsDlgButtonChecked( IDC_RADIO_HEAVY))
		g_iSharpen = SHARPEN_HEAVY;
	else if (IsDlgButtonChecked( IDC_RADIO_MEDIUM))
		g_iSharpen = SHARPEN_MEDIUM;
	else if (IsDlgButtonChecked( IDC_RADIO_LIGHT))
		g_iSharpen = SHARPEN_LIGHT;
	else
		g_iSharpen = SHARPEN_NONE;



	CDialog::OnOK();
}

void CRemipDlg::OnBtnUseRecommended() 
{
	CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    1);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, 1);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     0);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       0);

	CheckDlgButton( IDC_RADIO_HEAVY,           0);
	CheckDlgButton( IDC_RADIO_MEDIUM,          1);
	CheckDlgButton( IDC_RADIO_LIGHT,           0);
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   0);
}

void CRemipDlg::OnBtnUseQuake() 
{
	CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    0);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, 1);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     0);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       0);

	CheckDlgButton( IDC_RADIO_HEAVY,           0);
	CheckDlgButton( IDC_RADIO_MEDIUM,          0);
	CheckDlgButton( IDC_RADIO_LIGHT,           0);
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   1);
}

BOOL CRemipDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton( IDC_CHECK_REBUILD_SUBMIPS, g_bRebuildSubMipsOnSave);
	CheckDlgButton( IDC_CHECK_AUTO_REMIP,      g_bAutoRemip);
	CheckDlgButton( IDC_CHECK_AUTO_REMIP2,     g_bAutoRemipAfterEdit);
	CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    g_bWeightedAverage);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, g_bErrorDiffusion);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     g_bNoise);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       !( g_bErrorDiffusion | g_bNoise));

	CheckDlgButton( IDC_RADIO_HEAVY,           (g_iSharpen == SHARPEN_HEAVY));
	CheckDlgButton( IDC_RADIO_MEDIUM,          (g_iSharpen == SHARPEN_MEDIUM));
	CheckDlgButton( IDC_RADIO_LIGHT,           (g_iSharpen == SHARPEN_LIGHT));
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   (g_iSharpen == SHARPEN_NONE));

	return TRUE;  // return TRUE unless you set the focus to a control
}
