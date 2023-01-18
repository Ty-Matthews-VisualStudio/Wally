/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ResizeDlg.h : header file
//
// Created by Neal White III, 1-13-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _RESIZEDLG_H_
#define _RESIZEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

class CResizeDlg : public CDialog
{
// Construction
public:
	CResizeDlg(CWnd* pParent = NULL);   // standard constructor

public:
// Dialog Data
	//{{AFX_DATA(CResizeDlg)
	enum { IDD = IDD_RESIZE_DLG };
	BOOL	m_bProportial;
	int		m_iWidth;
	int		m_iHeight;
	int		m_iScaleX;
	int		m_iScaleY;
	int		m_iResizeType;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableDlgButton( int iCtrlID, BOOL bEnable);
	void RecalcProportionalNumbers(int iCtrlID);

	// Generated message map functions
	//{{AFX_MSG(CResizeDlg)
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioClick();
	afx_msg void OnCheckProportial();
	afx_msg void OnChangeEditWidth();
	afx_msg void OnChangeEditHeight();
	afx_msg void OnChangeEditScaleX();
	afx_msg void OnChangeEditScaleY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif  // _RESIZEDLG_H_
