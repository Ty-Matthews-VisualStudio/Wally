// DIBToDocHelper.h: interface for the CDibSection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DIB_DOC_HELPER_H__
#define __DIB_DOC_HELPER_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;
class CBrowseDIBItem;
class CBrowseDocument;	

class CWallyDocPTR
{
// Members
public:
	CWallyDoc		*p_WallyDoc;
	CWallyDocPTR	*p_Next;
	CWallyDocPTR	*p_Previous;

// Implementation
public:
	CWallyDocPTR (CWallyDoc *p_Doc)
	{
		p_WallyDoc = p_Doc;
		p_Next = p_Previous = NULL;
	}
	~CWallyDocPTR ()
	{
		p_WallyDoc = NULL;
		p_Next = p_Previous = NULL;
	}
};

class CDIBToDocItem
{
// Members
public:
	CWallyDocPTR	*p_FirstWallyDoc;
	CWallyDocPTR	*p_ReadWallyDoc;
	CWallyDocPTR	*p_WriteWallyDoc;
	CBrowseDIBItem	*p_BrowseDIB;
	CBrowseDocument *p_BrowseDoc;
	CDIBToDocItem	*p_Next;
	CDIBToDocItem	*p_Previous;

// Methods
public:
	CDIBToDocItem(CBrowseDocument *p_Doc, CBrowseDIBItem *p_DIB)
	{
		p_FirstWallyDoc = p_ReadWallyDoc = p_WriteWallyDoc = NULL;
		p_BrowseDIB = p_DIB;
		p_BrowseDoc = p_Doc;
		p_Next = p_Previous = NULL;
	}
	~CDIBToDocItem()
	{		
		FlushList();		
		p_BrowseDIB = NULL;
		p_BrowseDoc = NULL;
		p_Next = p_Previous = NULL;
	}
	bool IsEmpty()
	{
		return (p_FirstWallyDoc == NULL);
	}
	void OpenDocument(CString FileName);
	void CloseDocument(CWallyDoc *p_Doc);
	void UpdateDIB(unsigned char *p_Data, int iSize);
	void FlushList();
};

class CDIBToDocHelper
{
// Members
public:
	CDIBToDocItem *p_First;
	CDIBToDocItem *p_Read;
	CDIBToDocItem *p_Write;

// Methods
public:
	CDIBToDocHelper ()
	{
		p_First = p_Read = p_Write = NULL;
	}
	~CDIBToDocHelper ()
	{
		FlushList();		
	}
	bool IsEmpty()
	{
		return (p_First == NULL);
	}
	void AddDIBItem(CBrowseDocument *p_Doc, CBrowseDIBItem *p_DIB);	
	void AddWallyItem(CString FileName, CBrowseDIBItem *p_DIB);	
	void FlushList();

};



#endif