// EdgeDetectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdgeDetectDlg dialog

class CEdgeDetectDlg : public CDialog
{
// Construction
public:
	CEdgeDetectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEdgeDetectDlg)
	enum { IDD = IDD_EDGE_DETECT_DLG };
	BOOL	m_bMonochrome;
	int		m_iEdgeDetectAmount;
	int		m_iBackgroundColor;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdgeDetectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEdgeDetectDlg)
	afx_msg void OnFilterPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
