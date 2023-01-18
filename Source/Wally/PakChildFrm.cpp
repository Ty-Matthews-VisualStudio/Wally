// PakChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PakChildFrm.h"
#include "PakTreeView.h"
#include "PakListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CPakChildFrm

IMPLEMENT_DYNCREATE(CPakChildFrm, CMDIChildWnd)

CPakChildFrm::CPakChildFrm()
{
}

CPakChildFrm::~CPakChildFrm()
{
}


BEGIN_MESSAGE_MAP(CPakChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPakChildFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakChildFrm message handlers

BOOL CPakChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");    
		return FALSE;
	}

	// Explorer view first
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CPakTreeView), CSize(200, 0), 
		pContext))
	{
		TRACE0("Failed to create explorer tree view\n");    
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CPakListView), CSize(400, 0), 
		pContext))
	{
		TRACE0("Failed to create browse view window\n");
		return FALSE;
	}


	// Set the active view  
	SetActiveView((CView*) m_wndSplitter.GetPane(0, 0));
	
/*	WINDOWPLACEMENT ParentWP;
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
		int iChildWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		int iChildHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;

		wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, max ((ParentWidth - 350), 0));		
		wp.rcNormalPosition.right = ParentWidth - 150;
		
		wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, max ((ParentHeight - 300), 0));
		wp.rcNormalPosition.bottom = ParentHeight - 150;

		SetWindowPlacement (&wp);
	}
	*/

	return TRUE;	
}
