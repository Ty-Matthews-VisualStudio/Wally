// PackageTreeControl.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PackageTreeControl.h"
#include "WADList.h"
#include "MiscFunctions.h"
#include "RenameImageDlg.h"
#include "PackageDoc.h"
#include "WallyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_PACKAGETREECONTROL_CUSTOM = RegisterWindowMessage( "WM_PACKAGETREECONTROL_CUSTOM" );

/////////////////////////////////////////////////////////////////////////////
// CPackageTreeEntryBase

CPackageTreeEntryBase::CPackageTreeEntryBase() : m_pCurrentNameItemVector( &m_NameItemVector ), m_strPreviousFilter( "" )
{
}

CPackageTreeEntryBase::~CPackageTreeEntryBase()
{
}

/* virtual */ void CPackageTreeEntryBase::FilterItems( const char *szFilter )
{
	CString strFilter( szFilter );
	CString strName("");
	_itNameItem itNameItem;
	char *f, *n;
	
	if( (szFilter == NULL) || (strFilter.GetLength() == 0) )
	{
		// Just point to the unfiltered vector
		m_pCurrentNameItemVector = &m_NameItemVector;
	}
	else
	{
		// See if the filter is different from before
		if( _stricmp( m_strPreviousFilter.c_str(), szFilter ) )
		{
			m_strPreviousFilter = szFilter;
			
			// Wipe out the current filter list
			m_FilteredNameItemVector.clear();
			strFilter.MakeUpper();
			f = strFilter.GetBuffer( strFilter.GetLength() );

			itNameItem = m_NameItemVector.begin();
			while( itNameItem != m_NameItemVector.end() )
			{
				strName = (*itNameItem).first.c_str();
				strName.MakeUpper();
				n = strName.GetBuffer( strName.GetLength() );

				if( fnmatch( f, n, FNM_PATHNAME | FNM_PERIOD) != FNM_NOMATCH )
				{
					m_FilteredNameItemVector.push_back( std::make_pair( (*itNameItem).first.c_str(), (*itNameItem).second ) );
				}
				itNameItem++;
			}			

			
		}
		m_pCurrentNameItemVector = &m_FilteredNameItemVector;
	}
}

/* virtual */ void CPackageTreeEntryBase::RefreshFilter()
{
	std::string strFilter;
	strFilter = m_strPreviousFilter;
	m_strPreviousFilter = "";
	FilterItems( strFilter.c_str() );

	if( GetParent() )
	{
		GetParent()->RefreshFilter();
	}
}

/* virtual */ CPackageTreeEntryBase *CPackageTreeEntryBase::GetParent()
{
	return m_pParentEntry;
}

/* virtual */ void CPackageTreeEntryBase::SetParent( CPackageTreeEntryBase *pParent )
{
	m_pParentEntry = pParent;
}

/* virtual */ void CPackageTreeEntryBase::AddNameItem( const char *szName, CWADItem *pItem, bool bCheckForDuplicate /* = false */ )
{
	if( bCheckForDuplicate )
	{
		if( ItemExists( pItem ) )
		{
			return;
		}
	}
	
	// Add to our list
	m_NameItemVector.push_back( std::make_pair( szName, pItem ) );

	// Now call up the chain
	if( GetParent() )
	{
		GetParent()->AddNameItem( szName, pItem, bCheckForDuplicate );
	}
}

