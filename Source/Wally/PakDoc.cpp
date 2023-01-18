// PakDoc.cpp : implementation file
//

// This is for wide-character string support:
//#include <wchar.h>

#include "stdafx.h"
#include "wally.h"
#include "PakDoc.h"
#include "PakList.h"
#include "PakListView.h"
#include "MiscFunctions.h"
#include "PakTreeView.h"
#include "PakImportDlg.h"
#include "PakExportDlg.h"
#include "DirectoryList.h"
//#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_PAKDOC_CUSTOM	= RegisterWindowMessage ("WM_PAKDOC_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CPakDoc

IMPLEMENT_DYNCREATE(CPakDoc, CDocument)

CPakDoc::CPakDoc()
{	
	SetSortOrder(0);

	m_pWnd = new CPakDocWnd(this);
	CRect rcClient (0, 0, 0, 0);
	if (!m_pWnd->Create (NULL, "PakDocWnd", WS_DISABLED, rcClient, AfxGetMainWnd(), 0))
	{
		ASSERT (FALSE);
	}

	m_iModifiedFlag = SERIALIZE_MODIFIED_UNKNOWN;

	SetCurrentDirectory (m_PakList.GetRootDirectory());
}

BOOL CPakDoc::OnNewDocument()
{	
	if (!CDocument::OnNewDocument())
		return FALSE;	
	return TRUE;
}

CPakDoc::~CPakDoc()
{
	if (m_pWnd)
	{
		delete m_pWnd;
		m_pWnd = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPakDoc, CDocument)
	//{{AFX_MSG_MAP(CPakDoc)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CPakDocWnd, CWnd)
	//{{AFX_MSG_MAP(CPakDocWnd)
	ON_REGISTERED_MESSAGE(WM_PAKDOC_CUSTOM, OnPakDocCustomMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPakDoc diagnostics

#ifdef _DEBUG
void CPakDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPakDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPakDoc serialization

void CPakDoc::Serialize(CArchive& ar)
{	
	CFile *pFile = ar.GetFile();
	int iFlags = 0;

	if (ar.IsStoring())
	{
		// Go grab the modified flag
		IsModified();

		iFlags = SERIALIZE_WRITE | m_iModifiedFlag;
		if( m_bSaveAs )
		{
			iFlags |= SERIALIZE_SAVE_AS;
		}
		
		if( !m_PakList.Serialize (pFile, m_strSaveActualOutputPAKFile, iFlags))
		{
			AfxMessageBox (m_PakList.GetError());
		}		
	}
	else
	{		
		if (!m_PakList.Serialize (pFile, m_strSaveActualOutputPAKFile, SERIALIZE_READ))
		{
			AfxMessageBox (m_PakList.GetError());
		}
		/*else
		{
			SetCurrentDirectory (m_PakList.GetRootDirectory());
		}
		*/
	}
}

BOOL CPakDoc::IsModified()
{
	ASSERT (this != NULL);
	
	m_iModifiedFlag ^= SERIALIZE_MODIFIED_UNKNOWN;
	
	return m_PakList.IsModified(&m_iModifiedFlag);
}
	

/////////////////////////////////////////////////////////////////////////////
// CPakDoc commands

CPakItem *CPakDoc::GetFirstPakItem()
{	
	return m_pCurrentDirectory->GetFirstPakItem();
}

CPakItem *CPakDoc::GetNextPakItem()
{	
	return m_pCurrentDirectory->GetNextPakItem();	
}

CPakDirectory *CPakDoc::GetRootDirectory()
{
	return m_PakList.GetRootDirectory();
}

void CPakDoc::UpdateListView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);

		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPakListView)))
			{			
				((CPakListView *)(pView))->UpdateList();
			}
		}
	}
}

void CPakDoc::OpenItem (CPakItem *pItem)
{
	if (!m_PakList.OpenItem (pItem))
	{
		AfxMessageBox (m_PakList.GetError(), MB_ICONSTOP);
	}
}

