// Package2ScrollView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "Package2ScrollView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackage2ScrollView

IMPLEMENT_DYNCREATE(CPackage2ScrollView, CScrollView)

CPackage2ScrollView::CPackage2ScrollView()
{
}

CPackage2ScrollView::~CPackage2ScrollView()
{
}


BEGIN_MESSAGE_MAP(CPackage2ScrollView, CScrollView)
	//{{AFX_MSG_MAP(CPackage2ScrollView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackage2ScrollView drawing

void CPackage2ScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CPackage2ScrollView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPackage2ScrollView diagnostics

#ifdef _DEBUG
void CPackage2ScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPackage2ScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackage2ScrollView message handlers
