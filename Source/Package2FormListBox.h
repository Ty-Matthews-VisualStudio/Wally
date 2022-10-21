#if !defined(AFX_PACKAGE2FORMLISTBOX_H__EFC858F2_7741_11D3_841B_00104BCBA50D__INCLUDED_)
#define AFX_PACKAGE2FORMLISTBOX_H__EFC858F2_7741_11D3_841B_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Package2FormListBox.h : header file
//

#define P2_FORM_LB_ITEMHEIGHT		16
/////////////////////////////////////////////////////////////////////////////
// CPackage2FormListBox window

class CPackage2FormListBox : public CListBox
{
// Construction
public:
	CPackage2FormListBox();

// Attributes
public:
	CFont m_foDrawText;
	CBrush m_brColor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackage2FormListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackage2FormListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackage2FormListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGE2FORMLISTBOX_H__EFC858F2_7741_11D3_841B_00104BCBA50D__INCLUDED_)
