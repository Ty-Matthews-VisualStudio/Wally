// QuakeUnrealDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "QuakeUnrealDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvertQuakeUnrealDlg dialog


CConvertQuakeUnrealDlg::CConvertQuakeUnrealDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertQuakeUnrealDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConvertQuakeUnrealDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConvertQuakeUnrealDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvertQuakeUnrealDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvertQuakeUnrealDlg, CDialog)
	//{{AFX_MSG_MAP(CConvertQuakeUnrealDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvertQuakeUnrealDlg message handlers
