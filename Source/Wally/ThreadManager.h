// ThreadManager.h: interface for the CThreadManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _THREADMANAGER_H__
#define _THREADMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define THREAD_MGR_MESSAGE_START		0
#define THREAD_MGR_MESSAGE_STARTED		(THREAD_MGR_MESSAGE_START + 1)
#define THREAD_MGR_MESSAGE_STOPPED		(THREAD_MGR_MESSAGE_START + 2)
#define THREAD_MGR_MESSAGE_STATUS		(THREAD_MGR_MESSAGE_START + 3)

class CThreadMessage
{
private:
	LPVOID		m_lpAppSpecific;
	DWORD		m_dwMessageID;
	CString		m_strMessage;
	
	typedef map<CString, long> KeyValueMap;
	typedef KeyValueMap::value_type vtKeyValue;
	typedef KeyValueMap::iterator itKeyValue;
	
	KeyValueMap m_KeyValueMap;

public:
	CThreadMessage( LPVOID lpAppSpecific = NULL) : m_strMessage("")
	{
		SetAppSpecific( lpAppSpecific );
	}

	void SetAppSpecific( LPVOID lpAppSpecific )
	{
		m_lpAppSpecific = lpAppSpecific;
	}
	LPVOID GetAppSpecific()
	{
		return m_lpAppSpecific;
	}
	void SetMessageID( DWORD dwMessageID )
	{
		m_dwMessageID = dwMessageID;
	}
	DWORD GetMessageID()
	{
		return m_dwMessageID;
	}
	void SetMessage( LPCTSTR szMessage )
	{
		m_strMessage = szMessage;
	}
	void AddToMessage( LPCTSTR szMessage )
	{
		m_strMessage += szMessage;
	}
	LPCTSTR GetMessage()
	{
		return (LPCTSTR)m_strMessage;
	}
	void AddKeyValue( LPCTSTR szKey, long lValue )
	{
		m_KeyValueMap.insert( vtKeyValue( szKey, lValue ));		
	}
	void Clear()
	{
		m_strMessage = "";
		m_KeyValueMap.clear();
	}
};

typedef void (WINAPI ThreadManagerCallBack) ( CThreadMessage* );
typedef ThreadManagerCallBack *LPThreadManagerCallBack;

class CThreadManager  
{
// Members
private:
#if 0
	typedef list<LPVOID> ThreadQueue;
	typedef ThreadQueue *LPThreadQueue;
	typedef ThreadQueue::iterator itThreadQueue;
		
	ThreadQueue m_WaitingQueue;
	ThreadQueue m_ProcessingQueue;
	ThreadQueue m_FinishedQueue;	

protected:
	LPThreadQueue GetWaitingQueue();
	LPThreadQueue GetProcessingQueue();
	LPThreadQueue GetFinishedQueue();
#endif

private:
	HANDLE m_hEvent;
	HANDLE m_hFinished;
	DWORD m_dwMaxThreads;

	LPThreadManagerCallBack m_fnCallBack;
	CThreadMessage m_ThreadMessage;
	std::list<LPVOID> JobQueue;

public:
	CThreadManager( DWORD dwMaxThreads = 1 );
	virtual ~CThreadManager();

	virtual void Initialize();
	virtual void Start();
	virtual void Stop();
	virtual BOOL Finished( BOOL bSetEvent = FALSE );
	virtual BOOL Stopped( BOOL bSetEvent = FALSE );
	virtual void AddJob(LPVOID lpJob)
	{
		JobQueue.push_back(lpJob);
	}
	virtual void ClearQueue()
	{
		JobQueue.erase(JobQueue.begin(), JobQueue.end());
	}
	std::list<LPVOID>* GetQueue()
	{
		return &JobQueue;
	}

	static UINT WINAPI Process( LPVOID lpParameter );
	DWORD GetMaxThreads();
	void SetMaxThreads( DWORD dwMaxThreads );
	void RegisterCallBack( LPThreadManagerCallBack lpCallBack, CThreadMessage *lpMessage = NULL );
	void SendMessage( DWORD dwMessageID, LPCTSTR szMessage = NULL );
#if 0
	void ClearQueues()
	{
		m_WaitingQueue.erase(m_WaitingQueue.begin(), m_WaitingQueue.end());
		m_ProcessingQueue.erase(m_ProcessingQueue.begin(), m_ProcessingQueue.end());
		m_FinishedQueue.erase(m_FinishedQueue.begin(), m_FinishedQueue.end());		
	}
#endif

};

#endif // #ifndef _THREADMANAGER_H__
