#if !defined __SAFEFILE_H
#define __SAFEFILE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Error code definitions
#define		SF_SUCCESS				0
#define		SF_OUT_OF_MEMORY		(SF_SUCCESS + 1)
#define		SF_ERROR_READING_FILE	(SF_SUCCESS + 2)
#define		SF_FILE_NOT_FOUND		(SF_SUCCESS + 3)
#define		SF_UNKNOWN_FILE_SIZE	(SF_SUCCESS + 4)
#define		SF_ERROR_MAPPING_FILE	(SF_SUCCESS + 5)

class CSafeFile 
{
// Members
private:		
	DWORD	m_dwDataSize;
	LPVOID	m_lpData;
	DWORD	m_dwErrorCode;
	
	CString m_strFileName;
	CString m_strErrorCode;

public:
	CSafeFile();
	CSafeFile( LPCTSTR szFileName );
	~CSafeFile();
	
	BOOL Open( LPCTSTR szFileName = NULL );
	void SetErrorCode( DWORD dwCode )
	{
		m_dwErrorCode = dwCode;
	}
	DWORD GetErrorCode()
	{
		return m_dwErrorCode;
	}
	LPCTSTR GetErrorText();
	DWORD GetDataSize()
	{
		return m_dwDataSize;
	}
	operator LPBYTE ()
	{
		return (LPBYTE)m_lpData;
	}
	LPBYTE GetBuffer()
	{
		return (LPBYTE)m_lpData;
	}
	void ReleaseFile();
};


#endif // __SAFEFILE_H