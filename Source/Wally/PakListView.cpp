/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  PakListView.cpp : implementation of the CPakListView class
//
//  Created by Ty Matthews, 11-07-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "PakListView.h"
#include "PakDoc.h"
#include "MiscFunctions.h"
#include "ImageIconList.h"
//#include "objidl.h"
#include "SelectPakTreeDlg.h"
//#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPakListView

IMPLEMENT_DYNCREATE(CPakListView, CListView)

CPakListView::CPakListView()
{	
	m_bImageListInitialized = FALSE;

	//m_oleDelayRender.SetPakList (this);
}

CPakListView::~CPakListView()
{
}


//BEGIN_MESSAGE_MAP(CPakListView, CListView)
BEGIN_MESSAGE_MAP(CPakListView, CListView)
	//{{AFX_MSG_MAP(CPakListView)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_PAK_POPUP_RENAME, OnPakPopupRename)
	ON_UPDATE_COMMAND_UI(ID_PAK_POPUP_RENAME, OnUpdatePakPopupRename)
	ON_COMMAND(ID_PAK_POPUP_OPEN, OnPakPopupOpen)
	ON_UPDATE_COMMAND_UI(ID_PAK_POPUP_OPEN, OnUpdatePakPopupOpen)
	ON_COMMAND(ID_PAK_POPUP_DELETE, OnPakPopupDelete)
	ON_UPDATE_COMMAND_UI(ID_PAK_POPUP_DELETE, OnUpdatePakPopupDelete)
	ON_COMMAND(ID_PAK_POPUP_COPY, OnPakPopupCopy)
	ON_UPDATE_COMMAND_UI(ID_PAK_POPUP_COPY, OnUpdatePakPopupCopy)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_PAK_POPUP_PASTE, OnPakPopupPaste)
	ON_UPDATE_COMMAND_UI(ID_PAK_POPUP_PASTE, OnUpdatePakPopupPaste)
	ON_COMMAND(ID_PAK_POPUP_NEW_FOLDER, OnPakPopupNewFolder)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_COMMAND(ID_PAK_PLAYSOUND, OnPakPlaysound)
	ON_UPDATE_COMMAND_UI(ID_PAK_PLAYSOUND, OnUpdatePakPlaysound)
	ON_COMMAND(ID_PAK_POPUP_NEW_TXTFILE, OnPakPopupNewTxtfile)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_PACKAGE_OPEN, OnPackageOpen)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_OPEN, OnUpdatePackageOpen)
	ON_COMMAND(ID_PACKAGE_EXPORT, OnPackageExport)
	ON_COMMAND(ID_PAK_POPUP_EXPORT, OnPakPopupExport)
	ON_COMMAND(ID_PAK_POPUP_IMPORT, OnPakPopupImport)
	ON_COMMAND(ID_PACKAGE_ADD, OnPackageAdd)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPakListView drawing

void CPakListView::OnDraw(CDC* pDC)
{
	CPakDoc *pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPakListView diagnostics

#ifdef _DEBUG
void CPakListView::AssertValid() const
{
	CListView::AssertValid();
}

void CPakListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CPakDoc* CPakListView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPakDoc)));
	return (CPakDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPakListView message handlers

void CPakListView::InitializeImageList()
{
	m_bImageListInitialized = TRUE;

	m_ImageList.Create( GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON), 
		ILC_COLOR24, 50, 50);

	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) );

	CBitmap Bmp;
    CBitmap *pOld;
    CBitmap *pNewBmp;
    CDC DC;

    DC.CreateCompatibleDC(NULL);
    Bmp.CreateBitmap (32,32,1,24,NULL);
    pOld=DC.SelectObject (&Bmp);
    
	DC.FillSolidRect (0,0,31,31,GetSysColor(COLOR_WINDOW));
    pNewBmp = DC.SelectObject (pOld);
    DC.DeleteDC ();
    
	m_ImageList.Add (pNewBmp, COLORREF(0x00));

	// Add the closed folder for child folders
	HICON hIcon = NULL;
	ExtractIconEx( "shell32.dll", 3, NULL, &hIcon, 1);
	m_iClosedFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	GetListCtrl().SetImageList (&m_ImageList, LVSIL_SMALL);
}	

