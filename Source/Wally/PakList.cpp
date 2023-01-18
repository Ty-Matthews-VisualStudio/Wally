// PakList.cpp: implementation of the CPakItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "PakList.h"
#include "MiscFunctions.h"
#include "DirectoryList.h"
//#include "ProgressBar.h"
#include <mmsystem.h>	// requires linking with "winmm.lib"
#include "SafeFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CWallyApp theApp;

void CPakPasteItem::SetNext(CPakPasteItem *pNext)
{
	m_pNext = pNext;
}

CPakPasteItem *CPakPasteItem::GetNext()
{
	return m_pNext;
}

void CPakPasteItem::SetPrevious(CPakPasteItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CPakPasteItem *CPakPasteItem::GetPrevious()
{
	return m_pPrevious;
}

CPakItem::CPakItem()
{	
	Initialize();
}

CPakItem::CPakItem(lp_packfile_t lpPackFile)
{
	Initialize();

	memcpy (&m_PackFileHeader, lpPackFile, sizeof (m_PackFileHeader));

	m_strShortName = m_PackFileHeader.name;	
	SetDisplaySize (m_PackFileHeader.filelen);
	
	int iLength = 0;

	while (m_strShortName.Find ('/') != -1)
	{
		iLength = m_strShortName.GetLength();
		m_strShortName = m_strShortName.Right ((iLength - m_strShortName.Find ('/')) - 1);
	}

	SetDisplayName (m_strShortName);	
}

void CPakItem::Initialize()
{
	SetNext (NULL);
	SetPrevious (NULL);

	memset (&m_PackFileHeader, 0, sizeof (m_PackFileHeader));

	SetIconIndex (-1);
	SetPakType (PAK_TYPE_ITEM);
	SetDisplaySize (0);
	
	m_PakParam.pPakItem = this;
	m_PakParam.pPakBase = (CPakBase *)this;	
	m_strDataFile = "";
	m_strShortName = "";

	m_iNewFileLen = 0;
	m_iNewFilePos = 0;

	m_bNameModified = FALSE;

	m_pbyTempBuffer = NULL;
}

CPakItem::~CPakItem()
{
	SetNext (NULL);
	SetPrevious (NULL);

	if (m_strDataFile != "")
	{
		// We're still out there... maybe we're being deleted prior to serialize()?  
		// Time to yank the file regardless
		remove (m_strDataFile);	
	}

	if (m_pbyTempBuffer)
	{
		delete []m_pbyTempBuffer;
		m_pbyTempBuffer = NULL;
	}
}

BOOL CPakItem::Create (LPCTSTR szDataFile, LPCTSTR szDirectory, LPCTSTR szTempDirectory)
{
	CFile f;
	CFileException fe;
	
	if (!f.Open ( szDataFile, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary, &fe))
	{			
		m_strError.Format ("File %s could not be opened.\nError code: %lu", szDataFile, fe.m_cause);
		SetErrorCode (PAK_ERROR_FILEOPEN);
		return FALSE;
	}

	CString strFileName("");

	m_strShortName = GetRawFileNameWExt (szDataFile);
	SetDisplayName (m_strShortName);
	
	strFileName.Format ("%s%s", szDirectory, m_strShortName);

	if (strFileName.GetLength() > (sizeof (m_PackFileHeader.name) - 1))
	{
		SetErrorCode (PAK_ERROR_NAME_EXCEED_LENGTH);
		m_strError = strFileName;
		return FALSE;
	}
	
	strcpy_s (m_PackFileHeader.name, sizeof(m_PackFileHeader.name), strFileName);
	m_PackFileHeader.filelen = f.GetLength();
	SetDisplaySize (m_PackFileHeader.filelen);
	m_PackFileHeader.filepos = 0;

	f.Close();

	// Convert the Unix-style slash '/' to Windows '\'
	CString strDirectory (szDirectory);
	CString strMkDir("");

	for (int j = 0; j < strDirectory.GetLength(); j++)
	{
		if (strDirectory.GetAt(j) == '/')
		{
			strDirectory.SetAt(j, '\\');
			strMkDir.Format ("%s\\%s\\%s\\", g_strTempDirectory, szTempDirectory, strDirectory.Left (j));
			_mkdir (strMkDir);
		}
	}

	strFileName.Format ("%s\\%s\\%s%s", g_strTempDirectory, szTempDirectory, strDirectory, m_strShortName);	
	m_strDataFile = strFileName;

	// Go copy the file to the temporary directory for editing, etc.  If we can't copy the file to the temp
	// directory, we must fail out as we can't edit the file
	if (!CopyFile (szDataFile, m_strDataFile, TRUE))
	{
		DWORD dwError = GetLastError();
		m_strError.Format ("File %s could not be created.\nError code: %lu", m_strDataFile, dwError);
		SetErrorCode (PAK_ERROR_COPY_FILE);
		return FALSE;
	}

	time_t tGenesis;
	tGenesis = 0;
	m_ctLastModified = tGenesis;
	
	return TRUE;
}

BOOL CPakItem::IsModified(int *piFlags)
{
	BOOL bModified = FALSE;
	
	if (m_bNameModified)
	{
		(*piFlags) |= SERIALIZE_MODIFIED_NAME;
		bModified = TRUE;
	}

	if (m_strDataFile != "")
	{
		CFileStatus fStatus;

		if (CFile::GetStatus (m_strDataFile, fStatus))
		{
			CTime ctLastModified = fStatus.m_mtime;
			if (m_ctLastModified != ctLastModified)
			{
				(*piFlags) |= SERIALIZE_MODIFIED_DATA;
				bModified = TRUE;
			}
		}
		else
		{
			// We've got a file out there, but we can't retrieve the info on it... assume they're working
			// with it or something, and notify the user.
			bModified = TRUE;
		}		
	}
		
	return bModified;
}

BOOL CPakItem::ReplaceDataFile (LPCTSTR szFileName)
{
	if (!CopyFile (szFileName, m_strDataFile, FALSE))
	{
		CString strError("");
		strError.Format ("Failed to copy %s over %s.\nError code: %lu", szFileName, m_strDataFile, GetLastError());
		SetErrorCode (PAK_ERROR_COPY_FILE);
		SetErrorString (strError);
		return FALSE;
	}

	m_ctLastModified = 0;
	return TRUE;
}

BOOL CPakItem::CreateTempFile (LPCTSTR szPakFile, LPCTSTR szDirectory, int iFlags /* = 0 */)
{
	CFile f;
	CFileException fe;
	
	if ((szPakFile) && (!m_pbyTempBuffer))
	{	
		CString strError("");
		
		if (!f.Open ( szPakFile, CFile::modeRead | CFile::typeBinary, &fe))
		{
			SetErrorCode (PAK_ERROR_FILEOPEN);
			m_strError.Format ("File %s could not be opened.\nError code : %lu", szPakFile, fe.m_cause);
			return FALSE;
		}
		
		f.Seek (GetFilePos(), CFile::begin);
	}

	CString strTemp (m_PackFileHeader.name);
	CString strDirectory ("");
	CString strPathFromHere ("");
	
	if (strTemp.Find ('/') != -1)
	{
		strPathFromHere = strTemp;
		int iLastSlash = -1;
		int j = 0;
		
		// Find the last slash in the name, so we can trim off the actual filename
		for (j = strPathFromHere.GetLength() - 1; (j >= 0) && (iLastSlash == -1); j--)
		{
			if (strPathFromHere.GetAt(j) == '/')
			{
				iLastSlash = j;				
			}
		}

		// Trim off the filename.
		strPathFromHere = strPathFromHere.Left(iLastSlash + 1);

		// Convert the Unix-style slash '/' to Windows '\'
		CString strMkDir("");
		for (j = 0; j < strPathFromHere.GetLength(); j++)
		{
			if (strPathFromHere.GetAt(j) == '/')
			{
				strPathFromHere.SetAt(j, '\\');

				// Make the temporary directories

				if (iFlags & PAK_EXPORT_FLAG_ADD_TEMP)
				{
					strMkDir.Format ("%s\\%s\\%s\\", g_strTempDirectory, szDirectory, strPathFromHere.Left (j));
				}
				else
				{
					strMkDir.Format ("%s\\%s\\", szDirectory, strPathFromHere.Left (j));
				}
				
				if (iFlags & PAK_EXPORT_FLAG_RETAIN)
				{
					_mkdir (strMkDir);
				}
			}
		}	
	}

	CString strFileName("");

	if (iFlags & PAK_EXPORT_FLAG_ADD_TEMP)
	{
		if (iFlags & PAK_EXPORT_FLAG_RETAIN)
		{
			strFileName.Format ("%s\\%s\\%s%s", g_strTempDirectory, szDirectory, strPathFromHere, m_strShortName);
		}
		else
		{
			strFileName.Format ("%s\\%s\\%s", g_strTempDirectory, szDirectory, m_strShortName);
		}
		m_strDataFile = strFileName;
	}
	else
	{
		if (iFlags & PAK_EXPORT_FLAG_RETAIN)
		{
			strFileName.Format ("%s\\%s%s", szDirectory, strPathFromHere, m_strShortName);
		}
		else
		{
			strFileName.Format ("%s\\%s", szDirectory, m_strShortName);
		}
	}

	if ((iFlags & PAK_EXPORT_FLAG_REPLACE) == 0)
	{
		// Check to see if the file exists... if so, we don't want to overwrite it
		CFile fr;
		CFileException fe;

		if (fr.Open ( strFileName, CFile::modeRead | CFile::typeBinary, &fe))
		{			
			return TRUE;
		}		
	}
	
	CFile fw;

	if (!fw.Open ( strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &fe))
	{
		SetErrorCode (PAK_ERROR_FILEOPEN);
		m_strError.Format ("File %s could not be opened.\nError code : %lu", strFileName, fe.m_cause);
		return FALSE;
	}
	
	BYTE *pbyTempBuffer = NULL;
	UINT iSize = 0;

	if (szPakFile)
	{
		if ((m_strDataFile != "") && ((iFlags & PAK_EXPORT_FLAG_ADD_TEMP) == 0))
		{
			// If iFlags & PAK_EXPORT_FLAG_ADD_TEMP == 0, we're not going to the temp directory,
			// we're exporting the file.			
			return CopyFile (m_strDataFile, strFileName, FALSE);			
		}
		else
		{
			if (!m_pbyTempBuffer)
			{
				iSize = GetFileLen();
				pbyTempBuffer = new BYTE[iSize];

				if (!pbyTempBuffer)
				{
					SetErrorCode (PAK_ERROR_OUTOFMEMORY);		
					return FALSE;
				}

				if (f.Read (pbyTempBuffer, iSize) != iSize)
				{
					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}
					m_strError = szPakFile;
					SetErrorCode (PAK_ERROR_READ_FAILURE);
					return FALSE;
				}
				
				f.Close();
			}
			else
			{
				pbyTempBuffer = m_pbyTempBuffer;
			}
		}
	}
	else
	{
		// Just put something there... szPakFile is NULL, meaning we just want a temporary file and
		// we don't really care about what goes in there.
		iSize = 4;
		pbyTempBuffer = new BYTE[iSize];
	}

	fw.Write (pbyTempBuffer, iSize);
	fw.Close();

	if (iFlags & PAK_EXPORT_FLAG_ADD_TEMP)
	{
		CFileStatus fStatus;

		if (CFile::GetStatus (strFileName, fStatus))
		{
			m_ctLastModified = fStatus.m_mtime;
		}
		else
		{
			m_ctLastModified = CTime::GetCurrentTime();
		}
	}

	// Yank out the temporary buffer, as we're now actually on disk
	if (m_pbyTempBuffer)
	{
		delete []m_pbyTempBuffer;
		m_pbyTempBuffer = NULL;
	}
	else
	{
		if (pbyTempBuffer)
		{
			delete []pbyTempBuffer;
			pbyTempBuffer = NULL;
		}
	}

	return TRUE;
}

