// MemBuffer.h: interface for the CMemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MEMBUFFER_H_
#define _MEMBUFFER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MEMBUFFER_FLAG_ZEROMEMORY			0x1
#define MEMBUFFER_FLAG_MATCH_CASE			0x2
#define MEMBUFFER_FLAG_MATCH_FIRST_ONLY		0x4
#define MEMBUFFER_FLAG_INSERT_BEFORE		0x8
#define MEMBUFFER_FLAG_INSERT_AFTER			0x10
#define MEMBUFFER_FLAG_MOVE_LEAVE_TAGS		0x20
#define MEMBUFFER_FLAG_MIN_SIZE_64			0x40
#define MEMBUFFER_FLAG_MIN_SIZE_128			0x80
#define MEMBUFFER_FLAG_MIN_SIZE_256			0x100
#define MEMBUFFER_FLAG_MIN_SIZE_512			0x200
#define MEMBUFFER_FLAG_MIN_SIZE_1024		0x400
#define MEMBUFFER_FLAG_MIN_SIZE_2048		0x800
#define MEMBUFFER_FLAG_MIN_SIZE_4096		0x1000
#define MEMBUFFER_FLAG_MIN_SIZE_8192		0x2000
#define MEMBUFFER_FLAG_MIN_SIZE_16384		0x4000

#define MEMBUFFER_MIN_SIZE_MASK				(MEMBUFFER_FLAG_MIN_SIZE_64 | MEMBUFFER_FLAG_MIN_SIZE_128 | MEMBUFFER_FLAG_MIN_SIZE_256 | MEMBUFFER_FLAG_MIN_SIZE_512 | MEMBUFFER_FLAG_MIN_SIZE_1024 | MEMBUFFER_FLAG_MIN_SIZE_2048 | MEMBUFFER_FLAG_MIN_SIZE_4096 | MEMBUFFER_FLAG_MIN_SIZE_8192 | MEMBUFFER_FLAG_MIN_SIZE_16384)
#define MEMBUFFER_GET_MIN_SIZE(dwFlags)		dwFlags & MEMBUFFER_MIN_SIZE_MASK

////////////////////////////////////////////////////////////////////////////////////
// Enumeration callback function declarations
////////////////////////////////////////////////////////////////////////////////////

typedef void (EnumSequencesCallBack) ( DWORD, LPVOID );
typedef EnumSequencesCallBack *LPEnumSequencesCallBack;


class CMemBuffer  
{
// Private members
private:
	LPBYTE m_pbyBuffer;
	DWORD m_dwDataSize;			// What's being used
	DWORD m_dwTrueDataSize;		// What's actually allocated
	DWORD m_dwMinDataSize;		// Minimum data size
	CString m_strName;

// Private methods
private:
	void Init();	
	LPBYTE AllocateBuffer(DWORD dwDataSize, DWORD dwFlags);

// Public members
public:
	CMemBuffer( LPCTSTR szName = NULL);
	CMemBuffer(DWORD dwDataSize, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY);
	virtual ~CMemBuffer();
	
	BOOL Release();
	LPBYTE GetBuffer();
	LPBYTE GetBuffer( DWORD dwDataSize, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY);
	LPBYTE InitFromFile( FILE *fp, DWORD dwFlags = 0);
	LPBYTE InitFromFile( LPCTSTR szFileName, DWORD dwFlags = 0);
	void WriteToFile( LPCTSTR szFileName);
	void ReplaceString( LPCTSTR szReplace, LPCTSTR szNewString, DWORD dwFlags = 0);
	void InsertString( LPCTSTR szMatch, LPCTSTR szNewString, DWORD dwFlags = MEMBUFFER_FLAG_INSERT_AFTER);
	void InsertBuffer( LPCTSTR szMatch, CMemBuffer *pmbInsertBlock, DWORD dwFlags = MEMBUFFER_FLAG_INSERT_AFTER);
	void MoveStringBlock( LPCTSTR szBegin, LPCTSTR szEnd, CMemBuffer *pmbNewBlock, DWORD dwFlags = 0);
	void CopyBuffer( CMemBuffer *pmbCopy);
	BOOL AdjustBufferAtPosition( DWORD dwBeginPosition, DWORD dwRemoveLength, DWORD dwInsertLength);
	CString GetTagValue( LPCTSTR szTag, DWORD dwFlags = 0);
	CString GetAllTagValues( LPCTSTR szTag, LPEnumSequencesCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags = 0);
	
	void SetBuffer( LPBYTE pbyBuffer)
	{
		m_pbyBuffer = pbyBuffer;
	}
	DWORD GetDataSize()
	{
		return m_dwDataSize;
	}
	void SetDataSize( DWORD dwDataSize)
	{
		m_dwDataSize = dwDataSize;
	}
	DWORD GetTrueDataSize()
	{
		return m_dwTrueDataSize;
	}
	void SetTrueDataSize( DWORD dwTrueDataSize)
	{
		m_dwTrueDataSize = dwTrueDataSize;
	}
	DWORD GetMinDataSize()
	{
		return m_dwMinDataSize;
	}
	void SetMinDataSize( DWORD dwMinDataSize)
	{
		m_dwMinDataSize = dwMinDataSize;
	}
	operator BYTE *()
	{
		return m_pbyBuffer;
	}	
};

#endif // #ifndef _MEMBUFFER_H_
