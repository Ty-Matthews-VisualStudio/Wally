
#ifndef _WNDLIST_H_
#define _WNDLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <vector>

#define INVALIDATE_PALETTE_CHANGE				0x0001
#define INVALIDATE_AMOUNT_CHANGE				0x0002
#define INVALIDATE_DECAL_SELECTION_CHANGE		0x0004


class CWndItem
{
// Members
private:
	CWnd *m_pWnd;
	int m_iInvalidateFlag;

// Methods
public:
	CWndItem (CWnd *pWnd, int iFlag);	

	void Invalidate(int iFlag);
	CWnd *GetWnd();

};

class CWndList
{
// Members
private:
	std::vector<CWndItem> m_Items;

// Methods
public:
	void AddItem (CWnd *pWnd, int iFlag);

	void PurgeList();
	void InvalidateAll(int iFlag);
	bool IsInList (CWnd *pWnd);

};



#endif		// _WNDLIST_H_