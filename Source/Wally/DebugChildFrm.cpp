// DebugChildFrm.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "DebugChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugChildFrm

IMPLEMENT_DYNCREATE(CDebugChildFrm, CMDIChildWnd)

CDebugChildFrm::CDebugChildFrm()
{
}

CDebugChildFrm::~CDebugChildFrm()
{
}


BEGIN_MESSAGE_MAP(CDebugChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDebugChildFrm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugChildFrm message handlers
