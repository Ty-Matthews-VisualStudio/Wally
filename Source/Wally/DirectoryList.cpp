// DirectoryList.cpp: implementation of the CDirectoryList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "DirectoryList.h"
#include <set>

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
	m_bRecurseSubdirectories = bRecurseSubdirectories;

	m_strRoot = "";
}

void CDirectoryList::SearchDirectories()
{	
	ASSERT (m_strRoot != "");

	m_Files.clear();
	SearchDirectory (m_strRoot);
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

int CDirectoryList::GetFileCount()
{
	return (int)m_Files.size();
}

void CDirectoryList::GetAllFiles (CStringArray *pStringArray)
{
	for (const CString &strFile : m_Files)
	{
		pStringArray->Add (strFile);
	}
}

void CDirectoryList::SearchDirectory (const CString &strDirectory)
{
	struct _finddata_t c_file;
	intptr_t hFile;

	CString strSearch ("");
	CString strCompare ("");

	// Look for subdirectories first
	if (RecurseSubdirectories())
	{
		strSearch.Format ("%s\\*.*", strDirectory);
		
		if ((hFile = _findfirst (strSearch, &c_file)) != -1)
		{
			do
			{
				strCompare = c_file.name;
				
				if ((c_file.attrib & _A_SUBDIR) && (strCompare != ".") && (strCompare != ".."))
				{
					CString strSubDirectory ("");
					strSubDirectory.Format ("%s\\%s", strDirectory, c_file.name);
					SearchDirectory (strSubDirectory);
				}
			}
			while (_findnext (hFile, &c_file) == 0);

			_findclose (hFile);
		}
	}

	// More than one wildcard can match the same file
	std::set<CString> setSeen;

	for (int j = 0; j < m_saWildcards.GetSize(); j++)
	{
		strSearch.Format ("%s\\%s", strDirectory, m_saWildcards.GetAt(j));
    		
		if ((hFile = _findfirst (strSearch, &c_file)) != -1)
		{
			do
			{
				if ((c_file.attrib & _A_SUBDIR) == 0)
				{
					strCompare.Format ("%s\\%s", strDirectory, c_file.name);

					CString strKey (strCompare);
					strKey.MakeUpper();

					if (setSeen.insert (strKey).second)
					{
						m_Files.push_back (strCompare);
					}
				}
			}
			while (_findnext (hFile, &c_file) == 0);

			_findclose (hFile);
		}
	}
}
