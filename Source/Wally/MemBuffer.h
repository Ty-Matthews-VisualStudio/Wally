// MemBuffer.h: interface for the CMemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MEMBUFFER_H_
#define _MEMBUFFER_H_

#include <filesystem>

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
#define MEMBUFFER_FLAG_MIN_SIZE_32768		0x8000
#define MEMBUFFER_FLAG_ENUM_MOVE_REPLACE_TAGS	0x10000
#define MEMBUFFER_FLAG_ENUM_MOVE_IGNORE_BLOCK	0x20000
#define MEMBUFFER_FLAG_ENUM_MOVE_LEAVE_DATA		0x40000
#define MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG		0x80000
#define MEMBUFFER_FLAG_PARSE_LOWER_CASE_PARAMS	0x100000
#define MEMBUFFER_FLAG_ENUM_REPL_REPLACE_STRING	0x200000
#define MEMBUFFER_FLAG_ENUM_REPL_IGNORE_TAG		0x400000
#define MEMBUFFER_FLAG_MIN_DOUBLE_FACTOR		0x800000
#define MEMBUFFER_FLAG_WRITEFILE_OVERWRITE		0x1000000
#define MEMBUFFER_FLAG_WRITEFILE_SKIP_IF_EXISTS	0x2000000

#define MEMBUFFER_MIN_SIZE_MASK				(MEMBUFFER_FLAG_MIN_SIZE_64 | MEMBUFFER_FLAG_MIN_SIZE_128 | MEMBUFFER_FLAG_MIN_SIZE_256 | MEMBUFFER_FLAG_MIN_SIZE_512 | MEMBUFFER_FLAG_MIN_SIZE_1024 | MEMBUFFER_FLAG_MIN_SIZE_2048 | MEMBUFFER_FLAG_MIN_SIZE_4096 | MEMBUFFER_FLAG_MIN_SIZE_8192 | MEMBUFFER_FLAG_MIN_SIZE_16384 | MEMBUFFER_FLAG_MIN_SIZE_32768)
#define MEMBUFFER_GET_MIN_SIZE(dwFlags)		dwFlags & MEMBUFFER_MIN_SIZE_MASK


// STD lists, vectors, etc
#pragma warning( disable: 4786 )
#include <string>
#include <vector>
#include <map>
#include <comdef.h>
#include <assert.h>
#include <tchar.h>

// Forward declaration
class CMemBuffer;

typedef std::vector< std::string > _MBStringVector;
typedef _MBStringVector::iterator _MBitvString;

typedef struct
{
	std::string strType;
	std::string strValue;
	std::string strGUID;
} PRAGMA_STRUCT, *LPPRAGMA_STRUCT;

typedef std::vector< PRAGMA_STRUCT > _PragmaVector;
typedef _PragmaVector::iterator _itPragmaVector;

typedef std::map< std::string, std::string > _AttributeMap;
typedef _AttributeMap::iterator _itAttribute;
typedef _AttributeMap::value_type _valAttribute;

////////////////////////////////////////////////////////////////////////////////////
// Enumeration callback function declarations
////////////////////////////////////////////////////////////////////////////////////

typedef void (EnumSequencesCallBack) ( DWORD, LPVOID );
typedef EnumSequencesCallBack *LPEnumSequencesCallBack;

typedef BOOL (EnumReplaceFunctionsCallBack) ( _AttributeMap &, std::string &, DWORD &, LPVOID );
typedef EnumReplaceFunctionsCallBack *LPEnumReplaceFunctionsCallBack;

// First param is the list of attributes in the block.  The second param
// is the data buffer between the begin and end tags.  The third param is 
// the string to write in place of the block if the options are to replace
// all of the tags (MEMBUFFER_FLAG_MOVE_REPLACE_TAGS.)  The fourth param
// is the current flags as they were passed to the function.  This allows
// the callback routine to (temporarily) override the flags for this particular
// block.  Meaning, the callback routine may decide to leave the block as it
// sits instead of pulling it out.  The last param is the app-defined value.
typedef BOOL (EnumAndMoveCallBack) ( _AttributeMap &, CMemBuffer &, std::string &, DWORD &, LPVOID );
typedef EnumAndMoveCallBack *LPEnumAndMoveCallBack;

