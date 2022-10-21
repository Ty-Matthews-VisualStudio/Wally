#if !defined(AFX_DEBUGDOC_H__0CA8E920_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_)
#define AFX_DEBUGDOC_H__0CA8E920_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DebugDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugDoc document

class CDebugDoc : public CDocument
{
protected:
	CDebugDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDebugDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDebugDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDebugDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGDOC_H__0CA8E920_A78D_11D3_A9FC_0000C04D1FBF__INCLUDED_)
