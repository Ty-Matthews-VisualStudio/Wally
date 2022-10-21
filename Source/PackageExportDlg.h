#if !defined(AFX_PACKAGEEXPORTDLG_H__E9F72380_9827_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_PACKAGEEXPORTDLG_H__E9F72380_9827_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImageHelper.h"

// PackageExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackageExportDlg dialog

class CPackageExportDlg : public CDialog
{
// Members
public:
	CImageHelper m_ihHelper;
	CString m_strDirectory;
	CString m_strExtension;	

// Construction
public:
	CPackageExportDlg(CWnd* pParent = NULL);   // standard constructor
	CString GetExtension ()
	{
		return m_strExtension;
	}
	void SetDirectory(LPCTSTR szDirectory)
	{
		m_strDirectory = szDirectory;
	}
	CString GetDirectory()
	{
		return m_strDirectory;
	}

// Dialog Data
	//{{AFX_DATA(CPackageExportDlg)
	enum { IDD = IDD_PACKAGE_EXPORT_DLG };
	CComboBox	m_cbOutputFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPackageExportDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOutputDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGEEXPORTDLG_H__E9F72380_9827_11D2_8410_F4900FC10000__INCLUDED_)
