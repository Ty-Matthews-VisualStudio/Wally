#if !defined(AFX_PAKLISTCTRL_H__7B9F9444_9249_11D3_A9FC_444553540000__INCLUDED_)
#define AFX_PAKLISTCTRL_H__7B9F9444_9249_11D3_A9FC_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPakListCtrl window

class CPakListCtrl : public CListCtrl
{
// Construction
public:
	CPakListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakListCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPakListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPakListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAKLISTCTRL_H__7B9F9444_9249_11D3_A9FC_444553540000__INCLUDED_)
