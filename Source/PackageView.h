#ifndef _PACKAGEVIEW_H_
#define _PACKAGEVIEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageView.h : header file
//

#include "PackagePreviewWnd.h"
#include "PackageListBox.h"
#include "PackageTreeControl.h"

/*
#define PFORMVIEW_LB_OFFSET_LEFT		10
#define PFORMVIEW_LB_OFFSET_RIGHT		10
#define PFORMVIEW_LB_OFFSET_TOP			28
#define PFORMVIEW_LB_OFFSET_BOTTOM		58
*/

#define PFORMVIEW_LB_OFFSET_LEFT		10
#define PFORMVIEW_LB_OFFSET_RIGHT		10
#define PFORMVIEW_LB_OFFSET_TOP			28
#define PFORMVIEW_LB_OFFSET_BOTTOM		100

#define PFORMVIEW_ED_OFFSET_LEFT		60

/////////////////////////////////////////////////////////////////////////////
// CPackageView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//#ifndef __WALLYDOC_H__
//	#include "WallyDoc.h"
//#endif
#ifndef _WALLYVIEW_H_
	#include "WallyView.h"
#endif

#define	PV_FLAG_REMIP		0x1
#define	PV_FLAG_UPDATE		0x2
#define PV_FLAG_REPLACE		0x4

#define PV_DRAG_UNKNOWN		0x0
#define PV_DRAG_MOVE		0x1
#define PV_DRAG_COPY		0x2
#define PV_DRAG_CANCEL		0x3

#include "PackageDoc.h"

class CProgressBar;
class CPackageReMipThread;
class CPackageBrowseView;

class CPackageView : public CFormView
{
protected:
	CPackageView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackageView)

	/*inline CPackageDoc* GetDocument()
	{
		return (CPackageDoc *)m_pDocument;
	}*/
	
	CPackageDoc* GetDocument()
	{
		return (CPackageDoc *)m_pDocument;
	}


// Form Data
public:
	//{{AFX_DATA(CPackageView)
	enum { IDD = IDD_WADPAK_VIEW_DLG };
	CSliderCtrl	m_slThumbnailSize;
	CStatic	m_stThumbnailSize;
	CPackageTreeControl	m_tcImages;
	CButton	m_btnAnimate;
	CButton	m_btnRandom;
	CButton	m_btnBrowseMode;
	CButton	m_btnTileMode;
	CButton	m_btnSelectAll;
	CButton	m_ckFilter;
	CEdit	m_edFilter;
	CPackageListBox	m_lbImages;
	CString	m_strFilterLimit;
	int		m_iThumbnailSize;
	//}}AFX_DATA

// Members
private:
	bool m_bFirstTime;
	CPackagePreviewWnd m_wndPreview;
	int m_iMipNumber;
	CString m_strStatusText;
	bool m_bItemIsMip;
	int m_iDragType;
	
	CBrush m_brDlgColor;
	int m_iOldRCWidth;
	int m_iOldRCHeight;

	CBitmap m_bmRandom;
	CBitmap m_bmAnimate;
	CBitmap m_bmTileMode;
	CBitmap m_bmBrowseMode;
	
	// Local RAM buffer
	unsigned char *m_pbyOutputData;
	int m_iOutputDataSize;

	int m_iLastWADType;
	CImageList  m_imgList;

	int m_iSelectionTimerID;
	int m_iSelectionTimerTicks;
	int m_iFilterTimerID;
	int m_iFilterTimerTicks;
	int m_iListBoxTimerID;
	int m_iListBoxTimerTicks;
	int m_iTreeCtrlTimerID;
	int m_iTreeCtrlTimerTicks;
	CString m_strFilter;
	bool m_bFilter;

	COleDropTarget m_oleDropTarget;
	CPackageView *m_pDragSourcePackage;
	unsigned char *m_pbyClipboardBuffer;

	CPackageReMipThread *m_pReMipThread;
	int m_iReMipFlag;
	CProgressBar *m_pReMipProgressBar;

	CPackageBrowseView *m_pBrowseView;


