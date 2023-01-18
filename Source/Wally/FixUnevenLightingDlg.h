/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                           © Copyright 1998-2001,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// BlendDlg.h : header file
//
// Created by Neal White III, 9-23-2001
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXUNEVENLIGHTINGDLG_H__D82D4401_B038_11D5_ACC8_0050DA17523C__INCLUDED_)
#define AFX_FIXUNEVENLIGHTINGDLG_H__D82D4401_B038_11D5_ACC8_0050DA17523C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FixUnevenLightingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixUnevenLightingDlg dialog

class CFixUnevenLightingDlg : public CDialog
{
// Construction
public:
	CFixUnevenLightingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFixUnevenLightingDlg)
	enum { IDD = IDD_FIX_UNEVEN_LIGHTING_DLG };
	int		m_iFixUnevenLightingAmount;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixUnevenLightingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFixUnevenLightingDlg)
	afx_msg void OnFilterPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXUNEVENLIGHTINGDLG_H__D82D4401_B038_11D5_ACC8_0050DA17523C__INCLUDED_)
