// PackageExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PackageExportDlg.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackageExportDlg dialog


CPackageExportDlg::CPackageExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackageExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPackageExportDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CPackageExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackageExportDlg)
	DDX_Control(pDX, IDC_COMBO_OUTPUT_FORMAT, m_cbOutputFormat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPackageExportDlg, CDialog)
	//{{AFX_MSG_MAP(CPackageExportDlg)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT_DIR, OnButtonOutputDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageExportDlg message handlers

BOOL CPackageExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	bool bAtLeastOne = false;

	CWildCardItem *pItem = m_ihHelper.GetFirstSupportedImage();
	if (pItem)
	{
		bAtLeastOne = true;
	}
	
	CheckDlgButton (IDC_CHECK_OVERWRITE, g_bOverWriteFiles);	
	CheckDlgButton (IDC_CHECK_SUMMARY, g_bDisplayExportSummary);
	
	CString strAddString;
	int iItemAdded = 0;

	while (pItem)
	{
		strAddString = pItem->GetDescription();	
		iItemAdded = m_cbOutputFormat.AddString (strAddString);
		m_cbOutputFormat.SetItemData (iItemAdded, (DWORD)pItem);
		pItem = m_ihHelper.GetNextSupportedImage();
	}

	SetDlgItemText (IDC_EDIT_OUTPUT_DIR, m_strDirectory);

	if (bAtLeastOne)
	{
		if (m_cbOutputFormat.GetCount() > g_iFileExportExtension)
		{
			m_cbOutputFormat.SetCurSel(g_iFileExportExtension);
		}
		else
		{
			m_cbOutputFormat.SetCurSel(0);
		}
	}

	m_strExtension = "";
	m_strDirectory = "";

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPackageExportDlg::OnOK() 
{	
	GetDlgItemText (IDC_EDIT_OUTPUT_DIR, m_strDirectory);

	int iSelection = m_cbOutputFormat.GetCurSel();

	if (iSelection != CB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)(m_cbOutputFormat.GetItemData(iSelection));
		m_strExtension = pItem->GetWildCardExtension();
		g_iFileExportExtension = iSelection;
	}

	g_bOverWriteFiles = (IsDlgButtonChecked(IDC_CHECK_OVERWRITE) > 0);
	g_bDisplayExportSummary = (IsDlgButtonChecked( IDC_CHECK_SUMMARY) > 0);

	CDialog::OnOK();
}

void CPackageExportDlg::OnButtonOutputDir() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_OUTPUT_DIR, strDirectory);

	strDirectory = TrimSlashes (strDirectory);
	
	strDirectory = BrowseForFolder ("Please select the destination directory:", strDirectory);
	
	if (strDirectory != "")
	{
		m_strDirectory = strDirectory;
		SetDlgItemText (IDC_EDIT_OUTPUT_DIR, strDirectory);
	}	
}