BOOL CPakItem::ExportFile (LPCTSTR szPakFile, LPCTSTR szDirectory, CStringArray *psaWildCards, CProgressBar *pProgressBar)
{
	int iFlags = 0;
	
	iFlags |= g_bPakExportReplaceExistingItems ? PAK_EXPORT_FLAG_REPLACE : 0;
	iFlags |= g_bPakExportRetainStructure ? PAK_EXPORT_FLAG_RETAIN : 0;

	CString strShortName("");
	strShortName = GetShortName();

	if (psaWildCards)
	{
		if (MatchesWildCard (strShortName.GetBuffer (strShortName.GetLength()), psaWildCards))
		{
			pProgressBar->StepIt();
			return CreateTempFile (szPakFile, szDirectory, iFlags);
		}
	}
	else
	{
		pProgressBar->StepIt();
		return CreateTempFile (szPakFile, szDirectory, iFlags);
	}

	return TRUE;
}

void CPakItem::SetShortName(LPCTSTR szShortName)
{
	// Note:  this code assumes that checking was already done on duplicate names for the current
	// directory.  The CPakListView::OnEndLabelEdit won't allow you to rename a file if another
	// already exists, so we *should* be able to rename this here.

	// If we've got a DataFile, time to rename it.
	if (m_strDataFile != "")
	{
		CString strNewFileName("");

		strNewFileName = TrimSlashes (GetPathToFile (m_strDataFile));
		strNewFileName += "\\";
		strNewFileName += szShortName;
		
		rename (m_strDataFile, strNewFileName);

		m_strDataFile = strNewFileName;
	}

	m_strShortName = szShortName;
	SetDisplayName (szShortName);
	
	m_bNameModified = TRUE;
}

void CPakItem::RenameDataFile (LPCTSTR szTempDirectory)
{
	// Only update it if we actually exist.
	if (m_strDataFile != "")
	{
		m_strDataFile.Format ("%s\\%s", TrimSlashes (szTempDirectory), m_strShortName);		
	}

	// Seeing how our parent directory doesn't actually "exist" as an object in the 
	// PAK, we have to mark ourselves as being modified to indicate to the user
	// that our individual path has been changed.

	m_bNameModified = TRUE;
}

BOOL CPakItem::OpenItem()
{
	ASSERT (m_strDataFile != ""); 

	if (g_bPakQuickPlaySound)
	{
		CString strExtension = GetExtension (m_strDataFile);

		if (!strExtension.CompareNoCase (".wav"))
		{
			PlaySound( m_strDataFile, AfxGetInstanceHandle(), 
				SND_ASYNC | SND_FILENAME | SND_NODEFAULT);
			return TRUE;
		}
	}
	
	//HINSTANCE hInst = ShellExecute( NULL, NULL, m_strDataFile, NULL, NULL, SW_SHOWDEFAULT);

	SHELLEXECUTEINFO shInfo;

	int iDataSize = sizeof (SHELLEXECUTEINFO);
	
	memset (&shInfo, 0, iDataSize);
	shInfo.cbSize = iDataSize;
	shInfo.fMask = SEE_MASK_NOCLOSEPROCESS;	
	shInfo.lpFile = m_strDataFile.GetBuffer (m_strDataFile.GetLength());
	shInfo.nShow = SW_SHOWNORMAL;
	
	ShellExecuteEx (&shInfo);

	if ((ULONG)shInfo.hInstApp == ERROR_NO_ASSOCIATION )
	{
		SetErrorCode (PAK_ERROR_NO_ASSOC);
		return FALSE;
	}

	return TRUE;
}

BOOL CPakItem::QuickPlay (LPCTSTR szPakFile)
{
	if (!m_pbyTempBuffer)
	{	
		// Create a temporary buffer so we don't bother writing out to disk
		CFile f;
		CFileException fe;
		
		CString strError("");
			
		if (!f.Open ( szPakFile, CFile::modeRead | CFile::typeBinary, &fe))
		{
			SetErrorCode (PAK_ERROR_FILEOPEN);
			m_strError.Format ("File %s could not be opened.\nError code : %lu", szPakFile, fe.m_cause);
			return FALSE;
		}
			
		f.Seek (GetFilePos(), CFile::begin);
		
		UINT iSize = GetFileLen();
		m_pbyTempBuffer = new BYTE[iSize];

		if (!m_pbyTempBuffer)
		{
			SetErrorCode (PAK_ERROR_OUTOFMEMORY);
			return FALSE;
		}
		
		if (f.Read (m_pbyTempBuffer, iSize) != iSize)
		{
			m_strError = szPakFile;
			SetErrorCode (PAK_ERROR_READ_FAILURE);
			return FALSE;
		}
	}

	PlaySound( (char *)m_pbyTempBuffer, AfxGetInstanceHandle(), 
		SND_ASYNC | SND_MEMORY | SND_NODEFAULT);
	
	return TRUE;
}

void CPakItem::SetNext (CPakItem *pNext)
{
	m_pNext = pNext;
}

CPakItem *CPakItem::GetNext()
{
	return m_pNext;
}

