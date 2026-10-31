// ImageIconList.h: interface for the CImageIconItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _IMAGEICONLIST_H_
#define _IMAGEICONLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//	CImageIconList is a class that wraps CImageList.  It's main function is to reduce the number of icons needed for 
//  a CListCtrl.  As each icon is added to the list, it is first checked against it's internal map for a possible
//	match.  Each known file type has a unique system icon number, which is returned on calls to SHGetFileInfo().  Once
//	a particular icon has been added, it does not make sense to have to add it again.  If a match is found, that index
//	is returned to the caller without ever adding a new item to the list.

class CImageIconList : public CImageList
{
private:
	std::unordered_map<int, int> m_mapSystemToListIcon;

public:
	int FindMatch (int iSystemIcon);
	void AddToList (int iSystemIcon, int iListIcon);

	int Add( CBitmap* pbmImage, CBitmap* pbmMask, int iSystemIcon = -1);
	int Add( CBitmap* pbmImage, COLORREF crMask, int iSystemIcon = -1);
	int Add( HICON hIcon, int iSystemIcon = -1);
};

#endif // #ifndef _IMAGEICONLIST_H_
