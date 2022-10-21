/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// OffsetDlg.h : header file
//
// Created by Neal White III, 1-3-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _OFFSETDLG_H_
#define _OFFSETDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class	CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg dialog

class COffsetDlg : public CDialog
{
// Construction
public:
	COffsetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COffsetDlg)
	enum { IDD = IDD_OFFSET_DLG };
	CSpinButtonCtrl	m_ctrlSpinOffsetY;
	CSpinButtonCtrl	m_ctrlSpinOffsetX;
	int		m_iOffsetX;
	int		m_iOffsetY;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COffsetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COffsetDlg)
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _OFFSETDLG_H_
