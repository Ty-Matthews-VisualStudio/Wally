#ifndef _PAKCHILDFRM_H_
#define _PAKCHILDFRM_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPakChildFrm frame

class CPakChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPakChildFrm)
protected:
	CPakChildFrm();           // protected constructor used by dynamic creation

// Private
private:
	CSplitterWnd m_wndSplitter;

// Operations
public:	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakChildFrm)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPakChildFrm();

	// Generated message map functions
	//{{AFX_MSG(CPakChildFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKCHILDFRM_H_