/* virtual */ bool CPackageTreeEntryBase::ItemExists( CWADItem *pItem )
{
	_itNameItem itNameItem;
	itNameItem = m_NameItemVector.begin();
	while( itNameItem != m_NameItemVector.end() )
	{
		if( (*itNameItem).second == pItem )
		{
			return true;
		}
		itNameItem++;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CPackageTreeEntryCategory

CPackageTreeEntryCategory::CPackageTreeEntryCategory( const char *szName, CPackageTreeEntryBase *pParent ) : m_strName( szName )
{
	SetParent( pParent );
}

/* virtual */ CPackageTreeEntryCategory::~CPackageTreeEntryCategory()
{
}

/* virtual */ void CPackageTreeEntryCategory::FilterItems( const char *szFilter )
{
	// First call the parent function
	CPackageTreeEntryBase::FilterItems( szFilter );

	// Now call for our children
	_itPackageTreeEntry itPackageTreeEntry;
	itPackageTreeEntry = m_ChildVector.begin();

	while( itPackageTreeEntry != m_ChildVector.end() )
	{
		(*itPackageTreeEntry)->FilterItems( szFilter );
		itPackageTreeEntry++;
	}
}

/* virtual */ const char *CPackageTreeEntryCategory::GetName()
{
	return m_strName.c_str();
}

/* virtual */ void CPackageTreeEntryCategory::SetName( const char *szName )
{
	m_strName = szName;
}

/* virtual */ CPackageTreeEntryBase *CPackageTreeEntryCategory::AddItem( const char *szName, CWADItem *pItem, bool bCheckForDuplicate /* = false */ )
{
	CPackageTreeEntryBase *pBase = NULL;

	if( bCheckForDuplicate )
	{
		if( ItemExists( pItem ) )
		{
			// We're already in the list, no sense inserting it again.
			return NULL;
		}
	}

	// We have to do the allocation, since the object exists at our level
	pBase = static_cast< CPackageTreeEntryBase * >( new CPackageTreeEntryImage( szName, pItem, this ) );

	// Add the name to our list, and our parent's
	AddNameItem( szName, pItem, bCheckForDuplicate );

	// Redo the filter
	RefreshFilter();
	
	return pBase;
}

/* virtual */ CWADItem *CPackageTreeEntryCategory::GetItem()
{
	_itNameItem itNameItem;
	// Return the first one in our list
	if( m_pCurrentNameItemVector )
	{
		itNameItem = m_pCurrentNameItemVector->begin();
		if( itNameItem != m_pCurrentNameItemVector->end() )
		{
			return (*itNameItem).second;
		}
	}
	return NULL;
}

/* virtual */ UINT CPackageTreeEntryCategory::GetItemCount()
{
	return m_NameItemVector.size();
}

/* virtual */ CWADItem *CPackageTreeEntryCategory::GetItemAtPosition( UINT iDesiredPosition )
{	
	_NameItemPair NameItemPair;
	if( m_pCurrentNameItemVector )
	{
		try
		{
			NameItemPair = m_pCurrentNameItemVector->at( iDesiredPosition );
			return NameItemPair.second;
		}
		catch(...)
		{
		}		
	}
	return NULL;
}

/* virtual */ void CPackageTreeEntryCategory::RenameItem( CPackageDoc *pDoc, CPackageTreeControl *pTreeCtrl )
{
	HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
	pTreeCtrl->EditLabel( hItem );
}

/////////////////////////////////////////////////////////////////////////////
// CPackageTreeEntryImage

CPackageTreeEntryImage::CPackageTreeEntryImage( const char *szName, CWADItem *pItem, CPackageTreeEntryBase *pParent )
{
	SetName( szName );
	SetItem( pItem );
	SetParent( pParent );
}

/* virtual */ CPackageTreeEntryImage::~CPackageTreeEntryImage()
{
}

/* virtual */ const char *CPackageTreeEntryImage::GetName()
{
	return m_NameItemPair.first.c_str();
}

/* virtual */ void CPackageTreeEntryImage::SetName( const char *szName )
{
	m_NameItemPair.first = szName;
}

/* virtual */ CWADItem *CPackageTreeEntryImage::GetItem()
{
	return m_NameItemPair.second;
}

/* virtual */ void CPackageTreeEntryImage::SetItem( CWADItem *pItem )
{
	m_NameItemPair.second = pItem;
}

/* virtual */ CWADItem *CPackageTreeEntryImage::GetItemAtPosition( UINT iDesiredPosition )
{	
	// Go to our parent category and call that one
	if( GetParent() )
	{
		return GetParent()->GetItemAtPosition( iDesiredPosition );
	}
	return GetItem();
}

/* virtual */ UINT CPackageTreeEntryImage::GetItemCount()
{
	// Go to our parent category and call that one
	if( GetParent() )
	{
		return GetParent()->GetItemCount();
	}
	return 1;
}

/* virtual */ void CPackageTreeEntryImage::SetSelected( UINT nState )
{
	if( GetItem() )
	{
		GetItem()->SetSelected( (nState & TVIS_SELECTED) ? TRUE : FALSE );
	}	
}

/* virtual */ void CPackageTreeEntryImage::RenameItem( CPackageDoc *pDoc, CPackageTreeControl *pTreeCtrl )
{
	ASSERT( pDoc );
	ASSERT( pTreeCtrl );

	CWADItem *pItem = pTreeCtrl->GetSelectedImage();
	CString strName( pItem->GetName() );
	CRenameImageDlg dlgRename;
	CWallyDoc *pWallyDoc = NULL;

	dlgRename.SetMaxLength( 15 );
	dlgRename.SetName( strName );

	bool bFinished = false;

	while( !bFinished )
	{
		if( dlgRename.DoModal() == IDOK )
		{
			strName = dlgRename.GetName();

			if( pDoc->IsNameInList( strName ) )
			{
				AfxMessageBox ("That name is already in the list.  Please enter a unique name.", MB_ICONSTOP);
			}
			else
			{		
				pItem->SetName( strName );
				_PackageTreeEntryVector PackageTreeEntryVector;
				pItem->GetPackageEntries( PackageTreeEntryVector );
				_itPackageTreeEntry itPackageTreeEntry;

				itPackageTreeEntry = PackageTreeEntryVector.begin();
				while( itPackageTreeEntry != PackageTreeEntryVector.end() )
				{
					pTreeCtrl->SetItem( (*itPackageTreeEntry)->GetTreeItem(), TVIF_TEXT, strName, 0, 0, 0, 0, 0 );
					itPackageTreeEntry++;
				}
				pWallyDoc = pItem->GetWallyDoc();
				if (pWallyDoc)
				{
					pWallyDoc->SetTitle( strName );
				}
				pWallyDoc = NULL;

				pDoc->SetModifiedFlag( true );
				bFinished = true;
			}
		}	
		else
		{
			bFinished = true;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPackageTreeControl

CPackageTreeControl::CPackageTreeControl() : m_bSelectPending(FALSE), m_hClickedItem(NULL), m_hFirstSelectedItem(NULL), m_bSelectionComplete(TRUE), m_bEditLabelPending(FALSE)
{
}

CPackageTreeControl::~CPackageTreeControl()
{
	// Wipe out the vector
	_itPackageTreeEntry itEntry;
	CPackageTreeEntryBase *pEntry = NULL;
	itEntry = m_PackageTreeVector.begin();
	while( itEntry != m_PackageTreeVector.end() )
	{
		pEntry = (*itEntry);
		if( pEntry )
		{
			delete pEntry;
			pEntry = NULL;
		}
		itEntry++;
	}
}


BEGIN_MESSAGE_MAP(CPackageTreeControl, CTreeCtrl)
	//{{AFX_MSG_MAP(CPackageTreeControl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_NOTIFY_REFLECT_EX(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT_EX(NM_KILLFOCUS, OnKillfocus)
	ON_WM_RBUTTONDOWN()
	//ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()	
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CPackageTreeControl::OnTvnSelchanged)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPackageTreeControl, CTreeCtrl)

/////////////////////////////////////////////////////////////////////////////
// CPackageTreeControl message handlers

BOOL CPackageTreeControl::Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
#if _MFC_VER < 0x0700
	return CreateEx( dwExStyle, WC_TREEVIEW, NULL, dwStyle,
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, 
		pParentWnd->GetSafeHwnd(), (HMENU)nID );
#else
	return CTreeCtrl::CreateEx( dwExStyle, dwStyle, rect, pParentWnd, nID );
#endif
}

BOOL CPackageTreeControl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CTreeCtrl::Create(dwStyle, rect, pParentWnd, nID);
}


/////////////////////////////////////////////////////////////////////////////
// CPackageTreeControl message handlers


///////////////////////////////////////////////////////////////////////////////
// The tree control dosn't support multiple selection. However we can simulate 
// it by taking control of the left mouse click and arrow key press before the
// control gets them, and setting/clearing the TVIS_SELECTED style on the items

void CPackageTreeControl::OnLButtonDown( UINT nFlags, CPoint point )
{

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest( point, &nHitFlags );

	// Must invoke label editing explicitly. The base class OnLButtonDown would normally
	// do this, but we can't call it here because of the multiple selection...
	if( !( nFlags&( MK_CONTROL|MK_SHIFT ) ) && ( GetStyle() & TVS_EDITLABELS ) && ( nHitFlags & TVHT_ONITEMLABEL ) )
		if ( hClickedItem == GetSelectedItem() )
		{
			// Clear multple selection before label editing
			ClearSelection();
			SelectItem( hClickedItem );

			// Invoke label editing
			m_bEditLabelPending = TRUE;
			m_idTimer = SetTimer(TCEX_EDITLABEL, GetDoubleClickTime(), NULL);

			return;
		}

	m_bEditLabelPending = FALSE;

	if( nHitFlags & TVHT_ONITEM )
	{
		SetFocus();

		m_hClickedItem = hClickedItem;

		// Is the clicked item already selected ?
		BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;

		if ( bIsClickedItemSelected )
		{
			// Maybe user wants to drag/drop multiple items!
			// So, wait until OnLButtonUp() to do the selection stuff. 
			m_bSelectPending=TRUE;
		}
		else
		{
			SelectMultiple( hClickedItem, nFlags, point );
			m_bSelectPending=FALSE;
		}

		m_ptClick=point;
	}
	else
		CTreeCtrl::OnLButtonDown( nFlags, point );
}

void CPackageTreeControl::OnLButtonUp( UINT nFlags, CPoint point )
{
	if ( m_bSelectPending )
	{
		// A select has been waiting to be performed here
		SelectMultiple( m_hClickedItem, nFlags, point );
		m_bSelectPending=FALSE;
	}

	m_hClickedItem=NULL;

	CTreeCtrl::OnLButtonUp( nFlags, point );
}


void CPackageTreeControl::OnMouseMove( UINT nFlags, CPoint point )
{
	// If there is a select pending, check if cursor has moved so much away from the 
	// down-click point that we should cancel the pending select and initiate
	// a drag/drop operation instead!

	if ( m_hClickedItem )
	{
		CSize sizeMoved = m_ptClick-point;

		if ( abs(sizeMoved.cx) > GetSystemMetrics( SM_CXDRAG ) || abs(sizeMoved.cy) > GetSystemMetrics( SM_CYDRAG ) )
		{
			m_bSelectPending=FALSE;

			// Notify parent that he may begin drag operation
			// Since we have taken over OnLButtonDown(), the default handler doesn't
			// do the normal work when clicking an item, so we must provide our own
			// TVN_BEGINDRAG notification for the parent!

			CWnd* pWnd = GetParent();
			if ( pWnd && !( GetStyle() & TVS_DISABLEDRAGDROP ) )
			{
				NM_TREEVIEW tv;

				tv.hdr.hwndFrom = GetSafeHwnd();
				tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
				tv.hdr.code = TVN_BEGINDRAG;

				tv.itemNew.hItem = m_hClickedItem;
				tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );
				tv.itemNew.lParam = GetItemData( m_hClickedItem );

				tv.ptDrag.x = point.x;
				tv.ptDrag.y = point.y;

				pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );
			}

			m_hClickedItem=NULL;
		}
	}

	CTreeCtrl::OnMouseMove( nFlags, point );
}


