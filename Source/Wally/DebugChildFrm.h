#if !defined(AFX_DEBUGCHILDFRM_H__0CA8E921_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_)
#define AFX_DEBUGCHILDFRM_H__0CA8E921_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DebugChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugChildFrm frame

class CDebugChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDebugChildFrm)
protected:
	CDebugChildFrm();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugChildFrm)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDebugChildFrm();

	// Generated message map functions
	//{{AFX_MSG(CDebugChildFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGCHILDFRM_H__0CA8E921_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_)
