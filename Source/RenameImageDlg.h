#if !defined(AFX_RENAMEIMAGEDLG_H__016D8EA0_907B_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_RENAMEIMAGEDLG_H__016D8EA0_907B_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RenameImageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenameImageDlg dialog

class CRenameImageDlg : public CDialog
{
// Construction
public:
	CRenameImageDlg(CWnd* pParent = NULL);   // standard constructor

private:
	int m_iMaxLength;
	CString m_strName;

public:
	void SetMaxLength (int iMaxLength)
	{
		m_iMaxLength = iMaxLength;		
	}
	int GetMaxLength ()
	{
		return m_iMaxLength;
	}
	void SetName(LPCTSTR szName)
	{
		m_strName = szName;		
	}
	CString GetName()
	{
		return m_strName;
	}

// Dialog Data
	//{{AFX_DATA(CRenameImageDlg)
	enum { IDD = IDD_RENAME_IMAGE_DLG };
	CButton	m_btnOK;
	CEdit	m_edName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRenameImageDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENAMEIMAGEDLG_H__016D8EA0_907B_11D2_8410_F4900FC10000__INCLUDED_)