void CPakListView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	DragAcceptFiles (true);
	m_oleDropTarget.Register (this);
		
	GetListCtrl().InsertColumn (0, "Name", LVCFMT_LEFT, 200, 0);
	GetListCtrl().InsertColumn (1, "Size", LVCFMT_RIGHT, 100, 1);
	GetListCtrl().InsertColumn (2, "Type", LVCFMT_LEFT, 150, 2);	

	UpdateList();
}

void CPakListView::UpdateList()
{
	if (!m_bImageListInitialized)
	{
		InitializeImageList();
	}

	GetListCtrl().DeleteAllItems();

	CPakDoc *pDoc = GetDocument();
	CPakItem *pItem = NULL;

	CString strText("");
	CString strExtension("");
	LV_ITEM lvText;

	pItem = pDoc->GetFirstPakItem();
	int iIcon = 0;
	int iItem = 0;

	SHFILEINFO shInfo;

	// First add the folders (children of the current directory)
	CPakDirectory *pDirectory = pDoc->GetCurrentDirectory()->GetFirstDirectory();
	lpPAK_PARAM lpPakParam = NULL;
	
	while (pDirectory)
	{
		strText = pDirectory->GetDisplayName();
		lpPakParam = (lpPAK_PARAM)(*pDirectory);
		
		lvText.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = strText.GetBuffer(strText.GetLength());
		lvText.cchTextMax = strText.GetLength();
		lvText.lParam = (ULONG)lpPakParam;
		lvText.iImage = m_iClosedFolderIcon;

		GetListCtrl().InsertItem (&lvText);

		pDirectory = pDirectory->GetNext();		
	}

	while (pItem)
	{
		strText = pItem->GetDisplayName();		
		lpPakParam = (lpPAK_PARAM)(*pItem);

		iIcon = pItem->GetIconIndex();
		
		if (iIcon == -1)
		{
			strExtension = GetExtension (strText);

			SHGetFileInfo( strExtension, FILE_ATTRIBUTE_NORMAL,
				&shInfo, 
				sizeof(shInfo), 
				SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON);

			pItem->SetType (shInfo.szTypeName);
			pItem->SetDisplayType (shInfo.szTypeName);
			iIcon = m_ImageList.Add(shInfo.hIcon, shInfo.iIcon);

			pItem->SetIconIndex (iIcon);
		}		
		
		lvText.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = strText.GetBuffer(strText.GetLength());
		lvText.cchTextMax = strText.GetLength();
		lvText.lParam = (ULONG)lpPakParam;
		lvText.iImage = iIcon;

		iItem = GetListCtrl().InsertItem (&lvText);

		strText.Format ("%luKB", (pItem->GetDisplaySize() + 1024) / 1024);
		GetListCtrl().SetItemText (iItem, 1, strText.GetBuffer(strText.GetLength()));

		strText = pItem->GetDisplayType ();
		GetListCtrl().SetItemText (iItem, 2, strText.GetBuffer(strText.GetLength()));

		pItem = pDoc->GetNextPakItem();		
	}

	GetListCtrl().SortItems(PakListViewCompareItems, (LPARAM) pDoc);
}

BOOL CPakListView::PreCreateWindow(CREATESTRUCT& cs) 
{	
	cs.style |= LVS_EDITLABELS;
	cs.style |= LVS_REPORT;
	cs.style |= WS_EX_ACCEPTFILES;
	cs.style |= LVS_ICON;
	cs.style &= ~(LVS_SORTDESCENDING | LVS_SORTASCENDING);
	
	return CListView::PreCreateWindow(cs);
}

