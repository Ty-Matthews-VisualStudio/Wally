// ExplorerTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "ExplorerTreeView.h"
#include "SortStringArray.h"
#include "BrowseDoc.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExplorerTreeView

IMPLEMENT_DYNCREATE(CExplorerTreeView, CTreeView)

CExplorerTreeView::CExplorerTreeView()
{
	m_szLogicalDriveStrings = NULL;
	m_iTimerTicks = 0;
	m_iTimerID = 0;
}

CExplorerTreeView::~CExplorerTreeView()
{
	if (m_szLogicalDriveStrings)
	{
		delete []m_szLogicalDriveStrings;
		m_szLogicalDriveStrings = NULL;
	}
}


BEGIN_MESSAGE_MAP(CExplorerTreeView, CTreeView)
	//{{AFX_MSG_MAP(CExplorerTreeView)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExplorerTreeView drawing

void CExplorerTreeView::OnDraw(CDC* pDC)
{
	CBrowseDoc* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CExplorerTreeView diagnostics

#ifdef _DEBUG
void CExplorerTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CExplorerTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CBrowseDoc* CExplorerTreeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBrowseDoc)));
	return (CBrowseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExplorerTreeView message handlers

BOOL CExplorerTreeView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASLINES;
	cs.style |= TVS_LINESATROOT;
	cs.style |= TVS_HASBUTTONS;	

	return CTreeView::PreCreateWindow(cs);
}

void CExplorerTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	LPITEMIDLIST idList;
	LPITEMIDLIST idNetworkList;
	char szBuffer[300];
	
	int iDesktopIcon;
	int iMyComputerIcon;
	int iNetworkIcon;
    int iReturn;

	GetTreeCtrl().SetImageList (NULL, 0);

	SHGetSpecialFolderLocation (this->m_hWnd, CSIDL_DESKTOP, &idList);
	SHGetPathFromIDList (idList, szBuffer);

	m_strDesktopPath = szBuffer;

	SHGetSpecialFolderLocation (this->m_hWnd, CSIDL_NETWORK, &idNetworkList);
	SHGetPathFromIDList (idNetworkList, szBuffer);

	m_strNetworkPath = szBuffer;

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
    
	iReturn = m_ImageList.Add (pNewBmp, COLORREF(0x00));

	// Desktop icon
	HICON hIcon = ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 34);
	iDesktopIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	// My computer icon
	hIcon = ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15);
	iMyComputerIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	// Nethood icon
	hIcon = ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 17);
	iNetworkIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	/*
	m_htMyComputerRoot = AddItem_Core("My Computer", TVI_ROOT, true, iMyComputerIcon, iMyComputerIcon);
	m_htNetworkRoot = AddItem_Core("Network Neighborhood", TVI_ROOT, true, iNetworkIcon, iNetworkIcon);
	*/
	m_htMyComputerRoot = GetTreeCtrl().InsertItem( "My Computer", iMyComputerIcon, iMyComputerIcon, TVI_ROOT);
	GetTreeCtrl().InsertItem("", m_htMyComputerRoot);
	m_htNetworkRoot = GetTreeCtrl().InsertItem( "Network Neighborhood", iNetworkIcon, iNetworkIcon, TVI_ROOT);
	GetTreeCtrl().InsertItem("", m_htNetworkRoot);
		
	/*
	m_htDesktopRoot = AddItem ( m_strDesktopPath, TVI_ROOT, true, iDesktopIcon, iDesktopIcon);
	*/
	CString strPathWildCard;    
    
	strPathWildCard = m_strDesktopPath + "\\*.*";

    CFileFind fFinder;
    BOOL bWorking = fFinder.FindFile(strPathWildCard);
    
	while (bWorking)
	{
        bWorking = fFinder.FindNextFile();
        if ( fFinder.IsDirectory() && !fFinder.IsDots() )
		{                
            HTREEITEM htItem = AddItem (m_strDesktopPath + "\\" + fFinder.GetFileName(), TVI_ROOT, true);
            //GetTreeCtrl().InsertItem("", htItem);
		}
	}	
}

void CExplorerTreeView::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM tvItem = pNMTreeView->itemNew;
	HTREEITEM htItemParent = GetTreeCtrl().GetParentItem (tvItem.hItem);

	if (tvItem.state & TVIS_EXPANDED)
	{
		ExpandBranch(tvItem.hItem, htItemParent);
	}
	else
	{
		//DeleteAllChild(tvItem.hItem);
	}
	
	*pResult = 0;
}

void CExplorerTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	m_tvSelectedItem = pNMTreeView->itemNew;

	if (!m_iTimerID)
	{
		m_iTimerID = SetTimer( TIMER_ETV_ID, 100, NULL);
	}
	else
	{
		m_iTimerTicks = 0;
	}
		
	*pResult = 0;
}

void CExplorerTreeView::OnTimer(UINT nIDEvent) 
{
	CTreeView::OnTimer(nIDEvent);

	if (nIDEvent == TIMER_ETV_ID)
	{
		m_iTimerTicks++;

		if (m_iTimerTicks > 10)
		{
			KillTimer( TIMER_ETV_ID );
			m_iTimerTicks = 0;
			m_iTimerID = 0;

			if (m_tvSelectedItem.hItem)
			{
				CBrowseDoc* pDoc = GetDocument();

				CString strPath("");
				strPath = GetPathFromHere (strPath, m_tvSelectedItem.hItem);

				if (strPath != "")
				{
					pDoc->SetPath(strPath);
				}
			}			
		}		
	}	
}

HTREEITEM CExplorerTreeView::AddItem_Core (LPCTSTR szName, HTREEITEM htParent, bool bIsDir, int iIcon, int iIconOpen)
{
	HTREEITEM htItem;

	htItem = GetTreeCtrl().InsertItem( szName, iIcon, iIconOpen, htParent);
	CString strTemp("");

	if (bIsDir)
	{
		strTemp = GetPathFromHere ("", htItem);
		
		if (HasChildDirectories (strTemp))
		{
			GetTreeCtrl().InsertItem("", htItem);
		}
		//GetTreeCtrl().InsertItem( "", htItem);
	}

	return htItem;
}

HTREEITEM CExplorerTreeView::AddItem(LPCTSTR szPath, HTREEITEM htParent, bool bIsDir /* = FALSE */, int iIconEx /* = -1 */, int iIconEx2 /* = -1 */)
{
	SHFILEINFO shInfo, shInfoSel;

	SHGetFileInfo( szPath, NULL, 
				   &shInfo, 
				   sizeof(shInfo), 
				   SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);

	SHGetFileInfo( szPath, NULL, 
				   &shInfoSel, 
				   sizeof(shInfoSel), 
				   SHGFI_DISPLAYNAME |  SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);

	int iIcon    = iIconEx  !=-1 ? iIconEx	: m_ImageList.Add(shInfo.hIcon);
    int iIconSel = iIconEx2 !=-1 ? iIconEx2 : m_ImageList.Add(shInfoSel.hIcon);

	GetTreeCtrl().SetImageList (&m_ImageList, LVSIL_NORMAL);

	return AddItem_Core ( shInfo.szDisplayName, htParent, bIsDir, iIcon, iIconSel);
}

