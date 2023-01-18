#ifndef _PASTE_SPECIAL_DLG_H_
#define _PASTE_SPECIAL_DLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PasteSpecialDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasteSpecialDlg dialog

class CPasteSpecialDlg : public CDialog
{
// Construction
public:
	CPasteSpecialDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasteSpecialDlg)
	enum { IDD = IDD_PASTE_SPECIAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasteSpecialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasteSpecialDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PASTE_SPECIAL_DLG_H_