void CPakListView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	LVITEM pItem = pDispInfo->item;
	CPakDoc *pDoc = GetDocument();
	
	// Only allow updates if there's still text there
	CString strText("");

	if (pItem.mask & LVIF_TEXT)
	{
		strText = pItem.pszText;
	}
	
	if (strText != "")
	{
		LV_ITEM lvSelected;
		lvSelected.mask = LVIF_PARAM;
		lvSelected.iItem = pItem.iItem;
		
		if (GetListCtrl().GetItem (&lvSelected))
		{
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvSelected.lParam;

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					if (pDoc->GetCurrentDirectory()->FindNameInList (strText))
					{
						AfxMessageBox ("That item name already exists.  Please enter a unique name.", MB_ICONSTOP);
						*pResult = FALSE;
						
						GetListCtrl().EditLabel(pItem.iItem);
						return;
					}

					CPakItem *pPakItem = (CPakItem *)(lpPakParam->pPakItem);
					pPakItem->SetShortName (strText);
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					if (pDoc->GetCurrentDirectory()->FindDirectoryInList (strText))
					{
						AfxMessageBox ("That directory name already exists.  Please enter a unique name.", MB_ICONSTOP);
						*pResult = FALSE;

						GetListCtrl().EditLabel(pItem.iItem);
						return;
					}

					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);
					pPakDirectory->SetDirectoryName (strText);

					// Go rebuild the tree, seeing how we've changed names here
					if (pDoc)
					{
						pDoc->BuildTree();
					}					
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}			
		}
	}

	// Update the list
	//UpdateList();
	
	// Always set this to false... we accept the change and manually make our changes.  UpdateList() will refresh the view
	// with the correct name
	*pResult = TRUE;
}

void CPakListView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	// Need this point to position the menu, relative to the screen.
	CPoint ptScreenPos;
	GetCursorPos( &ptScreenPos);
	
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_PAK_POPUP));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptScreenPos.x, ptScreenPos.y,
		pWndPopupOwner);	

}


void CPakListView::OpenSelected()
{
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	CPakDoc *pDoc = GetDocument();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	
	while( iItem != -1 )
	{
		lvItem.iItem = iItem;
		if (GetListCtrl().GetItem (&lvItem))
		{
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvItem.lParam;

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					CPakItem *pPakItem = (CPakItem *)(lpPakParam->pPakItem);
					pDoc->OpenItem (pPakItem);
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);
					CString strPath = pPakDirectory->GetPathFromHere('/');

					if (pDoc)
					{
						pDoc->HighlightDirectory (strPath, FALSE);
					}
				}
				break;

			default:
				ASSERT (FALSE);
			}
		}
		
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}
}

void CPakListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;	
	*pResult = 0;

	OpenSelected();	
}

void CPakListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_RETURN:
		{
			OpenSelected();
		}
		break;
	}
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPakListView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	CPakDoc *pDoc = GetDocument();
	pDoc->SetSortOrder (pNMListView->iSubItem);
	GetListCtrl().SortItems(PakListViewCompareItems, (LPARAM) pDoc);
	
	*pResult = 0;
}

