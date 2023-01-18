#ifndef _PAKIMPORTDLG_H_
#define _PAKIMPORTDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakImportDlg.h : header file
//

class CPakDoc;
class CPakDirectory;

/////////////////////////////////////////////////////////////////////////////
// CPakImportDlg dialog

class CPakImportDlg : public CDialog
{
private:
	CPakDoc *m_pDoc;
	CPakDirectory *m_pPakDirectory;

	CString m_strSourceDirectory;
	CString m_strOffsetDirectory;
	CString m_strWildCards;
	CString m_strPakDirectory;

// Construction
public:
	CPakImportDlg(CWnd* pParent = NULL);   // standard constructor

	void SetDocument(CPakDoc *pDoc)
	{
		m_pDoc = pDoc;
	}
	CPakDoc *GetDocument()
	{
		return m_pDoc;
	}

	void SetPakDirectory(CPakDirectory *pPakDirectory)
	{
		m_pPakDirectory = pPakDirectory;
	}
	CString GetPakDirectory()
	{
		return m_strPakDirectory;
	}

	CString GetSourceDirectory()
	{
		return m_strSourceDirectory;
	}
	CString GetOffsetDirectory()
	{
		return m_strOffsetDirectory;
	}
	CString GetWildCards()
	{
		return m_strWildCards;
	}

// Dialog Data
	//{{AFX_DATA(CPakImportDlg)
	enum { IDD = IDD_PAK_IMPORT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPakImportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDestination();
	afx_msg void OnButtonSource();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKIMPORTDLG_H_
