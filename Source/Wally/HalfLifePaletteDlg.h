#if !defined(AFX_HALFLIFEPALETTEDLG_H__FCB88160_8AF0_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_HALFLIFEPALETTEDLG_H__FCB88160_8AF0_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HalfLifePaletteDlg.h : header file
//

#include "PaletteWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteDlg dialog

class CHalfLifePaletteDlg : public CDialog
{
// Members
private:
	CPaletteWnd m_wndPalette;	// Our custom CWnd-derived class that is the 
										// palette selector
	bool m_bFirstTime;
	unsigned char m_byPalette[768];
	CString m_strPath;


// Methods
public:
	void Update();
	void ChangeSelection();
	unsigned char *GetPalette()
	{
		return m_byPalette;
	}

// Construction
public:
	CHalfLifePaletteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHalfLifePaletteDlg)
	enum { IDD = IDD_HALF_LIFE_PALETTE_DLG };
	CComboBox	m_cbPalette;
	CWnd	m_edIndex;
	CWnd	m_edRed;
	CWnd	m_edGreen;
	CWnd	m_edBlue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHalfLifePaletteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHalfLifePaletteDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPalette();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusEditIndex();
	afx_msg void OnKillfocusEditBlue();
	afx_msg void OnKillfocusEditGreen();
	afx_msg void OnKillfocusEditRed();
	afx_msg void OnButtonDefault();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonBlend();
	virtual void OnOK();
	afx_msg LRESULT OnPaletteWndCustomMessage(WPARAM nType, LPARAM nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HALFLIFEPALETTEDLG_H__FCB88160_8AF0_11D2_8410_F4900FC10000__INCLUDED_)
