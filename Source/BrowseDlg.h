#if !defined(AFX_BROWSEDLG_H__68F57A41_F8A7_11D1_8068_5C2203C10627__INCLUDED_)
#define AFX_BROWSEDLG_H__68F57A41_F8A7_11D1_8068_5C2203C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BrowseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrowseDlg dialog

class CBrowseDlg : public CDialog
{
// Members
private:
	CString m_szDirectory;

// Construction
public:
	CBrowseDlg(CWnd* pParent = NULL);   // standard constructor
	void SetDirectory (CString Directory)
	{
		m_szDirectory = Directory;
	}
	CString GetDirectory ()
	{
		return m_szDirectory;
	}
	void SetDefaultBrowseDirectory (CString Directory)
	{
		m_szDirectoryEdit = Directory;
	}




// Dialog Data
	//{{AFX_DATA(CBrowseDlg)
	enum { IDD = IDD_BROWSE_DLG };
	CEdit	m_DirectoryEdit;
	CString	m_szDirectoryEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBrowseDlg)
	afx_msg void OnOk();
	afx_msg void OnSelectButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSEDLG_H__68F57A41_F8A7_11D1_8068_5C2203C10627__INCLUDED_)
