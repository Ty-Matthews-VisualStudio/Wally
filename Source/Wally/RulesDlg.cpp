// RulesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "RulesDlg.h"
#include "NewRuleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRulesDlg dialog


CRulesDlg::CRulesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRulesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRulesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRulesDlg)
	DDX_Control(pDX, IDC_LIST_RULES, m_lbRules);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRulesDlg, CDialog)
	//{{AFX_MSG_MAP(CRulesDlg)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulesDlg message handlers

BOOL CRulesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int iaTabStops[2];
	iaTabStops[0] = 14;
	iaTabStops[1] = 55;
	m_lbRules.SetTabStops (2, iaTabStops);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRulesDlg::OnButtonNew() 
{
	CNewRuleDlg dlgNew;
	CRuleItem *pItem = new CRuleItem;

	dlgNew.SetRuleItem (pItem);

	if (dlgNew.DoModal() == IDOK)
	{
		//m_RuleList.
	}

	if (pItem)
	{
		delete pItem;
	}
}
