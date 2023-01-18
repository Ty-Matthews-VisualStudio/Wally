// WallyException.cpp: implementation of the CWallyException class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "WallyException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallyException::CWallyException()
{
	m_strFile = "";
	m_dwLineNumber = 0;
	m_dwErrorCode = 0;
	m_dwLastErrorCode = 0;
}

CWallyException::CWallyException( LPCTSTR szFile, DWORD dwLineNumber )
{
	m_strFile = szFile;
	m_dwLineNumber = dwLineNumber;
}

CWallyException::~CWallyException()
{

}

void CWallyException::SetErrorCode( DWORD dwErrorCode )
{
	m_dwErrorCode = dwErrorCode;
}

DWORD CWallyException::GetErrorCode()
{
	return m_dwErrorCode;
}

void CWallyException::SetLastErrorCode( DWORD dwErrorCode )
{
	m_dwLastErrorCode = dwErrorCode;
}

DWORD CWallyException::GetLastErrorCode()
{
	return m_dwLastErrorCode;
}

void CWallyException::SetErrorMessage( LPCTSTR szErrorMessage )
{
	m_strErrorMessage = szErrorMessage;
}

LPCTSTR CWallyException::GetErrorMessage()
{
	CString strMessage("Unknown exception.");

	if( m_dwLineNumber )
	{
		strMessage.Format( "CWallyException thrown in file %s line number %lu\nSend the above information to Ty and Neal to help troubleshoot the problem.", m_strFile, m_dwLineNumber);
		return strMessage.GetBuffer(strMessage.GetLength());
	}

	if( m_dwErrorCode )
	{
		strMessage.Format( "CWallyException thrown.\nError code = %lu\nError message = %s", m_dwErrorCode, m_strErrorMessage );
		return strMessage.GetBuffer(strMessage.GetLength());
	}

	if( m_dwLastErrorCode )
	{
		LPVOID lpMsgBuf;
		
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			m_dwLastErrorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);		
		
		strMessage.Format( "Exception caught.\nGetLastError() = %lu\n%s", m_dwLastErrorCode, (LPCTSTR)lpMsgBuf);		
		LocalFree( lpMsgBuf );
		return strMessage.GetBuffer(strMessage.GetLength());
	}

	return strMessage.GetBuffer(strMessage.GetLength());
}