void CPackageTreeControl::SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point )
{
	// Start preparing an NM_TREEVIEW struct to send a notification after selection is done
	NM_TREEVIEW tv;
	memset(&tv.itemOld, 0, sizeof(tv.itemOld));

	CWnd* pWnd = GetParent();

	HTREEITEM hOldItem = GetSelectedItem();

	if ( hOldItem )
	{
		tv.itemOld.hItem = hOldItem;
		tv.itemOld.state = GetItemState( hOldItem, 0xffffffff );
		tv.itemOld.lParam = GetItemData( hOldItem );
		tv.itemOld.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;
	}

	// Flag signaling that selection process is NOT complete.
	// (Will prohibit TVN_SELCHANGED from being sent to parent)
	m_bSelectionComplete = FALSE;

	// Action depends on whether the user holds down the Shift or Ctrl key
	if ( nFlags & MK_SHIFT )
	{
		// Select from first selected item to the clicked item
		if ( !m_hFirstSelectedItem )
			m_hFirstSelectedItem = GetSelectedItem();

		SelectItems( m_hFirstSelectedItem, hClickedItem );
	}
	else if ( nFlags & MK_CONTROL )
	{
		// Find which item is currently selected
		HTREEITEM hSelectedItem = GetSelectedItem();

		// Is the clicked item already selected ?
		BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;
		BOOL bIsSelectedItemSelected = FALSE;
		if ( hSelectedItem )
			bIsSelectedItemSelected = GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED;

		// Must synthesize a TVN_SELCHANGING notification
		if ( pWnd )
		{
			tv.hdr.hwndFrom = GetSafeHwnd();
			tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
			tv.hdr.code = TVN_SELCHANGING;

			tv.itemNew.hItem = hClickedItem;
			tv.itemNew.state = GetItemState( hClickedItem, 0xffffffff );
			tv.itemNew.lParam = GetItemData( hClickedItem );

			tv.itemOld.hItem = NULL;
			tv.itemOld.mask = 0;

			tv.action = TVC_BYMOUSE;

			tv.ptDrag.x = point.x;
			tv.ptDrag.y = point.y;

			pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );
		}

		// If the previously selected item was selected, re-select it
		if ( bIsSelectedItemSelected )
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );

		// We want the newly selected item to toggle its selected state,
		// so unselect now if it was already selected before
		if ( bIsClickedItemSelected )
			SetItemState( hClickedItem, 0, TVIS_SELECTED );
		else
		{
			SelectItem(hClickedItem);
			SetItemState( hClickedItem, TVIS_SELECTED, TVIS_SELECTED );
		}

		// If the previously selected item was selected, re-select it
		if ( bIsSelectedItemSelected && hSelectedItem != hClickedItem )
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );

		// Store as first selected item (if not already stored)
		if ( m_hFirstSelectedItem==NULL )
			m_hFirstSelectedItem = hClickedItem;
	}
	else
	{
		// Clear selection of all "multiple selected" items first
		ClearSelection();

		// Then select the clicked item
		SelectItem( hClickedItem );
		SetItemState( hClickedItem, TVIS_SELECTED, TVIS_SELECTED );

		// Store as first selected item
		m_hFirstSelectedItem = hClickedItem;
	}

	// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
	// notification provided by Windows' treectrl, we must now produce one ourselves,
	// so that our parent gets to know about the change of selection.
	m_bSelectionComplete = TRUE;

	if ( pWnd )
	{
		tv.hdr.hwndFrom = GetSafeHwnd();
		tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
		tv.hdr.code = TVN_SELCHANGED;

		tv.itemNew.hItem = m_hClickedItem;
		tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );
		tv.itemNew.lParam = GetItemData( m_hClickedItem );
		tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

		tv.action = TVC_UNKNOWN;

		pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );
	}
}

