/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Selection.cpp : implementation file
//
// Created by Neal White III, 1-21-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Selection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelection dialog


CSelection::CSelection()
{
	m_pDoc                   = NULL;
	m_iOffsetX               = 0;

	m_bNeedsFindNearestColor = FALSE;
	m_bUseSelection          = TRUE;

	static DWORD Bits[4] = { 0x1E1E0F0F, 0x78783C3C, 0xE1E1F0F0, 0x8787C3C3 };

	m_bmpHatch.CreateBitmap( 8, 8, 1, 1, &Bits);

	m_brEdge.CreatePatternBrush( &m_bmpHatch);
}

CSelection::~CSelection()
{
	m_bmpHatch.DeleteObject();
}

void CSelection::DrawSelection( CDC* pDC, const double dfZoom, const CPoint& ptScrollOffset)
{
//	ASSERT( iZoom > 0);

	if (pDC)
	{
		if (m_bUseSelection)
		{
			CRect rBorder( m_rBoundsRect);
			rBorder.top    = (int )(rBorder.top * dfZoom);
			rBorder.left   = (int )(rBorder.left * dfZoom);
			rBorder.right  = (int )(rBorder.right * dfZoom);
			rBorder.bottom = (int )(rBorder.bottom * dfZoom);
			rBorder.OffsetRect( ptScrollOffset.x, ptScrollOffset.y);

			COLORREF rgbTextColor = pDC->SetTextColor( RGB( 0, 0, 0));
			COLORREF rgbBkColor   = pDC->SetBkColor( RGB( 255, 208, 0));

			pDC->SetBrushOrg( (m_iOffsetX + ptScrollOffset.x) & 7,
						(ptScrollOffset.y) & 7);
			m_brEdge.UnrealizeObject();

			pDC->FrameRect( rBorder, &m_brEdge);

			pDC->SetTextColor( rgbTextColor);
			pDC->SetBkColor( rgbBkColor);
		}
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CSelection::Serialize( CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_bNeedsFindNearestColor;
		ar << m_bUseSelection;
		ar << m_rBoundsRect;
	}
	else	// loading
	{
		ar >> m_bNeedsFindNearestColor;
		ar >> m_bUseSelection;
		ar >> m_rBoundsRect;
	}
	m_Layer.Serialize( ar, FALSE);
}
