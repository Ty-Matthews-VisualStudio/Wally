#ifndef _BROWSECHILDFRM_H__
#define _BROWSECHILDFRM_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BrowseChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrowseChildFrm frame

class CBrowseChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBrowseChildFrm)
protected:
	CBrowseChildFrm();           // protected constructor used by dynamic creation

// Private
private:
	CSplitterWnd m_wndSplitter;

// Operations
public:
	void SaveWindowState();
	BOOL RestoreWindowState();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseChildFrm)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBrowseChildFrm();

	// Generated message map functions
	//{{AFX_MSG(CBrowseChildFrm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _BROWSECHILDFRM_H__
