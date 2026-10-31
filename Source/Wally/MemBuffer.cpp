// MemBuffer.cpp: implementation of the CMemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MemBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

CMemBuffer::CMemBuffer()
{
	Init();
}

CMemBuffer::CMemBuffer( const CMemBuffer &mbCopy )
{
	Init();
	mbCopy.CopyBuffer( this );
}

CMemBuffer::CMemBuffer(DWORD dwDataSize, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */ )
{
	Init();
	AllocateBuffer(dwDataSize, dwFlags);
}

#ifdef __AFX_H__
CMemBuffer::CMemBuffer(LPCTSTR szFileName, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */)
{
	Init();
	InitFromFile(szFileName, dwFlags);
}
#else
CMemBuffer::CMemBuffer( const char *szFileName, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */ )
{
	Init();
	InitFromFile( szFileName, dwFlags );	
}
#endif

CMemBuffer::~CMemBuffer()
{
	Release();
}

void CMemBuffer::Init()
{
	m_pbyBuffer = NULL;
	SetDataSize(0);
	SetTrueDataSize(0);
	SetMinDataSize(0);
}

LPBYTE CMemBuffer::AllocateBuffer(DWORD dwDataSize, DWORD dwFlags)
{	
	// Round up to the nearest chunk (gives room for growth without reallocation)
	DWORD dwMinSize = MEMBUFFER_GET_MIN_SIZE(dwFlags);	
	DWORD dwAllocSize = 0;

	if (dwMinSize)
	{
		SetMinDataSize( dwMinSize);

		if( dwFlags & MEMBUFFER_FLAG_MIN_DOUBLE_FACTOR )
		{
			dwAllocSize = ((dwDataSize + (dwMinSize * 2) - 1) & ~(dwMinSize - 1));
		}
		else
		{
			dwAllocSize = ((dwDataSize + dwMinSize - 1) & ~(dwMinSize - 1));
		}
	}
	else
	{
		dwAllocSize = dwDataSize;
	}

	// Only allocate if we're smaller
	if (GetTrueDataSize() < dwAllocSize)
	{
		// Clean up anything out there first
		Release();
		
		if( dwAllocSize > 0 )
		{
			m_pbyBuffer	= new BYTE[dwAllocSize];
		}

		SetTrueDataSize(dwAllocSize);
	}	

	SetDataSize(dwDataSize);

	if( (dwFlags & MEMBUFFER_FLAG_ZEROMEMORY) && (GetDataSize() > 0) )
	{
		ZeroMemory (m_pbyBuffer, GetDataSize());
	}

	return m_pbyBuffer;
}

BOOL CMemBuffer::Release()
{
	if (m_pbyBuffer)
	{
		delete []m_pbyBuffer;
		m_pbyBuffer = NULL;
	}

	SetDataSize(0);
	SetTrueDataSize(0);

	return TRUE;
}

LPBYTE CMemBuffer::GetBuffer() const
{	
	if( GetDataSize() != 0 )
	{
		return m_pbyBuffer;
	}
	return NULL;	
}

LPBYTE CMemBuffer::GetBuffer(DWORD dwDataSize, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */)
{	
	return AllocateBuffer(dwDataSize, dwFlags);
}

char * CMemBuffer::GetBufferAsString( DWORD dwFlags /* = 0 */ )
{
	if( GetDataSize() > 0 )
	{
		// Check to see if the NULL terminator is already there
		if( GetBuffer()[GetDataSize() - 1] != 0 )
		{
			// See if we have enough room at the end to set the NULL terminator
			if( GetDataSize() != GetTrueDataSize() )
			{
				GetBuffer()[GetDataSize()] = 0;
				return reinterpret_cast< char * >( GetBuffer() );
			}
			else
			{			
				// Need to grow the buffer so we can write out the NULL
				if( AdjustBufferAtPosition( GetDataSize(), 0, max( GetMinDataSize(), 1 ) ) )
				{
					GetBuffer()[GetDataSize() - 1] = 0;
					return reinterpret_cast< char * >( GetBuffer() );
				}
			}
		}
		else
		{
			return reinterpret_cast< char * >( GetBuffer() );
		}
	}

	return NULL;	
}

void CMemBuffer::SetBuffer( LPBYTE pbyData, DWORD dwDataLength )
{
	assert( pbyData);

	LPBYTE pbyLocalData = GetBuffer( dwDataLength );

	if( pbyLocalData )
	{
		memcpy( pbyLocalData, pbyData, dwDataLength );
	}
}

void CMemBuffer::SetBuffer( const char *szBuffer, DWORD dwDataLength )
{
	assert( szBuffer );

	LPBYTE pbyLocalData = GetBuffer( dwDataLength );

	if( pbyLocalData )
	{
		memcpy( pbyLocalData, szBuffer, dwDataLength );
	}
}

void CMemBuffer::CopyBuffer( CMemBuffer &mbCopy, DWORD dwBeginPosition /* = 0 */, DWORD dwEndPosition /* = 0 */ ) const 
{
	DWORD dwDataSize = 0;
	DWORD dwMinSize = GetMinDataSize();
	LPBYTE pbyCopyBuffer = NULL;
	LPBYTE pbySourceBuffer = GetBuffer();
	
	if( 
		((dwBeginPosition != 0) && (dwEndPosition != 0)) &&
		(dwBeginPosition < dwEndPosition) &&
		(dwEndPosition <= GetDataSize())
	)
	{
		dwDataSize = dwEndPosition - dwBeginPosition;
		pbySourceBuffer += dwBeginPosition;
	}
	else
	{
		dwDataSize = GetDataSize();		
	}	

	pbyCopyBuffer = mbCopy.GetBuffer( dwDataSize, dwMinSize );	

	if( pbyCopyBuffer )
	{		
		memcpy( pbyCopyBuffer, pbySourceBuffer, dwDataSize );
	}
}

void CMemBuffer::CopyBuffer( CMemBuffer *pmbCopy, DWORD dwBeginPosition /* = 0 */, DWORD dwEndPosition /* = 0 */ ) const 
{
	assert (pmbCopy);
	DWORD dwDataSize = 0;
	DWORD dwMinSize = GetMinDataSize();
	LPBYTE pbyCopyBuffer = NULL;
	LPBYTE pbySourceBuffer = GetBuffer();
	
	if( 
		((dwBeginPosition != 0) && (dwEndPosition != 0)) &&
		(dwBeginPosition < dwEndPosition) &&
		(dwEndPosition <= GetDataSize())
	)
	{
		dwDataSize = dwEndPosition - dwBeginPosition;
		pbySourceBuffer += dwBeginPosition;
	}
	else
	{
		dwDataSize = GetDataSize();		
	}	

	pbyCopyBuffer = pmbCopy->GetBuffer( dwDataSize, dwMinSize );	

	if( pbyCopyBuffer )
	{		
		memcpy( pbyCopyBuffer, pbySourceBuffer, dwDataSize );
	}	
}


#ifdef __AFX_H__

LPBYTE CMemBuffer::InitFromFile( CFile *fp, DWORD dwFlags /* = 0 */)
{
	assert( fp );
	DWORD dwDataSize = fp->GetLength();
	LPBYTE pbyBuffer = NULL;

	if (dwDataSize > 0)
	{
		pbyBuffer = AllocateBuffer( dwDataSize, dwFlags);
	
		if (pbyBuffer)
		{
			if ( fp->Read( pbyBuffer, dwDataSize) != dwDataSize)
			{
				Release();
				return NULL;
			}
		}
	}

	return pbyBuffer;
}

LPBYTE CMemBuffer::InitFromFile( LPCTSTR szFileName, DWORD dwFlags /* = 0 */)
{
	assert( szFileName );
	CFile fp;
	CFileException e;
	LPBYTE pbyBuffer = NULL;
		
	if( fp.Open( szFileName, CFile::modeRead | CFile::shareExclusive | CFile::typeBinary, &e ) )
	{
		if( fp.GetLength() > 0 )
		{			
			pbyBuffer = InitFromFile( &fp, dwFlags);
		}
		fp.Close();
	}

	return pbyBuffer;
}

LPBYTE CMemBuffer::InitFromFile(std::string& sFileName, DWORD dwFlags /* = 0 */)
{
	return InitFromFile(sFileName.c_str(), dwFlags);
}

LPBYTE CMemBuffer::InitFromFile(std::stringstream& sFileName, DWORD dwFlags /* = 0 */)
{
	return InitFromFile(sFileName.str().c_str(), dwFlags);
}

BOOL CMemBuffer::AddFile(const LPCTSTR szFileName, DWORD dwFlags /* = 0 */)
{
	assert(szFileName);
	CMemBuffer mbData;

	if (mbData.InitFromFile(szFileName, dwFlags))
	{
		AppendBuffer(&mbData, dwFlags);
		return true;
	}
	return false;
}

BOOL CMemBuffer::WriteToFile(LPCSTR szFileName, DWORD dwFlags /* = MEMBUFFER_FLAG_WRITEFILE_OVERWRITE */)
{
	// Convert to wide character format to pass along to existing function
	wstring wFileName;
	string sFileName(szFileName);
	wFileName.assign(sFileName.begin(), sFileName.end());
	return WriteToFile(wFileName);
}

BOOL CMemBuffer::WriteToFile(const std::wstring& sFileName, DWORD dwFlags /*= MEMBUFFER_FLAG_WRITEFILE_OVERWRITE*/)
{
	return WriteToFile(sFileName.c_str(), dwFlags);
}

BOOL CMemBuffer::WriteToFile(const std::wstringstream& ssFileName, DWORD dwFlags /*= MEMBUFFER_FLAG_WRITEFILE_OVERWRITE*/)
{
	return WriteToFile(ssFileName.str().c_str(), dwFlags);
}

