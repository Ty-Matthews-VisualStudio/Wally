// ImageIconList.h: interface for the CImageIconItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _IMAGEICONLIST_H_
#define _IMAGEICONLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CImageIconItem  
{
private:
	CImageIconItem *m_pNext;
	CImageIconItem *m_pPrevious;

	int m_iSystemIcon;
	int m_iListIcon;

public:
	CImageIconItem(int iSystemIcon, int iListIcon);
	virtual ~CImageIconItem();

	void SetNext (CImageIconItem *pNext);
	CImageIconItem *GetNext();
	void SetPrevious (CImageIconItem *pPrevious);
	CImageIconItem *GetPrevious();

	int GetSystemIcon()
	{
		return m_iSystemIcon;
	}
	int GetListIcon()
	{
		return m_iListIcon;
	}

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//	CImageIconList is a class that wraps CImageList.  It's main function is to reduce the number of icons needed for 
//  a CListCtrl.  As each icon is added to the list, it is first checked against it's internal linked list for a possible
//	match.  Each known file type has a unique system icon number, which is returned on calls to SHGetFileInfo().  Once
//	a particular icon has been added, it does not make sense to have to add it again.  If a match is found, that index
//	is returned to the caller without ever adding a new item to the list.

class CImageIconList : public CImageList
{
private:
	CImageIconItem *m_pFirst;
	CImageIconItem *m_pWrite;

public:
	CImageIconList();
	virtual ~CImageIconList();

	CImageIconItem *GetFirst();
	void SetFirst (CImageIconItem *pFirst);
	CImageIconItem *GetWrite();
	void SetWrite (CImageIconItem *pWrite);

	void PurgeAll();
	int FindMatch (int iSystemIcon);
	void AddToList (int iSystemIcon, int iListIcon);

	int Add( CBitmap* pbmImage, CBitmap* pbmMask, int iSystemIcon = -1);
	int Add( CBitmap* pbmImage, COLORREF crMask, int iSystemIcon = -1);
	int Add( HICON hIcon, int iSystemIcon = -1);
};

#endif // #ifndef _IMAGEICONLIST_H_
