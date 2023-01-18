// ThreadList.cpp: implementation of the CThreadList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "ThreadList.h"
#include "ThreadSpooler.h"
#include "ThreadItem.h"
#include "BatchDlg.h"
#include "PackageDoc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadList::CThreadList(LPVOID pParam)
{
	SetMaxThreads (MAX_THREADS);	
	SetQueueAll (NULL);
	SetProcessingAll (NULL);
	SetFinishedAll (NULL);
	m_pDialog = pParam;
	m_iSpoolerStatus = THREADSPOOLER_STOPPED;
	m_iSpoolerMessage = THREADSPOOLER_NOMESSAGE;
	m_iDestinationType = DESTINATION_TEXTURES;
	m_pPackage = NULL;

	// Fire off the main spooler thread that will keep in constant contact with 
	// this class
	//m_pThreadSpooler = (CThreadSpooler *) AfxBeginThread ( (AFX_THREADPROC) CThreadSpooler::MainLoop, (LPVOID)this);
}

CThreadList::~CThreadList()
{
	if ((GetSpoolerStatus() != THREADSPOOLER_FINISHED) && (GetSpoolerStatus() != THREADSPOOLER_END_THREAD))
	{
		SetSpoolerMessage (THREADSPOOLER_FINISH);

		// Wait until everything has finished up
		while ((GetSpoolerStatus() != THREADSPOOLER_FINISHED) && (GetSpoolerStatus() != THREADSPOOLER_END_THREAD))
		{
			Sleep (100);
		}		
	}
}


void CThreadList::SetQueueFirst (CThreadItem *pItem)
{
	m_pQueueFirst = pItem;
}

CThreadItem *CThreadList::GetQueueFirst ()
{
	return m_pQueueFirst;
}

void CThreadList::SetQueueRead (CThreadItem *pItem)
{
	m_pQueueRead = pItem;
}

CThreadItem *CThreadList::GetQueueRead ()
{
	return m_pQueueRead;
}

void CThreadList::SetQueueWrite (CThreadItem *pItem)
{
	m_pQueueWrite = pItem;
}

CThreadItem *CThreadList::GetQueueWrite ()
{
	return m_pQueueWrite;
}

void CThreadList::SetQueueAll (CThreadItem *pItem)
{
	SetQueueFirst (pItem);
	SetQueueWrite (pItem);
	SetQueueRead (pItem);	
}



void CThreadList::SetProcessingFirst (CThreadItem *pItem)
{
	m_pProcessingFirst = pItem;
}

CThreadItem *CThreadList::GetProcessingFirst ()
{
	return m_pProcessingFirst;
}

void CThreadList::SetProcessingRead (CThreadItem *pItem)
{
	m_pProcessingRead = pItem;
}

CThreadItem *CThreadList::GetProcessingRead ()
{	
	return m_pProcessingRead;
}

void CThreadList::SetProcessingWrite (CThreadItem *pItem)
{
	m_pProcessingWrite = pItem;
}

CThreadItem *CThreadList::GetProcessingWrite ()
{
	return m_pProcessingWrite;
}

void CThreadList::SetProcessingAll (CThreadItem *pItem)
{
	SetProcessingFirst (pItem);
	SetProcessingWrite (pItem);
	SetProcessingRead (pItem);
}

void CThreadList::SetFinishedFirst (CThreadItem *pItem)
{
	m_pFinishedFirst = pItem;
}

CThreadItem *CThreadList::GetFinishedFirst ()
{
	return m_pFinishedFirst;
}

void CThreadList::SetFinishedRead (CThreadItem *pItem)
{
	m_pFinishedRead = pItem;	
}

CThreadItem *CThreadList::GetFinishedRead ()
{
	return m_pFinishedRead;
}

void CThreadList::SetFinishedWrite (CThreadItem *pItem)
{
	m_pFinishedWrite = pItem;
}

