#ifndef _PALETTE_EDITOR_DLG_H_
#define _PALETTE_EDITOR_DLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PaletteEditorDlg.h : header file
//

#ifndef _PALETTE_WND_H_
	#include "PaletteWnd.h"
#endif

#ifndef _WALLYPAL_H_
	#include "WallyPal.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg dialog

class CPaletteEditorDlg : public CDialog
{
// Members
private:
	CPaletteWnd m_wndPalette;	// Our custom CWnd-derived class that is the 
										// palette selector
	bool m_bFirstTime;
	unsigned char m_byPalette[768];

// Methods
public:
	void Update();
	void ChangeSelection();
	unsigned char *GetPalette()
	{
		return m_byPalette;
	}
	void SetPalette(CWallyPalette *pPal)
	{
		pPal->GetPalette (m_byPalette, 256);
	}
	void SetPalette(unsigned char *pPal = NULL)
	{
		if (pPal)
		{
			memcpy (m_byPalette, pPal, 768);
		}
		else
		{
			memset (m_byPalette, 0, 768);
		}
	}

// Construction
public:
	CPaletteEditorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaletteEditorDlg)
	enum { IDD = IDD_PALETTE_EDITOR_DLG };
	CEdit	m_edIndex;
	CEdit	m_edRed;
	CEdit	m_edGreen;
	CEdit	m_edBlue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaletteEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnPaletteWndCustomMessage(WPARAM nType, LPARAM nFlags);
	afx_msg void OnButtonBlend();
	afx_msg void OnKillfocusEditBlue();
	afx_msg void OnKillfocusEditGreen();
	afx_msg void OnKillfocusEditRed();
	afx_msg void OnButtonDefault();
	afx_msg void OnKillfocusEditIndex();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined _PALETTE_EDITOR_DLG_H_