class CMemBuffer  
{
// Private members
private:
	LPBYTE m_pbyBuffer;	
	DWORD m_dwDataSize;			// What's being used
	DWORD m_dwTrueDataSize;		// What's actually allocated
	DWORD m_dwMinDataSize;		// Minimum data size
	std::string m_strName;

	static BYTE m_byHashCode[256];

// Private methods
private:
	void Init();	
	LPBYTE AllocateBuffer(DWORD dwDataSize, DWORD dwFlags);
	void SetBuffer( LPBYTE pbyBuffer )
	{
		m_pbyBuffer = pbyBuffer;
	}
	void SetTrueDataSize( DWORD dwTrueDataSize)
	{
		m_dwTrueDataSize = dwTrueDataSize;
	}		
	// Sets the minimum size the buffer can be
	void SetMinDataSize( DWORD dwMinDataSize)
	{
		m_dwMinDataSize = dwMinDataSize;
	}

	void BuildHashString( unsigned char **kQey, const unsigned char **kLoc, const char *sKey ) const;

// Public members
public:
	CMemBuffer();
	CMemBuffer(DWORD dwDataSize, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY );
	CMemBuffer( const CMemBuffer &mbCopy );	
	virtual ~CMemBuffer();
	void SetDataSize(DWORD dwDataSize)
	{
		m_dwDataSize = dwDataSize;
	}

#ifdef __AFX_H__
	CMemBuffer(LPCTSTR szFileName, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY);
	LPBYTE InitFromFile(CFile *fp, DWORD dwFlags = 0);
	LPBYTE InitFromFile(const LPCTSTR szFileName, DWORD dwFlags = 0);
	LPBYTE InitFromFile(std::string& sFileName, DWORD dwFlags = 0);
	LPBYTE InitFromFile(std::stringstream& sFileName, DWORD dwFlags = 0);
	BOOL AddFile(const LPCTSTR szFileName, DWORD dwFlags = 0);
	BOOL WriteToFile(const LPCSTR szFileName, DWORD dwFlags = MEMBUFFER_FLAG_WRITEFILE_OVERWRITE);
	BOOL WriteToFile(const wchar_t *szFileName, DWORD dwFlags = MEMBUFFER_FLAG_WRITEFILE_OVERWRITE);
	BOOL WriteToFile(const std::wstring &sFileName, DWORD dwFlags = MEMBUFFER_FLAG_WRITEFILE_OVERWRITE);
	BOOL WriteToFile(const std::wstringstream &ssFileName, DWORD dwFlags = MEMBUFFER_FLAG_WRITEFILE_OVERWRITE);
#else
	BOOL WriteToFile(const char *szFileName);
	CMemBuffer(const char *szFileName, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY);
	LPBYTE InitFromFile(FILE *fp, DWORD dwFlags = 0);
	LPBYTE InitFromFile(const char *szFileName, DWORD dwFlags = 0);
	bool AddFile(const char *szFileName, DWORD dwFlags = 0);
#endif
	
	BOOL Release();
	LPBYTE GetBuffer() const;
	LPBYTE GetBuffer( DWORD dwDataSize, DWORD dwFlags = MEMBUFFER_FLAG_ZEROMEMORY);
	char * GetBufferAsString( DWORD dwFlags = 0);
	
