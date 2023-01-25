// MemBuffer.cpp: implementation of the CMemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MemBuffer.h"
#include "MiscFunctions.h"
#include "Wally.h"

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemBuffer::CMemBuffer( LPCTSTR szName /* = NULL */ ) : m_strName( szName )
{
	Init();
}

CMemBuffer::CMemBuffer(DWORD dwDataSize, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */ )
{
	Init();
	AllocateBuffer(dwDataSize, dwFlags);
}

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
		dwAllocSize = ((dwDataSize + dwMinSize - 1) & ~(dwMinSize - 1));
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
		
		m_pbyBuffer	= new BYTE[dwAllocSize];
		if (!m_pbyBuffer)
		{
			return NULL;
		}

		SetTrueDataSize(dwAllocSize);
	}	

	SetDataSize(dwDataSize);

	if (dwFlags & MEMBUFFER_FLAG_ZEROMEMORY)
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

LPBYTE CMemBuffer::GetBuffer()
{	
	return m_pbyBuffer;
}

LPBYTE CMemBuffer::GetBuffer(DWORD dwDataSize, DWORD dwFlags /* = MEMBUFFER_FLAG_ZEROMEMORY */)
{	
	return AllocateBuffer(dwDataSize, dwFlags);
}

LPBYTE CMemBuffer::InitFromFile( FILE *fp, DWORD dwFlags /* = 0 */)
{
	ASSERT (fp);
	DWORD dwDataSize = GetFileLength(fp);

	LPBYTE pbyBuffer = AllocateBuffer( dwDataSize, dwFlags);
	
	if (pbyBuffer)
	{
		if (fread( pbyBuffer, 1, dwDataSize, fp) != dwDataSize)
		{
			Release();
			return NULL;
		}
	}

	return pbyBuffer;
}

LPBYTE CMemBuffer::InitFromFile( LPCTSTR szFileName, DWORD dwFlags /* = 0 */)
{
	ASSERT( szFileName);
	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, szFileName, "rb");
	LPBYTE pbyBuffer = NULL;
	
	if (err == 0)
	{
		pbyBuffer = InitFromFile( fp, dwFlags);
		fclose (fp);		
	}

	return pbyBuffer;
}

