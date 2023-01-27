// ThreadManager.cpp: implementation of the CThreadManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "ThreadManager.h"
#include "ThreadJob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadManager::CThreadManager( DWORD dwMaxThreads /* = 1 */ ) : m_hEvent( NULL ), m_hFinished( NULL ), m_fnCallBack( NULL )
{
	SetMaxThreads( dwMaxThreads );
}

CThreadManager::~CThreadManager()
{
	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
	}
	if (m_hFinished)
	{
		CloseHandle(m_hFinished);
	}	
	//ClearQueues();
}

void CThreadManager::Initialize()
{
	m_hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hFinished = CreateEvent( NULL, TRUE, FALSE, NULL );
}

void CThreadManager::Start()
{
	Initialize();
	
	unsigned uiThreadID;
	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, CThreadManager::Process, (LPVOID)this, CREATE_SUSPENDED, &uiThreadID);	

	if( hThread )
	{
		SetThreadPriority( hThread, THREAD_PRIORITY_IDLE );
		ResumeThread( hThread );
	}	
}

void CThreadManager::Stop()
{
	Stopped( TRUE );
}

BOOL CThreadManager::Stopped( BOOL bSetEvent /* = FALSE */ )
{
	if( m_hEvent )
	{
		if( bSetEvent )
		{
			SetEvent( m_hEvent );
		}
		else
		{
			if( WaitForSingleObject( m_hEvent, 0 ) == WAIT_OBJECT_0 )
			{
				return TRUE;
			}
		}
	}
	else
	{
		// The event object should always be created, but just to be safe...
		return TRUE;
	}

	return FALSE;
}

BOOL CThreadManager::Finished( BOOL bSetEvent /* = FALSE */ )
{
	if( m_hFinished )
	{
		if( bSetEvent )
		{
			SetEvent( m_hFinished );
		}
		else
		{
			if( WaitForSingleObject( m_hFinished, 0 ) == WAIT_OBJECT_0 )
			{
				return TRUE;
			}
		}
	}
	else
	{
		// The event object should always be created, but just to be safe...
		return TRUE;
	}

	return FALSE;
}

DWORD CThreadManager::GetMaxThreads()
{
	return m_dwMaxThreads;
}

void CThreadManager::SetMaxThreads( DWORD dwMaxThreads )
{
	m_dwMaxThreads = dwMaxThreads;
}

void CThreadManager::RegisterCallBack( LPThreadManagerCallBack lpCallBack, CThreadMessage *lpMessage /* = NULL */)
{
	m_fnCallBack = lpCallBack;

	if( lpMessage )
	{
		m_ThreadMessage = (*lpMessage);
	}
}

void CThreadManager::SendMessage( DWORD dwMessageID, LPCTSTR szMessage /* = NULL */ )
{
	if( m_fnCallBack )
	{
		m_ThreadMessage.SetMessageID( dwMessageID );

		if( szMessage )
		{
			m_ThreadMessage.SetMessage( szMessage );
		}
		m_fnCallBack( &m_ThreadMessage );
		m_ThreadMessage.Clear();
	}	
}

#if 0
CThreadManager::LPThreadQueue CThreadManager::GetWaitingQueue()
{
	return &m_WaitingQueue;
}

CThreadManager::LPThreadQueue CThreadManager::GetProcessingQueue()
{
	return &m_ProcessingQueue;
}

CThreadManager::LPThreadQueue CThreadManager::GetFinishedQueue()
{
	return &m_FinishedQueue;
}

void CThreadManager::AddJob(LPVOID lpJob)
{
	m_WaitingQueue.push_back(lpJob);
}

