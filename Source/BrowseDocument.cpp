/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// BrowseDocument.cpp : implementation file
//
// Created by Ty Matthews, 3-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Afxext.h"
#include "BrowseDocument.h"
#include "BrowseView.h"
#include "BrowseDlg.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CBrowseDocument

IMPLEMENT_DYNCREATE(CBrowseDocument, CDocument)

CBrowseDocument::CBrowseDocument()
{
}

BOOL CBrowseDocument::OnNewDocument()
{	
	CBrowseDlg GetDirectory;
	GetDirectory.SetDefaultBrowseDirectory (DefaultBrowseDirectory);

	if (GetDirectory.DoModal() == IDOK)
	{ 
		CString strPathName = GetDirectory.GetDirectory();
		DIBList.PurgeList();

		//DefaultBrowseDirectory = m_BrowseDirectory = m_BrowseDirectoryDialog.GetPathName().Left(m_BrowseDirectoryDialog.m_ofn.nFileOffset);			
		if (strPathName.GetAt(strPathName.GetLength() - 1) == '\\')
		{
			DefaultBrowseDirectory = m_BrowseDirectory = strPathName;
		}
		else
		{
			DefaultBrowseDirectory = m_BrowseDirectory = strPathName + "\\";
		}

		if (!CDocument::OnNewDocument())
			return FALSE;
	
		CString Title ("Browsing " + m_BrowseDirectory);
		SetTitle (Title);

		WIN32_FIND_DATA m_FileInfo;
		HANDLE hFileHandle;
		CString m_SearchString (m_BrowseDirectory + "*.wal");		
		CString m_TempName;			
		CBrowseDIBItem *p_TempDIB;
		int returnval = 1; 

		BeginWaitCursor();
		// Find the first instance of our wildcard
		hFileHandle = FindFirstFile ((LPCTSTR)m_SearchString, &m_FileInfo);
		
		// So long as something is returned, keep looking for more
		while ((hFileHandle != INVALID_HANDLE_VALUE) && (returnval))
		{				
			m_TempName = m_BrowseDirectory + m_FileInfo.cFileName;			// Convert to a CString			
			p_TempDIB = DIBList.AddItem (&m_TempName);						// Go add it to the linked list
			if (!p_TempDIB || g_bOutOfMemory)    
			{
				AfxMessageBox ("Not enough memory to load textures!");
				returnval = 0;
			}
			else
			{
				DocHelper.AddDIBItem(this, p_TempDIB);
				returnval = FindNextFile (hFileHandle, &m_FileInfo);
			}

		}
		EndWaitCursor();
		return TRUE;
	}		
	
	return FALSE;

}

CBrowseDocument::~CBrowseDocument()
{
	theApp.BrowseOpen = false;
	DIBList.PurgeList();
	BeginWaitCursor();	
	EndWaitCursor();

}


BEGIN_MESSAGE_MAP(CBrowseDocument, CDocument)
	//{{AFX_MSG_MAP(CBrowseDocument)
	ON_COMMAND(ID_FILE_SELECT_FOLDER, OnFileSelectFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseDocument diagnostics

#ifdef _DEBUG
void CBrowseDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CBrowseDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowseDocument serialization

void CBrowseDocument::Serialize(CArchive& ar)
{
	ASSERT (false);			// Shouldn't ever get here!
	return;

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseDocument commands


void CBrowseDocument::OnFileSelectFolder() 
{	
	OnNewDocument();	
	POSITION Pos = GetFirstViewPosition();
	CBrowseView* pFirstView = (CBrowseView *)GetNextView(Pos);
	pFirstView->SetScrollPos (SB_VERT, 0, true);
	pFirstView->Invalidate();
	pFirstView->MaxHButtons = 0;
	pFirstView->MaxVButtons = 0;
	
}

void CBrowseDocument::OpenWallyDocument(int TextureNumber)
{
	CString FileName;
	CBrowseDIBItem *p_Temp;

	p_Temp = DIBList.GetAtPosition(TextureNumber);
	FileName = m_BrowseDirectory + p_Temp->m_Name + ".wal";

	DocHelper.AddWallyItem (FileName, p_Temp);

}

void CBrowseDocument::RefreshView()
{
	POSITION Pos = GetFirstViewPosition();
	while (Pos != NULL)
	{
		CBrowseView* pView = (CBrowseView *)GetNextView( Pos);
		pView->InvalidateRect( NULL, FALSE);
	}	
}





	
	
