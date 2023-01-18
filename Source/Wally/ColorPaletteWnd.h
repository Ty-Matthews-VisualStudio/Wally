// ColorPaletteWnd.h : header file
//
#ifndef _COLORPALETTEWND_H_
#define _COLORPALETTEWND_H_

#define UPDATE_GAMMA -1

class CColorPalette;

/////////////////////////////////////////////////////////////////////////////
// CColorSwatchWnd window

class CColorSwatchWnd : public CWnd
{
// Construction
public:
	CColorSwatchWnd()  { m_pColorPaletteToolbar = NULL; };

// Attributes
public:
	CColorPalette* m_pColorPaletteToolbar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPaletteWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorSwatchWnd() {};
	void DisplayColorPicker( BOOL bLeftColor);

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPaletteWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CRect m_rLeft, m_rRight;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CColorPaletteWnd window

class CColorPaletteWnd : public CWnd
{
// Construction
public:
	CColorPaletteWnd();

// Attributes
public:
	int  m_iColumns;
	int  m_iRows;
	int  m_iSelectedIndex;

	BOOL m_bUpdateMasterColors;

// Operations
public:
	void Update( int iColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPaletteWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorPaletteWnd();

	int CalcZone( CPoint ptPos);
	void UpdateStatusBar( CPoint ptPos);

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPaletteWnd)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // _COLORPALETTEWND_H_