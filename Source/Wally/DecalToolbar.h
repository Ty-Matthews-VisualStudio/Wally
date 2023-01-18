#ifndef _DECAL_TOOLBAR_H_
#define _DECAL_TOOLBAR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DecalToolbar.h : header file
//

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
// CDecalToolbar dialog

class CDecalToolbar : public CDialogBar
{
// Construction
public:
	CDecalToolbar(CWnd* pParent = NULL);   // standard constructor

// Methods
public:
	bool GetCurrentSelection();
	void EnableControls (bool bState = TRUE);
	void LoadImageToTool (int iMouseButton);
	void EnableRightButton (bool bEnable = TRUE);
	void SetStatus (LPCTSTR szText, int iPageNumber = 1);

// Members
private:
	bool m_bFirstTimePreview;
	bool m_bFirstTimeMouse;
	bool m_bFirstTimeTab;
	bool m_bFirstTimeHiding;
	bool m_bEnableControls;
	CPackagePreviewWnd m_wndPreview;
	CMouseWnd m_wndMouseButton;
	CWADItem *m_pWADItem;
	
	CStringArray m_strWADList;
	CImageHelper m_ihHelper;


// Dialog Data
	//{{AFX_DATA(CDecalToolbar)
	enum { IDD = IDD_DECAL_TOOLBAR };
	CStatic	m_scColorTolerance;
	CSpinButtonCtrl	m_spnColorTolerance;
	CEdit	m_edColorTolerance;
	CStatic	m_scStatus;
	CDecalListBox	m_lbImages;
	CComboBox	m_cbWAD;	
	CButton	m_btnPreview;	
	CSTabCtrl	m_TabCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecalToolbar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FirstTimeInit( void);
	void Update();

	// Generated message map functions
	//{{AFX_MSG(CDecalToolbar)
	afx_msg LRESULT OnDecalListBoxCustomMessage(WPARAM nType, LPARAM nFlags);
	afx_msg LRESULT OnMouseWndCustomMessage (WPARAM nType, LPARAM nFlags);
	afx_msg void OnPaint();
	afx_msg void OnSelchangeDtTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusDt1ComboWad();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeDt1ComboWad();
	afx_msg void OnSelchangeDt1ListImages();
	afx_msg void OnSelchangingDtTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef _DECAL_TOOLBAR_H_

