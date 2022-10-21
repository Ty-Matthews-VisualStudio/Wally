#if !defined(AFX_WLYDOC_H__95F50184_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_)
#define AFX_WLYDOC_H__95F50184_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WLYDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWLYDoc document

class CWLYDoc : public CDocument
{
protected:
	CWLYDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWLYDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWLYDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWLYDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWLYDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WLYDOC_H__95F50184_EC25_11D2_BB1B_00104BCBA50D__INCLUDED_)
