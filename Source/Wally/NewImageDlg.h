#ifndef _NEWIMAGEDLG_H_
#define _NEWIMAGEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewImageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewImageDlg dialog

class CNewImageDlg : public CDialog
{
// Construction
public:
	CNewImageDlg(CWnd* pParent = NULL);   // standard constructor

// Members
private:
	CString m_strTextureName;
	int m_iFileType;
	unsigned char m_byPalette[768];
	
// Methods
public:
	void SetTextureName (LPCTSTR szName);
	CString GetTextureName ();
	unsigned char *GetPalette()
	{
		return m_byPalette;
	}
	int GetFileType()
	{
		return m_iFileType;
	}

// Dialog Data
	//{{AFX_DATA(CNewImageDlg)
	enum { IDD = IDD_NEW_IMAGE_DLG };
	CEdit	m_edWidth;
	CEdit	m_edHeight;
	CEdit	m_edName;
	CSpinButtonCtrl	m_spnWidth;
	CSpinButtonCtrl	m_spnHeight;
	CComboBox	m_cbFileType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewImageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnKillfocusEditWidth();
	afx_msg void OnSelchangeComboGameType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _NEWIMAGEDLG_H_
