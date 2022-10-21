// Package2Doc.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "Package2Doc.h"
#include "Package2FormView.h"
#include "PackageBrowseView.h"
#include "ImageHelper.h"

#ifndef _WALLYDOC_H_
	#include "WallyDoc.h"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackage2Doc

IMPLEMENT_DYNCREATE(CPackage2Doc, CDocument)

CPackage2Doc::CPackage2Doc()
{
	m_pbyFileData = NULL;
}

BOOL CPackage2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPackage2Doc::~CPackage2Doc()
{
	CWADItem *pItem = m_ihHelper.GetFirstImage();
	CWallyDoc *pWallyDoc = NULL;

	while (pItem)
	{
		pWallyDoc = pItem->GetWallyDoc();
		if (pWallyDoc)
		{
			pWallyDoc->DisconnectPackage (false);
		}
		pItem = m_ihHelper.GetNextImage();
	}
	
	pItem = NULL;
	pWallyDoc = NULL;
}


BEGIN_MESSAGE_MAP(CPackage2Doc, CDocument)
	//{{AFX_MSG_MAP(CPackage2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackage2Doc diagnostics

#ifdef _DEBUG
void CPackage2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPackage2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackage2Doc serialization

void CPackage2Doc::Serialize(CArchive& ar)
{
	CFile* pFile = ar.GetFile();

	if (ar.IsStoring())
	{
		m_ihHelper.SaveWAD(pFile);
	}
	else
	{
		BeginWaitCursor();
		m_ihHelper.LoadImage (pFile);
		m_iWADType = m_ihHelper.GetImageType() == IH_WAD2_TYPE ? WAD2_TYPE : WAD3_TYPE;
		EndWaitCursor();
	}

	if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		AfxMessageBox (m_ihHelper.GetErrorText(), MB_ICONSTOP);		
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPackage2Doc commands
CPackageBrowseView *CPackage2Doc::GetScrollView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);
		
		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPackageBrowseView)))
			{
				return (CPackageBrowseView *)pView;				
			}
		}
	}
	return NULL;
}

CPackage2FormView *CPackage2Doc::GetFormView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);
		
		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPackage2FormView)))
			{
				return (CPackage2FormView *)pView;
			}
		}
	}
	return NULL;
}

CWADItem *CPackage2Doc::AddImage (BYTE *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight, bool bUpdateView)
{
	CWADItem *pWADItem = m_ihHelper.AddImageToWAD (pbyBits, pPalette, szName, iWidth, iHeight);	
	
	if (bUpdateView)
	{		
		// This is for the batch conversion, which adds images to the package
		// without having contact with the view class.
		CPackage2FormView *pFormView = GetFormView();

		if (pFormView)
		{			
			//pFormView->AddString (pWADItem, szName, false);
		}
	}
	SetModifiedFlag (true);
	return pWADItem;
}

CWADItem *CPackage2Doc::AddImage (CWallyDoc *pWallyDoc, LPCTSTR szName, bool bUpdateView)
{
	ASSERT (pWallyDoc);

	BYTE *pbyBits[4];
	for (int k = 0; k < 4; k++)
	{
		pbyBits[k] = pWallyDoc->GetBits(k);
	}

	int iWidth = pWallyDoc->Width();
	int iHeight = pWallyDoc->Height();

	CWADItem *pWADItem = m_ihHelper.AddImageToWAD (pbyBits, pWallyDoc->GetPalette(), szName, iWidth, iHeight);
	
/*	POSITION Pos = GetFirstViewPosition();	
	while (Pos != NULL)
	{
		CPackageView *pView = (CPackageView *)GetNextView( Pos);
		pView->AddString (pWADItem, szName, bUpdateView);
	}
	*/

	for (k = 0; k < 4; k++)
	{
		pbyBits[k] = NULL;
	}
	return pWADItem;
}

void CPackage2Doc::RemoveImage (CWADItem *pItem)
{
	m_ihHelper.RemoveImageFromWAD (pItem);
	SetModifiedFlag (true);
}

void CPackage2Doc::RemoveImage (LPCTSTR szName)
{
	CWADItem *pItem = m_ihHelper.IsNameInList(szName);
	ASSERT (pItem);	// It'd better be in there!

	if (pItem)
	{
		m_ihHelper.RemoveImageFromWAD (pItem);
		SetModifiedFlag (true);
	}	
}

CWADItem *CPackage2Doc::FindNameInList (LPCTSTR szName)
{
	CWADItem *pItem = m_ihHelper.IsNameInList(szName);
	ASSERT (pItem);	// It'd better be in there!
	return pItem;	
}