void CPakDoc::PasteFiles()
{	
	if (::IsClipboardFormatAvailable(CF_HDROP))
	{		
		CWnd *pWnd = NULL;

		POSITION Pos = GetFirstViewPosition();
	
		while (Pos != NULL)
		{
			CView *pView = GetNextView(Pos);

			if (pView)
			{
				pWnd = (CWnd *)pView;
			}
		}
		
		if (!pWnd)
		{
			AfxMessageBox ("Failed to open the clipboard.", MB_ICONSTOP);			
			return;
		}		

		if (pWnd->OpenClipboard())
		{
			HGLOBAL hgData = ::GetClipboardData(CF_HDROP);
			if (!hgData)
			{
				AfxMessageBox ("Error reading from clipboard", MB_ICONSTOP);
				return;
			}

			// Determine the data size
			int iDataSize = GlobalSize(hgData);
						
			// Alloc memory
			BYTE *pbyData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

			if (!pbyData)
			{
				AfxMessageBox ("Ran out of memory trying to access clipboard", MB_ICONSTOP);
				return;
			}

			// Grab the data					
			CopyMemory(pbyData, GlobalLock(hgData), iDataSize); 
			
			// Get out as fast as we can from the clipboard
			GlobalUnlock(hgData);
			CloseClipboard();

			LPDROPFILES pDropFiles;
		
			pDropFiles = (LPDROPFILES)pbyData;

			// find the offset where the starting point of the files start
			int iCurOffset = pDropFiles->pFiles;

			// copy the filename
			CString strFileName("");			
			BOOL bDone = FALSE;
			BOOL bUpdateView = FALSE;

			while (!bDone)
			{
				if (*(pbyData + iCurOffset) == 0)
				{
					bDone = TRUE;
				}
				else
				{
					if (pDropFiles->fWide)
					{
						wchar_t *szWideString = (wchar_t *)(pbyData + iCurOffset);
						strFileName = szWideString;

						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += (strFileName.GetLength() + 1) * 2;						
					}
					else
					{
						strFileName = (LPSTR) ((LPSTR)(pDropFiles) + iCurOffset);
						
						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += strFileName.GetLength() + 1;
					}
					
					// Add it to the directory
					if (AddFile (strFileName, (*(pbyData + iCurOffset) != 0)))
					{
						bUpdateView = TRUE;
					}
				}
			}			
						
			if (pbyData)
			{
				GlobalFree ((HGLOBAL) pbyData);
				pbyData = NULL;
			}

			if (bUpdateView)
			{
				UpdateListView();
			}
		}					// if (pWnd->OpenClipboard())
	}						// if (::IsClipboardFormatAvailable(CF_HDROP))
}

BOOL CPakDoc::AddFile (LPCTSTR szFileName, BOOL bMoreFiles /* = FALSE */)
{	
	// If it's a directory, we want to add all of the files underneath it	
	CFileStatus status;
	
	if (CFile::GetStatus (szFileName, status))
	{
		if (status.m_attribute & CFile::directory)
		{
			CStringArray saWildcards;
			saWildcards.Add ("*.*");

			if (!m_pCurrentDirectory->AddFileDirectory (szFileName, &saWildcards, PAK_ADD_FLAG_RECURSE))
			{
				AfxMessageBox (m_pCurrentDirectory->GetError(), MB_ICONSTOP);
				return FALSE;
			}			
		}
		else
		{
			// Add it to the directory
			if (!m_pCurrentDirectory->AddFile (szFileName, NULL))
			{
				if (m_pCurrentDirectory->GetErrorCode() == PAK_ERROR_DUPLICATE_NAME)
				{
					CString strWarning("");
					strWarning.Format ("%s\n\nDo you want to replace this file?", m_pCurrentDirectory->GetError());

					int iReturn = AfxMessageBox (strWarning, MB_YESNO);

					if (iReturn == IDYES)
					{
						if (!m_pCurrentDirectory->AddFile (szFileName, NULL, PAK_ADD_FLAG_REPLACE))
						{
							AfxMessageBox (m_pCurrentDirectory->GetError(), MB_ICONSTOP);
							return FALSE;
						}
					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					return FALSE;
				}				
			}	
		}		
	}

	// Only go rebuild the tree if we're done adding stuff.  No sense in spending that extra effort
	// if there's still work to do.
	if (!bMoreFiles)
	{
		BuildTree();
	}

	return TRUE;
}

void CPakDoc::BuildTree()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);

		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPakTreeView)))
			{			
				((CPakTreeView*)(pView))->BuildTree(TRUE);
			}
		}
	}
}

