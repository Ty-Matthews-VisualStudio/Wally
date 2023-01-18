// PackageDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PackageDoc.h"
#include "PackageView.h"
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
// CPackageDoc

IMPLEMENT_DYNCREATE(CPackageDoc, CDocument)

CPackageDoc::CPackageDoc()
{
	m_pFileData = NULL;

	InitializeCriticalSection( &m_CriticalSection );
}

BOOL CPackageDoc::OnNewDocument()
{	
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPackageDoc::~CPackageDoc()
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
	
	DeleteCriticalSection( &m_CriticalSection );
}


BEGIN_MESSAGE_MAP(CPackageDoc, CDocument)
	//{{AFX_MSG_MAP(CPackageDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageDoc diagnostics

#ifdef _DEBUG
void CPackageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPackageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackageDoc serialization

void CPackageDoc::Serialize(CArchive& ar)
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
		return;
	}		
}


/////////////////////////////////////////////////////////////////////////////
// CPackageDoc commands

////////////////////////////////////////////////////////////////////////////////////////////
CWADItem *CPackageDoc::AddImage( LPBYTE pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight, BOOL bUpdateView, BOOL bSingleAccess /* = FALSE */ )
{
	CWADItem *pWADItem = NULL;
	
	__try
	{
		EnterCriticalSection( &m_CriticalSection );
	
		pWADItem = m_ihHelper.AddImageToWAD (pbyBits, pPalette, szName, iWidth, iHeight);		

		if (bUpdateView)
		{
			// This is for the batch conversion, which adds images to the package
			// without having contact with the view class.
			CPackageView *pView = GetFormView();
		
			if (pView)
			{
				pView->AddString (pWADItem, szName, false);		
			}
		}
		
		SetModifiedFlag (true);
	}
	__finally
	{
		LeaveCriticalSection( &m_CriticalSection );
	}

	
	if (bUpdateView)
	{
		CPackageBrowseView *pBrowseView = GetBrowseView();

		if( pBrowseView )
		{
			pBrowseView->InvalidateRect( NULL, FALSE );
		}
	}
	
	return pWADItem;
}

CWADItem *CPackageDoc::AddImage (CWallyDoc *pWallyDoc, LPCTSTR szName, bool bUpdateView)
{
	ASSERT (pWallyDoc);

	LPBYTE pbyBits[4];
	int k = 0;
	for( k = 0; k < 4; k++ )
	{
		pbyBits[k] = pWallyDoc->GetBits(k);
	}

	int iWidth = pWallyDoc->Width();
	int iHeight = pWallyDoc->Height();

	CWADItem *pWADItem = m_ihHelper.AddImageToWAD (pbyBits, pWallyDoc->GetPalette(), szName, iWidth, iHeight);

	CPackageView *pView = GetFormView();
	
	if (pView)
	{
		pView->AddString (pWADItem, szName, bUpdateView);
	}

	for (k = 0; k < 4; k++)
	{
		pbyBits[k] = NULL;
	}
	return pWADItem;
}

void CPackageDoc::AddImage( CWADItem *pItem )
{	
	if( !pItem )
	{
		ASSERT( FALSE );
		return;
	}
	else
	{
		
		m_ihHelper.AddImageToWAD( pItem );

		CPackageView *pView = GetFormView();
		if (pView)
		{
			pView->AddString( pItem, pItem->GetName(), false);			
		}

		CPackageBrowseView *pBrowseView = GetBrowseView();

		if( pBrowseView )
		{
			pBrowseView->InvalidateRect( NULL, FALSE );
		}
	}
	
}

void CPackageDoc::RemoveImage (CWADItem *pItem)
{
	m_ihHelper.RemoveImageFromWAD (pItem);
	SetModifiedFlag (true);
}

void CPackageDoc::RemoveImage (LPCTSTR szName)
{
	CWADItem *pItem = m_ihHelper.IsNameInList(szName);
	ASSERT (pItem);	// It'd better be in there!

	if (pItem)
	{
		m_ihHelper.RemoveImageFromWAD (pItem);
		SetModifiedFlag (true);
	}	
}

CWADItem *CPackageDoc::FindNameInList (LPCTSTR szName, BOOL bFailOnError /* = TRUE */)
{
	CWADItem *pItem = m_ihHelper.IsNameInList(szName);

	if (bFailOnError)
	{
		ASSERT (pItem);	// It'd better be in there!
	}
	return pItem;	
}

CWADItem *CPackageDoc::ReplaceImage (CWallyDoc *pWallyDoc, LPCTSTR szName)
{
	unsigned char *pbyBits[4];
	CWallyPalette *pPalette = pWallyDoc->GetPalette();
	int iWidth	= pWallyDoc->GetWidth();
	int iHeight = pWallyDoc->GetHeight();

	for (int j = 0; j < 4; j++)
	{
		pbyBits[j] = pWallyDoc->GetBits(j);
	}
	
	return ReplaceImage (pbyBits, pPalette, szName, iWidth, iHeight);
}

