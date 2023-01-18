// PakTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PakTreeView.h"
#include "PakDoc.h"
#include "PakList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPakTreeView

IMPLEMENT_DYNCREATE(CPakTreeView, CTreeView)

CPakTreeView::CPakTreeView()
{
	m_bImageListInitialized = FALSE;
}

CPakTreeView::~CPakTreeView()
{
}


BEGIN_MESSAGE_MAP(CPakTreeView, CTreeView)
	//{{AFX_MSG_MAP(CPakTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_PAK_POPUP_RENAME, OnPakPopupRename)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_UPDATE_COMMAND_UI(ID_PAK_PLAYSOUND, OnUpdatePakPlaysound)
	ON_COMMAND(ID_PAK_PLAYSOUND, OnPakPlaysound)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_PACKAGE_ADD, OnPackageAdd)
	ON_COMMAND(ID_PACKAGE_EXPORT, OnPackageExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakTreeView drawing

void CPakTreeView::OnDraw(CDC* pDC)
{
	CPakDoc *pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPakTreeView diagnostics

#ifdef _DEBUG
void CPakTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CPakTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CPakDoc* CPakTreeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPakDoc)));
	return (CPakDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPakTreeView message handlers

BOOL CPakTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_EDITLABELS;
	cs.style |= TVS_HASLINES;
	cs.style |= TVS_LINESATROOT;
	cs.style |= TVS_HASBUTTONS;

	return CTreeView::PreCreateWindow(cs);
}

void CPakTreeView::InitializeImageList()
{
	m_bImageListInitialized = TRUE;

	GetTreeCtrl().SetImageList (NULL, 0);

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
    
	m_iBlankIcon = m_ImageList.Add (pNewBmp, COLORREF(0x00));
	
	HICON hIcon = NULL;
	ExtractIconEx( "shell32.dll", 3, NULL, &hIcon, 1);
	m_iClosedFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	ExtractIconEx( "shell32.dll", 4, NULL, &hIcon, 1);
	m_iOpenFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	GetTreeCtrl().SetImageList (&m_ImageList, LVSIL_NORMAL);
}

void CPakTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();	
	BuildTree();
}

void CPakTreeView::BuildTree(BOOL bRetainPosition /* = FALSE */)
{
	if (!m_bImageListInitialized)
	{
		InitializeImageList();
	}

	CString strCurrentDirectory("");
	BOOL bExpand = FALSE;

	if (bRetainPosition)
	{
		HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();

		if (htItem)
		{
			TV_ITEM tvItem;

			tvItem.mask = TVIF_PARAM | TVIF_STATE;
			tvItem.hItem = htItem;
			
			GetTreeCtrl().GetItem (&tvItem);

			bExpand = (tvItem.state & TVIS_EXPANDED);

			CPakDirectory *pDirectory = (CPakDirectory *)(tvItem.lParam);

			if (pDirectory)
			{
				strCurrentDirectory = pDirectory->GetPathFromHere('/');
			}
		}
	}

	GetTreeCtrl().DeleteAllItems();

	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakDirectory *pDirectory = pDoc->GetRootDirectory();

		pDirectory->BuildTree (GetTreeCtrl(), TVI_ROOT, m_iClosedFolderIcon, m_iOpenFolderIcon);

		// Always expand the root item
		HTREEITEM htItem = GetTreeCtrl().GetRootItem();
		GetTreeCtrl().Expand (htItem, TVE_EXPAND);

		if (bRetainPosition)
		{
			HighlightDirectory (strCurrentDirectory, bExpand);
		}
	}
}

void CPakTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM tvItem = pNMTreeView->itemNew;

	CPakDoc *pDoc = GetDocument();
	
	if (pDoc)
	{
		CPakDirectory *pDirectory = (CPakDirectory *)(tvItem.lParam);
		pDoc->SetCurrentDirectory (pDirectory);	
	}
		
	*pResult = 0;
}


BOOL CPakTreeView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO:  Fix this to scroll properly?
	return TRUE;
	//return CTreeView::OnMouseWheel(nFlags, zDelta, pt);
}

CString CPakTreeView::GrabNextDirectory (CString *szPath)
{
	int iPosition = 0;
	CString strSource (*szPath);
	int iLength = strSource.GetLength();
	BOOL bDone = FALSE;
	int iFinalPosition = 0;

	for (iPosition = 0; (iPosition < iLength) && !bDone; iPosition++)
	{
		if (strSource.GetAt(iPosition) == '/')
		{
			iFinalPosition = iPosition;
			bDone = TRUE;			
		}	
	}

	strSource = strSource.Left (iFinalPosition);
	(*szPath) = (*szPath).Right (iLength - iFinalPosition - 1);

	return strSource;
}

