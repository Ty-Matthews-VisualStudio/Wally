// RenameImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "RenameImageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameImageDlg dialog


CRenameImageDlg::CRenameImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameImageDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	SetMaxLength(15);
}


void CRenameImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameImageDlg)	
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edName);
	//}}AFX_DATA_MAP

	int iLength = GetMaxLength();
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, iLength);
}


BEGIN_MESSAGE_MAP(CRenameImageDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameImageDlg)
	ON_EN_UPDATE(IDC_EDIT_NAME, OnUpdateEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameImageDlg message handlers

void CRenameImageDlg::OnOK() 
{
	CDialog::OnOK();
	
	CString strTemp ("");
	GetDlgItemText (IDC_EDIT_NAME, strTemp);
	int iLength = strTemp.GetLength();
	m_strName = "";

	// HL texture names can't have spaces
	for (int j = 0; j < iLength; j++)
	{
		if (strTemp.GetAt(j) != ' ')
		{
			m_strName += strTemp.GetAt(j);
		}
	}	
	
}

BOOL CRenameImageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText (IDC_EDIT_NAME, m_strName);
	m_edName.SetSel (0, -1, true);
	m_edName.SetFocus();

	if (m_strName.GetLength() == 0)
	{
		m_btnOK.EnableWindow (false);
	}

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRenameImageDlg::OnUpdateEditName() 
{
	CString strText ("");
	GetDlgItemText (IDC_EDIT_NAME, strText);

	m_btnOK.EnableWindow (strText.GetLength() > 0);
}
