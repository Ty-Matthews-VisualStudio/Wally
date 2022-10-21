/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyPropertyPage1.h : header file
//
// Created by Ty Matthews, 1-1-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef __WALLYPROPERTYPAGE1_H__
#define __WALLYPROPERTYPAGE1_H__

class CWallyDoc;


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage1 dialog

class CWallyPropertyPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage1)

// Construction
public:
	CWallyPropertyPage1();
	~CWallyPropertyPage1();

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage1)
	enum { IDD = IDD_PROPPAGE1 };
	CEdit	m_edCacheGroom;
	CSliderCtrl	m_CtrlGamma;
	CSpinButtonCtrl	m_SpinMaxItems;
	CSpinButtonCtrl	m_SpinZoomControl;
	int		m_iZoomValue;
	int		m_iPaletteConversion;
	int		m_iMaxUndoItems;
	double	m_dfGamma;
	BOOL	m_bGammaPaletteToolbar;
	//}}AFX_DATA

	CWallyDoc* m_pDoc;
	BOOL m_bEditZoomValueLockOut;
	double     m_dfOriginalGamma;
	BOOL       m_bOriginalGammaCorrectPaletteToolbar;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage1)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditGamma();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCheckGammaPaletteToolbar();
	afx_msg void OnBtnFreeUndoMemory();
	afx_msg void OnChangeEditZoomValue();
	afx_msg void OnRadioDontRemoveCache();
	afx_msg void OnRadioRemoveCache();
	afx_msg void OnKillfocusEditCacheGroom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage2 dialog

class CWallyPropertyPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage2)
// Construction
public:
	CWallyPropertyPage2();
	~CWallyPropertyPage2();

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage2)
	enum { IDD = IDD_PROPPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage2)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage2)
	afx_msg void OnBtnUseRecommended();
	afx_msg void OnBtnUseQuake();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage3 dialog

class CWallyPropertyPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage3)
// Construction
public:
	CWallyPropertyPage3();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage3)
	enum { IDD = IDD_PROPPAGE3 };
	CComboBox	m_cbFileType;
	CEdit	m_PresetEditWnd;
	CSpinButtonCtrl	m_SpinHeight;
	CSpinButtonCtrl	m_SpinWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage3)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage3)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioBlankdir();
	afx_msg void OnRadioPresetdir();
	afx_msg void OnRadioParentdir();
	afx_msg void OnKillfocusEditPresetdir();
	afx_msg void OnCheckNon16();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnKillfocusEditWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage4 dialog

class CWallyPropertyPage4 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage4)
// Construction
public:
	CWallyPropertyPage4();   // standard constructor

	BOOL EnableDlgItem( int iID, BOOL bEnable)
			{ return ::EnableWindow( GetDlgItem( iID)->GetSafeHwnd(), bEnable); };

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage4)
	enum { IDD = IDD_PROPPAGE4 };
	int m_iGridHorizontal;
	int m_iGridVertical;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage4)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage4)
	afx_msg void OnBtnEvery1();
	afx_msg void OnBtnEvery2();
	afx_msg void OnBtnEvery4();
	afx_msg void OnBtnEvery8();
	afx_msg void OnBtnEvery10();
	afx_msg void OnBtnEvery12();
	afx_msg void OnBtnEvery16();
	afx_msg void OnBtnEvery32();
	afx_msg void OnBtnEvery64();
	afx_msg void OnCheckCourseGrid();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage5 dialog

class CWallyPropertyPage5 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage5)
// Construction
public:
	CWallyPropertyPage5();   // standard constructor

	BOOL EnableDlgItem( int iID, BOOL bEnable)
	{ 
		return ::EnableWindow( GetDlgItem( iID)->GetSafeHwnd(), bEnable); 
	};
	void SelectPalette (int iID);

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage5)
	enum { IDD = IDD_PROPPAGE5 };
	CButton	m_btnQ2;
	CButton	m_btnQ1;
	CEdit	m_edQ2Custom;
	CEdit	m_edQ1Custom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage5)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage5)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioQ1Default();
	afx_msg void OnRadioQ2Default();
	afx_msg void OnRadioQ1Custom();
	afx_msg void OnRadioQ2Custom();
	afx_msg void OnButtonQ1Custom();
	afx_msg void OnButtonQ2Custom();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefaultPalette();
	afx_msg void OnButtonEditPalette();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __WALLYPROPERTYPAGE1_H__


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage6 dialog

class CWallyPropertyPage6 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage6)

// Construction
public:
	CWallyPropertyPage6();
	~CWallyPropertyPage6();

	BOOL EnableDlgItem( int iID, BOOL bEnable)
	{ 
		return ::EnableWindow( GetDlgItem( iID)->GetSafeHwnd(), bEnable); 
	};
	void EnableDisableControls();
	CString SelectPalette ();

	BOOL m_bCalledFromBatchDlg;
	

// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage6)
	enum { IDD = IDD_PROPPAGE6 };
	CButton	m_btnRetainSettings;
	CSpinButtonCtrl	m_spnMaxThreads;
	CEdit	m_edSourcePalette;
	CEdit	m_edDestPalette;
	CComboBox	m_cbImages;
	CButton	m_btnSource;
	CButton	m_btnDest;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage6)
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage6)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDest();
	afx_msg void OnButtonSource();
	afx_msg void OnRadioDestCurrent();
	afx_msg void OnRadioDestCustom();
	afx_msg void OnRadioSourceCurrent();
	afx_msg void OnRadioSourceCustom();
	afx_msg void OnSelchangeComboImageType();
	afx_msg void OnKillfocusEditMaxThreads();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage7 dialog

class CWallyPropertyPage7 : public CPropertyPage
{
	DECLARE_DYNCREATE(CWallyPropertyPage7)
// Construction
public:
	CWallyPropertyPage7();
	~CWallyPropertyPage7();
	
// Dialog Data
	//{{AFX_DATA(CWallyPropertyPage7)
	enum { IDD = IDD_PROPPAGE7 };
	CCheckListBox	m_lbWildCards;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertyPage7)
	virtual void OnOK();
	protected:		
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallyPropertyPage7)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListWildcards();
	afx_msg void OnButtonCheckAll();
	afx_msg void OnButtonUncheckAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
