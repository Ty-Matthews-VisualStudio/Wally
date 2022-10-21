#if !defined(AFX_WADMERGEADDDLG_H__F8F2D471_AFDC_4BB4_83FD_5C817E2B1394__INCLUDED_)
#define AFX_WADMERGEADDDLG_H__F8F2D471_AFDC_4BB4_83FD_5C817E2B1394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WadMergeAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWadMergeAddDlg dialog

class CWadMergeJob;


class CWadMergeAddDlg : public CDialog
{
// Construction
public:
	CWadMergeAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWadMergeAddDlg)
	enum { IDD = IDD_WADMERGE_ADD_DLG };	
	CString	m_strWadFile;
	CString	m_strWildcard;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWadMergeAddDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	DWORD	m_dwMaxHeight;
	DWORD	m_dwMaxWidth;
	DWORD	m_dwMinHeight;
	DWORD	m_dwMinWidth;

	CWadMergeJob *m_pMergeJob;

	virtual void ValidateField( int iID, DWORD *pDWORD );
	virtual void SetField( int iID, DWORD dwValue );

public:
	virtual void SetMergeJob( CWadMergeJob *pMergeJob );
	virtual CWadMergeJob *GetMergeJob();


protected:
	// Generated message map functions
	//{{AFX_MSG(CWadMergeAddDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonWad();
	afx_msg void OnKillfocusEditMaximumHeight();
	afx_msg void OnKillfocusEditMaximumWidth();
	afx_msg void OnKillfocusEditMinimumHeight();
	afx_msg void OnKillfocusEditMinimumWidth();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WADMERGEADDDLG_H__F8F2D471_AFDC_4BB4_83FD_5C817E2B1394__INCLUDED_)
