#ifndef _BATCHDLG_H__
#define _BATCHDLG_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchDlg.h : header file
//


#define		BUTTON_GO		0
#define		BUTTON_STOP		1

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog

#include "ThreadManager.h"

class CBatchDlg : public CDialog
{
// Members
private:
	CString m_strSourceDir;
	CString m_strDestinationDir;
	CString m_strStatusText;
	CString m_strWallyOptions;
	//CThreadList *m_pThreadList;

	int m_iMaxThreads;
	int m_iButtonState;
	int m_iJobCount;
	int m_iFinishedCount;

	int m_iDestinationType;
	CPackageDoc *m_pDestinationPackage;
	CThreadManager m_ThreadManager;

public:
	CThreadMessage m_ThreadMessage;

// Methods
private:
	void UpdateWallyOptions ();

public:
	void AddToFinishedTotal(int iStatus, LPCTSTR szSourceFile, LPCTSTR szDestinationFile, LPCTSTR szErrorMessage);
	void UpdateThreadStatus(int iProcessingCount, int iQueuedCount);
	int GetMaxThreads();
	void SetMaxThreads(int iMaxThreads);
	void SpoolerStopped();
	static void WINAPI ThreadMessageCallBack( CThreadMessage *lpMessage );
	void EnableDisable();
	

// Construction
public:
	CBatchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchDlg)
	enum { IDD = IDD_BATCH_DLG };
	CComboBox	m_cbWADType;
	CEdit	m_edWildcards;
	CButton	m_btnDestinationDirectory;
	CEdit	m_edDestinationDirectory;
	CComboBox	m_cbPackages;
	CComboBox	m_cbOutputFormat;
	CProgressCtrl	m_pbStatus;
	CButton	m_btnOk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchDlg)
	afx_msg void OnButtonDest();
	afx_msg void OnButtonSource();
	afx_msg void OnButtonGo();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonViewOptions();
	virtual void OnOK();
	afx_msg void OnRadioCurrentPackage();
	afx_msg void OnRadioTextures();
	afx_msg void OnSelchangeComboPackage();
	afx_msg void OnKillfocusEditDestDir();
	afx_msg void OnKillfocusEditSourceDir();
	//afx_msg void OnBatchDlgCustomMessage(UINT nType, UINT nFlags);
	afx_msg LRESULT OnBatchDlgCustomMessage(WPARAM nType, LPARAM nFlags);
	afx_msg void OnRadioNewPackage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _BATCHDLG_H__
