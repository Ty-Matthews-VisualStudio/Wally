#ifndef _RULESDLG_H_
#define _RULESDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RulesDlg.h : header file
//

#include "TabCheckListBox.h"
#include "RuleList.h"

/////////////////////////////////////////////////////////////////////////////
// CRulesDlg dialog

class CRulesDlg : public CDialog
{
// Construction
public:
	CRulesDlg(CWnd* pParent = NULL);   // standard constructor

// Members
private:
	CRuleList m_RuleList;

// Dialog Data
	//{{AFX_DATA(CRulesDlg)
	enum { IDD = IDD_RULES_DLG };
	CTabCheckListBox	m_lbRules;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRulesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _RULESDLG_H_
