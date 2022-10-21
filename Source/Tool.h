/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Tool.h : interface of the CTool class
//
// Created by Neal White III, 3-25-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _WALLYDOC_H_
	#include "WallyDoc.h"
#endif

#ifndef _TOOL_H_
#define _TOOL_H_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MOUSE_MOVE			0
#define MOUSE_BUTTON_DOWN	1
#define MOUSE_BUTTON_UP		2
#define TIMER_MSG			MOUSE_MOVE

#define SPRAY_COVERAGE 25		// 25 percent coverage

//#define HINT_UPDATE_STANDARD	0
//#define HINT_UPDATE_DIBS		1
//#define HINT_UPDATE_PAL			2

#include "DibSection.h"

extern BYTE g_byCursorLookUp[256];

extern int    g_iDarkenPercent;
extern double g_dfLightenPercent;


class CWallyDoc;

#define TOOL_EFFECT_NONE		0
#define TOOL_EFFECT_PRIMARY		111
#define TOOL_EFFECT_SECONDARY	222

#define SCRATCH_TYPE_DARKEN			0
#define SCRATCH_TYPE_DEEP_GROOVE	1
#define SCRATCH_TYPE_LIGHTEN		2

class CToolColor
{
public:
	COLOR_IRGB m_irgbColor;
	int        m_iAlpha;			// 0..255
	int        m_iEffect;			// TOOL_EFFECT_NONE, TOOL_EFFECT_PRIMARY, TOOL_EFFECT_SECONDARY

	CToolColor()
	{
		m_irgbColor = COLOR_DEBUG_PURPLE;
		m_iAlpha    = 255;
		m_iEffect   = TOOL_EFFECT_NONE;
	}
};

// CToolStrokeInfo is intended to be a temporary object used to pass
// parameters to the stroke and spot functions.  Do NOT keep it around
// as a permentent object.  Create it, init it, and pass it.  The stroke
// functions are allowed to change the member vars.  The spot functions
// are not.
//
class CToolStrokeInfo: public CToolColor
{
public:
	CWallyDoc*	   m_pDoc;
	CWallyPalette* m_pPal;
	CWallyView*	   m_pView;
	CPoint		   m_ptImage;		// for rectangle, ptMin
	CPoint		   m_ptImage2;		// for rectangle, ptMax
	COLOR_IRGB     m_irgbReplaceColor;
	COLOR_IRGB     m_irgbColor2;
	COLOR_IRGB     m_irgbEffectColor;
	int			   m_iSize;
	int			   m_iCoverage;		// used by spray tools
	int			   m_iMouseCode;	// MOUSE_BUTTON_DOWN, MOUSE_BUTTON_UP, etc.
	int			   m_iEditMode;
	int            m_iAmount;
	int            m_iFilterMult;	// used by FilterSpot
	int            m_iScratchType;	// 0: SCRATCH_TYPE_DARKEN, 1: SCRATCH_TYPE_LIGHTEN
	int            m_iBrushShape;
	double         m_dfAmount;
	double         m_dfHue;
	double         m_dfSaturation;
	double         m_dfValue;
	double         m_dfReplaceHue;
	double         m_dfReplaceSaturation;
	double         m_dfReplaceValue;
	BOOL		   m_bLeftButton;
	BOOL		   m_bHollow;		// only used by spot functions
	BOOL		   m_bRect;			// only used by spot functions

	CToolStrokeInfo();
	void Init( CWallyView* pView, int iEditMode, int iMouseCode, COLOR_IRGB irgbColor, COLOR_IRGB irgbColor2);
	void SetReplaceColor( COLOR_IRGB irgbReplaceColor);
};

DWORD __stdcall ThreadFloodFill( LPVOID pvoidWallyView);

class CTool
{
public:
	CTool();
	virtual ~CTool();

// Attributes
public:

// Members
public:
	CPoint m_ptFlood;
	CPoint m_ptLastBigCursorPos;
	int    m_iLastBigCursorSize;
	CPoint m_ptLastHollowCursorPos;
	int    m_iLastHollowCursorSize;

	CPoint m_ptLastMouseDown;		// in client coords!
	CPoint m_ptLastMouseUp;			// in client coords!
	BOOL   m_bLastMouseDownIsValid;
	BOOL   m_bLastMouseUpIsValid;
	BYTE*  m_pbySpotMarker;
	int    m_iSpotMarkerSize;

// Operations
public:
	void InitTool( CWallyView* pView, int iToolMode);
	void UseTool( CWallyView* pView, UINT nFlags, CPoint ptPos, int iMouseCode);
	void DrawBigCursor( CWallyView* pView, CPoint ptImage, int iSize, bool bHollow);
	void ClearBigCursor( CWallyView* pView);
	
	void DrawSpot( const CToolStrokeInfo& Spot);
	void SpraySpot( const CToolStrokeInfo& Spot);
	BOOL DrawRivet(  const CToolStrokeInfo& Spot);
	void DecalSpot(   const CToolStrokeInfo& Spot);
	void FilterSpot(   const CToolStrokeInfo& Spot);
	void ReplaceSpot(   const CToolStrokeInfo& Spot);
	void PatternedSpot(  const CToolStrokeInfo& Spot);
	BOOL DrawBulletHole(  const CToolStrokeInfo& Spot);

	void DrawStroke(  CToolStrokeInfo* pStroke);
	void DrawLine(     CToolStrokeInfo* pStroke);
	void PaintLine(     CToolStrokeInfo* pStroke);
	void SprayLine(      CToolStrokeInfo* pStroke);
	void DecalLine(       CToolStrokeInfo* pStroke);
	void ReplaceLine(      CToolStrokeInfo* pStroke);
	void PatternedLine(     CToolStrokeInfo* pStroke);
	void DrawSolidRectangle( CToolStrokeInfo* pStroke);

	void InternalFloodFill( int iX, int iY, int iDepth);
	void FloodFill( CWallyView* pView, CPoint ptImage, COLOR_IRGB irgbInteriorColor, COLOR_IRGB irgbColor);
	void ShowToolInfo( CWallyView* pView, CPoint ptPos);
	void UpdateConstrainPaneText( void);

	LPCSTR GetToolName( int iEditMode);
	
// Implementation

protected:
	void DeleteSpotMarker( void);

#ifdef _DEBUG
//	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL  m_bFinalStroke;
};

/////////////////////////////////////////////////////////////////////////////

#endif		// _TOOL_H_
