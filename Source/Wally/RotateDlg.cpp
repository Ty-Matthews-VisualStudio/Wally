/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// RotateDlg.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "RotateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg dialog


CRotateDlg::CRotateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRotateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotateDlg, CDialog)
	//{{AFX_MSG_MAP(CRotateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg message handlers

void CRotateDlg::OnOK() 
{
	CDialog::OnOK();

	if (IsDlgButtonChecked(IDC_LEFT_RADIO))
		g_iRotateDirection = ROTATE_LEFT;
	else
		g_iRotateDirection = ROTATE_RIGHT;

	if (IsDlgButtonChecked(IDC_90DEG_RADIO))
		g_iRotateDegrees = ROTATE_90;
	else if (IsDlgButtonChecked(IDC_180DEG_RADIO))
		g_iRotateDegrees = ROTATE_180;
	else
		g_iRotateDegrees = ROTATE_270;

}

BOOL CRotateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton( IDC_LEFT_RADIO,  (g_iRotateDirection == ROTATE_LEFT));
	CheckDlgButton( IDC_RIGHT_RADIO, (g_iRotateDirection == ROTATE_RIGHT));

	CheckDlgButton( IDC_90DEG_RADIO,	 (g_iRotateDegrees   == ROTATE_90));
	CheckDlgButton( IDC_180DEG_RADIO,	 (g_iRotateDegrees   == ROTATE_180));
	CheckDlgButton( IDC_270DEG_RADIO,	 (g_iRotateDegrees   == ROTATE_270));
	


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