void CPackageTreeControl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	CWnd* pWnd = GetParent();

	if ( nChar==VK_NEXT || nChar==VK_PRIOR )
	{
		if ( !( GetKeyState( VK_SHIFT )&0x8000 ) )
		{
			// User pressed Pg key without holding 'Shift':
			// Clear multiple selection (if multiple) and let base class do 
			// normal selection work!
#if 0
			if ( GetSelectedCount()>1 )
#endif
				ClearSelection( TRUE );

			CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
			m_hFirstSelectedItem = GetSelectedItem();
			SetSelected( m_hFirstSelectedItem );
			return;
		}

		// Flag signaling that selection process is NOT complete.
		// (Will prohibit TVN_SELCHANGED from being sent to parent)
		m_bSelectionComplete = FALSE;

		// Let base class select the item
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
		HTREEITEM hSelectedItem = GetSelectedItem();

		// Then select items in between
		SelectItems( m_hFirstSelectedItem, hSelectedItem );

		// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
		// notification provided by Windows' treectrl, we must now produce one ourselves,
		// so that our parent gets to know about the change of selection.
		m_bSelectionComplete = TRUE;

		if (pWnd)
		{
			NM_TREEVIEW tv;
			memset(&tv.itemOld, 0, sizeof(tv.itemOld));

			tv.hdr.hwndFrom = GetSafeHwnd();
			tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
			tv.hdr.code = TVN_SELCHANGED;

			tv.itemNew.hItem = hSelectedItem;
			tv.itemNew.state = GetItemState(hSelectedItem, 0xffffffff);
			tv.itemNew.lParam = GetItemData(hSelectedItem);
			tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

			tv.action = TVC_UNKNOWN;

			pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
		}
	}
	else if ( nChar==VK_UP || nChar==VK_DOWN )
	{
		// Find which item is currently selected
		HTREEITEM hSelectedItem = GetSelectedItem();
		HTREEITEM hNextItem;
		if ( nChar==VK_UP )
			hNextItem = GetPrevVisibleItem( hSelectedItem );
		else
			hNextItem = GetNextVisibleItem( hSelectedItem );

		if ( !( GetKeyState( VK_SHIFT )&0x8000 ) )
		{
			// User pressed arrow key without holding 'Shift':
			// Clear multiple selection (if multiple) and let base class do 
			// normal selection work!
#if 0
			if ( GetSelectedCount()>1 )
#endif
				ClearSelection( TRUE );

			if ( hNextItem )
			{
				CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
			}
			m_hFirstSelectedItem = GetSelectedItem();
			SetSelected( m_hFirstSelectedItem );
			
			return;
		}

		if ( hNextItem )
		{
			// Flag signaling that selection process is NOT complete.
			// (Will prohibit TVN_SELCHANGED from being sent to parent)
			m_bSelectionComplete = FALSE;

			// If the next item is already selected, we assume user is
			// "moving back" in the selection, and thus we should clear 
			// selection on the previous one
			BOOL bSelect = !( GetItemState( hNextItem, TVIS_SELECTED ) & TVIS_SELECTED );

			// Select the next item (this will also deselect the previous one!)
			SelectItem( hNextItem );

			// Now, re-select the previously selected item
			if ( bSelect || ( !( GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED ) ) )
				SelectItems( m_hFirstSelectedItem, hNextItem );

			// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED 
			// notification provided by Windows' treectrl, we must now produce one ourselves,
			// so that our parent gets to know about the change of selection.
			m_bSelectionComplete = TRUE;

			if (pWnd)
			{
				NM_TREEVIEW tv;
				memset(&tv.itemOld, 0, sizeof(tv.itemOld));

				tv.hdr.hwndFrom = GetSafeHwnd();
				tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);
				tv.hdr.code = TVN_SELCHANGED;

				tv.itemNew.hItem = hNextItem;
				tv.itemNew.state = GetItemState(hNextItem, 0xffffffff);
				tv.itemNew.lParam = GetItemData(hNextItem);
				tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;

				tv.action = TVC_UNKNOWN;

				pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);
			}
		}

		// Since the base class' OnKeyDown() isn't called in this case,
		// we must provide our own TVN_KEYDOWN notification to the parent

		CWnd* pWnd = GetParent();
		if ( pWnd )
		{
			NMTVKEYDOWN tvk;

			tvk.hdr.hwndFrom = GetSafeHwnd();
			tvk.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );
			tvk.hdr.code = TVN_KEYDOWN;

			tvk.wVKey = nChar;
			tvk.flags = 0;

			pWnd->SendMessage( WM_NOTIFY, tvk.hdr.idFrom, (LPARAM)&tvk );
		}
	}	
	else
	{
		// Behave normally
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
	}		
}


void CPackageTreeControl::SetSelected( HTREEITEM hItem )
{
	if( hItem )
	{
		CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			pBase->SetSelected( GetItemState( hItem, TVIF_STATE ) );
		}
	}
}

CWADItem *CPackageTreeControl::GetImageFromHTREEITEM( HTREEITEM hItem )
{
	if( hItem )
	{
		CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			if( pBase->GetType() == ePackageTreeEntryImage )
			{
				return pBase->GetItem();
			}
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// I want clicking on an item with the right mouse button to select the item,
// but not if there is currently a multiple selection

void CPackageTreeControl::OnRButtonDown( UINT nFlags, CPoint point )
{
	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest( point, &nHitFlags );

	if( nHitFlags&TVHT_ONITEM )
	{
		if ( GetSelectedCount()<2 )
		{
			SelectItem( hClickedItem );
			return;
		}		
	}

	CTreeCtrl::OnRButtonDown( nFlags, point );	
}


///////////////////////////////////////////////////////////////////////////////
// Get number of selected items

UINT CPackageTreeControl::GetSelectedCount() const
{
	// Only visible items should be selected!
	UINT uCount=0;
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			uCount++;

	return uCount;
}

UINT CPackageTreeControl::GetVisibleCount()
{
	// Which one is currently selected?
	HTREEITEM hItem = GetSelectedItem();
	return CountImages( hItem );	
}

UINT CPackageTreeControl::CountImages( HTREEITEM hItem )
{
#if 0
	// Recursive function that adds up the number of images in the current branch, then calls
	// off recursively into this function to add up all of the child branches.
	UINT iCount = 0;	
	HTREEITEM hChildItem = GetChildItem( hItem );
	CWADItem *pItem = NULL;

	while( hChildItem != NULL )
	{
		if( GetItemData( hChildItem ) != NULL )
		{
			pItem = reinterpret_cast< CWADItem * >( GetItemData( hChildItem ) );
			if( pItem->IsValidMip() )
			{
				iCount++;
			}
		}
		else
		{
			iCount += CountImages( hChildItem );
		}
		hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
	}
	return iCount;
#endif
	
	CPackageTreeEntryBase *pBase = NULL;
	if( hItem )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			return pBase->GetItemCount();		
		}
	}
	
	return 0;
}