void CExplorerTreeView::ExpandBranch (HTREEITEM htParent, HTREEITEM htGrandParent)
{
	int j = 0;
	int iMatch = 0;
	int iArraySize = 0;
	CString strCompare("");
	CString strCompare2("");
	CSortStringArray strArray;
	
	if (htParent == m_htMyComputerRoot)
	{
		int iBufferSize = GetLogicalDriveStrings(0, m_szLogicalDriveStrings);

		DeleteAllChild (htParent, FALSE);

		if (m_szLogicalDriveStrings)
		{
			delete []m_szLogicalDriveStrings;
			m_szLogicalDriveStrings = NULL;
		}
		m_szLogicalDriveStrings = new char[iBufferSize];
		memset (m_szLogicalDriveStrings, 0, iBufferSize);

		GetLogicalDriveStrings(iBufferSize, m_szLogicalDriveStrings);

		char *szTemp = m_szLogicalDriveStrings;
		
		while (*szTemp)
		{            
			AddItem (szTemp, m_htMyComputerRoot, TRUE);
			
			// The drive letters are separated by a NULL space
			szTemp += strlen(szTemp) + 1;
		}
		
		return;
	}

	if (htParent == m_htNetworkRoot)
	{
		// TODO:  How to handle Network paths et al??
		DeleteAllChild (htParent, FALSE);
		return;
	}
	
	// No grandparent means the parent is at the root... like A:\ or C:\  .
	if (!htGrandParent)
	{
		CString strFullPath ("");		
        strFullPath = GetPathFromHere (strFullPath, htParent);
		
		FindAllDirectories (strFullPath, (CStringArray*)(&strArray), WANT_FULLPATH);
	
		/*
		if( (hFile = _findfirst( strPathWildCard, &c_file )) != -1L )	
		{
			strCompare = c_file.name;

			if ((strCompare != ".") && (strCompare != ".."))
			{
				if (c_file.attrib & _A_SUBDIR)
				{
					strArray.Add (strFullPath + strCompare);
				}
			}
		
			while( _findnext( hFile, &c_file ) == 0 )
			{
				strCompare = c_file.name;
				
				if ((strCompare != ".") && (strCompare != ".."))
				{
					if (c_file.attrib & _A_SUBDIR)
					{						
						strArray.Add (strFullPath + strCompare);
					}
				}
			}
		}
		*/

		DeleteAllChild (htParent, FALSE);
		strArray.Sort();

		for (j = 0; j < strArray.GetSize(); j++)
		{
			HTREEITEM htItem = AddItem (strArray.GetAt(j), htParent, TRUE);
			//GetTreeCtrl().InsertItem("", htItem);
		}
		strArray.RemoveAll();
		return;
	}

	// If we got this far, then the item must be a directory somewhere on MyComputer.
	
    CString strFullPath;
	CString strPathWildCard;

    strFullPath = GetPathFromHere (strFullPath, htParent);
    //DeleteAllChild (htParent, FALSE);
    
	//strPathWildCard = strFullPath + "*.*";

    // Find all of the subdirectories, and add them to the CStringArray.
	// We do this first so that the loop is tighter.  The CStringArray
	// will be checked against what's already in the tree to see if we
	// want to add or remove items.

	strArray.RemoveAll();

	FindAllDirectories (strFullPath, &strArray, WANT_DIRECTORY);

	/*
	if( (hFile = _findfirst( strPathWildCard, &c_file )) != -1L )	
	{
		strCompare = c_file.name;

		if ((strCompare != ".") && (strCompare != ".."))
		{
			if (c_file.attrib & _A_SUBDIR)
			{
				strArray.Add (strCompare);
			}
		}
	
		while( _findnext( hFile, &c_file ) == 0 )
		{
			strCompare = c_file.name;
			
			if ((strCompare != ".") && (strCompare != ".."))
			{
				if (c_file.attrib & _A_SUBDIR)
				{
					strArray.Add (strCompare);
				}
			}
		}
	}
	*/
	
	// Find the first child of this directory
	HTREEITEM htChild;
	htChild = GetTreeCtrl().GetChildItem(htParent);
	CString strAddString("");

	strArray.Sort();
	
	while (htChild)
	{
		strCompare = GetTreeCtrl().GetItemText (htChild);
		strCompare.MakeUpper();

		for (j = 0, iMatch = -1; (j < strArray.GetSize()) && (iMatch == -1); j++)			
		{
			strCompare2 = strArray.GetAt(j);
			strCompare2.MakeUpper();

			if (strCompare == strCompare2)
			{
				iMatch = j;
			}
		}

		if (iMatch == -1)
		{
			// The directory no longer exists... go blow it away
			if (GetTreeCtrl().GetChildItem(htChild))
			{
				DeleteAllChild(GetTreeCtrl().GetChildItem(htChild), FALSE);
			}
			GetTreeCtrl().DeleteItem (htChild);
			htChild = GetTreeCtrl().GetChildItem(htParent);
		}
		else
		{
			// The directory is already in the tree, no need to add it again
			strArray.RemoveAt(iMatch);
			htChild = GetTreeCtrl().GetNextSiblingItem(htChild);
		}			
	}

	for (j = 0; j < strArray.GetSize(); j++)
	{
		strAddString = strFullPath + strArray.GetAt(j);
		HTREEITEM htItem = AddItem (strAddString, htParent, true);			
	}
	strArray.RemoveAll();	
}

BOOL CExplorerTreeView::HasChildDirectories (LPCTSTR szPath)
{
	
	CFileFind fFinder;
	BOOL bWorking = TRUE;
	CString strCompare("");
	CString strWildCard("");

	strWildCard = szPath;
	strWildCard.MakeUpper();

	// WinNT and Win95 return different strings for the directory paths.
	// NT lists it as being C:, 95 lists it as being C:\.  We'll strip
	// off the trailing \ if it happens to be there, and just always add one
	// for the wildcard *.*

	strWildCard = TrimSlashes(strWildCard);

	// If it's the A: or B: drive, just assume that there are subdirectories (IE, put the [+]
	// button there) so that we don't waste time spinning the floppy.
	if ((strWildCard == "A:") || (strWildCard == "B:"))
	{
		return TRUE;
	}

	// Chances are good that a subdirectory doesn't have a "." in its name, so look
	// first for those
	strWildCard += "\\*.";

	bWorking = fFinder.FindFile (strWildCard);

	while (bWorking)
	{
		bWorking = fFinder.FindNextFile();
		if ( !fFinder.IsDots() )
		{
			if (fFinder.IsDirectory())
			{
				return TRUE;
			}
		}
	}

	strWildCard += "\\*.*";

	bWorking = fFinder.FindFile (strWildCard);

	while (bWorking)
	{
		bWorking = fFinder.FindNextFile();
		if ( !fFinder.IsDots() )
		{
			if (fFinder.IsDirectory())
			{
				return TRUE;
			}
		}
	} 	

	return FALSE;
}

