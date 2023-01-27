#ifndef _THREADJOB_H__
#define _THREADJOB_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ThreadJob.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThreadJob thread

typedef enum
{
	TJIdle = 0,
	TJInitialized,
	TJRunning,
	TJFinished,
	TJClosed
} eThreadJobStatus;

class CThreadJob
{
// Members
protected:
	HANDLE m_hEvent;
	HANDLE m_hThread;
	BOOL m_bInitialized;
	CString m_strErrorMessage;
	CString m_strMessage;
	eThreadJobStatus m_eStatus;

// Private Methods
protected:
		

// Public Methods
public:
	CThreadJob();
	virtual ~CThreadJob();

	virtual void Initialize();
	virtual BOOL Finished();
	virtual void Process() = 0;
	virtual void Stop();
	virtual BOOL Stopped();
	virtual void CleanUp();
	virtual void Start();
	virtual void SetStatus(eThreadJobStatus eStatus)
	{
		m_eStatus = eStatus;
	}
	virtual eThreadJobStatus GetStatus()
	{
		return m_eStatus;
	}

	void SetErrorMessage( LPCTSTR szErrorMessage );
	void AddToErrorMessage( LPCTSTR szErrorMessage );
	void AddToErrorMessage( const char cErrorMessage );
	virtual LPCTSTR GetErrorMessage();
	virtual LPCTSTR GetMessage() = 0;

	static UINT WINAPI StartThread( LPVOID lpParameter );
};


#endif // #ifndef _THREADJOB_H__
