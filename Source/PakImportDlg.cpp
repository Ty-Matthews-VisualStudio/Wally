// PakImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PakImportDlg.h"
#include "PakList.h"
#include "PakDoc.h"
#include "SelectPakTreeDlg.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPakImportDlg dialog


CPakImportDlg::CPakImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPakImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPakImportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetDocument (NULL);
	SetPakDirectory (NULL);
}


void CPakImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPakImportDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPakImportDlg, CDialog)
	//{{AFX_MSG_MAP(CPakImportDlg)
	ON_BN_CLICKED(IDC_BUTTON_DESTINATION, OnButtonDestination)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakImportDlg message handlers

BOOL CPakImportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT (GetDocument());

	SetDlgItemText (IDC_EDIT_SOURCE_DIR, g_strPakImportSourceDirectory);
	SetDlgItemText (IDC_EDIT_OFFSET, g_strPakImportOffsetDirectory);
	SetDlgItemText (IDC_EDIT_WILDCARDS, g_strPakImportWildCards);

	CheckDlgButton (IDC_CHECK_RECURSE, g_bPakImportRecurseSubDirectories);
	CheckDlgButton (IDC_CHECK_REPLACE, g_bPakImportReplaceExistingItems);
	CheckDlgButton (IDC_CHECK_RETAIN, g_bPakImportRetainStructure);

	if (m_pPakDirectory)
	{
		CString strDirectory("");
		strDirectory = m_pPakDirectory->GetPathFromHere('\\');

		SetDlgItemText (IDC_EDIT_DESTINATION_DIR, strDirectory);		
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPakImportDlg::OnButtonDestination() 
{
	CSelectPakTreeDlg dlgPak;
	dlgPak.SetDocument (GetDocument());

	if (dlgPak.DoModal() == IDOK)
	{
		SetPakDirectory (dlgPak.GetPakDirectory());

		if (m_pPakDirectory)
		{
			CString strDirectory("");
			strDirectory = m_pPakDirectory->GetPathFromHere('/');

			SetDlgItemText (IDC_EDIT_DESTINATION_DIR, strDirectory);
		}
	}	
}

void CPakImportDlg::OnButtonSource() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);
	
	strDirectory = BrowseForFolder ("Select the source directory", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);

		CString strOffset("");
		GetDlgItemText (IDC_EDIT_OFFSET, strOffset);

		if (strOffset == "")
		{
			SetDlgItemText (IDC_EDIT_OFFSET, strDirectory);
		}
	}
}

void CPakImportDlg::OnOK() 
{	
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, m_strSourceDirectory);

	if (m_strSourceDirectory == "")
	{
		AfxMessageBox ("Please select a source directory.", MB_ICONWARNING);
		return;
	}

	m_strSourceDirectory = TrimSlashes (m_strSourceDirectory) + "\\";

	GetDlgItemText (IDC_EDIT_OFFSET, m_strOffsetDirectory);
	m_strOffsetDirectory = TrimSlashes (m_strOffsetDirectory) + "\\";	
	
	CString strDifference("");
	strDifference = TrimFromLeft (m_strSourceDirectory, m_strOffsetDirectory);

	if (strDifference.GetLength() != (m_strSourceDirectory.GetLength() - m_strOffsetDirectory.GetLength()))
	{
		AfxMessageBox ("The offset directory is not a valid parent of the source directory.", MB_ICONWARNING);
		return;
	}

	g_strPakImportSourceDirectory = m_strSourceDirectory;
	g_strPakImportOffsetDirectory = m_strOffsetDirectory;
	
	GetDlgItemText (IDC_EDIT_WILDCARDS, m_strWildCards);
	g_strPakImportWildCards = m_strWildCards;

	GetDlgItemText (IDC_EDIT_DESTINATION_DIR, m_strPakDirectory);

	m_strPakDirectory = TrimLeadingCharacters (m_strPakDirectory, ' ');

	if (m_strPakDirectory != "")
	{
		m_strPakDirectory = ConvertAllCharacters (m_strPakDirectory, '/', '\\');
		m_strPakDirectory = TrimSlashes (m_strPakDirectory);
		m_strPakDirectory = TrimLeadingSlashes (m_strPakDirectory) + "\\";
	}

	g_bPakImportRecurseSubDirectories = (IsDlgButtonChecked (IDC_CHECK_RECURSE) > 0);		
	g_bPakImportReplaceExistingItems = (IsDlgButtonChecked (IDC_CHECK_REPLACE) > 0);
	g_bPakImportRetainStructure = (IsDlgButtonChecked (IDC_CHECK_RETAIN) > 0);

	CDialog::OnOK();
}

