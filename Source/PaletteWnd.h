#ifndef _PALETTE_WND_H_
#define _PALETTE_WND_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PaletteWnd.h : header file
//

#define PALETTE_THUMBNAIL_WIDTH		11
#define PALETTE_THUMBNAIL_HEIGHT	11
#define PALETTE_HIGHLIGHT_WIDTH		(PALETTE_THUMBNAIL_WIDTH + 4)
#define PALETTE_HIGHLIGHT_HEIGHT	(PALETTE_THUMBNAIL_HEIGHT + 4)

#define BUTTON_LEFT		0
#define BUTTON_RIGHT	1

#define	PALETTEWND_LBUTTONDOWN		0 
#define	PALETTEWND_LBUTTONDBLCLK	1
#define	PALETTEWND_RBUTTONDOWN		2
#define	PALETTEWND_RBUTTONDBLCLK	3

extern int WM_PALETTEWND_CUSTOM;

#ifndef _DIBSECTION_H__
	#include "DibSection.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteWnd window

class CPaletteWnd : public CWnd
{
// Construction
public:
	CPaletteWnd();

// Members
private:
	CDibSection m_dsView;
	unsigned char m_byPalette[768];
	int m_iColumns;
	int m_iRows;
	int m_iSelectedIndexLeft;
	int m_iSelectedIndexRight;

// Operations
public:
	void SetPalette(unsigned char *pPal);
	unsigned char *GetPalette();
	void SetPaletteIndex (int iColorType, int iColor, int iButton);	
	int CalcZone( CPoint ptPos);
	int GetSelectedIndex(int iButton);
	COLORREF GetSelectedIndexColor(int iButton);
	void SetSelectedIndex(int iIndex, int iButton);
	void Update();
	void Blend();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPaletteWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined _PALETTE_WND_H_
