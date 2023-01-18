#if !defined(AFX_WADMERGESTATUSDLG_H__3E5CB18F_0319_40B8_8A22_E719011A8E95__INCLUDED_)
#define AFX_WADMERGESTATUSDLG_H__3E5CB18F_0319_40B8_8A22_E719011A8E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WadMergeStatusDlg.h : header file
//

extern int WM_WAD_MERGE_STATUS_CUSTOM;

#define WAD_MERGE_STATUS_IMAGE_COUNT		1
#define WAD_MERGE_STATUS_STEP				2

/////////////////////////////////////////////////////////////////////////////
// CWadMergeStatusDlg dialog

class CWadMergeStatusDlg : public CDialog
{
// Construction
public:
	CWadMergeStatusDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWadMergeStatusDlg)
	enum { IDD = IDD_WAD_MERGE_STATUS };
	CButton	m_btnOK;
	CProgressCtrl	m_ctlProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWadMergeStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual void SetThreadParameter( LPVOID lpParam )
	{
		m_lpThreadParameter = lpParam;
	}

	typedef struct
	{
		CString		strImage;
		CString		strStatus;
		DWORD		dwNumImages;
	} MERGE_STATUS, *LPMERGE_STATUS;

// Implementation
protected:
	LPVOID m_lpThreadParameter;
	HANDLE m_hThreadHandle;
	UINT m_iTimerID;
	
	// Generated message map functions
	//{{AFX_MSG(CWadMergeStatusDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnWadMergeStatusDlgCustomMessage(WPARAM nType, LPARAM nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WADMERGESTATUSDLG_H__3E5CB18F_0319_40B8_8A22_E719011A8E95__INCLUDED_)