void CPakItem::SetPrevious (CPakItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CPakItem *CPakItem::GetPrevious()
{
	return m_pPrevious;
}

BOOL CPakItem::Serialize (int iStage, CPakDirectory *pParent, CFile *pNewPakFile, CFile *pOldPakFile, UINT *piPosition, UINT *piCount, CProgressBar *pProgressBar)
{
	switch (iStage)
	{
	case SERIALIZE_STAGE_DATA:
		{
			CSafeFile ReadFile;
			BYTE *pbyTempBuffer = NULL;
			UINT iDataSize = 0;

			// if m_strDataFile != "", then we were either opened for editing, or were created after
			// the fact.  The data file needs to be loaded up and written out to the PAK file
			if( m_strDataFile != "" )
			{				
				if( !ReadFile.Open( m_strDataFile ) )
				{				
					m_strError = ReadFile.GetErrorText();
					SetErrorCode( PAK_ERROR_FILEOPEN );
					return FALSE;
				}

				iDataSize = ReadFile.GetDataSize();

				if( iDataSize <= 0 )
				{
					SetErrorCode( PAK_ERROR_ZERO_LENGTH );
					SetErrorString( m_strDataFile );
					return FALSE;
				}

				pbyTempBuffer = ReadFile.GetBuffer();
			
				// Write
				TRY
				{
					pNewPakFile->Write( pbyTempBuffer, iDataSize );
				}
				CATCH( CFileException, e )
				{
					m_strError.Format( "%s\nError code: %lu", pNewPakFile->GetFilePath(), e->m_cause );
					SetErrorCode (PAK_ERROR_WRITE_FAILURE);
					return FALSE;
				}
				END_CATCH

				// Remove the data file
				remove( m_strDataFile );
				m_strDataFile = "";
			}
			else
			{	
				// The data still exists in our source PAK file
				int iFilePos = GetFilePos();
				pOldPakFile->Seek( iFilePos, CFile::begin );

				iDataSize = GetFileLen();
				pbyTempBuffer = new BYTE[iDataSize];

				if( !pbyTempBuffer )
				{			
					SetErrorCode (PAK_ERROR_OUTOFMEMORY);
					return FALSE;
				}

				// Go read from the source PAK file.  We work with two SEPARATE files during Serialize.
				// This is because we don't want to be loading hundreds of megs of data into RAM.  In
				// order to accomplish this, we need to read from the current file, write to a temporary
				// file, delete the current file, then rename the temporary file so that it replaces
				// the current file.  Yes, it's kldugy, but it is much easier and saner than having to
				// try and allocate huge amounts of RAM.  The Half-Life PAK file is 300 megs
				UINT iDataRead = pOldPakFile->Read( pbyTempBuffer, iDataSize );
				if( iDataRead != iDataSize )
				{
					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}
					SetErrorString (pOldPakFile->GetFilePath());					
					SetErrorCode (PAK_ERROR_READ_FAILURE);
					return FALSE;
				}

				// Write
				TRY
				{
					pNewPakFile->Write( pbyTempBuffer, iDataSize );
				}
				CATCH( CFileException, e )
				{
					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}

					m_strError.Format( "%s\nError code: %lu", pNewPakFile->GetFilePath(), e->m_cause );
					SetErrorCode (PAK_ERROR_WRITE_FAILURE);
					return FALSE;
				}
				END_CATCH
			}
			pProgressBar->StepIt();			
					
			// Store these away for a bit... we'll call the proper stage later after we're all
			// finished to solidify these values
			m_iNewFileLen = iDataSize;
			m_iNewFilePos = (*piPosition);

			// Update the position
			(*piPosition) += iDataSize;
					
			pProgressBar->StepIt();
		}
		break;			// case SERIALIZE_STAGE_DATA	

	case SERIALIZE_STAGE_ENTRY:
		{
			// This is a temporary header with temporary values
			packfile_t PackFileHeader;
			int iHeaderSize = sizeof (PackFileHeader);

			memcpy (&PackFileHeader, &m_PackFileHeader, iHeaderSize);
			
			if (m_iNewFilePos != 0)
			{
				PackFileHeader.filepos = m_iNewFilePos;
			}

			if (m_iNewFileLen != 0)
			{
				PackFileHeader.filelen = m_iNewFileLen;
			}

			CString strPath("");
			strPath = pParent->GetPathFromHere('/');
			strPath += m_strShortName;

			if (strPath.GetLength() > (sizeof (PackFileHeader.name) - 1))
			{
				// Whoops!!
				SetErrorCode (PAK_ERROR_NAME_EXCEED_LENGTH);
				m_strError = strPath;
				return FALSE;
			}

			strcpy_s (PackFileHeader.name, sizeof(PackFileHeader.name), strPath);

			// Write the entry

			TRY
			{
				pNewPakFile->Seek ((*piPosition), CFile::begin);
				pNewPakFile->Write (&PackFileHeader, iHeaderSize);
			}
			CATCH( CFileException, e )
			{
				m_strError.Format ("%s\nError code: %lu", pNewPakFile->GetFilePath(), e->m_cause);
				SetErrorCode (PAK_ERROR_WRITE_FAILURE);
				return FALSE;
			}
			END_CATCH
						
			(*piPosition) += iHeaderSize;
			(*piCount)++;
			pProgressBar->StepIt();
		}
		break;			// case SERIALIZE_STAGE_ENTRY

	case SERIALIZE_STAGE_CLEANUP:
		{
			if (m_iNewFilePos != 0)
			{
				m_PackFileHeader.filepos = m_iNewFilePos;
			}

			if (m_iNewFileLen != 0)
			{
				m_PackFileHeader.filelen = m_iNewFileLen;
			}

			SetDisplaySize (m_iNewFileLen);

			m_iNewFilePos = 0;
			m_iNewFileLen = 0;

			m_bNameModified = FALSE;

			pProgressBar->StepIt();
		}
		break;			// case SERIALIZE_STAGE_CLEANUP

	default:
		ASSERT (FALSE);		// Unhandled stage?
		break;
	}
		
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPakDirectory Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPakDirectory::CPakDirectory()
{
	SetFirstItem (NULL);
	SetWriteItem (NULL);

	SetNext (NULL);
	SetPrevious (NULL);

	SetFirstDirectory (NULL);
	SetWriteDirectory (NULL);
	
	SetPakType (PAK_TYPE_DIRECTORY);
	m_PakParam.pPakDirectory = this;
	m_PakParam.pPakBase = (CPakBase *)this;

	m_bModified = FALSE;
}

CPakDirectory::CPakDirectory(LPCTSTR szDirectory, CPakDirectory *pParent, CString *pstrTempDirectory, CString *pstrPakFile)
{
	m_strDirectoryName = szDirectory;
	SetDisplayName (m_strDirectoryName);
	SetPakType (PAK_TYPE_DIRECTORY);
	m_PakParam.pPakDirectory = this;
	m_PakParam.pPakBase = (CPakBase *)this;

	SetFirstItem (NULL);
	SetWriteItem (NULL);

	SetNext (NULL);
	SetPrevious (NULL);

	SetFirstDirectory (NULL);
	SetWriteDirectory (NULL);

	m_pParent = pParent;
	m_pstrRootTempDirectory = pstrTempDirectory;
	m_pstrPakFile = pstrPakFile;

	// Create the temp directory name
	m_strTempDirectoryName.Format ("%s\\%s\\%s", g_strTempDirectory, TrimSlashes (*pstrTempDirectory), GetPathFromHere('\\'));
	m_strTempDirectoryName = TrimSlashes (m_strTempDirectoryName);

	m_bModified = FALSE;
}

CPakDirectory::~CPakDirectory()
{
	PurgeAll();

	// Delete the directory.	
	_rmdir (m_strTempDirectoryName);
}


void CPakDirectory::PurgeAll()
{
	CPakItem *pItem = GetFirstItem();

	while (pItem)
	{
		SetFirstItem(GetFirstItem()->GetNext());		
		delete pItem;
		pItem = GetFirstItem();
	}

	SetFirstItem (NULL);
	SetWriteItem (NULL);

	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)
	{
		SetFirstDirectory (GetFirstDirectory()->GetNext());
		delete pDirectory;
		pDirectory = GetFirstDirectory();
	}

	SetFirstDirectory (NULL);
	SetWriteDirectory (NULL);
}

void CPakDirectory::SetNext(CPakDirectory *pNext)
{
	m_pNext = pNext;
}

CPakDirectory *CPakDirectory::GetNext()
{
	return m_pNext;
}

void CPakDirectory::SetPrevious(CPakDirectory *pPrevious)
{
	m_pPrevious = pPrevious;
}

