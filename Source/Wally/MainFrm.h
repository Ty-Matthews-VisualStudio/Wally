// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
// 

#if !defined(AFX_MDICLIENT_H__B824C982_B3B9_11D1_9297_00A024DF24C3__INCLUDED_)
#include "MdiClient.h"
#endif

// MFC multi-threaded extensions
#include <afxmt.h>

#ifndef _WALLYTOOLBAR_H_
	#include "WallyToolBar.h"
#endif

#ifndef _MAINFRM_H_
#define _MAINFRM_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ColorPalette.h"
#include "DecalToolbar.h"
#include "ToolSettingsToolbar.h"

class CWallyApp;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNCREATE(CMainFrame)
	//DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CWallyApp* m_pWallyApp;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	//CStatusBar	m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolBar		m_wndImageEditToolBar;
	//CWallyToolBar	m_wndImageEditToolBar;
	CToolBar		m_wndDrawModesToolBar;
	
	void ApplyAmount( int iAmount);
	void OnUpdateApply( CCmdUI* pCmdUI, int iTestAmount);

	CMutex* m_pMutex;
	
// Public methods
public:
	bool RestoreWindowState( void);
	void SaveWindowState( void);
	bool m_bProgressCreated;
	void PostSingleInstance(LPCTSTR szCommandLine);

	CWndMdiClient *GetMdiClient()
	{
		return &m_WndMdiClient;
	}

	CWndMdiClient m_WndMdiClient;
	CColorPalette m_wndPaletteToolBar;
	
	// Ty- use the new ToolSettings toolbar instead of Decal
	//CDecalToolbar m_wndDecalToolbar;
	CToolSettingsToolbar m_wndToolSettingsToolbar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnApplyLeast();
	afx_msg void OnApplyALittleBit();
	afx_msg void OnApplyMedium();
	afx_msg void OnApplyMore();
	afx_msg void OnApplyMost();
	afx_msg void OnUpdateApplyLeast(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyALittleBit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyMedium(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyMore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyMost(CCmdUI* pCmdUI);
	afx_msg void OnShapeSquare();
	afx_msg void OnShapeDiamond();
	afx_msg void OnShapeCircular();
	afx_msg void OnUpdateShapeSquare(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShapeDiamond(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShapeCircular(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnShowSubMips();
	afx_msg void OnUpdateShowSubMips(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnViewImageEditBar();
	afx_msg void OnUpdateImageEditBar(CCmdUI* pCmdUI);
	afx_msg void OnViewModesBar();
	afx_msg void OnUpdateModesBar(CCmdUI* pCmdUI);
	afx_msg void OnViewFileBar();
	afx_msg void OnUpdateFileBar(CCmdUI* pCmdUI);
	afx_msg void OnViewPaletteBar();
	afx_msg void OnUpdatePaletteBar(CCmdUI* pCmdUI);
	afx_msg void OnViewToolSettingsToolbar();
	afx_msg void OnUpdateToolSettingsToolbar(CCmdUI* pCmdUI);
	afx_msg void OnShowFineGrid();
	afx_msg void OnUpdateShowFineGrid(CCmdUI* pCmdUI);
	afx_msg void OnShowCourseGrid();
	afx_msg void OnUpdateShowCourseGrid(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSingleInstanceCustomMessage(WPARAM w, LPARAM l);
	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _MAINFRM_H_
