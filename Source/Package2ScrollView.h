#if !defined(AFX_PACKAGE2SCROLLVIEW_H__ACB3B145_76AC_11D3_841A_00104BCBA50D__INCLUDED_)
#define AFX_PACKAGE2SCROLLVIEW_H__ACB3B145_76AC_11D3_841A_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Package2ScrollView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackage2ScrollView view

class CPackage2ScrollView : public CScrollView
{
protected:
	CPackage2ScrollView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackage2ScrollView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackage2ScrollView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackage2ScrollView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPackage2ScrollView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGE2SCROLLVIEW_H__ACB3B145_76AC_11D3_841A_00104BCBA50D__INCLUDED_)