void CExplorerTreeView::DeleteAllChild (HTREEITEM htItem,  bool bIsEmpty /* = TRUE */)
{
	HTREEITEM htChild;

	htChild = GetTreeCtrl().GetChildItem(htItem);

	while (htChild)
	{
		int iImage1, iImage2;
		
        if (GetTreeCtrl().GetChildItem(htChild))
		{
            DeleteAllChild(GetTreeCtrl().GetChildItem(htChild), bIsEmpty);
		}

        GetTreeCtrl().GetItemImage (htChild, iImage1, iImage2);

        if (iImage2 != 0 && iImage2 != iImage1) 
		{
            m_ImageList.Remove(iImage2);
            
            RefreshTreeImages(m_htMyComputerRoot, iImage2);
			RefreshTreeImages(m_htNetworkRoot, iImage2);
            //RefreshTreeImages(m_htDesktopRoot, iImage2);
		}

		if (iImage1 != 0)
		{
            m_ImageList.Remove(iImage1);

            RefreshTreeImages(m_htMyComputerRoot, iImage1);
			RefreshTreeImages(m_htNetworkRoot, iImage1);
            //RefreshTreeImages(m_htDesktopRoot, iImage1);
		}
		
        GetTreeCtrl().DeleteItem (htChild);

        GetTreeCtrl().SetImageList(&m_ImageList, TVSIL_NORMAL);

		htChild = GetTreeCtrl().GetChildItem(htItem);
	}

	if (bIsEmpty)
	{
		GetTreeCtrl().InsertItem( "", htItem);
	}
}

void CExplorerTreeView::RefreshTreeImages(HTREEITEM htItem, int iImage)
{
    int iStandard, iOpen;
    HTREEITEM htCurrent;
       
    htCurrent = GetTreeCtrl().GetChildItem(htItem);
    
    while (htCurrent)
	{
        if (GetTreeCtrl().GetItemImage(htCurrent, iStandard, iOpen))
		{
            if (iStandard > iImage)
			{
				iStandard--;
			}
            if (iOpen > iImage)
			{
				iOpen--;
			}

            GetTreeCtrl().SetItemImage(htCurrent, iStandard, iOpen);
		}

        if (GetTreeCtrl().ItemHasChildren( htCurrent) != 0)
		{
            RefreshTreeImages(htCurrent, iImage);
		}

        htCurrent = GetTreeCtrl().GetNextSiblingItem(htCurrent);
	}
}

CString CExplorerTreeView::GetPathFromHere (CString strPath, HTREEITEM htItem)
{
	HTREEITEM htParent = GetTreeCtrl().GetParentItem(htItem);
    HTREEITEM htChild = GetTreeCtrl().GetChildItem(htItem);

    if (htItem == m_htMyComputerRoot)
	{
		return "";
	}

	if (!htParent)
	{
		if (GetTreeCtrl().GetItemText(htItem) != CString(""))
		{
            strPath = GetTreeCtrl().GetItemText(htItem) + '\\' + strPath;
		}
		strPath = m_strDesktopPath + '\\' + strPath;
		return strPath;
	}
        
	if (htParent == m_htMyComputerRoot)
	{
		HTREEITEM htThisChild;

		char *szDrive = m_szLogicalDriveStrings;
		
		htThisChild = GetTreeCtrl().GetChildItem(htParent);
		
		while (htThisChild)
		{
			if (htThisChild == htItem)
			{
				strPath = CString(szDrive) + strPath;
				return strPath;
			}
			szDrive += strlen(szDrive) + 1;
			htThisChild=GetTreeCtrl().GetNextItem(htThisChild, TVGN_NEXT);
		}
	}
	else if (htItem == m_htDesktopRoot)
	{
		strPath = m_strDesktopPath + '\\' + strPath;
		return strPath;
	}
    else
    {
        if (GetTreeCtrl().GetItemText(htItem) != CString(""))
		{
            strPath = GetTreeCtrl().GetItemText(htItem) + '\\' + strPath;
		}

        strPath = GetPathFromHere(strPath, htParent);
    }

    // remove the last '\' if it isn't a directory
    if (!htChild && strPath.GetLength() > 0)
	{
        strPath = strPath.Left (strPath.GetLength() - 1);
	}

	return strPath;
}

