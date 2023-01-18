#ifndef _DECALLISTBOX_H_
#define _DECALLISTBOX_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DecalListBox.h : header file

// Custom message map flags
#define	DECALLISTBOX_SELCHANGE	0
#define	DECALLISTBOX_ENTER		1

extern int WM_DECALLISTBOX_CUSTOM;

/////////////////////////////////////////////////////////////////////////////
// CDecalListBox window

class CDecalListBox : public CListBox
{
// Construction
public:
	CDecalListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecalListBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDecalListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDecalListBox)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef _DECALLISTBOX_H_
