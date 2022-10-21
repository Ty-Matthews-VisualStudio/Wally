#ifndef _PACKAGEBROWSEVIEW_H_
#define _PACKAGEBROWSEVIEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CPackageBrowseView view


#define PACKAGE_BROWSE_IMAGE_WIDTH		256 // 96
#define PACKAGE_BROWSE_IMAGE_HEIGHT		256
#define PACKAGE_BROWSE_BUTTON_WIDTH		(PACKAGE_BROWSE_IMAGE_WIDTH + 10)
#define PACKAGE_BROWSE_BUTTON_HEIGHT	(PACKAGE_BROWSE_IMAGE_HEIGHT + 14 + 16)

#define MB_LEFT_BUTTON			0
#define MB_RIGHT_BUTTON			1

#define PACKAGE_BROWSE_VIEWMODE_BROWSE		0
#define PACKAGE_BROWSE_VIEWMODE_TILE		1

#define PB_TIMER_ANIMATE_LIQUID				150
#define PB_TIMER_ANIMATE_NORMAL				250

extern int WM_PACKAGE_BROWSE_CUSTOM;

class CWADItem;
class CPackageListBox;
class CPackageTreeControl;
class CPackageView;
class CDibSection;

typedef struct
{
	CDibSection *pdsImage;
	CWADItem *pWADItem;
} PBDibStruct, *LP_PBDibStruct;

class CPackageBrowseView : public CView
{
protected:
	CPackageBrowseView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackageBrowseView)

// Members
private:
	COleDropTarget m_oleDropTarget;
	int m_iHorzButtons;
	int m_iVertButtons;
	int m_iNumImageRows;
	int m_iFocusItem;
	int m_iViewMode;

	// This has to be 10; more than 10 would screw up the routine for finding other random tiles
	#define PACKAGE_BROWSE_NUM_RANDOM_DIBS		10

	// Neal - allow 16 liquid texture animations (looks much better)
	#define PACKAGE_BROWSE_NUM_ANIMATION_DIBS	64

	PBDibStruct m_pdsTiledDIBs[PACKAGE_BROWSE_NUM_ANIMATION_DIBS];
		
	int m_iNumDIBTiles;
	int m_iAnimateTimer;
	int m_iAnimateTimerValue;
	int m_iCurrentAnimation;
	int m_iBrowseImageSize;

	CWADItem *m_pPreviousTiledItem;
	int m_iPreviousTiledItem;
	int m_iZoomLevel;
	BOOL m_bZoomMode;
	BOOL m_bRandomMode;
	BOOL m_bAllowRandomMode;
	BOOL m_bAnimateMode;
	BOOL m_bAllowAnimateMode;
	BOOL m_bForceDIBRebuild;

	CFont m_ftText;
	CFont m_ftTextSmall;
	CDC m_dcBackroundDraw;
	CBitmap m_bmBackgroundDraw;
	CBrush m_brBackroundErase;
	int m_iPreviousClientWidth;
	int m_iPreviousClientHeight;
	BOOL m_bBackgroundDCInitialized;
	
	CSize m_csDocSize;
	BOOL m_bInitialized;
	BOOL m_bCalcSizes;
	BOOL m_bSized;
	BOOL m_bScrolling;
	BOOL m_bResetScrollbars;
	int m_iCurrentScrollPos;
	int m_iInvalidateScrollRange;

	int m_iRButtonItem;
	BOOL m_bRButtonDown;
	CPoint m_ptRButtonDown;
	
	CPoint m_ptLButtonDown;
	int m_iLButtonItem;
	BOOL m_bLButtonDown;
	BOOL m_bDragging;

// Operations
public:
	CPackageDoc* GetDocument();
	void ResetScrollbars (BOOL bReset = TRUE)
	{
		m_bResetScrollbars = bReset;
	}
	void SetViewMode (int iViewMode)
	{
		m_iViewMode = min (iViewMode, PACKAGE_BROWSE_VIEWMODE_TILE);
		m_iViewMode = max (m_iViewMode, PACKAGE_BROWSE_VIEWMODE_BROWSE);
	}
	int GetViewMode()
	{
		return m_iViewMode;
	}
	int GetZoomLevel()
	{
		return m_iZoomLevel;
	}
	void SetZoomMode (BOOL bZoom = TRUE);
	BOOL IsInZoomMode();
	void SetAllowRandomMode (BOOL bAllow = TRUE)
	{
		m_bAllowRandomMode = bAllow;
	}
	void SetAllowAnimateMode (BOOL bAllow = TRUE)
	{
		m_bAllowAnimateMode = bAllow;
	}
	void ForceDIBRebuild (BOOL bForce = TRUE)
	{
		m_bForceDIBRebuild = bForce;
	}
	void SetBrowseImageSize( int iSize )
	{
		m_iBrowseImageSize = iSize;
	}

private:
	void CalcMaxButtons();	
	void DrawItem (CWADItem *pItem, BOOL bIsSelected, BOOL bIsFocus, CDC *pMemDC, int iRow, int iColumn);
	int GetItemNumber (CPoint pt);
	CWADItem *GetWADItem (CPoint pt, int iButton);
	void CheckForMipsUpdate (CCmdUI* pCmdUI);
	CPackageView *GetPackageView();
	CPackageListBox *GetPackageListBox();
	CPackageTreeControl *GetPackageTreeCtrl();
	
	void DrawBrowseMode(CDC* pDC);
	void DrawTileMode(CDC* pDC);
	void DrawRandomTileMode(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageBrowseView)
	public:
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackageBrowseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPackageBrowseView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPackageExport();
	afx_msg void OnUpdatePackageExport(CCmdUI* pCmdUI);
	afx_msg void OnPackageNewImage();
	afx_msg void OnPackageOpen();
	afx_msg void OnUpdatePackageOpen(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftBulletSource();
	afx_msg void OnUpdateUseAsLeftBulletSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftDecalSource();
	afx_msg void OnUpdateUseAsLeftDecalSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftPatternSource();
	afx_msg void OnUpdateUseAsLeftPatternSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightBulletSource();
	afx_msg void OnUpdateUseAsRightBulletSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightDecalSource();
	afx_msg void OnUpdateUseAsRightDecalSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightPatternSource();
	afx_msg void OnUpdateUseAsRightPatternSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRivetSource();
	afx_msg void OnUpdateUseAsRivetSource(CCmdUI* pCmdUI);
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteOver();
	afx_msg void OnUpdateEditPasteOver(CCmdUI* pCmdUI);
	afx_msg void OnEditPastePackage();
	afx_msg void OnUpdateEditPastePackage(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditClear();
	afx_msg void OnTpZoom();
	afx_msg void OnUpdateTpZoom(CCmdUI* pCmdUI);
	afx_msg void OnTpSelection();
	afx_msg void OnUpdateTpSelection(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPackageAdd();
	afx_msg void OnUpdatePackageAdd(CCmdUI* pCmdUI);
	afx_msg void OnPackageRemipAll();
	afx_msg void OnPackageRemipSelected();
	afx_msg void OnUpdatePackageRemipSelected(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  
inline CPackageDoc* CPackageBrowseView::GetDocument()
   { return (CPackageDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PACKAGEBROWSEVIEW_H_