BOOL CMemBuffer::WriteToFile(const wchar_t *szFileName, DWORD dwFlags /*= MEMBUFFER_FLAG_WRITEFILE_OVERWRITE*/)
{
	BOOL bReturn = TRUE;
	FILE *fp = NULL;
	std::filesystem::path source = szFileName;
	std::error_code ec;
	if (dwFlags & MEMBUFFER_FLAG_WRITEFILE_SKIP_IF_EXISTS)
	{
		if (std::filesystem::exists(source, ec))
		{
			return bReturn;
		}
	}
	
	_wfopen_s(&fp, szFileName, L"wb");

	if (!fp)
	{
		bReturn = FALSE;
	}
	else
	{
		if (fwrite(GetBuffer(), 1, GetDataSize(), fp) != GetDataSize())
		{
			bReturn = FALSE;
		}
		fclose(fp);
	}

	return bReturn;
}

#else		// #ifdef __AFX_H__

LPBYTE CMemBuffer::InitFromFile( FILE *fp, DWORD dwFlags /* = 0 */)
{
	assert( fp );
	int		pos				= 0;
	DWORD	dwDataSize		= 0;
	LPBYTE	pbyBuffer		= NULL;

	pos = ftell (fp);
	fseek (fp, 0, SEEK_END);
	dwDataSize = ftell (fp);
	fseek (fp, pos, SEEK_SET);	
	
	if( dwDataSize > 0 )
	{
		pbyBuffer = AllocateBuffer( dwDataSize, dwFlags );
	
		if (pbyBuffer)
		{
			if( fread( pbyBuffer, 1, dwDataSize, fp ) != dwDataSize)
			{
				Release();
				return NULL;
			}
		}
	}

	return pbyBuffer;
}

LPBYTE CMemBuffer::InitFromFile( const char *szFileName, DWORD dwFlags /* = 0 */)
{
	assert( szFileName);
	FILE *fp = NULL;
	LPBYTE pbyBuffer = NULL;

	fp = fopen( szFileName, "rb" );
	if( fp )
	{	
		pbyBuffer = InitFromFile( fp, dwFlags);
		fclose( fp );
	}		
	
	return pbyBuffer;
}
bool CMemBuffer::AddFile(const char *szFileName, DWORD dwFlags /* = 0 */)
{
	assert(szFileName);
	CMemBuffer mbData;

	if (mbData.InitFromFile(szFileName, dwFlags))
	{
		AppendBuffer(&mbData, dwFlags);
		return true;
	}
	return false;
}

BOOL CMemBuffer::WriteToFile(const char *szFileName)
{
	BOOL bReturn = TRUE;
	FILE *fp = NULL;
	fopen_s(&fp, szFileName, "wb");

	if (!fp)
	{
		bReturn = FALSE;
	}
	else
	{
		if (fwrite(GetBuffer(), 1, GetDataSize(), fp) != GetDataSize())
		{
			bReturn = FALSE;
		}
		fclose(fp);
	}

	return bReturn;
}
#endif		// #ifdef __AFX_H__

BOOL CMemBuffer::IncreaseBufferSize( DWORD dwInsertLength )
{
	DWORD dwOldDataSize = GetDataSize();
	DWORD dwOldTrueDataSize = GetTrueDataSize();

	// Only increase the buffer size if this addition takes us over the true data size,
	// since we may have built in some fudge factor space
	if( (dwOldDataSize + dwInsertLength) >= dwOldTrueDataSize )
	{
		return AdjustBufferAtPosition( dwOldDataSize, 0, dwInsertLength );
	}
	else
	{
		SetDataSize( dwOldDataSize  + dwInsertLength );
	}
	return TRUE;
}

BOOL CMemBuffer::AdjustBufferAtPosition( DWORD dwBeginPosition, DWORD dwRemoveLength, DWORD dwInsertLength)
{
	// We might have to adjust the buffer size
	DWORD dwOldDataSize = GetDataSize();
	DWORD dwOldTrueDataSize = GetTrueDataSize();
	DWORD dwEndPosition = dwBeginPosition + dwRemoveLength;
	BOOL bNewAlloc = FALSE;
	DWORD dwNewDataSize = 0;
	DWORD dwGrowSize = 0;
	
	if (dwInsertLength > dwRemoveLength)
	{
		dwGrowSize = (dwInsertLength - dwRemoveLength);
		dwNewDataSize = dwGrowSize + dwOldDataSize;
		bNewAlloc = (dwNewDataSize > dwOldTrueDataSize);
	}	
	
	if (bNewAlloc)
	{
		DWORD dwMinSize = GetMinDataSize();
		DWORD dwAllocSize = 0;
		if (dwMinSize)
		{			
			dwAllocSize = ((dwNewDataSize + dwMinSize - 1) & ~(dwMinSize - 1));
		}
		else
		{
			dwAllocSize = dwNewDataSize;
		}
		
		LPBYTE pbyNewBuffer = new BYTE[dwAllocSize];

		if (!pbyNewBuffer)
		{
			return FALSE;
		}

		// Time to move over the existing data
		LPBYTE pbyOldBuffer = GetBuffer();
		
		memcpy (pbyNewBuffer, pbyOldBuffer, dwBeginPosition);
		memcpy (pbyNewBuffer + dwBeginPosition + dwInsertLength, pbyOldBuffer + dwEndPosition, dwOldDataSize - dwEndPosition);
		
		// Free the existing, and reset the sizes
		Release();
		SetDataSize(dwNewDataSize);
		SetTrueDataSize(dwAllocSize);

		SetBuffer( pbyNewBuffer);		
	}
	else
	{
		// See how much space we have free for copying things around
		DWORD dwFreeSpace = dwOldTrueDataSize - dwOldDataSize;
		DWORD dwMoveSize = dwOldDataSize - dwEndPosition;
		LPBYTE pbyBuffer = GetBuffer();

		if (dwMoveSize > dwFreeSpace)
		{
			// Drat, we don't have enough room to move in-place.  Gotta make a temporary spot and throw it there
			LPBYTE pbyTempBuffer = new BYTE[dwMoveSize];
			
			if (!pbyTempBuffer)
			{
				return FALSE;
			}

			// Copy to the temp spot
			memcpy (pbyTempBuffer, pbyBuffer + dwEndPosition, dwMoveSize);

			// Copy back to the new spot
			memcpy (pbyBuffer + dwBeginPosition + dwInsertLength, pbyTempBuffer, dwMoveSize);

			// Wipe out
			delete []pbyTempBuffer;
			pbyTempBuffer = NULL;
		}
		else
		{
			// Copy to the free space
			memcpy (pbyBuffer + dwOldDataSize, pbyBuffer + dwEndPosition, dwMoveSize);

			// Copy back to the new spot
			MoveMemory (pbyBuffer + dwBeginPosition + dwInsertLength, pbyBuffer + dwOldDataSize, dwMoveSize);
		}

		SetDataSize((dwOldDataSize - dwRemoveLength) + dwInsertLength);
	}

	return TRUE;
}

void CMemBuffer::ReplaceString(const wchar_t *szReplace, const wchar_t *szNewString, DWORD dwFlags /* = 0 */)
{
	// This is an unfortunate hack; the MemBuffer class needs to be completely overhauled to work with files encoded
	// in something other than UTF-8 (e.g. UCS-2).  Here, we assume the data can be represented by UTF-8
	// and force the conversion.
	std::string sReplace;
	sReplace = CW2A(szReplace);
	std::string sNewString;
	sNewString = CW2A(szNewString);
	return ReplaceString(sReplace.c_str(), sNewString.c_str(), dwFlags);
}

