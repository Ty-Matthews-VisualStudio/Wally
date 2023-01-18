// Implementation file for CSafeFile

#include "stdafx.h"
#include "SafeFile.h"


CSafeFile::CSafeFile( LPCTSTR szFileName) : m_lpData( NULL ), m_dwDataSize( 0 ), m_dwErrorCode( 0 ), m_strFileName( szFileName )
{
	Open( NULL );
}

CSafeFile::CSafeFile() : m_lpData( NULL ), m_dwDataSize( 0 ), m_dwErrorCode( 0 ), m_strFileName( "" )
{
}

BOOL CSafeFile::Open( LPCTSTR szFileName /* = NULL */ )
{
	HANDLE hFile = NULL;
	HANDLE hFileMapping = NULL;
	LPVOID lpView = NULL;
	DWORD dwFileSize = 0;
	DWORD dwError = 0;

	if( szFileName )
	{
		m_strFileName = szFileName;
	}
	else
	{
		szFileName = m_strFileName.GetBuffer( m_strFileName.GetLength() );
	}

	hFile = CreateFile( 
		szFileName,					// Name of file
		GENERIC_READ,				// Desired access
		FILE_SHARE_READ,			// Share mode
		NULL,						// Security attributes
		OPEN_EXISTING,				// Creation disposition
		FILE_FLAG_SEQUENTIAL_SCAN,	// Attributes and flags
		NULL);						// Template file	

	if( hFile == INVALID_HANDLE_VALUE )
	{
		// CreateFile() does not return NULL on error
		SetErrorCode( SF_FILE_NOT_FOUND );
		return FALSE;
	}

	// Store this away for now...
	dwFileSize = GetFileSize( hFile, NULL );

	if( dwFileSize == -1 )
	{
		dwError = ::GetLastError();
		CloseHandle( hFile );
		SetErrorCode( SF_UNKNOWN_FILE_SIZE );
		return FALSE;
	}
	
	hFileMapping = CreateFileMapping( 
		hFile,						// Handle to file
		NULL,						// Security attributes
		PAGE_READONLY,				// Protection
		0,							// Max size high
		0,							// Max size low
		NULL);						// Name of mapping object	
	
	if (hFileMapping == NULL)
	{
		dwError = ::GetLastError();
		CloseHandle( hFile);
		SetErrorCode( SF_ERROR_MAPPING_FILE );
		return FALSE;
	}

	// We don't need this anymore
	CloseHandle( hFile);

	// Map to the entire file
	lpView = MapViewOfFile(
		hFileMapping,				// Handle to the mapping
		FILE_MAP_READ,				// Desired access
		0,							// Offset high
		0,							// Offset low
		0);							// Number of bytes

	if (lpView == NULL)
	{
		dwError = ::GetLastError();
		CloseHandle( hFileMapping);
		SetErrorCode( SF_ERROR_MAPPING_FILE );
		return FALSE;
	}

	// We don't need this anymore
	CloseHandle( hFileMapping);
	
	m_lpData = lpView;	
	m_dwDataSize = dwFileSize;	
	
	return TRUE;
}

CSafeFile::~CSafeFile()
{
	ReleaseFile();
}

void CSafeFile::ReleaseFile()
{
	if( m_lpData )
	{
		UnmapViewOfFile( m_lpData );
		m_lpData = NULL;
	}
}

LPCTSTR CSafeFile::GetErrorText()
{	
	switch( GetErrorCode() )
	{
	case SF_SUCCESS :
		{
			ASSERT (false);		// Why are you retrieving the error text when it was succesful?
		}
		break;

	case SF_OUT_OF_MEMORY :
		{
			m_strErrorCode = "Out of memory error occured when trying to allocate for the file.";
		}
		break;

	case SF_ERROR_READING_FILE :
		{
			m_strErrorCode = "There was an error while reading " + m_strFileName;
		}
		break;

	case SF_FILE_NOT_FOUND :
		{
			m_strErrorCode =  "File " + m_strFileName;
			m_strErrorCode += " could not be located";
		}
		break;

	case SF_UNKNOWN_FILE_SIZE:
		{
			m_strErrorCode = "There was an error retrieving the size of file " + m_strFileName;
		}
		break;

	case SF_ERROR_MAPPING_FILE:
		{
			m_strErrorCode = "There was an error mapping into file " + m_strFileName;
		}
		break;
		
	default:
		ASSERT (false);		// Unhandled error code
		break;
	}
	
	return m_strErrorCode.GetBuffer( m_strErrorCode.GetLength() );
}