UINT WINAPI CThreadManager::Process( LPVOID lpParameter )
{
	CThreadManager *pThis = NULL;
	CThreadJob *pJob = NULL;
	unsigned uiThreadID = 0;	
	BOOL bDone = FALSE;
	CString strMessage("");
	itThreadQueue itJob;
	
	if( lpParameter )
	{
		pThis = (CThreadManager *)lpParameter;
		pThis->SendMessage( THREAD_MGR_MESSAGE_STARTED );		

		while( !bDone )
		{
			if( !pThis->GetWaitingQueue()->empty() )
			{				
				// Let's see if we have room in the processing queue
				while( pThis->GetProcessingQueue()->size() < pThis->GetMaxThreads() )
				{					
					itJob = pThis->GetWaitingQueue()->begin();
					if( itJob == pThis->GetWaitingQueue()->end() )
					{						
						break;
					}

					pJob = (CThreadJob *)(*itJob);					
					HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, CThreadJob::Start, (LPVOID)pJob, CREATE_SUSPENDED, &uiThreadID );

					try
					{
						if( !hThread )
						{
							throw( "Could not start up thread" );							
						}
						
						if( !SetThreadPriority( hThread, THREAD_PRIORITY_BELOW_NORMAL ) )
						{
							throw( "Could not set thread priority" );
						}
						
						if( ResumeThread( hThread ) == -1 )
						{
							throw( "Could not resume thread" );							
						}						
							
						pThis->GetProcessingQueue()->push_back( (LPVOID)pJob );

					}
					catch( LPCTSTR szErrorMessage )
					{						
						pJob->SetErrorMessage( szErrorMessage );
						pThis->GetFinishedQueue()->push_back( (LPVOID)pJob );
					}					
					
					pThis->GetWaitingQueue()->erase( itJob );					
				}
			}

			// Are any of these jobs finished?
			itJob = pThis->GetProcessingQueue()->begin();
			while( itJob != pThis->GetProcessingQueue()->end() )
			{
				pJob = (CThreadJob *)(*itJob);
					
				if( pJob->Finished() )
				{
					pJob->CleanUp();
					pThis->GetFinishedQueue()->push_back( (LPVOID)pJob );

					// Go send a message						
					pThis->SendMessage( THREAD_MGR_MESSAGE_STATUS, pJob->GetMessage() );
					(*itJob) = NULL;
					itJob = pThis->GetProcessingQueue()->erase( itJob );
				}
				else
				{				
					itJob++;
				}
			}			

			// Are we signalled to break?
			if( pThis->Stopped() )
			{				
				// Shuttle everything currently waiting into the finished queue
				itJob = pThis->GetWaitingQueue()->begin();

				while( itJob != pThis->GetWaitingQueue()->end() )
				{						
					pThis->GetFinishedQueue()->push_back(*itJob);
					(*itJob) = NULL;
					itJob = pThis->GetWaitingQueue()->erase( itJob );						
				}

				// Now signal the ones that are processing that we're stopping
				itJob = pThis->GetProcessingQueue()->begin();
				while( itJob != pThis->GetProcessingQueue()->end() )
				{
					CThreadJob *pJob = (CThreadJob *)(*itJob);
					pJob->Stop();
					pThis->GetFinishedQueue()->push_back(*itJob);
					(*itJob) = NULL;
					itJob = pThis->GetProcessingQueue()->erase(itJob);					
				}
			}

			if (pThis->GetWaitingQueue()->empty() && pThis->GetProcessingQueue()->empty())
			{
				bDone = TRUE;
			}
			
			Sleep( 10 );
		}

		pThis->Finished( TRUE );		
		pThis->SendMessage( THREAD_MGR_MESSAGE_STOPPED );
		pThis = NULL;
	
	}	
	_endthreadex(0);
	return 0;
}
#else
UINT WINAPI CThreadManager::Process(LPVOID lpParameter)
{
	CThreadManager* pThis = NULL;
	CThreadJob* pJob = NULL;
	BOOL bDone = FALSE;
	CString strMessage("");
	
	if (lpParameter)
	{
		pThis = (CThreadManager*)lpParameter;
		pThis->SendMessage(THREAD_MGR_MESSAGE_STARTED);

		while(1)
		{
			int iRunCount = 0;
			int iClosedCount = 0;
			// Get count of currently running threads
			for (LPVOID pIT : (*pThis->GetQueue()))
			{
				pJob = (CThreadJob*)pIT;
				switch (pJob->GetStatus())
				{
				case TJRunning:
				case TJInitialized:
					iRunCount++;
					if (pThis->Stopped())
					{
						pJob->Stop();
					}
					break;

				case TJFinished:
					if (pJob->Finished())
					{
						pJob->CleanUp();
						// Go send a message						
						pThis->SendMessage(THREAD_MGR_MESSAGE_STATUS, pJob->GetMessage());						
					}
					break;

				case TJClosed:
					iClosedCount++;
					break;

				case TJIdle:
				default:
					break;
				}
			}

			if (iClosedCount == pThis->GetQueue()->size())
			{
				break;
			}

			for (LPVOID pIT : (*pThis->GetQueue()))
			{
				if (iRunCount >= pThis->GetMaxThreads())
				{
					break;
				}
				pJob = (CThreadJob*)pIT;
				switch (pJob->GetStatus())
				{
				case TJIdle:
					pJob->Start();
					iRunCount++;
					break;

				case TJRunning:
				case TJInitialized:
				case TJFinished:
				default:
					break;
				}
			}			
			Sleep(10);
		}

		pThis->Finished(TRUE);
		pThis->SendMessage(THREAD_MGR_MESSAGE_STOPPED);
		pThis->ClearQueue();
		pThis = NULL;
	}
	_endthreadex(0);
	return 0;
}
#endif