// Operations
public:
	void Update();
	void DoBrowsingPaste();
	void DoPaste (bool bReplaceCurrent = false);
	void AddString (CWADItem *pItem, LPCTSTR szName, bool bSetSelection = false);
	bool ExportImage (CWADItem *pItem, LPCTSTR szFileName, CString *pstrErrorMessage);
	void ImportImage (LPCTSTR szPath, BOOL bForDecal = FALSE);
	bool AddImage (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight, UINT iFlags, CProgressBar *pProgress = NULL);
	void RemoveImage (int iIndex);
	
	void CheckForMipsUpdate (CCmdUI* pCmdUI);

	void ReMipAll();
	void RemipSelected();
	void UpdateAllOpenImages(LPCTSTR szPath);
	void UpdateImageData (CWallyDoc *pWallyDoc);
	void RenameImage (CWallyDoc *pWallyDoc);
	void BreakDocConnection (CWallyDoc *pWallyDoc);
	CString GetName();
	void DoDragDrop(MSG *pMsg);
	BOOL DoDragDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	void FinishDragDrop();
	void DoPackageExport (LPCTSTR szDirectory = NULL);
	
	void EditDelete( int iSelection = -1, bool bPrompt = true);

	void FilterList();
	CPackageListBox	*GetListBox()
	{
		return &m_lbImages;
	}
	CPackageTreeControl *GetTreeCtrl()
	{
		return &m_tcImages;
	}

	int GetReMipFlag ()
	{
		return m_iReMipFlag;
	}
	void StopThread();
	
	bool InitReMipProgress(LPCTSTR szText, int iPercent, int iMaxItems, bool bSmooth, int iPane);
	void StepReMipProgress ();
	void ClearReMipProgress ();
	void OpenSelectedImages(BOOL bCaretOnly = FALSE);
	void ImportImages();
	void ShowRightButtonPopup(CPoint ptPos);
	void CreateNewImage();
	void RenameImage();
	void UpdateStatusBar();
	void ResetListBox();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPackageView)
	afx_msg void OnPackageListBoxCustomMessage(UINT nType, UINT nFlags);
	afx_msg void OnPackageTreeControlCustomMessage( UINT nType, UINT nFlags );
	afx_msg void OnSelchangeListImages();
	afx_msg void OnPaint();
	afx_msg BOOL OnToolTipNotify ( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEditCopy();
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI* pCmdUI);
	afx_msg void OnDblclkListImages();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPackageOpen();
	afx_msg void OnPackageAdd();
	afx_msg void OnEditPastePackage();
	afx_msg void OnUpdateEditPastePackage(CCmdUI* pCmdUI);	
	afx_msg void OnEditPasteOver();
	afx_msg void OnUpdateEditPasteOver(CCmdUI* pCmdUI);
	afx_msg void OnEditZoomin();
	afx_msg void OnEditZoomout();
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnFileBatchConversion();
	afx_msg void OnUpdatePackageOpen(CCmdUI* pCmdUI);
	afx_msg void OnPackageExport();
	afx_msg void OnUpdatePackageExport(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCheckFilter();
	afx_msg void OnDoubleclickedCheckFilter();
	afx_msg void OnUpdateEditFilter();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonSelect();
	afx_msg void OnPackageDragCancel();
	afx_msg void OnPackageDragCopy();
	afx_msg void OnPackageDragMove();
	afx_msg void OnUseAsRivetSource();
	afx_msg void OnUpdateUseAsRivetSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftBulletSource();
	afx_msg void OnUpdateUseAsLeftBulletSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightBulletSource();
	afx_msg void OnUpdateUseAsRightBulletSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftPatternSource();
	afx_msg void OnUpdateUseAsLeftPatternSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightPatternSource();
	afx_msg void OnUpdateUseAsRightPatternSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsLeftDecalSource();
	afx_msg void OnUpdateUseAsLeftDecalSource(CCmdUI* pCmdUI);
	afx_msg void OnUseAsRightDecalSource();
	afx_msg void OnUpdateUseAsRightDecalSource(CCmdUI* pCmdUI);
	afx_msg void OnPackageRemipAll();
	afx_msg void OnPackageRemipSelected();
	afx_msg void OnPackageNewImage();
	afx_msg void OnUpdateTpSelection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTpZoom(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPushAnimate();
	afx_msg void OnPushBrowseMode();
	afx_msg void OnPushRandom();
	afx_msg void OnPushTileMode();
	afx_msg void OnTpZoom();
	afx_msg void OnTpSelection();
	afx_msg void OnPackageResize();
	afx_msg void OnUpdatePackageResize(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePackageRemipSelected(CCmdUI* pCmdUI);
	afx_msg void OnSelchangedTreeImages(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderThumbnailSize(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PACKAGEVIEW_H_
