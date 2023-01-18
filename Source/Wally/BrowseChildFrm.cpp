// BrowseChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BrowseChildFrm.h"
#include "ExplorerTreeView.h"
#include "BrowseView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBrowseChildFrm

IMPLEMENT_DYNCREATE(CBrowseChildFrm, CMDIChildWnd)

CBrowseChildFrm::CBrowseChildFrm()
{
}

CBrowseChildFrm::~CBrowseChildFrm()
{
}


BEGIN_MESSAGE_MAP(CBrowseChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CBrowseChildFrm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseChildFrm message handlers

BOOL CBrowseChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// Create the splitter window with two columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");    
		return FALSE;
	}

	// Explorer view first
	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CExplorerTreeView), CSize(200, 0), 
		pContext))
	{
		TRACE0("Failed to create explorer tree view\n");    
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CBrowseView), CSize(200, 0), 
		pContext))
	{
		TRACE0("Failed to create browse view window\n");
		return FALSE;
	}


	// Set the active view  
	SetActiveView((CView*) m_wndSplitter.GetPane(0, 0));

	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);

	RestoreWindowState();

	return TRUE;
}

void CBrowseChildFrm::SaveWindowState()
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);

	GetWindowPlacement (&wp);
	CString RegistryKey ("Browser");

	// MainFrame window positions
	theApp.WriteProfileInt (RegistryKey, "Flags",   wp.flags);
	theApp.WriteProfileInt (RegistryKey, "ShowCmd", wp.showCmd);
	theApp.WriteProfileInt (RegistryKey, "Left",    wp.rcNormalPosition.left);
	theApp.WriteProfileInt (RegistryKey, "Top",     wp.rcNormalPosition.top);
	theApp.WriteProfileInt (RegistryKey, "Right",   wp.rcNormalPosition.right);
	theApp.WriteProfileInt (RegistryKey, "Bottom",  wp.rcNormalPosition.bottom);
}

BOOL CBrowseChildFrm::RestoreWindowState()
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);
	
	GetWindowPlacement (&wp);
	CString RegistryKey ("Browser");
	
	if (((wp.flags =
		    theApp.GetProfileInt (RegistryKey, "Flags", -1)) != -1)  &&
		((wp.showCmd =
		    theApp.GetProfileInt (RegistryKey, "ShowCmd", -1)) != -1) &&
		((wp.rcNormalPosition.left = 
			theApp.GetProfileInt (RegistryKey, "Left", -1)) != -1) &&
		((wp.rcNormalPosition.top =
			theApp.GetProfileInt (RegistryKey, "Top", -1)) != -1) &&
		((wp.rcNormalPosition.right =
			theApp.GetProfileInt (RegistryKey, "Right", -1)) != -1) &&
		((wp.rcNormalPosition.bottom =
			theApp.GetProfileInt (RegistryKey, "Bottom", -1)) != -1))
	{
		wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, 
			::GetSystemMetrics (SM_CXSCREEN) - 
			::GetSystemMetrics (SM_CXICON));
	
		wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, 
			::GetSystemMetrics (SM_CYSCREEN) - 
			::GetSystemMetrics (SM_CYICON));
	

		WINDOWPLACEMENT ParentWP;
		ParentWP.length = sizeof (WINDOWPLACEMENT);
		
		if (theApp.GetBackgroundWindowPlacement (&ParentWP))
		{
			// Make sure we're not larger than the mainframe			
			int ParentWidth = (ParentWP.rcNormalPosition.right - ParentWP.rcNormalPosition.left) - 4;
			int ParentHeight = (ParentWP.rcNormalPosition.bottom - ParentWP.rcNormalPosition.top) - 4;
			
			wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, max ((ParentWidth - 350), 0));
			wp.rcNormalPosition.right = min (wp.rcNormalPosition.right, ParentWidth);
			wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, max ((ParentHeight - 300), 0));
			wp.rcNormalPosition.bottom = min (wp.rcNormalPosition.bottom, ParentHeight);
		}
		
		SetWindowPlacement (&wp);
		return TRUE;
	}
	return FALSE;
}

BOOL CBrowseChildFrm::DestroyWindow() 
{
	SaveWindowState();	
	return CMDIChildWnd::DestroyWindow();
}
