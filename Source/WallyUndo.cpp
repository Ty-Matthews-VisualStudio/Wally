/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyUndo.cpp : implementation of the CWallyUndo class
//
// Created by Neal White III, 1-1-1998
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <AfxPriv.h>

#include "Wally.h"
#include "WallyDoc.h"
#include "WallyUndo.h"

extern CWallyApp theApp;

int CWallyUndo::m_iNumUndosInProgress = 0;

/////////////////////////////////////////////////////////////////////////////
// Name:        CUndoState
// Action:      CUndoState constructor
/////////////////////////////////////////////////////////////////////////////
CUndoState::CUndoState()
{
	m_iUndoType   = -1;
	m_bModified   = TRUE;
	m_bCompressed = FALSE;
	m_hGlobal     = NULL;
	m_pVoid       = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        ~CUndoState
// Action:      CUndoState destructor
/////////////////////////////////////////////////////////////////////////////
CUndoState::~CUndoState()
{
	// clean up
	m_strUndoString.Empty();

	// free allocated memory block?
	if (m_hGlobal != NULL)
	{
		GlobalUnlock( m_hGlobal);

		if (GlobalFree( m_hGlobal) != NULL)
		{
			ASSERT( FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:        RemoveAll
// Action:      Removes all elements and deallocates the memory used by list
/////////////////////////////////////////////////////////////////////////////
void RemoveAll( UndoStateList* pList)
{
	ASSERT_VALID( pList);

	// destroy elements
    while (! pList->IsEmpty())
    {
        delete (pList->RemoveHead());
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        CWallyUndo
// Action:      CWallyUndo constructor
/////////////////////////////////////////////////////////////////////////////
CWallyUndo::CWallyUndo()
{
	//m_iMaxItems   = MAX_UNDO_ITEMS;

	// store max number of undos in ini file (where user can change it)
	CString WallyKey( "Settings");

	m_iMaxItems = theApp.GetProfileInt( WallyKey, "MaxUndoItems", MAX_UNDO_ITEMS);
	theApp.WriteProfileInt(             WallyKey, "MaxUndoItems", m_iMaxItems);

	m_bEnabled    = TRUE;
	m_bInProgress = FALSE;

	m_pDoc        = NULL; 
	m_pUndoList   = NULL;
	m_pRedoList   = NULL;
};

/////////////////////////////////////////////////////////////////////////////
// Name:        ~CWallyUndo
// Action:      CWallyUndo destructor
/////////////////////////////////////////////////////////////////////////////
CWallyUndo::~CWallyUndo()
{
	// clean up lists
	ClearUndoBuffer();
	ClearRedoBuffer();
}

/////////////////////////////////////////////////////////////////////////////
// Name:        ClearUndoBuffer
// Action:      Frees all undo list memory
/////////////////////////////////////////////////////////////////////////////
void CWallyUndo::ClearUndoBuffer( void)
{
    if (m_pUndoList)
    {
        RemoveAll( m_pUndoList);
        delete m_pUndoList;
        m_pUndoList = NULL;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Name:        ClearRedoBuffer
// Action:      Frees all redo list memory
/////////////////////////////////////////////////////////////////////////////
void CWallyUndo::ClearRedoBuffer( void)
{
    if (m_pRedoList)
    {
        RemoveAll( m_pRedoList);
        delete m_pRedoList;
        m_pRedoList = NULL;
    }
}

BOOL CWallyUndo::NowInProgress()
{
	BOOL bOldProgress = m_bInProgress;
	m_bInProgress     = TRUE;

	m_iNumUndosInProgress++;

	return bOldProgress;
}

void CWallyUndo::NoLongerInProgress( BOOL bOldProgress)
{
	m_bInProgress = bOldProgress;	// restore it

	m_iNumUndosInProgress--;
}

void CWallyUndo::SetMaxItems( int iMaxItems)
{
	// discard any undo items past the limit

	if (m_pUndoList != NULL)
	{
		while (m_pUndoList->GetCount() > iMaxItems)
		{
			if (m_pUndoList->GetCount() > 0)
			{
				delete (m_pUndoList->RemoveTail());
			}
			else
			{
				ASSERT( FALSE);
				break;
			}
		}
	}

	// discard any redo items past the limit

	if (m_pRedoList != NULL)
	{
		while (m_pRedoList->GetCount() > iMaxItems)
		{
			if (m_pRedoList->GetCount() > 0)
			{
				delete (m_pRedoList->RemoveTail());
			}
			else
			{
				ASSERT( FALSE);
				break;
			}
		}
	}

	m_iMaxItems = iMaxItems;

	// store max number of undos in ini file (where user can change it)
	CString WallyKey( "Settings");
	theApp.WriteProfileInt( WallyKey, "MaxUndoItems", m_iMaxItems);
};

/////////////////////////////////////////////////////////////////////////////
// Name:			SaveCurrentState
// Action:			Saves the current undo state, so user can UNDO/REDO
//
// Parameter(s):	lpszMenuString: used to build "Undo <MenuString>" for menuitem
// 
/////////////////////////////////////////////////////////////////////////////
CUndoState* CWallyUndo::SaveCurrentState( CWallyDoc* pDoc, LPCSTR lpszUndoString, int iType)
{
	if (! m_bEnabled)
		return NULL;

	if (m_pDoc != pDoc)
	{
		ASSERT( m_pDoc == NULL);
		m_pDoc = pDoc;
	}

	if (m_pDoc == NULL)
	{
		ASSERT( FALSE);
		return NULL;
	}

	CUndoState* pCurrentState = NULL;
	BOOL        bDone         = TRUE;

	while (bDone)
	{
		TRY
		{
			// add a new element to undo list
			if (pCurrentState == NULL)
			{
				int iNumItems = m_pUndoList->GetCount();

				// too many active undo items?
				if ((iNumItems >= m_iMaxItems) && (iNumItems > 0))
					pCurrentState = m_pUndoList->RemoveTail();

				// neal - NOT else if ()
				// it can legitimately be NULL
				if (pCurrentState == NULL)
					pCurrentState = new CUndoState;

				pCurrentState->m_strUndoString = lpszUndoString;
				pCurrentState->m_iUndoType     = iType;

				BOOL bModified = pDoc->IsModified();

				if (! bModified)
					MarkAsModified();

				pCurrentState->m_bModified = bModified;
			}

			// Create a shared memory file and associate a CArchive with it
			CSharedFile File;
			CArchive ar( &File, CArchive::store);

			// call document Serialize to save current state
			BOOL bOldProgress = NowInProgress();

			switch (iType)
			{				
			case UNDO_DOCUMENT:
				m_pDoc->Serialize( ar);
				break;

			case UNDO_PALETTE:
				m_pDoc->GetPalette()->Serialize( ar);
				break;

			case UNDO_DOC_AND_PAL:
				m_pDoc->Serialize( ar);
				m_pDoc->GetPalette()->Serialize( ar);
				break;

			default:
				ASSERT( FALSE);
				break;
			}

			NoLongerInProgress( bOldProgress);

			ar.Close();

			DWORD dwOriginalSize = File.GetLength();

//			pCurrentState->m_hGlobal = GlobalHandle( File.Detach());
			pCurrentState->m_hGlobal = File.Detach();

			if (g_bCompressUndo)
			{
				DWORD dwCompressedSize = 0;

				HGLOBAL hCompressed = CompressMemory( &(pCurrentState->m_hGlobal), 
						dwOriginalSize, &dwCompressedSize, TRUE);

				if (hCompressed)
				{
					pCurrentState->m_bCompressed = TRUE;
					pCurrentState->m_hGlobal     = hCompressed;
				}
			}
			else
			{
				pCurrentState->m_bCompressed = FALSE;
			}

			bDone = FALSE;
		}
		CATCH_ALL( e)
		{
			// try to free up some memory

			if (m_pRedoList && (m_pRedoList->GetCount() > 0))
			{
				ClearRedoBuffer();
			}
			else if (m_pUndoList && (m_pUndoList->GetCount() > 0))
			{
				delete (m_pUndoList->RemoveTail());
			}
			else
			{
				if (pCurrentState != NULL)
				{
					delete pCurrentState;
					pCurrentState = NULL;
				}

				ASSERT( FALSE);
				bDone = FALSE;
			}
		}
		END_CATCH_ALL
	}

	if (pCurrentState == NULL)
		AfxMessageBox( "Undo::SaveCurrentState failed - out of memory.");

	return pCurrentState;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        SaveCurrentState
// Action:      Gets string from string table and calls SaveCurrentState() above
/////////////////////////////////////////////////////////////////////////////
CUndoState* CWallyUndo::SaveCurrentState( CWallyDoc* pDoc, int iMenuStringID, int iType)
{
	CString strMenuString;
	strMenuString.LoadString( iMenuStringID);

	return SaveCurrentState( pDoc, strMenuString, iType);
}

/////////////////////////////////////////////////////////////////////////////
// Name:        GetLastState
// Action:      Gets the last save undo state
/////////////////////////////////////////////////////////////////////////////
CUndoState* CWallyUndo::GetLastState()
{
	if (m_pUndoList)
	{
		if (m_pUndoList->GetCount() > 0)
			return m_pUndoList->GetHead();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        SaveUndoState
// Action:      Saves the current state of the document, so that the user
//				can UNDO his action(s) at a later time.
//
// Parameter:   lpszMenuString: used to build "Undo <MenuString>" for menuitem
// 
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::SaveUndoState( CWallyDoc* pDoc, LPCSTR lpszMenuString, int iType)
{
	if (! m_bEnabled)		// skip it if we aren't enabled
		return FALSE;

	if (m_pDoc != pDoc)
	{
		ASSERT( m_pDoc == NULL);
		m_pDoc = pDoc;
	}

	if (m_pDoc == NULL)
	{
		ASSERT( FALSE);
		return FALSE;
	}

    TRY
    {
		ClearRedoBuffer();		// can't redo after an undoable action!

		if (m_pUndoList == NULL)
		    m_pUndoList = new UndoStateList;

		CUndoState* pCurrentState = SaveCurrentState( m_pDoc, lpszMenuString, iType);

		if (pCurrentState)
			m_pUndoList->AddHead( pCurrentState);

		return (pCurrentState != NULL);
	}
	CATCH_ALL( e)
    {
		ASSERT( FALSE);
    }
    END_CATCH_ALL

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        RestoreState
// Action:      Saves current state on pDestList and restores state from pSrcList
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::RestoreState( CUndoState* pState)
{
	if (pState == NULL)
	{
		ASSERT( FALSE);
		return FALSE;
	}

	BOOL bOldProgress = NowInProgress();

	if (pState->m_hGlobal == NULL)
	{
		ASSERT( FALSE);
		AfxMessageBox( "Undo::RestoreState failed - Memory may be corrupt.\nPlease save all your work and exit Wally now!");
		return (FALSE);
	}

	HGLOBAL hUncompressed = NULL;

	if (pState->m_bCompressed)
	{
		hUncompressed = DecompressMemory( &(pState->m_hGlobal), FALSE);

		if (hUncompressed == NULL)
		{
			ASSERT( FALSE);
			AfxMessageBox( "Undo::RestoreState failed - unable to decompress memory.");
			return (FALSE);
		}
	}
	else
	{
		hUncompressed = pState->m_hGlobal;
	}

	TRY
	{
		// restore doc to saved undo state via Serialize
		CSharedFile File;
		File.SetHandle( hUncompressed, FALSE);

		CArchive ar( &File, CArchive::load);
		ar.m_pDocument = m_pDoc;

		//m_pDoc->DeleteContents();	// delete contents of document (to prevent duplicates)
		
		// restore undo's version of the data

		switch (pState->m_iUndoType)
		{				
		case UNDO_DOCUMENT:
			m_pDoc->Serialize( ar);
			m_pDoc->GetPalette()->InitHSVPalette();
			break;

		case UNDO_PALETTE:
			m_pDoc->GetPalette()->Serialize( ar);
			break;

		case UNDO_DOC_AND_PAL:
			m_pDoc->Serialize( ar);
			m_pDoc->GetPalette()->Serialize( ar);
			break;

		default:
			ASSERT( FALSE);
			break;
		}

		ar.Close();
		File.Detach();

		if (pState->m_bCompressed)
			GlobalFree( hUncompressed);

		m_pDoc->SetModifiedFlag( pState->m_bModified);

		NoLongerInProgress( bOldProgress);

		m_pDoc->UpdateAllDIBs( FALSE);

		return TRUE;
	}
	CATCH_ALL( e)
	{
		AfxMessageBox( "Undo::RestoreState failed - out of memory.");
	}
	END_CATCH_ALL

	return (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// Name:        UpdateListsAndRestoreState
// Action:      Saves current state on pDestList and restores state from pSrcList
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::UpdateListsAndRestoreState( UndoStateList* pSrcList, UndoStateList* pDestList)
{
	ASSERT( (pSrcList->GetCount() >= 0)  && (pSrcList->GetCount() < 2000));
	ASSERT( (pDestList->GetCount() >= 0) && (pDestList->GetCount() < 2000));

	if (pSrcList->IsEmpty())	// sometimes happens when UI doesn't get a chance to update
		return (FALSE);

	CUndoState* pState = pSrcList->RemoveHead();

	if (pState == NULL)
	{
		ASSERT( FALSE);
		return FALSE;
	}

	// move current state to pDestList
	CUndoState* pCurrentState = SaveCurrentState( m_pDoc, pState->m_strUndoString, 
				pState->m_iUndoType);

	if (pCurrentState)
		pDestList->AddHead( pCurrentState);

	RestoreState( pState);

	// Ty- flag the doc that we've been changed
//	m_pDoc->SetModifiedFlag (TRUE);		// Neal - fixes undo/modified bug

	// nw - remove extra flashing
	if (! m_bInProgress)
	{
		//m_pDoc->UpdateAllViews( NULL);
		//BOOL bSizeChanged = m_pDoc->UpdateAllDIBs( FALSE);

		m_pDoc->UpdateAllDIBs( FALSE);

		// neal - if the size changed, the DIBs get wiped out !!!

		// NEAL-TEST

//		if (bSizeChanged)
//		{
//			RestoreState( pState);
//			m_pDoc->UpdateAllDIBs();
//		}
	}

	delete pState;					// don't mem leak!

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Name:        DoUndo
// Action:      Does the actual UNDO command
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::DoUndo( void)
{
	if (! m_bEnabled)
	{
		ASSERT( FALSE);
		return FALSE;
	}

	if (m_pUndoList != NULL)
	{
		// build a redo list, if we don't have one yet
		TRY
		{
			if (m_pRedoList == NULL)
			    m_pRedoList = new UndoStateList;
		}
		CATCH_ALL( e)
		{
			ASSERT( FALSE);
			return FALSE;
		}
		END_CATCH_ALL

		return UpdateListsAndRestoreState( m_pUndoList, m_pRedoList);
	}
	else
	{
		ASSERT( FALSE);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:        DoRedo
// Action:      Does the actual REDO command
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::DoRedo( void)
{
	if (! m_bEnabled)
	{
		ASSERT( FALSE);
		return FALSE;
	}

	if (m_pRedoList != NULL)
	{
		// build a undo list, if we don't have one yet
		TRY
		{
			if (m_pUndoList == NULL)
			    m_pUndoList = new UndoStateList;
		}
		CATCH_ALL( e)
		{
			ASSERT( FALSE);
			return FALSE;
		}
		END_CATCH_ALL

		return UpdateListsAndRestoreState( m_pRedoList, m_pUndoList);
	}
	else
	{
		ASSERT( FALSE);
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:        CanRedo
// Action:      Returns TRUE if the user can Undo
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::CanUndo( void) const
{
	if ((m_pUndoList != NULL) && m_bEnabled)
	    return (m_pUndoList->GetCount() > 0);
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        CanRedo
// Action:      Returns TRUE if the user can Redo
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyUndo::CanRedo( void) const
{
	if ((m_pRedoList != NULL) && m_bEnabled)
	    return (m_pRedoList->GetCount() > 0);
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        GetUndoString
// Action:      Returns the menu string for the "Undo" menu item
/////////////////////////////////////////////////////////////////////////////
LPCSTR CWallyUndo::GetUndoString( void) const
{
	static CString strMsg;

	if (CanUndo())
		strMsg = "Undo " + m_pUndoList->GetHead()->m_strUndoString + "\tCtrl+Z";
	else
		strMsg = "Can't Undo\tCtrl+Z";

	return (LPCSTR )strMsg;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        GetRedoString
// Action:      Returns the menu string for the "Redo" menu item
/////////////////////////////////////////////////////////////////////////////
LPCSTR CWallyUndo::GetRedoString( void) const
{
	static CString strMsg;

	if (CanRedo())
		strMsg = "Redo " + m_pRedoList->GetHead()->m_strUndoString + "\tCtrl+Y";
	else
		strMsg = "Can't Redo\tCtrl+Y";

	return strMsg;
}

/////////////////////////////////////////////////////////////////////////////
// Name:	MarkAsModified
// Action:	Sets the entire undo/redo chains "dirty"
//
// Note:	This is used to fix the long-standing undo/modified bug
/////////////////////////////////////////////////////////////////////////////
void CWallyUndo::MarkAsModified()
{
	if (m_pUndoList != NULL)
	{
		if (m_pUndoList->GetCount() >= 1)
		{
			POSITION Pos = m_pUndoList->GetHeadPosition();
			while (Pos != NULL)
			{
				CUndoState* pState = m_pUndoList->GetNext( Pos);
				pState->m_bModified = TRUE;
			}
		}
	}
	if (m_pRedoList != NULL)
	{
		if (m_pRedoList->GetCount() >= 1)
		{
			POSITION Pos = m_pRedoList->GetHeadPosition();
			while (Pos != NULL)
			{
				CUndoState* pState = m_pRedoList->GetNext( Pos);
				pState->m_bModified = TRUE;
			}
		}
	}
}