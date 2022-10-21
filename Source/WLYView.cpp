// WLYView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "WLYView.h"
#include "DibSection.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWLYView

IMPLEMENT_DYNCREATE(CWLYView, CScrollView)

CWLYView::CWLYView()
{
}	

CWLYView::~CWLYView()
{
}


BEGIN_MESSAGE_MAP(CWLYView, CScrollView)
	//{{AFX_MSG_MAP(CWLYView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWLYView drawing

void CWLYView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();	
}

/////////////////////////////////////////////////////////////////////////////
// CWLYView diagnostics

#ifdef _DEBUG
void CWLYView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWLYView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWLYView message handlers


void CWLYView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	SetScrollSizes( MM_TEXT, CSize(100, 100) );	
}
