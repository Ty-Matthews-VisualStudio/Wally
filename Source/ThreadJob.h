#ifndef _THREADJOB_H__
#define _THREADJOB_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ThreadJob.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThreadJob thread

class CThreadJob
{
// Members
protected:
	HANDLE m_hEvent;
	BOOL m_bInitialized;
	CString m_strErrorMessage;
	CString m_strMessage;

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

	void SetErrorMessage( LPCTSTR szErrorMessage );
	void AddToErrorMessage( LPCTSTR szErrorMessage );
	void AddToErrorMessage( const char cErrorMessage );
	virtual LPCTSTR GetErrorMessage();
	virtual LPCTSTR GetMessage() = 0;

	static UINT WINAPI Start( LPVOID lpParameter );
};


#endif // #ifndef _THREADJOB_H__
