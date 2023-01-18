#ifndef _BROWSEVIEW_H__
#define _BROWSEVIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Browse2View.h : header file
//

#ifndef _DIBSECTION_H__
	#include "DibSection.h"
#endif

#pragma pack (1)
typedef struct
{
	int iFileNameLength;
	char szFileName;
} BROWSE_CLIPBOARD_S, *LPBROWSE_CLIPBOARD_S;
#pragma pack()      // Revert to previous packing

typedef struct
{
	int iNumSelections;
	int iFirstEntryOffset;
} BC_HEADER_S, *LPBC_HEADERS_S;

#define	BROWSE_CLIPBOARD_SIZE	sizeof (BROWSE_CLIPBOARD_S)
#define BC_HEADER_SIZE			sizeof (BC_HEADER_S)

class CBrowserCacheItem;


#define BROWSE_IMAGE_WIDTH		96
#define BROWSE_IMAGE_HEIGHT		96
#define BROWSE_BUTTON_WIDTH		(BROWSE_IMAGE_WIDTH + 10)
#define BROWSE_BUTTON_HEIGHT	(BROWSE_IMAGE_HEIGHT + 14 + 16)

#define MB_LEFT_BUTTON			0
#define MB_RIGHT_BUTTON			1

/////////////////////////////////////////////////////////////////////////////
// CBrowseView view

class CBrowseView : public CView
{
protected:
	CBrowseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrowseView)

// Private members
private:
	int m_iHorzButtons;
	int m_iVertButtons;
	int m_iNumImageRows;
	CSize m_csDocSize;
	BOOL m_bInitialized;
	BOOL m_bCalcSizes;
	BOOL m_bSized;
	BOOL m_bScrolling;
	BOOL m_bResetScrollbars;

	CDibSection m_dsWadThumbnail;
	
	int m_iRButtonItem;
	BOOL m_bRButtonDown;
	
	CPoint m_ptLButtonDown;
	int m_iLButtonItem;
	BOOL m_bLButtonDown;
	BOOL m_bDragging;
	int m_iCurrentScrollPos;
	int m_iInvalidateScrollRange;
	
	CFont m_ftText;
	COleDropTarget m_oleDropTarget;
	CStringArray m_saSelectedItems;

// Operations
public:
	CBrowseDoc* GetDocument();
	void ResetScrollbars (BOOL bReset = TRUE)
	{
		m_bResetScrollbars = bReset;
	}

private:
	void CalcMaxButtons();
	void DrawItem (CBrowserCacheItem *pItem, CDC *pMemDC, int iRow, int iColumn);
	int GetItemNumber (CPoint pt);
	CBrowserCacheItem *GetCurrentCacheItem(int iMouseButton);
	CString GetCurrentDirectory();
	BOOL PasteFile(LPCTSTR szFileName, BOOL bPrompt);
	int GetSelectedList ();
	void DeleteSelectedItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseView)
	public:
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBrowseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBrowseView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBvpopupCopy();
	afx_msg void OnBvpopupPaste();
	afx_msg void OnUpdateBvpopupPaste(CCmdUI* pCmdUI);
	afx_msg void OnBvpopupCopyClipboard();
	afx_msg void OnBvpopupDelete();
	afx_msg void OnBvpopupInfo();
	afx_msg void OnUpdateBvpopupCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBvpopupCopyClipboard(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBvpopupDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBvpopupInfo(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnEditClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CBrowseDoc* CBrowseView::GetDocument()
   { return (CBrowseDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _BROWSEVIEW_H__