int CALLBACK PakListViewCompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CPakDoc *pDoc = (CPakDoc*) lParamSort;
	lpPAK_PARAM lpPakLeft = (lpPAK_PARAM)lParam1;
	lpPAK_PARAM lpPakRight = (lpPAK_PARAM)lParam2;

	CPakBase *pLeft = lpPakLeft->pPakBase;
	CPakBase *pRight = lpPakRight->pPakBase;

	int nResult = 0;

	// Directories will always get displayed first
	if (lpPakLeft->iPakType != lpPakRight->iPakType)
	{
		return (lpPakLeft->iPakType == PAK_TYPE_DIRECTORY ? -1 : 1);
	}
	
	switch (pDoc->GetSortOrder())
	{
	case 0: // File name (display name)
		{			
			CString strLeft("");
			CString strRight("");			

			strLeft = pLeft->GetDisplayName();
			strRight = pRight->GetDisplayName();
						
			nResult = strLeft.CompareNoCase(strRight);
		}
		break;

	case 1: // File size (display size)
		{
			int iLeft = 0;
			int iRight = 0;

			iLeft = pLeft->GetDisplaySize();
			iRight = pRight->GetDisplaySize();				
						
			if (iLeft > iRight)
				nResult = 1;
			else if (iLeft < iRight)
				nResult = -1;
			else
				nResult = 0;
		}
		break;

	case 2: // File type (display type)
		{
			CString strLeft(""); 
			CString strRight("");

			strLeft = pLeft->GetDisplayType();
			strRight = pRight->GetDisplayType();
						
			nResult = strLeft.CompareNoCase(strRight);
		}
		break;
	
	default:
		ASSERT(FALSE);
	}

	return nResult;
}

BOOL CPakListView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO:  Fix this to scroll properly?
	return TRUE;	
	//return CListView::OnMouseWheel(nFlags, zDelta, pt);
}

void CPakListView::UpdateWhenSelected (CCmdUI* pCmdUI, int iFlags /* = 0 */)
{
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	int iCounter = 0;

	while( iItem != -1 )
	{
		iCounter++;
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);

		if ((iCounter > 1) && (iFlags & PAK_UPDATE_FLAG_ONLYONE))
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}

	if (iFlags & PAK_UPDATE_FLAG_ONLYONE)
	{
		pCmdUI->Enable (iCounter == 1);
		return;
	}

	if (iFlags & PAK_UPDATE_FLAG_ATLEASTONE)
	{
		pCmdUI->Enable(iCounter > 0);
		return;
	}

	pCmdUI->Enable (FALSE);
}

void CPakListView::OnPakPopupRename() 
{
	// F2 key as well
	int iSelected = 0;
	int iItem = iSelected = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	int iCounter = 0;
	
	
	while( iItem != -1 )
	{
		iCounter++;

		if (iCounter > 1)
		{			
			return;
		}
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}

	GetListCtrl().EditLabel(iSelected);
}

void CPakListView::OnUpdatePakPopupRename(CCmdUI* pCmdUI) 
{
	UpdateWhenSelected (pCmdUI, PAK_UPDATE_FLAG_ONLYONE);
}

void CPakListView::OnPakPopupOpen() 
{
	OpenSelected();	
}

void CPakListView::OnUpdatePakPopupOpen(CCmdUI* pCmdUI) 
{
	UpdateWhenSelected (pCmdUI, PAK_UPDATE_FLAG_ATLEASTONE);
}

void CPakListView::OnPackageOpen() 
{
	OpenSelected();	
}

void CPakListView::OnUpdatePackageOpen(CCmdUI* pCmdUI) 
{
	UpdateWhenSelected (pCmdUI, PAK_UPDATE_FLAG_ATLEASTONE);	
}

void CPakListView::DeleteSelected()
{
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	CPakDoc *pDoc = GetDocument();

	if (!pDoc)
	{
		return;
	}

	BOOL bRebuildTree = FALSE;
	BOOL bUpdateList = FALSE;

	LV_ITEM lvItem;
	lvItem.mask = LVIF_PARAM;
	
	while( iItem != -1 )
	{
		lvItem.iItem = iItem;
		if (GetListCtrl().GetItem (&lvItem))
		{
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvItem.lParam;

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					CPakItem *pPakItem = (CPakItem *)(lpPakParam->pPakItem);
					pDoc->DeleteEntry (pPakItem);
					bUpdateList = TRUE;
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);
					pDoc->DeleteEntry (pPakDirectory);
					bRebuildTree = TRUE;
					bUpdateList = TRUE;
				}
				break;

			default:
				ASSERT (FALSE);
			}
		}
		
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}

	if (bRebuildTree)
	{
		// Go rebuild the tree, seeing how we've changed names here					
		pDoc->BuildTree();
	}

	if (bUpdateList)
	{
		UpdateList();
	}
}

