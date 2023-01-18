#ifndef _FREEUNDODLG_H_
#define _FREEUNDODLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FreeUndoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFreeUndoDlg dialog

class CFreeUndoDlg : public CDialog
{
// Construction
public:
	CFreeUndoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFreeUndoDlg)
	enum { IDD = IDD_FREE_UNDOS_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CWallyDoc* m_pDoc;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFreeUndoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFreeUndoDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _FREEUNDODLG_H_
