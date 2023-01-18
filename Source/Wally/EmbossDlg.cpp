// EmbossDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "EmbossDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmbossDlg dialog


CEmbossDlg::CEmbossDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEmbossDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmbossDlg)
	m_iEmbossAmount = -1;
	m_bMonochrome = FALSE;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CEmbossDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmbossDlg)
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iEmbossAmount);
	DDX_Check(pDX, IDC_CHECK_MONOCHROME, m_bMonochrome);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEmbossDlg, CDialog)
	//{{AFX_MSG_MAP(CEmbossDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmbossDlg message handlers

void CEmbossDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);
	
	g_Filter.DoEmboss( m_pLayer, iAmount,
			IsDlgButtonChecked( IDC_CHECK_MONOCHROME));

	m_bPreviewApplied = TRUE;
}
