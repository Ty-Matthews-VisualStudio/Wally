// PasteSpecialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PasteSpecialDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasteSpecialDlg dialog


CPasteSpecialDlg::CPasteSpecialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasteSpecialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasteSpecialDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPasteSpecialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasteSpecialDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasteSpecialDlg, CDialog)
	//{{AFX_MSG_MAP(CPasteSpecialDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasteSpecialDlg message handlers

void CPasteSpecialDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CPasteSpecialDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
