/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// CAddNoiseDlg.h : header file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _ADDNOISEDLG_H_
#define _ADDNOISEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLayer;

/////////////////////////////////////////////////////////////////////////////
// CAddNoiseDlg dialog

class CAddNoiseDlg : public CDialog
{
// Construction
public:
	CAddNoiseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddNoiseDlg)
	enum { IDD = IDD_ADD_NOISE_DLG };
	int		m_iAddNoiseAmount;
	BOOL	m_bMonochromeAddNoise;
	int		m_iAddNoiseDistribution;
	int		m_iAddNoiseType;
	int		m_iAddNoiseMinLength;
	int		m_iAddNoiseMaxLength;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNoiseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddNoiseDlg)
	afx_msg void OnFilterPreview();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  // _ADDNOISEDLG_H_