CPakDirectory *CPakDirectory::GetPrevious()
{
	return m_pPrevious;
}

int CPakDirectory::AddToList (LPCTSTR szFileName, lp_packfile_t lpPackFile)
{
	// This is a recursive function.  Our job here is to find out whether the passed-in szFileName belongs to ourself
	// or to one of our children (or grandchildren thereof.)  szFileName is modified as we go along, stripping off each 
	// directory as we hit the parents.  This way, we don't modify the actual PackFile entry.  When the szFileName parameter
	// has no directory (IE no '/') then we're at the end of the recursion.

	// We need to pass back the error code, as we can't easily tell exactly which instance failed.

	// Find out if we need to add a directory
	CString strFileName = szFileName;
	int iSlashPosition = strFileName.Find ('/');

	if (iSlashPosition == -1)
	{
		// Must be at our intended spot.  Add the entry to our internal linked list of PakItems
		CPakItem *pNewItem = new CPakItem (lpPackFile);

		if (!pNewItem)
		{
			return PAK_ERROR_OUTOFMEMORY;			
		}

		if (!GetFirstItem())
		{
			SetFirstItem (pNewItem);
			SetWriteItem (pNewItem);
		}
		else
		{
			GetWriteItem()->SetNext(pNewItem);
			pNewItem->SetPrevious(GetWriteItem());
			SetWriteItem (pNewItem);
		}
	}
	else
	{
		// We've got another directory
		CString strDirectory = strFileName.Left (iSlashPosition);

		// Trim the directory name off
		strFileName = strFileName.Right ((strFileName.GetLength() - iSlashPosition) - 1);

		// Check to see if it's in the list
		CPakDirectory *pDirectory = AddChildDirectory(strDirectory);

		if (!pDirectory)
		{			
			return PAK_ERROR_OUTOFMEMORY;
		}
		
		// pDirectory will be pointing to the match if found, otherwise it'll be pointing to the new item added.  Either
		// way, it's time to hand off control to our child
		return (pDirectory->AddToList (strFileName, lpPackFile));		
	}

	return PAK_ERROR_SUCCESS;
}

void CPakDirectory::BuildTree (CTreeCtrl& List, HTREEITEM htParent, int iClosedIcon, int iOpenIcon)
{
	// This is a recursive function.  Once called, the first thing we do is add ourself to the TreeCtrl.  We are then
	// responsible for adding all of our children to the TreeCtrl.  Our children happen to be the same class as ourself.
	// Once we add ourself, that returned HTREEITEM is passed along to our children so that they are inserted into
	// the proper location on the tree.

	// Add ourself
	CString strDirectory("");
	strDirectory = GetDirectoryName();
	int iLength = strDirectory.GetLength();
	
	TV_INSERTSTRUCT tvInsert;
	HTREEITEM htItem;

	tvInsert.hParent = htParent; 
	tvInsert.hInsertAfter = NULL;		
	
	tvInsert.item.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.pszText = strDirectory.GetBuffer(iLength);
	tvInsert.item.cchTextMax = iLength;
	tvInsert.item.iImage = iClosedIcon;
	tvInsert.item.iSelectedImage = iOpenIcon;
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = (ULONG)this;

	htItem = List.InsertItem (&tvInsert);

	// Add our children
	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)
	{
		pDirectory->BuildTree (List, htItem, iClosedIcon, iOpenIcon);
		pDirectory = pDirectory->GetNext();
	}
}

BOOL CPakDirectory::Serialize (int iStage, CFile *pNewPakFile, CFile *pOldPakFile, UINT *piPosition, UINT *piCount, CProgressBar *pProgressBar)
{
	// First Serialize our items
	CPakItem *pPakItem = GetFirstItem();

	while (pPakItem)
	{
		if (!pPakItem->Serialize (iStage, this, pNewPakFile, pOldPakFile, piPosition, piCount, pProgressBar))
		{
			SetErrorCode (pPakItem->GetErrorCode());
			SetErrorString (pPakItem->GetErrorString());
			return FALSE;
		}

		pPakItem = pPakItem->GetNext();
	}

	// Now serialize our children
	CPakDirectory *pPakDirectory = GetFirstDirectory();

	while (pPakDirectory)
	{
		if (!pPakDirectory->Serialize (iStage, pNewPakFile, pOldPakFile, piPosition, piCount, pProgressBar))
		{
			SetErrorCode (pPakDirectory->GetErrorCode());
			SetErrorString (pPakDirectory->GetErrorString());
			return FALSE;
		}

		pPakDirectory = pPakDirectory->GetNext();
	}

	m_bModified = FALSE;
	// Directories don't get serialized (there's nothing to them)
	return TRUE;
}

CString CPakDirectory::GetDirectoryName()
{
	return m_strDirectoryName;
}

void CPakDirectory::SetDirectoryName (LPCTSTR szDirectory /*  = NULL */)
{
	if (szDirectory)
	{
		m_strDirectoryName = szDirectory;
		SetDisplayName (szDirectory);
	}

	CString strNewDirectoryName("");

	strNewDirectoryName.Format ("%s\\%s\\%s", g_strTempDirectory, TrimSlashes (*m_pstrRootTempDirectory), GetPathFromHere('\\'));
	strNewDirectoryName = TrimSlashes (strNewDirectoryName);

	// Go rename it; it might not exist, but we need to check
	rename (m_strTempDirectoryName, strNewDirectoryName);

	m_strTempDirectoryName = strNewDirectoryName;

	// We have to change the data files for our items
	CPakItem *pItem = GetFirstItem();

	while (pItem)
	{		
		pItem->RenameDataFile(m_strTempDirectoryName);

		pItem = pItem->GetNext();
	}

	// Time to update our child directories... they need to adjust themselves, too.
	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)
	{
		// Passing NULL tells the kids to just re-use their current name, but rebuild
		// their temp name because we've changed.
		pDirectory->SetDirectoryName(NULL);

		pDirectory = pDirectory->GetNext();
	}
}

CString CPakDirectory::GetTempDirectoryName()
{
	return m_strTempDirectoryName;
}

void CPakDirectory::SetFirstItem(CPakItem *pFirst)
{
	m_pFirstItem = pFirst;
}

CPakItem *CPakDirectory::GetFirstItem()
{
	return m_pFirstItem;
}

void CPakDirectory::SetWriteItem(CPakItem *pWrite)
{
	m_pWriteItem = pWrite;
}

CPakItem *CPakDirectory::GetWriteItem()
{
	return m_pWriteItem;
}

CPakItem *CPakDirectory::GetFirstPakItem()
{
	m_pReadItem = GetFirstItem();
	return m_pReadItem;
}

CPakItem *CPakDirectory::GetNextPakItem()
{
	if (m_pReadItem)
	{
		m_pReadItem = m_pReadItem->GetNext();
	}

	return m_pReadItem;
}

void CPakDirectory::GetItemCount(UINT *piCount, UINT *piSize /* = NULL */, CStringArray *psaWildCards /* = NULL */, int iFlags /* = 0 */)
{
	CPakItem *pPakItem = GetFirstItem();
	CString strShortName("");
	
	while (pPakItem)
	{
		if (psaWildCards)
		{
			strShortName = pPakItem->GetShortName();
		
			// Skip the item if we don't match any of the wildcards
			if (!MatchesWildCard (strShortName.GetBuffer (strShortName.GetLength()), psaWildCards))
			{
				pPakItem = pPakItem->GetNext();
				continue;
			}
		}
		
		(*piCount)++;

		if (piSize)
		{
			(*piSize) += pPakItem->GetFileLen();
		}

		pPakItem = pPakItem->GetNext();
	}

	if ((iFlags & PAK_COUNT_SELF_ONLY) == 0)
	{
		CPakDirectory *pPakDirectory = GetFirstDirectory();

		while (pPakDirectory)
		{
			pPakDirectory->GetItemCount (piCount, piSize, psaWildCards, iFlags);
			pPakDirectory = pPakDirectory->GetNext();
		}
	}
}

void CPakDirectory::SetFirstDirectory(CPakDirectory *pFirst)
{
	m_pFirstDirectory = pFirst;
}

CPakDirectory *CPakDirectory::GetFirstDirectory()
{
	return m_pFirstDirectory;
}

void CPakDirectory::SetWriteDirectory(CPakDirectory *pWrite)
{
	m_pWriteDirectory = pWrite;
}

CPakDirectory *CPakDirectory::GetWriteDirectory()
{
	return m_pWriteDirectory;
}