void CPakTreeView::HighlightDirectory (LPCTSTR szPath, BOOL bExpand /* = FALSE */)
{		
	CString strRollingPath(szPath);
	
	if (strRollingPath == "")
	{
		return;
	}

	CPakDoc *pDoc = GetDocument();
		
	CTreeCtrl &thisTree = GetTreeCtrl();

	HTREEITEM htItem = thisTree.GetRootItem();
	htItem = thisTree.GetChildItem (htItem);
	TV_ITEM tvItem;

	tvItem.mask = TVIF_PARAM;
	
	BOOL bDone = FALSE;
	CString strDirectory ("");
	//CString strNextDirectory ("");
	CString strItemText("");
	
	strDirectory = GrabNextDirectory (&strRollingPath);
	CPakDirectory *pDirectory = NULL;
	
	while ((!bDone) && (htItem))
	{
		tvItem.hItem = htItem;
		GetTreeCtrl().GetItem (&tvItem);

		pDirectory = (CPakDirectory *)(tvItem.lParam);

		strItemText = pDirectory->GetDirectoryName();
				
		if (!strItemText.CompareNoCase (strDirectory))
		{
			strDirectory = GrabNextDirectory (&strRollingPath);			

			if (strDirectory != "")			
			{
				thisTree.Expand (htItem, TVE_EXPAND);
				htItem = thisTree.GetChildItem (htItem);
			}
			else // strDirectory == ""
			{
				// We've found the last directory in the desired path
				if (bExpand)
				{
					thisTree.Expand (htItem, TVE_EXPAND);
				}

				thisTree.SelectItem (htItem);

				if (pDoc)
				{
					pDoc->SetCurrentDirectory (pDirectory);
				}
				
				bDone = TRUE;
			}
		}
		else
		{		
			htItem = thisTree.GetNextSiblingItem (htItem);
		}		
	}
}


void CPakTreeView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	LPTVITEM lpItem = &(pTVDispInfo->item);

	CString strDirectory("");
	
	if (lpItem->mask & TVIF_TEXT)
	{
		strDirectory = lpItem->pszText;	

		if (strDirectory != "")
		{	
			*pResult = TRUE;
			HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();

			TV_ITEM tvItem;

			tvItem.mask = TVIF_PARAM;
			tvItem.hItem = htItem;
				
			GetTreeCtrl().GetItem (&tvItem);

			CPakDirectory *pDirectory = (CPakDirectory *)(tvItem.lParam);

			if (pDirectory)
			{
				pDirectory->SetDirectoryName (strDirectory);			
			}
		}
		else
		{
			*pResult = NULL;
		}
	}
}

void CPakTreeView::OnPakPopupRename() 
{
	// F2 key as well
	HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();
	GetTreeCtrl().EditLabel(htItem);
}

void CPakTreeView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();

	// The root directory isn't really a directory in the PAK file.  Renaming it doesn't make sense
	if (htItem == GetTreeCtrl().GetRootItem())
	{
		*pResult = 1;
		return;
	}
	
	*pResult = 0;
}

void CPakTreeView::OnPakPlaysound() 
{
	g_bPakQuickPlaySound = !g_bPakQuickPlaySound;	
}

void CPakTreeView::OnUpdatePakPlaysound(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_bPakQuickPlaySound);
}

BOOL CPakTreeView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		UpdateStatusBar();
	}		
	return CTreeView::OnSetCursor(pWnd, nHitTest, message);
}

void CPakTreeView::UpdateStatusBar()
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

void CPakTreeView::OnPackageAdd() 
{	
	ImportFiles();
}

void CPakTreeView::OnPackageExport() 
{	
	ExportFiles();
}

void CPakTreeView::ImportFiles()
{
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		pDoc->ImportFiles();
	}
}

void CPakTreeView::ExportFiles()
{
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		pDoc->ExportFiles(GetTreeCtrl().GetSelectedItem() != NULL, (CView *)this);
	}
}

void CPakTreeView::ExportSelected(LPCTSTR szDirectory)
{
	CPakDoc *pDoc = GetDocument();
	CString strPakFile = pDoc->GetPakFileName();

	HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();

	TV_ITEM tvItem;

	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = htItem;
		
	GetTreeCtrl().GetItem (&tvItem);

	CPakDirectory *pPakDirectory = (CPakDirectory *)(tvItem.lParam);

	if (pPakDirectory)
	{
		UINT iItemCount = 0;
		pPakDirectory->GetItemCount (&iItemCount);

		CProgressBar ctlProgress ("Exporting...", 70, iItemCount, false, 0);
		
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
		
		ctlProgress.Clear();
	}
}
