#if !defined(AFX_PERCENTDLG_H__FFF34FC0_3583_11D3_A7F3_0000C0D88D2D__INCLUDED_)
#define AFX_PERCENTDLG_H__FFF34FC0_3583_11D3_A7F3_0000C0D88D2D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PercentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPercentDlg dialog

class CPercentDlg : public CDialog
{
// Construction
public:
	CPercentDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPercentDlg)
	enum { IDD = IDD_PERCENT_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPercentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	int m_iPercent;
	int GetPercent()
	{
		return m_iPercent;
	}


	// Generated message map functions
	//{{AFX_MSG(CPercentDlg)
	virtual void OnOK();
	afx_msg void OnKillfocusEditPercent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERCENTDLG_H__FFF34FC0_3583_11D3_A7F3_0000C0D88D2D__INCLUDED_)
