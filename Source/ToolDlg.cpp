// ToolDlg.cpp : implementation file
// 

#include "stdafx.h"
#include "Wally.h"
#include "ToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog


CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	//{{AFX_MSG_MAP(CToolDlg)
	ON_BN_CLICKED(IDC_BTN_USE_RECOMMENDED, OnBtnUseRecommended)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolDlg message handlers

void CToolDlg::OnBtnUseRecommended() 
{
	CComboBox* pCombo = NULL;
/*
	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_RBTN_SIZE);
	pCombo->SetCurSel( 0);		// 3 x 3
*/
	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_LIGHTEN);
	pCombo->SetCurSel( 2);		// medium

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_DARKEN);
	pCombo->SetCurSel( 2);		// medium

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_SCRATCH);
	pCombo->SetCurSel( 2);		// medium
}

void CToolDlg::OnOK() 
{
	int i;
	CComboBox* pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_LIGHTEN);
	i = pCombo->GetCurSel();

	g_iLightenAmount = i;

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_DARKEN);
	i = pCombo->GetCurSel();

	g_iDarkenAmount = i;

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_SCRATCH);
	g_iScratchAmount = pCombo->GetCurSel();

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_TINT);
	i = pCombo->GetCurSel();

	g_iTintAmount = i;

	CDialog::OnOK();
}

BOOL CToolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int i;
	CComboBox* pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_LIGHTEN);

	switch (g_iLightenAmount)
	{
	case 5:			// 5%?
		i = 0;
		break;
	case 10:		// 10%?
	default:
		i = 1;
		break;
	case 20:		// 20%?
		i = 2;
		break;
	case 30:		// 30%?
		i = 3;
		break;
	case 50:		// 50%?
		i = 4;
		break;
	}
	pCombo->SetCurSel( i);

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_DARKEN);

	switch (g_iDarkenAmount)
	{
	case 95:		// 95%?
		i = 0;
		break;
	case 90:		// 90%?
		i = 1;
		break;
	case 80:		// 80%?
	default:
		i = 2;
		break;
	case 70:		// 70%?
		i = 3;
		break;
	case 50:		// 50%?
		i = 4;
		break;
	}
	pCombo->SetCurSel( i);

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_SCRATCH);
	pCombo->SetCurSel( g_iScratchAmount);

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_TINT);
	switch (g_iTintAmount)
	{
	case 10:		// 10%?
		i = 0;
		break;
	case 20:		// 20%?
		i = 1;
		break;
	default:
	case 30:		// 30%?
		i = 2;
		break;
	case 40:		// 40%?
		i = 3;
		break;
	case 50:		// 50%?
		i = 4;
		break;
	}
	pCombo->SetCurSel( i);

	return TRUE;  // return TRUE unless you set the focus to a control
}
