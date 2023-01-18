/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// BlendDlg.h : header file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _BLENDDLG_H_
#define _BLENDDLG_H_

#if _MSC_VER >= 1000
	#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CBlendDlg dialog

class CBlendDlg : public CDialog
{
// Construction
public:
	CBlendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBlendDlg)
	enum { IDD = IDD_BLEND_DLG };
	int		m_iBlendAmount;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBlendDlg)
	afx_msg void OnFilterPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  // _BLENDDLG_H_