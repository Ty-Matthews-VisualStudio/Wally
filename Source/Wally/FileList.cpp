// FileList.cpp: implementation of the CFileList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Wally.h"
#include "FileList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileList::CFileList()
{
	SetFirst (NULL);
	SetWrite (NULL);
}

CFileList::~CFileList()
{
	PurgeList();
}

void CFileList::PurgeList()
{
	CFileItem *pTemp = GetFirst();

	while (pTemp)
	{
		SetFirst (pTemp->GetNext());
		delete pTemp;
		pTemp = GetFirst();			
	}

	pTemp = NULL;
	SetFirst (NULL);
	SetWrite (NULL);
}

void CFileList::AddFile (LPCTSTR szFileName)
{
	// Check to see if it's there already
	CFileItem *pItem = GetFirst();
	CString strCompare(szFileName);

	while (pItem)
	{
		if (!strCompare.CompareNoCase (pItem->GetFileName()))
		{
			// We're already there; skip adding it
			return;
		}
		pItem = pItem->GetNext();
	}

	CFileItem *pNewItem = new CFileItem (szFileName);

	if (!GetFirst())
	{
		SetFirst (pNewItem);
		SetWrite (pNewItem);
	}
	else
	{
		GetWrite()->SetNext(pNewItem);
		pNewItem->SetPrevious(GetWrite());
		SetWrite(pNewItem);
	}

	pNewItem = NULL;
}

BOOL CFileList::HasFile (LPCTSTR szFileName)
{
	CString strCompare1 (szFileName);
	strCompare1.MakeUpper();
	
	CString strCompare2 ("");

	CFileItem *pTemp = GetFirst();

	while (pTemp)
	{
		strCompare2 = pTemp->GetFileName();
		strCompare2.MakeUpper();
		
		if (strCompare2 == strCompare1)
		{
			pTemp = NULL;
			return TRUE;
		}
		
		pTemp = pTemp->GetNext();
	}

	pTemp = NULL;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// CFileItem Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileItem::CFileItem(LPCTSTR szFileName)
{
	SetNext (NULL);
	SetPrevious (NULL);

	m_strFileName = szFileName;
}

CFileItem::~CFileItem()
{
	SetNext (NULL);
	SetPrevious (NULL);
}