void CMemBuffer::ReplaceString( const char *szReplace, const char *szNewString, DWORD dwFlags /* = 0 */)
{
	assert( szReplace );
	assert( szNewString );
	
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwMask			= 0;
	
	UINT j					= 0;
	UINT iStringLength		= strlen(szReplace);
	UINT iNewStringLength	= strlen(szNewString);
	std::string strCompare;	

	strCompare				= szReplace;
	char *szCompare			= new char[iStringLength + 1];	
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= szReplace[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatch = GetLowerDWORD( dwMatch, true );
	}

	while ((dwPosition + iStringLength) <= dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatch )		
		{			
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLength + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLength))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strCompare == szCompare )
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if( !_stricmp( szCompare, strCompare.c_str() ))
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
		}

		if (dwEndPosition > 0)
		{
			if (iStringLength == iNewStringLength)
			{
				// We can just out-right replace it
				memcpy( pbyBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
			}
			else
			{
				// Need to adjust the size/positioning
				if ( AdjustBufferAtPosition( dwBeginPosition, iStringLength, iNewStringLength))
				{
					if( iNewStringLength > 0 )
					{
						pbyBuffer = GetBuffer();
						memcpy (pbyBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
					}
				}
			}

			dwBufferSize = GetDataSize();
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_FIRST_ONLY)
			{
				// Just set this so we break out
				dwPosition = dwBufferSize; 
				continue;
			}
			else
			{						
				dwPosition = dwBeginPosition + iNewStringLength;

				dwEndPosition = 0;
				dwBeginPosition = 0;
			}
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}

void CMemBuffer::AppendString( const char *szString, DWORD dwFlags /* = 0 */ )
{
	assert( szString );
	
	LPBYTE pbyBuffer = NULL;
	DWORD dwLength = strlen( szString );
	DWORD dwPosition = GetDataSize();
	IncreaseBufferSize( dwLength );	

	pbyBuffer = GetBuffer();
	memcpy( pbyBuffer + dwPosition, (LPBYTE)szString, dwLength );		
}

void CMemBuffer::AppendBuffer( LPBYTE pbyData, DWORD dwDataLength, DWORD dwFlags /* = 0 */ )
{
	assert( pbyData );
	DWORD dwPosition = GetDataSize();	
	IncreaseBufferSize( dwDataLength );	

	LPBYTE pbyBuffer = GetBuffer();
	memcpy( pbyBuffer + dwPosition, pbyData, dwDataLength );
}

void CMemBuffer::AppendBuffer( CMemBuffer *pBuffer, DWORD dwFlags /* = 0 */ )
{
	assert( pBuffer );
	
	LPBYTE pbyBuffer = NULL;
	DWORD dwLength = pBuffer->GetDataSize();
	DWORD dwPosition = GetDataSize();

	if( AdjustBufferAtPosition( dwPosition, 0, dwLength ))
	{
		pbyBuffer = GetBuffer();
		memcpy( pbyBuffer + dwPosition, pBuffer->GetBuffer(), dwLength );		
	}
}

void CMemBuffer::InsertString( const char *szMatch, const char *szNewString, DWORD dwFlags /* = MEMBUFFER_FLAG_INSERT_AFTER */)
{
	assert (szMatch);
	assert (szNewString);
	
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwMask			= 0;
	
	UINT j					= 0;
	UINT iStringLength		= strlen(szMatch);
	UINT iNewStringLength	= strlen(szNewString);
	std::string	strCompare;
		
	strCompare = szMatch;
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= szMatch[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatch = GetLowerDWORD( dwMatch, true );
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatch )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLength + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLength))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strCompare == szCompare )				
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if( !_stricmp( strCompare.c_str(), szCompare ))
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
		}

		if (dwEndPosition > 0)
		{			
			// This makes sure we only have one type of insert ("after" has the preference)
			DWORD dwInsertType = 0;
			if (dwFlags & MEMBUFFER_FLAG_INSERT_BEFORE)
			{
				dwInsertType = MEMBUFFER_FLAG_INSERT_BEFORE;
			}

			if (dwFlags & MEMBUFFER_FLAG_INSERT_AFTER)
			{
				dwInsertType = MEMBUFFER_FLAG_INSERT_AFTER;
			}
			
			// Need to adjust the size/positioning
			switch (dwInsertType)
			{
			case MEMBUFFER_FLAG_INSERT_BEFORE:
				{
					if ( AdjustBufferAtPosition( dwBeginPosition, 0, iNewStringLength))
					{
						pbyBuffer = GetBuffer();
						memcpy (pbyBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
					}
				}
				break;

			case MEMBUFFER_FLAG_INSERT_AFTER:
				{
					if ( AdjustBufferAtPosition( dwEndPosition, 0, iNewStringLength))
					{
						pbyBuffer = GetBuffer();
						memcpy (pbyBuffer + dwEndPosition, (LPBYTE)szNewString, iNewStringLength);
					}				
				}
				break;

			default:
				assert( false );
				break;
			}

			dwBufferSize = GetDataSize();
			dwPosition = dwBeginPosition + iStringLength + iNewStringLength;

			if (dwFlags & MEMBUFFER_FLAG_MATCH_FIRST_ONLY)
			{
				// Just set this so we break out
				dwPosition = dwBufferSize; 
				continue;
			}
			else
			{
				dwEndPosition = 0;
				dwBeginPosition = 0;
			}
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}

void CMemBuffer::InsertBuffer( const char *szMatch, CMemBuffer &mbInsertBlock, DWORD dwFlags /* = MEMBUFFER_FLAG_INSERT_AFTER */)
{
	// Just add a NULL terminator at the end and treat it like a string
	LPBYTE pbyInsertBlock = mbInsertBlock.GetBuffer();
	LPBYTE pbyNewBlock = NULL;
	DWORD dwInsertBlockSize = mbInsertBlock.GetDataSize();
	CMemBuffer mbNewBlock;

	if( dwInsertBlockSize == 0 )
	{
		return;
	}

	if (pbyInsertBlock)
	{
		if (pbyInsertBlock[dwInsertBlockSize - 1] == 0)
		{
			InsertString( szMatch, (LPSTR)pbyInsertBlock, dwFlags);
		}
		else
		{
			// Do we have room to write the NULL terminator?
			if( mbInsertBlock.GetTrueDataSize() > mbInsertBlock.GetDataSize() )
			{
				pbyInsertBlock[dwInsertBlockSize - 1] = 0;
			}
			else
			{
				// Make a temporary buffer with a NULL terminator				
				pbyNewBlock = mbNewBlock.GetBuffer( dwInsertBlockSize + 1 );
				memcpy( pbyNewBlock, pbyInsertBlock, dwInsertBlockSize );
				pbyInsertBlock = pbyNewBlock;
			}

			if( pbyInsertBlock )
			{				
				InsertString( szMatch, (LPSTR)pbyInsertBlock, dwFlags);
			}
		}
	}
}

void CMemBuffer::RemoveBlock( const char *szMatch, DWORD dwFlags /* = 0 */)
{
	assert( szMatch);

	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatchBegin		= 0;
	DWORD dwBeginPosition	= 0;
	//LPDWORD lpdwBeginPosition	= NULL;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwMask			= 0;
	
	UINT j					= 0;
	UINT iStringLengthMatch	= strlen(szMatch);
	std::string strCompareBegin;
	
	strCompareBegin = szMatch;
	char *szCompare			= new char[iStringLengthMatch + 1];
	bool bAdjust = false;
	
	for (j = 0; j < min( iStringLengthMatch, 4); j++)
	{
		dwMatchBegin |= szMatch[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatchBegin = GetLowerDWORD( dwMatchBegin, true );
	}
	
	while (dwPosition + 4 < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);
		bAdjust = false;
		
		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchBegin )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLengthMatch + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLengthMatch))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strCompareBegin == szCompare )
				{
					dwBeginPosition = dwPosition;
					bAdjust = true;
				}
			}
			else
			{
				if( !_stricmp( strCompareBegin.c_str(), szCompare ))
				{
					dwBeginPosition = dwPosition;
					bAdjust = true;
				}
			}
		}	

		if( bAdjust )
		{
			// We have to adjust the buffer size			
			AdjustBufferAtPosition( dwBeginPosition, iStringLengthMatch, 0);
			
			pbyBuffer = GetBuffer();
			dwBufferSize = GetDataSize();			

			if (dwFlags & MEMBUFFER_FLAG_MATCH_FIRST_ONLY)
			{
				// Just set this so we break out
				dwPosition = dwBufferSize; 
				continue;
			}
			else
			{
				dwPosition = dwBeginPosition;
			}
		}
		else
		{		
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}

