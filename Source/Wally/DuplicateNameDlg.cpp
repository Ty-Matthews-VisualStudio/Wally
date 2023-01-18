// DuplicateNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "DuplicateNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDuplicateNameDlg dialog


CDuplicateNameDlg::CDuplicateNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDuplicateNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDuplicateNameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetMaxLength(20);
}


void CDuplicateNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDuplicateNameDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edName);
	//}}AFX_DATA_MAP
	
	int iLength = GetMaxLength();
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, iLength);
}


BEGIN_MESSAGE_MAP(CDuplicateNameDlg, CDialog)
	//{{AFX_MSG_MAP(CDuplicateNameDlg)
	ON_BN_CLICKED(IDC_RADIO_REPLACE, OnRadioReplace)
	ON_BN_CLICKED(IDC_RADIO_CHANGE_NAME, OnRadioChangeName)
	ON_EN_UPDATE(IDC_EDIT_NAME, OnUpdateEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDuplicateNameDlg message handlers

BOOL CDuplicateNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CheckDlgButton( IDC_RADIO_CHANGE_NAME, false);
	CheckDlgButton( IDC_RADIO_REPLACE, true);
	
	SetDlgItemText (IDC_EDIT_NAME, m_strName);

	CString strText ("");
	strText.Format ("The name %s already exists in the WAD file you are trying to save to.  How do you wish to handle this?", m_strName);	
	SetDlgItemText (IDC_STATIC_MESSAGE, strText);
	m_edName.EnableWindow (false);
	m_bReplaceImage = true;
	MessageBeep( MB_ICONQUESTION);

	return TRUE;  // return TRUE unless you set the focus to a control
				   // EXCEPTION: OCX Property Pages should return FALSE
}

void CDuplicateNameDlg::OnOK() 
{
	GetDlgItemText (IDC_EDIT_NAME, m_strName);
	CDialog::OnOK();
}

void CDuplicateNameDlg::OnRadioReplace() 
{
	m_bReplaceImage = (IsDlgButtonChecked( IDC_RADIO_REPLACE));	
	
	m_edName.EnableWindow (!m_bReplaceImage);

	if (!m_bReplaceImage)
	{
		GetDlgItemText (IDC_EDIT_NAME, m_strName);
		if (m_strName.GetLength() == 0)
		{
			m_btnOK.EnableWindow (false);
		}
	}
}

void CDuplicateNameDlg::OnRadioChangeName() 
{
	OnRadioReplace();
	m_edName.SetSel (0, -1, true);
	m_edName.SetFocus();
}

void CDuplicateNameDlg::OnUpdateEditName() 
{	
	GetDlgItemText (IDC_EDIT_NAME, m_strName);
	m_btnOK.EnableWindow (m_strName.GetLength() > 0);
}