void CPakListView::OnPakPopupDelete() 
{
	int iReturn = AfxMessageBox ("Are you sure you want to delete the selected items?", MB_YESNO);

	if (iReturn == IDYES)
	{
		DeleteSelected();
	}
}

void CPakListView::OnEditClear() 
{
	// This to handle the DELETE accelerator
	OnPakPopupDelete();		
}

void CPakListView::OnUpdatePakPopupDelete(CCmdUI* pCmdUI) 
{
	UpdateWhenSelected (pCmdUI, PAK_UPDATE_FLAG_ATLEASTONE);	
}

int CPakListView::GetSelectedCount()
{
	int iCount = 0;
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	
	while( iItem != -1 )
	{
		iCount++;
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}

	return iCount;
}

UINT CPakListView::GetSelectedCountPlusChildren()
{
	UINT iCount = 0;
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_PARAM;	
		
	while( iItem != -1 )
	{	
		lvItem.iItem = iItem;
		if (GetListCtrl().GetItem (&lvItem))
		{
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvItem.lParam;

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					iCount++;
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);
					pPakDirectory->GetItemCount (&iCount);					
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}
		}

		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}
	
	return iCount;
}

HGLOBAL CPakListView::RenderData()
{
	CString strFileName("");
	
	HGLOBAL hGlobal;

	CPakDoc *pDoc = GetDocument();

	if (!pDoc)
	{
		SetErrorString ("There is no attached document.");
		return NULL;
	}

	CPakList *pPakList = pDoc->GetPakList();

	if (!pPakList)
	{
		SetErrorString ("There is no attached PAK list.");
		return NULL;
	}

	int iCount = pPakList->GetPasteCount();
	
	if (iCount)
	{
		// allocate space for DROPFILE structure plus the number of file and one extra byte for final NULL terminator
		int iDropDataSize = sizeof(DROPFILES) + ((_MAX_PATH) * iCount) + 1;
		
		hGlobal = GlobalAlloc (
			GHND | GMEM_SHARE | GMEM_ZEROINIT, 
			(DWORD) (iDropDataSize) );
		
		if (!hGlobal)
		{
			SetErrorString ("Failed to allocate HGLOBAL.");
			return NULL;
		}
	
		LPDROPFILES pDropFiles = NULL;
		BYTE *pbyHGlobal = (BYTE *)GlobalLock(hGlobal);
		
		pDropFiles = (LPDROPFILES)pbyHGlobal;
		
		// set the offset where the starting point of the files start
		pDropFiles->pFiles = sizeof(DROPFILES);
		
		// file contains wide characters
		pDropFiles->fWide = FALSE;

		int iCurPosition = sizeof(DROPFILES);

		CPakPasteItem *pItem = pPakList->GetFirstPasteItem();
		CPakBase *pPakBase = NULL;

		while (pItem)
		{			
			pPakBase = pItem->GetPakBase();
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)(*pPakBase);

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					CPakItem *pPakItem = (CPakItem *)(lpPakParam->pPakItem);

					// Go create the file
					if (!pPakList->CreateItem (pPakItem))
					{
						SetErrorString (pPakList->GetError());
						return NULL;
					}
					
					// Grab it's name
					strFileName = pPakItem->GetDataFile();
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);

					// Go create the directory (and all it's children)
					if (!pPakList->CreateDirectory (pPakDirectory))
					{
						SetErrorString (pPakList->GetError());
						return NULL;
					}

					strFileName = pPakDirectory->GetTempDirectoryName();
				}
				break;

			default:
				break;
			}

			// copy the file into global memory
			lstrcpy( (LPSTR) ((LPSTR)(pDropFiles) + iCurPosition), TEXT(strFileName));
			
			// move the current position beyond the file name copied
			// don't forget the Null terminator (+1)
			iCurPosition += strFileName.GetLength() + 1;

			pItem = pItem->GetNext();
		}				
		
		// final null terminator as per CF_HDROP Format specs.
		((LPSTR) pDropFiles) [iCurPosition] = 0;

		GlobalUnlock(hGlobal);		
	}
	else
	{
		SetErrorString ("There were no files to copy.");
		return NULL;
	}

	return hGlobal;
}