void CMemBuffer::GetStringBlock( const char *szBegin, const char *szEnd, CMemBuffer &mbNewBlock, DWORD dwFlags /* = 0 */)
{
	assert (szBegin);
	assert (szEnd);
	
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatchBegin		= 0;
	DWORD dwMatchEnd		= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBeginMask		= 0;
	DWORD dwEndMask			= 0;
	LPDWORD lpdwBeginPosition	= NULL;
	LPDWORD lpdwEndPosition		= NULL;
	DWORD dwBufferSize		= GetDataSize();
	
	UINT j					= 0;
	UINT iStringLengthBegin	= strlen(szBegin);
	UINT iStringLengthEnd	= strlen(szEnd);
	UINT iMaxStringLength	= max (iStringLengthBegin, iStringLengthEnd);
	std::string strCompareBegin;	
	std::string strCompareEnd;
		
	strCompareBegin = szBegin;
	strCompareEnd = szEnd;
	char *szCompare			= new char[iMaxStringLength + 1];
	
	for (j = 0; j < min( iStringLengthBegin, 4); j++)
	{
		dwMatchBegin |= szBegin[j] << (j * 8);
		dwBeginMask |= 0xff << (j * 8);
	}

	for (j = 0; j < min( iStringLengthEnd, 4); j++)
	{
		dwMatchEnd |= szEnd[j] << (j * 8);
		dwEndMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatchBegin = GetLowerDWORD( dwMatchBegin, true );
		dwMatchEnd = GetLowerDWORD( dwMatchEnd, true );
	}

	while (dwPosition + 4 < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if (!lpdwBeginPosition)
		{
			if( GetLowerDWORD( ((*lpdwCompare) & dwBeginMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchBegin )
			{
				// Found a possible match; compare against the full string
				memset( szCompare, 0, iMaxStringLength + 1);
				DWORD dwCompare = dwPosition;
				DWORD x = 0;

				while ((dwCompare < dwBufferSize) && (x < iStringLengthBegin))
				{
					szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
				}
				
				if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
				{
					if( strCompareBegin == szCompare )
					{
						dwBeginPosition = dwPosition + iStringLengthBegin;						
						lpdwBeginPosition = &dwBeginPosition;
					}
				}
				else
				{
					if( !_stricmp( strCompareBegin.c_str(), szCompare ) )
					{
						dwBeginPosition = dwPosition + iStringLengthBegin;						
						lpdwBeginPosition = &dwBeginPosition;
					}
				}
			}
		}

		if (!lpdwEndPosition)
		{
			if( GetLowerDWORD( ((*lpdwCompare) & dwEndMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchEnd )
			{
				// Found a possible match; compare against the full string
				memset( szCompare, 0, iMaxStringLength + 1);
				DWORD dwCompare = dwPosition;
				DWORD x = 0;

				while ((dwCompare < dwBufferSize) && (x < iStringLengthEnd))
				{
					szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
				}
				
				if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
				{
					if( strCompareEnd == szCompare )
					{
						dwEndPosition = dwPosition;
						lpdwEndPosition = &dwEndPosition;
					}
				}
				else
				{
					if( !_stricmp( strCompareEnd.c_str(), szCompare ))
					{
						dwEndPosition = dwPosition;
						lpdwEndPosition = &dwEndPosition;
					}
				}
			}
		}

		if (lpdwBeginPosition && lpdwEndPosition)
		{
			if (dwBeginPosition < dwEndPosition)
			{				
				// We have to adjust the buffer size
				DWORD dwRemoveLength = dwEndPosition - dwBeginPosition;

				if (dwRemoveLength > 0)
				{
					LPBYTE pbyBlockBuffer = mbNewBlock.GetBuffer( dwRemoveLength, dwFlags);

					if (pbyBlockBuffer)
					{
						memcpy (
							pbyBlockBuffer, 
							pbyBuffer + dwBeginPosition,
							dwRemoveLength);
					}					
				}
		
				lpdwBeginPosition = NULL;
				lpdwEndPosition = NULL;

				pbyBuffer = GetBuffer();
				dwBufferSize = GetDataSize();

				dwPosition = dwBeginPosition;
				
				// Just set this so we break out.  We don't support multiple matches, because 
				// we're only getting one CMemBuffer pointer coming in (they'd override otherwise)					
				dwPosition = dwBufferSize; 
				continue;				
			}
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}

void CMemBuffer::MoveStringBlock( const char *szBegin, const char *szEnd, CMemBuffer *pmbNewBlock, DWORD dwFlags /* = 0 */)
{
	assert (szBegin);
	assert (szEnd);
	assert (pmbNewBlock);
	
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatchBegin		= 0;	
	DWORD dwBeginMask		= 0;
	DWORD dwMatchEnd		= 0;
	DWORD dwEndMask			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	LPDWORD lpdwBeginPosition	= NULL;
	LPDWORD lpdwEndPosition		= NULL;
	DWORD dwBufferSize		= GetDataSize();
	
	UINT j					= 0;
	UINT iStringLengthBegin	= strlen(szBegin);
	UINT iStringLengthEnd	= strlen(szEnd);
	UINT iMaxStringLength	= max (iStringLengthBegin, iStringLengthEnd);
	std::string strCompareBegin;	
	std::string strCompareEnd;
		
	strCompareBegin = szBegin;
	strCompareEnd = szEnd;

	char *szCompare			= new char[iMaxStringLength + 1];
	
	for (j = 0; j < min( iStringLengthBegin, 4); j++)
	{
		dwMatchBegin |= szBegin[j] << (j * 8);
		dwBeginMask |= 0xff << (j * 8);
	}

	for (j = 0; j < min( iStringLengthEnd, 4); j++)
	{
		dwMatchEnd |= szEnd[j] << (j * 8);
		dwEndMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatchBegin = GetLowerDWORD( dwMatchBegin, true );
		dwMatchEnd = GetLowerDWORD( dwMatchEnd, true );
	}

	while (dwPosition + 4 < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if (!lpdwBeginPosition)
		{
			if( GetLowerDWORD( ((*lpdwCompare) & dwBeginMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchBegin )			
			{
				// Found a possible match; compare against the full string
				memset( szCompare, 0, iMaxStringLength + 1);
				DWORD dwCompare = dwPosition;
				DWORD x = 0;

				while ((dwCompare < dwBufferSize) && (x < iStringLengthBegin))
				{
					szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
				}
				
				if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
				{
					if( strCompareBegin == szCompare )
					{
						if (dwFlags & MEMBUFFER_FLAG_MOVE_LEAVE_TAGS)
						{
							dwBeginPosition = dwPosition + iStringLengthBegin;
						}
						else
						{
							dwBeginPosition = dwPosition;
						}
						lpdwBeginPosition = &dwBeginPosition;
					}
				}
				else
				{
					if( !_stricmp( strCompareBegin.c_str(), szCompare ))
					{
						if (dwFlags & MEMBUFFER_FLAG_MOVE_LEAVE_TAGS)
						{
							dwBeginPosition = dwPosition + iStringLengthBegin;
						}
						else
						{
							dwBeginPosition = dwPosition;
						}
						lpdwBeginPosition = &dwBeginPosition;
					}
				}
			}
		}

		if (!lpdwEndPosition)
		{
			if( GetLowerDWORD( ((*lpdwCompare) & dwEndMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchEnd )			
			{
				// Found a possible match; compare against the full string
				memset( szCompare, 0, iMaxStringLength + 1);
				DWORD dwCompare = dwPosition;
				DWORD x = 0;

				while ((dwCompare < dwBufferSize) && (x < iStringLengthEnd))
				{
					szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
				}
				
				if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
				{
					if( strCompareEnd == szCompare )
					{
						if (dwFlags & MEMBUFFER_FLAG_MOVE_LEAVE_TAGS)
						{
							dwEndPosition = dwPosition;							
						}
						else
						{
							dwEndPosition = dwPosition + iStringLengthEnd;
						}
						lpdwEndPosition = &dwEndPosition;
					}
				}
				else
				{
					if( !_stricmp( strCompareEnd.c_str(), szCompare ))
					{
						if (dwFlags & MEMBUFFER_FLAG_MOVE_LEAVE_TAGS)
						{
							dwEndPosition = dwPosition;							
						}
						else
						{
							dwEndPosition = dwPosition + iStringLengthEnd;
						}
						lpdwEndPosition = &dwEndPosition;
					}
				}
			}
		}

		if (lpdwBeginPosition && lpdwEndPosition)
		{
			if (dwBeginPosition < dwEndPosition)
			{				
				// We have to adjust the buffer size
				DWORD dwRemoveLength = dwEndPosition - dwBeginPosition;

				if (dwRemoveLength > 0)
				{
					if (pmbNewBlock)
					{
						LPBYTE pbyBlockBuffer = pmbNewBlock->GetBuffer( dwRemoveLength, dwFlags);

						if (pbyBlockBuffer)
						{
							memcpy (
								pbyBlockBuffer, 
								pbyBuffer + dwBeginPosition,
								dwRemoveLength);
						}
					}
				}
				
				AdjustBufferAtPosition( dwBeginPosition, dwRemoveLength, 0);

				lpdwBeginPosition = NULL;
				lpdwEndPosition = NULL;

				pbyBuffer = GetBuffer();
				dwBufferSize = GetDataSize();

				dwPosition = dwBeginPosition;
				
				if (pmbNewBlock)
				{
					// Just set this so we break out.  We don't support multiple matches, because 
					// we're only getting one CMemBuffer pointer coming in (they'd override otherwise)					
					dwPosition = dwBufferSize; 
					continue;
				}				
			}
			else
			{
				// TODO: what happens when the begin match is found AFTER the end match?  Switch the position of the pointers?
				throw std::exception("MEMBUFFER FAILURE");
			}
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}


BOOL CMemBuffer::GetGUID( std::string &strGUID )
{
	CoInitialize( NULL );
	GUID guid;
	wchar_t szGUID[39];
	_bstr_t bsGUID = _T("");
	std::string strTemp;
	
	HRESULT hr = CoCreateGuid( &guid );
	if( FAILED(hr) )
	{
		return FALSE;
	}

	if( !::StringFromGUID2( guid, szGUID, 39 ) )
	{		
		return FALSE;
	}
	
	bsGUID = szGUID;	
	strTemp = (char *)bsGUID;
	Trim( strTemp, '{' );
	Trim( strTemp, '}' );

	// This marker will turn the GUID into a comment in LUA, just in case
	// the caller doesn't remember to replace the string
	strGUID = "--";
	strGUID += strTemp;

	// Add a very unique string to the end, to even further guarantee uniqueness
	strGUID += "--##--LUA_GUID--##--";

	return TRUE;
}

void CMemBuffer::GetPragmas( _PragmaVector &vcPragmas, DWORD dwFlags /* = 0 */ )
{
	// This function will pull out all of the @@pragma statements within the buffer, and 
	// replace the text of that section with a comment-prefixed GUID.  The purpose of the GUID
	// is so we can later identify it 
	LPBYTE pbyBuffer		= GetBuffer();
	DWORD dwPosition		= 1;	// Always start at 1, since pragmas begin with @@
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwCompare			= 0;
	DWORD x					= 0;
	BOOL bInQuoteString		= FALSE;
	BOOL bInDblQuoteString	= FALSE;
	BOOL bInExtString		= FALSE;
	BOOL bAdvance			= FALSE;
	UINT j					= 0;
	PRAGMA_STRUCT PragmaStruct;
		
	while( dwPosition < dwBufferSize )
	{
		bAdvance = TRUE;
		if( (!bInQuoteString) && (!bInDblQuoteString) && (!bInExtString) )
		{
			if( (pbyBuffer[dwPosition] == '@') && (pbyBuffer[dwPosition - 1] == '@') )
			{
				// Pragma line here
				dwBeginPosition = dwPosition - 1;
				PragmaStruct.strType = "";
				PragmaStruct.strValue = "";

				// Read up until the first space
				while( 
					(++dwPosition < dwBufferSize) && 
					( pbyBuffer[dwPosition] != ' ' )
				)
				{
					PragmaStruct.strType += pbyBuffer[dwPosition];
					//reinterpret_cast< const char *>( pbyBuffer + dwPosition );
				}

				// Now read everything up until the end of the line
				while( 
					(++dwPosition < dwBufferSize) && 
					( pbyBuffer[dwPosition] != 0x0d )
				)
				{
					PragmaStruct.strValue += pbyBuffer[dwPosition];
				}

				// Read past all of the EOL stuff
				while( 
					(++dwPosition < dwBufferSize) && 
					( ( pbyBuffer[dwPosition] == 0x0d ) ||
					  ( pbyBuffer[dwPosition] == 0x0a ) )
				);

#if 0
				{
					PragmaStruct.strValue += pbyBuffer[dwPosition];
				}
#endif

				Trim( PragmaStruct.strType, '"' );
				Trim( PragmaStruct.strType, '\'' );
				Trim( PragmaStruct.strValue, '"' );
				Trim( PragmaStruct.strValue, '\'' );

				if( GetGUID( PragmaStruct.strGUID ) )
				{
					if( AdjustBufferAtPosition( dwBeginPosition, dwPosition - dwBeginPosition, PragmaStruct.strGUID.length() ) )
					{
						dwBufferSize = GetDataSize();
						pbyBuffer = GetBuffer();

						// Write in our GUID 
						memcpy( pbyBuffer + dwBeginPosition, PragmaStruct.strGUID.c_str(), PragmaStruct.strGUID.length() );
						vcPragmas.push_back( PragmaStruct );

						// Reset position and buffer size
						dwPosition = dwBeginPosition + PragmaStruct.strGUID.length();						
					}
				}

				bAdvance = FALSE;
			}
			else
			{
				if( pbyBuffer[dwPosition] == '"' )
				{
					bInDblQuoteString = TRUE;					
				}
				else
				{
					if( pbyBuffer[dwPosition] == '\'' )
					{
						bInQuoteString = TRUE;					
					}
					else
					{
						if( (pbyBuffer[dwPosition] == '[') && (pbyBuffer[dwPosition - 1] != '[') )
						{
							bInExtString = TRUE;						
						}
					}
				}
			}
		}
		else
		{
			// Check to see if the string is ending
			if( bInDblQuoteString )
			{				
				if( (pbyBuffer[dwPosition] == '"') && (pbyBuffer[dwPosition - 1] != '\\') )
				{
					bInDblQuoteString = FALSE;					
				}
			}
			else
			{
				if( bInQuoteString )
				{				
					if( (pbyBuffer[dwPosition] == '\'') && (pbyBuffer[dwPosition - 1] != '\\') )
					{
						bInQuoteString = FALSE;					
					}
				}
				else
				{
					if( bInExtString )
					{
						if( (pbyBuffer[dwPosition] == ']') && (pbyBuffer[dwPosition - 1] != ']') )
						{
							bInExtString = FALSE;
							
						}
					}
				}
			}
		}

		if( bAdvance )
		{
			dwPosition++;
		}		
	}	
}

std::string CMemBuffer::GetTagValue( const char *szTag, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwMask			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();

	std::string strReturn;
	std::string strTag;
	strTag = szTag;
	
	UINT j					= 0;
	UINT iStringLength		= strTag.size();	
	
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= strTag[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatch = GetLowerDWORD( dwMatch, true );
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatch )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLength + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLength))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strTag == szCompare )
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if( !_stricmp( strTag.c_str() , szCompare ) )
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
		}

		if (dwBeginPosition > 0)
		{
			while ( (dwBeginPosition < dwBufferSize) && (pbyBuffer[dwBeginPosition] != '-') )
			{
				strReturn += pbyBuffer[dwBeginPosition++];
			}

			if (szCompare)
			{
				delete []szCompare;
				szCompare = NULL;
			}
			return strReturn;
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
	return strReturn;
}

void CMemBuffer::GetAllElements( const char *szElement, _MBStringVector &vcElements, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= GetBuffer();
	CHAR *pChar				= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwCompare = dwPosition;
	DWORD x = 0;
	std::string strElement;
	std::string strHTML = "";

	strElement = szElement; 

	UINT j					= 0;
	int iStringLength		= strlen( szElement );

	char *szCompare			= new char[iStringLength + 1];
		
	while( (dwPosition + iStringLength) < dwBufferSize)
	{		
		pChar = (CHAR *)(pbyBuffer + dwPosition);

		if( (*pChar) == '<' )
		{
			// Found an element
			memset( szCompare, 0, iStringLength + 1);
			dwCompare = dwPosition;
			x = 0;

			// Skip any leading spaces (the first pass through the condition will move it off of <
			while( 
					(++dwCompare < dwBufferSize) && 
					( *( (LPSTR)(pbyBuffer + dwCompare) ) == ' ' )
			);
			
			// See if this element is one we're looking for
			while( (dwCompare < dwBufferSize) && (x < iStringLength) )
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if( dwFlags & MEMBUFFER_FLAG_MATCH_CASE )
			{
				if( strElement == szCompare )
				{
					dwBeginPosition = dwPosition + 1;
				}
			}
			else
			{
				if( !_stricmp( strElement.c_str(), szCompare ) )
				{
					dwBeginPosition = dwPosition + 1;
				}
			}
		}

		if( dwBeginPosition > 0 )
		{
			while( (dwBeginPosition < dwBufferSize) && (pbyBuffer[dwBeginPosition] != '>') )
			{
				strHTML += pbyBuffer[dwBeginPosition++];
			}

			vcElements.push_back( strHTML.c_str() );

			dwPosition = dwBeginPosition;
			dwBeginPosition = 0;
			strHTML = "";
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}	
}

std::string CMemBuffer::GetAllTagValues( const char *szTag, LPEnumSequencesCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwMask			= 0;	
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();

	std::string strReturn;
	std::string strTag;
	strTag = szTag;
	
	UINT j					= 0;
	UINT iStringLength		= strTag.size();	
	
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= strTag[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatch = GetLowerDWORD( dwMatch, true );
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatch )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLength + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLength))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strTag == szCompare )
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if( !_stricmp( strTag.c_str(), szCompare ) )
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
		}

		if (dwBeginPosition > 0)
		{
			while ( (dwBeginPosition < dwBufferSize) && (pbyBuffer[dwBeginPosition] != '-') )
			{
				strReturn += pbyBuffer[dwBeginPosition++];
			}

			lpCallBack( atol( strReturn.c_str() ), lpAppDefined);

			dwPosition = dwBeginPosition;
			dwBeginPosition = 0;
			strReturn = "";
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
	return strReturn;
}

DWORD CMemBuffer::ConvertEscapeCharacter( LPBYTE pbyData, LPBYTE pbyEndData, LPBYTE pbyCharacter)
{
	std::string strEscapeString;
	DWORD dwLength = 0;
	BOOL bDone = FALSE;

	while ( (pbyData < pbyEndData) && (!bDone) && (dwLength < 8) )
	{ 
		if ((*pbyData) == ';')
		{
			bDone = TRUE;
		}
		strEscapeString += *(pbyData++);
		dwLength++;
	}

	if( (dwLength > 1) && (strEscapeString[1] != '#') )
	{
		switch( strEscapeString[1] )
		{
		case 'a':
		case 'A':
		case 'l':
		case 'L':
		case 'g':
		case 'G':
		case 'q':
		case 'Q':
			{
				if( !_stricmp( strEscapeString.c_str(), "&amp;" ) )
				{
					(*pbyCharacter) = '&';
					return dwLength;
				}

				if( !_stricmp( strEscapeString.c_str(), "&lt;" ) )	
				{
					(*pbyCharacter) = '<';
					return dwLength;
				}

				if( !_stricmp( strEscapeString.c_str(), "&gt;" ) )	
				{
					(*pbyCharacter) = '>';
					return dwLength;
				}

				if( !_stricmp( strEscapeString.c_str(), "&apos;" ) )	
				{
					(*pbyCharacter) = '\'';
					return dwLength;
				}

				if( !_stricmp( strEscapeString.c_str(), "&quot;" ) )	
				{
					(*pbyCharacter) = '"';
					return dwLength;
				}
			}
			break;
		
		default:
			{
			}
			break;
		}		
	}
	else
	{
		if( dwLength >= 4 )
		{
			if( strEscapeString[0] == '&')
			{				
				BYTE byData = 0;
				BYTE byHex[4] = { 0, 0, 0, 0 };
				
				if( strEscapeString[2] == 'x')
				{
					int j = 3;
					int k = 0;
					while ((j < strEscapeString.size() ) && (k < 2))
					{
						if( strEscapeString[j] != ';')
						{
							byHex[k] = strEscapeString[j];
						}
						j++;
						k++;
					}

					if (byHex[0] == 0)
					{
						byHex[0] = '0';
					}

					if (byHex[1] == 0)
					{
						byHex[1] = byHex[0];
						byHex[0] = '0';
					}

					if ((byHex[0] >= 'A') && (byHex[0] <= 'F'))
					{
						byData = ((byHex[0] - 'A') + 10) * 16;
					}

					if ((byHex[0] >= 'a') && (byHex[0] <= 'f'))
					{
						byData = ((byHex[0] - 'a') + 10) * 16;
					}

					if ((byHex[0] >= '0') && (byHex[0] <= '9'))
					{
						byData = (byHex[0] - '0') * 16;
					}

					if ((byHex[1] >= 'A') && (byHex[1] <= 'F'))
					{
						byData += (byHex[1] - 'A') + 10;
					}

					if ((byHex[1] >= 'a') && (byHex[1] <= 'f'))
					{
						byData += (byHex[1] - 'a') + 10;
					}

					if ((byHex[1] >= '0') && (byHex[1] <= '9'))
					{
						byData += (byHex[1] - '0');
					}
					(*pbyCharacter) = byData;
				}
				else
				{
					int j = 2;
					int k = 0;
					while ((j < strEscapeString.size() ) && (k < 3))
					{
						if( strEscapeString[j] != ';')
						{
							byHex[k] = strEscapeString[j];
						}
						j++;
						k++;
					}

					byData = (BYTE)atoi((LPSTR)byHex);
					(*pbyCharacter) = byData;
				}
				return dwLength;				
			}
		}
	}

	// Unrecognized escape sequence... set to & and advance one character
	(*pbyCharacter) = '&';
	return 1;
}

DWORD CMemBuffer::ParseAttributes( LPBYTE pbyBuffer, DWORD dwDataSize, _AttributeMap &AttributeMap, DWORD dwFlags /* = 0 */ )
{
	// buffer will be something like this:
	// attribute="blah blah" attr1="yeah yeah" />
	// First thing is to ignore all spaces until the first non-space character is
	// found.  Then read until either a space (which would break
	// XML standards so the attribute is abandoned) or an equals sign is encountered.
	// After the equals sign, ignore everything until a quote or double-quote is
	// encountered.

	DWORD dwBytesRead = 0;
	DWORD dwPosition = 0;
	DWORD dwLineNumber = 1;
	LPBYTE pbyData = pbyBuffer;
	LPBYTE pbyEndData = pbyBuffer + dwDataSize;
	BYTE byData = 0;
	
	BOOL bAttributeDiscovery = FALSE;
	BOOL bAttributeValueDiscovery = FALSE;
	BOOL bParseFinished = FALSE;
	BOOL bInQuotes = FALSE;
	BOOL bCharConverted = FALSE;
	BOOL bEndTag = FALSE;
	BOOL bEndTagFirstCharacter = FALSE;

	BYTE byQuote = 0;
	
	std::string strElementName = "";
	std::string strElementValue = "";
	std::string strAttributeName = "";
	std::string strAttributeValue = "";	

	while ( (pbyData < pbyEndData) && (!bParseFinished))
	{
		// Check for an escape character
		if (((*pbyData) == '&') && (!bInQuotes))
		{
			// Convert to the true character, saving the length of the data to advance past
			// bInQuotes is handled in another step...
			dwPosition = ConvertEscapeCharacter( pbyData, pbyEndData, &byData );
			bCharConverted = TRUE;
		}
		else
		{
			byData = (*pbyData);
			dwPosition = 1;
			bCharConverted = FALSE;			
		}

		switch( byData )
		{
		case ' ':
			{
				if( (!bAttributeValueDiscovery) && (!bInQuotes) )
				{
					if( strAttributeName.size() > 0 )
					{
						// Add the attribute to the map
						AttributeMap.insert( _valAttribute( strAttributeName, "" ) );						
						strAttributeName = "";
					}
					bAttributeDiscovery = TRUE;
				}
				else
				{
					strAttributeValue += byData;
				}				
			}
			break;

		case 0x0a:
			// Flow through
		case 0x0d:			
			break;

		case '&':
			{
				// We're in quotes... convert to the real character
				dwPosition = ConvertEscapeCharacter( pbyData, pbyEndData, &byData );
				strAttributeValue += byData;
			}
			break;

		case '/':
			{
				if( bAttributeValueDiscovery )
				{
					strAttributeValue += byData;
				}
				else
				{
					// That's it; we're good to go
					bParseFinished = TRUE;
				}									
			}
			break;

		case '>':
			{
				if( bAttributeValueDiscovery )
				{
					// If we're not in quotes, this signals the end of the tag, otherwise it's part of the data
					if( bInQuotes )
					{
						strAttributeValue += byData;
					}
					else
					{
						bParseFinished = TRUE;
						
						// Add this attribute to the map
						AttributeMap.insert( _valAttribute( strAttributeName, strAttributeValue ) );
						bAttributeValueDiscovery = FALSE;
						strAttributeValue = "";
						strAttributeName = "";
					}
				}
				else
				{
					bParseFinished = TRUE;
				}				
			}
			break;

		case '"':		// Flow through
		case '\'':
			{					
				if( bAttributeValueDiscovery )
				{
					// Are we already in the middle of a string literal?
					if( bInQuotes )
					{
						// Check to make sure it's the same delimiter
						if( byQuote == byData )
						{								
							// Add this attribute to the map						
							AttributeMap.insert( _valAttribute( strAttributeName, strAttributeValue ) );
							bAttributeValueDiscovery = FALSE;
							bInQuotes = FALSE;
							strAttributeValue = "";
							strAttributeName = "";
						}
						else
						{
							strAttributeValue += byData;
						}
					}
					else
					{
						bInQuotes = TRUE;
						byQuote = byData;
					}
				}
			}
			break;

		case '=':
			{
				if( bAttributeDiscovery )
				{					
					bAttributeValueDiscovery = TRUE;
					bAttributeDiscovery = FALSE;		
					strAttributeValue = "";
				}
				else
				{
					if( bAttributeValueDiscovery )
					{
						strAttributeValue += byData;
					}					
				}
			}
			break;
		
		default:
			{
				if( bAttributeDiscovery )
				{
					// See if we're to smooth the parameter names
					if( dwFlags & MEMBUFFER_FLAG_PARSE_LOWER_CASE_PARAMS )
					{
						strAttributeName += tolower( byData );
					}
					else
					{
						strAttributeName += byData;
					}
				}
				else
				{
					if( bAttributeValueDiscovery )
					{
						strAttributeValue += byData;
					}					
				}
			}
			break;
		}				
		
		pbyData += dwPosition;
		dwBytesRead += dwPosition;
	}
	
	return dwBytesRead;
}

void CMemBuffer::EnumerateAndMoveBlocks( const char *szBegin, const char *szEnd, LPEnumAndMoveCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags /* = 0 */ )
{	
	_AttributeMap AttributeMap;
	LPBYTE pbyBuffer		= GetBuffer();
	LPBYTE pbyData			= NULL;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwTempFlags		= 0;
	DWORD dwPosition		= 0;
	DWORD dwMatchBegin		= 0;
	DWORD dwBeginMask		= 0;
	DWORD dwMatchEnd		= 0;
	DWORD dwEndMask			= 0;
	DWORD dwBeginPosition	= NULL;
	DWORD dwBeginDataPosition = NULL;
	DWORD dwEndPosition		= NULL;
	DWORD dwEndDataPosition = NULL;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwInsertLength	= 0;
	DWORD dwBeginTagLength	= 0;
	DWORD dwDataLength		= 0;
	DWORD dwNumLevels		= 0;
	DWORD dwCompare			= 0;
	DWORD x					= 0;
	BOOL bBeginPos = FALSE;
	BOOL bEndPos = dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG ? TRUE : FALSE;
	BOOL bLevelAdded = FALSE;
	BOOL bKeepProcessing = TRUE;
	std::string strCompareBegin;	
	std::string strCompareEnd;
	std::string strReplaceTag;

	assert (szBegin);
	strCompareBegin = szBegin;
	if( (dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG) == 0 )
	{
		assert (szEnd);
		strCompareEnd = szEnd;
	}

	CMemBuffer mbBlockData;
	
	UINT j					= 0;
	UINT iStringLengthBegin	= strlen(szBegin);
	UINT iStringLengthEnd	= dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG ? 0 : strlen(szEnd);
	UINT iMaxStringLength	= max (iStringLengthBegin, iStringLengthEnd);	

	char *szCompare			= new char[iMaxStringLength + 1];
	
	for (j = 0; j < min( iStringLengthBegin, 4); j++)
	{
		dwMatchBegin |= szBegin[j] << (j * 8);
		dwBeginMask |= 0xff << (j * 8);
	}

	for (j = 0; j < min( iStringLengthEnd, 4); j++)
	{
		dwMatchEnd |= szEnd[j] << (j * 8);
		dwEndMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatchBegin = GetLowerDWORD( dwMatchBegin, true );
		dwMatchEnd = GetLowerDWORD( dwMatchEnd, true );
	}

	while (dwPosition + 4 < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		DWORD dwTest = GetLowerDWORD( ((*lpdwCompare) & dwBeginMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 );
		
		if( dwTest == dwMatchBegin )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iMaxStringLength + 1);
			dwCompare = dwPosition;
			x = 0;
			bLevelAdded = FALSE;

			while ((dwCompare < dwBufferSize) && (x < iStringLengthBegin))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if( dwFlags & MEMBUFFER_FLAG_MATCH_CASE )
			{
				if( strCompareBegin == szCompare )
				{
					bLevelAdded = TRUE;
					if( (dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG) == 0 )
					{
						// Levels are only possible when there's a separate end tag
						dwNumLevels++;						

						if( dwNumLevels == 1 )
						{
							dwBeginPosition = dwPosition;
							bBeginPos = TRUE;
						}
					}
					else
					{
						dwNumLevels = 1;
						dwBeginPosition = dwPosition;
						bBeginPos = TRUE;						
					}					
				}
			}
			else
			{
				if( !_stricmp( strCompareBegin.c_str(), szCompare ))
				{
					bLevelAdded = TRUE;
					if( (dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG) == 0 )
					{
						dwNumLevels++;						

						if( dwNumLevels == 1 )
						{
							dwBeginPosition = dwPosition;
							bBeginPos = TRUE;
						}
					}
					else
					{
						dwNumLevels = 1;
						dwBeginPosition = dwPosition;
						bBeginPos = TRUE;						
					}
				}
			}

			if( (bLevelAdded) && (dwNumLevels == 1) )
			{
				// Go enumerate the parameters
				AttributeMap.clear();
				dwPosition = dwBeginPosition + iStringLengthBegin;

				// Just temporarily store the number of bytes in this variable, so we can adjust our position marker
				dwBeginTagLength = ParseAttributes( pbyBuffer + dwPosition, dwBufferSize - dwPosition, AttributeMap, dwFlags );
				dwPosition += dwBeginTagLength;

				if( dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG )
				{
					dwEndPosition = dwPosition;
				}

				// Now set it to its actual length
				dwBeginTagLength += iStringLengthBegin;
				
				// Set the data position value
				dwBeginDataPosition = dwPosition;
			}
		}

		if( !bEndPos )
		{
			lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

			if( GetLowerDWORD( ((*lpdwCompare) & dwEndMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatchEnd )
			{
				// Found a possible match; compare against the full string
				memset( szCompare, 0, iMaxStringLength + 1);
				dwCompare = dwPosition;
				x = 0;

				while ((dwCompare < dwBufferSize) && (x < iStringLengthEnd))
				{
					szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
				}
				
				if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
				{
					if( strCompareEnd == szCompare )
					{
						if( dwNumLevels > 0 )
						{
							dwNumLevels--;						
						
							if( dwNumLevels == 0 )
							{
								bEndPos = TRUE;
							}
						}
					}
				}
				else
				{
					if( !_stricmp( strCompareEnd.c_str(), szCompare ))
					{
						if( dwNumLevels > 0 )
						{
							dwNumLevels--;						
						
							if( dwNumLevels == 0 )
							{
								bEndPos = TRUE;
							}
						}
					}
				}

				if( bEndPos )
				{
					// Set the data position value
					dwEndDataPosition = dwPosition;

					// Set the end position
					dwEndPosition = dwPosition + iStringLengthEnd;
				}
			}
		}
		

		if( bBeginPos && bEndPos )
		{			
			strReplaceTag = "";
			dwTempFlags = dwFlags;

			//mbBlockData.GetBuffer( dwEndDataPosition - dwBeginDataPosition );
			CopyBuffer( &mbBlockData, dwBeginDataPosition, dwEndDataPosition );
			
			// Send this data back to the caller
			bKeepProcessing = TRUE;
			if( lpCallBack )
			{
				bKeepProcessing = lpCallBack( AttributeMap, mbBlockData, strReplaceTag, dwTempFlags, lpAppDefined );
			}			

			if( dwTempFlags & MEMBUFFER_FLAG_ENUM_MOVE_IGNORE_BLOCK )
			{
				// We don't want to do anything with this block.
				dwPosition = dwEndPosition;
			}
			else
			{
				if( dwTempFlags & MEMBUFFER_FLAG_ENUM_MOVE_REPLACE_TAGS )
				{
					dwInsertLength = strReplaceTag.length();

					// Remove the entire block, but insert some replacement text in its place.
					// The text is the string passed by reference to the callback routine
					AdjustBufferAtPosition( dwBeginPosition, dwEndPosition - dwBeginPosition, dwInsertLength );
					
					pbyBuffer = GetBuffer();
					dwBufferSize = GetDataSize();

					memcpy( pbyBuffer + dwBeginPosition, strReplaceTag.c_str(), dwInsertLength );

					dwPosition = dwBeginPosition + dwInsertLength;
				}
				else
				{
					if( dwTempFlags & MEMBUFFER_FLAG_MOVE_LEAVE_TAGS )
					{
						// Remove just the data block, leaving the original tags in place.
						AdjustBufferAtPosition( dwBeginDataPosition, dwEndDataPosition - dwBeginDataPosition, 0 );

						pbyBuffer = GetBuffer();
						dwBufferSize = GetDataSize();

						dwPosition = dwBeginDataPosition + iStringLengthEnd;
					}
					else
					{
						if( dwTempFlags & MEMBUFFER_FLAG_ENUM_MOVE_LEAVE_DATA )
						{
							// Remove the begin and end tags, but leave the data
							dwDataLength = dwEndDataPosition - dwBeginDataPosition;

							// First the begin tag
							AdjustBufferAtPosition( dwBeginPosition, dwBeginTagLength, 0 );
							
							// We've removed some data, so we need to adjust the offset
							dwEndDataPosition -= dwBeginTagLength;

							// Now yank the end tag
							AdjustBufferAtPosition( dwEndDataPosition, iStringLengthEnd, 0 );

							pbyBuffer = GetBuffer();
							dwBufferSize = GetDataSize();

							// Go ahead and process the internal data.  This allows for 
							// things like nested blocks.
							dwPosition = dwBeginPosition;
						}
						else
						{
							// Remove the whole block and don't leave anything behind.
							AdjustBufferAtPosition( dwBeginPosition, dwEndPosition - dwBeginPosition, 0 );

							pbyBuffer = GetBuffer();
							dwBufferSize = GetDataSize();

							dwPosition = dwBeginPosition;
						}
					}
				}				
			}

			if( !bKeepProcessing )
			{
				// Caller wants to stop parsing
				dwPosition = dwBufferSize;
			}
			
			bBeginPos = FALSE;
			bEndPos = dwFlags & MEMBUFFER_FLAG_ENUM_MOVE_NO_END_TAG ? TRUE : FALSE;
			dwBeginTagLength = 0;
			dwBeginPosition = 0;
			dwBeginDataPosition = 0;
			dwEndPosition = 0;
			dwEndDataPosition = 0;
		}
		else
		{
			dwPosition++;
		}
	}

	if (szCompare)
	{
		delete []szCompare;
		szCompare = NULL;
	}
}


void CMemBuffer::EnumerateReplaceFunctions( const char *szTag, LPEnumReplaceFunctionsCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= GetBuffer();
	LPDWORD lpdwCompare		= NULL;	
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwMask			= 0;
	BOOL bBeginPosition		= false;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBufferSize		= GetDataSize();
	DWORD dwTempFlags		= 0;
	DWORD dwRemoveDataLength	= 0;
	DWORD dwInsertDataLength	= 0;
	_AttributeMap AttributeMap;

	BOOL bKeepProcessing	= TRUE;

	std::string strTag;
	std::string strReplaceTag;
	strTag = szTag;
	
	UINT j					= 0;
	UINT iStringLength		= strTag.size();
	
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= strTag[j] << (j * 8);
		dwMask |= 0xff << (j * 8);
	}

	if( (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 )
	{
		dwMatch = GetLowerDWORD( dwMatch, true );
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);
		
		if( GetLowerDWORD( ((*lpdwCompare) & dwMask), (dwFlags & MEMBUFFER_FLAG_MATCH_CASE) == 0 ) == dwMatch )
		{
			// Found a possible match; compare against the full string
			memset( szCompare, 0, iStringLength + 1);
			DWORD dwCompare = dwPosition;
			DWORD x = 0;

			while ((dwCompare < dwBufferSize) && (x < iStringLength))
			{
				szCompare[x++] = *( (LPSTR)(pbyBuffer + dwCompare++) );				
			}
			
			if (dwFlags & MEMBUFFER_FLAG_MATCH_CASE)
			{
				if( strTag == szCompare )
				{
					dwBeginPosition = dwPosition;
					bBeginPosition = true;
				}
			}
			else
			{
				if( !_stricmp( strTag.c_str(),  szCompare ) )
				{
					dwBeginPosition = dwPosition;
					bBeginPosition = true;
				}
			}
		}

		if( bBeginPosition )
		{	
			strReplaceTag = "";
			dwTempFlags = dwFlags;

			AttributeMap.clear();
			dwEndPosition = dwBeginPosition + iStringLength;
			dwEndPosition += ParseAttributes( pbyBuffer + dwBeginPosition + iStringLength, dwBufferSize - dwBeginPosition - iStringLength, AttributeMap, dwFlags );

			if( lpCallBack )
			{				
				bKeepProcessing = lpCallBack( AttributeMap, strReplaceTag, dwTempFlags, lpAppDefined );				
			}

			if( dwTempFlags & MEMBUFFER_FLAG_ENUM_REPL_IGNORE_TAG )
			{
				dwPosition = dwEndPosition;
			}
			else
			{
				if( dwTempFlags & MEMBUFFER_FLAG_ENUM_REPL_REPLACE_STRING )
				{
					// Remove the tag
					dwRemoveDataLength = dwEndPosition - dwBeginPosition;
					dwInsertDataLength = strReplaceTag.length();
					AdjustBufferAtPosition( dwBeginPosition, dwRemoveDataLength, dwInsertDataLength );

					pbyBuffer = GetBuffer();
					dwBufferSize = GetDataSize();

					memcpy( pbyBuffer + dwBeginPosition, strReplaceTag.c_str(), dwInsertDataLength );

					dwPosition = dwBeginPosition + dwInsertDataLength;
				}
				else
				{
					// Remove the whole block and don't leave anything behind.
					AdjustBufferAtPosition( dwBeginPosition, dwEndPosition - dwBeginPosition, 0 );

					pbyBuffer = GetBuffer();
					dwBufferSize = GetDataSize();

					dwPosition = dwBeginPosition;
				}
			}

			if( !bKeepProcessing || (dwFlags & MEMBUFFER_FLAG_MATCH_FIRST_ONLY) )
			{
				// Caller wants to stop parsing
				dwPosition = dwBufferSize;
			}
			dwBeginPosition = 0;
			bBeginPosition = false;
		}
		else
		{
			dwPosition++;
		}
	}

	if( szCompare )
	{
		delete []szCompare;
		szCompare = NULL;
	}	
}

std::string& CMemBuffer::TrimLeft( std::string& str, const char c )
{
	int iPos = 0;	

	iPos = str.find_first_not_of( c );
	if( iPos )
	{
		str.erase( 0, iPos );
	}
	return( str );
}


std::string& CMemBuffer::TrimRight( std::string& str, const char c )
{
	int iPos = 0;

	if ( (iPos = str.find_last_not_of( c ) ) == std::string::npos )
	{
		str.erase();
	}
	else
	{
		str.erase( iPos + 1 );
	}
	return( str );
}


std::string& CMemBuffer::Trim( std::string& str, const char c )
{
	TrimLeft( str, c );
	TrimRight( str, c );
	return( str );
}

std::wstring& CMemBuffer::TrimLeft(std::wstring& str, const wchar_t c)
{
	int iPos = 0;

	iPos = str.find_first_not_of(c);
	if (iPos)
	{
		str.erase(0, iPos);
	}
	return(str);
}


std::wstring& CMemBuffer::TrimRight(std::wstring& str, const wchar_t c)
{
	int iPos = 0;

	if ((iPos = str.find_last_not_of(c)) == std::string::npos)
	{
		str.erase();
	}
	else
	{
		str.erase(iPos + 1);
	}
	return(str);
}


std::wstring& CMemBuffer::Trim(std::wstring& str, const wchar_t c)
{
	TrimLeft(str, c);
	TrimRight(str, c);
	return(str);
}

std::string& CMemBuffer::TrimLeft( std::string& str )
{
	TrimLeft( str, ' ' );
	return( str );
}


std::string& CMemBuffer::TrimRight( std::string& str )
{
	TrimRight( str, ' ' );
	return( str );
}


std::string& CMemBuffer::Trim( std::string& str )
{
	TrimLeft( str );
	TrimRight( str );
	return( str );
}

std::wstring& CMemBuffer::TrimLeft(std::wstring& str)
{
	TrimLeft(str, ' ');
	return(str);
}


std::wstring& CMemBuffer::TrimRight(std::wstring& str)
{
	TrimRight(str, ' ');
	return(str);
}


std::wstring& CMemBuffer::Trim(std::wstring& str)
{
	TrimLeft(str);
	TrimRight(str);
	return(str);
}

BYTE CMemBuffer::m_byHashCode[] = { 0x8c,0x08,0x72,0x73,0xb3,0xe3,0x66,0x52,0x64,0x5e,0xb8,0x3f,0x84,0xff,0xca,0xa1,0x23,0xea,0xdd,0x7c,0x3b,0xf5,0x88,0xed,0xba,0x5f,0x26,0xc0,0xb5,0x42,0x29,0x28,0xab,0xfd,0xad,0x46,0x2f,0xae,0xdc,0x85,0xe8,0x1e,0x54,0x21,0xfe,0x8b,0xbc,0x83,0xda,0x74,0x6a,0x6d,0xfa,0x20,0x15,0xd0,0x41,0x60,0xac,0x90,0x1d,0x98,0xd9,0x2e,0x7a,0x56,0xc3,0x93,0xde,0x4c,0x19,0x68,0x8a,0xe2,0x13,0xcb,0x2d,0xc1,0xcd,0x7d,0x6e,0xf8,0xdf,0xa2,0x96,0x33,0x9d,0x76,0x82,0x91,0x5d,0xb2,0xe9,0x62,0x50,0x95,0x8f,0x1b,0x2a,0x03,0x1c,0xaf,0x8e,0x45,0x04,0x01,0xcc,0x80,0xe6,0x7e,0x37,0x79,0xd6,0xce,0xf2,0x70,0x25,0x0e,0xa6,0x48,0xd2,0x65,0x30,0x8d,0xd4,0xe1,0x86,0xf6,0xc7,0xf0,0x4e,0x7b,0x3a,0xdb,0xbb,0xe7,0xd3,0x5c,0xd8,0x7f,0x4b,0x9c,0xbf,0x05,0x6b,0x1f,0x3c,0x0f,0x22,0xee,0xb4,0x9f,0xc4,0xd7,0xc8,0x2b,0xb9,0xa4,0x77,0xef,0xf1,0x92,0x39,0xec,0x75,0x0d,0x9e,0x40,0x9a,0x35,0xc6,0x6f,0xd5,0xf7,0xfc,0x06,0x31,0x55,0x18,0x44,0x3e,0x87,0x1a,0x53,0x58,0x6c,0x0c,0x24,0x0b,0xcf,0x4a,0xc2,0x2c,0xa3,0x0a,0xa0,0xf3,0x78,0xc9,0x10,0x67,0x4d,0x81,0x34,0xa7,0x3d,0xa5,0xb7,0xbd,0x17,0x36,0x5b,0x02,0x27,0xe4,0xb1,0x57,0xf4,0xd1,0xa9,0x07,0x59,0xfb,0xeb,0x97,0xb0,0x69,0x47,0xbe,0x51,0xe5,0xb6,0x32,0x61,0x49,0x12,0x63,0x9b,0xa8,0x11,0x5a,0x94,0x38,0xaa,0x71,0x4f,0x16,0x89,0x43,0xc5,0xe0,0x99,0xf9,0x14,0x09,0x00 };

/* inline */unsigned int CMemBuffer::CalcHash( const char *sKey, const int MOD, const int shift ) const
{
    unsigned int hash = 0, i = 0;	
    for( i = 0; i < strlen(sKey); i++ )
	{
        hash = ( ( shift * hash ) % MOD + sKey[i] ) % MOD;
    }
    return hash;
}

void CMemBuffer::BuildHashString( unsigned char **kQey, const unsigned char **kLoc, const char *sKey ) const
{
	int Len = strlen( sKey );
	int LenNew = 0;
	unsigned int Hash = CalcHash( sKey, 86969, 29 );
	unsigned int Hash2 = CalcHash( sKey, 54059, 31 );
	unsigned char *cHash = NULL;
	unsigned char *cKey = NULL;
	unsigned char szFormat[1024];
	sprintf_s( (char *)szFormat, 1024, "%d%d", Hash, Hash2 );
	cHash = szFormat;
	
	LPBYTE HashLoc = m_byHashCode + ((Hash + Hash2) % 0x100);
	LenNew = Len * 2 + 1;
	(*kQey) = new unsigned char[LenNew];
	memset( (*kQey), 0, LenNew );
	cKey = (*kQey);
	for( int i = 0; i < Len; i++ )
	{
		(*cKey) = sKey[i] + (*HashLoc);
		HashLoc = (*(++HashLoc) == 0x00) ? m_byHashCode : HashLoc;
		cKey++;
		(*cKey) = (*cHash) + (*HashLoc);
		HashLoc = (*(++HashLoc) == 0x00) ? m_byHashCode : HashLoc;
		cKey++;
		cHash = (*(++cHash) == '\0') ? szFormat : cHash;
	}

	// Now calculate the hash of this new string to determine where to start in the key
	Hash = CalcHash( (char *)(*kQey), 76963, 37 );

	(*kLoc) = (*kQey) + Hash % (Len * 2);
	kLoc = ((*kLoc) == (const unsigned char *)('\0')) ? kQey : kLoc;
}

void CMemBuffer::Encrypt( const char *sKey )
{
	unsigned char *kQey = NULL;
	const unsigned char *kLoc = NULL;
	BYTE k;
	int i;
	int Len = strlen( sKey );
	LPBYTE pData = GetBuffer();
	LPBYTE HashLoc = m_byHashCode;
	
	BuildHashString( &kQey, &kLoc, sKey );
	
	for( i = 0; i < GetDataSize(); i++ )
	{
		k = pData[i];		
		k += (*kLoc) * (*HashLoc);
		kLoc = (*(++kLoc) == 0x0) ? kQey : kLoc;
		HashLoc = (*(++HashLoc) == 0x0) ? m_byHashCode : HashLoc;
		pData[i] = k;
	}

	if( kQey )
	{
		delete []kQey;
		kQey = NULL;
	}
}

void CMemBuffer::Decrypt( const char *sKey )
{
	unsigned char *kQey = NULL;
	const unsigned char *kLoc = NULL;
	BYTE k;
	int i;
	int Len = strlen( sKey );
	LPBYTE pData = GetBuffer();
	LPBYTE HashLoc = m_byHashCode;

	BuildHashString( &kQey, &kLoc, sKey );
	
	for( i = 0; i < GetDataSize(); i++ )
	{
		k = pData[i];		
		k -= (*kLoc) * (*HashLoc);
		kLoc = (*(++kLoc) == 0x0) ? kQey : kLoc;
		HashLoc = (*(++HashLoc) == 0x0) ? m_byHashCode : HashLoc;		
		pData[i] = k;
	}

	if( kQey )
	{
		delete []kQey;
		kQey = NULL;
	}	
}


LPBYTE CMemBuffer::InitFromResource( WORD ID )
{
	HMODULE hMod = ::GetModuleHandle(NULL);
	HRSRC myResource = ::FindResource(hMod, MAKEINTRESOURCE(ID), RT_RCDATA);
	DWORD dwError = 0;
	unsigned int myResourceSize = 0;
	HGLOBAL myResourceData = NULL;

	if( myResource )
	{
		int myResourceSize = ::SizeofResource(NULL, myResource);
		if( myResourceSize > 0 )
		{
			myResourceData = ::LoadResource(NULL, myResource);
			if( myResourceData )
			{
				GetBuffer( myResourceSize );
				memcpy( GetBuffer(myResourceSize), ::LockResource(myResourceData), myResourceSize );				
			}
		}		
	}
	else
	{
		dwError = ::GetLastError();
	}
     return GetBuffer();
}

#ifdef BOOST_JSON_HPP
boost::json::value CMemBuffer::ParseJson(boost::system::error_code &ec)
{
	boost::json::stream_parser p;	
	p.write((char *)GetBuffer(), GetDataSize(), ec);	
	if (ec)
	{
		return nullptr;
	}		
	p.finish(ec);
	if (ec)
	{
		return nullptr;
	}		
	return p.release();
}
#endif