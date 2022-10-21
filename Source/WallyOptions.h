#if !defined(AFX_WALLYOPTIONS_H__5EEB8CA1_9ECB_11D1_B1D1_0000C07E139E__INCLUDED_)
#define AFX_WALLYOPTIONS_H__5EEB8CA1_9ECB_11D1_B1D1_0000C07E139E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WallyOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWallyOptions dialog

class CWallyOptions : public CDialog
{
// Construction
public:
	CWallyOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWallyOptions)
	enum { IDD = IDD_WALLY_OPTIONS };
	CStatic	m_HeightString;
	CStatic	m_WidthString;
	CSpinButtonCtrl	m_SpinHeight;
	CSpinButtonCtrl	m_SpinWidth;
	CSpinButtonCtrl	m_SpinZoomControl;
	int		m_ZoomValue;
	UINT	m_Width;
	UINT	m_Height;
	int		m_PaletteConversion;
	BOOL	m_RetainSizeCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWallyOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditZoomvalue();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WALLYOPTIONS_H__5EEB8CA1_9ECB_11D1_B1D1_0000C07E139E__INCLUDED_)
