// BatchSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "BatchSummaryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchSummaryDlg dialog


CBatchSummaryDlg::CBatchSummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchSummaryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchSummaryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBatchSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchSummaryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchSummaryDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchSummaryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchSummaryDlg message handlers

BOOL CBatchSummaryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText (IDC_EDIT_STATUS_TEXT, m_strStatusText);
	SetWindowText (m_strTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchSummaryDlg::SetStatusText (LPCTSTR szStatusText)
{
	m_strStatusText = szStatusText;
}