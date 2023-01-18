// PercentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PercentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPercentDlg dialog


CPercentDlg::CPercentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPercentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPercentDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPercentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPercentDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPercentDlg, CDialog)
	//{{AFX_MSG_MAP(CPercentDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_PERCENT, OnKillfocusEditPercent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPercentDlg message handlers

void CPercentDlg::OnOK() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_PERCENT, strText);

	m_iPercent = atol (strText);
	m_iPercent = min (m_iPercent, 200);
	m_iPercent = max (m_iPercent, 1);
	
	CDialog::OnOK();
}

void CPercentDlg::OnKillfocusEditPercent() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_PERCENT, strText);

	int iPercent = atol (strText);
	iPercent = min (iPercent, 200);
	iPercent = max (iPercent, 1);

	strText.Format ("%d", iPercent);
	SetDlgItemText (IDC_EDIT_PERCENT, strText);
}
