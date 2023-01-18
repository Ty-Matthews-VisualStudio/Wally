// EmbossDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmbossDlg dialog

class CEmbossDlg : public CDialog
{
// Construction
public:
	CEmbossDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEmbossDlg)
	enum { IDD = IDD_EMBOSS_DLG };
	int		m_iEmbossAmount;
	BOOL	m_bMonochrome;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmbossDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmbossDlg)
	afx_msg void OnFilterPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
