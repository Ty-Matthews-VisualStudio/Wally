// DirectoryList.cpp: implementation of the CDirectoryList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "DirectoryList.h"
#include "FileList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectoryList::CDirectoryList(BOOL bRecurseSubdirectories /* = FALSE */)
{
	SetFirst (NULL);
	SetWrite (NULL);

	m_bRecurseSubdirectories = bRecurseSubdirectories;

	m_strRoot = "";
}

CDirectoryList::~CDirectoryList()
{	
	PurgeList();
}

void CDirectoryList::PurgeList()
{
	CDirectoryEntry *pTemp = NULL;

	pTemp = GetFirst();

	while (pTemp)
	{
		SetFirst(pTemp->GetNext());
		delete pTemp;
		pTemp = GetFirst();
	}

	pTemp = NULL;
	SetFirst (NULL);
	SetWrite (NULL);
}

void CDirectoryList::AddDirectory(LPCTSTR szDirectory)
{
	CDirectoryEntry *pNewDirectory = new CDirectoryEntry(szDirectory, this);

	if (!GetFirst())
	{
		SetFirst (pNewDirectory);
		SetWrite (pNewDirectory);		
	}
	else
	{
		GetWrite()->SetNext(pNewDirectory);
		pNewDirectory->SetPrevious(GetWrite());
		SetWrite(pNewDirectory);
	}

	pNewDirectory = NULL;
}

int CDirectoryList::FindFile (LPCTSTR szFile, CStringArray *pStringArray)
{
	CDirectoryEntry *pTemp = GetFirst();
	CString strFileName("");
	CString strDirectory("");	
	FILE *rp = NULL;

	while (pTemp)
	{
		if (pTemp->HasFile(szFile))
		{
			strDirectory = pTemp->GetDirectory();
			
			strFileName.Format ("%s\\%s", strDirectory, szFile);

			// Make sure it's really there
			errno_t err = fopen_s(&rp, strFileName, "rb");

			if (err == 0)
			{
				// Add the directory to the StringArray
				pStringArray->Add(strDirectory);				
				fclose (rp);
			}			
		}
		pTemp = pTemp->GetNext();
	}

	return pStringArray->GetSize();
}

void CDirectoryList::SearchDirectories()
{	
	ASSERT (m_strRoot != "");

	PurgeList();
	AddDirectory (m_strRoot);
}

void CDirectoryList::AddWildcard (LPCTSTR szWildcard)
{
	ASSERT (szWildcard);
	m_saWildcards.Add (szWildcard);
}

void CDirectoryList::ClearWildcards ()
{
	m_saWildcards.RemoveAll();
}

CStringArray *CDirectoryList::GetWildcards()
{
	return &m_saWildcards;
}

int CDirectoryList::GetFileCount()
{
	CDirectoryEntry *pDir = GetFirst();
	CFileList *pList = NULL;
	CFileItem *pFile = NULL;
	int iCount = 0;

	while (pDir)
	{
		pList = pDir->GetFileList();
		pFile = pList->GetFirst();

		while (pFile)
		{
			iCount++;
			pFile = pFile->GetNext();
		}

		pDir = pDir->GetNext();
	}

	return iCount;
}

void CDirectoryList::GetAllFiles (CStringArray *pStringArray)
{
	CDirectoryEntry *pDir = GetFirst();
	CFileList *pList = NULL;
	CFileItem *pFile = NULL;

	while (pDir)
	{
		pList = pDir->GetFileList();
		pFile = pList->GetFirst();

		while (pFile)
		{
			pStringArray->Add (pFile->GetFileName());			
			pFile = pFile->GetNext();
		}

		pDir = pDir->GetNext();
	}
}

//////////////////////////////////////////////////////////////////////
// CDirectoryEntry Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectoryEntry::CDirectoryEntry(LPCTSTR szDirectory, CDirectoryList *pParent)
{
	SetNext (NULL);
	SetPrevious (NULL);
	m_strDirectory = szDirectory;
	m_pParent = pParent;

	ASSERT (pParent);
	ASSERT (szDirectory);

	struct _finddata_t c_file;
	long hFile;

	CString strSearch ("");
	CString strDirectory ("");
	CString strCompare ("");
	CString strWildcard ("");

	CStringArray *pWildcards = pParent->GetWildcards();
	int j = 0;

	// Look for subdirectories first
	if (pParent->RecurseSubdirectories())
	{
		strSearch.Format ("%s\\*.*", m_strDirectory);
		
		if( (hFile = _findfirst( strSearch.GetBuffer(strSearch.GetLength()), &c_file )) != -1L )
		{       
			strCompare = c_file.name;
			
			if (c_file.attrib & _A_SUBDIR)
			{
				if ((strCompare != ".") && (strCompare != ".."))
				{
					strDirectory.Format ("%s\\%s", m_strDirectory, c_file.name);
					m_pParent->AddDirectory (strDirectory);
				}			
			}
			
			while( _findnext( hFile, &c_file ) == 0 )
			{
				strCompare = c_file.name;
				
				if (c_file.attrib & _A_SUBDIR)
				{					
					if ((strCompare != ".") && (strCompare != ".."))
					{
						strDirectory.Format ("%s\\%s", m_strDirectory, c_file.name);
						m_pParent->AddDirectory (strDirectory);
					}					
				}				
			}
		}		// if( (hFile = _wfindfirst( (LPTSTR)(strSearch.GetBuffer(strSearch.GetLength())), &c_file )) != -1L )
	}
    	

	for (j = 0; j < pWildcards->GetSize(); j++)
	{
		strWildcard = pWildcards->GetAt(j);

		strSearch.Format ("%s\\%s", m_strDirectory, strWildcard);
    		
		if( (hFile = _findfirst( strSearch.GetBuffer(strSearch.GetLength()), &c_file )) != -1L )
		{       
			strCompare = c_file.name;
			
			if ((c_file.attrib & _A_SUBDIR) == 0)
			{
				/*if (pParent->RecurseSubdirectories())
				{
					if ((strCompare != ".") && (strCompare != ".."))
					{
						strDirectory.Format ("%s\\%s", m_strDirectory, c_file.name);
						m_pParent->AddDirectory (strDirectory);
					}
				}
			}
			else
			{*/
				strCompare.Format ("%s\\%s", m_strDirectory, c_file.name);
				AddFile(strCompare);
			}
            
			while( _findnext( hFile, &c_file ) == 0 )
			{
				strCompare = c_file.name;
				
				if ((c_file.attrib & _A_SUBDIR) == 0)
				{					
					/*if (pParent->RecurseSubdirectories())
					{
						if ((strCompare != ".") && (strCompare != ".."))
						{
							strDirectory.Format ("%s\\%s", m_strDirectory, c_file.name);
							m_pParent->AddDirectory (strDirectory);
						}
					}
				}
				else
				{*/
					strCompare.Format ("%s\\%s", m_strDirectory, c_file.name);
					AddFile(strCompare);
				}
			}
		}		// if( (hFile = _wfindfirst( (LPTSTR)(strSearch.GetBuffer(strSearch.GetLength())), &c_file )) != -1L )
	}			// for (j = 0; j < pWildcards->GetSize(); j++)
}

void CDirectoryEntry::AddFile (LPCTSTR szFileName)
{
	m_FileList.AddFile (szFileName);
}

BOOL CDirectoryEntry::HasFile (LPCTSTR szFileName)
{
	return m_FileList.HasFile (szFileName);
}