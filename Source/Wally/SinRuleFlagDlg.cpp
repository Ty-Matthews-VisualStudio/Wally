// SinRuleFlagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "SinRuleFlagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSinRuleFlagDlg dialog


CSinRuleFlagDlg::CSinRuleFlagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSinRuleFlagDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSinRuleFlagDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSinRuleFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSinRuleFlagDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSinRuleFlagDlg, CDialog)
	//{{AFX_MSG_MAP(CSinRuleFlagDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSinRuleFlagDlg message handlers

void CSinRuleFlagDlg::Update()
{
	m_wndColor.SetColor (m_SinHeader.color[0], m_SinHeader.color[1], m_SinHeader.color[2]);
	
	// Invalidate the IDC_PALETTE button so that it hides itself
	CWnd* pWnd = GetDlgItem( IDC_BUTTON_COLOR);
	if (pWnd)
	{		
		pWnd->InvalidateRect( NULL, FALSE);
	}
	else
	{
		ASSERT( FALSE);
	}

	// Invalidate our custom CPaletteWnd class	
	m_wndColor.Update();
}

void CSinRuleFlagDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}


BOOL CSinRuleFlagDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

