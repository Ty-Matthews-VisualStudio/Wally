#if !defined(AFX_BROWSERCACHETHREAD_H__3D6B1C63_5412_11D3_BB9C_00104BCBA50D__INCLUDED_)
#define AFX_BROWSERCACHETHREAD_H__3D6B1C63_5412_11D3_BB9C_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BrowserCacheThread.h : header file
//

#include "BrowserCacheList.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowserCacheThread thread

class CBrowserCacheThread : public CWinThread
{
	DECLARE_DYNCREATE(CBrowserCacheThread)
protected:
	CBrowserCacheThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	static void MainLoop (LPVOID pParam);
	static void CleanupProc (CBrowserCacheList *pCacheList);
	static void GroomCacheFiles (CBrowserCacheList *pCacheList);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserCacheThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBrowserCacheThread();

	// Generated message map functions
	//{{AFX_MSG(CBrowserCacheThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERCACHETHREAD_H__3D6B1C63_5412_11D3_BB9C_00104BCBA50D__INCLUDED_)
