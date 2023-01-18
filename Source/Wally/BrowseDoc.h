#ifndef _BROWSEDOC_H__
#define _BROWSEDOC_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BrowseDoc.h : header file
//

// Current state
#define BROWSE_DOC_THREAD_IDLE			0
#define BROWSE_DOC_THREAD_STOPPED		1
#define BROWSE_DOC_THREAD_INIT			2

// Messages
#define BROWSE_DOC_MESSAGE_NONE			0
#define BROWSE_DOC_MESSAGE_STOP			1
#define BROWSE_DOC_MESSAGE_GETFILES		2
#define BROWSE_DOC_MESSAGE_REFRESH		3

/////////////////////////////////////////////////////////////////////////////
// CBrowseDoc document

class CBrowseWorkerThread;

#ifndef _SORTSTRINGARRAY_H__
	#include "SortStringArray.h"
#endif

#ifndef _BROWSERCACHELIST_H__
	#include "BrowserCacheList.h"
#endif

class CBrowseDoc : public CDocument
{
protected:
	CBrowseDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrowseDoc)

// Members
private:
	CString m_strPath;
	CStringArray m_strArray;
	CString m_strCacheFileName;
	CSortStringArray m_saCacheFiles;

	CBrowserCacheList m_BrowserCacheList;
	
	CBrowseWorkerThread *m_pWorkerThread;
	int m_iThreadStatus;
	int m_iThreadMessage;
	

// Operations
public:
	void SetPath (LPCTSTR szPath);
	void UpdateExplorer(LPCTSTR szPath);
	void RefreshView();

	CBrowserCacheList *GetCacheList()
	{
		return &m_BrowserCacheList;
	}
	int GetItemCount()
	{
		return m_BrowserCacheList.GetCount();
	}
	void ResetScrollbars(BOOL bReset = TRUE);

	CString GetPath();
	CStringArray *GetArray();
	int GetThreadStatus();
	void SetThreadStatus (int iStatus);
	int GetThreadMessage();
	void SetThreadMessage (int iMessage);

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowse2Doc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowseDoc)
	afx_msg void OnFileRefresh();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _BROWSEDOC_H__