CThreadItem *CThreadList::GetFinishedWrite ()
{
	return m_pFinishedWrite;
}

void CThreadList::SetFinishedAll (CThreadItem *pItem)
{
	SetFinishedFirst (pItem);
	SetFinishedWrite (pItem);
	SetFinishedRead (pItem);
}


int CThreadList::GetQueueCount()
{
	int iCount = 0;
	CThreadItem *pItem = GetQueueFirst();
	
	while (pItem)
	{
		iCount++;
		pItem = pItem->GetNext();
	}

	pItem = NULL;
	return iCount;

}

int CThreadList::GetProcessingCount()
{
	int iCount = 0;
	CThreadItem *pItem = GetProcessingFirst();
	
	while (pItem)
	{
		iCount++;
		pItem = pItem->GetNext();
	}

	pItem = NULL;
	return iCount;

}

int CThreadList::GetMaxThreads()
{
	return m_iMaxThreads;
}

void CThreadList::SetMaxThreads(int iMaxThreads)
{
	m_iMaxThreads = iMaxThreads;
}

void CThreadList::SetPackage (CPackageDoc *pDoc)
{
	m_pPackage = pDoc;
}

void CThreadList::CheckForFinishedItems()
{	
	// This function checks the current list of threads and determines if any of them
	// have finished their job.  If so, they will be removed from the list so that other 
	// threads can be started.
	
	if (!GetProcessingFirst() && !GetQueueFirst())
	{
		// Nothing in either list!  Let's stop the spooler.
		SetSpoolerMessage (THREADSPOOLER_STOP);		
		return;
	}	
	
	CThreadItem *pItem = GetProcessingFirst();
	CThreadItem *pTemp = NULL;

	unsigned char *pbyBits[4];
	CWallyPalette *pPalette = NULL;
	int iWidth = 0;
	int iHeight = 0;
	int j = 0;
	CString strName ("");
	CString strOriginalName ("");

	while (pItem)
	{		
		if (pItem->GetJobStatus() > THREADJOB_DONE)
		{	
			if (GetDestinationType() == DESTINATION_PACKAGE)
			{
				if (pItem->GetJobStatus() == THREADJOB_SUCCESS)
				{
					for (j = 0; j < 4; j++)
					{
						pbyBits[j] = pItem->GetBits(j);
					}
					
					pPalette = pItem->GetPalette();
					iWidth = pItem->GetWidth();
					iHeight = pItem->GetHeight();
					strOriginalName = pItem->GetName();
					strName = strOriginalName.Left (15);
					
					if (!g_bOverWriteFiles)
					{
						// Come up with a unique name
						j = 1;
						while (m_pPackage->IsNameInList (strName))
						{
							strName.Format ("%s%04d", strOriginalName.Left(11), j++);
						}
						m_pPackage->AddImage (pbyBits, pPalette, strName, iWidth, iHeight, true);
					}
					else
					{
						if (m_pPackage->IsNameInList (strName))
						{
							m_pPackage->ReplaceImage (pbyBits, pPalette, strName, iWidth, iHeight);
						}
						else
						{
							m_pPackage->AddImage (pbyBits, pPalette, strName, iWidth, iHeight, true);
						}
					}
					
					
					for (j = 0; j < 4; j++)
					{
						pbyBits[j] = NULL;
					}
				}
			}
			ASSERT (m_pDialog);		// We can't update something that isn't there!
			((CBatchDlg *)m_pDialog)->AddToFinishedTotal(pItem->GetJobStatus(), pItem->m_strSourceFile, pItem->m_strDestinationFile, pItem->m_strErrorMessage);
			
			// Item has finished its thing, remove it from the processing list, and to 
			// the finished list.  We can't delete the item because the thread might not have finished
			// first.
			if (pItem->GetPrevious())
			{
				pItem->GetPrevious()->SetNext(pItem->GetNext());
			}
			
			if (pItem->GetNext())
			{
				pItem->GetNext()->SetPrevious(pItem->GetPrevious());
			}
			
			if (pItem == GetProcessingFirst())
			{
				SetProcessingFirst(pItem->GetNext());
			}

			if (pItem == GetProcessingWrite())
			{
				SetProcessingWrite(pItem->GetPrevious());
			}			
			pTemp = pItem->GetNext();

			pItem->SetNext(NULL);
			pItem->SetPrevious(NULL);


			// Item is out of the processing list, add it to the finished list
			if (!GetFinishedFirst())
			{
				SetFinishedAll (pItem);
			}
			else
			{
				GetFinishedWrite()->SetNext(pItem);
				pItem->SetPrevious(GetFinishedWrite());				
				SetFinishedWrite (pItem);				
			}			
			pItem = pTemp;
		}
		else
		{
			pItem = pItem->GetNext();
		}		
	}
}

