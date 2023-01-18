#if !defined(AFX_WADMERGEDLG_H__5F43BBF1_BF8B_4DC9_9D22_681E35E0D0C8__INCLUDED_)
#define AFX_WADMERGEDLG_H__5F43BBF1_BF8B_4DC9_9D22_681E35E0D0C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WadMergeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWadMergeDlg dialog

class CWadMergeJob;
class CWADList;
class CPackageDoc;
class CWadMergeStatusDlg;

class CWadMergeDlg : public CDialog
{
// Construction
public:
	CWadMergeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWadMergeDlg)
	enum { IDD = IDD_WADMERGE_DLG };
	CComboBox	m_cbOpenWAD;
	CComboBox	m_cbWADType;
	CListCtrl	m_lstWADs;
	CButton	m_btnExisting;
	CEdit	m_edExisting;
	CString	m_strExistingWAD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWadMergeDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	typedef list< CWadMergeJob * > MergeJobs;
	typedef MergeJobs *LPMergeJobs;
	typedef MergeJobs::iterator itMergeJob;

	MergeJobs m_MergeJobs;
	BOOL m_bOpenPackage;

public:
	typedef struct
	{
		int					iDestinationChoice;
		int					iNewWADType;
		LPMergeJobs			lpMergeJobs;
		CWADList			*pWADList;
		LPSTR				szExistingWAD;
		DWORD				dwFlags;
		CPackageDoc			*pPackageDoc;
		DWORD				dwNumImages;
		CWadMergeStatusDlg	*pMergeStatus;
	} THREAD_PARAM, *LPTHREAD_PARAM;	

protected:
	virtual void AddJob( CWadMergeJob *pNewJob );
	virtual void EnableDisable();
	virtual void UpdateList();
	virtual DWORD GetNumImages();

public:
	static UINT WINAPI GoThread( LPVOID lpParameter );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWadMergeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonExistingWad();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonGo();
	afx_msg void OnRadioExistingWad();
	afx_msg void OnRadioNewWad();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonProperties();
	afx_msg void OnRadioOpenWad();
	afx_msg void OnButtonQuickAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WADMERGEDLG_H__5F43BBF1_BF8B_4DC9_9D22_681E35E0D0C8__INCLUDED_)
