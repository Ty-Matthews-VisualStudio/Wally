/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998, 1999
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  DelayRender.cpp : implementation of the CDelayRender class
//
//  Created by Ty Matthews, 11-11-1999
//
//
//	Overview:
//
//	This class is a derivative of COleDataSource.  It is intended to be a wrapper
//	for delayed rendering of clipboard data.  COleDataSource provides the base
//	mechanism for this, but it stops short of actually doing anything.  You still
//	must execute the proper functions and build the code practically from scratch.
//	This class is designed to simplify this, and provide an easy method for delaying
//	the rendering of clipboard data.  This is based loosely on the COleDataSourceEx
//	class I found on CodeGuru by Keith Rule.  His class gave me the foundation, but
//	he didn't support the one thing I was looking for: CF_HDROP.
//
//	To use this, you must do two things.  One, derive your CView-derived class
//	from CDelayRenderView as well:
//
//	class CMyCoolView : public CListView, public CDelayRenderView
//	{
//	public:
//		HGLOBAL RenderData();
//	}
//
//	Second, you must override RenderData():
//
//	HGLOBAL CMyCoolView::RenderData()
//	{
//		// Build your HGLOBAL data here
//	}
// 
//  Be sure NOT to call OpenClipboard(), CloseClipboard(), or SetClipboardData() at any
//	point during the RenderData() function.  This is because MFC is handling the clipboard
//	functions behind the scenes courtesy of COleDataSource.  Simply GlobalAlloc() your HGLOBAL
//	variable, GlobalLock() it, plop in your data, GlobalUnlock() it, then return that HGLOBAL.
//	This class right now only supports the CF_HDROP format, so obviously you should be rendering
//	with DROPFILES as the actual data.  Additional formats should be simple to plug in.
//
//  Now it comes time to actually call the delayed render.  There are two ways of doing this.  One
//	is via Drag n Drop, the other is by directly calling a SetClipboard() function:
//
//	void CMyCoolView::OnBegindrag()
//	{
//		CDelayRender DelayData(this);
//		DelayData.DoDragDrop();
//	}
//
//	void CMyCoolView::OnCopy()
//	{
//		CDelayRender *pDelayData = new CDelayRender(this);
//		pDelayData->SetClipboard();
//	}
//
//	The first example creates the DelayData object and calls DoDragDrop().  This means MFC will
//	start the entire drag-n-drop operation for you, and prepare the clipboard exactly the way
//	it should be for delayed rendering.  The class deconstructor IS called, and the life of 
//	this item is only for that function.  This is because the clipboard data is built having
//	the correct format, but with additional flags to say the data can be rendered via the 
//	CDelayRenderView->RenderData() function.  IE, the address of this function is added to the
//	clipboard and Windows handles the execution prior to giving the application that is requesting
//	the data control of the clipboard.  Hence the whole reason for even doing delayed rendering...
//	the data isn't pushed up to the clipboard until it's actually needed (if ever.)
//
//	The second item creates a new pointer on the stack.  The life of this variable extends past
//	the scope of the local function.  This is because nothing does the work of opening the clipboard 
//	and building the foundation like DoDragDrop() does.  This is an even more delayed version, in 
//	that the base format isn't even put onto the clipboard until it comes time to render.  Everything 
//	happens at once at that point, and if the object was removed from memory, there's nothing for
//	Windows to call in order to check for the proper render formats.  Fortunately, Windows also takes
//	care of removing the item from memory after the clipboard has been closed and moved on.
///////////////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
#include "wally.h"
#include "DelayRender.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HGLOBAL CDelayRenderView::RenderData()
{
	ASSERT (FALSE);		// Derived classes must override this function and render the data
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDelayRender::CDelayRender( CDelayRenderView *pView) : COleDataSource()
{
	m_pView = pView;
}

CDelayRender::~CDelayRender()
{
	Empty();
}

BOOL CDelayRender::OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
{
	if (lpFormatEtc)
	{
		if (lpFormatEtc->cfFormat == CF_HDROP)
		{		
			return DelayRenderHDROP (lpFormatEtc, lpStgMedium);
		}

		if (lpFormatEtc->cfFormat == ::RegisterClipboardFormat( CFSTR_PREFERREDDROPEFFECT ))
		{
			return DelayRenderDROPEFFECT (lpFormatEtc, lpStgMedium);
		}
	}

	return COleDataSource::OnRenderData(lpFormatEtc, lpStgMedium);
}

BOOL CDelayRender::DelayRenderDROPEFFECT ( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
{	
	try
	{
		lpStgMedium->tymed = TYMED_HGLOBAL;	
		lpStgMedium->hGlobal = RenderDROPEFFECT();
		return TRUE;
	}
	catch (...)
	{
	}
	
	return FALSE;
}

BOOL CDelayRender::DelayRenderHDROP ( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium )
{
	try
	{
		lpStgMedium->tymed = TYMED_HGLOBAL;	
		lpStgMedium->hGlobal = RenderHDROP();
		return TRUE;
	}
	catch (...)
	{
	}
	
	return FALSE;
}

HGLOBAL CDelayRender::RenderHDROP()
{
	return m_pView->RenderData();
}

HGLOBAL CDelayRender::RenderDROPEFFECT()
{
	// This marks the data as copy-only.  This is to prevent Explorer or any other app from
	// blatantly removing our source file.  Who do they think they are, anyways?!
	HGLOBAL hGlobal = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, sizeof(DWORD) );
	
	DWORD *pdw = (DWORD *)GlobalLock(hGlobal);	

	// Change this to whatever the preferred drop format is
	(*pdw) = DROPEFFECT_COPY;
	
	GlobalUnlock(hGlobal);
	
	return hGlobal;
}

DROPEFFECT CDelayRender::DoDragDrop( 
									DWORD dwEffects /* = DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK */,
									LPCRECT lpRectStartDrag /* = NULL */,
									COleDropSource* pDropSource /* = NULL */)
{

	// DROPEFFECT_Format() MUST appear first here.  This tells Explorer and any other app that
	// the source files are to be copied, regardless of what state the left mouse button is in.
	// If it appears after the CF_HDROP section, then Explorer has already done its deed.
	DROPEFFECT_Format();
	HDROPFormat();
	DROPEFFECT retval = COleDataSource::DoDragDrop(dwEffects, lpRectStartDrag, pDropSource);	
	return retval;
}

void CDelayRender::HDROPFormat()
{
	try
	{
		if (m_pView != NULL)
		{
			DelayRenderData(CF_HDROP);			
		}
	}
	catch (...)
	{
	}
}

void CDelayRender::DROPEFFECT_Format()
{
	try
	{
		if (m_pView != NULL)
		{
			DelayRenderData( ::RegisterClipboardFormat( CFSTR_PREFERREDDROPEFFECT ));			
		}
	}
	catch (...)
	{
	}
}

void CDelayRender::SetClipboard()
{
	DROPEFFECT_Format();
	HDROPFormat();	
	COleDataSource::SetClipboard();
}