CWADItem *CPackageTreeControl::GetSelectedImage()
{
	HTREEITEM hItem = GetSelectedItem();
	CPackageTreeEntryBase *pBase = NULL;
	if( hItem )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			return pBase->GetItem();
		}
	}
	return NULL;
}

CWADItem *CPackageTreeControl::GetImageAtPosition( UINT iDesiredPosition )
{
	HTREEITEM hItem = GetSelectedItem();
	CPackageTreeEntryBase *pBase = NULL;
	if( hItem )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			return pBase->GetItemAtPosition( iDesiredPosition );
		}
	}
#if 0
	UINT iPosition = 0;
	return _GetImageAtPosition( hItem, iDesiredPosition, iPosition );
#endif
	return NULL;
}

CWADItem *CPackageTreeControl::_GetImageAtPosition( HTREEITEM hItem, UINT iDesiredPosition, UINT &iPosition )
{	
	CWADItem *pItem = NULL;
	HTREEITEM hChildItem = GetChildItem( hItem );
	while( hChildItem != NULL )
	{
		if( GetItemData( hChildItem ) != NULL )
		{
			pItem = reinterpret_cast< CWADItem * >( GetItemData( hChildItem ) );
			if( pItem->IsValidMip() )
			{
				iPosition++;
				if( iPosition > iDesiredPosition )
				{
					return pItem;
				}
			}
		}
		else
		{
			// Call off recursively
			_GetImageAtPosition( hChildItem, iDesiredPosition, iPosition );
		}
		hChildItem = GetNextItem( hChildItem, TVGN_NEXT );
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Overloaded to catch our own special code

HTREEITEM CPackageTreeControl::GetNextItem(HTREEITEM hItem, UINT nCode)
{
	if (nCode==TVGN_EX_ALL)
	{
		// This special code lets us iterate through ALL tree items regardless 
		// of their parent/child relationship (very handy)
		HTREEITEM hNextItem;

		// If it has a child node, this will be the next item
		hNextItem = GetChildItem( hItem );
		if (hNextItem)
			return hNextItem;

		// Otherwise, see if it has a next sibling item
		hNextItem = GetNextSiblingItem(hItem);
		if (hNextItem)
			return hNextItem;

		// Finally, look for next sibling to the parent item
		HTREEITEM hParentItem=hItem;
		while (!hNextItem && hParentItem)
		{
			// No more children: Get next sibling to parent
			hParentItem = GetParentItem(hParentItem);
			hNextItem = GetNextSiblingItem(hParentItem);
		}

		return hNextItem; // will return NULL if no more parents
	}
	else
		return CTreeCtrl::GetNextItem(hItem, nCode);	// standard processing
}

///////////////////////////////////////////////////////////////////////////////
// Helpers to list out selected items. (Use similar to GetFirstVisibleItem(), 
// GetNextVisibleItem() and GetPrevVisibleItem()!)

HTREEITEM CPackageTreeControl::GetFirstSelectedItem()
{
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}

HTREEITEM CPackageTreeControl::GetNextSelectedItem( HTREEITEM hItem )
{
	for ( hItem = GetNextVisibleItem( hItem ); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}

HTREEITEM CPackageTreeControl::GetPrevSelectedItem( HTREEITEM hItem )
{
	for ( hItem = GetPrevVisibleItem( hItem ); hItem!=NULL; hItem = GetPrevVisibleItem( hItem ) )
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Select/unselect item without unselecting other items

BOOL CPackageTreeControl::SelectItemEx(HTREEITEM hItem, BOOL bSelect/*=TRUE*/)
{
	HTREEITEM hSelItem = GetSelectedItem();

	if ( hItem==hSelItem )
	{
		if ( !bSelect )
		{
			SelectItem( NULL );
			return TRUE;
		}

		return FALSE;
	}

	SelectItem( hItem );
	m_hFirstSelectedItem=hItem;

	// Reselect previous "real" selected item which was unselected byt SelectItem()
	if ( hSelItem )
		SetItemState( hSelItem, TVIS_SELECTED, TVIS_SELECTED );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Select visible items between specified 'from' and 'to' item (including these!)
// If the 'to' item is above the 'from' item, it traverses the tree in reverse 
// direction. Selection on other items is cleared!

BOOL CPackageTreeControl::SelectItems( HTREEITEM hFromItem, HTREEITEM hToItem )
{
	// Determine direction of selection 
	// (see what item comes first in the tree)
	HTREEITEM hItem = GetRootItem();

	while ( hItem && hItem!=hFromItem && hItem!=hToItem )
		hItem = GetNextVisibleItem( hItem );

	if ( !hItem )
		return FALSE;	// Items not visible in tree

	BOOL bReverse = hItem==hToItem;

	// "Really" select the 'to' item (which will deselect 
	// the previously selected item)

	SelectItem( hToItem );

	// Go through all visible items again and select/unselect

	hItem = GetRootItem();
	BOOL bSelect = FALSE;

	while ( hItem )
	{
		if ( hItem == ( bReverse ? hToItem : hFromItem ) )
			bSelect = TRUE;

		if ( bSelect )
		{
			if ( !( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ) )
				SetItemState( hItem, TVIS_SELECTED, TVIS_SELECTED );
		}
		else
		{
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				SetItemState( hItem, 0, TVIS_SELECTED );
		}

		if ( hItem == ( bReverse ? hFromItem : hToItem ) )
			bSelect = FALSE;

		hItem = GetNextVisibleItem( hItem );
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Clear selected state on all visible items

void CPackageTreeControl::ClearSelection(BOOL bMultiOnly/*=FALSE*/)
{
//	if ( !bMultiOnly )
//		SelectItem( NULL );
	CPackageTreeEntryBase *pBase = NULL;

	for ( HTREEITEM hItem=GetRootItem(); hItem!=NULL; hItem=GetNextVisibleItem( hItem ) )
	{
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			SetItemState( hItem, 0, TVIS_SELECTED );

		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			pBase->SetSelected( GetItemState( hItem, TVIF_STATE ) );
			pBase = NULL;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// If a node is collapsed, we should clear selections of its child items 

BOOL CPackageTreeControl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if ( pNMTreeView->action == TVE_COLLAPSE )
	{
		HTREEITEM hItem = GetChildItem( pNMTreeView->itemNew.hItem );

		while ( hItem )
		{
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
				SetItemState( hItem, 0, TVIS_SELECTED );

			// Get the next node: First see if current node has a child
			HTREEITEM hNextItem = GetChildItem( hItem );
			if ( !hNextItem )
			{
				// No child: Get next sibling item
				if ( !( hNextItem = GetNextSiblingItem( hItem ) ) )
				{
					HTREEITEM hParentItem = hItem;
					while ( !hNextItem )
					{
						// No more children: Get parent
						if ( !( hParentItem = GetParentItem( hParentItem ) ) )
							break;

						// Quit when parent is the collapsed node
						// (Don't do anything to siblings of this)
						if ( hParentItem == pNMTreeView->itemNew.hItem )
							break;

						// Get next sibling to parent
						hNextItem = GetNextSiblingItem( hParentItem );
					}

					// Quit when parent is the collapsed node
					if ( hParentItem == pNMTreeView->itemNew.hItem )
						break;
				}
			}

			hItem = hNextItem;
		}
	}
	
	*pResult = 0;
	return FALSE;	// Allow parent to handle this notification as well
}


///////////////////////////////////////////////////////////////////////////////
// Intercept TVN_SELCHANGED and pass it only to the parent window of the
// selection process is finished
/*
BOOL CPackageTreeControl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Return TRUE if selection is not complete. This will prevent the 
	// notification from being sent to parent.
	return !m_bSelectionComplete;	
}
*/


///////////////////////////////////////////////////////////////////////////////
// Ensure the multiple selected items are drawn correctly when loosing/getting
// the focus

BOOL CPackageTreeControl::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate();
	*pResult = 0;
	return FALSE;
}

BOOL CPackageTreeControl::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult) 
{
	Invalidate();
	*pResult = 0;
	return FALSE;
}

void CPackageTreeControl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// We stop label editing.
	m_bEditLabelPending = FALSE;
	CTreeCtrl::OnLButtonDblClk(nFlags, point);

	CWnd* pParent = GetParent();
	if( pParent )
	{			
		pParent->SendMessage( WM_PACKAGETREECONTROL_CUSTOM, PACKAGETREECONTROL_OPENTEXTURE, 0 );
	}
}

void CPackageTreeControl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TCEX_EDITLABEL)
	{
		// Stop the timer.
		KillTimer(m_idTimer);

		// Invoke label editing.
		if (m_bEditLabelPending)
			EditLabel(GetSelectedItem());

		m_bEditLabelPending = FALSE;
		return;
	}

	CTreeCtrl::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// Retreives a tree ctrl item given the item's data

HTREEITEM CPackageTreeControl::ItemFromData(DWORD dwData, HTREEITEM hStartAtItem/*=NULL*/) const
{
	// Traverse all items in tree control
	HTREEITEM hItem;
	if ( hStartAtItem )
		hItem = hStartAtItem;
	else
		hItem = GetRootItem();

	while ( hItem )
	{
		if ( dwData == (DWORD)GetItemData( hItem ) )
			return hItem;

		// Get first child node
		HTREEITEM hNextItem = GetChildItem( hItem );

		if ( !hNextItem )
		{
			// Get next sibling child
			hNextItem = GetNextSiblingItem( hItem );

			if ( !hNextItem )
			{
				HTREEITEM hParentItem=hItem;
				while ( !hNextItem && hParentItem )
				{
					// No more children: Get next sibling to parent
					hParentItem = GetParentItem( hParentItem );
					hNextItem = GetNextSiblingItem( hParentItem );
				}
			}
		}

		hItem = hNextItem;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Global function to retreive a HTREEITEM from a tree control, given the 
// item's itemdata.

HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD dwData, HTREEITEM hStartAtItem /*=NULL*/)
{
	// Traverse from given item (or all items if hFromItem is NULL)
	HTREEITEM hItem;
	if ( hStartAtItem )
		hItem=hStartAtItem;
	else
		hItem = treeCtrl.GetRootItem();

	while ( hItem )
	{
		if ( dwData == (DWORD)treeCtrl.GetItemData( hItem ) )
			return hItem;

		// Get first child node
		HTREEITEM hNextItem = treeCtrl.GetChildItem( hItem );

		if ( !hNextItem )
		{
			// Get next sibling child
			hNextItem = treeCtrl.GetNextSiblingItem( hItem );

			if ( !hNextItem )
			{
				HTREEITEM hParentItem=hItem;
				while ( !hNextItem && hParentItem )
				{
					// No more children: Get next sibling to parent
					hParentItem = treeCtrl.GetParentItem( hParentItem );
					hNextItem = treeCtrl.GetNextSiblingItem( hParentItem );
				}
			}
		}
		hItem = hNextItem;
	}
	return NULL;
}


void CPackageTreeControl::OnInitialUpdate()
{
	InitializeImageList();
	
#if 0
	// First add the root (which is always there)
	CString strCategory("");
	strCategory = "All Images";
	int iLength = strCategory.GetLength();

	CPackageTreeEntryBase *pBase = NULL;

	pBase = static_cast< CPackageTreeEntryBase * >( CreateCategory( strCategory, NULL ) );
	ASSERT( pBase );
	
	TV_INSERTSTRUCT tvInsert;	

	tvInsert.hParent = TVI_ROOT; 
	tvInsert.hInsertAfter = NULL;		
	
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.pszText = strCategory.GetBuffer( iLength );
	tvInsert.item.cchTextMax = iLength;
	tvInsert.item.iImage = m_iClosedFolderIcon;
	tvInsert.item.iSelectedImage = m_iOpenFolderIcon;	
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = reinterpret_cast< ULONG >( pBase );

	m_htAllImages = InsertItem( &tvInsert );
	pBase->SetTreeItem( m_htAllImages );

	strCategory = "All Categories";
	pBase = static_cast< CPackageTreeEntryBase * >( CreateCategory( strCategory, NULL ) );
	ASSERT( pBase );
	
	iLength = strCategory.GetLength();
	tvInsert.item.pszText = strCategory.GetBuffer( iLength );
	tvInsert.item.cchTextMax = iLength;
	tvInsert.item.lParam = reinterpret_cast< ULONG >( pBase );

	m_htCategories = InsertItem( &tvInsert );
	pBase->SetTreeItem( m_htCategories );

	// Create another category, just for grins
	CPackageTreeEntryBase *pTransparent = NULL;
	strCategory = "Transparent";
	pTransparent = static_cast< CPackageTreeEntryBase * >( CreateCategory( strCategory, pBase ) );
	
	tvInsert.hParent = m_htCategories; 
	tvInsert.hInsertAfter = NULL;		
	
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.pszText = strCategory.GetBuffer( iLength );
	tvInsert.item.cchTextMax = iLength;
	tvInsert.item.iImage = m_iClosedFolderIcon;
	tvInsert.item.iSelectedImage = m_iOpenFolderIcon;	
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = reinterpret_cast< ULONG >( pTransparent );

	m_htTransparent = InsertItem( &tvInsert );
	pTransparent->SetTreeItem( m_htTransparent );

	// Create another category, just for grins
	CPackageTreeEntryBase *pTransparentFloors = NULL;
	strCategory = "Floors";
	pTransparentFloors = static_cast< CPackageTreeEntryBase * >( CreateCategory( strCategory, pTransparent ) );
	
	tvInsert.hParent = m_htTransparent; 
	tvInsert.hInsertAfter = NULL;		
	
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.pszText = strCategory.GetBuffer( iLength );
	tvInsert.item.cchTextMax = iLength;
	tvInsert.item.iImage = m_iClosedFolderIcon;
	tvInsert.item.iSelectedImage = m_iOpenFolderIcon;	
	tvInsert.item.cChildren = 0;
	tvInsert.item.lParam = reinterpret_cast< ULONG >( pTransparentFloors );

	m_htTransparentFloors = InsertItem( &tvInsert );
	pTransparentFloors->SetTreeItem( m_htTransparentFloors );
#endif
	
}

void CPackageTreeControl::InitializeImageList()
{
	m_bImageListInitialized = TRUE;
#if 0
	SetImageList (NULL, 0);

	m_ImageList.Create( GetSystemMetrics( SM_CXSMICON ), GetSystemMetrics( SM_CYSMICON ), ILC_COLOR24, 50, 50 );
	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) );

	CBitmap Bmp;
	Bmp.LoadBitmap( IDB_PACKAGE_TREE_IMAGE );    
	m_iBlankIcon = m_ImageList.Add( &Bmp, RGB( 0, 0, 0) );
	
	HICON hIcon = NULL;
	ExtractIconEx( "shell32.dll", 3, NULL, &hIcon, 1);
	m_iClosedFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	ExtractIconEx( "shell32.dll", 4, NULL, &hIcon, 1);
	m_iOpenFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	SetImageList (&m_ImageList, LVSIL_NORMAL);
#endif
}

void CPackageTreeControl::AddToAllImages( CWADItem *pItem )
{
#if 0
	TV_INSERTSTRUCT tvInsert;
	CPackageTreeEntryBase *pBase = NULL;
	CPackageTreeEntryCategory *pCategory = NULL;
	CPackageTreeEntryImage *pImage = NULL;
	CString strName("");

	if( !pItem )
	{
		ASSERT( FALSE );
		return;
	}

	strName = pItem->GetName();
	int iLength = strName.GetLength();

	pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( m_htAllImages ) );
	if( pBase->GetType() != ePackageTreeEntryCategory )
	{
		return;
	}

	pCategory = static_cast< CPackageTreeEntryCategory * >( pBase );
	pImage = static_cast< CPackageTreeEntryImage * >( pCategory->AddItem( strName, pItem ) );
	if( !pImage )
	{
		ASSERT( FALSE );
		return;
	}

	tvInsert.hParent = m_htAllImages; 
	tvInsert.hInsertAfter = NULL;	
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM; // | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvInsert.item.hItem = NULL;
	tvInsert.item.state = 0;
	tvInsert.item.stateMask = 0;
	tvInsert.item.pszText = strName.GetBuffer( iLength );
	tvInsert.item.cchTextMax = iLength;	
	tvInsert.item.cChildren = 0;
	//tvInsert.item.iImage = m_iBlankIcon;
	//tvInsert.item.iSelectedImage = m_iBlankIcon;
	tvInsert.item.lParam = reinterpret_cast< ULONG >( pImage );

	pImage->SetTreeItem( InsertItem( &tvInsert ) );
	pItem->AddPackageEntry( pImage );

	if( strName.GetAt(0) == '{' )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( m_htTransparent ) );
		if( pBase->GetType() != ePackageTreeEntryCategory )
		{
			return;
		}

		pCategory = static_cast< CPackageTreeEntryCategory * >( pBase );
		pImage = static_cast< CPackageTreeEntryImage * >( pCategory->AddItem( strName, pItem ) );
		if( !pImage )
		{
			ASSERT( FALSE );
			return;
		}

		tvInsert.hParent = m_htTransparent; 
		tvInsert.hInsertAfter = NULL;	
		tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM; // | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvInsert.item.hItem = NULL;
		tvInsert.item.state = 0;
		tvInsert.item.stateMask = 0;
		tvInsert.item.pszText = strName.GetBuffer( iLength );
		tvInsert.item.cchTextMax = iLength;	
		tvInsert.item.cChildren = 0;		
		tvInsert.item.lParam = reinterpret_cast< ULONG >( pImage );

		pImage->SetTreeItem( InsertItem( &tvInsert ) );
		pItem->AddPackageEntry( pImage );

		if( strName.GetAt(1) == 'f' )
		{
			pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( m_htTransparentFloors ) );
			if( pBase->GetType() != ePackageTreeEntryCategory )
			{
				return;
			}

			pCategory = static_cast< CPackageTreeEntryCategory * >( pBase );
			pImage = static_cast< CPackageTreeEntryImage * >( pCategory->AddItem( strName, pItem, true ) );
			if( !pImage )
			{
				ASSERT( FALSE );
				return;
			}

			tvInsert.hParent = m_htTransparentFloors;
			tvInsert.hInsertAfter = NULL;	
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM; // | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvInsert.item.hItem = NULL;
			tvInsert.item.state = 0;
			tvInsert.item.stateMask = 0;
			tvInsert.item.pszText = strName.GetBuffer( iLength );
			tvInsert.item.cchTextMax = iLength;	
			tvInsert.item.cChildren = 0;		
			tvInsert.item.lParam = reinterpret_cast< ULONG >( pImage );

			pImage->SetTreeItem( InsertItem( &tvInsert ) );
			pItem->AddPackageEntry( pImage );
		}
	}