void CPakListView::AddPasteItems()
{
	// We need to add the currently selected items to this custom list FIRST.  This 
	// is because we're using delayed rendering, and it is entirely possible (and likely)
	// that the user clicks on other directories or files and invalidates the current
	// selections at the time of the Copy command (or drag) is initiated.

	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakList *pPakList = pDoc->GetPakList();

		if (pPakList)
		{
			// Purge any existing entries
			pPakList->PurgePasteList();

			int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
			LV_ITEM lvItem;
			lvItem.mask = LVIF_PARAM;	
			
			while( iItem != -1 )
			{	
				lvItem.iItem = iItem;
				if (GetListCtrl().GetItem (&lvItem))
				{
					lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvItem.lParam;

					pPakList->AddToPasteList (lpPakParam->pPakBase);
				}					
				
				iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);				
			}

		}		// if (pPakList)
	}			// if (pDoc)
}

void CPakListView::OnPakPopupCopy()
{	
	AddPasteItems();
	CDelayRender *pDelayData = new CDelayRender(this);
	pDelayData->SetClipboard();	
}

void CPakListView::OnUpdatePakPopupCopy(CCmdUI* pCmdUI) 
{
	UpdateWhenSelected (pCmdUI, PAK_UPDATE_FLAG_ATLEASTONE);	
}

void CPakListView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	AddPasteItems();
	CDelayRender DelayData(this);
	DelayData.DoDragDrop();
	
	*pResult = 0;
}

DROPEFFECT CPakListView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (pDataObject->IsDataAvailable(CF_HDROP)) 
	{
		return DROPEFFECT_COPY;		
	}

	return DROPEFFECT_NONE;
	//return CListView::OnDragOver(pDataObject, dwKeyState, point);
}


BOOL CPakListView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_DROPFILES)
	{
		DoDragDrop (pMsg);
		return true;
	}		
	return CListView::PreTranslateMessage(pMsg);
}

void CPakListView::DoDragDrop(MSG *pMsg)
{
	CPakDoc *pDoc = GetDocument();
	HDROP hDrop = (HDROP)pMsg->wParam;

	char szPath[_MAX_PATH];
	
	// Figure out how many files we're dealing with
	int iCount = DragQueryFile (hDrop, 0xffffffff, szPath, _MAX_PATH);

	BOOL bRefreshView = FALSE;

	for (int j = 0; j < iCount; j++)
	{
		DragQueryFile (hDrop, j, szPath, _MAX_PATH);
		if (pDoc->AddFile (szPath, (j == iCount - 1) ? FALSE : TRUE))
		{
			bRefreshView = TRUE;
		}
	}
	DragFinish(hDrop);

	if (bRefreshView)
	{
		UpdateList();
	}
}

void CPakListView::OnPakPopupPaste() 
{
	CPakDoc *pDoc = GetDocument();

	// Post a message to the CWnd in the Doc class.  This allows us to continue on, and process our own messages for
	// rendering the data, which we would otherwise have to be waiting for the document to finish it's thing
	(pDoc->GetWnd())->PostMessage (WM_PAKDOC_CUSTOM, PAK_DOC_MESSAGE_PASTE, 0);	
}

void CPakListView::OnUpdatePakPopupPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (::IsClipboardFormatAvailable(CF_HDROP));
}

void CPakListView::OnEditPaste() 
{
	OnPakPopupPaste();	
}

void CPakListView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (::IsClipboardFormatAvailable(CF_HDROP));	
}

