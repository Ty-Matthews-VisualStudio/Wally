// BrowseWorkerThread.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BrowseWorkerThread.h"
#include "MiscFunctions.h"
#include "BrowseDoc.h"
#include "BrowserCacheList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowseWorkerThread

IMPLEMENT_DYNCREATE(CBrowseWorkerThread, CWinThread)

CBrowseWorkerThread::CBrowseWorkerThread()
{
}

CBrowseWorkerThread::~CBrowseWorkerThread()
{
}

BOOL CBrowseWorkerThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CBrowseWorkerThread::ExitInstance()
{	
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CBrowseWorkerThread, CWinThread)
	//{{AFX_MSG_MAP(CBrowseWorkerThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseWorkerThread message handlers

void CBrowseWorkerThread::MainLoop (LPVOID pParam)
{
	CBrowseDoc *pDoc = (CBrowseDoc *)pParam;
	ASSERT (pDoc);

	//CString strPath ("");

	CBrowserCacheList *pCacheList = pDoc->GetCacheList();

	BOOL bDone = FALSE;
	int iMessage = 0;

	pDoc->SetThreadStatus (BROWSE_DOC_THREAD_IDLE);
	pDoc->SetThreadMessage (BROWSE_DOC_MESSAGE_NONE);
	
	while (!bDone)
	{
		iMessage = pDoc->GetThreadMessage();

		switch (iMessage)
		{				
		case BROWSE_DOC_MESSAGE_STOP:
			{
				// Show that we've handled the message
				pDoc->SetThreadMessage (BROWSE_DOC_MESSAGE_NONE);
				pDoc->SetThreadStatus (BROWSE_DOC_THREAD_STOPPED);
				
				bDone = TRUE;				
			}
			break;

		case BROWSE_DOC_MESSAGE_REFRESH:
		case BROWSE_DOC_MESSAGE_GETFILES:
			{	
				// Show that we've handled the message
				pDoc->SetThreadMessage (BROWSE_DOC_MESSAGE_NONE);

				pDoc->SetThreadStatus (BROWSE_DOC_THREAD_INIT);				

				// Neal - fixes BoundsChecker bitching(?)
				CString strPath = pDoc->GetPath();
				pCacheList->Initialize (strPath, pParam, iMessage == BROWSE_DOC_MESSAGE_REFRESH ? TRUE : FALSE);
				
				pDoc->RefreshView();
				
				pDoc->SetThreadStatus(BROWSE_DOC_THREAD_IDLE);
			}
			break;

		default:
			Sleep (500);
			break;
		}		
	}
	ExitThread(0);
}