CPakItem *CPakDirectory::FindNameInList (LPCTSTR szFileName)
{
	CString strRawFileName = GetRawFileNameWExt (szFileName);
	CString strCompare("");

	CPakItem *pItem = GetFirstItem();
	
	while (pItem)
	{
		strCompare = pItem->GetShortName();

		if (!strCompare.CompareNoCase (strRawFileName))
		{
			return pItem;
		}

		pItem = pItem->GetNext();
	}

	return NULL;
}

BOOL CPakDirectory::AddFileDirectory (LPCTSTR szDirectory, CStringArray *psaWildcards, int iFlags)
{
	CStringArray saFiles;
	CString strRootDirectory("");
	int j = 0;

	strRootDirectory.Format ("%s\\", TrimSlashes (szDirectory));
	
	CDirectoryList DirectoryList (iFlags & PAK_ADD_FLAG_RECURSE ? TRUE : FALSE);
	DirectoryList.SetRoot (szDirectory);	

	for (j = 0; j < psaWildcards->GetSize(); j++)
	{
		DirectoryList.AddWildcard (psaWildcards->GetAt(j));
	}	
	
	DirectoryList.SearchDirectories ();
	DirectoryList.GetAllFiles (&saFiles);

	CString strFileName("");
	CString strOffsetDirectory("");
	CString strParentDirectory("");
	
	strParentDirectory = GetParentDirectory (strRootDirectory);

	for (j = 0; j < saFiles.GetSize(); j++)
	{
		strFileName = saFiles.GetAt (j);

		// Come up with the directory list that will need to be added:
			
		// strRootDirectory (directory)		= c:\temp\
		// strFileName						= c:\temp\image\test.txt
		// strOffsetDirectory				= image\					...

		// TrimFromLeft takes the first path and subtracts it's entirety from
		// the second path, returning the difference between the two.  IE:
		// TrimFromLeft ("c:\temp\", "c:\temp\image\") will yield image\
		// 

		strOffsetDirectory = TrimSlashes (strParentDirectory) + "\\" + TrimFromLeft (GetPathToFile (strFileName), strRootDirectory);
		strOffsetDirectory = TrimSlashes (strOffsetDirectory) + "\\";

		// Add it to the directory		
		if (!AddFile (strFileName, strOffsetDirectory, iFlags))
		{		
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CPakDirectory::ExportFiles (LPCTSTR szDirectory, CStringArray *psaWildCards, CProgressBar *pProgressBar)
{
	CPakItem *pPakItem = GetFirstItem();
	
	while (pPakItem)
	{
		if (!pPakItem->ExportFile (*m_pstrPakFile, szDirectory, psaWildCards, pProgressBar))
		{
			SetErrorCode (pPakItem->GetErrorCode());
			SetErrorString (pPakItem->GetErrorString());

			return FALSE;
		}
		pPakItem = pPakItem->GetNext();
	}
	
	if (g_bPakExportRecurseSubDirectories)
	{
		CPakDirectory *pPakDirectory = GetFirstDirectory();
		
		while (pPakDirectory)
		{
			if (!pPakDirectory->ExportFiles (szDirectory, psaWildCards, pProgressBar))
			{
				SetErrorCode (pPakDirectory->GetErrorCode());
				SetErrorString (pPakDirectory->GetErrorString());

				return FALSE;
			}
			pPakDirectory = pPakDirectory->GetNext();
		}
	}

	return TRUE;
}

CPakDirectory *CPakDirectory::FindDirectory (LPCTSTR szDirectory)
{
	CString strDirectory("");
	strDirectory = TrimSlashes (szDirectory) + "\\";

	// Root:
	if (strDirectory == "\\")
	{
		return this;
	}

	if (strDirectory == GetPathFromHere('\\'))
	{
		return this;
	}

	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)	
	{
		if (pDirectory->FindDirectory(szDirectory))
		{
			return pDirectory->FindDirectory(szDirectory);
		}		
		pDirectory = pDirectory->GetNext();		
	}

	return NULL;
}

CPakDirectory *CPakDirectory::FindDirectoryInList (LPCTSTR szDirectory)
{
	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)	
	{
		CString strCompare("");
		strCompare = pDirectory->GetDirectoryName();

		if (!strCompare.CompareNoCase (szDirectory))
		{
			return pDirectory;
		}
		
		pDirectory = pDirectory->GetNext();		
	}

	return NULL;
}

CPakDirectory *CPakDirectory::AddChildDirectory (LPCTSTR szDirectory)
{
	// Check to see if it's in the list
	CPakDirectory *pDirectory = FindDirectoryInList(szDirectory);
	
	if (pDirectory)
	{
		return pDirectory;
	}

	// If we've gotten this far, then the directory doesn't exist.  Go add it!
	
	pDirectory = new CPakDirectory (szDirectory, this, m_pstrRootTempDirectory, m_pstrPakFile);

	if (!pDirectory)
	{
		return NULL;					
	}

	if (!GetFirstDirectory())
	{
		SetFirstDirectory(pDirectory);
		SetWriteDirectory(pDirectory);
	}
	else
	{

#define SORT_PAK_ITEM_LIST   1

		switch (SORT_PAK_ITEM_LIST)
		{
		case 1:
			{
				// Add it to the sorted list
				CPakDirectory *pRead = GetFirstDirectory();
				CString strLeft("");
				CString strRight(szDirectory);

				CPakDirectory *pLeft = NULL;
				CPakDirectory *pRight = NULL;

				while (pRead)
				{
					strLeft = pRead->GetDirectoryName();
					
					if (strLeft.CompareNoCase(strRight) == 1)
					{                           
						pLeft = pRead->GetPrevious();
						pRight = pRead;
						pRead = NULL;
					}
					else
					{
						pRead = pRead->GetNext();
					}
				}
				
				//  Item belongs at the end 
				if ((pRight == NULL) && (pLeft == NULL))
				{                   
					GetWriteDirectory()->SetNext (pDirectory);
					pDirectory->SetPrevious (GetWriteDirectory());
					SetWriteDirectory (pDirectory);
				}

				//  Item is the first in the list 
				if ((pLeft == NULL) && (pRight != NULL))
				{		    
					pDirectory->SetNext (GetFirstDirectory());
					GetFirstDirectory()->SetPrevious (pDirectory);
					SetFirstDirectory (pDirectory);
				}

				//  Item is somewhere in the middle of the list 
				if ((pLeft != NULL) && (pRight != NULL))
				{			
					pLeft->SetNext (pDirectory);
					pDirectory->SetPrevious (pLeft);
					pDirectory->SetNext (pRight);
					pRight->SetPrevious (pDirectory);
				}
			}
			break;

		case 0:
			{
				GetWriteDirectory()->SetNext(pDirectory);
				pDirectory->SetPrevious(GetWriteDirectory());
				SetWriteDirectory(pDirectory);
			}
			break;

		default:
			ASSERT (FALSE);
			break;
		}
	}

	return pDirectory;
}

void CPakDirectory::DeleteItem (CPakItem *pItem)
{
	CPakItem *pTemp = GetFirstItem();

	while (pTemp)
	{
		if (pTemp == pItem)
		{
			if (pTemp->GetPrevious())
			{
				pTemp->GetPrevious()->SetNext(pTemp->GetNext());
			}

			if (pTemp->GetNext())
			{
				pTemp->GetNext()->SetPrevious(pTemp->GetPrevious());
			}

			if (pTemp == GetFirstItem())
			{
				SetFirstItem (pTemp->GetNext());
			}

			if (pTemp== GetWriteItem())
			{
				SetWriteItem (pTemp->GetPrevious());
			}

			m_bModified = TRUE;

			delete pTemp;
			return;
		}
		pTemp = pTemp->GetNext();
	}	
}

void CPakDirectory::DeleteDirectory (CPakDirectory *pDirectory)
{
	CPakDirectory *pTemp = GetFirstDirectory();

	while (pTemp)
	{
		if (pTemp == pDirectory)
		{
			if (pTemp->GetPrevious())
			{
				pTemp->GetPrevious()->SetNext(pTemp->GetNext());
			}

			if (pTemp->GetNext())
			{
				pTemp->GetNext()->SetPrevious(pTemp->GetPrevious());
			}

			if (pTemp == GetFirstDirectory())
			{
				SetFirstDirectory (pTemp->GetNext());
			}

			if (pTemp== GetWriteDirectory())
			{
				SetWriteDirectory (pTemp->GetPrevious());
			}

			m_bModified = TRUE;

			delete pTemp;
			return;
		}
		pTemp = pTemp->GetNext();
	}
}

CPakDirectory *CPakDirectory::GetParent()
{
	return m_pParent;
}

BOOL CPakDirectory::CreateTempDirectory (LPCTSTR szPakFile, LPCTSTR szTempDirectory)
{
	// First create our own items
	CPakItem *pPakItem = GetFirstItem();

	while (pPakItem)
	{
		if (pPakItem->GetDataFile() == "")
		{
			if (!pPakItem->CreateTempFile (szPakFile, szTempDirectory, PAK_EXPORT_FLAG_ADD_TEMP | PAK_EXPORT_FLAG_RETAIN))
			{
				SetErrorCode (pPakItem->GetErrorCode());
				SetErrorString (pPakItem->GetErrorString());
				return FALSE;
			}
		}

		pPakItem = pPakItem->GetNext();
	}

	// Now create our children
	CPakDirectory *pPakDirectory = GetFirstDirectory();

	while (pPakDirectory)
	{
		if (!pPakDirectory->CreateTempDirectory (szPakFile, szTempDirectory))
		{
			SetErrorCode (pPakDirectory->GetErrorCode());
			SetErrorString (pPakDirectory->GetErrorString());
			return FALSE;
		}

		pPakDirectory = pPakDirectory->GetNext();
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	pErrorString is for recursion... we call our children, but the error string spit out by one needs to be carried back to the source
//  directory that initiated the recursive function

BOOL CPakDirectory::AddFile (LPCTSTR szFile, LPCTSTR szOffsetDirectory /* = NULL */, int iFlags /* = 0 */)
{
	if (szOffsetDirectory)
	{		
		// First thing to do is determine whether or not the file being added is supposed to go in one of our children
		CString strOffsetDirectory (szOffsetDirectory);
		
		int iSlashPosition = strOffsetDirectory.Find ('\\');

		if (iSlashPosition != -1)
		{
			// We've got another directory
			CString strChildDirectory = strOffsetDirectory.Left (iSlashPosition);

			// Trim the directory name off
			strOffsetDirectory = strOffsetDirectory.Right ((strOffsetDirectory.GetLength() - iSlashPosition) - 1);
			
			CPakDirectory *pDirectory = AddChildDirectory (strChildDirectory);

			if (!pDirectory)
			{
				SetErrorCode (PAK_ERROR_OUTOFMEMORY);
				return FALSE;
			}

			// pDirectory will be pointing to the match if found, otherwise it'll be pointing to the new item added.  Either
			// way, it's time to hand off control to our child
			
			if (!pDirectory->AddFile (szFile, strOffsetDirectory, iFlags))
			{
				SetErrorCode (pDirectory->GetErrorCode());
				SetErrorString (pDirectory->GetErrorString());
				return FALSE;
			}

			return TRUE;
		}
	}

	// We must be at the intended spot


	// Check first to see if the file already exists
	CPakItem *pItem = FindNameInList (szFile);

	if (pItem)
	{
		if (iFlags & PAK_ADD_FLAG_REPLACE)
		{
			CString strDataFile (pItem->GetDataFile());
			if (strDataFile == "")			
			{
				if (!pItem->CreateTempFile (NULL, (*m_pstrRootTempDirectory), PAK_EXPORT_FLAG_ADD_TEMP | PAK_EXPORT_FLAG_REPLACE | PAK_EXPORT_FLAG_RETAIN))
				{
					SetErrorCode (pItem->GetErrorCode());
					SetErrorString (pItem->GetErrorString());
					return FALSE;
				}
			}

			if (!pItem->ReplaceDataFile (szFile))
			{
				SetErrorCode (pItem->GetErrorCode());
				SetErrorString (pItem->GetErrorString());
				return FALSE;
			}

			return TRUE;
		}
		else
		{	
			SetErrorString (GetRawFileNameWExt (szFile));
			SetErrorCode (PAK_ERROR_DUPLICATE_NAME);
			return FALSE;
		}
	}
	
	pItem = new CPakItem;

	if (!pItem)
	{
		SetErrorCode (PAK_ERROR_OUTOFMEMORY);
		return FALSE;
	}

	if (!pItem->Create (szFile, GetPathFromHere('/'), (*m_pstrRootTempDirectory)))
	{
		SetErrorCode (pItem->GetErrorCode());
		SetErrorString (pItem->GetErrorString());		
		
		delete pItem;
		return FALSE;
	}

	if (!GetFirstItem())
	{
		SetFirstItem (pItem);
		SetWriteItem (pItem);
	}
	else
	{
		GetWriteItem()->SetNext(pItem);
		pItem->SetPrevious (GetWriteItem());
		SetWriteItem (pItem);
	}

	return TRUE;
}

CString CPakDirectory::GetPathFromHere(char cSeparator)
{
	CString strReturn ("");

	if (m_pParent)
	{
		strReturn = m_pParent->GetPathFromHere(cSeparator);
		strReturn += m_strDirectoryName;
		strReturn += cSeparator;
	}
	
	return strReturn;
}

BOOL CPakDirectory::IsModified(int *piFlags)
{
	// If one of our items or directories was deleted, we're smaller than before
	if (m_bModified)
	{
		(*piFlags) |= SERIALIZE_MODIFIED_DATA;
		return TRUE;
	}

	// Check our items first.  If any one of them is modified, no need to keep going
	CPakItem *pItem = GetFirstItem();

	while (pItem)
	{
		if (pItem->IsModified(piFlags))
		{
			return TRUE;
		}

		pItem = pItem->GetNext();
	}

	// Go through our kids
	CPakDirectory *pDirectory = GetFirstDirectory();

	while (pDirectory)
	{
		if (pDirectory->IsModified(piFlags))
		{
			return TRUE;
		}

		pDirectory = pDirectory->GetNext();
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// CPakList Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPakList::CPakList()
{
	CreateTempDirectory();
	m_pRoot = new CPakDirectory("Root", NULL, &m_strTempDirectory, &m_strFileName);

	SetFirstPasteItem (NULL);
	SetWritePasteItem (NULL);

}

CPakList::~CPakList()
{	
	if (m_pRoot)
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
	PurgePasteList();
}

void CPakList::CreateTempName(char *szTempName, int iLength)
{
	srand( (unsigned)time( NULL ) );
	
	div_t dt;
	int iNum = 0;
	int j = 0;

	memset (szTempName, 0, iLength);

	for (j = 0; j < iLength; j++)
	{
		dt = div (rand(), 16);
		iNum = dt.rem;
		
		sprintf_s (szTempName + j, iLength - j, "%x", iNum); 
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	The TempDirectory is a unique directory under g_strTempDirectory which is where this PAK file's items will be created, read,
//	and written to.  This is done because we can have multiple PAK files open at once, and we do not want to run into a situation
//	where we're putting out multiple filenames that might conflict.
void CPakList::CreateTempDirectory()
{
	char szName[9];
	BOOL bUnique = FALSE;
	CString strDirectory("");

	while (!bUnique)
	{
		CreateTempName (szName, 8);

		// Check to see if it's out there
		strDirectory.Format ("%s\\%s", g_strTempDirectory, szName);

		if (_mkdir (strDirectory) == 0)
		{
			bUnique = TRUE;
		}		
	}

	m_strTempDirectory = szName;	
}

BOOL CPakList::Serialize(CFile *pFile, LPCTSTR szActualFileName, int iFlags)
{
	// Store this filename away... we don't want to load in every byte of data, so we're only reading
	// the entries right now and displaying them.
	m_strFileName = pFile->GetFilePath();
	int iSerializeType = iFlags & 0x3;

	switch (iSerializeType)
	{
	case SERIALIZE_READ:
		{
			packheader_t PackHeader;
			int iLength = pFile->GetLength();
					
			if (iLength < sizeof (PackHeader))
			{
				SetErrorCode (PAK_ERROR_MALFORMED);				
				return FALSE;
			}
	
			pFile->Read (&PackHeader, sizeof (PackHeader));

			if (iLength < PackHeader.dirofs + PackHeader.dirlen)
			{
				SetErrorCode (PAK_ERROR_MALFORMED);				
				return FALSE;
			}

			if (PackHeader.ident != IDPAKHEADER)
			{
				SetErrorCode (PAK_ERROR_INVALID_PAK);
				return FALSE;
			}
			
			int iNumEntries = PackHeader.dirlen / sizeof (packfile_t);
			packfile_t PackFile;
						
			pFile->Seek (PackHeader.dirofs, CFile::begin);

			for (int j = 0; j < iNumEntries; j++)
			{
				pFile->Read (&PackFile, sizeof (packfile_t));

				SetErrorCode (m_pRoot->AddToList (PackFile.name, &PackFile));

				if (GetErrorCode() != PAK_ERROR_SUCCESS)
				{					
					return FALSE;
				}
			}
		}
		break;

	case SERIALIZE_WRITE:
		{
			// We do this in three stages.  The first stage is where we write out all of the raw data.
			// The second stage is where we write out each directory (packfile_t) entry.  The last
			// stage is where we go through and update each PakItem so that the new location and file
			// length is updated properly.  The reason for stages is that if any part fails, we don't 
			// want to be changing critical data, such as locations and sizes
			
			CString strTempPakFile("");
			BOOL bUpdateInPlace = ((iFlags & SERIALIZE_MODIFIED_DATA) == 0) && ((iFlags & SERIALIZE_SAVE_AS) == 0);

			if (!bUpdateInPlace)
			{
				UINT iItemCount = GetItemCount();

				if (iItemCount > MAX_FILES_IN_PACK)
				{
					CString strCount("");
					strCount.Format ("%lu", iItemCount);

					SetErrorString (strCount);
					SetErrorCode (PAK_ERROR_MAX_FILES);
					return FALSE;
				}

				// Create a temporary file to write to while we work on this
				char szName[9];
				BOOL bUnique = FALSE;

				while (!bUnique)
				{
					CreateTempName (szName, 8);

					// Check to see if it's out there
					strTempPakFile.Format ("%s\\%s.pak", g_strTempDirectory, szName);

					FILE* fp = NULL;
					errno_t err = fopen_s(&fp, strTempPakFile, "rb");
					if (err != 0)
					{
						bUnique = TRUE;
					}
					else
					{
						fclose (fp);
					}
				}

				CFile fNewPakFile;
				CFileException fe;

				if (!fNewPakFile.Open ( strTempPakFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &fe))
				{
					SetErrorCode (PAK_ERROR_FILEOPEN);
					m_strError.Format ("File %s could not be opened.\nError code : %lu", strTempPakFile, fe.m_cause);
					return FALSE;
				}			

				UINT iNumSteps = (iItemCount * 4) + 2;
				CProgressBar ctlProgress ("Saving...", 70, iNumSteps, false, 0);

				int iHeaderSize = sizeof (packheader_t);
				UINT iPosition = iHeaderSize;
				UINT iCount = 0;

				packheader_t PackHeader;
				PackHeader.ident = IDPAKHEADER;

				// Write out the empty header for now..
				fNewPakFile.Write (&PackHeader, iHeaderSize);
				
				if (!m_pRoot->Serialize (SERIALIZE_STAGE_DATA, &fNewPakFile, pFile, &iPosition, &iCount, &ctlProgress))
				{
					// Delete the temporary file
					fNewPakFile.Close();
					remove (strTempPakFile);

					// This is expected to be closed by us
					pFile->Abort();

					SetErrorString (m_pRoot->GetErrorString());
					SetErrorCode (m_pRoot->GetErrorCode());
					return FALSE;
				}

				// Here's where our directory starts
				PackHeader.dirofs = iPosition;

				if (!m_pRoot->Serialize (SERIALIZE_STAGE_ENTRY, &fNewPakFile, pFile, &iPosition, &iCount, &ctlProgress))
				{
					// Delete the temporary file
					fNewPakFile.Close();
					remove (strTempPakFile);

					// This is expected to be closed by us
					pFile->Abort();

					SetErrorString (m_pRoot->GetErrorString());
					SetErrorCode (m_pRoot->GetErrorCode());
					return FALSE;
				}

				if (!m_pRoot->Serialize (SERIALIZE_STAGE_CLEANUP, &fNewPakFile, pFile, &iPosition, &iCount, &ctlProgress))
				{
					// Delete the temporary file
					fNewPakFile.Close();
					remove (strTempPakFile);

					// This is expected to be closed by us
					pFile->Abort();

					SetErrorString (m_pRoot->GetErrorString());
					SetErrorCode (m_pRoot->GetErrorCode());
					return FALSE;
				}

				PackHeader.dirlen = iCount * sizeof (packfile_t);			
				fNewPakFile.SeekToBegin();		
				
				fNewPakFile.Write( &PackHeader, iHeaderSize );
				
				pFile->Close();
				fNewPakFile.Close();

				ctlProgress.StepIt();
				if( !(iFlags & SERIALIZE_SAVE_AS) )
				{				
					// Yank the old PAK file				
					remove( szActualFileName );
				}				

				// Rename the temp PAK to the actual PAK
				ctlProgress.StepIt();
				rename (strTempPakFile, szActualFileName );
							
				ctlProgress.Clear();
			}		// if (!bUpdateInPlace)
			else
			{
				// We don't need to create a temporary file, as we've only changed the names of the entries.  This
				// is a MAJOR speed-up, as we're not copying over every bit of data to a temporary file, then copying
				// the temporary file back to the original.  If someone is just renaming stuff, there's no need to do
				// all that extra work.
				packheader_t PackHeader;
				int iHeaderSize = sizeof (PackHeader);

				UINT iItemCount = GetItemCount();							
				UINT iNumSteps = iItemCount * 2;
				CProgressBar ctlProgress ("Saving...", 70, iNumSteps, false, 0);

				// Read the header
				pFile->Read (&PackHeader, iHeaderSize);

				// Grab the directory offset
				UINT iDirOffset = PackHeader.dirofs;
				UINT iDirLen = PackHeader.dirlen;
				UINT iCount = 0;
				pFile->Seek (iDirOffset, CFile::begin);

				// Read off the entire directory, in case something goes terribly wrong and we have to abort.
				UINT iPackFileSize = sizeof (packfile_t);
				
				if ((iDirLen / iPackFileSize) != iItemCount)
				{
					// If this ASSERTs, then we're in trouble.  Some item has been added/deleted since we were loaded
					// up, and the source PAK file can't be added to.  IE: this section should only be called when
					// we are SURE that only the names of items or directories has changed.  Otherwise, the first 
					// !bUpdateInPlace method should be used.

					ASSERT (FALSE);
					SetErrorCode (PAK_ERROR_WALLY_ERROR);

					CString strError("");

					strError.Format ("%s%s%s%lu%s%s%lu",
						"CPakList::Serialize() during in-place PAK replacement.\n",
						"if ((iDirLen / iPackFileSize) != iItemCount) ASSERT failure.\n",
						"iDirLen/iPackFileSize = ", iDirLen/iPackFileSize, "\n",
						"iItemCount = ", iItemCount);

					SetErrorString (strError);
					pFile->Abort();
					return FALSE;
				}
				
				BYTE *pbyTempBuffer = new BYTE[iDirLen]; 

				if (!pbyTempBuffer)
				{
					SetErrorCode (PAK_ERROR_OUTOFMEMORY);
					return FALSE;
				}

				memset (pbyTempBuffer, 0, iDirLen);				

				if (pFile->Read (pbyTempBuffer, iDirLen) != iDirLen)
				{
					SetErrorCode (PAK_ERROR_READ_FAILURE);
					SetErrorString (pFile->GetFileName());

					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}
					return FALSE;
				}
				
				// Note that below, the NewPakFile parameter has been changed to our current one, and the OldPakFile
				// parameter is NULL
				if (!m_pRoot->Serialize (SERIALIZE_STAGE_ENTRY, pFile, NULL, &iDirOffset, &iCount, &ctlProgress))
				{
					// Rewrite the old directory, as something has gone wrong
					pFile->Seek (iDirOffset, CFile::begin);
					pFile->Write (pbyTempBuffer, iDirLen);

					// This is expected to be closed by us
					pFile->Abort();

					SetErrorString (m_pRoot->GetErrorString());
					SetErrorCode (m_pRoot->GetErrorCode());

					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}

					return FALSE;
				}

				// Note that below, the NewPakFile parameter has been changed to our current one, and the OldPakFile
				// parameter is NULL
				if (!m_pRoot->Serialize (SERIALIZE_STAGE_CLEANUP, pFile, NULL, &iDirOffset, &iCount, &ctlProgress))
				{
					// Rewrite the old directory, as something has gone wrong
					pFile->Seek (iDirOffset, CFile::begin);
					pFile->Write (pbyTempBuffer, iDirLen);

					// This is expected to be closed by us
					pFile->Abort();

					SetErrorString (m_pRoot->GetErrorString());
					SetErrorCode (m_pRoot->GetErrorCode());

					if (pbyTempBuffer)
					{
						delete []pbyTempBuffer;
						pbyTempBuffer = NULL;
					}

					return FALSE;
				}

				if (pbyTempBuffer)
				{
					delete []pbyTempBuffer;
					pbyTempBuffer = NULL;
				}
				ctlProgress.Clear();
			}

		}
		break;
	
	default:
		ASSERT (FALSE);			// Unhandled type... 0x3?  Must either READ or WRITE, not both, and not neither!
		return FALSE;
		break;
	}

	return TRUE;

}

BOOL CPakList::Serialize(LPCTSTR szPakFile, int iFlags)
{	
	int iSerializeType = iFlags & 0x3;

	switch (iSerializeType)
	{
	case SERIALIZE_READ:
		{
			CFile f;
			CFileException fe;
			
			if (!f.Open ( szPakFile, CFile::modeRead | CFile::typeBinary, &fe))
			{			
				m_strError.Format ("File could not be opened.\n%lu", fe.m_cause);
				SetErrorCode (PAK_ERROR_FILEOPEN);
				return FALSE;
			}
			
			return Serialize (&f, szPakFile, iFlags);
		}
		break;

	case SERIALIZE_WRITE:
		{
			CFile f;
			CFileException fe;
			
			if (!f.Open ( szPakFile, CFile::modeRead | CFile::typeBinary, &fe))
			{			
				m_strError.Format ("File could not be opened.\n%lu", fe.m_cause);
				SetErrorCode (PAK_ERROR_FILEOPEN);
				return FALSE;
			}
			
			return Serialize (&f, szPakFile, iFlags);
		}
		break;

	default:
		ASSERT (FALSE);			// Unhandled type... 0x3?  Must either READ or WRITE, not both, and not neither!
		return FALSE;
		break;
	}

	return TRUE;
}

UINT CPakList::GetItemCount()
{
	UINT iCount = 0;
	m_pRoot->GetItemCount(&iCount);
	
	return iCount;
}

CPakDirectory *CPakList::GetRootDirectory()
{
	return m_pRoot;
}

CString CPakList::GetTempDirectory()
{
	return m_strTempDirectory;
}

CString CPakList::GetFileName()
{
	return m_strFileName;
}

BOOL CPakList::CreateDirectory (CPakDirectory *pDirectory)
{
	if (!pDirectory->CreateTempDirectory (m_strFileName, m_strTempDirectory))
	{
		SetErrorCode (pDirectory->GetErrorCode());
		SetErrorString (pDirectory->GetErrorString());
		return FALSE;
	}

	return TRUE;
}

BOOL CPakList::CreateItem (CPakItem *pItem)
{
	// Try and create the temp file

	if (pItem->GetDataFile() == "")
	{
		if (!pItem->CreateTempFile (m_strFileName, m_strTempDirectory, PAK_EXPORT_FLAG_ADD_TEMP | PAK_EXPORT_FLAG_RETAIN))
		{
			SetErrorCode (pItem->GetErrorCode());
			SetErrorString (pItem->GetErrorString());
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CPakList::OpenItem (CPakItem *pItem)
{
	if (pItem->GetDataFile() != "")
	{
		if (!pItem->OpenItem())
		{
			SetErrorCode (pItem->GetErrorCode());
			SetErrorString (pItem->GetErrorString());
			return FALSE;
		}		

		return TRUE;
	}
	else
	{
		if (g_bPakQuickPlaySound)
		{
			CString strExtension = GetExtension (pItem->GetShortName());

			if (!strExtension.CompareNoCase (".wav"))
			{
				if (!pItem->QuickPlay(m_strFileName))
				{
					SetErrorCode (pItem->GetErrorCode());
					SetErrorString (pItem->GetErrorString());
					return FALSE;
				}
				return TRUE;
			}
		}
	}

	if (!CreateItem (pItem))
	{
		return FALSE;
	}	

	// Go open it
	if (!pItem->OpenItem())
	{
		SetErrorCode (pItem->GetErrorCode());
		SetErrorString (pItem->GetErrorString());
		return FALSE;
	}
	
	return TRUE;
}

CPakDirectory *CPakList::FindDirectory (LPCTSTR szDirectory)
{
	return m_pRoot->FindDirectory (szDirectory);
}

BOOL CPakList::IsModified(int *piFlags)
{
	return m_pRoot->IsModified(piFlags);
}

void CPakList::AddToPasteList(CPakBase *pItem)
{
	CPakPasteItem *pNewItem = new CPakPasteItem (pItem);

	if (pNewItem)
	{
		if (!GetFirstPasteItem())
		{
			SetFirstPasteItem (pNewItem);
			SetWritePasteItem (pNewItem);
		}
		else
		{
			GetWritePasteItem()->SetNext (pNewItem);
			pNewItem->SetPrevious (GetWritePasteItem());
			SetWritePasteItem(pNewItem);
		}
	}	
}

int CPakList::GetPasteCount()
{
	int iCount = 0;

	CPakPasteItem *pItem = GetFirstPasteItem();

	while (pItem)
	{
		iCount++;
		pItem = pItem->GetNext();
	}

	return iCount;
}

void CPakList::PurgePasteList()
{
	CPakPasteItem *pItem = GetFirstPasteItem();

	while (pItem)
	{
		SetFirstPasteItem (GetFirstPasteItem()->GetNext());
		delete pItem;
		pItem = GetFirstPasteItem();
	}

	SetFirstPasteItem (NULL);
	SetWritePasteItem (NULL);
}

void CPakList::SetFirstPasteItem (CPakPasteItem *pFirst)
{
	m_pFirstPasteItem = pFirst;
}

CPakPasteItem *CPakList::GetFirstPasteItem()
{
	return m_pFirstPasteItem;
}

void CPakList::SetWritePasteItem (CPakPasteItem *pWrite)
{
	m_pWritePasteItem = pWrite;
}

CPakPasteItem *CPakList::GetWritePasteItem()
{
	return m_pWritePasteItem;
}

CPakBase::CPakBase()
{
	SetErrorCode (PAK_ERROR_SUCCESS);
	memset (&m_PakParam, 0, sizeof (m_PakParam));
}

CString CPakBase::GetError()
{
	CString strErrorCode("");

	switch (GetErrorCode())
	{
	case PAK_ERROR_MALFORMED:
		{
			strErrorCode = "Pak file is malformed.";
		}
		break;

	case PAK_ERROR_FILEOPEN:
		{
			strErrorCode = GetErrorString();
		}
		break;

	case PAK_ERROR_OUTOFMEMORY:
		{
			strErrorCode = "Out of memory.";
		}
		break;

	case PAK_ERROR_NAME_EXCEED_LENGTH:
		{
			strErrorCode.Format ("The name %s exceeds the 55-character limit.\nTry using a shorter name, or cut down the names of the directories.", GetErrorString());
		}
		break;

	case PAK_ERROR_READ_FAILURE:
		{
			strErrorCode = "There was an unknown failure reading from ";
			strErrorCode += GetErrorString();
		}
		break;

	case PAK_ERROR_WRITE_FAILURE:
		{
			strErrorCode = "There was an unknown failure writing to ";
			strErrorCode += GetErrorString();
		}
		break;

	case PAK_ERROR_NO_ASSOC:
		{
			strErrorCode = "There is no application associated with this item.";
		}
		break;

	case PAK_ERROR_COPY_FILE:
		{
			strErrorCode = GetErrorString();
		}
		break;

	case PAK_ERROR_DUPLICATE_NAME:
		{
			strErrorCode.Format ("The filename %s already exists in the specified PAK directory.", GetErrorString());
		}
		break;

	case PAK_ERROR_INVALID_PAK:
		{
			strErrorCode = "This is not a valid Pak file.";
		}
		break;

	case PAK_ERROR_ZERO_LENGTH:
		{
			strErrorCode.Format ("The file %s has zero length.", GetErrorString());
		}
		break;

	case PAK_ERROR_WALLY_ERROR:
		{
			strErrorCode = "There was an internal Wally error.  Send a bug report!\n\n";
			strErrorCode += GetErrorString();
		}
		break;

	case PAK_ERROR_MAX_FILES:
		{
			strErrorCode.Format ("There are %s files in this pack.  The maximum allowed is %lu.\nRemove %lu entries and re-save.  If needed, start a new PAK and move the files there.", GetErrorString(), MAX_FILES_IN_PACK, atoi (GetErrorString()) - MAX_FILES_IN_PACK);
		}
		break;

	case PAK_ERROR_SUCCESS:
		ASSERT (FALSE);
		// Fall through...
	
	default:
		{
			ASSERT (FALSE);
			strErrorCode = "No error encountered!";
		}
		break;
	}

	return strErrorCode;
}



/*
/// TODO:

  - Fix it so that when an item gets deleted, or when a directory gets deleted, the temp stuff
  gets removed AT THE SAME TIME
	

  */