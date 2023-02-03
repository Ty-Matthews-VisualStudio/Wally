/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyDocTemplate.cpp : implementation file
//
// Created by Ty Matthews, 3-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallyDocTemplate.h"
#include "MiscFunctions.h"
#include "GraphicsFunctions.h"
//#include "ReMip.h"
#include "WallyDoc.h"

extern BOOL g_bAutoRemip;


///////////////////////////////////////////////////////////////////////////////
//  Name:		OpenDocumentFile
//  Action:		This is an override of the default CMultiDocTemplate::OpenDocumentFile
//				function.  This is needed because the user is allowed to drag-n-drop
//				image files directly to the Wally desktop.  When MFC handles those 
//				drag-n-dropped files, it doesn't care what the format or extension
//				of the file is... it will attempt to open the document regardless,
//				.wal or not a .wal.  This of course causes problems when the image 
//				doesn't match what we're expecting.
///////////////////////////////////////////////////////////////////////////////
CDocument* CWallyDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible, int iGameType)
{
	m_iGameType = iGameType;
	CDocument* pReturn = OpenDocumentFile(lpszPathName, bMakeVisible);
	m_iGameType = -1;
	return pReturn;
}

CDocument* CWallyDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{		
	CString strFileName;
	CImageHelper ihHelper;
	bool bIsExisting = false;
	int iType = 0;
	CWallyDoc* pWallyDoc = NULL;
		
	// If the passed string is not NULL, it's an existing document
	if (lpszPathName != NULL)
	{		
		strFileName = lpszPathName;
		ihHelper.LoadImage (strFileName, IH_LOAD_ONLYIMAGE | IH_LOAD_DIMENSIONS);
		
		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			return NULL;
		}
	
		//  Set the global vars for width/height
		g_iDocWidth = ihHelper.GetImageWidth();
		g_iDocHeight = ihHelper.GetImageHeight();
		g_iDocColorDepth = ihHelper.GetColorDepth();
		bIsExisting = true;

		iType = ihHelper.GetImageType();

		if (
			(iType != IH_WAL_TYPE) && 
			(iType != IH_MIP_TYPE) && 
			(iType != IH_SWL_TYPE) && 
			(iType != IH_M8_TYPE) && 
			(iType != IH_TGA_TYPE) && 
			(iType != IH_PCX_TYPE) && 
			(iType != IH_BMP_TYPE) && 
			(iType != IH_PNG_TYPE) &&
			(iType != IH_JPG_TYPE) &&
			(iType != IH_TEX_TYPE))
		{
			lpszPathName = NULL;
		}
	}

	CDocument* pDocument = CreateNewDocument();
	if (pDocument == NULL)
	{
		TRACE0("CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
	CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);

	if (lpszPathName == NULL)
	{
		// create a new document - with default document name
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;
				
		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;

		//  Cast a CWallyDoc pointer to the CDocument object created.  We can do this 
		//  because CWallyDoc is derived from CDocument.  If the image type is not a .wal,
		//  call CWallyDoc::LoadImage to bring it in.
		pWallyDoc = (CWallyDoc*)pDocument;

		if (m_iGameType == -1)
		{
			pWallyDoc->SetGameType(g_iFileTypeDefault);	// Neal - fixes missing submips on new wal view
		}
		else
		{
			pWallyDoc->SetGameType(m_iGameType);
		}
		
		if (bIsExisting)
		{
			pWallyDoc->LoadImage(&ihHelper);
			pWallyDoc->SetTitle(GetRawFileName(strFileName));
			
			//  Rebuild the subs if desired
			if (g_bAutoRemip)
				pWallyDoc->RebuildSubMips();
			
			pWallyDoc->SetModifiedFlag (true);
		}
	}
	else
	{
		pWallyDoc = (CWallyDoc *)pDocument;
		// Custom stuff here:
		switch (iType)
		{
		case IH_WAL_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_QUAKE2);
			break;

		case IH_MIP_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_QUAKE1);
			break;

		case IH_M8_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_HERETIC2);
			break;

		case IH_SWL_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_SIN);
			break;

		case IH_TGA_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_TGA);
			break;

		case IH_PCX_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_PCX);
			break;

		case IH_BMP_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_BMP);
			break;

		case IH_PNG_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_PNG);
			break;

		case IH_JPG_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_JPG);
			break;

		case IH_TEX_TYPE:
			pWallyDoc->SetGameType (FILE_TYPE_TEX);
			break;

		default:
			break;
		}		
	
		// open an existing document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			pFrame->DestroyWindow();
			return NULL;
		}
#ifdef _MAC
		// if the document is dirty, we must have opened a stationery pad
		//  - don't change the pathname because we want to treat the document
		//  as untitled
		if (!pDocument->IsModified())
#endif
			pDocument->SetPathName(lpszPathName);	
		
	}

	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	return pDocument;
}

void CWallyDocTemplate::SetDefaultTitle(CDocument* pDocument)
{
	//  This function came from Docmulti.cpp in the MFC source code	
	CString strDocName;
	if (GetDocString(strDocName, CDocTemplate::docName) &&
		!strDocName.IsEmpty())
	{
/*		TCHAR szNum[8];
#ifndef _MAC
		wsprintf(szNum, _T("%d"), m_nUntitledCount+1);
#else
		wsprintf(szNum, _T(" %d"), m_nUntitledCount+1);
#endif */		
		strDocName += "wal";
	}
	else
	{
		// use generic 'untitled' - ignore untitled count
		VERIFY(strDocName.LoadString(AFX_IDS_UNTITLED));
	}
	pDocument->SetTitle(strDocName);
}