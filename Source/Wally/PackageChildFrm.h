#if !defined(AFX_PACKAGECHILDFRM_H__89BCFFD4_8F73_11D2_BA96_00104BCBA50D__INCLUDED_)
#define AFX_PACKAGECHILDFRM_H__89BCFFD4_8F73_11D2_BA96_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackageChildFrm frame

class CPackageChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPackageChildFrm)
protected:
	CPackageChildFrm();           // protected constructor used by dynamic creation

// Private
private:
	CSplitterWnd m_wndSplitter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageChildFrm)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackageChildFrm();

	// Generated message map functions
	//{{AFX_MSG(CPackageChildFrm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGECHILDFRM_H__89BCFFD4_8F73_11D2_BA96_00104BCBA50D__INCLUDED_)
