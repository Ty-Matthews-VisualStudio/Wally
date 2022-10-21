// NewRuleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "NewRuleDlg.h"
#include "MiscFunctions.h"
#include "ImageHelper.h"
#include "TextureInfo.h"
#include "SinFlagsDlg.h"
#include "RuleList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewRuleDlg dialog


CNewRuleDlg::CNewRuleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewRuleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewRuleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetRuleItem (NULL);
}


void CNewRuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewRuleDlg)
	DDX_Control(pDX, IDC_BUTTON_FLAGS, m_btnFlags);
	DDX_Control(pDX, IDC_COMBO_GAMETYPE, m_cbGameType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewRuleDlg, CDialog)
	//{{AFX_MSG_MAP(CNewRuleDlg)
	ON_BN_CLICKED(IDC_BUTTON_FLAGS, OnButtonFlags)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT_DIR, OnButtonOutputDir)
	ON_CBN_SELCHANGE(IDC_COMBO_GAMETYPE, OnSelchangeComboGametype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewRuleDlg message handlers

void CNewRuleDlg::SetRuleItem (CRuleItem *pItem)
{
	m_pRuleItem = pItem;
}

CRuleItem *CNewRuleDlg::GetRuleItem ()
{
	return m_pRuleItem;
}

BOOL CNewRuleDlg::OnInitDialog() 
{
	ASSERT (GetRuleItem());

	CDialog::OnInitDialog();
	
	// TODO:  When adding new games, change this to read CImageHelper, etc	
	CImageHelper ihHelper;

	CWildCardItem *pItem = ihHelper.GetFirstSupportedImage();
	CString strAddString("");
	int iItemAdded = 0;

	BOOL bAtLeastOne = FALSE;

	while (pItem)
	{
		if (ihHelper.IsGameType (pItem->GetImageType()))
		{
			bAtLeastOne = TRUE;
			strAddString = pItem->GetDescription();
			iItemAdded = m_cbGameType.AddString (strAddString);
			m_cbGameType.SetItemData (iItemAdded, pItem->GetImageType());						
		}
		pItem = ihHelper.GetNextSupportedImage();
	}

	if (bAtLeastOne)
	{
		m_cbGameType.SetCurSel (0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewRuleDlg::OnButtonFlags() 
{
	int iSelection = m_cbGameType.GetCurSel();

	if (iSelection != CB_ERR)
	{
		int iImageType = (int)(m_cbGameType.GetItemData(iSelection));
		
		switch (iImageType)
		{
		case IH_WAL_TYPE:
			{
				CTextureInfo TexInfoDlg;
				//TexInfoDlg.Init(&m_Q2Header);
				TexInfoDlg.DoModal();
			}
			break;

		case IH_SWL_TYPE:
			{
				CSinFlagsDlg dlgSin;
				dlgSin.SetSinHeader (GetRuleItem()->GetSinHeader());
				
				if (dlgSin.DoModal() == IDOK)
				{
					GetRuleItem()->SetSinHeader (dlgSin.GetSinHeader());					
				}
			}
			break;

		default:
			ASSERT (FALSE);
			break;
		}
	}
}

void CNewRuleDlg::OnButtonOutputDir() 
{
	CString strDirectory("");
	
	GetDlgItemText (IDC_EDIT_OUTPUT_DIR, strDirectory);
	strDirectory = TrimSlashes (strDirectory);

	strDirectory = BrowseForFolder ("Select the destination directory:", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_OUTPUT_DIR, strDirectory);
	}	
}

void CNewRuleDlg::OnSelchangeComboGametype() 
{
	int iSelection = m_cbGameType.GetCurSel();

	if (iSelection != CB_ERR)
	{
		int iImageType = (int)(m_cbGameType.GetItemData(iSelection));

		switch (iImageType)
		{
		case IH_MIP_TYPE:
			m_btnFlags.EnableWindow (FALSE);
			break;

		case IH_WAL_TYPE:
		case IH_SWL_TYPE:
			m_btnFlags.EnableWindow (TRUE);
			break;

		default:
			ASSERT (FALSE);
			break;
		}
	}
}
