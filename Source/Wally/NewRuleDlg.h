#ifndef _NEWRULEDLG_H_
#define _NEWRULEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewRuleDlg.h : header file
//
#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

class CRuleItem;

/////////////////////////////////////////////////////////////////////////////
// CNewRuleDlg dialog

class CNewRuleDlg : public CDialog
{
// Construction
public:
	CNewRuleDlg(CWnd* pParent = NULL);   // standard constructor
	void SetRuleItem (CRuleItem *pItem);
	CRuleItem *GetRuleItem ();

private:
	CRuleItem *m_pRuleItem;
	

// Dialog Data
	//{{AFX_DATA(CNewRuleDlg)
	enum { IDD = IDD_NEW_RULE_DLG };
	CButton	m_btnFlags;
	CComboBox	m_cbGameType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewRuleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewRuleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFlags();
	afx_msg void OnButtonOutputDir();
	afx_msg void OnSelchangeComboGametype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _NEWRULEDLG_H_
