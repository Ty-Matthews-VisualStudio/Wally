#if !defined(AFX_PACKAGEADDTODLG_H__A0FA6FA0_913F_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_PACKAGEADDTODLG_H__A0FA6FA0_913F_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageAddToDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackageAddToDlg dialog

class CPackageAddToDlg : public CDialog
{
private:
	CString m_strDirectory;
	CString m_strWildCards;

// Construction
public:
	CPackageAddToDlg(CWnd* pParent = NULL);   // standard constructor
	CString GetDirectory ()
	{
		return m_strDirectory;
	}
	CString GetWildCards()
	{
		return m_strWildCards;
	}

// Dialog Data
	//{{AFX_DATA(CPackageAddToDlg)
	enum { IDD = IDD_ADDTO_PACKAGE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageAddToDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPackageAddToDlg)
	afx_msg void OnButtonSource();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGEADDTODLG_H__A0FA6FA0_913F_11D2_8410_F4900FC10000__INCLUDED_)
