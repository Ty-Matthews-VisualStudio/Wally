#if !defined(AFX_NEWWALDLG_H__1D6B9EC1_975D_11D1_8068_5C2203C10627__INCLUDED_)
#define AFX_NEWWALDLG_H__1D6B9EC1_975D_11D1_8068_5C2203C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewWalDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewWalDlg dialog

class CNewWalDlg : public CDialog
{
// Construction
public:
	CNewWalDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewWalDlg)
	enum { IDD = IDD_NEW_WAL };
	CSpinButtonCtrl	m_ctrlSpinWidth;
	CSpinButtonCtrl	m_ctrlSpinHeight;
	CString	m_strNewName;
	UINT	m_uWidth;
	UINT	m_uHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewWalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewWalDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWWALDLG_H__1D6B9EC1_975D_11D1_8068_5C2203C10627__INCLUDED_)
