#ifndef _WALLY_LIST_H_
#define _WALLY_LIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWallyItem
{
// Private Members
private:
	BYTE *m_pbyData;
	int m_iItemSize;
	int m_iItemCategory;
	int m_iItemType;
	int m_iItemFlags;
	
	CWallyItem *m_pNext;
	CWallyItem *m_pPrevious;

// Public Methods
public:
	CWallyItem(int iItemType, int iItemSize, int iItemFlags, BYTE *pbyData);
	~CWallyItem();

	void PurgeData();

	BYTE *GetItemData();	

	int GetItemSize();
	void SetItemSize(int iSize);	

	int GetItemCategory();
	void SetItemCategory(int iCategory);
	
	int GetItemType();
	void SetItemType(int iType);

	int GetItemFlags();
	void SetItemFlags(int iFlags);
	
	CWallyItem *GetNext();
	void SetNext(CWallyItem *pNext);
	
	CWallyItem *GetPrevious();
	void SetPrevious(CWallyItem *pPrevious);


};

class CWallyList
{
// Private Members
private:
	CWallyItem *m_pFirst;
	CWallyItem *m_pRead;
	CWallyItem *m_pWrite;

// Public Methods
public:
	CWallyList();
	~CWallyList();

	// Linked List routines
	void SetAll (CWallyItem *pItem);	
	void SetFirst (CWallyItem *pFirst);
	CWallyItem *GetFirst ();
	void SetRead (CWallyItem *pRead);
	CWallyItem *GetRead ();
	void SetWrite (CWallyItem *pWrite);
	CWallyItem *GetWrite ();	
	void PurgeList();

	void AddItem (int iItemType, int iItemSize, int iItemFlags, BYTE *pbyData);

	int Serialize (LPCTSTR szFileName, CFile *pFile, BYTE *pbyData, bool bSave = false);

};



#endif		// _WALLY_LIST_H_
