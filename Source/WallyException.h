// WallyException.h: interface for the CWallyException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WALLYEXCEPTION_H__EC8C10B6_DE97_11D4_BECD_00500418578A__INCLUDED_)
#define AFX_WALLYEXCEPTION_H__EC8C10B6_DE97_11D4_BECD_00500418578A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _DEBUG
#undef ASSERT
#define ASSERT(bCondition)		if ( !(bCondition) && g_bThrowAsserts) { throw CWallyException( __FILE__, __LINE__); }
#endif

class CWallyException
{
private:
	CString m_strFile;
	CString m_strErrorMessage;
	DWORD m_dwLineNumber;
	DWORD m_dwLastErrorCode;
	DWORD m_dwErrorCode;

public:
	CWallyException();
	CWallyException( LPCTSTR szFile, DWORD dwLineNumber);
	virtual ~CWallyException();

	virtual void SetErrorCode( DWORD dwErrorCode );
	virtual DWORD GetErrorCode();
	virtual void SetLastErrorCode( DWORD dwErrorCode );
	virtual DWORD GetLastErrorCode();
	virtual void SetErrorMessage( LPCTSTR szErrorMessage );
	virtual LPCTSTR GetErrorMessage();	

};

#endif // !defined(AFX_WALLYEXCEPTION_H__EC8C10B6_DE97_11D4_BECD_00500418578A__INCLUDED_)