void CPakDoc::HighlightDirectory(LPCTSTR szPath, BOOL bExpand /* = FALSE */)
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CView *pView = GetNextView(Pos);

		if (pView)
		{
			if (pView->IsKindOf(RUNTIME_CLASS(CPakTreeView)))
			{			
				((CPakTreeView*)(pView))->HighlightDirectory(szPath, bExpand);
			}
		}
	}
}

BOOL CPakDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	if( lpszPathName == NULL )
	{
		m_strSaveAsSourcePAKFile = GetPakFileName();
		m_bSaveAs = TRUE;
	}
	else
	{
		m_bSaveAs = FALSE;
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
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
		}

		if (!AfxGetApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return FALSE;       // don't even attempt to save
	}

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
				ASSERT( FALSE );
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}

// This came out of DOCCORE.CPP.  We don't want to overwrite the current PAK file, we
// want to read from it first.
BOOL CPakDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFileException fe;
	CFile* pFile = NULL;

	m_strSaveActualOutputPAKFile = lpszPathName;
	// This line was changed, to remove CFile::modeCreate.  We can't have the original PAK file destroyed
	// by this modeCreate flag, as we need the original PAK data in order to write the temp PAK file.
	if( m_bSaveAs )
	{		
		pFile = GetFile(m_strSaveAsSourcePAKFile, CFile::modeReadWrite | CFile::shareExclusive, &fe);
		if (pFile == NULL)
		{
			// Something truly is wrong
			ReportSaveLoadException(m_strSaveAsSourcePAKFile, &fe,
				TRUE, AFX_IDP_INVALID_FILENAME);
			return FALSE;
		}
	}
	else
	{		
		pFile = GetFile(lpszPathName, CFile::modeReadWrite | CFile::shareExclusive, &fe);
		if (pFile == NULL)
		{
			// Okay, we might be trying to write a new file here.  Try again, this time with CFile::modeCreate
			pFile = GetFile(lpszPathName, CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive, &fe);

			if (pFile == NULL)
			{
				// Something truly is wrong
				ReportSaveLoadException(lpszPathName, &fe,
					TRUE, AFX_IDP_INVALID_FILENAME);
				return FALSE;
			}
		}
	}

	

	CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	saveArchive.m_pDocument = this;
	saveArchive.m_bForceFlat = FALSE;
	TRY
	{
		CWaitCursor wait;
		Serialize(saveArchive);     // save me
		saveArchive.Close();
		ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e)
	{		
		ReleaseFile(pFile, TRUE);

		TRY
		{
			ReportSaveLoadException(lpszPathName, e,
				TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
			e->Delete();
		}
		END_TRY
		//DELETE_EXCEPTION(e);
		return FALSE;
	}
	END_CATCH_ALL

#ifdef _MAC
	RecordDataFileOwner(lpszPathName, AfxGetAppName());
#endif

	SetModifiedFlag(FALSE);     // back to unmodified

	return TRUE;        // success
}

