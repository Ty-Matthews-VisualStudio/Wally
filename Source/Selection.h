/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Selection.h : header file
//
// Created by Neal White III, 1-21-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _SELECTION_H_
#define _SELECTION_H_

#ifndef _LAYER_H_
	#include "Layer.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWallyDoc;

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

class CSelection
{
// Construction
public:
	CSelection();   // standard constructor
	~CSelection();  // standard destructor

	CWallyDoc*  m_pDoc;
	CLayer      m_Layer;
	BOOL        m_bNeedsFindNearestColor;
	BOOL        m_bUseSelection;

	int         m_iOffsetX;	// used for animating boundry lines
	CBitmap     m_bmpHatch;
	CBrush      m_brEdge;

	CRect GetBoundsRect()  { return m_rBoundsRect; };
	inline void SetBoundsRect( int l, int t, int r, int b)
		{
			m_rBoundsRect.SetRect(l,t,r,b);
			m_Layer.m_rBoundsRect.SetRect(l,t,r,b);
		};
	inline void SetBoundsRect( CRect& Rect)
		{
			SetBoundsRect( Rect.left, Rect.top, Rect.right, Rect.bottom);
		};
	int Width()		{ return m_rBoundsRect.Width(); };
	int Height()	{ return m_rBoundsRect.Height(); };
	int TopLeftX()	{ return m_rBoundsRect.TopLeft().x; };
	int TopLeftY()	{ return m_rBoundsRect.TopLeft().y; };

	CLayer* GetLayer()  { return &m_Layer; };

private:
	CRect       m_rBoundsRect;  // outline to draw

// Operations
public:
	void DrawSelection( CDC* pDC, const double dfZoom, const CPoint& ptScrollOffset);
	void StepAnimation() { m_iOffsetX++; };

	void Serialize( CArchive& ar);

// Implementation
protected:
};

#endif  // _SELECTION_H_
