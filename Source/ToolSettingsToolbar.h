#ifndef _TOOLSETTINGSTOOLBAR_H_
#define _TOOLSETTINGSTOOLBAR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ToolSettingsToolbar.h : header file
//

#include "ToolSettingsPage.h"
#include "STabCtrl.h"
#include "DecalListBox.h"

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

#ifndef _PACKAGE_PREVIEW_WND_H_
	#include "PackagePreviewWnd.h"
#endif

#ifndef _MOUSE_WND_H_
	#include "MouseWnd.h"
#endif

#define	LEFT_BUTTON		1
#define RIGHT_BUTTON	2

/////////////////////////////////////////////////////////////////////////////
// CToolSettingsToolbar dialog

class CToolSettingsToolbar : public CDialogBar
{
// Members
private:
	CToolSettingsPage m_Page1;
	CToolSettingsPage m_Page2;

	BOOL m_bFirstTimePreview;
	BOOL m_bFirstTimeMouse;
	BOOL m_bFirstTimeTab;
	BOOL m_bFirstTimeHiding;
	BOOL m_bEnableControls;
	BOOL m_bFirstTimeWADList;
	CPackagePreviewWnd m_wndPreview;
	CMouseWnd m_wndMouseButton;
	CWADItem *m_pWADItem;
	
	CStringArray m_saWADList;
	CImageHelper m_ihHelper;

// Methods
public:
	bool GetCurrentSelection();
	void EnableControls (bool bState = TRUE);
	void LoadImageToTool (int iMouseButton);
	void EnableRightButton (bool bEnable = TRUE);
	void SetStatus (LPCTSTR szText, int iPageNumber = 1);

// Construction
public:
	CToolSettingsToolbar(CWnd* pParent = NULL);   // standard constructor
	void Update(BOOL bToolSwitch = FALSE);

// Dialog Data
	//{{AFX_DATA(CToolSettingsToolbar)
	enum { IDD = IDD_TOOL_SETTINGS_TOOLBAR };
	CSTabCtrl	m_TabCtrl;
	CSpinButtonCtrl	m_spnTST2_ColorTolerance;
	CDecalListBox	m_lbTST1_Images;
	CComboBox	m_cbTST1_WAD;	
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolSettingsToolbar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FirstTimeInit( void);
	void UpdateDlgItems();
	void RetrieveDlgItems();	

	// Generated message map functions
	//{{AFX_MSG(CToolSettingsToolbar)
	afx_msg void OnDecalListBoxCustomMessage(UINT nType, UINT nFlags);
	afx_msg void OnMouseWndCustomMessage (UINT nType, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSelchangeTstTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTstTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTst1ComboWad();
	afx_msg void OnSelchangeTst1ListImages();
	afx_msg void OnKillfocusTst2ColorTolerance();
	afx_msg void OnSelchangeTst2ComboSize();
	afx_msg void OnSelchangeTst2ComboShape();
	afx_msg void OnSelchangeTst2ComboAmount();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeTst2ComboZoom();
	afx_msg void OnUpdateTst2ComboZoom(CCmdUI* pCmdUI);
	afx_msg void OnSelendokTst1ComboWad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _TOOLSETTINGSTOOLBAR_H_
