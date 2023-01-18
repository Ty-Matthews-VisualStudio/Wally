#ifndef _PALETTEDLG_H_
#define _PALETTEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PaletteDlg.h : header file
//

#include "ColorPaletteWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CPalettePickerDlg dialog

class CPalettePickerDlg : public CDialog
{
// Construction
public:
	CPalettePickerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPalettePickerDlg)
	enum { IDD = IDD_PALETTE_PICKER_DLG };
	UINT	m_iIndex;
	//}}AFX_DATA

	CColorPaletteWnd m_wndColorPalette;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPalettePickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPalettePickerDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PALETTEDLG_H_)