void CExplorerTreeView::FindAllDirectories (LPCTSTR szPath, CStringArray *pStrArray, int iFlags)
{
	struct _finddata_t c_file;
	long hFile;
	CString strCompare("");
	
	// Trim off the trailing \ marks
	CString strFullPath (szPath);
	CString strPathWildCard("");

	if (strFullPath != "")
	{
		int iPos = strFullPath.GetLength() - 1;

		while (strFullPath.GetAt(iPos) == '\\')
		{
			iPos--;
		}

		iPos++;
		strFullPath = strFullPath.Left(iPos);
		strFullPath += "\\";
	}
	strPathWildCard = strFullPath + "*.*";

	if( (hFile = _findfirst( strPathWildCard, &c_file )) != -1L )	
	{
		strCompare = c_file.name;

		if ((strCompare != ".") && (strCompare != ".."))
		{
			if (c_file.attrib & _A_SUBDIR)
			{
				if (iFlags & WANT_FULLPATH)
				{
					pStrArray->Add (strFullPath + strCompare);
				}

				if (iFlags & WANT_DIRECTORY)
				{
					pStrArray->Add (strCompare);
				}
			}
		}
	
		while( _findnext( hFile, &c_file ) == 0 )
		{
			strCompare = c_file.name;
			
			if ((strCompare != ".") && (strCompare != ".."))
			{
				if (c_file.attrib & _A_SUBDIR)
				{						
					if (iFlags & WANT_FULLPATH)
					{
						pStrArray->Add (strFullPath + strCompare);
					}

					if (iFlags & WANT_DIRECTORY)
					{
						pStrArray->Add (strCompare);
					}
				}
			}
		}
	}
}

void CExplorerTreeView::HighlightDirectory (LPCTSTR szPath)
{
	// First, make sure that directory even exists
	
	FILE *rp = NULL;
	CString strPath("");
	CString strRollingPath("");
	strRollingPath = TrimSlashes (szPath);
	
	if (strRollingPath == "")
	{
		return;
	}

	strRollingPath += "\\";
	strRollingPath.MakeLower();

	strPath.Format ("%s\\NUL", TrimSlashes (szPath));

	errno_t err = fopen_s(&rp, strPath, "r");	
	if (err != 0)
	{		
		return;
	}
	fclose (rp);

	CTreeCtrl &thisTree = GetTreeCtrl();

	HTREEITEM htItem = thisTree.GetRootItem();
	if (htItem == m_htMyComputerRoot)
	{
		thisTree.Expand (htItem, TVE_EXPAND);
		htItem = thisTree.GetChildItem (htItem);
	}

	BOOL bDone = FALSE;
	CString strDirectory ("");
	CString strNextDirectory ("");
	CString strItemText("");
	
	strDirectory = GrabNextDirectory (&strRollingPath);
	
	while ((!bDone) && (htItem))
	{
		strItemText = "";
		strItemText = TrimSlashes (GetPathFromHere (strItemText, htItem));
		strItemText.MakeLower();
		
		if (strItemText== strDirectory)
		{
			strNextDirectory = GrabNextDirectory (&strRollingPath);			

			if (strNextDirectory != "")			
			{
				strDirectory += "\\";
				strDirectory += strNextDirectory;
				strDirectory.MakeLower();

				thisTree.Expand (htItem, TVE_EXPAND);
				htItem = thisTree.GetChildItem (htItem);
			}
			else // strDirectory == ""
			{
				// We've found the last directory in the desired path
				thisTree.SelectItem (htItem);
				bDone = TRUE;
			}
		}
		else
		{		
			htItem = thisTree.GetNextSiblingItem (htItem);
		}		
	}
}

CString CExplorerTreeView::GrabNextDirectory (CString *szPath)
{
	int iPosition = 0;
	CString strSource (*szPath);
	int iLength = strSource.GetLength();
	BOOL bDone = FALSE;
	int iFinalPosition = 0;

	for (iPosition = 0; (iPosition < iLength) && !bDone; iPosition++)
	{
		if (strSource.GetAt(iPosition) == '\\')
		{
			iFinalPosition = iPosition;
			bDone = TRUE;			
		}	
	}

	strSource = strSource.Left (iFinalPosition);
	(*szPath) = (*szPath).Right (iLength - iFinalPosition - 1);

	return strSource;
}



BOOL CExplorerTreeView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CTreeView::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}
