
#ifndef _WNDLIST_H_
#define _WNDLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define INVALIDATE_PALETTE_CHANGE				0x0001
#define INVALIDATE_AMOUNT_CHANGE				0x0002
#define INVALIDATE_DECAL_SELECTION_CHANGE		0x0004


class CWndItem
{
// Members
private:
	CWnd *m_pWnd;
	int m_iInvalidateFlag;
	CWndItem *m_pNext;
	CWndItem *m_pPrevious;

// Methods
public:
	CWndItem (CWnd *pWnd, int iFlag);	
	~CWndItem ();

	void SetNext (CWndItem *pNext);
	CWndItem *GetNext();
	void SetPrevious (CWndItem *pPrevious);
	CWndItem *GetPrevious();

	void Invalidate(int iFlag);
	CWnd *GetWnd();

};

class CWndList
{
// Members
private:
	CWndItem *m_pFirst;
	CWndItem *m_pRead;
	CWndItem *m_pWrite;

// Methods
public:
	CWndList ();
	~CWndList ();

	void AddItem (CWnd *pWnd, int iFlag);

	void SetFirst (CWndItem *pFirst);
	CWndItem *GetFirst ();
	void SetRead (CWndItem *pRead);
	CWndItem *GetRead ();
	void SetWrite (CWndItem *pWrite);
	CWndItem *GetWrite ();
	void SetAll (CWndItem *pWnd);

	void PurgeList();
	void InvalidateAll(int iFlag);
	bool IsInList (CWnd *pWnd);

};



#endif		// _WNDLIST_H_