/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// FilterBCDlg.h : header file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

//class	CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CFilterBrightnessContrastDlg dialog

class CFilterBrightnessContrastDlg : public CDialog
{
// Construction
public:
	CFilterBrightnessContrastDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilterBrightnessContrastDlg)
	enum { IDD = IDD_BRIGHTNESS_CONTRAST_DLG };
	CSpinButtonCtrl	m_ctrlSpinContrast;
	CSpinButtonCtrl	m_ctrlSpinBrightness;
	int		m_iFilterBrightnessAmount;
	int		m_iFilterContrastAmount;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterBrightnessContrastDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterBrightnessContrastDlg)
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
