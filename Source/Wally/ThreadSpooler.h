#ifndef _THREADSPOOLER_H_
#define _THREADSPOOLER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ThreadSpooler.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThreadSpooler thread

class CThreadJob;

class CThreadSpooler
{
// Members
private:
	typedef std::list<CThreadJob *> ThreadQueue;
	typedef ThreadQueue::iterator itThread;

	ThreadQueue m_WaitingQueue;
	ThreadQueue m_ProcessQueue;
	ThreadQueue m_FinishedQueue;

	DWORD m_dwMaxThreads;

// Private Methods
private:
	DWORD GetMaxThreads();

// Public Methods
public:
	CThreadSpooler( DWORD dwMaxThreads );

	void AddJob( CThreadJob *pJob );
	BOOL Finished();
	void Process();
	void CleanUp();

};


#endif		 // #ifndef _THREADSPOOLER_H_
