// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Wally.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStatusBar  m_wndStatusBar;

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_wndStatusBar.SetPaneText (0,"",true);
	m_wndStatusBar.SetPaneText (1,"",true);
	m_wndStatusBar.SetPaneText (2,"",true);
	m_wndStatusBar.SetPaneText (3,"",true);
	m_wndStatusBar.SetPaneText (4,"",true);
	
	CMDIChildWnd::OnMouseMove(nFlags, point);
}

BOOL CChildFrame::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	
	
	//return CMDIFrameWnd::OnEraseBkgnd(pDC);
	
}