#endif
}

BOOL CPackageTreeControl::PreTranslateMessage(MSG* pMsg) 
{
	// Handle the VK_RETURN here, since this is eaten by the Tree Control before OnKeyDown fires
	if( pMsg->message == WM_KEYUP )
	{
		/*
		if( pMsg->wParam == VK_RETURN )
		{
			CWnd* pParent = GetParent();
			if( pParent )
			{			
				pParent->SendMessage( WM_PACKAGETREECONTROL_CUSTOM, PACKAGETREECONTROL_OPENTEXTURE, 0 );
			}
		}
		*/
	}
	
	if( pMsg->message == WM_RBUTTONUP )
	{
		TRACE( "RButtonUp\n" );
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

BOOL CPackageTreeControl::SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask )
{	
	if( (hItem) && (nStateMask & TVIS_SELECTED) )
	{
		CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem) );
		if( pBase )
		{
			pBase->SetSelected( nState );
		}
	}
	return CTreeCtrl::SetItemState( hItem, nState, nStateMask );
}


CPackageTreeEntryCategory *CPackageTreeControl::CreateCategory( const char *szCategory, CPackageTreeEntryBase *pParent )
{
	CPackageTreeEntryCategory *pCategory = new CPackageTreeEntryCategory( szCategory, pParent );
	if( pCategory )
	{
		m_PackageTreeVector.push_back( static_cast< CPackageTreeEntryBase * >( pCategory ) );
	}
	return pCategory;
}

