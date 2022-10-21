// Package2ChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "Package2ChildFrm.h"
#include "PackageBrowseView.h"
#include "Package2FormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackage2ChildFrm

IMPLEMENT_DYNCREATE(CPackage2ChildFrm, CMDIChildWnd)

CPackage2ChildFrm::CPackage2ChildFrm()
{
}

CPackage2ChildFrm::~CPackage2ChildFrm()
{
}


BEGIN_MESSAGE_MAP(CPackage2ChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPackage2ChildFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackage2ChildFrm message handlers

BOOL CPackage2ChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");    
		return FALSE;
	}

	// Form view first
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CPackage2FormView), CSize(200, 0),
		pContext))
	{
		TRACE0("Failed to create explorer tree view\n");    
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CPackageBrowseView), CSize(200, 0), 
		pContext))
	{
		TRACE0("Failed to create browse view window\n");
		return FALSE;
	}


	// Set the active view  
	SetActiveView((CView*) m_wndSplitter.GetPane(0, 0));

	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);

	//RestoreWindowState();

	return TRUE;
}

