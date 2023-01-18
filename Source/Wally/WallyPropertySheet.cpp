/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyPropertySheet.cpp : implementation file
//
// Created by Ty Matthews, 3-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "WallyPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertySheet

IMPLEMENT_DYNAMIC(CWallyPropertySheet, CPropertySheet)

CWallyPropertySheet::CWallyPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	m_psh.dwFlags = PSH_NOAPPLYNOW | PSH_PROPSHEETPAGE;	

	AddPage( &m_Page1);
	AddPage( &m_Page2);
	AddPage( &m_Page3);
	AddPage( &m_Page4);
	AddPage( &m_Page5);
	AddPage( &m_Page6);
	AddPage( &m_Page7);
}

CWallyPropertySheet::~CWallyPropertySheet()
{
	// update registry immediately (in case Wally dies later)

	CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();
	pWndMain->SaveWindowState();
}


BEGIN_MESSAGE_MAP(CWallyPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CWallyPropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertySheet message handlers



