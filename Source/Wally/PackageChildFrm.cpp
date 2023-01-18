// PackageChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PackageChildFrm.h"
#include "PackageView.h"
#include "PackageBrowseView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPackageChildFrm

IMPLEMENT_DYNCREATE(CPackageChildFrm, CMDIChildWnd)

CPackageChildFrm::CPackageChildFrm()
{
}

CPackageChildFrm::~CPackageChildFrm()
{
}


BEGIN_MESSAGE_MAP(CPackageChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPackageChildFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageChildFrm message handlers

BOOL CPackageChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{	
	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");    
		return FALSE;
	}

	// Form view first
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CPackageView), CSize(150, 0),
		pContext))
	{
		TRACE0("Failed to create CPackageView\n");    
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CPackageBrowseView), CSize(250, 0), 
		pContext))
	{
		TRACE0("Failed to create browse view window\n");
		return FALSE;
	}

	// Set the active view  
	SetActiveView((CView*) m_wndSplitter.GetPane(0, 0));

	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);

	/*
	WINDOWPLACEMENT ParentWP;
	ParentWP.length = sizeof (WINDOWPLACEMENT);

	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);
	
	GetWindowPlacement (&wp);
		
	if (theApp.GetBackgroundWindowPlacement (&ParentWP))
	{
		wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, 
			::GetSystemMetrics (SM_CXSCREEN) - 
			::GetSystemMetrics (SM_CXICON));
	
		wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, 
			::GetSystemMetrics (SM_CYSCREEN) - 
			::GetSystemMetrics (SM_CYICON));
	
		// Make sure we're not larger than the mainframe 			
		int ParentWidth = (ParentWP.rcNormalPosition.right - ParentWP.rcNormalPosition.left) - 4;
		int ParentHeight = (ParentWP.rcNormalPosition.bottom - ParentWP.rcNormalPosition.top) - 4;
		
		wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, max ((ParentWidth - 350), 0));
		wp.rcNormalPosition.right = max (wp.rcNormalPosition.right, ParentWidth);
		wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, max ((ParentHeight - 300), 0));
		wp.rcNormalPosition.bottom = max (wp.rcNormalPosition.bottom, ParentHeight);

		SetWindowPlacement (&wp);
	}
	*/

	return TRUE;
}