void CPakDoc::ReleaseFile(CFile* pFile, BOOL bAbort)
{
	ASSERT_KINDOF(CFile, pFile);	

	// Closing is handled by the PakList
	/*
	if (bAbort)
		pFile->Abort(); // will not throw an exception
	else
		pFile->Close();
	*/

	delete pFile;
}


void CPakDoc::DeleteEntry (CPakItem *pItem)
{
	m_pCurrentDirectory->DeleteItem (pItem);
}

void CPakDoc::DeleteEntry (CPakDirectory *pDirectory)
{
	CPakDirectory *pParent = pDirectory->GetParent();

	if (pParent)
	{
		pParent->DeleteDirectory (pDirectory);
	}
}

LRESULT CPakDocWnd::OnPakDocCustomMessage(WPARAM nType, LPARAM nFlags)
{
	switch (nType)
	{
	case PAK_DOC_MESSAGE_PASTE:
		{
			if (m_pDoc)
			{
				m_pDoc->PasteFiles();
			}
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}
	return 0;
}

void CPakDoc::ImportFiles()
{
	CPakImportDlg dlgImport;
	dlgImport.SetDocument (this);
	dlgImport.SetPakDirectory (m_pCurrentDirectory);

	if (dlgImport.DoModal() == IDOK)
	{
		CString strSourceDirectory (dlgImport.GetSourceDirectory());
		CString strBaseOffsetDirectory (dlgImport.GetOffsetDirectory());
		CString strWildCards (dlgImport.GetWildCards());
		CString strPakDirectory (dlgImport.GetPakDirectory());

		CPakDirectory *pPakDirectory = m_PakList.FindDirectory (strPakDirectory);
		BOOL bAddNewDirectories = FALSE;

		if (!pPakDirectory)
		{
			bAddNewDirectories = (strPakDirectory != "");
			pPakDirectory = m_PakList.GetRootDirectory();
		}

		strWildCards = TrimLeadingCharacters (strWildCards, ' ');
		
		if (strWildCards == "")
		{
			AfxMessageBox ("Nothing to import!", MB_ICONSTOP);
			return;
		}

		strWildCards = ConvertAllCharacters (strWildCards, ' ', 0);

		CDirectoryList dirList (g_bPakImportRecurseSubDirectories);

		char *szWildCard = NULL;
		int iStrPosition = 0;
		int iStrLength = strWildCards.GetLength();
		szWildCard = strWildCards.GetBuffer(iStrLength);

		BOOL bMoreWildCards = TRUE;
		while (bMoreWildCards)
		{
			dirList.AddWildcard (szWildCard);
			szWildCard += (strlen(szWildCard) + 1);
			iStrPosition += (strlen(szWildCard) + 1);

			if (iStrPosition >= iStrLength)
			{
				bMoreWildCards = FALSE;
			}				
		}

		strWildCards.ReleaseBuffer();

		dirList.SetRoot (strSourceDirectory);
		dirList.SearchDirectories ();

		CString strOffsetDirectory("");
		CString strFileName("");
		CString strFilePath("");
		CDirectoryEntry *pDir = dirList.GetFirst();
		CFileList *pList = NULL;
		CFileItem *pFile = NULL;
		BOOL bUpdate = FALSE;

		CProgressBar ctlProgress ("Adding...", 70, dirList.GetFileCount(), false, 0);

		while (pDir)
		{
			pList = pDir->GetFileList();
			pFile = pList->GetFirst();

			while (pFile)
			{
				strFileName = pFile->GetFileName();
				strFilePath = GetPathToFile (strFileName);
				strOffsetDirectory = bAddNewDirectories ? strPakDirectory : "" + TrimFromLeft (strFilePath, strBaseOffsetDirectory);
				
				if (!pPakDirectory->AddFile ( strFileName, 
						(g_bPakImportRetainStructure ? strOffsetDirectory : ""),
						(g_bPakImportReplaceExistingItems ? PAK_ADD_FLAG_REPLACE : 0)							
					))
				{						
					CString strError("");
					strError.Format ("%s\n\nDo you want to continue with the other files?", m_pCurrentDirectory->GetError());
					
					int iReturn = AfxMessageBox (strError, MB_YESNO);

					if (iReturn == IDNO)
					{
						return;
					}
				}
				else
				{
					bUpdate = TRUE;
				}
				ctlProgress.StepIt();				
				
				pFile = pFile->GetNext();
			}

			pDir = pDir->GetNext();
		}

		ctlProgress.Clear();

		if (bUpdate)
		{
			BuildTree();
			UpdateListView();
		}
	}
}

BOOL CPakDoc::ExportFiles(BOOL bHasSelections, CView *pCaller)
{
	CPakExportDlg dlgExport;
	dlgExport.SetDocument (this);
	dlgExport.SetPakDirectory (m_pCurrentDirectory);
	dlgExport.HasSelections (bHasSelections);

	if (dlgExport.DoModal() == IDOK)
	{		
		CString strDestinationDirectory (dlgExport.GetDestinationDirectory());

		if (dlgExport.ExportSelected())
		{			
			POSITION Pos = GetFirstViewPosition();
			while (Pos != NULL)
			{
				CView *pView = GetNextView(Pos);

				if (pView)
				{
					if (pView == pCaller)
					{
						if (pView->IsKindOf(RUNTIME_CLASS(CPakTreeView)))
						{							
							((CPakTreeView*)(pView))->ExportSelected (strDestinationDirectory);
						}

						if (pView->IsKindOf(RUNTIME_CLASS(CPakListView)))
						{
							((CPakListView*)(pView))->ExportSelected (strDestinationDirectory);
						}
					}					
				}
			}
		}
		else
		{
			CString strPakDirectory (dlgExport.GetPakDirectory());
			CString strWildCards (dlgExport.GetWildCards());
		
			CPakDirectory *pPakDirectory = m_PakList.FindDirectory (strPakDirectory);

			if (!pPakDirectory)
			{
				strPakDirectory = ConvertAllCharacters (strPakDirectory, '\\', '/');

				CString strError("");
				strError.Format ("The directory %s cannot be found in this Pak file.\n", strPakDirectory);
				AfxMessageBox (strError, MB_ICONSTOP);

				return ExportFiles (bHasSelections, pCaller);
			}

			strWildCards = TrimLeadingCharacters (strWildCards, ' ');
			
			if (strWildCards == "")
			{
				AfxMessageBox ("Nothing to export!", MB_ICONSTOP);
				return FALSE;
			}

			strWildCards = ConvertAllCharacters (strWildCards, ' ', 0);

			char *szWildCard = NULL;
			int iStrPosition = 0;
			int iStrLength = strWildCards.GetLength();
			szWildCard = strWildCards.GetBuffer(iStrLength);
			CStringArray saWildCards;

			BOOL bMoreWildCards = TRUE;
			while (bMoreWildCards)
			{
				saWildCards.Add (szWildCard);			
				szWildCard += (strlen(szWildCard) + 1);
				iStrPosition += (strlen(szWildCard) + 1);

				if (iStrPosition >= iStrLength)
				{
					bMoreWildCards = FALSE;
				}
			}

			strWildCards.ReleaseBuffer();
			UINT iItemCount = 0;

			pPakDirectory->GetItemCount (&iItemCount, NULL, &saWildCards);

			CProgressBar ctlProgress ("Exporting...", 70, iItemCount, false, 0);

			if (!pPakDirectory->ExportFiles (strDestinationDirectory, &saWildCards, &ctlProgress))
			{
				AfxMessageBox (pPakDirectory->GetError(), MB_ICONSTOP);
				return FALSE;
			}

			ctlProgress.Clear();
		}
	}

	return TRUE;
}

CString CPakDoc::GetPakFileName()
{
	return m_PakList.GetFileName();
}
