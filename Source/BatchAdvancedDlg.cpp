// BatchAdvancedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BatchAdvancedDlg.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchAdvancedDlg dialog


CBatchAdvancedDlg::CBatchAdvancedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchAdvancedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchAdvancedDlg)
	//}}AFX_DATA_INIT
}


void CBatchAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchAdvancedDlg)
	DDX_Control(pDX, IDC_BUTTON_DEST, m_btnDest);
	DDX_Control(pDX, IDC_BUTTON_SOURCE, m_btnSource);
	DDX_Control(pDX, IDC_EDIT_DEST_PALETTE, m_edDestPalette);
	DDX_Control(pDX, IDC_EDIT_SOURCE_PALETTE, m_edSourcePalette);
	DDX_Control(pDX, IDC_COMBO_IMAGE_TYPE, m_cbImages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchAdvancedDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchAdvancedDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_TYPE, OnSelchangeComboImageType)
	ON_BN_CLICKED(IDC_RADIO_DEST_CURRENT, OnRadioDestCurrent)
	ON_BN_CLICKED(IDC_RADIO_DEST_CUSTOM, OnRadioDestCustom)
	ON_BN_CLICKED(IDC_RADIO_SOURCE_CURRENT, OnRadioSourceCurrent)
	ON_BN_CLICKED(IDC_RADIO_SOURCE_CUSTOM, OnRadioSourceCustom)
	ON_BN_CLICKED(IDC_BUTTON_DEST, OnButtonDest)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchAdvancedDlg message handlers

BOOL CBatchAdvancedDlg::OnInitDialog() 
{	
	CDialog::OnInitDialog();
	int iItemAdded = 0;
	bool bAtLeastOne = false;	

	CWildCardItem *pItem = m_ihHelper.GetFirstSupportedImage();

	while (pItem)
	{
		iItemAdded = m_cbImages.AddString (pItem->GetDescription());
		m_cbImages.SetItemData (iItemAdded, (DWORD)pItem);
		pItem = m_ihHelper.GetNextSupportedImage();
		bAtLeastOne = true;
	}

	if (bAtLeastOne)
	{
		m_cbImages.SetCurSel(0);		
	}

	g_WildCardList.PrepareForChanges();

	OnSelchangeComboImageType();	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchAdvancedDlg::OnSelchangeComboImageType() 
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));

		CWnd *pWnd = NULL;

		pWnd = GetDlgItem (IDC_RADIO_SOURCE_CURRENT);
		if (pWnd)
		{
			pWnd->EnableWindow (m_ihHelper.IsGameType (pItem->GetImageType()));
		}

		pWnd = GetDlgItem (IDC_RADIO_SOURCE_CUSTOM);
		if (pWnd)
		{
			pWnd->EnableWindow (m_ihHelper.IsGameType (pItem->GetImageType()));
		}
		
		bool bUseCurrentSource = pItem->UseCurrentSourcePalette();

		CheckDlgButton (IDC_RADIO_SOURCE_CURRENT, bUseCurrentSource);
		CheckDlgButton (IDC_RADIO_SOURCE_CUSTOM, !bUseCurrentSource);

		bool bUseCurrentDest = pItem->UseCurrentDestPalette();

		CheckDlgButton (IDC_RADIO_DEST_CURRENT, bUseCurrentDest);
		CheckDlgButton (IDC_RADIO_DEST_CUSTOM, !bUseCurrentDest);

		SetDlgItemText ( IDC_EDIT_DEST_PALETTE, pItem->GetDestPaletteFile());
		SetDlgItemText ( IDC_EDIT_SOURCE_PALETTE, pItem->GetSourcePaletteFile());
	}
	EnableDisableControls();
}

void CBatchAdvancedDlg::EnableDisableControls()
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
		
		bool bIsGameType = m_ihHelper.IsGameType (pItem->GetImageType());
		
		bool bUseCurrentSource = IsDlgButtonChecked (IDC_RADIO_SOURCE_CURRENT);
		pItem->m_bTempUseCurrentSourcePalette = bUseCurrentSource;

		m_edSourcePalette.EnableWindow (bIsGameType && !bUseCurrentSource);
		m_btnSource.EnableWindow (bIsGameType && !bUseCurrentSource);

		bool bUseCurrentDest = IsDlgButtonChecked (IDC_RADIO_DEST_CURRENT);
		pItem->m_bTempUseCurrentDestPalette = bUseCurrentDest;

		m_edDestPalette.EnableWindow (!bUseCurrentDest);
		m_btnDest.EnableWindow (!bUseCurrentDest);
	}
	
}

void CBatchAdvancedDlg::OnRadioDestCurrent() 
{
	EnableDisableControls();
}

void CBatchAdvancedDlg::OnRadioDestCustom() 
{
	EnableDisableControls();
}

void CBatchAdvancedDlg::OnRadioSourceCurrent() 
{
	EnableDisableControls();
}

void CBatchAdvancedDlg::OnRadioSourceCustom() 
{
	EnableDisableControls();
}

void CBatchAdvancedDlg::OnButtonDest() 
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		CString strPaletteFile = SelectPalette ();
		if (strPaletteFile != "")
		{
			SetDlgItemText (IDC_EDIT_DEST_PALETTE, strPaletteFile);
		
			CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
			pItem->SetDestPaletteFile (strPaletteFile);
		}
	}

}

void CBatchAdvancedDlg::OnButtonSource() 
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{		
		CString strPaletteFile = SelectPalette ();
		if (strPaletteFile != "")
		{
			SetDlgItemText (IDC_EDIT_SOURCE_PALETTE, strPaletteFile);

			CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
			pItem->SetSourcePaletteFile (strPaletteFile);
		}
	}
}


CString CBatchAdvancedDlg::SelectPalette ()
{
	m_ihHelper.ResetContent();

	CString strWildCard ("All Files (*.*)|*.*|");	
	CString strAddString = m_ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath ("");
	
	if (dlgPalette.DoModal() == IDOK)
	{
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		g_iOpenPaletteExtension = dlgPalette.m_ofn.nFilterIndex;
		strSourceFilePath         = dlgPalette.GetPathName();		

		m_ihHelper.LoadImage (strSourceFilePath);

		if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (m_ihHelper.GetErrorText());
			return "";
		}

		if (!m_ihHelper.IsValidPalette())
		{
			AfxMessageBox ("Unsupported palette file", MB_ICONSTOP);
			return "";
		}		
	}
	
	return strSourceFilePath;
}

void CBatchAdvancedDlg::OnOK() 
{
	g_WildCardList.SolidifyAllChanges();
	if (IsDlgButtonChecked (IDC_CHECK_RETAIN))
	{		
		g_WildCardList.WriteRegistry();
	}
	CDialog::OnOK();
}

void CBatchAdvancedDlg::OnCancel() 
{
	g_WildCardList.CancelAllChanges();
	CDialog::OnCancel();
}
