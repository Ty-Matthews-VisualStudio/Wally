#if !defined(AFX_BATCHSUMMARYDLG_H__4D3A32A0_8A42_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_BATCHSUMMARYDLG_H__4D3A32A0_8A42_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchSummaryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchSummaryDlg dialog

class CBatchSummaryDlg : public CDialog
{
// Construction
public:
	CBatchSummaryDlg(CWnd* pParent = NULL);   // standard constructor

// Members
public:
	CString m_strStatusText;
	CString m_strTitle;
	void SetStatusText (LPCTSTR szStatusText);

public:
	void SetTitle (LPCTSTR szTitle)
	{
		m_strTitle = szTitle;
	}

// Dialog Data
	//{{AFX_DATA(CBatchSummaryDlg)
	enum { IDD = IDD_BC_SUMMARY_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchSummaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchSummaryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHSUMMARYDLG_H__4D3A32A0_8A42_11D2_8410_F4900FC10000__INCLUDED_)
