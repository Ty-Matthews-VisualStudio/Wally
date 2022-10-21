#if !defined(AFX_DUPLICATENAMEDLG_H__40868140_9A9F_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_DUPLICATENAMEDLG_H__40868140_9A9F_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DuplicateNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDuplicateNameDlg dialog

class CDuplicateNameDlg : public CDialog
{
// Construction
public:
	CDuplicateNameDlg(CWnd* pParent = NULL);   // standard constructor
	
// Members
private:
	CString m_strName;
	bool m_bReplaceImage;
	int m_iMaxLength;	

public:
	void SetMaxLength (int iMaxLength)
	{
		m_iMaxLength = iMaxLength;		
	}
	int GetMaxLength ()
	{
		return m_iMaxLength;
	}
	void SetName (LPCTSTR szName)
	{
		m_strName = szName;
	}
	CString GetName()
	{
		return m_strName;
	}
	bool ReplaceImage()
	{
		return m_bReplaceImage;
	}





// Dialog Data
	//{{AFX_DATA(CDuplicateNameDlg)
	enum { IDD = IDD_DUPLICATE_NAME };
	CButton	m_btnOK;
	CEdit	m_edName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDuplicateNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDuplicateNameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioReplace();
	afx_msg void OnRadioChangeName();
	afx_msg void OnUpdateEditName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUPLICATENAMEDLG_H__40868140_9A9F_11D2_8410_F4900FC10000__INCLUDED_)
