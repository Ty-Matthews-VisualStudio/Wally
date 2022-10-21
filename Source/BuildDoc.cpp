// BuildDoc.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BuildDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildDoc

IMPLEMENT_DYNCREATE(CBuildDoc, CDocument)

CBuildDoc::CBuildDoc()
{
}

BOOL CBuildDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CBuildDoc::~CBuildDoc()
{
}


BEGIN_MESSAGE_MAP(CBuildDoc, CDocument)
	//{{AFX_MSG_MAP(CBuildDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildDoc diagnostics

#ifdef _DEBUG
void CBuildDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBuildDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBuildDoc serialization

void CBuildDoc::Serialize(CArchive& ar)
{
	CFile* pFile = ar.GetFile();

	if (ar.IsStoring())
	{
		m_ihHelper.SaveART(pFile);
	}
	else
	{
		BeginWaitCursor();
		m_ihHelper.LoadImage (pFile);
		//m_iWADType = m_ihHelper.GetImageType() == IH_WAD2_TYPE ? WAD2_TYPE : WAD3_TYPE;
		EndWaitCursor();
	}

	if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		AfxMessageBox (m_ihHelper.GetErrorText(), MB_ICONSTOP);
		return;
	}		
}

/////////////////////////////////////////////////////////////////////////////
// CBuildDoc commands


CBuildItem *CBuildDoc::GetBuildTile(int iTileNumber)
{
	return m_ihHelper.GetBuildTile (iTileNumber);
}

void CBuildDoc::ValidateTileNumber (int *pTile)
{
	int iLastTile = m_ihHelper.GetLastBuildTile();
	int iFirstTile = m_ihHelper.GetFirstBuildTile();

	(*pTile) = max (iFirstTile, (*pTile));
	(*pTile) = min (iLastTile, (*pTile));
}

int CBuildDoc::GetFirstBuildTile ()
{
	return m_ihHelper.GetFirstBuildTile();
}

int CBuildDoc::GetLastBuildTile ()
{
	return m_ihHelper.GetLastBuildTile();
}