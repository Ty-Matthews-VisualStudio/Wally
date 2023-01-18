#if !defined(AFX_HALFLIFEPALETTEWND_H__0B0FD4C2_8B87_11D2_BA93_00104BCBA50D__INCLUDED_)
#define AFX_HALFLIFEPALETTEWND_H__0B0FD4C2_8B87_11D2_BA93_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HalfLifePaletteWnd.h : header file
//

#define PALETTE_THUMBNAIL_WIDTH		11
#define PALETTE_THUMBNAIL_HEIGHT	11
#define PALETTE_HIGHLIGHT_WIDTH		(PALETTE_THUMBNAIL_WIDTH + 4)
#define PALETTE_HIGHLIGHT_HEIGHT	(PALETTE_THUMBNAIL_HEIGHT + 4)

#define BUTTON_LEFT		0
#define BUTTON_RIGHT	1

#include "DibSection.h"

/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteWnd window

class CHalfLifePaletteWnd : public CWnd
{
// Construction
public:
	CHalfLifePaletteWnd();

// Members
private:
	CDibSection m_dsView;
	unsigned char m_pbyPalette[768];
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

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHalfLifePaletteWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHalfLifePaletteWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHalfLifePaletteWnd)
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

#endif // !defined(AFX_HALFLIFEPALETTEWND_H__0B0FD4C2_8B87_11D2_BA93_00104BCBA50D__INCLUDED_)
