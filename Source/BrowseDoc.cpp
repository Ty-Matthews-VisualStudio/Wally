// BrowseDoc.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BrowseDoc.h"
#include "BrowseWorkerThread.h"
#include "BrowserCacheList.h"
#include "BrowseView.h"
#include "MiscFunctions.h"
#include "ExplorerTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBrowseDoc

IMPLEMENT_DYNCREATE(CBrowseDoc, CDocument)

CBrowseDoc::CBrowseDoc()
{
	m_strPath = "";
	m_pWorkerThread = NULL;
	m_pWorkerThread = (CBrowseWorkerThread *) AfxBeginThread ( (AFX_THREADPROC) CBrowseWorkerThread::MainLoop, (LPVOID)this);
}

BOOL CBrowseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}
	SetModifiedFlag (FALSE);

	SetTitle ("Browse");
	return TRUE;
}

CBrowseDoc::~CBrowseDoc()
{
	theApp.m_bBrowseOpen = FALSE;

	if (m_pWorkerThread)
	{
		SetThreadMessage (BROWSE_DOC_MESSAGE_STOP);

		while (GetThreadStatus() != BROWSE_DOC_THREAD_STOPPED)
		{
			Sleep(200);
		}

		// Sleep for a second so the thread can actually terminate
		Sleep (1000);

		m_pWorkerThread = NULL;
	}
}


BEGIN_MESSAGE_MAP(CBrowseDoc, CDocument)
	//{{AFX_MSG_MAP(CBrowseDoc)
	ON_COMMAND(ID_FILE_REFRESH, OnFileRefresh)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseDoc diagnostics

#ifdef _DEBUG
void CBrowseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBrowseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowseDoc serialization

void CBrowseDoc::Serialize(CArchive& ar)
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
// CBrowseDoc commands

void CBrowseDoc::SetPath (LPCTSTR szPath)
{
	m_strPath = TrimSlashes (szPath);
	g_strBrowseDirectory = m_strPath;
	
	CString strTitle("");

	strTitle.Format ("Browsing %s", m_strPath);
	SetTitle (strTitle);
	
	SetThreadMessage (BROWSE_DOC_MESSAGE_GETFILES);
}

void CBrowseDoc::UpdateExplorer(LPCTSTR szPath)
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);
		
		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CExplorerTreeView)))
			{
				CExplorerTreeView *pExplorerView = (CExplorerTreeView *)pView;
				pExplorerView->HighlightDirectory( szPath);
			}
		}
	}
}

void CBrowseDoc::ResetScrollbars(BOOL bReset /* = TRUE */)
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);
		
		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CBrowseView)))
			{
				CBrowseView *pBrowseView = (CBrowseView *)pView;
				pBrowseView->ResetScrollbars (bReset);
				pBrowseView->InvalidateRect (NULL, FALSE);
			}
		}
	}
}

void CBrowseDoc::RefreshView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);

		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CBrowseView)))
			{			
				pView->InvalidateRect( NULL, FALSE);
			}
		}
	}	
}

CString CBrowseDoc::GetPath()
{
	return m_strPath;
}

CStringArray *CBrowseDoc::GetArray()
{
	return &m_strArray;
}

int CBrowseDoc::GetThreadStatus()
{
	return m_iThreadStatus;
}

void CBrowseDoc::SetThreadStatus (int iStatus)
{
	m_iThreadStatus = iStatus;
}

int CBrowseDoc::GetThreadMessage()
{
	return m_iThreadMessage;
}

void CBrowseDoc::SetThreadMessage (int iMessage)
{
	m_iThreadMessage = iMessage;
}


void CBrowseDoc::OnFileRefresh() 
{	
	SetThreadMessage (BROWSE_DOC_MESSAGE_REFRESH);	
}

void CBrowseDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (FALSE);	
}
