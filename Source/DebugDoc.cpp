// DebugDoc.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "DebugDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugDoc

IMPLEMENT_DYNCREATE(CDebugDoc, CDocument)

CDebugDoc::CDebugDoc()
{
}

BOOL CDebugDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDebugDoc::~CDebugDoc()
{
}


BEGIN_MESSAGE_MAP(CDebugDoc, CDocument)
	//{{AFX_MSG_MAP(CDebugDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugDoc diagnostics

#ifdef _DEBUG
void CDebugDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDebugDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDebugDoc serialization

void CDebugDoc::Serialize(CArchive& ar)
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
// CDebugDoc commands
