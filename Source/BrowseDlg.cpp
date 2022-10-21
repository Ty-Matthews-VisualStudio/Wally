// BrowseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "BrowseDlg.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg dialog


CBrowseDlg::CBrowseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBrowseDlg)
	m_szDirectoryEdit = _T("");
	//}}AFX_DATA_INIT
}


void CBrowseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowseDlg)
	DDX_Control(pDX, IDC_EDIT_PATH, m_DirectoryEdit);
	DDX_Text(pDX, IDC_EDIT_PATH, m_szDirectoryEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowseDlg, CDialog)
	//{{AFX_MSG_MAP(CBrowseDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, OnSelectButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg message handlers

void CBrowseDlg::OnOk() 
{
	CString Directory;
	m_DirectoryEdit.GetWindowText (Directory);

	SetDirectory (Directory);
	CDialog::OnOK();
}

void CBrowseDlg::OnSelectButton() 
{
	CString Directory;
	Directory = BrowseForFolder("Please select the folder to browse.");

	if (Directory != "")
	{	
		m_DirectoryEdit.SetWindowText(Directory);
	}
}

BOOL CBrowseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_DirectoryEdit.SetFocus();
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
