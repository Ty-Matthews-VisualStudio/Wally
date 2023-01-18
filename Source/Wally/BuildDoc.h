#ifndef _BUILD_DOC_H_
#define _BUILD_DOC_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BuildDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBuildDoc document

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

class CBuildItem;

class CBuildDoc : public CDocument
{
protected:
	CBuildDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBuildDoc)

// Attributes
private:
	CImageHelper m_ihHelper;

// Operations
public:	
	CBuildItem *GetBuildTile(int iTileNumber);
	void ValidateTileNumber (int *pTile);
	int GetFirstBuildTile ();
	int GetLastBuildTile ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBuildDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBuildDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef _BUILD_DOC_H_
