#ifndef _PACKAGEDOC_H_
#define _PACKAGEDOC_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageDoc.h : header file
//

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif	//_IMAGE_HELPER_H_

class CPackageView;
class CPackageBrowseView;
/////////////////////////////////////////////////////////////////////////////
// CPackageDoc document

class CPackageDoc : public CDocument
{
protected:
	CPackageDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPackageDoc)

// Members
private:
	LPBYTE m_pFileData;
	int m_iWADType;
	CRITICAL_SECTION m_CriticalSection;

public:
	CImageHelper m_ihHelper;

// Attributes
public:

// Operations
public:
	CPackageBrowseView *GetBrowseView();
	CPackageView *GetFormView();
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
	CWADItem *GetLumpAtPosition(int iPosition)
	{
		return m_ihHelper.GetLumpAtPosition(iPosition);
	}
	int GetNumImages()
	{
		return m_ihHelper.GetNumImages();
	}
	int GetNumLumps()
	{
		return m_ihHelper.GetNumLumps();
	}
	CWADList *GetWADList()
	{
		return m_ihHelper.GetWADList();
	}

	void OverridePathName( LPCTSTR szPath);
	void ReMipAll();	
	void ImportImage( LPCTSTR szPath, BOOL bForDecal = FALSE);
	BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	CWADItem *AddImage( LPBYTE pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight, BOOL bUpdateView, BOOL bSingleAccess = FALSE );
	CWADItem *AddImage (CWallyDoc *pWallyDoc, LPCTSTR szName, bool bUpdateView);
	void AddImage( CWADItem *pItem );
	void RemoveImage (CWADItem *pItem);
	void RemoveImage (LPCTSTR szName);
	CWADItem *ReplaceImage (unsigned char *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight );
	CWADItem *ReplaceImage (CWallyDoc *pWallyDoc, LPCTSTR szName);
	bool IsNameInList (LPCTSTR szName);	
	CWADItem *FindNameInList (LPCTSTR szName, BOOL bFailOnError = TRUE);
	void UpdateImageData (CWallyDoc *pWallyDoc);
	void RenameImage (CWallyDoc *pWallyDoc);
	void BreakDocConnection (CWallyDoc *pWallyDoc);	
	void UpdateAllOpenImages ();
	CPackageView *GetView();
	void UpdateViews( BOOL bRefreshList = TRUE );

	void LockPackage();
	void UnLockPackage();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackageDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackageDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PACKAGEDOC_H_
