#ifndef _PAKDOC_H_
#define _PAKDOC_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PakDoc.h : header file
//

#include "PakList.h"

// Custom message map
extern int WM_PAKDOC_CUSTOM;
#define PAK_DOC_MESSAGE_PASTE		0

/////////////////////////////////////////////////////////////////////////////
// CPakDoc document

class CPakDoc;

class CPakDocWnd : public CWnd
{
private:
	CPakDoc *m_pDoc;	

public:
	CPakDocWnd(CPakDoc *pDoc) : CWnd()
	{
		m_pDoc = pDoc;
	}

	protected:
	//{{AFX_MSG(CPakDocWnd)	
	afx_msg void OnPakDocCustomMessage(UINT nType, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
	

class CPakDoc : public CDocument
{
protected:
	CPakDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPakDoc)

// Members
private:
	CPakList m_PakList;
	CPakDirectory *m_pCurrentDirectory;
	CString m_strSaveAsSourcePAKFile;
	CString m_strSaveActualOutputPAKFile;
	int m_iSortOrder;
	int m_iModifiedFlag;
	BOOL m_bSaveAs;

	// CWnd is so that we can get custom messages (yay)
	CPakDocWnd *m_pWnd;
	
// Operations
public:	
	CPakItem *GetFirstPakItem();
	CPakItem *GetNextPakItem();
	void SetCurrentDirectory(CPakDirectory *pDirectory)
	{
		m_pCurrentDirectory = pDirectory;
		UpdateListView();
	}
	CPakDirectory *GetCurrentDirectory()
	{
		return m_pCurrentDirectory;
	}
	
	CWnd *GetWnd()
	{
		return m_pWnd;
	}
	CPakDirectory *GetRootDirectory();	
	void UpdateListView();
	void SetSortOrder (int iColumn)
	{
		m_iSortOrder = iColumn;
	}
	int GetSortOrder()
	{
		return m_iSortOrder;
	}

	void OpenItem (CPakItem *pItem);
	void DeleteEntry (CPakItem *pItem);
	void DeleteEntry (CPakDirectory *pDirectory);
	void PasteFiles();
	BOOL AddFile (LPCTSTR szFileName, BOOL bMoreFiles = FALSE);
	void ImportFiles();
	BOOL ExportFiles(BOOL bHasSelections, CView *pCaller);
	
	void BuildTree();
	void HighlightDirectory(LPCTSTR szPath, BOOL bExpand = FALSE);
	void ReleaseFile(CFile* pFile, BOOL bAbort);
	CPakList *GetPakList()
	{
		return &m_PakList;
	}
	BOOL IsModified();
	CString GetPakFileName();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPakDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPakDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPakDoc)	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PAKDOC_H_
