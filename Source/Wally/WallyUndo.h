/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyUndo.h : implementation of the CWallyUndo class
//
// Created by Neal White III, 1-1-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef __AFXTEMPL_H__
	#include <AfxTempl.h>
#endif

#ifndef _WALLYUNDO_H_
#define _WALLYUNDO_H_

#define MAX_UNDO_ITEMS 50

#define UNDO_DOCUMENT		1001
#define UNDO_PALETTE		1002
#define UNDO_DOC_AND_PAL	1003

class CWallyDoc;

class CUndoState
{
// Constructor/Destructor
public:

    CUndoState();
	virtual ~CUndoState();

// Member Vars
public:
	int     m_iUndoType;
	BOOL    m_bCompressed;
	BOOL    m_bModified;
	CString m_strUndoString;
	HGLOBAL m_hGlobal;
	VOID*   m_pVoid;		// can be used as a ptr to a list, etc.

// Operations
public:

// Implementation
public:
};

/////////////////////////////////////////////////////////////////////////////

typedef CTypedPtrList<CPtrList, CUndoState*> UndoStateList;
typedef BOOL (CALLBACK* UNDO_CALLBACK)( CWallyDoc* pDoc, CUndoState* pState);

/////////////////////////////////////////////////////////////////////////////
// CWallyUndo - easy undo via Serialize and SetModifiedFlag

class CWallyUndo
{
// Constructors
public:

    CWallyUndo();
	virtual ~CWallyUndo();

// Member Vars
protected:
	int  m_iMaxItems;
	BOOL m_bEnabled;
	BOOL m_bInProgress;
	static int m_iNumUndosInProgress;

	CWallyDoc*     m_pDoc;
	UndoStateList* m_pUndoList;
	UndoStateList* m_pRedoList;

// Operations
public:
	BOOL SaveUndoState( CWallyDoc* pDoc, LPCSTR lpszMenuString, int iType = UNDO_DOCUMENT);
	BOOL SaveOrDeleteUndoState( BOOL bCondition, CWallyDoc* pDoc, CUndoState* pState);
	BOOL RestoreState( CUndoState* pState);
	CUndoState* GetLastState();

	CUndoState* SaveCurrentState( CWallyDoc* pDoc, LPCSTR lpszMenuString, int iType = UNDO_DOCUMENT);
	CUndoState* SaveCurrentState( CWallyDoc* pDoc, int iMenuStringID, int iType = UNDO_DOCUMENT);

	BOOL DoUndo( void);
	BOOL DoRedo( void);

	void MarkAsModified( void);
	void EnableUndo( BOOL bEnable)  { m_bEnabled = bEnable; };
	void ClearUndoBuffer( void);	// get rid of UNDO buffer (frees mem)
	void ClearRedoBuffer( void);	// get rid of REDO buffer (frees mem)

	int  GetMaxItems( void)  { return m_iMaxItems; };
	void SetMaxItems( int iMaxItems);

	LPCSTR GetUndoString( void) const;
	LPCSTR GetRedoString( void) const;

	BOOL CanUndo( void)      const;
	BOOL CanRedo( void)      const;
	BOOL IsEnabled( void)    const         { return m_bEnabled; };
	BOOL IsInProgress( void) const         { return m_bInProgress; };
	BOOL IsAnyUndoInProgress( void) const  { return (m_iNumUndosInProgress > 0); };
	BOOL NowInProgress( void);
	void NoLongerInProgress( BOOL bOldProgress);

// Implementation
protected:
	BOOL UpdateListsAndRestoreState( UndoStateList* pSrcList, UndoStateList* pDestList);
};

#endif		// _WALLYUNDO_H_