#ifndef _PAKEXPORTDLG_H_
#define _PAKEXPORTDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakExportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPakExportDlg dialog

class CPakDoc;
class CPakDirectory;

class CPakExportDlg : public CDialog
{
private:
	BOOL m_bHasSelections;
	BOOL m_bExportSelected;
	
	CPakDoc *m_pDoc;
	CPakDirectory *m_pPakDirectory;
	CString m_strSourceDirectory;
	CString m_strDestinationDirectory;
	CString m_strPakDirectory;
	CString m_strWildCards;

public:
	void HasSelections (BOOL bHasSelections = TRUE)
	{
		m_bHasSelections = bHasSelections;
	}
	void EnableDisableControls();
	void SetDocument(CPakDoc *pDoc)
	{
		m_pDoc = pDoc;
	}
	CPakDoc *GetDocument()
	{
		return m_pDoc;
	}
	void SetPakDirectory (CPakDirectory *pPakDirectory)
	{
		m_pPakDirectory = pPakDirectory;
	}
	
	CString GetSourceDirectory()
	{
		return m_strSourceDirectory;
	}
	CString GetDestinationDirectory()
	{
		return m_strDestinationDirectory;
	}
	CString GetWildCards()
	{
		return m_strWildCards;
	}
	CString GetPakDirectory()
	{
		return m_strPakDirectory;
	}
	BOOL ExportSelected()
	{
		return m_bExportSelected;
	}

// Construction
public:
	CPakExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPakExportDlg)
	enum { IDD = IDD_PAK_EXPORT_DLG };
	CButton	m_btnSource;
	CEdit	m_edWildCards;
	CEdit	m_edSource;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPakExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDestination();
	afx_msg void OnButtonSource();
	afx_msg void OnRadioExpSelected();
	afx_msg void OnRadioExpSpecific();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKEXPORTDLG_H_
