// MarbleTileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMarbleTileDlg dialog

class CMarbleTileDlg : public CDialog
{
// Construction
public:
	CMarbleTileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMarbleTileDlg)
	enum { IDD = IDD_MARBLE_TILE_DLG };
	int		m_iRandomSeed;
	int		m_iMarbleTileSize;
	int		m_iMarbleTileShape;
	int		m_iMarbleTileColor;
	int		m_iMarbleTileStyle;
	int		m_iGroutType;
	int		m_iBevelType;
	//}}AFX_DATA

	CLayer*  m_pLayer;
	CFilter* m_pFilter;

	BOOL m_bPreviewApplied;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarbleTileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetParams( MarbleParams* pParams);
	void DoFilterPreview( BOOL bFastPreview);

	// Generated message map functions
	//{{AFX_MSG(CMarbleTileDlg)
	afx_msg void OnFilterFastPreview();
	afx_msg void OnFilterPreview();
	afx_msg void OnMarbleProperties();
	afx_msg void OnBtnGroutColor();
	afx_msg void OnBtnRandomSeed();
	afx_msg void OnBtnJustDoIt();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
