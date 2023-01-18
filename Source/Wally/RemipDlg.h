// RemipDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRemipDlg dialog

class CRemipDlg : public CDialog
{
// Construction
public:
	CRemipDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRemipDlg)
	enum { IDD = IDD_REMIP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRemipDlg)
	virtual void OnOK();
	afx_msg void OnBtnUseRecommended();
	afx_msg void OnBtnUseQuake();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
