// BrowseDIBList.h: interface for the CBrowseDIBList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BROWSEDIBLIST_H__DA7A1C41_B54D_11D1_8068_5C2203C10627__INCLUDED_)
#define AFX_BROWSEDIBLIST_H__DA7A1C41_B54D_11D1_8068_5C2203C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DibSection.h"

extern bool	g_bOutOfMemory;

////////////////////////////////////////////////////////////////////////////////
//  Name:		Class CBrowseDIBItem
//  Action:		This class wraps the CDibSection class.  The additional 
//				functionality here is the linked list support, some default
//				handling characteristics, and the overriden constructor

class CBrowseDIBItem : public CDibSection
{
// Members
public:
	CBrowseDIBItem* p_Next;
	CBrowseDIBItem* p_Previous;
	CString m_Name;	
	CString m_FileName;

// Methods
public:
	CBrowseDIBItem(CString *m_WalFileName);
	virtual ~CBrowseDIBItem();	
	
	void Show (CDC* ActiveWindow, int X, int Y)
	// Base class override
	{
		CDibSection::Show(ActiveWindow, X, Y, 1);
	}

	void ShowRestricted (CDC* ActiveWindow, int X, int Y, int Size);
}; 


////////////////////////////////////////////////////////////////////////////////
//  Name:		Class CBrowseDIBList
//  Action:		This class maintains a linked list of CBrowseDIBItems

class CBrowseDIBList  
{
// Members
public:
	CBrowseDIBItem *p_First;
	CBrowseDIBItem *p_Read;
	CBrowseDIBItem *p_Write;	

// Methods
public:
	CBrowseDIBList();
	virtual ~CBrowseDIBList();	
	CBrowseDIBItem *AddItem(CString *FileName);	
	bool IsEmpty ()
	{
		return (p_First == NULL);
	}

	void PurgeList();	
	int GetLength();	
	CBrowseDIBItem* GetFirst();
	CBrowseDIBItem* GetNext();
	CBrowseDIBItem* GetAtPosition(int Position);

};

#endif // !defined(AFX_BROWSEDIBLIST_H__DA7A1C41_B54D_11D1_8068_5C2203C10627__INCLUDED_)
