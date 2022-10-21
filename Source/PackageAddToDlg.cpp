// PackageAddToDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PackageAddToDlg.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackageAddToDlg dialog


CPackageAddToDlg::CPackageAddToDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackageAddToDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPackageAddToDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPackageAddToDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackageAddToDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPackageAddToDlg, CDialog)
	//{{AFX_MSG_MAP(CPackageAddToDlg)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageAddToDlg message handlers

void CPackageAddToDlg::OnButtonSource() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);
	
	strDirectory = BrowseForFolder ("Select Source Directory", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);
		m_strDirectory = strDirectory;
	}
}

void CPackageAddToDlg::OnOK() 
{
	GetDlgItemText (IDC_EDIT_WILDCARDS, m_strWildCards);	
	CDialog::OnOK();
}
