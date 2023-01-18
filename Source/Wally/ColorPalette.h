// ColorPalette.h : header file
//
#ifndef _COLORPALETTE_H_
#define _COLORPALETTE_H_

#include "ColorPaletteWnd.h"

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CColorPalette dialog

class CColorPalette : public CDialogBar
{
// Construction
public:
	CColorPalette(CWnd* pParent = NULL);   // standard constructor

	CWallyDoc* m_pDoc;		// can be NULL, used to determine is color swatches
							// should be limited to 256 palette colors (or 24 bit)

	CColorPaletteWnd m_wndColorPalette;
	CColorSwatchWnd  m_wndColorSwatch;

	void Update( CWallyDoc* pDoc, BOOL bToolSwitch);

// Dialog Data
	//{{AFX_DATA(CColorPalette)
	enum { IDD = IDD_PALETTE_TOOLBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPalette)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  
	// Generated message map functions
	//{{AFX_MSG(CColorPalette)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif	//_COLORPALETTE_H_
