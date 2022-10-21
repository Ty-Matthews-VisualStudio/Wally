#ifndef _EXPLORERTREEVIEW_H__
#define _EXPLORERTREEVIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExplorerTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExplorerTreeView view

#define WANT_FULLPATH	0x1
#define WANT_DIRECTORY	0x2

class CExplorerTreeView : public CTreeView
{
protected:
	CExplorerTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExplorerTreeView)

// Private members
private:
	char *m_szLogicalDriveStrings;
	CString m_strDesktopPath;
	CString m_strNetworkPath;
	CImageList m_ImageList;

	HTREEITEM m_htMyComputerRoot;	
	HTREEITEM m_htNetworkRoot;
	HTREEITEM m_htDesktopRoot;
	TV_ITEM m_tvSelectedItem;

	int m_iTimerID;
	int m_iTimerTicks;

// Operations
private:
	HTREEITEM AddItem_Core (LPCTSTR szName, HTREEITEM htParent, bool bIsDir, int iIcon, int iIconOpen);
	HTREEITEM AddItem(LPCTSTR szPath, HTREEITEM htParent, bool bIsDir = FALSE, int iIconEx = -1, int iIconEx2 = -1);
	void ExpandBranch (HTREEITEM htParent, HTREEITEM htGrandParent);
	BOOL HasChildDirectories (LPCTSTR szPath);
	void DeleteAllChild (HTREEITEM htItem,  bool bIsEmpty = TRUE);
	void RefreshTreeImages(HTREEITEM htItem, int iImage);
	CString GetPathFromHere (CString strPath, HTREEITEM htItem);
	void FindAllDirectories (LPCTSTR szPath, CStringArray *pStrArray, int iFlags);
	
public:
	CBrowseDoc* GetDocument();
	void HighlightDirectory (LPCTSTR szPath);
	CString GrabNextDirectory (CString *szPath);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExplorerTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExplorerTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CExplorerTreeView)
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


// NOTE:  If this class is to be used elsewhere, either remove this
// function or change it appropriately.
#ifndef _DEBUG  
inline CBrowseDoc* CExplorerTreeView::GetDocument()
   { return (CBrowseDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _EXPLORERTREEVIEW_H__
