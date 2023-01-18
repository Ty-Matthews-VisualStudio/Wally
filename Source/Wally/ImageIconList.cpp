// ImageIconList.cpp: implementation of the CImageIconItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "ImageIconList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageIconItem::CImageIconItem(int iSystemIcon, int iListIcon)
{
	m_iSystemIcon = iSystemIcon;
	m_iListIcon = iListIcon;

	SetNext (NULL);
	SetPrevious (NULL);
}

CImageIconItem::~CImageIconItem()
{
	SetNext (NULL);
	SetPrevious (NULL);
}

void CImageIconItem::SetNext (CImageIconItem *pNext)
{
	m_pNext = pNext;
}

CImageIconItem *CImageIconItem::GetNext()
{
	return m_pNext;
}

void CImageIconItem::SetPrevious (CImageIconItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CImageIconItem *CImageIconItem::GetPrevious()
{
	return m_pPrevious;
}

//////////////////////////////////////////////////////////////////////
// CImageIconList Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageIconList::CImageIconList()
{
	CImageList::CImageList();

	SetFirst (NULL);
	SetWrite (NULL);
}

CImageIconList::~CImageIconList()
{
	CImageList::~CImageList();
	PurgeAll();
}

void CImageIconList::PurgeAll()
{
	CImageIconItem *pTemp = GetFirst();

	while (pTemp)
	{
		SetFirst (GetFirst()->GetNext());
		delete pTemp;
		pTemp = GetFirst();
	}

	SetFirst (NULL);
	SetWrite (NULL);
}

CImageIconItem *CImageIconList::GetFirst()
{
	return m_pFirst;
}

void CImageIconList::SetFirst (CImageIconItem *pFirst)
{
	m_pFirst = pFirst;
}

CImageIconItem *CImageIconList::GetWrite()
{
	return m_pWrite;
}

void CImageIconList::SetWrite (CImageIconItem *pWrite)
{
	m_pWrite = pWrite;
}

int CImageIconList::FindMatch (int iSystemIcon)
{
	CImageIconItem *pItem = GetFirst();

	while (pItem)
	{
		if (pItem->GetSystemIcon() == iSystemIcon)
		{
			return pItem->GetListIcon();
		}
		pItem = pItem->GetNext();
	}

	return -1;
}

int CImageIconList::Add( CBitmap* pbmImage, CBitmap* pbmMask, int iSystemIcon /* = -1 */)
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (pbmImage, pbmMask);
	}

	int iListIcon = CImageList::Add (pbmImage, pbmMask);
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}

int CImageIconList::Add( CBitmap* pbmImage, COLORREF crMask, int iSystemIcon /* = -1 */ )
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (pbmImage, crMask);
	}

	int iListIcon = CImageList::Add (pbmImage, crMask);
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}

int CImageIconList::Add( HICON hIcon, int iSystemIcon /* = -1 */ )
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (hIcon);
	}

	int iListIcon = CImageList::Add (hIcon);	
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}


void CImageIconList::AddToList (int iSystemIcon, int iListIcon)
{
	CImageIconItem *pItem = new CImageIconItem (iSystemIcon, iListIcon);

	if (pItem)
	{
		if (!GetFirst())
		{
			SetFirst (pItem);
			SetWrite (pItem);
		}
		else
		{
			GetWrite()->SetNext(pItem);
			pItem->SetPrevious (GetWrite());
			SetWrite (pItem);
		}
	}
}