CPackageTreeEntryImage *CPackageTreeControl::CreateImage( const char *szName, CWADItem *pItem, CPackageTreeEntryBase *pParent )
{
	CPackageTreeEntryImage *pImage = new CPackageTreeEntryImage( szName, pItem, pParent );
	if( pImage )
	{
		m_PackageTreeVector.push_back( static_cast< CPackageTreeEntryBase * >( pImage ) );
	}
	return pImage;
}

BOOL CPackageTreeControl::SelectItem(HTREEITEM hItem)
{
	BOOL bReturn = CTreeCtrl::SelectItem( hItem );

	if( hItem )
	{
		CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem) );
		if( pBase )
		{
			pBase->SetSelected( GetItemState( hItem, TVIF_STATE ) );
		}
	}	
	return bReturn;
}

void CPackageTreeControl::RenameEntry( HTREEITEM hItem, const char *szName )
{
	CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
	if( pBase )
	{
		switch( pBase->GetType() )
		{
		case ePackageTreeEntryImage:
			{
				CWADItem *pWADItem = GetImageFromHTREEITEM( hItem );
				if( pWADItem )
				{
					pWADItem->SetName( szName );
					pBase->SetName( szName );

					_PackageTreeEntryVector PackageTreeEntryVector;
					pWADItem->GetPackageEntries( PackageTreeEntryVector );
					_itPackageTreeEntry itPackageTreeEntry;

					itPackageTreeEntry = PackageTreeEntryVector.begin();
					while( itPackageTreeEntry != PackageTreeEntryVector.end() )
					{
						SetItem( (*itPackageTreeEntry)->GetTreeItem(), TVIF_TEXT, szName, 0, 0, 0, 0, 0 );								
						itPackageTreeEntry++;
					}					
				}
			}
			break;

		case ePackageTreeEntryCategory:
			{
				CPackageTreeEntryCategory *pCategory = static_cast< CPackageTreeEntryCategory * >( pBase );
				pCategory->SetName( szName );				
			}
			break;

		default:
			{
			}
			break;
		}
	}
}

