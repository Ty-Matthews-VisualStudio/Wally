#if !defined(AFX_MDICLIENT_H__B824C982_B3B9_11D1_9297_00A024DF24C3__INCLUDED_)
#define AFX_MDICLIENT_H__B824C982_B3B9_11D1_9297_00A024DF24C3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MdiClient.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndMdiClient window

class CWndMdiClient : public CWnd
{
// Construction
public:
	CWndMdiClient();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndMdiClient)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndMdiClient();
	void InvalidateRect( LPCRECT lpRect, BOOL bErase = TRUE );

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndMdiClient)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDICLIENT_H__B824C982_B3B9_11D1_9297_00A024DF24C3__INCLUDED_)
