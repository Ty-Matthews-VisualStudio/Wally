// NewPaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "NewPaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewPaletteDlg dialog


CNewPaletteDlg::CNewPaletteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPaletteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewPaletteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_strPaletteName = "";
}


void CNewPaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPaletteDlg)
	DDX_Control(pDX, IDC_EDIT_PALETTE_NAME, m_edPaletteName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPaletteDlg, CDialog)
	//{{AFX_MSG_MAP(CNewPaletteDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPaletteDlg message handlers

void CNewPaletteDlg::OnOK() 
{	
	GetDlgItemText (IDC_EDIT_PALETTE_NAME, m_strPaletteName);

	CDialog::OnOK();
}

BOOL CNewPaletteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_edPaletteName.SetFocus();
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
