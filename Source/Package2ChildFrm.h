#ifndef _PACKAGE2CHILDFRM_H_
#define _PACKAGE2CHILDFRM_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Package2ChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackage2ChildFrm frame

class CPackage2ChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPackage2ChildFrm)
protected:
	CPackage2ChildFrm();           // protected constructor used by dynamic creation

// Private
private:
	CSplitterWnd m_wndSplitter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackage2ChildFrm)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackage2ChildFrm();

	// Generated message map functions
	//{{AFX_MSG(CPackage2ChildFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PACKAGE2CHILDFRM_H_
