// DirectoryList.h: interface for the CDirectoryList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DIRECTORYLIST_H_
#define _DIRECTORYLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _FILELIST_H_
	#include "FileList.h"
#endif

class CDirectoryList;

class CDirectoryEntry  
{
public:
	CDirectoryEntry(LPCTSTR szDirectory, CDirectoryList *pParent);
	~CDirectoryEntry()
	{
		SetNext (NULL);
		SetPrevious (NULL);
	}
	CString GetDirectory()
	{
		return m_strDirectory;
	}

public:
	CDirectoryEntry *GetNext()
	{
		return m_pNext;
	}
	void SetNext (CDirectoryEntry *pNext)
	{
		m_pNext = pNext;
	}
	CDirectoryEntry *GetPrevious()
	{
		return m_pPrevious;
	}
	void SetPrevious(CDirectoryEntry *pPrevious)
	{
		m_pPrevious = pPrevious;
	}
	CFileList *GetFileList()
	{
		return &m_FileList;
	}
	
	void AddFile (LPCTSTR szFileName);
	BOOL HasFile (LPCTSTR szFileName);

private:
	CDirectoryEntry *m_pNext;
	CDirectoryEntry *m_pPrevious;

	CString m_strDirectory;
	CDirectoryList *m_pParent;

	CFileList m_FileList;
};

class CDirectoryList  
{
public:
	CDirectoryList(BOOL bRecurseSubdirectories = FALSE);
	~CDirectoryList();
	void PurgeList();
	void AddDirectory (LPCTSTR szDirectory);
	int FindFile (LPCTSTR szFile, CStringArray *pStringArray);
	void SetRoot (LPCTSTR szRoot)
	{
		m_strRoot = szRoot;

		int iPosition = m_strRoot.GetLength() - 1;

		while (m_strRoot.GetAt(iPosition) == '\\')
		{
			iPosition--;
		}
		
		iPosition++;

		m_strRoot = m_strRoot.Left(iPosition);
	}
	void SearchDirectories();
	void AddWildcard (LPCTSTR szWildcard);
	void ClearWildcards ();
	CStringArray *GetWildcards();

	int GetFileCount();
	void GetAllFiles (CStringArray *pStringArray);

	BOOL RecurseSubdirectories()
	{
		return m_bRecurseSubdirectories;
	}
	
// Private methods
private:
	void SetFirst(CDirectoryEntry *pFirst)
	{
		m_pFirst = pFirst;
	}
	void SetWrite(CDirectoryEntry *pWrite)
	{
		m_pWrite = pWrite;
	}

public:
	CDirectoryEntry *GetFirst()
	{
		return m_pFirst;
	}	
	CDirectoryEntry *GetWrite()
	{
		return m_pWrite;
	}

// Private members
private:
	CDirectoryEntry *m_pFirst;
	CDirectoryEntry *m_pWrite;
	CStringArray m_saWildcards;

	BOOL m_bRecurseSubdirectories;

	CString m_strRoot;

};



#endif // #ifndef _DIRECTORYLIST_H_