void CPakListView::OnPakPopupNewFolder() 
{
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakDirectory *pDirectory = pDoc->GetCurrentDirectory();
		CPakDirectory *pNewDirectory = pDirectory->AddChildDirectory ("New folder");

		LV_ITEM lvText;
		lpPAK_PARAM lpPakParam = NULL;
	
		CString strText("");
		strText = pNewDirectory->GetDisplayName();
		lpPakParam = (lpPAK_PARAM)(*pNewDirectory);
		
		lvText.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = strText.GetBuffer(strText.GetLength());
		lvText.cchTextMax = strText.GetLength();
		lvText.lParam = (ULONG)lpPakParam;
		lvText.iImage = m_iClosedFolderIcon;

		int iItem = GetListCtrl().InsertItem (&lvText);
		GetListCtrl().EditLabel(iItem);
	}
}

void CPakListView::OnPakPopupNewTxtfile() 
{
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakDirectory *pDirectory = pDoc->GetCurrentDirectory();
		CString strFileName("");
		CString strCount("");
		CString strShortName("");

		BOOL bDone = FALSE;
		int iCount = 0;

		while (!bDone)
		{
			strCount.Format ("%d", iCount);
			strShortName.Format ("New Text Document%s.txt", iCount == 0 ? "" : strCount.GetBuffer());

			if (!pDirectory->FindNameInList (strShortName))
			{			
				strFileName.Format ("%s\\%s", TrimSlashes (g_strTempDirectory), strShortName);

				FILE* fp = NULL;
				errno_t err = fopen_s(&fp, strFileName, "r");

				if (err == 0)
				{				
					fclose (fp);
				}
				else
				{
					err = fopen_s(&fp, strFileName, "w");

					if (err == 0)
					{
						fprintf (fp, "\n");
						fclose (fp);
						bDone = TRUE;
					}
					else
					{
						CString strError("");
						strError.Format ("Failed to create %s.", strFileName);
						AfxMessageBox (strError, MB_ICONSTOP);
						return;
					}
				}
			}

			iCount++;
		}

		if (!pDirectory->AddFile (strFileName))
		{
			remove (strFileName);
			AfxMessageBox (pDirectory->GetError(), MB_ICONSTOP);
			return;
		}

		remove (strFileName);
		CPakItem *pItem = pDirectory->FindNameInList (strFileName);

		ASSERT (pItem);
		
		LV_ITEM lvText;
		lpPAK_PARAM lpPakParam = NULL;
	
		CString strText("");
		strText = pItem->GetDisplayName();
		lpPakParam = (lpPAK_PARAM)(*pItem);
		
		int iIcon = 0;		
		CString strExtension (".txt");

		SHFILEINFO shInfo;
		SHGetFileInfo( strExtension, FILE_ATTRIBUTE_NORMAL,
			&shInfo, 
			sizeof(shInfo), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON);

		pItem->SetType (shInfo.szTypeName);
		pItem->SetDisplayType (shInfo.szTypeName);		
		iIcon = m_ImageList.Add(shInfo.hIcon, shInfo.iIcon);

		pItem->SetIconIndex (iIcon);
				
		lvText.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvText.iItem = 4096;
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = strText.GetBuffer(strText.GetLength());
		lvText.cchTextMax = strText.GetLength();
		lvText.lParam = (ULONG)lpPakParam;
		lvText.iImage = iIcon;

		int iItem = GetListCtrl().InsertItem (&lvText);

		strText.Format ("%luKB", (pItem->GetDisplaySize() + 1024) / 1024);
		GetListCtrl().SetItemText (iItem, 1, strText.GetBuffer(strText.GetLength()));

		strText = pItem->GetDisplayType ();
		GetListCtrl().SetItemText (iItem, 2, strText.GetBuffer(strText.GetLength()));		
		GetListCtrl().EditLabel(iItem);
		
	}
}

