#ifndef _NEWPACKAGEIMAGEDLG_H_
#define _NEWPACKAGEIMAGEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewPackageImageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewPackageImageDlg dialog

class CNewPackageImageDlg : public CDialog
{
// Construction
public:
	CNewPackageImageDlg(CWnd* pParent = NULL);   // standard constructor

// Members
private:
	int m_iWidth;
	int m_iHeight;
	CString m_strName;

// Methods
public:
	void ValidateNameField();
	int GetWidth()
	{
		return m_iWidth;
	}
	void SetWidth (int iWidth)
	{
		m_iWidth = iWidth;		
	}
		
	int GetHeight()
	{
		return m_iHeight;
	}
	void SetHeight (int iHeight)
	{
		m_iHeight = iHeight;
	}

	CString GetName()
	{
		return m_strName;
	}


// Dialog Data
	//{{AFX_DATA(CNewPackageImageDlg)
	enum { IDD = IDD_NEW_PACKAGE_IMAGE };
	CButton	m_btnOK;
	CEdit	m_edName;
	CSpinButtonCtrl	m_spnWidth;
	CSpinButtonCtrl	m_spnHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPackageImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPackageImageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnKillfocusEditWidth();
	virtual void OnOK();
	afx_msg void OnUpdateEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _NEWPACKAGEIMAGEDLG_H_