void CThreadList::UpdateStatus()
{
	ASSERT (m_pDialog);		// We can't update something that isn't there!

	((CBatchDlg *)m_pDialog)->UpdateThreadStatus(GetProcessingCount(), GetQueueCount());
}

void CThreadList::AddJob (LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory)
{
	// This function adds a job to the current queue list
	CThreadItem *pNewItem = new CThreadItem (szSourceFile, szDestinationDirectory, szOffsetDirectory, GetDestinationType(), GetImageType(), m_pPackage);

	CThreadItem *pWrite = NULL;

	if (!GetQueueFirst())
	{
		SetQueueAll (pNewItem);
	}
	else
	{
		pWrite = GetQueueWrite();
		pWrite->SetNext (pNewItem);
		pNewItem->SetPrevious (pWrite);
		SetQueueWrite(pNewItem);		
	}

	pWrite = NULL;
}

void CThreadList::ProcessNextItem()
{
	CThreadItem *pQueuedJob = GetQueueFirst();

	ASSERT (pQueuedJob);		// This function should not be called unless there is a job
								// to be processed!
	if (!pQueuedJob)
	{
		return;
	}

	// First thing is to remove the queue job from the queue list
	CThreadItem *pQueueNext = pQueuedJob->GetNext();
	if (pQueueNext)
	{
		pQueueNext->SetPrevious(NULL);
	}

	SetQueueFirst(pQueueNext);

	if (pQueuedJob == GetQueueWrite())
	{
		SetQueueWrite (NULL);
	}

	pQueuedJob->SetNext (NULL);
	pQueuedJob->SetPrevious (NULL);

	// Now we move it over to the processing list
	CThreadItem *pProcessingWrite = NULL;

	if (!GetProcessingFirst())
	{
		SetProcessingAll (pQueuedJob);		
	}
	else
	{
		pProcessingWrite = GetProcessingWrite();
		pProcessingWrite->SetNext (pQueuedJob);
		pQueuedJob->SetPrevious (pProcessingWrite);
		SetProcessingWrite (pQueuedJob);		
	}

	// And finally... start it up
	pQueuedJob->StartJob();	
}

void CThreadList::SetSpoolerStatus(int iStatus)
{
	ASSERT (iStatus > THREADSPOOLER_STATUS_CODES);

	m_iSpoolerStatus = iStatus;
}

int CThreadList::GetSpoolerStatus ()
{
	return m_iSpoolerStatus;
}

void CThreadList::SetSpoolerMessage (int iMessage)
{
	ASSERT (iMessage < THREADSPOOLER_MESSAGES_END);

	if ((iMessage == THREADSPOOLER_START) && (m_iSpoolerStatus != THREADSPOOLER_STOPPED))
	{
		// You can't start up the spooler unless it's already stoppped!

		ASSERT (false);
	}

	while (GetSpoolerMessage() != THREADSPOOLER_NOMESSAGE)
	{		
		// Another message is being processed, please hold!
		Sleep (500);
	}
	m_iSpoolerMessage = iMessage;
}

