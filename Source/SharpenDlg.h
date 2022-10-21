/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// SharpenDlg.h : header file
//
// Created by Neal White III, 1-10-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _SHARPENDLG_H_
#define _SHARPENDLG_H_

#if _MSC_VER >= 1000
	#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CSharpenDlg dialog

class CSharpenDlg : public CDialog
{
// Construction
public:
	CSharpenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSharpenDlg)
	enum { IDD = IDD_SHARPEN_DLG };
	int		m_iSharpenAmount;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSharpenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSharpenDlg)
	afx_msg void OnFilterPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _SHARPENDLG_H_
