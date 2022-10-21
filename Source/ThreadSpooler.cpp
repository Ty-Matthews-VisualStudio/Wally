// ThreadSpooler.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "ThreadSpooler.h"
#include "ThreadJob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadSpooler

CThreadSpooler::CThreadSpooler( DWORD dwMaxThreads )
{
	m_dwMaxThreads = dwMaxThreads;
}

DWORD CThreadSpooler::GetMaxThreads()
{
	return m_dwMaxThreads;
}

void CThreadSpooler::AddJob( CThreadJob *pJob )
{
	m_WaitingQueue.push_back( pJob );
}

BOOL CThreadSpooler::Finished()
{
	if( m_WaitingQueue.empty() )
	{
		if( m_ProcessQueue.empty() )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CThreadSpooler::Process()
{
	if( !m_WaitingQueue.empty() )
	{
		// Let's see if we have room in the processing queue
		while( m_ProcessQueue.size() < GetMaxThreads() )
		{
			itThread itJob = m_WaitingQueue.begin();

			if( itJob == m_WaitingQueue.end() )
			{
				// There are no more jobs waiting
				break;
			}

			CThreadJob *pJob = (*itJob);

			pJob->Initialize();
			_beginthread( ThreadJobFunction, 0, (LPVOID)pJob );
			
			m_ProcessQueue.push_back( pJob );
			m_WaitingQueue.erase( itJob );
		}
	}

	if( !m_ProcessQueue.empty() )
	{
		// Are any of these jobs finished?
		itThread itJob = m_ProcessQueue.begin();

		while( itJob != m_ProcessQueue.end() )
		{
			CThreadJob *pJob = (*itJob);
			
			if( pJob->Finished() )
			{
				m_FinishedQueue.push_back( pJob );
				itJob = m_ProcessQueue.erase( itJob );
			}
			else
			{				
				itJob++;
			}
		}
	}
}

void CThreadSpooler::CleanUp()
{	
	itThread itJob = m_FinishedQueue.begin();

	while( itJob != m_FinishedQueue.end() )
	{
		CThreadJob *pJob = (*itJob);
		
		delete pJob;
		pJob = NULL;

		itJob = m_FinishedQueue.erase( itJob );
	}
}