CWADItem *CPackage2Doc::ReplaceImage (CWallyDoc *pWallyDoc, LPCTSTR szName)
{
	BYTE *pbyBits[4];
	CWallyPalette *pPalette = pWallyDoc->GetPalette();
	int iWidth	= pWallyDoc->GetWidth();
	int iHeight = pWallyDoc->GetHeight();

	for (int j = 0; j < 4; j++)
	{
		pbyBits[j] = pWallyDoc->GetBits(j);
	}
	
	return ReplaceImage (pbyBits, pPalette, szName, iWidth, iHeight);
}

CWADItem *CPackage2Doc::ReplaceImage (BYTE *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight)
{
	CWADItem *pItem = m_ihHelper.IsNameInList(szName);
	ASSERT (pItem);	// It'd better be in there!

	if (pItem)
	{
		pItem->ReplaceData (pbyBits, pPalette, szName, iWidth, iHeight);
		SetModifiedFlag (true);
		return pItem;
	}
	
	return NULL;
}

//  Name:		DoSave
//	Action:		Handles the saving of files, overridden here to trap the directory
//				that the file was saved to.  This code came straight out of doccore.cpp

BOOL CPackage2Doc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{	

	if (m_ihHelper.GetNumLumps() == 0)
	{
		AfxMessageBox ("There are no images in this WAD!", MB_ICONSTOP);
		return false;
	}
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{		
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
#ifndef _MAC
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
#else
			int iBad = newName.FindOneOf(_T(":"));
#endif
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

#ifndef _MAC
			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
#endif
		}	
		
		// This is the custom stuff:		
		// Build some strings based on the String Table entries
		CString strWildCard("");
		switch (GetWADType())
		{
		case WAD2_TYPE:
			strWildCard = "Quake1 Package (*.wad)|*.wad||";
			break;

		case WAD3_TYPE:
			strWildCard = "Half-Life Package (*.wad)|*.wad||";
			break;

		default:
			ASSERT (false);
			break;
		}
		
		CString strTitle("Save As");
		CString strAppendExtension("wad");
	
		// Create a CFileDialog, init with our strings
		CFileDialog	dlgSave (FALSE, strAppendExtension, newName, 
			OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, strWildCard, NULL);
	
		// Set some of the CFileDialog vars
		if (g_strFileSaveDirectory !=  "")
		{
			dlgSave.m_ofn.lpstrInitialDir = g_strFileSaveDirectory;
		}
		dlgSave.m_ofn.lpstrTitle = strTitle;	
		
		if (dlgSave.DoModal() == IDOK)
		{
			g_strFileSaveDirectory = dlgSave.GetPathName().Left(dlgSave.m_ofn.nFileOffset);
			newName = dlgSave.GetPathName();
			
			int iExtensionMarker = dlgSave.m_ofn.nFileExtension;
			int iFileNameLength = newName.GetLength();

			// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
			// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
			// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
			// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
			// We have to check for each instance and add the extension, as required.

			if ((newName.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
			{
				newName += ".";
			}

			if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
			{				
				// User didn't type in the extension
				newName += "wad";
			}			
		}
		else
		{
			return FALSE;  // don't even attempt to save
		}		
		
	
	
		//  Original code follows:
//		if (!AfxGetApp()->DoPromptFileName(newName,
//		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
//		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
//			return FALSE;        
	}

	// Nothing past here has been changed

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				e->Delete();
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
	{
		SetPathName(newName);
		UpdateAllOpenImages ();
	}

	return TRUE;        // success
}

void CPackage2Doc::UpdateAllOpenImages ()
{	
	CString strPath = GetPathName();
	CPackage2FormView *pFormView = GetFormView();
	//pFormView->UpdateAllOpenImages(strPath);
}

////////////////////////////////////////////////////////////////////////
//	Name:	UpdateImageData()
//	Action:	Called by CWallyDoc::SerializeHalfLife() to update the image data
////////////////////////////////////////////////////////////////////////
void CPackage2Doc::UpdateImageData (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);

	SetModifiedFlag (TRUE);

	CPackage2FormView *pFormView = GetFormView();
	//pFormView->UpdateImageData(pWallyDoc);	
}

void CPackage2Doc::BreakDocConnection (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);
	CPackage2FormView *pFormView = GetFormView();
	//pFormView->BreakDocConnection(pWallyDoc);		
}

void CPackage2Doc::RenameImage (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);
	
	SetModifiedFlag (true);

	CPackage2FormView *pFormView = GetFormView();
	//pFormView->RenameImage(pWallyDoc);		
}
