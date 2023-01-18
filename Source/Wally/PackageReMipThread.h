#if !defined(AFX_PACKAGEREMIPTHREAD_H__BB1631A2_DC9C_11D2_BAFB_00104BCBA50D__INCLUDED_)
#define AFX_PACKAGEREMIPTHREAD_H__BB1631A2_DC9C_11D2_BAFB_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageReMipThread.h : header file
//


#define PACKAGE_REMIP_ALL		0
#define PACKAGE_REMIP_SELECTED	1


/////////////////////////////////////////////////////////////////////////////
// CPackageReMipThread thread

class CPackageReMipThread : public CWinThread
{
	DECLARE_DYNCREATE(CPackageReMipThread)
protected:
	CPackageReMipThread();           // protected constructor used by dynamic creation

// Attributes
public:
	static void MainLoop( LPVOID pParam );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageReMipThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPackageReMipThread();

	// Generated message map functions
	//{{AFX_MSG(CPackageReMipThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGEREMIPTHREAD_H__BB1631A2_DC9C_11D2_BAFB_00104BCBA50D__INCLUDED_)