void CPackageTreeControl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;	
	LPTVITEM lpItem = &(pTVDispInfo->item);	
	
	CString strName("");
	if( lpItem->mask & TVIF_TEXT )
	{
		strName = lpItem->pszText;
		if( strName != "" )
		{
			RenameEntry( lpItem->hItem, strName );
			*pResult = TRUE;
			return;
		}
	}
	
	*pResult = 0;
}

void CPackageTreeControl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	LPTVITEM lpItem = &(pTVDispInfo->item);	
	CEdit* pEdit = GetEditControl();
	if( pEdit )
	{
		CPackageTreeEntryBase *pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( lpItem->hItem ) );
		if( pBase )
		{
			switch( pBase->GetType() )
			{
			case ePackageTreeEntryImage:
				{
					pEdit->SetLimitText( 15 );
				}
				break;

			case ePackageTreeEntryCategory:
			default:
				{
				}
				break;
			}
		}		
	}	
	*pResult = 0;
}

void CPackageTreeControl::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	CTreeCtrl::OnRButtonUp(nFlags, point);
	CWnd* pParent = GetParent();
	if( pParent )
	{		
		ClientToScreen( &point );
		pParent->ScreenToClient( &point );
		pParent->SendMessage( WM_RBUTTONUP, nFlags, MAKELONG( point.x, point.y));
	}
}

void CPackageTreeControl::RenameSelectedItem( CPackageDoc *pDoc )
{		
	CString strName ("");

	int iSelCount = GetSelectedCount();
	ASSERT( iSelCount == 1 );		// rename should not be possible with multiple items selected!
	HTREEITEM hItem = GetSelectedItem();
	CPackageTreeEntryBase *pBase = NULL;
	
	if( hItem )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( GetItemData( hItem ) );
		if( pBase )
		{
			pBase->RenameItem( pDoc, this );
		}
	}
}



void CPackageTreeControl::OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here	
	*pResult = !m_bSelectionComplete;
}
