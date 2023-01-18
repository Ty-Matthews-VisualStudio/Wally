// MarbleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMarbleDlg dialog

class CFilter;

class CMarbleDlg : public CDialog
{
// Construction
public:
	CMarbleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMarbleDlg)
	enum { IDD = IDD_MARBLE_DLG };
	CSpinButtonCtrl	m_ctrlSpinScaleZ;
	CSpinButtonCtrl	m_ctrlSpinScaleY;
	CSpinButtonCtrl	m_ctrlSpinScaleX;
	CSpinButtonCtrl	m_ctrlSpinOffsetZ;
	CSpinButtonCtrl	m_ctrlSpinOffsetY;
	CSpinButtonCtrl	m_ctrlSpinOffsetX;
	CSpinButtonCtrl	m_ctrlSpinNumThinStripes;
	CSpinButtonCtrl	m_ctrlSpinLevels;
	CSpinButtonCtrl	m_ctrlSpinColorVariance;
	CSpinButtonCtrl	m_ctrlSpinAmplitude;
	CSpinButtonCtrl	m_ctrlSpinNumThickStripes;
	BOOL	m_bMarbleExtraSmooth;
	BOOL	m_bMarbleSeamlessTexture;
	UINT	m_iMarbleColorVariance;
	//}}AFX_DATA

	CString    m_strDlgTitle;

	CLayer*       m_pLayer;
	CFilter*      m_pFilter;
	MarbleParams* m_pTileParams;
	BOOL          m_bPreviewApplied;

	int    m_iMarbleSeed;
	int    m_iMarbleLevels;
	int    m_iMarbleNumThickStripes;
	int    m_iMarbleNumThinStripes;
	int    m_iShape;

	double m_dfMarbleAmplitude;
	Vec    m_vMarbleScale;
	Vec    m_vMarbleOffset;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarbleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DoFilterPreview( BOOL bFastPreview);

	// Generated message map functions
	//{{AFX_MSG(CMarbleDlg)
	afx_msg void OnBtnJustDoIt();
	afx_msg void OnBtnRandomSeed();
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	afx_msg void OnFilterFastPreview();
	virtual void OnOK();
	afx_msg void OnBtnBackgroundColor();
	afx_msg void OnBtnThickStripeColor();
	afx_msg void OnBtnThinStripeColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDeltaPos( NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
