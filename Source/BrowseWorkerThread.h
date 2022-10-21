#ifndef _BROWSEWORKERTHREAD_H__
#define _BROWSEWORKERTHREAD_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BrowseWorkerThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CBrowseWorkerThread thread

class CBrowseWorkerThread : public CWinThread
{
	DECLARE_DYNCREATE(CBrowseWorkerThread)
protected:
	CBrowseWorkerThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	static void MainLoop (LPVOID pParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseWorkerThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	~CBrowseWorkerThread();

	// Generated message map functions
	//{{AFX_MSG(CBrowseWorkerThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _BROWSEWORKERTHREAD_H__
