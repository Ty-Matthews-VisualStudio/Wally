#if !defined(AFX_WLYVIEW_H__95F50183_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_)
#define AFX_WLYVIEW_H__95F50183_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WLYView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWLYView view

class CWLYView : public CScrollView
{
protected:
	CWLYView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWLYView)

// Members
private:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWLYView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWLYView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWLYView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WLYVIEW_H__95F50183_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_)
