// DirectoryList.h: interface for the CDirectoryList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DIRECTORYLIST_H_
#define _DIRECTORYLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>

class CDirectoryList  
{
public:
	CDirectoryList(BOOL bRecurseSubdirectories = FALSE);

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

	// Full paths, with each subdirectory's files ahead of its parent's
	const std::vector<CString> &GetFiles() const
	{
		return m_Files;
	}
	int GetFileCount();
	void GetAllFiles (CStringArray *pStringArray);

	BOOL RecurseSubdirectories()
	{
		return m_bRecurseSubdirectories;
	}

// Private methods
private:
	void SearchDirectory (const CString &strDirectory);

// Private members
private:
	std::vector<CString> m_Files;
	CStringArray m_saWildcards;

	BOOL m_bRecurseSubdirectories;

	CString m_strRoot;
};



#endif // #ifndef _DIRECTORYLIST_H_
