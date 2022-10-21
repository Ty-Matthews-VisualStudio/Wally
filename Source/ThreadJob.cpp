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

CThreadJob::CThreadJob() : m_hEvent( NULL )
{
	m_bInitialized = FALSE;
}

CThreadJob::~CThreadJob()
{
	CleanUp();
}

void CThreadJob::CleanUp()
{
	CloseHandle( m_hEvent );
}

void CThreadJob::Initialize()
{
	m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_bInitialized = TRUE;
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

UINT WINAPI CThreadJob::Start( LPVOID lpParameter )
{
	CThreadJob *pJob = NULL;

	if( lpParameter )
	{
		pJob = (CThreadJob *)lpParameter;
	
		pJob->Initialize();
		pJob->Process();
		pJob->Stop();		
	}

	pJob = NULL;
	_endthreadex(0);

	return 0;
}