// ThreadJob.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "ThreadJob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadJob

CThreadJob::CThreadJob() : m_hEvent( NULL ), m_hThread ( NULL )
{
	m_bInitialized = FALSE;
	SetStatus(TJIdle);
}

CThreadJob::~CThreadJob()
{
	CleanUp();
}

void CThreadJob::CleanUp()
{
	if (m_hEvent)
	{
		CloseHandle(m_hEvent);
	}	
	m_hEvent = NULL;
	m_hThread = NULL;
	SetStatus(TJClosed);
}

void CThreadJob::Initialize()
{
	m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_bInitialized = TRUE;
	SetStatus(TJInitialized);
}

BOOL CThreadJob::Finished()
{
	if( m_hEvent )
	{
		if( WaitForSingleObject( m_hEvent, 0 ) == WAIT_OBJECT_0 )
		{
			return TRUE;
		}
	}
	else
	{
		if( m_bInitialized )
		{
			// The event object should always be created, but just to be safe...		
			return TRUE;
		}
	}

	return FALSE;
}

void CThreadJob::Stop()
{
	if( m_hEvent )
	{
		SetEvent( m_hEvent );
	}
	if (m_hThread)
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

BOOL CThreadJob::Stopped()
{
	return Finished();
}

void CThreadJob::SetErrorMessage( LPCTSTR szErrorMessage )
{
	m_strErrorMessage = szErrorMessage;
}

void CThreadJob::AddToErrorMessage( LPCTSTR szErrorMessage )
{
	m_strErrorMessage += szErrorMessage;
}

void CThreadJob::AddToErrorMessage( const char cErrorMessage )
{
	m_strErrorMessage += cErrorMessage;
}

LPCTSTR CThreadJob::GetErrorMessage()
{
	return (LPCTSTR)m_strErrorMessage;
}

UINT WINAPI CThreadJob::StartThread( LPVOID lpParameter )
{
	CThreadJob *pJob = NULL;

	if( lpParameter )
	{
		pJob = (CThreadJob *)lpParameter;
	
		pJob->Initialize();
		pJob->SetStatus(TJRunning);
		pJob->Process();
		pJob->SetStatus(TJFinished);
		pJob->Stop();		
	}

	pJob = NULL;
	_endthreadex(0);
	return 0;
}

void CThreadJob::Start()
{
	unsigned uiThreadID = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, CThreadJob::StartThread, (LPVOID)this, CREATE_SUSPENDED, &uiThreadID);
	if (m_hThread)
	{
		if (!SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL))
		{
			SetErrorMessage("Could not set thread priority");
		}
		else
		{
			if (ResumeThread(m_hThread) == -1)
			{
				SetErrorMessage("Could not start thread");
			}
		}
	}
	else
	{
		SetErrorMessage("Could not create thread");
	}	
}