void CPackageDoc::LockPackage()
{
	EnterCriticalSection( &m_CriticalSection );	
}

void CPackageDoc::UnLockPackage()
{
	LeaveCriticalSection( &m_CriticalSection );
}

#if 0
{
	__try
	{
		EnterCriticalSection( &m_CriticalSection );
	}
	__finally
	{
		LeaveCriticalSection( &m_CriticalSection );
	}
}
#endif

bool CPackageDoc::IsNameInList (LPCTSTR szName)
{
	bool bReturn = FALSE;
	if (!szName)
	{
		return false;
	}
	
	__try
	{
		EnterCriticalSection( &m_CriticalSection );

		bReturn = m_ihHelper.IsNameInList(szName) != NULL;
	}
	__finally
	{
		LeaveCriticalSection( &m_CriticalSection );
	}

	return bReturn;
}

CWADItem *CPackageDoc::ReplaceImage (unsigned char *pbyBits[], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight )
{
	CWADItem *pItem = NULL;

	__try
	{
		EnterCriticalSection( &m_CriticalSection );
		
		pItem = m_ihHelper.IsNameInList(szName);
		ASSERT (pItem);	// It'd better be in there!

		if (pItem)
		{
			pItem->ReplaceData( pbyBits, pPalette, szName, iWidth, iHeight );
			SetModifiedFlag( true );			
		}
	}
	__finally
	{
		LeaveCriticalSection( &m_CriticalSection );
	}
	
	return pItem;
}

//  Name:		DoSave
//	Action:		Handles the saving of files, overridden here to trap the directory
//				that the file was saved to.  This code came straight out of doccore.cpp

BOOL CPackageDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
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

void CPackageDoc::UpdateAllOpenImages ()
{	
	CString strPath (GetPathName());
	CPackageView *pView = GetFormView();
	
	if (pView)
	{		
		pView->UpdateAllOpenImages(strPath);
	}
}

CPackageView *CPackageDoc::GetView()
{	
	POSITION Pos = GetFirstViewPosition();	
	CPackageView *pView = (CPackageView *)GetNextView( Pos);
	return pView;
}

void CPackageDoc::UpdateViews( BOOL bRefreshList /* = TRUE */ )
{
	CPackageView *pView = GetFormView();

	if (pView)
	{		
		if( bRefreshList )
		{
			pView->ResetListBox();
			CWADItem *pItem = GetFirstImage();

			while( pItem )
			{
				pView->AddString( pItem, pItem->GetName(), FALSE );
				pItem = GetNextImage();
			}
		}
	
	
		pView->InvalidateRect( NULL, FALSE );
	}

	CPackageBrowseView *pBrowseView = GetBrowseView();

	if( pBrowseView )
	{
		pBrowseView->InvalidateRect( NULL, FALSE );
	}
}

////////////////////////////////////////////////////////////////////////
//	Name:	UpdateImageData()
//	Action:	Called by CWallyDoc::SerializeHalfLife() to update the image data
////////////////////////////////////////////////////////////////////////
void CPackageDoc::UpdateImageData (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);

	SetModifiedFlag (true);

	CPackageView *pView = GetFormView();
	
	if (pView)
	{		
		pView->UpdateImageData(pWallyDoc);
	}	
}

CPackageBrowseView *CPackageDoc::GetBrowseView()
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

CPackageView *CPackageDoc::GetFormView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);
		
		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPackageView)))
			{
				return (CPackageView *)pView;
			}
		}
	}
	return NULL;
}

void CPackageDoc::BreakDocConnection (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);

	CPackageView *pView = GetFormView();
	
	if (pView)
	{
		pView->BreakDocConnection(pWallyDoc);
	}	
}

void CPackageDoc::RenameImage (CWallyDoc *pWallyDoc)
{
	ASSERT (pWallyDoc);
	
	SetModifiedFlag (true);

	CPackageView *pView = GetFormView();
	
	if (pView)
	{
		pView->RenameImage(pWallyDoc);
	}		
}


void CPackageDoc::OverridePathName( LPCTSTR szPath)
{
	m_strPathName = szPath;
}

void CPackageDoc::ReMipAll()
{
	CPackageView *pView = GetFormView();
	
	if (pView)
	{
		pView->ReMipAll();
	}
}

void CPackageDoc::ImportImage( LPCTSTR szPath, BOOL bForDecal /* = FALSE */)
{
	CPackageView *pView = GetFormView();
	
	if (pView)
	{
		pView->ImportImage( szPath, bForDecal);
	}
}