void CMemBuffer::WriteToFile( LPCTSTR szFileName)
{
	// If this errors, the exception will be thrown up the chain
	LPVOID lpFile = MakeFile( szFileName, GetDataSize() );

	if( lpFile )
	{
		memcpy( lpFile, GetBuffer(), GetDataSize() );
		CloseFile( lpFile );
	}	
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

void CMemBuffer::ReplaceString( LPCTSTR szReplace, LPCTSTR szNewString, DWORD dwFlags /* = 0 */)
{
	ASSERT (szReplace);
	ASSERT (szNewString);
	
	LPBYTE pbyBuffer		= m_pbyBuffer;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBufferSize		= GetDataSize();
	
	UINT j					= 0;
	UINT iStringLength		= strlen(szReplace);
	UINT iNewStringLength	= strlen(szNewString);
	CString strCompare( szReplace);

	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= szReplace[j] << (j * 8);
	}

	while ((dwPosition + iStringLength) <= dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if ((*lpdwCompare) == dwMatch)
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
				if (!strcmp(strCompare, szCompare))
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if (!strCompare.CompareNoCase( szCompare))
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
					pbyBuffer = GetBuffer();
					memcpy (pbyBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
				}
			}

			dwBufferSize = GetDataSize();
/*						
					}

				memcpy (pbyNewBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
				
				PBYTE pbyNewBuffer = NULL;
				BOOL bNewAlloc = FALSE;
				
				if (dwNewBufferSize > GetTrueDataSize())
				{
					DWORD dwAllocSize = ((dwNewBufferSize + ALLOCATE_SIZE) & ~ALLOCATE_SIZE));
					pbyNewBuffer = new BYTE[dwAllocSize];					
				
					bNewAlloc = TRUE;					
				}
				else
				{
					pbyNewBuffer = GetBuffer();
				}
				
				if (pbyNewBuffer)
				{
					memcpy (pbyNewBuffer, pbyBuffer, dwBeginPosition);

					if (iNewStringLength > 0)
					{
						memcpy (pbyNewBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
					}
					
					memcpy (pbyNewBuffer + dwBeginPosition + iNewStringLength, pbyBuffer + dwEndPosition, dwBufferSize - dwEndPosition);
										
					m_pbyBuffer = pbyNewBuffer;

					if (bNewAlloc)
					{
						Release();
						
						SetDataSize(dwNewBufferSize);
						SetTrueDataSize(dwAllocSize);
					}					

					pbyBuffer = GetBuffer();
					dwBufferSize = GetDataSize();
				}				
			}
			*/
			
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

void CMemBuffer::InsertString( LPCTSTR szMatch, LPCTSTR szNewString, DWORD dwFlags /* = MEMBUFFER_FLAG_INSERT_AFTER */)
{
	ASSERT (szMatch);
	ASSERT (szNewString);
	
	LPBYTE pbyBuffer		= m_pbyBuffer;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	DWORD dwBufferSize		= GetDataSize();
	
	UINT j					= 0;
	UINT iStringLength		= strlen(szMatch);
	UINT iNewStringLength	= strlen(szNewString);
	CString strCompare( szMatch);

	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= szMatch[j] << (j * 8);
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if ((*lpdwCompare) == dwMatch)
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
				if (!strCompare.Compare( szCompare))
				{
					dwBeginPosition = dwPosition;
					dwEndPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if (!strCompare.CompareNoCase( szCompare))
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
				ASSERT (FALSE);
				break;
			}

			dwBufferSize = GetDataSize();
			dwPosition = dwBeginPosition + iStringLength + iNewStringLength;

			/*
			// We might have to adjust the buffer size				
				DWORD dwNewBufferSize = (dwBufferSize - iStringLength) + iNewStringLength;
				
				if (dwNewBufferSize > GetTrueDataSize())
				{
					DWORD dwAllocSize = ((dwNewBufferSize + ALLOCATE_SIZE) & ~ALLOCATE_SIZE));
					LPBYTE pbyNewBuffer = new BYTE[dwAllocSize];
				
					if (pbyNewBuffer)
					{
						memcpy (pbyNewBuffer, pbyBuffer, dwBeginPosition);

						if (iNewStringLength > 0)
						{
							memcpy (pbyNewBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
						}
						
						memcpy (pbyNewBuffer + dwBeginPosition + iNewStringLength, pbyBuffer + dwEndPosition, dwBufferSize - dwEndPosition);

						
						m_pbyBuffer = pbyNewBuffer;

						SetDataSize(dwNewBufferSize);
						SetTrueDataSize(dwAllocSize);

						pbyBuffer = GetBuffer();
						dwBufferSize = GetDataSize();
					}
				}



			// We might have to adjust the buffer size			
			DWORD dwNewBufferSize = dwBufferSize + iNewStringLength;

			if (dwNewBufferSize > GetTrueDataSize())
			{
				DWORD dwAllocSize = ((dwNewBufferSize + ALLOCATE_SIZE) & ~ALLOCATE_SIZE));
				LPBYTE pbyNewBuffer = new BYTE[dwAllocSize];				
			
				DWORD dwInsertType = 0;

				if (pbyNewBuffer)
				{
					

					switch (dwInsertType)
					{
					case MEMBUFFER_FLAG_INSERT_BEFORE:
						{
							memcpy (pbyNewBuffer, pbyBuffer, dwBeginPosition);
							memcpy (pbyNewBuffer + dwBeginPosition, (LPBYTE)szNewString, iNewStringLength);
							memcpy (pbyNewBuffer + dwBeginPosition + iNewStringLength, pbyBuffer + dwBeginPosition, dwBufferSize - dwBeginPosition);						
						}
						break;

					case MEMBUFFER_FLAG_INSERT_AFTER:
						{
							memcpy (pbyNewBuffer, pbyBuffer, dwEndPosition);
							memcpy (pbyNewBuffer + dwEndPosition, (LPBYTE)szNewString, iNewStringLength);
							memcpy (pbyNewBuffer + dwEndPosition + iNewStringLength, pbyBuffer + dwBeginPosition, dwBufferSize - dwBeginPosition);						
						}
						break;

					default:
						ASSERT (FALSE);
						break;
					}
					
					dwPosition = dwBeginPosition + iStringLength + iNewStringLength;

					Release();
					m_pbyBuffer = pbyNewBuffer;
					m_dwDataSize = dwNewBufferSize;

					pbyBuffer = m_pbyBuffer;
					dwBufferSize = m_dwDataSize;
				}
			}
			*/
			
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

void CMemBuffer::InsertBuffer( LPCTSTR szMatch, CMemBuffer *pmbInsertBlock, DWORD dwFlags /* = MEMBUFFER_FLAG_INSERT_AFTER */)
{
	// Just add a NULL terminator at the end and treat it like a string
	LPBYTE pbyInsertBlock = pmbInsertBlock->GetBuffer();
	DWORD dwInsertBlockSize = pmbInsertBlock->GetDataSize();

	if (pbyInsertBlock)
	{
		if (pbyInsertBlock[dwInsertBlockSize - 1] == 0)
		{
			InsertString( szMatch, (LPSTR)pbyInsertBlock, dwFlags);
		}
		else
		{
			CMemBuffer mbNewBlock;
			mbNewBlock.GetBuffer( dwInsertBlockSize + 1);
			pmbInsertBlock->CopyBuffer( &mbNewBlock);
			pbyInsertBlock = mbNewBlock.GetBuffer();

			if (pbyInsertBlock)
			{				
				InsertString( szMatch, (LPSTR)pbyInsertBlock, dwFlags);
			}
		}
	}
}

void CMemBuffer::MoveStringBlock( LPCTSTR szBegin, LPCTSTR szEnd, CMemBuffer *pmbNewBlock, DWORD dwFlags /* = 0 */)
{
	ASSERT (szBegin);
	ASSERT (szEnd);
	
	LPBYTE pbyBuffer		= m_pbyBuffer;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatchBegin		= 0;
	DWORD dwMatchEnd		= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwEndPosition		= 0;
	LPDWORD lpdwBeginPosition	= NULL;
	LPDWORD lpdwEndPosition		= NULL;
	DWORD dwBufferSize		= GetDataSize();
	
	UINT j					= 0;
	UINT iStringLengthBegin	= strlen(szBegin);
	UINT iStringLengthEnd	= strlen(szEnd);
	UINT iMaxStringLength	= max (iStringLengthBegin, iStringLengthEnd);
	CString strCompareBegin( szBegin);
	CString strCompareEnd( szEnd);

	char *szCompare			= new char[iMaxStringLength + 1];
	
	for (j = 0; j < min( iStringLengthBegin, 4); j++)
	{
		dwMatchBegin |= szBegin[j] << (j * 8);
	}

	for (j = 0; j < min( iStringLengthEnd, 4); j++)
	{
		dwMatchEnd |= szEnd[j] << (j * 8);
	}

	while (dwPosition + 4 < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if (!lpdwBeginPosition)
		{
			if ((*lpdwCompare) == dwMatchBegin)
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
					if (!strCompareBegin.Compare( szCompare))
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
					if (!strCompareBegin.CompareNoCase( szCompare))
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
			if ((*lpdwCompare) == dwMatchEnd)
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
					if (!strCompareEnd.Compare( szCompare))
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
					if (!strCompareEnd.CompareNoCase( szCompare))
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
				
				/*
				DWORD dwNewBufferSize = dwBufferSize - dwRemoveLength;
				LPBYTE pbyNewBuffer = new BYTE[dwNewBufferSize];
								
				if (pbyNewBuffer)
				{
					memcpy (
						pbyNewBuffer, 
						pbyBuffer, 
						dwBeginPosition);
					
					memcpy (
						pbyNewBuffer + dwBeginPosition, 
						pbyBuffer + dwEndPosition, 
						dwBufferSize - dwEndPosition);
					
					dwPosition = dwBeginPosition;

					DWORD dwBlockLength = dwEndPosition - dwBeginPosition; // dwRemoveLength - (iStringLengthBegin + iStringLengthEnd);

					

					Release();
					m_pbyBuffer = pbyNewBuffer;
					m_dwDataSize = dwNewBufferSize;

					pbyBuffer = m_pbyBuffer;
					dwBufferSize = m_dwDataSize;
					
				}				
				*/
				
				if (pmbNewBlock)
				{
					// Just set this so we break out.  We don't support multiple matches, because 
					// we're only getting one CMemBuffer pointer coming in (they'd override otherwise)					
					dwPosition = dwBufferSize; 
					continue;
				}				
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

void CMemBuffer::CopyBuffer( CMemBuffer *pmbCopy)
{
	ASSERT (pmbCopy);

	DWORD dwDataSize = GetDataSize();
	DWORD dwMinSize = GetMinDataSize();
	LPBYTE pbyCopyBuffer = NULL;

	if (pmbCopy->GetDataSize() < dwDataSize)
	{
		pbyCopyBuffer = pmbCopy->GetBuffer( dwDataSize, dwMinSize);
	}
	else
	{
		pbyCopyBuffer = pmbCopy->GetBuffer();
	}

	if (pbyCopyBuffer)
	{
		LPBYTE pbySourceBuffer = GetBuffer();
		memcpy( pbyCopyBuffer, pbySourceBuffer, dwDataSize);
	}	
}

CString CMemBuffer::GetTagValue( LPCTSTR szTag, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= m_pbyBuffer;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();

	CString strReturn;
	CString strTag("");
	strTag.Format ("<!--%s=", szTag);
	
	UINT j					= 0;
	UINT iStringLength		= strTag.GetLength();	
	
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= strTag.GetAt(j) << (j * 8);
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if ((*lpdwCompare) == dwMatch)
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
				if (!strTag.Compare (szCompare))
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if (!strTag.CompareNoCase( szCompare))
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

CString CMemBuffer::GetAllTagValues( LPCTSTR szTag, LPEnumSequencesCallBack lpCallBack, LPVOID lpAppDefined, DWORD dwFlags /* = 0 */)
{
	LPBYTE pbyBuffer		= m_pbyBuffer;
	LPDWORD lpdwCompare		= NULL;
	DWORD dwPosition		= 0;
	DWORD dwMatch			= 0;
	DWORD dwBeginPosition	= 0;
	DWORD dwBufferSize		= GetDataSize();

	CString strReturn("");
	CString strTag("");
	strTag.Format ("<!--%s=", szTag);
	
	UINT j					= 0;
	UINT iStringLength		= strTag.GetLength();	
	
	char *szCompare			= new char[iStringLength + 1];
	
	for (j = 0; j < min( iStringLength, 4); j++)
	{
		dwMatch |= strTag.GetAt(j) << (j * 8);
	}

	while ((dwPosition + iStringLength) < dwBufferSize)
	{		
		lpdwCompare = (LPDWORD)(pbyBuffer + dwPosition);

		if ((*lpdwCompare) == dwMatch)
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
				if (!strTag.Compare (szCompare))
				{
					dwBeginPosition = dwPosition + iStringLength;
				}
			}
			else
			{
				if (!strTag.CompareNoCase( szCompare))
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

			lpCallBack( atol (strReturn), lpAppDefined);

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

LPBYTE CMemBuffer::InitFromResource(WORD ID)
{
	HMODULE hMod = ::GetModuleHandle(NULL);
	HRSRC myResource = ::FindResource(hMod, MAKEINTRESOURCE(ID), MAKEINTRESOURCE(RT_RCDATA));
	DWORD dwError = 0;
	unsigned int myResourceSize = 0;
	HGLOBAL myResourceData = NULL;

	if (myResource)
	{
		int myResourceSize = ::SizeofResource(NULL, myResource);
		if (myResourceSize > 0)
		{
			myResourceData = ::LoadResource(NULL, myResource);
			if (myResourceData)
			{
				GetBuffer(myResourceSize);
				memcpy(GetBuffer(myResourceSize), ::LockResource(myResourceData), myResourceSize);
			}
		}
	}
	else
	{
		dwError = ::GetLastError();
	}
	return GetBuffer();
}
