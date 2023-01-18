// PaletteNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PaletteNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteNameDlg dialog


CPaletteNameDlg::CPaletteNameDlg(CWnd* pParent /*=NULL*/, LPCTSTR szName)
	: CDialog(CPaletteNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaletteNameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	if (szName)
	{
		m_strName = szName;
	}
	else
	{
		m_strName = "";
	}
}


void CPaletteNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaletteNameDlg)
	DDX_Control(pDX, IDC_EDIT_NAME, m_edName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaletteNameDlg, CDialog)
	//{{AFX_MSG_MAP(CPaletteNameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteNameDlg message handlers

BOOL CPaletteNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText (IDC_EDIT_NAME, m_strName);
	m_edName.SetFocus();	
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaletteNameDlg::OnOK() 
{
	GetDlgItemText (IDC_EDIT_NAME, m_strName);
	
	CDialog::OnOK();
}
