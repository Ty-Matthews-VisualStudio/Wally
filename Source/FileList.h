// FileList.h: interface for the CFileList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FILELIST_H_
#define _FILELIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CFileItem  
{
public:
	CFileItem(LPCTSTR szFileName);
	~CFileItem();

// Public methods
public:
	CFileItem *GetNext()
	{
		return m_pNext;
	}
	void SetNext (CFileItem *pNext)
	{
		m_pNext = pNext;
	}
	CFileItem *GetPrevious()
	{
		return m_pPrevious;
	}
	void SetPrevious(CFileItem *pPrevious)
	{
		m_pPrevious = pPrevious;
	}

	CString GetFileName()
	{
		return m_strFileName;
	}

private:
	CFileItem *m_pNext;
	CFileItem *m_pPrevious;

	CString m_strFileName;

};


class CFileList  
{
public:
	CFileList();
	~CFileList();
	void PurgeList();
	void AddFile (LPCTSTR szFileName);
	BOOL HasFile (LPCTSTR szFileName);

// Private methods
private:
	void SetFirst(CFileItem *pFirst)
	{
		m_pFirst = pFirst;
	}
	void SetWrite(CFileItem *pWrite)
	{
		m_pWrite = pWrite;
	}
public:
	CFileItem *GetFirst()
	{
		return m_pFirst;
	}
	CFileItem *GetWrite()
	{
		return m_pWrite;
	}

// Private members
private:
	CFileItem *m_pFirst;
	CFileItem *m_pWrite;
};


#endif // #ifndef _FILELIST_H_
