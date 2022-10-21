#if !defined(AFX_PACKAGE2FORMVIEW_H__ACB3B143_76AC_11D3_841A_00104BCBA50D__INCLUDED_)
#define AFX_PACKAGE2FORMVIEW_H__ACB3B143_76AC_11D3_841A_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Package2FormView.h : header file
//

#include "DibSection.h"
#include "Package2FormListBox.h"

#define P2_FORMVIEW_LB_OFFSET_LEFT		10
#define P2_FORMVIEW_LB_OFFSET_RIGHT		10
#define P2_FORMVIEW_LB_OFFSET_TOP		28
#define P2_FORMVIEW_LB_OFFSET_BOTTOM	54

#define P2_FORMVIEW_ED_OFFSET_LEFT		60

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPackage2FormView : public CFormView
{
protected:
	CPackage2FormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackage2FormView)

// Form Data
public:
	//{{AFX_DATA(CPackage2FormView)
	enum { IDD = IDD_PACKAGE2VIEW_FORM };
	CButton	m_btnSelectAll;
	CButton	m_ckFilter;
	CStatic	m_csNumItems;
	CPackage2FormListBox	m_lbImages;
	CEdit	m_edFilter;
	//}}AFX_DATA

// Attributes
public:
	CBrush m_brDlgColor;
	//CPackage2FormListBox m_lbImages;
	//CEdit m_edFilter;
	BOOL m_bLBImagesShowing;
	BOOL m_bEDFilterShowing;

	int m_iOldRCWidth;
	int m_iOldRCHeight;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackage2FormView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackage2FormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPackage2FormView)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCheckFilter();
	afx_msg void OnEditPaste();
	afx_msg void OnButtonSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGE2FORMVIEW_H__ACB3B143_76AC_11D3_841A_00104BCBA50D__INCLUDED_)
