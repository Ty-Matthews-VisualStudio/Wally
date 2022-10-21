// PakListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PakListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPakListCtrl

CPakListCtrl::CPakListCtrl()
{
}

CPakListCtrl::~CPakListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPakListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CPakListCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakListCtrl message handlers

BOOL CPakListCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT;
	return CListCtrl::PreCreateWindow(cs);
}
