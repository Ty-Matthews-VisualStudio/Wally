#ifndef _PAKLISTVIEW_H_
#define _PAKLISTVIEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PakListView.h : header file
//

#include "ImageIconList.h"

class CPakDoc;

#define PAK_UPDATE_FLAG_ONLYONE			0x1
#define PAK_UPDATE_FLAG_ATLEASTONE		0x2

extern UINT WM_PAKLISTVIEW_CUSTOM;

//class CPakListView;

/////////////////////////////////////////////////////////////////////////////
// CPakListView view

class CPakListView : public CListView, public CDelayRenderView
{
protected:
	CPakListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPakListView)

// Attributes
private:
	CListCtrl m_ListCtrl;
	int m_iItemSelected;
	int m_iSortOrder;
	CImageIconList m_ImageList;
	BOOL m_bImageListInitialized;	
	int m_iClosedFolderIcon;

	COleDropTarget m_oleDropTarget;	
	CString m_strError;

// Operations
public:
	CPakDoc* GetDocument();
	void UpdateList();
	void DeleteSelected();
	void OpenSelected();
	void InitializeImageList();	
	void UpdateWhenSelected (CCmdUI* pCmdUI, int iFlags = 0);
	void DoDragDrop(MSG *pMsg);
	void AddPasteItems();

	int GetSelectedCount();
	UINT GetSelectedCountPlusChildren();
	void SetErrorString (LPCTSTR szError)
	{
		m_strError = szError;
	}
	CString GetErrorString ()
	{
		return m_strError;
	}
	void UpdateStatusBar();
	void ImportFiles();
	void ExportFiles();
	void ExportSelected(LPCTSTR szDirectory);

// Virtual functions from parent derivative CDelayRenderView:
	HGLOBAL RenderData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakListView)
	public:
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPakListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPakListView)
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPakPopupRename();
	afx_msg void OnUpdatePakPopupRename(CCmdUI* pCmdUI);
	afx_msg void OnPakPopupOpen();
	afx_msg void OnUpdatePakPopupOpen(CCmdUI* pCmdUI);
	afx_msg void OnPakPopupDelete();
	afx_msg void OnUpdatePakPopupDelete(CCmdUI* pCmdUI);
	afx_msg void OnPakPopupCopy();
	afx_msg void OnUpdatePakPopupCopy(CCmdUI* pCmdUI);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPakPopupPaste();
	afx_msg void OnUpdatePakPopupPaste(CCmdUI* pCmdUI);
	afx_msg void OnPakPopupNewFolder();
	afx_msg void OnEditClear();
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPakPlaysound();
	afx_msg void OnUpdatePakPlaysound(CCmdUI* pCmdUI);
	afx_msg void OnPakPopupNewTxtfile();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPackageOpen();
	afx_msg void OnUpdatePackageOpen(CCmdUI* pCmdUI);
	afx_msg void OnPackageExport();
	afx_msg void OnPakPopupExport();
	afx_msg void OnPakPopupImport();
	afx_msg void OnPackageAdd();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

int CALLBACK PakListViewCompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

#ifndef _DEBUG  
inline CPakDoc* CPakListView::GetDocument()
   { return (CPakDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKLISTVIEW_H_
