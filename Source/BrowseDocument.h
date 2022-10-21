#if !defined(AFX_BROWSEDOCUMENT_H__332EA742_A2B9_11D1_B1D1_0000C07E139E__INCLUDED_)
#define AFX_BROWSEDOCUMENT_H__332EA742_A2B9_11D1_B1D1_0000C07E139E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BrowseDIBList.h"
#include "DIBToDocHelper.h"

// BrowseDocument.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBrowseDocument document

class CBrowseDocument : public CDocument
{
protected:
	CBrowseDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBrowseDocument)


// Members
public:
	CString m_BrowseDirectory;	
	CBrowseDIBList DIBList;
	CDIBToDocHelper DocHelper;

// Attributes
public:

// Operations
public:
	void OpenWallyDocument(int TextureNumber);	
	void RefreshView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowseDocument)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBrowseDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowseDocument)
	afx_msg void OnFileSelectFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSEDOCUMENT_H__332EA742_A2B9_11D1_B1D1_0000C07E139E__INCLUDED_)