	void ReplaceString( const char *szReplace, const char *szNewString, DWORD dwFlags = 0);
	void ReplaceString(const wchar_t *szReplace, const wchar_t *szNewString, DWORD dwFlags = 0);
	void InsertString( const char *szMatch, const char *szNewString, DWORD dwFlags = MEMBUFFER_FLAG_INSERT_AFTER);
	void InsertBuffer( const char *szMatch, CMemBuffer &mbInsertBlock, DWORD dwFlags = MEMBUFFER_FLAG_INSERT_AFTER);
	void MoveStringBlock( const char *szBegin, const char *szEnd, CMemBuffer *pmbNewBlock, DWORD dwFlags = 0);
	void GetStringBlock( const char *szBegin, const char *szEnd, CMemBuffer &mbNewBlock, DWORD dwFlags = 0);
	void RemoveBlock( const char *szMatch, DWORD dwFlags = 0);
	void SetBuffer( LPBYTE pbyData, DWORD dwDataLength);
	void SetBuffer( const char *szBuffer, DWORD dwDataLength );
	void CopyBuffer( CMemBuffer *pmbCopy, DWORD dwBeginPosition = 0, DWORD dwEndPosition = 0 ) const;
	void CopyBuffer( CMemBuffer &mbCopy, DWORD dwBeginPosition = 0, DWORD dwEndPosition = 0 ) const;
	void AppendString( const char *szString, DWORD dwFlags = 0 );	
	void AppendBuffer( CMemBuffer *pBuffer, DWORD dwFlags = 0);
	void AppendBuffer( LPBYTE pbyData, DWORD dwDataLength, DWORD dwFlags = 0 );
	BOOL AdjustBufferAtPosition( DWORD dwBeginPosition, DWORD dwRemoveLength, DWORD dwInsertLength);
	BOOL IncreaseBufferSize( DWORD dwInsertLength );
	std::string GetTagValue( const char *szTag, DWORD dwFlags = 0);
	void GetPragmas( _PragmaVector &vcPragmas, DWORD dwFlags = 0 );
	BOOL GetGUID( std::string &strGUID );
	std::string GetAllTagValues( const char *szTag, LPEnumSequencesCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags = 0);
	void GetAllElements( const char *szElement, _MBStringVector &vcElements, DWORD dwFlags = 0);
	DWORD ConvertEscapeCharacter( LPBYTE pbyData, LPBYTE pbyEndData, LPBYTE pbyCharacter);
	DWORD ParseAttributes( LPBYTE pbyBuffer, DWORD dwDataSize, _AttributeMap &AttributeMap, DWORD dwFlags = 0 );
	void EnumerateReplaceFunctions( const char *szTag, LPEnumReplaceFunctionsCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags = 0);
	void EnumerateAndMoveBlocks( const char *szBegin, const char *szEnd, LPEnumAndMoveCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags = 0);
	inline unsigned int CalcHash( const char *sKey, const int MOD, const int shift ) const;
	void Encrypt( const char *sKey );
	void Decrypt( const char *sKey );
	LPBYTE InitFromResource( WORD );

#ifdef BOOST_JSON_HPP
	boost::json::value ParseJson(boost::json::error_code& ec);
#endif
	
	static std::string& TrimLeft( std::string& str, const char c );
	static std::string& TrimRight( std::string& str, const char c );
	static std::string& Trim( std::string& str, const char c );
	static std::wstring& TrimLeft(std::wstring& str, const wchar_t c);
	static std::wstring& TrimRight(std::wstring& str, const wchar_t c);
	static std::wstring& Trim(std::wstring& str, const wchar_t c);
	// These assume the space character (0x20, ' ')
	static std::string& TrimLeft( std::string& str );
	static std::string& TrimRight( std::string& str );
	static std::string& Trim( std::string& str );
	static std::wstring& TrimLeft(std::wstring& str);
	static std::wstring& TrimRight(std::wstring& str);
	static std::wstring& Trim(std::wstring& str);

	// Clears the buffer, resetting the data pointer
	void Clear()
	{
		SetDataSize( 0 );
	}

	// Gets the size of the buffer currently in use
	DWORD GetDataSize() const
	{
		return m_dwDataSize;
	}	

	// Gets the actual size of the buffer, with padding included
	DWORD GetTrueDataSize() const
	{
		return m_dwTrueDataSize;
	}
	
	// Gets the minimum size the buffer can be
	DWORD GetMinDataSize() const
	{
		return m_dwMinDataSize;
	}
	
	operator BYTE *()
	{
		return m_pbyBuffer;
	}	

	void SetName( const char *szName )
	{
		m_strName = szName;
	}

	inline DWORD GetLowerDWORD( const DWORD dwSource, const bool bCompareLowerCase )
	{
		if( !bCompareLowerCase )
		{
			return dwSource;
		}
		DWORD dwReturn = 0;
		DWORD dwMask = 0;
		for( int i = 0; i < 4; i++ )
		{
			dwMask = 0xff << (i * 8);
			if( 
				((dwSource & dwMask) >= ('A' << (i * 8))) && ((dwSource & dwMask) <= ('Z' << (i * 8)) )
			)
			{
				dwReturn |= (dwSource & dwMask) + (32 << (i * 8));
			}
			else
			{
				dwReturn |= dwSource & dwMask;
			}
		}

		return dwReturn;
	}
};

#endif // #ifndef _MEMBUFFER_H_
