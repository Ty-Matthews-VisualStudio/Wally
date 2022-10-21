#ifndef _PACKAGE2DOC_H_
#define _PACKAGE2DOC_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Package2Doc.h : header file
//

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif	//_IMAGE_HELPER_H_

class CPackageBrowseView;
class CPackage2FormView;

/////////////////////////////////////////////////////////////////////////////
// CPackage2Doc document

class CPackage2Doc : public CDocument
{
protected:
	CPackage2Doc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackage2Doc)

// Members
private:
	BYTE *m_pbyFileData;
	int m_iWADType;

public:
	CImageHelper m_ihHelper;

// Operations
public:
	CPackageBrowseView *GetScrollView();
	CPackage2FormView *GetFormView();
	void SetWADType (int iType)
	{
		m_iWADType = iType;
		m_ihHelper.SetWADType (iType);
	}
	int GetWADType ()
	{
		return m_iWADType;
	}
	CString GetPathName()
	{
		return m_strPathName;
	}
	CWADItem *GetFirstImage()
	{		
		return m_ihHelper.GetFirstImage();
	}
	CWADItem *GetNextImage()
	{
		return m_ihHelper.GetNextImage();
	}
	CWADItem *GetFirstLump()
	{		
		return m_ihHelper.GetFirstLump();
	}
	CWADItem *GetNextLump()
	{		
		return m_ihHelper.GetNextLump();
	}
	BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	CWADItem *AddImage (BYTE *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight, bool bUpdateView);
	CWADItem *AddImage (CWallyDoc *pWallyDoc, LPCTSTR szName, bool bUpdateView);
	void RemoveImage (CWADItem *pItem);
	void RemoveImage (LPCTSTR szName);
	CWADItem *ReplaceImage (BYTE *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight);
	CWADItem *ReplaceImage (CWallyDoc *pWallyDoc, LPCTSTR szName);
	bool IsNameInList (LPCTSTR szName)
	{
		if (!szName)
		{
			return false;
		}
		return (m_ihHelper.IsNameInList (szName) != NULL);
	}
	CWADItem *FindNameInList (LPCTSTR szName);
	void UpdateImageData (CWallyDoc *pWallyDoc);
	void RenameImage (CWallyDoc *pWallyDoc);
	void BreakDocConnection (CWallyDoc *pWallyDoc);	
	void UpdateAllOpenImages ();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackage2Doc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackage2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackage2Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _PACKAGE2DOC_H_
