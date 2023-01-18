/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1999,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ReduceColorsDlg.h : header file
//
// Created by Neal White III, 1-29-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _REDUCECOLORSDLG_H_
#define _REDUCECOLORSDLG_H_

#if _MSC_VER >= 1000
	#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CReduceColorsDlg dialog

class CReduceColorsDlg : public CDialog
{
// Construction
public:
	CReduceColorsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReduceColorsDlg)
	enum { IDD = IDD_REDUCE_COLORS_DLG };
	int		m_iReserveColor1;
	int		m_iReserveColor2;
	int		m_iReserveColor3;
	int		m_iReserveColor4;
	int		m_iMaxColors;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReduceColorsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetReserved( int iReserved[4]);

	// Generated message map functions
	//{{AFX_MSG(CReduceColorsDlg)
	afx_msg void OnBtnHalfLifeLiquid();
	afx_msg void OnBtnHalfLifeTransparent();
	afx_msg void OnBtnUnrealTransparent();
	afx_msg void OnFilterPreview();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif  // _REDUCECOLORSDLG_H_
