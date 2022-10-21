// WLYDoc.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "WLYDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWLYDoc

IMPLEMENT_DYNCREATE(CWLYDoc, CDocument)

CWLYDoc::CWLYDoc()
{
}

BOOL CWLYDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWLYDoc::~CWLYDoc()
{
}


BEGIN_MESSAGE_MAP(CWLYDoc, CDocument)
	//{{AFX_MSG_MAP(CWLYDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWLYDoc diagnostics

#ifdef _DEBUG
void CWLYDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWLYDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWLYDoc serialization

void CWLYDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWLYDoc commands
