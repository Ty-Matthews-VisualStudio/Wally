#if !defined(AFX_COLORDECALDLG_H__9584AC01_D9F2_11D4_A621_0000C04D1FBF__INCLUDED_)
#define AFX_COLORDECALDLG_H__9584AC01_D9F2_11D4_A621_0000C04D1FBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorDecalDlg.h : header file
//

#define COLOR_DECAL_CLIPBOARD		0x1
#define COLOR_DECAL_FILE			0x2
#define COLOR_DECAL_EDIT			0x4
#define COLOR_DECAL_SOURCE_MASK		( COLOR_DECAL_CLIPBOARD | COLOR_DECAL_FILE | COLOR_DECAL_EDIT )
#define COLOR_DECAL_CROP			0x8
#define COLOR_DECAL_RESIZE			0x10
#define COLOR_DECAL_ERROR			0x20
#define COLOR_DECAL_DIMENSION_MASK	( COLOR_DECAL_CROP | COLOR_DECAL_RESIZE | COLOR_DECAL_ERROR )
#define COLOR_DECAL_RESIZE_ASPECT_RATIO			0x40
#define COLOR_DECAL_RESIZE_STRETCH_FIT			0x80
#define COLOR_DECAL_RESIZE_OPTION_MASK	( COLOR_DECAL_RESIZE_ASPECT_RATIO | COLOR_DECAL_RESIZE_STRETCH_FIT )
#define COLOR_DECAL_RESIZE_TRANSPARENT_BLUE		0x100
#define COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL		0x200
#define COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC	0x400
#define COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SURROUND	0x800
#define COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK	( COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL | COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC | COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SURROUND )

#define COLOR_DECAL_MAX_SIZE		10752

/////////////////////////////////////////////////////////////////////////////
// CColorDecalDlg dialog

#include "DecalColorChooserWnd.h"

class CColorDecalDlg : public CDialog
{
// Construction
public:
	CColorDecalDlg(CWnd* pParent = NULL);   // standard constructor

// Members
private:
	DWORD m_dwWidth;
	DWORD m_dwHeight;
	DWORD m_dwOptions;
	CString m_strDirectory;
	CString m_strFileName;
	CDecalColorChooserWnd m_wndColor;
	bool m_bFirstTime;
	BYTE m_byRed;
	BYTE m_byGreen;
	BYTE m_byBlue;
	
// Methods
private:
	void VerifyDivisibleBy16( UINT iID );
	void VerifyWidthHeight( UINT iID );
	BOOL VerifyHalfLifeDirectory();
	int GetDlgInt( UINT iID );
	void EnableDisableResizeOptions();

public:
	DWORD GetWidth();
	DWORD GetHeight();
	DWORD GetOptions();
	LPCTSTR GetDirectory();
	LPCTSTR GetFileName();
	void GetBlendColor( BYTE &byRed, BYTE &byGreen, BYTE &byBlue );
	static void CalcNewDecalImageDimensions( UINT iWidth, UINT iHeight, UINT &iNewWidth, UINT &iNewHeight, UINT &iResizeWidth, UINT &iResizeHeight );
	static void OnWizardColorDecal( CWallyApp *pWallyApp );
	
// Dialog Data
	//{{AFX_DATA(CColorDecalDlg)
	enum { IDD = IDD_COLOR_DECAL };
	CComboBox	m_cbMod;
	CButton	m_btnFile;
	CEdit	m_edWidth;
	CEdit	m_edHeight;
	CEdit	m_edFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorDecalDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorDecalDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioClipboard();
	afx_msg void OnRadioFile();
	afx_msg void OnRadioLetMe();
	afx_msg void OnKillfocusEditWidth();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnButtonHlDir();
	virtual void OnOK();
	afx_msg void OnButtonFile();
	afx_msg void OnRadioError();
	afx_msg void OnRadioCrop();
	afx_msg void OnRadioResize();
	afx_msg void OnKillfocusEditHlDirectory();
	afx_msg void OnRadioMaintainAspect();
	afx_msg void OnRadioStretchToFit();
	afx_msg void OnCheckPureBluePixels();
	afx_msg void OnChangeColor (UINT nType, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnRadioBlendOverallColor();
	afx_msg void OnRadioBlendSpecificColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORDECALDLG_H__9584AC01_D9F2_11D4_A621_0000C04D1FBF__INCLUDED_)
