// ThreadList.h: interface for the CThreadList class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_THREADLIST_H__31A97D72_7731_11D2_BA7F_00104BCBA50D__INCLUDED_)
#define AFX_THREADLIST_H__31A97D72_7731_11D2_BA7F_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "PackageDoc.h"

// Messages
#define THREADSPOOLER_MESSAGES			0
#define THREADSPOOLER_NOMESSAGE			(THREADSPOOLER_MESSAGES + 1)
#define THREADSPOOLER_STOP				(THREADSPOOLER_MESSAGES + 2)
#define THREADSPOOLER_START				(THREADSPOOLER_MESSAGES + 3)
#define THREADSPOOLER_FINISH			(THREADSPOOLER_MESSAGES + 4)
#define THREADSPOOLER_MESSAGES_END		(THREADSPOOLER_FINISH + 1)


// True status
#define THREADSPOOLER_STATUS_CODES		1000
#define THREADSPOOLER_STOPPED			(THREADSPOOLER_STATUS_CODES + 1)
#define THREADSPOOLER_STARTED			(THREADSPOOLER_STATUS_CODES + 2)
#define THREADSPOOLER_FINISHED			(THREADSPOOLER_STATUS_CODES + 3)
#define THREADSPOOLER_END_THREAD		(THREADSPOOLER_STATUS_CODES + 4)


#define MAX_THREADS 10
//#define DESTINATION_PACKAGE			0
//#define DESTINATION_TEXTURES		1

class CThreadItem;
class CThreadSpooler;

class CThreadList  
{
// Members
private:
	int m_iMaxThreads;
	int m_iDestinationType;
	int m_iImageType;
	CPackageDoc *m_pPackage;

	CThreadItem *m_pQueueFirst;
	CThreadItem *m_pQueueRead;
	CThreadItem *m_pQueueWrite;

	CThreadItem *m_pProcessingFirst;
	CThreadItem *m_pProcessingRead;
	CThreadItem *m_pProcessingWrite;

	CThreadItem *m_pFinishedFirst;
	CThreadItem *m_pFinishedRead;
	CThreadItem *m_pFinishedWrite;

	CThreadSpooler *m_pThreadSpooler;
	LPVOID m_pDialog;

// Private Methods
private:
	void SetQueueFirst (CThreadItem *pItem);
	CThreadItem *GetQueueFirst ();
	void SetQueueRead (CThreadItem *pItem);
	CThreadItem *GetQueueRead ();
	void SetQueueWrite (CThreadItem *pItem);
	CThreadItem *GetQueueWrite ();
	void SetQueueAll (CThreadItem *pItem);

	void SetProcessingFirst (CThreadItem *pItem);	
	void SetProcessingRead (CThreadItem *pItem);
	CThreadItem *GetProcessingRead ();
	void SetProcessingWrite (CThreadItem *pItem);
	CThreadItem *GetProcessingWrite ();
	void SetProcessingAll (CThreadItem *pItem);

	void SetFinishedFirst (CThreadItem *pItem);
	CThreadItem *GetFinishedFirst ();
	void SetFinishedRead (CThreadItem *pItem);
	CThreadItem *GetFinishedRead ();
	void SetFinishedWrite (CThreadItem *pItem);
	CThreadItem *GetFinishedWrite ();
	void SetFinishedAll (CThreadItem *pItem);

	void PurgeAllQueues();

// Public Members
public:
	int m_iSpoolerStatus;
	int m_iSpoolerMessage;


// Public Methods
public:
	CThreadList(LPVOID pParam);
	virtual ~CThreadList();

	int GetQueueCount();
	int GetProcessingCount();
	int GetMaxThreads();
	void SetMaxThreads(int iMaxThreads);
	void SetDestinationType (int iType, CPackageDoc *pDoc = NULL);
	int GetDestinationType ();
	void SetImageType (int iType);
	int GetImageType ();
	void SetPackage (CPackageDoc *pDoc);
	
	void CheckForFinishedItems();
	void AddJob (LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory);
	void ProcessNextItem();
	
	void SetSpoolerStatus(int iStatus);
	int GetSpoolerStatus ();
	void SetSpoolerMessage (int iMessage);
	int GetSpoolerMessage ();
	void ClearMessage();
	
	void StopAllThreads();
	void UpdateStatus();
	void DoProcessing();

	bool IsMessage(int iStatusCode)
	{
		return (iStatusCode < THREADSPOOLER_MESSAGES_END);
	}

	// This one is public so that we can check on the status
	CThreadItem *GetProcessingFirst ();

};

#endif // !defined(AFX_THREADLIST_H__31A97D72_7731_11D2_BA7F_00104BCBA50D__INCLUDED_)
