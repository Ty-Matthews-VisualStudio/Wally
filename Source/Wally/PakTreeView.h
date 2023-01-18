#ifndef _PAKTREEVIEW_H_
#define _PAKTREEVIEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPakTreeView view

class CPakDoc;

class CPakTreeView : public CTreeView
{
protected:
	CPakTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPakTreeView)

// Attributes
private:
	CImageList m_ImageList;
	int m_iClosedFolderIcon;
	int m_iOpenFolderIcon;
	int m_iBlankIcon;
	BOOL m_bImageListInitialized;
	CString m_strCurrentDirectory;
	BOOL m_bClickInitiated;
	
// Operations
public:
	CPakDoc *GetDocument();
	void BuildTree(BOOL bRetainPosition = FALSE);
	void InitializeImageList();
	void HighlightDirectory (LPCTSTR szPath, BOOL bExpand = FALSE);
	CString GrabNextDirectory (CString *szPath);
	void UpdateStatusBar();
	void ImportFiles();
	void ExportFiles();
	void ExportSelected(LPCTSTR szDirectory);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPakTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPakTreeView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPakPopupRename();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdatePakPlaysound(CCmdUI* pCmdUI);
	afx_msg void OnPakPlaysound();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPackageAdd();
	afx_msg void OnPackageExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CPakDoc *CPakTreeView::GetDocument()
   { return (CPakDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKTREEVIEW_H_
