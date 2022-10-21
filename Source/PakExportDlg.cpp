// PakExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PakExportDlg.h"
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
// CPakExportDlg dialog


CPakExportDlg::CPakExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPakExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPakExportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	HasSelections (FALSE);
}


void CPakExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPakExportDlg)
	DDX_Control(pDX, IDC_BUTTON_SOURCE, m_btnSource);
	DDX_Control(pDX, IDC_EDIT_WILDCARDS, m_edWildCards);
	DDX_Control(pDX, IDC_EDIT_SOURCE, m_edSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPakExportDlg, CDialog)
	//{{AFX_MSG_MAP(CPakExportDlg)
	ON_BN_CLICKED(IDC_BUTTON_DESTINATION, OnButtonDestination)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	ON_BN_CLICKED(IDC_RADIO_EXP_SELECTED, OnRadioExpSelected)
	ON_BN_CLICKED(IDC_RADIO_EXP_SPECIFIC, OnRadioExpSpecific)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakExportDlg message handlers

BOOL CPakExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bExportSelected = m_bHasSelections;

	EnableDisableControls();
	
	SetDlgItemText (IDC_EDIT_DESTINATION_DIR, g_strPakExportDestinationDirectory);	
	SetDlgItemText (IDC_EDIT_WILDCARDS, g_strPakExportWildCards);

	CheckDlgButton (IDC_RADIO_EXP_SELECTED, m_bHasSelections);	
	CheckDlgButton (IDC_RADIO_EXP_SPECIFIC, !m_bHasSelections);

	CheckDlgButton (IDC_CHECK_RECURSE, g_bPakExportRecurseSubDirectories);
	CheckDlgButton (IDC_CHECK_REPLACE, g_bPakExportReplaceExistingItems);
	CheckDlgButton (IDC_CHECK_RETAIN, g_bPakExportRetainStructure);

	CString strDirectory("");
	strDirectory = m_pPakDirectory->GetPathFromHere('/');

	SetDlgItemText (IDC_EDIT_SOURCE, strDirectory);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPakExportDlg::OnButtonDestination() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_DESTINATION_DIR, strDirectory);
	
	strDirectory = BrowseForFolder ("Select the destination directory", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_DESTINATION_DIR, strDirectory);
	}
	
}

void CPakExportDlg::OnButtonSource() 
{
	CSelectPakTreeDlg dlgPak;
	dlgPak.SetDocument (GetDocument());

	if (dlgPak.DoModal() == IDOK)
	{		
		if (dlgPak.GetPakDirectory())
		{
			CString strDirectory("");
			strDirectory = dlgPak.GetPakDirectory()->GetPathFromHere('/');

			SetDlgItemText (IDC_EDIT_SOURCE, strDirectory);
		}
	}	
}

void CPakExportDlg::EnableDisableControls()
{
	CWnd *pWnd = GetDlgItem (IDC_RADIO_EXP_SELECTED);

	if (pWnd)
	{
		pWnd->EnableWindow(m_bHasSelections);		
	}

	m_edWildCards.EnableWindow (!m_bExportSelected);
	m_edSource.EnableWindow (!m_bExportSelected);
	m_btnSource.EnableWindow (!m_bExportSelected);
}

void CPakExportDlg::OnRadioExpSelected() 
{
	if (m_bHasSelections)
	{
		m_bExportSelected = IsDlgButtonChecked (IDC_RADIO_EXP_SELECTED);
		EnableDisableControls();
	}	
}

void CPakExportDlg::OnRadioExpSpecific() 
{
	if (m_bHasSelections)
	{
		m_bExportSelected = IsDlgButtonChecked (IDC_RADIO_EXP_SELECTED);
		EnableDisableControls();
	}	
}

void CPakExportDlg::OnOK() 
{
	g_bPakExportRecurseSubDirectories = (IsDlgButtonChecked (IDC_CHECK_RECURSE) > 0);		
	g_bPakExportReplaceExistingItems = (IsDlgButtonChecked (IDC_CHECK_REPLACE) > 0);
	g_bPakExportRetainStructure = (IsDlgButtonChecked (IDC_CHECK_RETAIN) > 0);

	GetDlgItemText (IDC_EDIT_SOURCE, m_strPakDirectory);
	m_strPakDirectory = TrimLeadingCharacters (m_strPakDirectory, ' ');

	if (m_strPakDirectory != "")
	{
		m_strPakDirectory = ConvertAllCharacters (m_strPakDirectory, '/', '\\');
		m_strPakDirectory = TrimSlashes (m_strPakDirectory);
		m_strPakDirectory = TrimLeadingSlashes (m_strPakDirectory) + "\\";
	}

	GetDlgItemText (IDC_EDIT_DESTINATION_DIR, m_strDestinationDirectory);
	g_strPakExportDestinationDirectory = m_strDestinationDirectory;

	GetDlgItemText (IDC_EDIT_WILDCARDS, m_strWildCards);
	g_strPakExportWildCards = m_strWildCards;
		
	CDialog::OnOK();
}
