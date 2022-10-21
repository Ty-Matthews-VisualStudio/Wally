// ImageManipulator.cpp: implementation of the CImageManipulator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "ImageManipulator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageManipulator::CImageManipulator() : m_mbIRGBData( "m_mbIRGBData" ), m_mbSourceData( "m_mbSourceData" )
{

}

CImageManipulator::~CImageManipulator()
{

}

void CImageManipulator::LoadFromClipboard()
{
#if 0
	CDibSection ClipboardDIB;
	
	if( !ClipboardDIB.InitFromClipboard( NULL ) )
	{
		CWallyException we;
		we.SetErrorMessage( "Failed to retrieve data from clipboard" );
		throw( we );		
	}
#endif
}

void CImageManipulator::LoadFromFile( LPCTSTR szFileName )
{
}

void CImageManipulator::SetExtents( eImageManipulatorExtent eExtent, DWORD dwExtent )
{
	switch( eExtent )
	{
	case IMExtentMaxWidth:		m_dwMaxWidth = dwExtent; break;
	case IMExtentMinWidth:		m_dwMinWidth = dwExtent; break;
	case IMExtentMaxHeight:		m_dwMaxHeight = dwExtent; break;
	case IMExtentMinHeight:		m_dwMinHeight = dwExtent; break;
	case IMExtentMaxSize:		m_dwMaxSize = dwExtent; break;
	case IMExtentMinSize:		m_dwMinSize = dwExtent; break;
	default:
		{
			ASSERT( FALSE );
		}
		break;
	}

}