int CThreadList::GetSpoolerMessage ()
{
	return m_iSpoolerMessage;
}

void CThreadList::ClearMessage()
{
	m_iSpoolerMessage = THREADSPOOLER_NOMESSAGE;
}

void CThreadList::StopAllThreads()
{
	ASSERT (m_pDialog);		// We can't update something that isn't there!

	if (GetSpoolerMessage() != THREADSPOOLER_FINISH)
	{
		((CBatchDlg *)m_pDialog)->BeginWaitCursor();
	}
	CThreadItem *pItem = GetProcessingFirst();

	while (pItem)
	{	
		if (pItem->GetJobStatus() != THREADJOB_WAITING)
		{
			pItem->SetMessage (THREADJOB_STOP);
		}
		pItem = pItem->GetNext();
	}

	pItem = GetQueueFirst();

	while (pItem)
	{
		if (pItem->GetJobStatus() != THREADJOB_WAITING)
		{
			pItem->SetMessage (THREADJOB_STOP);
		}
		pItem = pItem->GetNext();
	}
	
	PurgeAllQueues();
	
	if (GetSpoolerMessage() != THREADSPOOLER_FINISH)
	{
		// Only update the dialog if we're still going to be there
		((CBatchDlg *)m_pDialog)->SpoolerStopped();
		((CBatchDlg *)m_pDialog)->EndWaitCursor();
	}
	
}

void CThreadList::PurgeAllQueues()
{
	CThreadItem *pItem = GetQueueFirst();
	CThreadItem *pItem2 = NULL;		
	
	while (pItem)
	{
		while ((pItem->GetJobStatus() < THREADJOB_DONE) && (pItem->GetJobStatus() != THREADJOB_WAITING))
		{			
			Sleep(50);	// Wait until it has cancelled or finished
		}
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;		
	}
	SetQueueAll (NULL);	

	pItem = GetProcessingFirst();

	while (pItem)
	{
		while ((pItem->GetJobStatus() < THREADJOB_DONE) && (pItem->GetJobStatus() != THREADJOB_WAITING))
		{			
			Sleep(50);	// Wait until it has cancelled or finished
		}
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;		
	}
	SetProcessingAll (NULL);	

	pItem = GetFinishedFirst();

	while (pItem)
	{		
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;
	}
	SetFinishedAll (NULL);	

	pItem = NULL;
	pItem2 = NULL;
}

void CThreadList::DoProcessing()
{
	// Check to see if any items have finished	
	CheckForFinishedItems();

	// Update the dialog status items	
	UpdateStatus();

	// See if a stop message has been posted
	int iStatus = GetSpoolerStatus();
	if (iStatus == THREADSPOOLER_STOP)
	{		
		// Get out, no need to start any more threads.
		return;
	}

	// Determine the maximum number of threads
	int iMaxThreads = GetMaxThreads();

	// Check the queue to see if there are any jobs waiting to be processed
	int iQueueCount = GetQueueCount();

	if (iQueueCount == 0)
	{
		// No threads waiting to be processed
		return;
	}

	int iProcessingCount = GetProcessingCount();
	
	if (iProcessingCount < iMaxThreads)
	{
		// We have room for another thread, go fire it off
		ProcessNextItem();
	}
}

void CThreadList::SetDestinationType (int iType, CPackageDoc *pDoc)
{
	ASSERT ((iType == DESTINATION_PACKAGE) || (iType == DESTINATION_TEXTURES));
	if (iType == DESTINATION_PACKAGE)
	{
		ASSERT (pDoc);
	}

	m_iDestinationType = iType;
	if (iType == DESTINATION_PACKAGE)
	{
		m_pPackage = pDoc;
	}
}

int CThreadList::GetDestinationType ()
{
	return m_iDestinationType;
}

void CThreadList::SetImageType (int iType)
{
	m_iImageType = iType;
}

int CThreadList::GetImageType ()
{
	return m_iImageType;
}