void CPakListView::OnPakPlaysound() 
{
	g_bPakQuickPlaySound = !g_bPakQuickPlaySound;
}

void CPakListView::OnUpdatePakPlaysound(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_bPakQuickPlaySound);	
}


BOOL CPakListView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		UpdateStatusBar();
	}	
	return CListView::OnSetCursor(pWnd, nHitTest, message);
}


void CPakListView::UpdateStatusBar()
{
	UINT iItemCount = 0;
	UINT iItemSize = 0;
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakDirectory *pDirectory = pDoc->GetCurrentDirectory();
		if (pDirectory)
		{
			pDirectory->GetItemCount (&iItemCount, &iItemSize, NULL, PAK_COUNT_SELF_ONLY);
			CString strText("");

			// Name and Num Items
			strText.Format ("%d item%s", iItemCount, iItemCount == 1 ? "" : "s");
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), strText, true);
					
			// total size
			if (iItemSize != 0)
			{
				strText.Format ("%luKB", (iItemSize + 1024) / 1024);
			}
			else
			{
				strText = "";
			}
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), strText, true);

			// Item width and height
			strText = "";
			m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), strText, true);			
			m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), strText, true);		
		}	
		else	          
		{
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), "", true);
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), "",true);
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), "", true);
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), "", true);		
		}		
	}

}



void CPakListView::OnPackageExport() 
{
	ExportFiles();
}

void CPakListView::OnPakPopupExport() 
{
	ExportFiles();
}

void CPakListView::OnPakPopupImport() 
{		
	ImportFiles();
}

void CPakListView::OnPackageAdd() 
{	
	ImportFiles();
}

void CPakListView::ImportFiles()
{
	CPakDoc *pDoc = GetDocument();
	
	if (pDoc)
	{
		pDoc->ImportFiles();
	}	
}

void CPakListView::ExportFiles()
{
	CPakDoc *pDoc = GetDocument();
	
	if (pDoc)
	{
		pDoc->ExportFiles(GetSelectedCount() > 0, (CView *)this);
	}	
}

void CPakListView::ExportSelected(LPCTSTR szDirectory)
{
	int iItem = GetListCtrl().GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	LV_ITEM lvItem;
	lvItem.mask = LVIF_PARAM;	
	
	CPakDoc *pDoc = GetDocument();
	CString strPakFile = pDoc->GetPakFileName();
	
	CProgressBar ctlProgress ("Exporting...", 70, GetSelectedCountPlusChildren(), false, 0);

	while( iItem != -1 )
	{	
		lvItem.iItem = iItem;
		if (GetListCtrl().GetItem (&lvItem))
		{
			lpPAK_PARAM lpPakParam = (lpPAK_PARAM)lvItem.lParam;

			switch (lpPakParam->iPakType)
			{
			case PAK_TYPE_ITEM:
				{
					CPakItem *pPakItem = (CPakItem *)(lpPakParam->pPakItem);
					if (!pPakItem->ExportFile (strPakFile, szDirectory, NULL, &ctlProgress))
					{
						CString strError("");
						strError.Format ("%s\n\nDo you want to continue with the export?", pPakItem->GetError());
						
						int iReturn = AfxMessageBox (strError, MB_YESNO);

						if (iReturn == IDNO)
						{
							return;
						}						
					}
				}
				break;

			case PAK_TYPE_DIRECTORY:
				{
					CPakDirectory *pPakDirectory = (CPakDirectory *)(lpPakParam->pPakDirectory);
					if (!pPakDirectory->ExportFiles (szDirectory, NULL, &ctlProgress))
					{						
						CString strError("");
						strError.Format ("%s\n\nDo you want to continue with the export?", pPakDirectory->GetError());
						
						int iReturn = AfxMessageBox (strError, MB_YESNO);

						if (iReturn == IDNO)
						{
							return;
						}						
					}
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}			
		}					
		
		iItem = GetListCtrl().GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED);
	}

	ctlProgress.Clear();
}

