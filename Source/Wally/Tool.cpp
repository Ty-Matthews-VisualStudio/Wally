/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Tool.cpp : implementation of the CTool class
//
// Created by Neal White III, 3-25-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Tool.h"
#include "MiscFunctions.h"
//#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTool g_Tool;

CPoint g_ptLine1, g_ptLine2;

// trail is previous last point (used for improved scratch tool)
CPoint ptLastPos, ptTrail;

//extern BYTE g_byEffectsIndexLookUp[3][256];
extern BYTE g_byCursorLookUp[256];

int        g_iLastLeftColor     = 255;
int        g_iLastRightColor    = 255;
COLOR_IRGB g_irgbLastLeftColor  = RGB(   0,   0,   0);
COLOR_IRGB g_irgbLastRightColor = RGB( 255, 255, 255);
int        g_iDarkenPercent     = 0;
double     g_dfLightenPercent   = 0;

/////////////////////////////////////////////////////////////////////////
// int g_iColorTolerance = 10;	// TODO: remove me once Ty has added this var
//
// Ty, I've rethought this. The default should be 0, not 10.
//     The min value is 0; the max is 255 (for dlg box bounds checking)
//
/////////////////////////////////////////////////////////////////////////

//#define TIMER_REMIP         1001
//#define TIMER_AUTO_REMIP    1002
//#define TIMER_SPRAY         1003
//#define TIMER_CLEAR_CURSOR  1004
//#define TIMER_ANIMATE_SELECTION  1005

BOOL g_bConstrained             = FALSE;
BOOL g_bConstrainedHorizonal    = TRUE;
BOOL g_bFlipped                 = FALSE;
//BOOL g_bToolsUseTransparency255 = TRUE;

COLOR_IRGB g_irgbMixColor       = COLOR_DEBUG_PURPLE;
int        g_iBulletIndex       = -1;

#define _PI_	3.141592654		// neal - "_PI_" is much easier to find than "PI"

	/////////////////////////////////////////////////////////////////////////
	// Neal - speed up
	//
	// 1. create a shared buffer here in the tools
	//    it would only need to be resized if the size differs from
	//    the last size in UseTool
	//    (uses less memory)
	//    it can even avoid frequent resizing by:
	//			only resize if too small
	//			or if buffer is a lot larger than needed (say 4x)
	//			minimum size: 64x64
	//    called m_pbySpotMarker
	//
	// 2. on each mouse-down (or stroke), fill the buffer with zeros
	//
	// 3. whenever a pixel is drawn, change the buffer[index] as a mark
	//    (in all the spot functions
	//
	// 4. spot functions can then check the status of the mark and
	//    skip it if it has already been drawn
	/////////////////////////////////////////////////////////////////////////

//#define ToolUsesSpotMarker( x) (IsDrawingTool( x) && (x != EDIT_MODE_SPRAY) && (x != EDIT_MODE_SPRAY_RECOLOR))
#define ToolUsesSpotMarker( x) (IsDrawingTool( x) && (x != EDIT_MODE_SPRAY))

#define IsKeyPressed( vk) (GetKeyState( vk) < 0)
#define sq( x)            ((x) * (x))


inline void AddColor( COLORREF rgbColor, int iMult, int* piRed, int* piGreen, int* piBlue)
{
	*piRed   += iMult * GetRValue( rgbColor);
	*piGreen += iMult * GetGValue( rgbColor);
	*piBlue  += iMult * GetBValue( rgbColor);
}

///////////////////////////////////////////////////////////////////////
CToolStrokeInfo::CToolStrokeInfo()
{
	m_pDoc             = NULL;
	m_pPal             = NULL;
	m_pView            = NULL;
	m_iSize            = 0;
	m_irgbReplaceColor = COLOR_DEBUG_PURPLE;
	m_irgbColor2       = COLOR_DEBUG_PURPLE;
	m_iMouseCode       = 0;
	m_iEditMode        = g_iCurrentTool;
	m_iCoverage        = SPRAY_COVERAGE;
	m_iAmount          = 0;
	m_iFilterMult      = 0;
	m_iScratchType     = 0;
	m_dfAmount         = 0.0;
	m_dfHue            = 0.0;
	m_dfSaturation     = 0.0;
	m_dfValue          = 0.0;
	m_dfReplaceHue        = 0.0;
	m_dfReplaceSaturation = 0.0;
	m_dfReplaceValue      = 0.0;
	m_iBrushShape      = g_iBrushShape;

	m_bLeftButton      = FALSE;
	m_bHollow          = FALSE;
	m_bRect            = FALSE;
}

void CToolStrokeInfo::Init( CWallyView* pView, int iEditMode, int iMouseCode, 
						   COLOR_IRGB irgbColor, COLOR_IRGB irgbColor2)
{
	m_pView = pView;
	m_pDoc  = pView->GetDocument();
	m_pPal  = m_pDoc->GetPalette();

	m_iEditMode   = iEditMode;
	m_iMouseCode  = iMouseCode;
	m_iAmount     = (int )(ToolAmountToPercent( m_iEditMode));
	m_bLeftButton = m_pView->m_bLeftButtonTracking;

	if (m_bLeftButton)
	{
		m_irgbColor   = irgbColor;
		m_irgbColor2  = irgbColor2;
	}
	else
	{
		m_irgbColor   = irgbColor2;
		m_irgbColor2  = irgbColor;

		// Neal - fixes right button replace/recolor bug
		//
		irgbColor     = m_irgbColor;
		irgbColor2    = m_irgbColor2;
	}

	if (IsAnyEffect( iEditMode))
	{
		m_irgbEffectColor = m_irgbColor;

		if (! IsDecalTool( iEditMode))
//		if (! IsTintEffect( iEditMode) && ! IsDecalTool( iEditMode))
		{
			if (m_bLeftButton)
			{
				m_irgbColor  = 0;
				m_irgbColor2 = 1;
			}
			else
			{
				m_irgbColor  = 1;
				m_irgbColor2 = 0;
			}
		}
	}

	int r = GetRValue( irgbColor);
	int g = GetGValue( irgbColor);
	int b = GetBValue( irgbColor);

	switch (m_iEditMode)
	{
	case EDIT_MODE_SPRAY_RECOLOR:
		{
			switch (g_iSprayRecolorAmount)
			{
			case 0:
				m_iCoverage = m_iCoverage / 2;
				break;
			case 1:
				m_iCoverage = m_iCoverage;
				break;
			case 2:
				m_iCoverage = m_iCoverage * 3 / 2;
				break;
			case 3:
				m_iCoverage = m_iCoverage * 2;
				break;
			case 4:
				m_iCoverage = m_iCoverage * 3;
				break;
			default:
				ASSERT( FALSE);
			}
		}
		// drop thru

	case EDIT_MODE_SPRAY:
		RGBtoHSV( r, g, b, &m_dfHue, &m_dfSaturation, &m_dfValue);
		// drop thru

	case EDIT_MODE_TINT:
		m_iAmount = (int )(m_iAmount * 1.5);
		break;

	case EDIT_MODE_REPLACE:
	case EDIT_MODE_RECOLOR:
		RGBtoHSV( r, g, b, &m_dfHue, &m_dfSaturation, &m_dfValue);
		SetReplaceColor( irgbColor2);
		break;

	case EDIT_MODE_LIGHTEN:
		m_dfAmount = g_dfLightenPercent;

		if (! m_bLeftButton)
			m_dfAmount += 10;
		break;

	case EDIT_MODE_DARKEN:
		m_iAmount = g_iDarkenPercent;

		if (! m_bLeftButton)
			m_iAmount -= 10;
		break;

	case EDIT_MODE_BLEND:
		m_iFilterMult = 8;

		switch (g_iBlendAmount)
		{
		case FILTER_VERY_LIGHT:
			m_iFilterMult = 32;
			break;
		case FILTER_LIGHT:
			m_iFilterMult = 16;
			break;
		case FILTER_MEDIUM:
			m_iFilterMult = 8;
			break;
		case FILTER_HEAVY:
			m_iFilterMult = 4;
			break;
		case FILTER_VERY_HEAVY:
			m_iFilterMult = 2;
			break;
		default:
			ASSERT (FALSE);
			break;
		}

		if (! m_bLeftButton)	// right button == more effect than left button
		{
			m_iFilterMult /= 4;
			m_iFilterMult = max( 1, m_iFilterMult);
		}
		break;

	case EDIT_MODE_SCRATCH:
		m_iAmount  = ToolAmountToPercent( m_iEditMode);
		m_dfAmount = m_iAmount / 3.0;
		m_iAmount  = 100 - m_iAmount;
		break;

	case EDIT_MODE_ERASER:
		{
			// Neal - the eraser uses reversed colors
			COLOR_IRGB irgbSwap;

			irgbSwap     = m_irgbColor;
			m_irgbColor  = m_irgbColor2;
			m_irgbColor2 = irgbSwap;
		}
		break;

	default:
		break;
	}
}

void CToolStrokeInfo::SetReplaceColor( COLOR_IRGB irgbReplaceColor)
{
	m_irgbReplaceColor = irgbReplaceColor;

	int r = GetRValue( irgbReplaceColor);
	int g = GetGValue( irgbReplaceColor);
	int b = GetBValue( irgbReplaceColor);

	RGBtoHSV( r, g, b, 
			&m_dfReplaceHue, &m_dfReplaceSaturation, &m_dfReplaceValue);
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Name:	CTool
// Action:	Standard constructor
///////////////////////////////////////////////////////////////////////
CTool::CTool()
{
	m_pbySpotMarker         = NULL;
	m_iSpotMarkerSize       = 0;
	m_bFinalStroke          = FALSE;
	m_iLastBigCursorSize    = -1;
	m_iLastHollowCursorSize = -1;
};

///////////////////////////////////////////////////////////////////////
// Name:	~CTool
// Action:	Standard destructor
///////////////////////////////////////////////////////////////////////
CTool::~CTool()
{
	DeleteSpotMarker();
};

///////////////////////////////////////////////////////////////////////
// Name:	DeleteSpotMarker
// Action:	Deletes the SpotMarker buffer, if needed
///////////////////////////////////////////////////////////////////////
void CTool::DeleteSpotMarker()
{
	if (m_pbySpotMarker)
	{
		delete [] m_pbySpotMarker;
		m_pbySpotMarker  = NULL;
		m_iSpotMarkerSize = 0;
	}
};

inline int GetTweakedBrushShape( int iSize)
{
	int iShape = g_iBrushShape;

	if ((iSize == 3) && (iShape == SHAPE_CIRCULAR))
		iShape = SHAPE_DIAMOND;			// circular should not draw corners
	else if ((iSize == 4) && (iShape == SHAPE_DIAMOND))
		iShape = SHAPE_CIRCULAR;		// has better corners at size 4

	return iShape;
}

inline BOOL SkipPixel( int x, int y, int iShape, int iSize, int iDelta, 
			  int iDelta2, BOOL bEven, BOOL bHollow)
{
	if (iSize <= 2)
		return (FALSE);

	if (iSize > 4)
	{
		int iDist;
		int ix = (bEven && (x < 0)) ? x+1 : x;
		int iy = (bEven && (y < 0)) ? y+1 : y;
		int j  = (bEven) ? (iDelta-1) : iDelta;

		if (iShape == SHAPE_SQUARE)
		{
			if (bHollow)
			{
				if ((x == -iDelta) || (x == iDelta2) ||
							(y == -iDelta) || (y == iDelta2))
					return FALSE;
				else
					return TRUE;
			}
			else
				return FALSE;
		}
		else if (iShape == SHAPE_CIRCULAR)
		{
			iDist = (iy*iy + ix*ix);

			if (bHollow)
				if (iDist < ((j-2)*(j-2)+1))
					return (TRUE);

			if (iDist > (j*j+1))
				return (TRUE);
		}
		else if (iShape == SHAPE_DIAMOND)
		{
			iDist = (abs( iy) + abs( ix));

			if (bHollow)
				if (iDist < j-1)
					return (TRUE);

			if (iDist > j)
				return (TRUE);
		}
		else
		{
			ASSERT( FALSE);
		}
	}
	else	// iSize <= 4
	{
		// neal - too small to be hollow (doesn't draw anything)
		// it's also too small to look much like a diamond/circle
		// so just round off the corners

		if (iShape != SHAPE_SQUARE)
		{
			if (((y == -iDelta) || (y == iDelta2)) && 
						((x == -iDelta) || (x == iDelta2)))
				return (TRUE);
		}
	}

	return (FALSE);
}

///////////////////////////////////////////////////////////////////////
// Name:		DrawSpot
// Action:		Draws a spot with the current tool at ptPos
//
// Parameters:	ptImage:	point in image coordinates
//				iSize:		line thickness
//				iColor:		color index or effect index to use
//				bHollow:	a speed-up for drawing fat lines
///////////////////////////////////////////////////////////////////////
void CTool::DrawSpot( const CToolStrokeInfo& Spot)
{
	int iEditMode = Spot.m_iEditMode;

	if ((iEditMode == EDIT_MODE_BLEND) || (iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
	{
		FilterSpot( Spot);
		return;
	}

	// neal - just in case
	if (IsAnyEffect( iEditMode) && (Spot.m_pDoc->m_pEffectLayer == NULL))
	{
		ASSERT( FALSE);
		iEditMode = EDIT_MODE_BRUSH;
	}

	CWallyDoc* pDoc = Spot.m_pDoc;

	int    ix, iy, r, g, b;
	int    iTintR, iTintG, iTintB;
	int    iAmount  = Spot.m_iAmount;
	double dfAmount = Spot.m_dfAmount;

	COLOR_IRGB irgbColor;

//	CWallyPalette* pPal = Spot.m_pPal;
	r = GetRValue( Spot.m_irgbColor);
	g = GetGValue( Spot.m_irgbColor);
	b = GetBValue( Spot.m_irgbColor);

	if ((iEditMode == EDIT_MODE_PENCIL) || 
				(iEditMode == EDIT_MODE_BRUSH) ||
				(iEditMode == EDIT_MODE_ERASER) ||
				(iEditMode == EDIT_MODE_LIGHTEN) || 
				(iEditMode == EDIT_MODE_DARKEN))
	{
		// nothing to do
	}
	else if (iEditMode == EDIT_MODE_TINT)
	{
		iAmount = Spot.m_iAmount;
		r = GetRValue( Spot.m_irgbEffectColor);
		g = GetGValue( Spot.m_irgbEffectColor);
		b = GetBValue( Spot.m_irgbEffectColor);

		iTintR  = r * iAmount;		// Neal - scale tint values
		iTintG  = g * iAmount;
		iTintB  = b * iAmount;
	}
	else if (iEditMode == EDIT_MODE_SCRATCH)
	{
		ASSERT( (Spot.m_iScratchType >= SCRATCH_TYPE_DARKEN) && 
				(Spot.m_iScratchType <= SCRATCH_TYPE_LIGHTEN));
	}
	else
	{
		ASSERT( FALSE);		// have you added a new tool?
		return;
	}

	int iWidth  = pDoc->Width();
	int iHeight = pDoc->Height();

	int iSize  = Spot.m_iSize;

	int iDelta  = iSize / 2;
	int iDelta2 = (iSize-1) / 2;
	int iTile   = (Spot.m_pView->m_bTile) ? 3 : 1;
/*
	if ((iDelta == 0) && (iSize2 == 0))		// neal - single pixel wide line (speed up)
	{
		ix = iImageX;
		iy = iImageY;

		if ((iy >= 0) && (ix >= 0))
		{
			if ((iy < iTile*h) && (ix < iTile*w))
			{
				int iIndex = ((iy % h) * w) + (ix % w);

				if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
					return;

				if (IsAnyEffect( iEditMode))
				{
					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);

					if (iEditMode == EDIT_MODE_TINT)
					{
						// blend in a little bit of tint color

						r = GetRValue( irgbColor);
						g = GetGValue( irgbColor);
						b = GetBValue( irgbColor);

						r = (r * (100-iTintAmount) + iTintR) / 100;
						g = (g * (100-iTintAmount) + iTintG) / 100;
						b = (b * (100-iTintAmount) + iTintB) / 100;

						//ClampRGB( &r, &g, &b);

						pDoc->SetNearestColorWrappedPixel( pView, ix, iy, RGB( r, g, b));

						if (m_pbySpotMarker)
							m_pbySpotMarker[iIndex] = 255;
						return;
					}
					else
					{
						// TODO: effect needs to be a CLayer

						iColor = g_byEffectsIndexLookUp[iLookUp][iColor];
					}
				}
				pDoc->SetIndexColorWrappedPixel( pView, ix, iy, iColor);

				if (m_pbySpotMarker)
					m_pbySpotMarker[iIndex] = 255;
			}
		}
	}
	else	// more than one pixel thick
*/
	{
		int iStart, iEndX, iEndY;
		int iImageX = Spot.m_ptImage.x;
		int iImageY = Spot.m_ptImage.y;

		BOOL bEven = ! (iSize & 1);		// even width?

		if (Spot.m_bRect)		// we ARE drawing a rectangular area?
		{
			iStart = 0;
			iEndX  = Spot.m_ptImage2.x - iImageX;
			iEndY  = Spot.m_ptImage2.y - iImageY;
		}
		else					// no, just a spot
		{
			iStart = -iDelta;
			iEndX  = iEndY = iDelta2;
		}

		CLayer* pEffectLayer = pDoc->m_pEffectLayer;

		for (int y = iStart; y <= iEndY; y++)
		{
			for (int x = iStart; x <= iEndX; x++)
			{
				if (! Spot.m_bRect)		// are we drawing a spot?
					if (SkipPixel( x, y, Spot.m_iBrushShape, iSize, iDelta, iDelta2, bEven, Spot.m_bHollow))
						continue;

				ix = iImageX + x;
				iy = iImageY + y;

				if ((iy >= 0) && (ix >= 0))
				{
					if ((iy < iTile*iHeight) && (ix < iTile*iWidth))
					{
						int iIndex = ((iy % iHeight) * iWidth) + (ix % iWidth);

						if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
							continue;

						if (IsAnyEffect( iEditMode))
						{
							irgbColor = pEffectLayer->GetPixel( iIndex);

							r = GetRValue( irgbColor);
							g = GetGValue( irgbColor);
							b = GetBValue( irgbColor);

							if (iEditMode == EDIT_MODE_TINT)
							{
								// blend in a little bit of tint color

								r = (r * (100-iAmount) + iTintR) / 100;
								g = (g * (100-iAmount) + iTintG) / 100;
								b = (b * (100-iAmount) + iTintB) / 100;

								//ClampRGB( &r, &g, &b);
							}
							else if (iEditMode == EDIT_MODE_SCRATCH)
							{
								if (Spot.m_iScratchType < SCRATCH_TYPE_LIGHTEN)
								{
									goto darken;
								}
								else
								{
									goto lighten;
								}
							}
							else if (iEditMode == EDIT_MODE_LIGHTEN)
							{
lighten:
								r = (int )((255*dfAmount/100) + r * (100-dfAmount) / 100);
								g = (int )((255*dfAmount/100) + g * (100-dfAmount) / 100);
								b = (int )((255*dfAmount/100) + b * (100-dfAmount) / 100);
							}
							else if (iEditMode == EDIT_MODE_DARKEN)
							{
darken:
								r = (r * iAmount) / 100;  // DON'T use *= (loses precision)
								g = (g * iAmount) / 100;
								b = (b * iAmount) / 100;
							}
							else
							{
								ASSERT( FALSE);		// have you added a new tool?
								return;
							}
						}
//						pDoc->SetIndexColorWrappedPixel( pView, ix, iy, iColor);

						if (iEditMode != EDIT_MODE_SCRATCH)
						{
							pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));

							if (m_pbySpotMarker && (iEditMode != EDIT_MODE_SCRATCH))
								m_pbySpotMarker[iIndex] = 255;
						}
						else
						{
							pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));

							//if (Spot.m_iScratchType == SCRATCH_TYPE_DEEP_GROOVE)
							//	pEffectLayer->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));
						}
					}
				}		// End  if ((iy >= 0) && (ix >= 0))
			}			// End  for (int x = -iDelta; x <= iDelta2; x++)
		}				// End  for (int y = -iDelta; y <= iDelta2; y++)
	}					// End  else	// more than one pixel thick
}

///////////////////////////////////////////////////////////////////////
// Name:		FilterSpot
// Action:		Filters a spot with the current tool at ptPos
//
// Parameters:	ptImage:	point in image coordinates
//				iSize:		line thickness
//				iColor:		color index or effect index to use
//				bHollow:	a speed-up for drawing fat lines
///////////////////////////////////////////////////////////////////////
void CTool::FilterSpot( const CToolStrokeInfo& Spot)
{
	int iEditMode = g_iCurrentTool;

	if (! IsBufferEffect( iEditMode))
	{
		ASSERT( FALSE);
		return;
	}

	CWallyDoc* pDoc = Spot.m_pDoc;

	// neal - just in case
	if (IsBufferEffect( iEditMode) && (pDoc->m_pEffectLayer == NULL))
	{
//		if (iEditMode != EDIT_MODE_BLEND)
//		{
//			iEditMode = EDIT_MODE_BRUSH;
//		}
		ASSERT( FALSE);
	}

	int ix, iy;
	int w = pDoc->Width();
	int h = pDoc->Height();

	int iSize   = Spot.m_iSize;
	int iDelta  = iSize / 2;
	int iDelta2 = (iSize-1) / 2;
	int iTile   = (Spot.m_pView->m_bTile) ? 3 : 1;

	int            iIndex;
	int            iShape   = Spot.m_iBrushShape;
	int            iPercent = Spot.m_iAmount;
	BOOL           bEven    = ! (iSize & 1);		// even width?
	BOOL           bIsFullBright;
	COLOR_IRGB     irgbColor;

	int iColor  = 0;
	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;
/*
	if ((iDelta == 0) && (iSize2 == 0))		// neal - single pixel wide line (speed up)
	{
		ix = iImageX;
		iy = iImageY;

		if ((iy >= 0) && (ix >= 0))
		{
			if ((iy < iTile*h) && (ix < iTile*w))
			{
				iIndex = ((iy % h) * w) + (ix % w);

				if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
					return;

				if (iEditMode == EDIT_MODE_BLEND)
				{
					// center pixel
					irgbColor     = pDoc->m_pEffectLayer->GetPixel( iIndex);
					bIsFullBright = Spot.m_pPal->IsFullBright( GetIValue( irgbColor));

					//ASSERT( !bIsFullBright);
					//if (bIsFullBright)
					//	MessageBeep( 0);

					int iRed   = GetRValue( irgbColor) * iFilterMult;
					int iGreen = GetGValue( irgbColor) * iFilterMult;
					int iBlue  = GetBValue( irgbColor) * iFilterMult;

					// horizontal and vertical neighbors

					// +w instead of -1 to prevent MOD problems
					// neighbor LEFT
					iIndex    = ((iy % h) * w) + ((ix+w) % w);
					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
					AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

					// neighbor RIGHT
					iIndex    = ((iy % h) * w) + ((ix+1) % w);
					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
					AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

					// +h instead of -1 to prevent MOD problems
					// neighbor ABOVE
					iIndex    = (((iy+h) % h) * w) + (ix % w);
					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
					AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

					// +h instead of -1 to prevent MOD problems
					// neighbor BELOW
					iIndex    = (((iy+1) % h) * w) + (ix % w);
					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
					AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

					iRed   /= iFilterMult + 4*2;
					iGreen /= iFilterMult + 4*2;
					iBlue  /= iFilterMult + 4*2;

//					iColor = pPal->FindNearestColor( iRed, iGreen, iBlue, bIsFullBright);

					// Ty- yanked for now
					//ASSERT( iColor != 255);
//					irgbColor = IRGB( iColor, iRed, iGreen, iBlue);
					irgbColor = IRGB( 0, iRed, iGreen, iBlue);
				}
				else if ((iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
				{
					int iOffsetX = ix - pDoc->m_ptCloneStart.x + pDoc->m_ptCloneOffset.x;
					iOffsetX += 1000 * w;

					int iOffsetY = iy - pDoc->m_ptCloneStart.y + pDoc->m_ptCloneOffset.y;
					iOffsetY += 1000 * h;

					iIndex    = ((iOffsetY % h) * w) + (iOffsetX % w);

					irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
					int r     = GetRValue( irgbColor);
					int g     = GetGValue( irgbColor);
					int b     = GetBValue( irgbColor);

					if (iPercent < 100)
					{
						irgbColor = pDoc->GetWrappedPixel( ix, iy);

						r = ((r * iPercent) + ((100-iPercent) * GetRValue( irgbColor))) / 100;
						g = ((g * iPercent) + ((100-iPercent) * GetGValue( irgbColor))) / 100;
						b = ((b * iPercent) + ((100-iPercent) * GetBValue( irgbColor))) / 100;

//						iColor = pPal->FindNearestColor( r, g, b, FALSE);
					}

//					irgbColor = IRGB( iColor, r, g, b);
					irgbColor = IRGB( 0, r, g, b);
				}
				else
				{
					ASSERT( FALSE);
				}
//				pDoc->SetWrappedPixel( Spot.m_pView, ix, iy, irgbColor);
				pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, irgbColor, bIsFullBright);

				if (m_pbySpotMarker)
				{
					iIndex = ((iy % h) * w) + (ix % w);
					m_pbySpotMarker[iIndex] = 255;
				}
			}
		}
	}
	else	// more than one pixel thick
*/
	{
		int iStart, iEndX, iEndY;
		int iShape = Spot.m_iBrushShape;

		BOOL bEven = ! (iSize & 1);		// even width?

		if (Spot.m_bRect)		// we ARE drawing a rectangular area?
		{
			iStart = 0;
			iEndX  = Spot.m_ptImage2.x - iImageX;
			iEndY  = Spot.m_ptImage2.y - iImageY;
		}
		else					// no, just a spot
		{
			iStart = -iDelta;
			iEndX  = iEndY = iDelta2;
		}

		for (int y = iStart; y <= iEndY; y++)
		{
			for (int x = iStart; x <= iEndX; x++)
			{
				if (! Spot.m_bRect)		// are we drawing a spot?
					if (SkipPixel( x, y, iShape, iSize, iDelta, iDelta2, bEven, Spot.m_bHollow))
						continue;

				ix = iImageX + x;
				iy = iImageY + y;

				if ((iy >= 0) && (ix >= 0))
				{
					if ((iy < iTile*h) && (ix < iTile*w))
					{
						iIndex = ((iy % h) * w) + (ix % w);

						if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
							continue;

						if (iEditMode == EDIT_MODE_BLEND)
						{
							// center pixel
							irgbColor     = pDoc->m_pEffectLayer->GetPixel( iIndex);
							bIsFullBright = Spot.m_pPal->IsFullBright( GetIValue( irgbColor));

//							if (bIsFullBright)
//								MessageBeep( 0);

							int iRed   = GetRValue( irgbColor) * Spot.m_iFilterMult;
							int iGreen = GetGValue( irgbColor) * Spot.m_iFilterMult;
							int iBlue  = GetBValue( irgbColor) * Spot.m_iFilterMult;

							// horizontal and vertical neighbors

							// Neal - added "-1" to +w, +h (fixes asymetrical blend bug)

							// +w-1 instead of -1 to prevent MOD problems
							// neighbor LEFT
							iIndex    = ((iy % h) * w) + ((ix+w-1) % w);
							irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
							AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

							// neighbor RIGHT
							iIndex    = ((iy % h) * w) + ((ix+1) % w);
							irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
							AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

							// +h-1 instead of -1 to prevent MOD problems
							// neighbor ABOVE
							iIndex    = (((iy+h-1) % h) * w) + (ix % w);
							irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
							AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

							// neighbor BELOW
							iIndex    = (((iy+1) % h) * w) + (ix % w);
							irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
							AddColor( irgbColor, 2, &iRed, &iGreen, &iBlue);

							iRed   /= Spot.m_iFilterMult + 4*2;
							iGreen /= Spot.m_iFilterMult + 4*2;
							iBlue  /= Spot.m_iFilterMult + 4*2;

//							iColor = pPal->FindNearestColor( iRed, iGreen, iBlue, bIsFullBright);

							// Ty- yanked for now
							//ASSERT( iColor != 255);
//							irgbColor = IRGB( iColor, iRed, iGreen, iBlue);
							irgbColor = IRGB( 0, iRed, iGreen, iBlue);
						}
						else if ((iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
						{
							int iOffsetX = ix - pDoc->m_ptCloneStart.x + pDoc->m_ptCloneOffset.x;
							iOffsetX += 1000 * w;

							int iOffsetY = iy - pDoc->m_ptCloneStart.y + pDoc->m_ptCloneOffset.y;
							iOffsetY += 1000 * h;

							iIndex = ((iOffsetY % h) * w) + (iOffsetX % w);

							irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);
							int r     = GetRValue( irgbColor);
							int g     = GetGValue( irgbColor);
							int b     = GetBValue( irgbColor);

							if (iPercent < 100)
							{
								irgbColor = pDoc->GetWrappedPixel( ix, iy);

								r = ((r * iPercent) + ((100-iPercent) * GetRValue( irgbColor))) / 100;
								g = ((g * iPercent) + ((100-iPercent) * GetGValue( irgbColor))) / 100;
								b = ((b * iPercent) + ((100-iPercent) * GetBValue( irgbColor))) / 100;

//								iColor = pPal->FindNearestColor( r, g, b, FALSE);
							}

//							irgbColor = IRGB( iColor, r, g, b);
							irgbColor = IRGB( 0, r, g, b);
						}
						else
						{
							ASSERT( FALSE);
						}
//						pDoc->SetWrappedPixel( Spot.m_pView, ix, iy, irgbColor);
						pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, irgbColor, bIsFullBright);

						if (m_pbySpotMarker)
						{
							iIndex = ((iy % h) * w) + (ix % w);
							m_pbySpotMarker[iIndex] = 255;
						}
					}
				}		// End  if ((iy >= 0) && (ix >= 0))
			}			// End  for (int x = -iDelta; x <= iDelta2; x++)
		}				// End  for (int y = -iDelta; y <= iDelta2; y++)
	}					// End  else	// more than one pixel thick
}

///////////////////////////////////////////////////////////////////////
// Name:	SpraySpot
// Action:	Draws a spray paint spot at ptPos
///////////////////////////////////////////////////////////////////////
void CTool::SpraySpot( const CToolStrokeInfo& Spot)
{
	int ix, iy;
//	int iLookUp = iColor;		// neal used for effects like lighten/darken

	CWallyDoc* pDoc = Spot.m_pDoc;

	int w = pDoc->Width();
	int h = pDoc->Height();

	int  iSize       = Spot.m_iSize;
 	int  iDelta      = iSize / 2;
	int  iDelta2     = (iSize-1) / 2;
	int  iPercent    = 0;
	int  iTile       = (Spot.m_pView->m_bTile) ? 3 : 1;
	BOOL bEven       = ! (iSize & 1);		// even width?
	int  iTintAmount = Spot.m_iAmount;
	
	int    iSprayR = GetRValue( Spot.m_irgbColor);
	int    iSprayG = GetGValue( Spot.m_irgbColor);
	int    iSprayB = GetBValue( Spot.m_irgbColor);
	double dfPaintHue, dfPaintSaturation, dfPaintValue;

	int iEditMode = Spot.m_iEditMode;

	if (iEditMode == EDIT_MODE_SPRAY_RECOLOR)
	{
		dfPaintHue        = Spot.m_dfHue;
		dfPaintSaturation = Spot.m_dfSaturation;
		dfPaintValue      = Spot.m_dfValue;
	}

	// Neal - scale spray values

	iSprayR = iSprayR * iTintAmount;
	iSprayG = iSprayG * iTintAmount;
	iSprayB = iSprayB * iTintAmount;

	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;

	// Neal - speed up?
/*
	static bool bOneTime = TRUE;
#define MAX_TABLE 1001		// Neal - should be prime number
	static int  iRandomPercent[MAX_TABLE];
	static int  iNextNum = 0;
	if (bOneTime)
	{
		bOneTime = false;

		for (int i = 0; i < MAX_TABLE; i++)
			iRandomPercent[i] = (int )(Random() * 100.0);	// between 0 and 99
	}
*/

	int iStart, iEndX, iEndY;

	if (Spot.m_bRect)		// we ARE drawing a rectangular area?
	{
		iStart = 0;
		iEndX  = Spot.m_ptImage2.x - iImageX;
		iEndY  = Spot.m_ptImage2.y - iImageY;
	}
	else					// no, just a spot
	{
		iStart = -iDelta;
		iEndX  = iEndY = iDelta2;
	}

	for (int y = iStart; y <= iEndY; y++)
	{
		for (int x = iStart; x <= iEndX; x++)
		{
			if (! Spot.m_bRect)		// are we drawing a spot?
				if (SkipPixel( x, y, Spot.m_iBrushShape, iSize, iDelta, iDelta2, bEven, FALSE))
					continue;

			ix = iImageX + x;
			iy = iImageY + y;

			if ((iy >= 0) && (ix >= 0))
			{
				if ((iy < iTile*h) && (ix < iTile*w))
				{
					int iIndex = ((iy % h) * w) + (ix % w);

					if (iEditMode == EDIT_MODE_SPRAY_RECOLOR)
						if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
							continue;

					iPercent = (int )(Random() * 100.0);	// between 0 and 99
/*
					iPercent = iRandomPercent[iNextNum];	// Neal - speed up

					iNextNum++;
					if (iNextNum >= MAX_TABLE)
						iNextNum = 0;
*/
					if (iPercent < Spot.m_iCoverage)
					{
						// neal - spray is now fully 24 bit

						int r, g, b;
						COLOR_IRGB irgbMixColor;

						if (EDIT_MODE_SPRAY == iEditMode)
						{
							irgbMixColor = pDoc->GetWrappedPixel( ix, iy);

							r = GetRValue( irgbMixColor);
							g = GetGValue( irgbMixColor);
							b = GetBValue( irgbMixColor);

							// blend in a little bit of spray paint color

							r = (r * (100-iTintAmount) + iSprayR) / 100;
							g = (g * (100-iTintAmount) + iSprayG) / 100;
							b = (b * (100-iTintAmount) + iSprayB) / 100;

							//ClampRGB( &r, &g, &b);
						}
						else		// EDIT_MODE_SPRAY_RECOLOR == iEditMode
						{
							ASSERT (EDIT_MODE_SPRAY_RECOLOR == iEditMode);

							// Neal - fixes stroke build-up bug
							//
							irgbMixColor = pDoc->m_pEffectLayer->GetPixel( iIndex);

							double dfHue, dfSaturation, dfValue;
							CRGBtoHSV( irgbMixColor, &dfHue, &dfSaturation, &dfValue);

							// Neal - TODO: make these amounts ADJUSTABLE

							// Neal - now does a partial replacement of the brightness as well as hue
							//
							dfValue = 0.90 * dfValue + 0.10 * dfPaintValue;

							// Neal - now does a partial replacement of the saturation, too
							//
							// Neal - BUGFIX - gray was drawing as red (hue == 0)
							if (dfPaintSaturation > 0.05)
							{
								if (dfSaturation > 0.05)
								{
									if (fabs( dfHue - dfPaintHue) < 180.0)
										dfHue = 0.015 * dfHue + 0.985 * dfPaintHue;
									else
									{
										if (dfHue > dfPaintHue)
											dfHue = (0.015 * (dfHue-360.0) + 0.985 * dfPaintHue);
										else
											dfHue = (0.015 * dfHue + 0.985 * (dfPaintHue-360.0));

										if (dfHue < 0.0)
											dfHue += 360.0;
									}
								}
								else
								{
									dfHue = dfPaintHue;
								}
								ASSERT( (dfHue >= 0.0) && (dfHue <= 360.0));

								dfSaturation = 0.20 * dfSaturation + 0.80 * dfPaintSaturation;
							}
							else
								dfSaturation = 0.0;

							HSVtoRGB( dfHue, dfSaturation, dfValue, &r, &g, &b);
						}

						pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));

						if (m_pbySpotMarker)
							m_pbySpotMarker[iIndex] = 255;
					}
				}		// End  if Percent
			}		// End  if ((iy >= 0) && (ix >= 0))
		}			// End  for (int x = -iDelta; x <= iDelta2; x++)
	}				// End  for (int y = -iDelta; y <= iDelta2; y++)
}


void CTool::ReplaceSpot( const CToolStrokeInfo& Spot)
{
	int ix, iy, r, g, b;
	int iEffectsIndex = (Spot.m_bLeftButton) ? 0 : 1;
	int iReplaceR     = GetRValue( Spot.m_irgbReplaceColor);
	int iReplaceG     = GetGValue( Spot.m_irgbReplaceColor);
	int iReplaceB     = GetBValue( Spot.m_irgbReplaceColor);
	int iEditMode     = Spot.m_iEditMode;

	double dfReplaceHue        = Spot.m_dfReplaceHue;
	double dfReplaceSaturation = Spot.m_dfReplaceSaturation;

	double dfPaintHue        = Spot.m_dfHue;
	double dfPaintSaturation = Spot.m_dfSaturation;
	double dfPaintValue      = Spot.m_dfValue;

	CWallyDoc* pDoc = Spot.m_pDoc;

//	int iCheckColor;
	COLOR_IRGB irgbTempColor;
	int w = pDoc->Width();
	int h = pDoc->Height();

	int  iSize   = Spot.m_iSize;
	int  iDelta  = iSize / 2;
	int  iDelta2 = (iSize-1) / 2;
	int  iTile   = (Spot.m_pView->m_bTile) ? 3 : 1;
	BOOL bEven   = ! (iSize & 1);		// even width?

	int iStart, iEndX, iEndY;
	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;

	if (Spot.m_bRect)		// we ARE drawing a rectangular area?
	{
		iStart = 0;
		iEndX  = Spot.m_ptImage2.x - iImageX;
		iEndY  = Spot.m_ptImage2.y - iImageY;
	}
	else					// no, just a spot
	{
		iStart = -iDelta;
		iEndX  = iEndY = iDelta2;
	}
	int iRecolorTolerance = (g_iColorTolerance * 180) / 255;	// scaled to hue (0..360) / 2

	for (int y = iStart; y <= iEndY; y++)
	{
		for (int x = iStart; x <= iEndX; x++)
		{
			if (! Spot.m_bRect)		// are we drawing a spot?
				if (SkipPixel( x, y, Spot.m_iBrushShape, iSize, iDelta, iDelta2, bEven, Spot.m_bHollow))
					continue;

			ix = iImageX + x;
			iy = iImageY + y;

			if ((iy >= 0) && (ix >= 0))
			{
				if ((iy < iTile*h) && (ix < iTile*w))
				{
					int iIndex = ((iy % h) * w) + (ix % w);

					if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
						continue;

					// neal - this is now full 24 bit

					BOOL bPassed = FALSE;

					if (IsAnyEffect( iEditMode))
					{
						// check hue +/- delta

						COLOR_IRGB irgbCheckColor = pDoc->m_pEffectLayer->GetPixel( iIndex);

						double dfHue, dfSaturation, dfValue;
						CRGBtoHSV( irgbCheckColor, &dfHue, &dfSaturation, &dfValue);

						if ((dfReplaceSaturation > 0.05) || (dfSaturation > 0.05))
						{
							double dfDelta = dfHue - dfReplaceHue;

							if (dfDelta < 0.0)
								dfDelta = -dfDelta;

							if (dfDelta > 360.0)
								dfDelta -= 360.0;

							if (dfDelta < 5.0 + iRecolorTolerance)
								bPassed = TRUE;
						}
						else
						{
							//if ((dfReplaceSaturation <= 0.05) && (dfSaturation <= 0.05))
								bPassed = TRUE;
						}
						if (bPassed)
						{
							// Neal - TODO: make these amounts ADJUSTABLE

							// Neal - now does a partial replacement of the brightness as well as hue
							//
							dfValue = 0.90 * dfValue + 0.10 * dfPaintValue;

							// Neal - now does a partial replacement of the saturation, too
							//
							// Neal - BUGFIX - gray was drawing as red (hue == 0)
							if (dfPaintSaturation > 0.05)
							{
								if (dfSaturation > 0.05)
								{
									if (fabs( dfHue - dfPaintHue) < 180.0)
										dfHue = 0.015 * dfHue + 0.985 * dfPaintHue;
									else
									{
										if (dfHue > dfPaintHue)
											dfHue = (0.015 * (dfHue-360.0) + 0.985 * dfPaintHue);
										else
											dfHue = (0.015 * dfHue + 0.985 * (dfPaintHue-360.0));

										if (dfHue < 0.0)
											dfHue += 360.0;
									}
								}
								else
								{
									dfHue = dfPaintHue;
								}
								ASSERT( (dfHue >= 0.0) && (dfHue <= 360.0));

								dfSaturation = 0.20 * dfSaturation + 0.80 * dfPaintSaturation;
							}
							else
								dfSaturation = 0.0;

							HSVtoRGB( dfPaintHue, dfSaturation, dfValue, &r, &g, &b);
							irgbTempColor = IRGB( Spot.m_pPal->FindNearestColor( r, g, b, 
										(GetIValue( irgbCheckColor) == 255)), r, g, b);
						}
					}
					else 
					{
						COLOR_IRGB irgb = pDoc->GetWrappedPixel( ix, iy);

						r = GetRValue( irgb);
						g = GetGValue( irgb);
						b = GetBValue( irgb);

						if ((iReplaceR >= r - g_iColorTolerance) && (iReplaceR <= r + g_iColorTolerance) &&
								(iReplaceG >= g - g_iColorTolerance) && (iReplaceG <= g + g_iColorTolerance) &&
								(iReplaceB >= b - g_iColorTolerance) && (iReplaceB <= b + g_iColorTolerance))
						{
							bPassed       = TRUE;
							irgbTempColor = Spot.m_irgbColor;
						}
					}
					if (bPassed)
					{
						pDoc->SetWrappedPixel( Spot.m_pView, ix, iy, irgbTempColor);
					}

					if (m_pbySpotMarker)
						m_pbySpotMarker[iIndex] = 255;

				}	// End  if Percent
			}		// End  if ((iy >= 0) && (ix >= 0))
		}			// End  for (int x = -iDelta; x <= iDelta2; x++)
	}				// End  for (int y = -iDelta; y <= iDelta2; y++)
}

void CTool::PatternedSpot( const CToolStrokeInfo& Spot)
{
	CWallyDoc* pDoc = Spot.m_pDoc;

	int iPercent = Spot.m_iAmount;
	int ix, iy;
	int w = pDoc->Width();
	int h = pDoc->Height();

	int iDecalWidth  = 0;
	int iDecalHeight = 0;

	int  iSize   = Spot.m_iSize;
	int  iDelta  = iSize / 2;
	int  iDelta2 = (iSize-1) / 2;
	int  iTile   = (Spot.m_pView->m_bTile) ? 3 : 1;
	BOOL bEven   = ! (iSize & 1);		// even width?

	COLOR_IRGB* pIRGB_Buffer = NULL;

	if (Spot.m_pView->m_bLeftButtonTracking && g_LeftPatternToolLayerInfo.HasData())
	{
		pIRGB_Buffer = g_LeftPatternToolLayerInfo.GetData();
		iDecalWidth  = g_LeftPatternToolLayerInfo.GetWidth();
		iDecalHeight = g_LeftPatternToolLayerInfo.GetHeight();
	}
	else if (Spot.m_pView->m_bRightButtonTracking && g_RightPatternToolLayerInfo.HasData())
	{
		pIRGB_Buffer = g_RightPatternToolLayerInfo.GetData();
		iDecalWidth  = g_RightPatternToolLayerInfo.GetWidth();
		iDecalHeight = g_RightPatternToolLayerInfo.GetHeight();
	}
	else
	{
//		ASSERT( FALSE);
	}

	DWORD dwFlags = FLAG_DECAL_USE_EFFECT_BUFFER;

	// TODO - implement
	// if (this decal uses left/right button colors)
		dwFlags |= FLAG_DECAL_USE_DRAWING_COLORS;

	int iStart, iEndX, iEndY;
	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;

	if (Spot.m_bRect)		// we ARE drawing a rectangular area?
	{
		iStart = 0;
		iEndX  = Spot.m_ptImage2.x - iImageX;
		iEndY  = Spot.m_ptImage2.y - iImageY;
	}
	else					// no, just a spot
	{
		iStart = -iDelta;
		iEndX  = iEndY = iDelta2;
	}

	for (int y = iStart; y <= iEndY; y++)
	{
		iy = iImageY + y;

		int iDecalY = iy + h*100;	// negative modulus fix

		for (int x = iStart; x <= iEndX; x++)
		{
			if (! Spot.m_bRect)		// are we drawing a spot?
				if (SkipPixel( x, y, Spot.m_iBrushShape, iSize, iDelta, iDelta2, bEven, Spot.m_bHollow))
					continue;

			ix = iImageX + x;

			if ((iy >= 0) && (ix >= 0))
			{
				if ((iy < iTile*h) && (ix < iTile*w))
				{
					COLOR_IRGB irgbColor = COLOR_DEBUG_PURPLE;

					int iIndex = ((iy % h) * w) + (ix % w);

					if ((m_pbySpotMarker) && (m_pbySpotMarker[iIndex] != 0))
						continue;

					if (pIRGB_Buffer)
					{
						int iDecalX = (g_bFlipped) ? w-ix : ix;
						iDecalX += w*100;	// negative modulus fix

						irgbColor = pIRGB_Buffer[
									(iDecalY%iDecalHeight)*iDecalWidth + (iDecalX%iDecalWidth)];

						if (Spot.m_bLeftButton)
						{
							if (! g_LeftPatternToolLayerInfo.GetAppliedDecalPixelRGB( 
//									pDoc, g_LeftPatternToolLayerInfo.GetPalette(),
									pDoc, Spot.m_pPal,
									&irgbColor, ix, iy, iPercent, dwFlags))
							continue;
						}
						else
						{
							if (! g_RightPatternToolLayerInfo.GetAppliedDecalPixelRGB( 
//									pDoc, g_LeftPatternToolLayerInfo.GetPalette(),
									pDoc, Spot.m_pPal,
									&irgbColor, ix, iy, iPercent, dwFlags))
							continue;
						}

						//iColor = GetIValue( irgb);
						//iColor = Spot.m_pPal->FindNearestColor( GetRValue( irgb),
						//		GetGValue( irgb), GetBValue( irgb), FALSE);
					}
					else	// old method
					{
						int iSign   = (g_bFlipped) ? -1 : 1;
						int iOffset = (iy + iSign * ix);
						int iSize   = (Spot.m_bLeftButton) ? 4 : 8;

						if (iOffset & iSize)
						{
							if ((iOffset & (iSize-1)) != 0)
								irgbColor = Spot.m_irgbColor;
							else
								irgbColor = g_irgbMixColor;
						}
						else
						{
							if ((iOffset & (iSize-1)) != 0)
								irgbColor = Spot.m_irgbColor2;
							else
								irgbColor = g_irgbMixColor;
						}
					}
					pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, irgbColor, FALSE);

					if (m_pbySpotMarker)
						m_pbySpotMarker[iIndex] = 255;

				}	// End  if ((iy < iTile*h) && (ix < iTile*w))
			}		// End  if ((iy >= 0) && (ix >= 0))
		}			// End  for (int x = -iDelta; x <= iDelta2; x++)
	}				// End  for (int y = -iDelta; y <= iDelta2; y++)
}

void CTool::DecalSpot( const CToolStrokeInfo& Spot)
{
	CWallyDoc* pDoc = Spot.m_pDoc;

	int iWidth  = pDoc->Width();
	int iHeight = pDoc->Height();

	// neal - BUGFIX - don't draw rivets when pos is out-of-bounds

	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;
	int i       = (Spot.m_pView->m_bTile) ? 3 : 1;

	if ((iImageX < 0) || (iImageY < 0) ||
				(iImageX >= iWidth * i) || (iImageY >= iHeight * i))
		return;

	CPoint ptTemp( iImageX, iImageY);

//	BOOL bDraw = FALSE;

	// avoid "over-painting"
//	if (bDraw)
//	{
//		bDraw = ((iMouseCode == MOUSE_BUTTON_DOWN) || 
//				(ptTemp.x != ptLastPos.x) || (ptTemp.y != ptLastPos.y));
//	}

//	if (bDraw)
	{
		int iPercent = Spot.m_iAmount;

		DWORD dwFlags = FLAG_DECAL_USE_EFFECT_BUFFER;

		// TODO - implement
		// if (this decal uses left/right button colors)
		//	dwFlags |= FLAG_DECAL_USE_DRAWING_COLORS;

		if (Spot.m_bLeftButton)
		{
			if (g_LeftDecalToolLayerInfo.HasData())
			{
				if (g_LeftDecalToolLayerInfo.DrawDecal( pDoc, 
							ptTemp.x, ptTemp.y, 0, iPercent, dwFlags, NULL))
				{
					ptLastPos = ptTemp;
				}
			}
			else
			{
				ASSERT( FALSE);
			}
		}
		else
		{
			if (g_RightDecalToolLayerInfo.HasData())
			{
				if (g_RightDecalToolLayerInfo.DrawDecal( pDoc, 
							ptTemp.x, ptTemp.y, 0, iPercent, dwFlags, NULL))
				{
					ptLastPos = ptTemp;
				}
			}
			else
			{
				ASSERT( FALSE);
			}
		}
	}
}

BOOL CTool::DrawBulletHole( const CToolStrokeInfo& Spot)
{
	CWallyDoc* pDoc     = Spot.m_pDoc;
	int        iPercent = Spot.m_iAmount;
	BOOL       bRet     = FALSE;

	if (Spot.m_iMouseCode == MOUSE_BUTTON_DOWN)
		g_iBulletIndex = -1;

	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;

	DWORD dwFlags = 0;

	// TODO - implement
	// if (this decal usese left/right button colors)
		dwFlags |= FLAG_DECAL_USE_DRAWING_COLORS;

	if (Spot.m_bLeftButton)		// small bullet hole
	{
		if (g_LeftBulletLayerInfo.HasData())
		{
			bRet = g_LeftBulletLayerInfo.DrawDecal( pDoc, 
						iImageX, iImageY, 0, iPercent, dwFlags, &g_iBulletIndex);
		}
	}
	else	// right button => large bullet hole
	{
		if (g_RightBulletLayerInfo.HasData())
		{
			bRet = g_RightBulletLayerInfo.DrawDecal( pDoc, 
						iImageX, iImageY, 0, iPercent, dwFlags, &g_iBulletIndex);
		}
	}

	if (bRet)
	{
		ptLastPos.x = iImageX;
		ptLastPos.y = iImageY;

		return bRet;
	}

	static BYTE byBulletData[8][7*7] =
	{
		// Little Bullet Holes (for left button)
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 111, 128, 128, 128,
			128, 128, 149,   0, 105, 128, 128,
			128, 128, 128, 198, 149, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 105,  43, 128, 128, 128,
			105,  86,  55,   0, 149, 128, 128,
			128, 128, 128, 149, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 111, 128, 128, 128, 128,
			128, 128, 128, 111, 128, 128, 128,
			128, 128, 105,  62, 128, 128, 128,
			128, 128, 128, 174, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 105, 128, 128, 128,
			128, 128, 105,  62, 128, 128, 128,
			128, 128,  62,  30, 149, 128, 128,
			128, 128, 128, 186, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},

		// Big Bullet Holes (for right button)

		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128,  62,  62, 128, 128, 128,
			128,  62,   0,   0, 105, 128, 128,
			128, 105,   0,   0, 149, 128, 128,
			128, 128, 149, 198, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
		{
			128, 111,  85,  85, 105, 128, 128,
			111,  85,  62,  62,  85, 105, 128,
			 85,  62,   0,   0,  62, 111, 136,
			 85,  62,   0,   0,  62, 105, 161,
			111,  85,  62,  62,  85, 111, 149,
			149, 105, 111, 105, 111, 149, 136,
			128, 149, 180, 180, 149, 136, 128
		},
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128,  62,  86,  62, 128, 128,
			128,  62,  43,   0,  62, 149, 128,
			128,  86,   0,   0,  43,  85, 128,
			128, 149,  62,  43,  85, 180, 128,
			128, 128, 149,  85, 149, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
		{
			128, 128, 128, 128, 128, 128, 128,
			128, 128, 111, 105, 105, 128, 128,
			128, 111,  49,  62, 105, 149, 128,
			128, 105,  62,  37, 105, 161, 128,
			128, 111,  86, 111, 128, 180, 128,
			128, 128, 149, 161, 149, 128, 128,
			128, 128, 128, 128, 128, 128, 128
		},
	};

	// pick a random bullet decal on every mouse down

	static int iRandBullet;

	if (Spot.m_iMouseCode == MOUSE_BUTTON_DOWN)
	{
		iRandBullet = (int )(Random() * 4);		// 0..3

		ASSERT( (iRandBullet >= 0) && (iRandBullet <= 3));
	}

	int iBullet = iRandBullet;

	if (! Spot.m_bLeftButton)
		iBullet += 4;					// pick a big bullet hole

	for (int j = 0; j < 7; j++)
	{
		for (int i = 0; i < 7; i++)
		{
			int t = byBulletData[iBullet][j*7 + i] - 128;

			if (t)
			{
				// scale range by AMOUNT

				t = (t * 2 * iPercent) / 100;

				int ix = iImageX + i - 3; 
				int iy = iImageY + j - 3;

				COLOR_IRGB irgbColor = pDoc->GetWrappedPixel( ix, iy);

				int r = GetRValue( irgbColor) + t;
				int g = GetGValue( irgbColor) + t;
				int b = GetBValue( irgbColor) + t;

				ClampRGB( &r, &g, &b);

				pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));
			}
		}
	}
	ptLastPos.x = iImageX;
	ptLastPos.y = iImageY;
	return TRUE;
}

BOOL CTool::DrawRivet( const CToolStrokeInfo& Spot)
{
	CWallyDoc* pDoc    = Spot.m_pDoc;
	int        iWidth  = pDoc->Width();
	int        iHeight = pDoc->Height();

	// neal - BUGFIX - don't draw rivets when pos is out-of-bounds

	int iImageX = Spot.m_ptImage.x;
	int iImageY = Spot.m_ptImage.y;
	int i       = (Spot.m_pView->m_bTile) ? 3 : 1;

	if ((iImageX < 0) || (iImageY < 0) ||
				(iImageX >= iWidth * i) || (iImageY >= iHeight * i))
		return FALSE;

	CPoint ptTemp( iImageX, iImageY);

	BOOL bDraw = FALSE;

	// MUST be a POWER-OF-TWO >= 4  (4,8,16,32, etc.)
	//int iSpacing = 8;
	int iSpacing = g_iRivetDistance;

	if (Spot.m_bLeftButton)
	{
		int iMask = (iSpacing / 4) - 1;
		iMask     = max( iMask, 1);

		// make it easier to hit "magic spots"

		ptTemp.x += iMask;			// was 3
		ptTemp.y += iMask;

		//iMask = (iSpacing / 2) - 1;
		//iMask = max( iMask, 3);
		iMask = iSpacing - 1;

		// drop bottom bits

		ptTemp.x &= ~iMask;			// was 7
		ptTemp.y &= ~iMask;

		ptTemp.x += (iSpacing / 2);
		ptTemp.y += (iSpacing / 2);

		if (iSpacing > 4)
		{
//			iMask    = iSpacing - 1;	// was 0x0F
//			iSpacing /= 2;				// was 8
//
//			bDraw = (((ptTemp.x & iMask) == iSpacing) && ((ptTemp.y & iMask) == iSpacing));

			int k = iSpacing / 4;
			bDraw = ((iImageX >= ptTemp.x - k) && (iImageX <= ptTemp.x + k) &&
					 (iImageY >= ptTemp.y - k) && (iImageY <= ptTemp.y + k));
		}
		else
			bDraw = TRUE;
	}
	else
	{
		ASSERT( ! Spot.m_bLeftButton);

		bDraw = (Spot.m_iMouseCode == MOUSE_BUTTON_DOWN);
	}

	// avoid "over-painting"
	if (bDraw)
		bDraw = ((Spot.m_iMouseCode == MOUSE_BUTTON_DOWN) || 
				(ptTemp.x != ptLastPos.x) || (ptTemp.y != ptLastPos.y));

	if (bDraw)
	{
		int iPercent = Spot.m_iAmount;

		if (g_RivetToolLayerInfo.HasData())
		{
			DWORD dwFlags = FLAG_DECAL_CLEAR_BACKGROUND | FLAG_DECAL_USE_EFFECT_BUFFER;

			// TODO - implement
			// if (this decal usese left/right button colors)
				dwFlags |= FLAG_DECAL_USE_DRAWING_COLORS;

			BOOL bRet = g_RivetToolLayerInfo.DrawDecal( pDoc, 
						ptTemp.x, ptTemp.y, iSpacing, iPercent, dwFlags, NULL);

			if (bRet)
				ptLastPos = ptTemp;

			return bRet;
		}

		int iWidth  = 4;
		int iHeight = 6;

		int iStripStart = 0;
		int iStripWidth = iWidth;

		static BYTE byRivetData[4*6] =
		{
			149, 254, 230, 149,
			180, 149, 128, 167,
			 74, 128, 105,  49,
			 99,   0,   0,  99,
			111,  86,  80, 111,
			128, 105, 111, 128
		};

		int iOffsetX = iWidth  / 2;
		int iOffsetY = iHeight / 2;
		int k        = (iSpacing > 4) ? iHeight : 4;

		for (int j = 0; j < k; j++)
		{
			for (int i = 0; i < iWidth; i++)
			{
				int t, r, g, b, iIndex;

				t = byRivetData[j*iStripWidth + i] - 128;

				if (t)
				{
					// scale range by AMOUNT

					t = (t * 2 * iPercent) / 100;

					int ix = ptTemp.x + i - iOffsetX;
					int iy = ptTemp.y + j - iOffsetY;

					iIndex = pDoc->GetWrappedIndex( ix, iy);

					COLOR_IRGB irgbColor = pDoc->m_pEffectLayer->GetPixel( iIndex);

					r = GetRValue( irgbColor) + t;
					g = GetGValue( irgbColor) + t;
					b = GetBValue( irgbColor) + t;

					ClampRGB( &r, &g, &b);
					//pDoc->SetMipPixel( pView, iIndex, r, g, b);
					pDoc->SetNearestColorWrappedPixel( Spot.m_pView, ix, iy, RGB( r, g, b));
				}
			}
		}
		ptLastPos = ptTemp;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Name:		DrawBigCursor
// Action:		Draws a zoomed brush-shaped tool cursor at ptPos
//
// Parameters:	ptImage:	point in image coordinates
//				iSize:		line thickness
///////////////////////////////////////////////////////////////////////
void CTool::DrawBigCursor( CWallyView* pView, CPoint ptImage, int iSize, bool bHollow)
{
	if (bHollow)
	{
		m_ptLastHollowCursorPos = ptImage;
		m_iLastHollowCursorSize = iSize;
	}
	else
	{
		m_ptLastBigCursorPos = ptImage;
		m_iLastBigCursorSize = iSize;
	}

	CWallyDoc*     pDoc = (CWallyDoc *)(pView->GetDocument());
	CWallyPalette* pPal = pDoc->GetPalette();

	int  ix, iy;
	int  w          = pDoc->Width();
	int  h          = pDoc->Height();
	int  iNumBits   = pDoc->GetNumBits();
	int  iPixelSize = iNumBits / 8;
	int  iPadWidth  = PadDWORD( w * iPixelSize);
	int  iDelta     = iSize / 2;
	int  iDelta2    = (iSize-1) / 2;
	int  iShape     = g_iBrushShape;
	int  iTile      = (pView->m_bTile) ? 3 : 1;
	BOOL bEven      = ! (iSize & 1);		// even width?

	// Ty - switched static pointer to DIB bits with dynamic
	BYTE* pbyDibBits = (BYTE *)(pView->m_DIB[0]);

	for (int y = -iDelta; y <= iDelta2; y++)
	{
		for (int x = -iDelta; x <= iDelta2; x++)
		{
			if (SkipPixel( x, y, iShape, iSize, iDelta, iDelta2, bEven, bHollow))
				continue;

			ix = ptImage.x + x;
			iy = ptImage.y + y;

			// neal - fix tiled cursor bug
			if (((iy >= 0) && (ix >= 0)) || (pView->m_bTile))
			{
				if (((iy < h) && (ix < w)) || (pView->m_bTile))
				{
					// neal - TODO: update to full 24 bit (per channel)

					COLOR_IRGB irgbColor = pDoc->GetWrappedPixel( ix, iy);
					int        iColor    = g_byCursorLookUp[ GetIValue( irgbColor)];

					// neal - fix negative modulus problem
					ix += 1000 * w;
					iy += 1000 * h;

					int iIndex = ((iy % h) * iPadWidth) + (iPixelSize * (ix % w));

					if (iPixelSize == 1)
					{
						pbyDibBits[iIndex] = iColor;
					}
					else
					{
						pbyDibBits[iIndex  ] = pPal->GetR( iColor);
						pbyDibBits[iIndex+1] = pPal->GetG( iColor);
						pbyDibBits[iIndex+2] = pPal->GetB( iColor);
					}
				}
			}		// End  if ((iy >= 0) && (ix >= 0))
		}			// End  for (int x = -iDelta; x <= iDelta2; x++)
	}				// End  for (int y = -iDelta; y <= iDelta2; y++)
}

///////////////////////////////////////////////////////////////////////
// Name:		ClearBigCursor
// Action:		Clears the zoomed brush-shaped tool cursor at ptPos
//
// Parameters:	ptImage:	point in image coordinates
//				iSize:		line thickness
///////////////////////////////////////////////////////////////////////
void CTool::ClearBigCursor( CWallyView* pView)
{
	if ((m_iLastBigCursorSize <= 0) && (m_iLastHollowCursorSize <= 0))
		return;

	pView->KillTimer( TIMER_TOOL_CLEAR_CURSOR);

	CWallyDoc* pDoc = (CWallyDoc *)(pView->GetDocument());

	// Ty - switched static pointer to DIB bits with dynamic one
	BYTE* pbyDibBits = (BYTE *)(pView->m_DIB[0]);

	int        ix, iy;
	int        iIndex, iColor;
	COLOR_IRGB irgbColor;

	int w          = pDoc->Width();
	int h          = pDoc->Height();
	int iNumBits   = pDoc->GetNumBits();
	int iPixelSize = iNumBits / 8;
	int iPadWidth  = PadDWORD( w * iPixelSize);
	int iDelta     = m_iLastBigCursorSize / 2;
	int iDelta2    = (m_iLastBigCursorSize-1) / 2;
	int iTile      = (pView->m_bTile) ? 3 : 1;

	if (m_iLastBigCursorSize > 0)
	{
		for (int y = -iDelta; y <= iDelta2; y++)
		{
			for (int x = -iDelta; x <= iDelta2; x++)
			{
				ix = m_ptLastBigCursorPos.x + x;
				iy = m_ptLastBigCursorPos.y + y;

				// neal - fix tiled cursor bug
				if (((iy >= 0) && (ix >= 0)) || (pView->m_bTile))
				{
					if (((iy < h) && (ix < w)) || (pView->m_bTile))
					{
						irgbColor = pDoc->GetWrappedPixel( ix, iy);

						// neal - fix negative modulus problem
						ix += 1000 * w;
						iy += 1000 * h;

						iIndex = ((iy % h) * iPadWidth) + (iPixelSize * (ix % w));

						if (iPixelSize == 1)
						{
							iColor             = GetIValue( irgbColor);
							pbyDibBits[iIndex] = iColor;
						}
						else
						{
							// Neal - this is CORRECT: remember DIBs are BGR not RGB

							pbyDibBits[iIndex  ] = GetBValue( irgbColor);
							pbyDibBits[iIndex+1] = GetGValue( irgbColor);
							pbyDibBits[iIndex+2] = GetRValue( irgbColor);
						}
					}
				}		// End  if ((iy >= 0) && (ix >= 0))
			}			// End  for (int x = -iDelta; x <= iDelta2; x++)
		}				// End  for (int y = -iDelta; y <= iDelta2; y++)
	}

	if (m_iLastHollowCursorSize > 0)
	{
		int iDelta  = m_iLastHollowCursorSize / 2;
		int iDelta2 = (m_iLastHollowCursorSize-1) / 2;

		for (int y = -iDelta; y <= iDelta2; y++)
		{
			for (int x = -iDelta; x <= iDelta2; x++)
			{
				ix = m_ptLastHollowCursorPos.x + x;
				iy = m_ptLastHollowCursorPos.y + y;

				// neal - fix tiled cursor bug
				if (((iy >= 0) && (ix >= 0)) || (pView->m_bTile))
				{
					if (((iy < h) && (ix < w)) || (pView->m_bTile))
					{
						irgbColor = pDoc->GetWrappedPixel( ix, iy);

						// neal - fix negative modulus problem
						ix += 1000 * w;
						iy += 1000 * h;

						iIndex = ((iy % h) * iPadWidth) + (iPixelSize * (ix % w));

						if (iPixelSize == 1)
						{
							iColor             = GetIValue( irgbColor);
							pbyDibBits[iIndex] = iColor;
						}
						else
						{
							// Neal - this is CORRECT: remember DIBs are BGR not RGB

							pbyDibBits[iIndex  ] = GetBValue( irgbColor);
							pbyDibBits[iIndex+1] = GetGValue( irgbColor);
							pbyDibBits[iIndex+2] = GetRValue( irgbColor);
						}
					}
				}		// End  if ((iy >= 0) && (ix >= 0))
			}			// End  for (int x = -iDelta; x <= iDelta2; x++)
		}				// End  for (int y = -iDelta; y <= iDelta2; y++)
	}

	// prevents extra clears

	m_iLastBigCursorSize    = 0;
	m_iLastHollowCursorSize = 0;
}

///////////////////////////////////////////////////////////////////////
// Name:		 
// Action:	Draws a line with the current tool at ptPos
///////////////////////////////////////////////////////////////////////
void CTool::DrawLine( CToolStrokeInfo* pStroke)
{
	switch (pStroke->m_iEditMode)
	{
	case EDIT_MODE_REPLACE:
	case EDIT_MODE_RECOLOR:
		ReplaceLine( pStroke);
		break;

	case EDIT_MODE_PATTERNED_PAINT:
		PatternedLine( pStroke);
		break;

	case EDIT_MODE_DECAL:
		DecalLine( pStroke);
		break;

	case EDIT_MODE_SPRAY:
	case EDIT_MODE_SPRAY_RECOLOR:
		SprayLine( pStroke);
		break;

	case EDIT_MODE_SCRATCH:
		{
			CToolStrokeInfo ScratchStroke = *pStroke;

			CPoint ptImage1( ScratchStroke.m_ptImage);
			CPoint ptImage2( ScratchStroke.m_ptImage2);
			CPoint ptPos1( ptImage1);
			CPoint ptPos2( ptImage2);

			// first order smoothing gives better slope
			if (GetDrawingMode( ScratchStroke.m_iEditMode) == MODE_FREEHAND)
				ptPos2 = ptTrail;

			int iH = abs( ptPos1.x - ptPos2.x);
			int iV = abs( ptPos1.y - ptPos2.y);

			BOOL bVert = (iV > iH);
			BOOL bHorz = (iV < iH);

			// single point or diagonal line?
			if (! (bVert || bHorz))
			{
				// just a point?
				// or true diagonal line (not rounded corner)?
				if ((ptPos1 == ptPos2) || (max( iV, iH) > 1))
				{
					bHorz = TRUE;
				}
				else if (ptPos1.y > ptPos2.y)
				{
					// there's only a problem drawing upwards
					// (diagonals can overwrite previous segment)
					bHorz = TRUE;
				}
			}

			// mostly vertical?
			if (bVert)
			{
				// right edge
				ScratchStroke.m_ptImage.x  = ptImage1.x + 1;
				ScratchStroke.m_ptImage.y  = ptImage1.y;
				ScratchStroke.m_ptImage2.x = ptImage2.x + 1;
				ScratchStroke.m_ptImage2.y = ptImage2.y;
			}
			// mostly horizontal?
			else if (bHorz)
			{
				// bottom edge
				ScratchStroke.m_ptImage.x  = ptImage1.x;
				ScratchStroke.m_ptImage.y  = ptImage1.y + 1;
				ScratchStroke.m_ptImage2.x = ptImage2.x;
				ScratchStroke.m_ptImage2.y = ptImage2.y + 1;
			}

			// highlight
			ScratchStroke.m_iScratchType = SCRATCH_TYPE_LIGHTEN;
			PaintLine( &ScratchStroke);

			// groove
			if (ScratchStroke.m_bLeftButton)	// shallow scratch
			{
				ScratchStroke.m_ptImage  = ptImage1;
				ScratchStroke.m_ptImage2 = ptImage2;
			}
			else
			{
				// mostly vertical?
				if (bVert)
				{
					// right edge
					ScratchStroke.m_ptImage.x  = ptImage1.x - 1;
					ScratchStroke.m_ptImage.y  = ptImage1.y;
					ScratchStroke.m_ptImage2.x = ptImage2.x - 1;
					ScratchStroke.m_ptImage2.y = ptImage2.y;
				}
				// mostly horizontal?
				else if (bHorz)
				{
					// bottom edge
					ScratchStroke.m_ptImage.x  = ptImage1.x;
					ScratchStroke.m_ptImage.y  = ptImage1.y - 1;
					ScratchStroke.m_ptImage2.x = ptImage2.x;
					ScratchStroke.m_ptImage2.y = ptImage2.y - 1;
				}
			}
			ScratchStroke.m_iScratchType = SCRATCH_TYPE_DARKEN;
			PaintLine( &ScratchStroke);

			if (! ScratchStroke.m_bLeftButton)	// deep scratch
			{
				ScratchStroke.m_iScratchType = SCRATCH_TYPE_DEEP_GROOVE;
				ScratchStroke.m_ptImage  = ptImage1;
				ScratchStroke.m_ptImage2 = ptImage2;
				ScratchStroke.m_iAmount -= 20;

				PaintLine( &ScratchStroke);
			}
		}
		break;

	default:
		PaintLine( pStroke);
		break;
	}
}

///////////////////////////////////////////////////////////////////////
// Name:	DrawSolidRectangle
// Action:	Draws a rectangle with the current tool at ptPos
///////////////////////////////////////////////////////////////////////
void CTool::DrawSolidRectangle( CToolStrokeInfo* pStroke)
{
	pStroke->m_bRect = TRUE;

	switch (pStroke->m_iEditMode)
	{
	case EDIT_MODE_REPLACE:
	case EDIT_MODE_RECOLOR:
		ReplaceSpot( *pStroke); 
		break;

	case EDIT_MODE_BLEND:
	case EDIT_MODE_CLONE:
	case EDIT_MODE_RUBBER_STAMP:
		FilterSpot( *pStroke); 
		break;

	case EDIT_MODE_PATTERNED_PAINT:
		{
			int r = (GetRValue( pStroke->m_irgbColor) + GetRValue( pStroke->m_irgbColor2)) / 2;
			int g = (GetGValue( pStroke->m_irgbColor) + GetGValue( pStroke->m_irgbColor2)) / 2;
			int b = (GetBValue( pStroke->m_irgbColor) + GetBValue( pStroke->m_irgbColor2)) / 2;
			g_irgbMixColor = IRGB( pStroke->m_pPal->FindNearestColor( r, g, b, FALSE), r, g, b);

			PatternedSpot( *pStroke);
		}
		break;

	case EDIT_MODE_SPRAY:
	case EDIT_MODE_SPRAY_RECOLOR:
		pStroke->m_iCoverage = SPRAY_COVERAGE*2;
		SpraySpot( *pStroke);
		break;

	case EDIT_MODE_SCRATCH:
		{
			pStroke->m_iSize = 1;

			int iStart = pStroke->m_ptImage.y;
			int iStop  = pStroke->m_ptImage2.y;

			for (int j = iStart; j <= iStop; j++)
			{
				// draw separated horizontal scratches
				if ((j & 3) == 1)
				{
					pStroke->m_ptImage.y = pStroke->m_ptImage2.y = j;
					DrawLine( pStroke);
				}
			}
		}
		break;

	default:
		DrawSpot( *pStroke);
		break;
	}
}

///////////////////////////////////////////////////////////////////////
// Name:	DrawStroke
// Action:	Draws a stroke with the current tool at ptPos
///////////////////////////////////////////////////////////////////////
void CTool::DrawStroke( CToolStrokeInfo* pStroke)
{
	int             iEditMode  = pStroke->m_iEditMode;
	BOOL            bDraw      = TRUE;
	CToolStrokeInfo StrokeInfo = *pStroke;

	ASSERT( pStroke->m_pView->m_bLeftButtonTracking || 
			pStroke->m_pView->m_bRightButtonTracking);	// should be tracking left or right

	g_ptLine1 = pStroke->m_ptImage;
	g_ptLine2 = pStroke->m_ptImage2;

	CPoint ptMin( min( g_ptLine1.x, g_ptLine2.x), min( g_ptLine1.y, g_ptLine2.y));
	CPoint ptMax( max( g_ptLine1.x, g_ptLine2.x), max( g_ptLine1.y, g_ptLine2.y));
	CPoint ptTemp;

	int iDrawingMode = GetDrawingMode( pStroke->m_iEditMode);

	switch (iDrawingMode)
	{
	case MODE_FREEHAND:
	case MODE_LINE:
	case MODE_POLYLINE:
	case MODE_RAY:
		DrawLine( &StrokeInfo);
		break;

	case MODE_SOLID_RECT:
	case MODE_EDGED_RECT:
		StrokeInfo.m_ptImage  = ptMin;
		StrokeInfo.m_ptImage2 = ptMax;
		DrawSolidRectangle( &StrokeInfo);

		if (iDrawingMode == MODE_SOLID_RECT)
			break;

		// Neal - fixes border of edged solid rect
		//
		if (m_pbySpotMarker && ToolUsesSpotMarker( pStroke->m_iEditMode))
		{
			int iWidth  = pStroke->m_pDoc->Width();
			int iHeight = pStroke->m_pDoc->Height();

			ZeroMemory( m_pbySpotMarker, iWidth * iHeight);
		}
		// drop thru

	case MODE_HOLLOW_RECT:

		if ((iDrawingMode == MODE_EDGED_RECT) &&
					(iEditMode != EDIT_MODE_RECOLOR) &&
					(iEditMode != EDIT_MODE_REPLACE))
		{
			StrokeInfo.m_irgbColor = StrokeInfo.m_irgbColor2;
		}
		StrokeInfo.m_bRect = FALSE;

		ptTemp.x = ptMax.x;
		ptTemp.y = ptMin.y;

		// top edge
		StrokeInfo.m_ptImage  = ptMin;
		StrokeInfo.m_ptImage2 = ptTemp;
		DrawLine( &StrokeInfo);
		// right edge
		StrokeInfo.m_ptImage  = ptTemp;
		StrokeInfo.m_ptImage2 = ptMax;
		DrawLine( &StrokeInfo);

		ptTemp.x = ptMin.x;
		ptTemp.y = ptMax.y;

		// neal - scratch box bugfix
		if (iEditMode == EDIT_MODE_SCRATCH)
		{
			int i = (StrokeInfo.m_bLeftButton) ? 1 : 2;
			ptTemp.y -= i;
			ptMin.y  += i;

			// MinY exceeds bottom? (happens with very small rects)
			if (ptMin.y > ptTemp.y)
				bDraw = FALSE;

			// left edge
			if (bDraw)
			{
				StrokeInfo.m_ptImage  = ptTemp;
				StrokeInfo.m_ptImage2 = ptMin;
				DrawLine( &StrokeInfo);
			}

			// bottom edge
			StrokeInfo.m_ptImage    = ptMax;
			StrokeInfo.m_ptImage2.x = ptMin.x;
			StrokeInfo.m_ptImage2.y = ptMax.y;
			DrawLine( &StrokeInfo);
		}
		else	// this improves the results for decal hollow box
		{
			// left edge
			if (bDraw)
			{
				StrokeInfo.m_ptImage  = ptMin;
				StrokeInfo.m_ptImage2 = ptTemp;
				DrawLine( &StrokeInfo);
			}

			// bottom edge
			StrokeInfo.m_ptImage.x = ptMin.x;
			StrokeInfo.m_ptImage.y = ptMax.y;
			StrokeInfo.m_ptImage2  = ptMax;
			DrawLine( &StrokeInfo);
		}
		break;

	default:
		//DrawLine( ptImage1, ptImage2, iSize, iColor1, iColor2, bLeftButton);
		ASSERT( FALSE);
		break;
	}
}

///////////////////////////////////////////////////////////////////////
// Name:		PaintLine
// Action:	Draws a line with solid a solid color
///////////////////////////////////////////////////////////////////////
void CTool::PaintLine( CToolStrokeInfo* pStroke)
{
	CWallyDoc* pDoc = pStroke->m_pDoc;
	CPoint     ptImage1( pStroke->m_ptImage);
	CPoint     ptImage2( pStroke->m_ptImage2);

	// just one dot?
	if ((ptImage1.x == ptImage2.x) && (ptImage1.y == ptImage2.y))
	{
		DrawSpot( *pStroke);
		return;
	}

	int i, numpixels;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	int w = pDoc->Width();
	int h = pDoc->Height();

	// Calculate deltax and deltay for initialisation
	int deltax = abs( ptImage2.x - ptImage1.x);
	int deltay = abs( ptImage2.y - ptImage1.y);

	// Initialize all vars based on which is the independent variable
	if (deltax >= deltay)
	{
		// x is independent variable
		numpixels = deltax + 1;
		dinc1 = deltay << 1;
		d     = dinc1 - deltax;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
	}
	else
	{
		// y is independent variable }
		numpixels = deltay + 1;
		dinc1 = deltax << 1;
		d     = dinc1 - deltay;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
	}

	// Make sure x and y move in the right directions
	if (ptImage1.x > ptImage2.x)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (ptImage1.y > ptImage2.y)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	// Start drawing at <ptImage1.x, ptImage1.y>
	x = ptImage1.x;
	y = ptImage1.y;

	// Draw the pixels
	for (i = 0; i < numpixels; i++)
	{
		pStroke->m_ptImage.x = x;
		pStroke->m_ptImage.y = y;
		pStroke->m_bHollow = (i > 0);
		DrawSpot( *pStroke);

		if (d >= 0)
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
		else
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Name:		SprayLine
// Action:	Draws a line for a spray tool
///////////////////////////////////////////////////////////////////////
void CTool::SprayLine( CToolStrokeInfo* pStroke)
{
	CWallyView* pView = pStroke->m_pView;

	if (pView->m_bLeftButtonTracking || pView->m_bRightButtonTracking || 
					(GetDrawingMode( pStroke->m_iEditMode) != MODE_FREEHAND))
		{
			CWallyDoc* pDoc = pStroke->m_pDoc;
			CPoint     ptImage1( pStroke->m_ptImage);
			CPoint     ptImage2( pStroke->m_ptImage2);

			// just one pixel?
			if ((ptImage1.x == ptImage2.x) && (ptImage1.y == ptImage2.y))
			{
				//SpraySpot( pDoc, pView, ptImage1.x, ptImage1.y, iSize, 0, iColor, SPRAY_COVERAGE);
				SpraySpot( *pStroke);
				return;
			}

			// tweak for best appearance
			int i = (pStroke->m_iSize < 15) ? 2 : 3;
			i = pStroke->m_iSize / i;
			i = max( 1, i);

			int iNumSpots = (int )(sqrt( (double )sq( ptImage1.x - ptImage2.x) + 
					(double )sq( ptImage1.y - ptImage2.y)) / i) + 1;

			iNumSpots *= 2;  // 50% overlap

			int iMax = iNumSpots - 1;
			iMax = max( 1, iMax);

			// neal - skip last spot for better-looking connected lines
			for (i = 0; i < iMax; i++)
			{
				// use linear interpolation

				pStroke->m_ptImage.x = (ptImage1.x * (iMax-i) + (ptImage2.x * i)) / iMax;
				pStroke->m_ptImage.y = (ptImage1.y * (iMax-i) + (ptImage2.y * i)) / iMax;

				SpraySpot( *pStroke);
			}
		}
}

void CTool::ReplaceLine( CToolStrokeInfo* pStroke)
{
	CWallyDoc* pDoc = pStroke->m_pDoc;
	CPoint     ptImage1( pStroke->m_ptImage);
	CPoint     ptImage2( pStroke->m_ptImage2);

	// just one pixel?
	if ((ptImage1.x == ptImage2.x) && (ptImage1.y == ptImage2.y))
	{
//		ReplaceSpot( pDoc, pView, ptImage1.x, ptImage1.y, iSize, 0, iColor, iReplaceColor, bLeftButton, FALSE);
		ReplaceSpot( *pStroke);
		return;
	}

	int i, numpixels;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	int w = pDoc->GetWidth();
	int h = pDoc->GetHeight();

	// Calculate deltax and deltay for initialisation
	int deltax = abs( ptImage2.x - ptImage1.x);
	int deltay = abs( ptImage2.y - ptImage1.y);

	// Initialize all vars based on which is the independent variable
	if (deltax >= deltay)
	{
		// x is independent variable
		numpixels = deltax + 1;
		dinc1 = deltay << 1;
		d     = dinc1 - deltax;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
	}
	else
	{
		// y is independent variable }
		numpixels = deltay + 1;
		dinc1 = deltax << 1;
		d     = dinc1 - deltay;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
	}

	// Make sure x and y move in the right directions
	if (ptImage1.x > ptImage2.x)
	{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (ptImage1.y > ptImage2.y)
	{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	// Start drawing at <ptImage1.x, ptImage1.y>
	x = ptImage1.x;
	y = ptImage1.y;

	// Draw the pixels
	for (i = 0; i < numpixels; i++)
	{
//		ReplaceSpot( pDoc, pView, x, y, iSize, 0, iColor, iReplaceColor, bLeftButton, (i > 0));
		pStroke->m_ptImage.x = x;
		pStroke->m_ptImage.y = y;
		pStroke->m_bHollow = (i > 0);
		ReplaceSpot( *pStroke);

		if (d >= 0)
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
		else
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
	}
}

void CTool::PatternedLine( CToolStrokeInfo* pStroke)
{
	int i, numpixels;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	CWallyDoc* pDoc = pStroke->m_pDoc;
	CPoint     ptImage1( pStroke->m_ptImage);
	CPoint     ptImage2( pStroke->m_ptImage2);

	int r = (GetRValue( pStroke->m_irgbColor) + GetRValue( pStroke->m_irgbColor2)) / 2;
	int g = (GetGValue( pStroke->m_irgbColor) + GetGValue( pStroke->m_irgbColor2)) / 2;
	int b = (GetBValue( pStroke->m_irgbColor) + GetBValue( pStroke->m_irgbColor2)) / 2;

	g_irgbMixColor = IRGB( pDoc->FindNearestColor( r, g, b, FALSE), r, g, b);

	// just one pixel?
	if ((ptImage1.x == ptImage2.x) && (ptImage1.y == ptImage2.y))
	{
		PatternedSpot( *pStroke);
		return;
	}

	int w = pDoc->GetWidth();
	int h = pDoc->GetHeight();

	// Calculate deltax and deltay for initialisation
	int deltax = abs( ptImage2.x - ptImage1.x);
	int deltay = abs( ptImage2.y - ptImage1.y);

	// Initialize all vars based on which is the independent variable
	if (deltax >= deltay)
		{
		// x is independent variable
		numpixels = deltax + 1;
		dinc1 = deltay << 1;
		d     = dinc1 - deltax;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
		}
	else
		{
		// y is independent variable }
		numpixels = deltay + 1;
		dinc1 = deltax << 1;
		d     = dinc1 - deltay;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
		}

	// Make sure x and y move in the right directions
	if (ptImage1.x > ptImage2.x)
		{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
		}
	if (ptImage1.y > ptImage2.y)
		{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
		}

	// Start drawing at <ptImage1.x, ptImage1.y>
	x = ptImage1.x;
	y = ptImage1.y;

	// Draw the pixels
	for (i = 0; i < numpixels; i++)
	{
		pStroke->m_ptImage.x = x;
		pStroke->m_ptImage.y = y;
		pStroke->m_bHollow = (i > 0);
		PatternedSpot( *pStroke);

		if (d >= 0)
		{
			d += dinc2;
			x += xinc2;
			y += yinc2;
		}
		else
		{
			d += dinc1;
			x += xinc1;
			y += yinc1;
		}
	}
}

void CTool::DecalLine( CToolStrokeInfo* pStroke)
{
	int i, numpixels;
	int d, dinc1, dinc2;
	int x, xinc1, xinc2;
	int y, yinc1, yinc2;

	CWallyDoc* pDoc = pStroke->m_pDoc;
	CPoint     ptImage1( pStroke->m_ptImage);
	CPoint     ptImage2( pStroke->m_ptImage2);

	// just one pixel?
	if ((ptImage1.x == ptImage2.x) && (ptImage1.y == ptImage2.y))
	{
		DecalSpot( *pStroke);
		return;
	}

	int w = pDoc->GetWidth();
	int h = pDoc->GetHeight();

	// Calculate deltax and deltay for initialisation
	int deltax = abs( ptImage2.x - ptImage1.x);
	int deltay = abs( ptImage2.y - ptImage1.y);

	// Initialize all vars based on which is the independent variable
	if (deltax >= deltay)
		{
		// x is independent variable
		numpixels = deltax + 1;
		dinc1 = deltay << 1;
		d     = dinc1 - deltax;
		dinc2 = (deltay - deltax) << 1;
		xinc1 = xinc2 = yinc2 = 1;
		yinc1 = 0;
		}
	else
		{
		// y is independent variable }
		numpixels = deltay + 1;
		dinc1 = deltax << 1;
		d     = dinc1 - deltay;
		dinc2 = (deltax - deltay) << 1;
		xinc1 = 0;
		xinc2 = yinc1 = yinc2 = 1;
		}

	// Make sure x and y move in the right directions
	if (ptImage1.x > ptImage2.x)
		{
		xinc1 = -xinc1;
		xinc2 = -xinc2;
		}
	if (ptImage1.y > ptImage2.y)
		{
		yinc1 = -yinc1;
		yinc2 = -yinc2;
		}

	// Start drawing at <ptImage1.x, ptImage1.y>
	x = ptImage1.x;
	y = ptImage1.y;

	int iSpacing = (numpixels + 4-1) / 4;
	iSpacing     = max( 3, iSpacing);
	int j        = iSpacing;

	// Draw the pixels
	for (i = 0; i < numpixels; i++)
	{
		if (m_bFinalStroke)
		{
			pStroke->m_ptImage.x = x;
			pStroke->m_ptImage.y = y;
			DecalSpot( *pStroke);

			if (i & 1)		// update screen every-other time
			{
//				pView->InvalidateRect( NULL, FALSE);
//				pView->UpdateWindow();
				pDoc->UpdateAllViews( NULL, HINT_UPDATE_TOOL_DIBS);
				pStroke->m_pView->UpdateWindow();
			}
		}
		else if ((j == iSpacing) || (i == numpixels-1))
		{
			pStroke->m_ptImage.x = x;
			pStroke->m_ptImage.y = y;
			DecalSpot( *pStroke);
			j = 0;
		}
		j++;

		if (d >= 0)
			{
			d += dinc2;
			x += xinc2;
			y += yinc2;
			}
		else
			{
			d += dinc1;
			x += xinc1;
			y += yinc1;
			}
	}
}

// neal - TODO: improve floodfill on large textures (use queue)
#define MAX_FLOOD_FILL_DEPTH 8000

static BOOL bFloodFailed = FALSE;

#ifdef _DEBUG
	static int iMaxReached = 0;
#endif

// neal - these don't change during the life of a flood fill
// get them off the stack (32 bytes per pixel!)
CWallyView* pFloodView = NULL;
CWallyDoc*  pFloodDoc  = NULL;
BOOL        bFloodHasSelection = FALSE;
CRect       rFloodSel(0,0,0,0);
CPoint      ptFlood(0,0);
COLOR_IRGB  irgbFloodColor     = -1;
COLOR_IRGB  irgbFloodInterior  = -1;
int         iFloodInterior     = -1;
int         iFloodWidth        = 0;
int         iFloodHeight       = 0;
int         iFloodCount        = 0;
BOOL        g_bFloodInProgress = FALSE;
BOOL        bFloodTile         = FALSE;

void CTool::InternalFloodFill( int iX, int iY, int iDepth)
{
	if (iDepth == 0)
	{
		bFloodFailed = FALSE;

		if (pFloodDoc == NULL)
		{
			ASSERT( FALSE);
			return;
		}

		iFloodCount        = 0;
		iFloodWidth        = pFloodDoc->Width();
		iFloodHeight       = pFloodDoc->Height();
		bFloodHasSelection = pFloodDoc->HasSelection();
		rFloodSel          = pFloodDoc->GetSelection()->GetBoundsRect();
	}
	iDepth++;

#ifdef _DEBUG
	iMaxReached = max( iDepth, iMaxReached);
#endif

//InternalFloodFill( iX, iY, iDepth);	// neal - just for testing the exception handling

	if (iDepth > MAX_FLOOD_FILL_DEPTH)		// don't go so deep we blow up
	{
		bFloodFailed = TRUE;
		return;
	}

	if (bFloodHasSelection && ((iX < rFloodSel.left) || (iY < rFloodSel.top) ||
			(iX >= rFloodSel.right) || (iY >= rFloodSel.bottom)))
	{
		return;
	}

	int x = iX;
	int y = iY;

	if (iFloodInterior == GetIValue( pFloodDoc->GetPixel( x, y)))
	{
		// TODO: neal: still needs more optimization (use list of active points)

		// scaning to the left edge
		int iLastX = (bFloodTile) ? ((iX+1) % iFloodWidth) : 0;

		// nw - MAJOR bug with flood fill "h" should be "w"
		while ((x != iLastX) && (iFloodInterior == 
				GetIValue( pFloodDoc->GetWrappedPixel( (x-1), y))))
		{
			if (bFloodHasSelection && ((x <= rFloodSel.left) || (x > rFloodSel.right)))
				break;

			x--;

			// fixes negative number modulus problem
			if (bFloodTile)
				x = (x + iFloodWidth) % iFloodWidth;
		}

		// scaning to the right edge
		iLastX = (bFloodTile) ? (((x-1) + iFloodWidth) % iFloodWidth) : iFloodWidth;

		// scan right - row below
		iX = x;

		BOOL bEdge = TRUE;

		// pixel row below
		y = iY + 1;

		if (bFloodTile)
			y = y % iFloodHeight;

		while ((x != iLastX) && 
			(iFloodInterior == GetIValue( pFloodDoc->GetPixel( x, iY))))
		{
			if (bFloodHasSelection && ((x < rFloodSel.left) || (x >= rFloodSel.right)))
				break;

			// mark it during first pass

			pFloodDoc->SetPixel( pFloodView, x, iY, irgbFloodColor);
			//iFloodCount++;

			if ((y >= 0) && (y < iFloodHeight))
			{
				if (iFloodInterior == GetIValue( pFloodDoc->GetPixel( x, y)))
				{
					if (bEdge)
					{
						InternalFloodFill( x, y, iDepth);
						bEdge = FALSE;
					}
				}
				else
				{
					bEdge = TRUE;
				}
			}
			x++;

			if (bFloodTile)
				x %= iFloodWidth;
		}
		iLastX = x;

		// scan right - row above
		x     = iX;
		bEdge = TRUE;

		// pixel row above
		y = iY - 1;

		// fixes negative number modulus problem
		if (bFloodTile)
			y = (y + iFloodHeight) % iFloodHeight;

		while (x != iLastX)
		{
			if (bFloodHasSelection && (x >= rFloodSel.right))
				break;

			// neal - no need to do it during second pass
			//pFloodDoc->SetMipPixel ((y * iFloodWidth) + x, iFloodColor, 0);

			if ((y >= 0) && (y < iFloodHeight))
			{
				if (iFloodInterior == GetIValue( pFloodDoc->GetPixel( x, y)))
				{
					if (bEdge)
					{
						InternalFloodFill( x, y, iDepth);
						bEdge = FALSE;
					}
				}
				else
				{
					bEdge = TRUE;
				}
			}
			x++;

			if (bFloodTile)
				x %= iFloodWidth;
		}
	}
}

// this function creates another thread to do the actual flood fill for three reasons:
// 1. a separate thread get a whole 1 MB stack for itself
// 2. an individual thread can be notified of a stack overflow only ONCE
// 3. the stack's storage is freed when the thread exits
DWORD __stdcall ThreadFloodFill( LPVOID pvoidTool)
{
	CTool* pTool = (CTool* )pvoidTool;

#ifdef _DEBUG
	iMaxReached = 0;
#endif

	BOOL bSuccess = FALSE;

	__try
	{
		g_Tool.InternalFloodFill( ptFlood.x, ptFlood.y, 0);

		bSuccess = ! bFloodFailed;	// bFloodFailed is set by internal flood fill
	}
    __except (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		// oops... stack overflowed due to too complex flood region (data dependant)
		bSuccess = FALSE;

		// TODO: neal - why can't we flood fill again?

		//MessageBeep( MB_ICONEXCLAMATION);
		//MessageBox( "WARNING!  Wally is now in an unstable state.\n"
		//		"Save your work and exit NOW!");
	}

	return (bSuccess);
}

void CTool::FloodFill( CWallyView* pView, CPoint ptImage, 
			COLOR_IRGB irgbInteriorColor, COLOR_IRGB irgbColor)
{
	if (g_bFloodInProgress)
	{
		ASSERT( FALSE);
		return;
	}

	if (irgbInteriorColor == irgbColor)
		return;		// nothing to do

	g_bFloodInProgress = TRUE;

	pView->BeginWaitCursor();

	CWallyDoc* pDoc = pView->GetDocument();
	pDoc->m_Undo.SaveUndoState( pDoc, GetToolName( EDIT_MODE_FLOOD_FILL));

//	if (GetDocument()->GetSelection())
//	{
//		ASSERT( FALSE);
//		AfxMessageBox( "FloodFill is not yet implemented for selections");
//	}
	
	// create a worker thread to do the actual flood fill (for reasons above)

	DWORD dwThreadID = NULL;

	// neal - can only pass one ptr to ThreadFloodFill make it "this"
	ptFlood    = ptImage;
	pFloodDoc  = pDoc;
	bFloodTile = pView->m_bTile;

	// Neal - added color tolerance to flood fill

//	CWallyPalette* pPal       = pDoc->GetPalette();
//	int            iInteriorR = pPal->GetR( iFloodInterior);
//	int            iInteriorG = pPal->GetG( iFloodInterior);
//	int            iInteriorB = pPal->GetB( iFloodInterior);
	int iInteriorR = GetRValue( irgbInteriorColor);
	int iInteriorG = GetGValue( irgbInteriorColor);
	int iInteriorB = GetBValue( irgbInteriorColor);

	int        i, j, r, g, b, indx;
	COLOR_IRGB irgb;
	CRect      rFloodSel( 0, 0, pDoc->Width(), pDoc->Height());

	if (pDoc->HasSelection())
		rFloodSel = pDoc->GetSelection()->GetBoundsRect();

	// flag interior with special color index (we'll restore it later)
	iFloodInterior    = 255;
	irgbFloodInterior = IRGB( iFloodInterior, iInteriorR, iInteriorG, iInteriorB);
	irgbFloodColor    = irgbColor;

	if (GetIValue( irgbColor) == iFloodInterior)	// force a non-match
		irgbFloodColor &= 0x00FFFFFF;

	for (j = rFloodSel.top; j < rFloodSel.bottom; j++)
	{
		for (i = rFloodSel.left; i < rFloodSel.right; i++)
		{
			irgb = pDoc->GetPixel( i, j);
			indx = GetIValue( irgb);
			r    = GetRValue( irgb);
			g    = GetGValue( irgb);
			b    = GetBValue( irgb);

			if ((iInteriorR >= r - g_iColorTolerance) && (iInteriorR <= r + g_iColorTolerance) &&
				(iInteriorG >= g - g_iColorTolerance) && (iInteriorG <= g + g_iColorTolerance) &&
				(iInteriorB >= b - g_iColorTolerance) && (iInteriorB <= b + g_iColorTolerance))
			{
				//pDoc->SetIndexColorPixel( pView, i, j, iFloodInterior);
				pDoc->SetPixel( pView, i, j, IRGB( iFloodInterior, r, g, b));
			}
			else if (indx == iFloodInterior)
			{
				// Neal - this takes care of a poorly matching palette
				pDoc->SetPixel( pView, i, j, IRGB( 0, r, g, b));
			}
		}
	}

	HANDLE hThread = CreateThread( NULL, 0, ThreadFloodFill,
		(LPVOID )this, 0, &dwThreadID);

	// we need to wait for thread to terminate
	//if (hThread != (HANDLE )-1)
	//	WaitForSingleObject( hThread, INFINITE);

	// improved dynamic redraw version

	if (hThread != (HANDLE )-1)
	{
		while (TRUE)
		{
			MSG Msg;
//			while (PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE))
			if (PeekMessage( &Msg, NULL, WM_PAINT, WM_ERASEBKGND, PM_REMOVE))
			{
				TranslateMessage( &Msg);
				DispatchMessage( &Msg);
			}

			if (WaitForSingleObject( hThread, 100) != WAIT_TIMEOUT)
				break;
		}
	}

	pFloodDoc = NULL;

	// get result
	BOOL bSuccess = FALSE;
	GetExitCodeThread( hThread, (DWORD *)&bSuccess);

	if (! bSuccess)
	{
		pView->MessageBox( "Flood Fill terminated early because it ran out of stack space.\n"
					"Unfortunately, Windows only allows a maximum of 1 MB per thread.\n\n"
					"Please refill any areas that were missed during the recovery phase.\n\n"
					"Sorry.",
					"Incredibly Verbose Error Message  :-)");

		// MessageBox can eat button up (so clear flags)
		pView->m_bLeftButtonTracking  = FALSE;
		pView->m_bRightButtonTracking = FALSE;
	}

	// let the system destroy the thread object
	CloseHandle( hThread);

	// recalc to restore color index

	for (j = rFloodSel.top; j < rFloodSel.bottom; j++)
	{
		for (i = rFloodSel.left; i < rFloodSel.right; i++)
		{
			irgb = pDoc->GetPixel( i, j);

			// we only need to reset the ones we flagged

			indx = GetIValue( irgb);
			if ((indx == iFloodInterior)  || (indx == 0))
				pDoc->SetNearestColorPixel( pView, i, j, irgb);
		}
	}

	g_bFloodInProgress = FALSE;
	pView->EndWaitCursor();
}

LPCSTR CTool::GetToolName( int iEditMode)
{
	LPCSTR lpszToolName = "<Unknown Tool ID>";

	switch (iEditMode)
	{
	case EDIT_MODE_SELECTION:
		lpszToolName = "Selection Tool";
		break;

	case EDIT_MODE_ZOOM:
		lpszToolName = "Zoom Tool";
		break;

	case EDIT_MODE_EYEDROPPER:
		lpszToolName = "Eyedropper";
		break;

	case EDIT_MODE_PENCIL:
		lpszToolName = "Pencil";
		break;

	case EDIT_MODE_BRUSH:
		lpszToolName = "Paintbrush";
		break;

	case EDIT_MODE_ERASER:
		lpszToolName = "Eraser";
		break;

	case EDIT_MODE_RUBBER_STAMP:
		lpszToolName = "Rubber Stamp";
		break;
	case EDIT_MODE_CLONE:
		lpszToolName = "Clone";
		break;
	case EDIT_MODE_REPLACE:
		lpszToolName = "Replacer";
		break;
	case EDIT_MODE_RECOLOR:
		lpszToolName = "Recolor";
		break;
	case EDIT_MODE_PATTERNED_PAINT:
		lpszToolName = "Patterned Paint";
		break;

	case EDIT_MODE_DARKEN:
		lpszToolName = "Darken";
		break;
	case EDIT_MODE_LIGHTEN:
		lpszToolName = "Lighten";
		break;
	case EDIT_MODE_TINT:
		lpszToolName = "Tint";
		break;

	case EDIT_MODE_DECAL:
		lpszToolName = "Decal";
		break;
	case EDIT_MODE_DENT:
		lpszToolName = "Draw Dent";
		break;
	case EDIT_MODE_BUMP:
		lpszToolName = "Draw Bump";
		break;

	case EDIT_MODE_DRIP:
		lpszToolName = "Drip";
		break;
	case EDIT_MODE_BLEND:
		lpszToolName = "Blend";
		break;
	case EDIT_MODE_SHARPEN:
		lpszToolName = "Sharpen";
		break;
	case EDIT_MODE_SPRAY:
		lpszToolName = "Spray";
		break;
	case EDIT_MODE_SPRAY_RECOLOR:
		lpszToolName = "Spray Recolor";
		break;
	case EDIT_MODE_BULLET_HOLES:
		lpszToolName = "Draw Bullet Holes";
		break;
	case EDIT_MODE_RIVETS:
		lpszToolName = "Draw Rivets";
		break;

	case EDIT_MODE_SCRATCH:
		lpszToolName = "Draw Scratch";
		break;
	case EDIT_MODE_FLOOD_FILL:
		lpszToolName = "Flood Fill";
		break;

	default:
		ASSERT( FALSE);		// missed a new tool?
	}
	return lpszToolName;
}

double CalcLineAngle( CPoint ptCenter, CPoint ptRadial, BOOL bConstrain)
{
	double dfAngle;

	if (ptCenter.x != ptRadial.x)
	{
		// neal - yes, this ~is~ correct (0,0 is upper left)
		dfAngle = atan( (ptRadial.y - ptCenter.y) /
					 (double )(ptCenter.x - ptRadial.x));

		dfAngle = dfAngle * 180.0 / _PI_;

		if (ptRadial.x < ptCenter.x)
			dfAngle = 180.0 + dfAngle;
		else if (dfAngle < 0.0)
			dfAngle += 360.0;
	}
	else if (ptRadial.y < ptCenter.y)
		dfAngle = 90.0;
	else if (ptCenter != ptRadial)
		dfAngle = 270.0;
	else
		dfAngle = 0.0;		// just one pixel long

	// truncate to nearest 15 degree mark
	if (bConstrain)
	{
		dfAngle = (dfAngle + 7.5) / 15.0;
		modf( dfAngle, &dfAngle);		// grab integer part
		dfAngle *= 15.0;
	}

	return dfAngle;
}

// Place the current image coordinates onto the status bar
void CTool::ShowToolInfo( CWallyView* pView, CPoint ptPos)
{
	CPoint ptTrue( ptPos);
	ldiv_t xDiv, yDiv;
	int TileValue = (pView->m_bTile) ? 3 : 1;

	CWallyDoc* pDoc = pView->GetDocument();
	int w = pDoc->Width();
	int h = pDoc->Height();

	char sbText[80];
	CString strInfo;

	xDiv = ldiv( ptTrue.x, w);
	yDiv = ldiv( ptTrue.y, h);
	ptTrue.x = xDiv.rem;
	ptTrue.y = yDiv.rem;
	sprintf_s( sbText, sizeof(sbText), "Pos:%3d,%3d", ptTrue.x, ptTrue.y);
	strInfo = sbText;

	// if a line or shape is in progress
	if ((pView->m_bLeftButtonTracking || (pView->m_bRightButtonTracking)) &&
			ToolHasConstrain( g_iCurrentTool))
	{
		// add angle
		int iDrawingMode = GetDrawingMode( g_iCurrentTool);

		if ((iDrawingMode == MODE_LINE)       ||
				(iDrawingMode == MODE_POLYLINE) ||
				(iDrawingMode == MODE_RAY))
		{
			sprintf_s( sbText, sizeof(sbText), "    Angle:%6.2f°",
						CalcLineAngle( ptLastPos, ptPos, g_bConstrained));
			strInfo += sbText;
		}
		else if (IsRectMode( GetDrawingMode( g_iCurrentTool)))
		{
			int iWidth  = abs( ptPos.x - ptLastPos.x) + 1;
			int iHeight = abs( ptPos.y - ptLastPos.y) + 1;

			sprintf_s( sbText, sizeof(sbText), "    Size:%3d x %3d", iWidth, iHeight);
			strInfo += sbText;
		}
	}
	else if ((g_iCurrentTool == EDIT_MODE_EYEDROPPER) || (IsKeyPressed( VK_CONTROL)))
	{
		// within bounds?

		BOOL bInBounds = FALSE;

		if ((ptTrue.x >= 0) && (ptTrue.y >= 0))
		{
			if (pView->m_bTile)
			{
				bInBounds = ((ptTrue.x < w * 3) && (ptTrue.y < h * 3));
			}
			else
			{				
				bInBounds = ((ptTrue.x < w) && (ptTrue.y < h));
			}
		}

		if (bInBounds)
		{
			COLOR_IRGB irgbColor = pDoc->GetWrappedPixel( ptPos.x, ptTrue.y);

			sprintf_s( sbText, sizeof(sbText), "    RGB:%3d,%3d,%3d",
					GetRValue( irgbColor), GetGValue( irgbColor), GetBValue( irgbColor));

//			// add to doc title
//			m_wndStatusBar.SetPaneText( 0, pDoc->GetTitle() + sbText, true);

			strInfo += sbText;

			if (pDoc->GetNumBits() <= 8)	// has a palette?
			{
				sprintf_s( sbText, sizeof(sbText), "    Index:%3d", GetIValue( irgbColor));
				strInfo += sbText;
			}
		}
	}
	else
	{
		if ((ptPos.x >= w * TileValue) || (ptPos.y >= h * TileValue) ||
				(ptPos.x < 0) || (ptPos.y < 0))
			strInfo = "";
	}

	// current cursor position
//	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), strInfo, true);
	m_wndStatusBar.SetPaneText(0, strInfo, true);
}

void CTool::UpdateConstrainPaneText()
{
	CString strText;
	int     iMode = GetDrawingMode( g_iCurrentTool);

	if (ToolHasConstrain( g_iCurrentTool))
	{
		if (IsRectMode( iMode))
			strText = "Square";
		else if (IsLineMode( iMode))
			strText = "15° Increments";
		else if (g_bConstrainedHorizonal)
			strText = "Horizontal";
		else
			strText = "Vertical";

		if (g_bConstrained)
			strText = "[on] " + strText;
	}

	m_wndStatusBar.SetPaneText( m_wndStatusBar.CommandToIndex(ID_CONSTRAIN_STATUSBAR), strText, true);
}

///////////////////////////////////////////////////////////////////////
// Name:	UseTool
// Action:	Applies the current tool at ptPos
///////////////////////////////////////////////////////////////////////
void CTool::UseTool( CWallyView* pView, UINT nFlags, CPoint ptPos, int iMouseCode)
{
	int        i, j;
	BOOL       bNeedToRebuild = FALSE;
	int        iMult          = (pView->m_bTile) ? 3 : 1;
	int        iDrawingMode   = GetDrawingMode( g_iCurrentTool);
	CWallyDoc* pDoc           = pView->GetDocument();

	CPoint pt, ptClientPos( ptPos);

	pView->GetTrueImageCoordinates( &ptPos);

	if (g_bConstrained && (iMouseCode != MOUSE_BUTTON_DOWN) &&
			(pView->m_bLeftButtonTracking || pView->m_bRightButtonTracking))
	{
		if (IsLineMode( iDrawingMode))
		{
			// constrain line to 15° increments
			//
			// NOTE: there is a bit of variance, due to
			//       integer round-off to nearest pixel
			//       (it's not really a bug)

			switch (iDrawingMode)
				{
				case MODE_LINE:
					pt = m_ptLastMouseDown;
					break;

				case MODE_POLYLINE:
					if (m_bLastMouseUpIsValid)
						pt = m_ptLastMouseUp;
					else
						pt = m_ptLastMouseDown;
					break;

				case MODE_RAY:
					pt = m_ptLastMouseDown;
					break;

				default:
					ASSERT( FALSE);
					break;
				}

			pView->GetTrueImageCoordinates( &pt);

			i = ptPos.x - pt.x;
			j = ptPos.y - pt.y;

			double dfLength = sqrt( (double )(i*i + j*j));
			double dfAngle  = CalcLineAngle( pt, ptPos, g_bConstrained);

			double dfCos = cos( dfAngle * _PI_ / 180.0);
			double dfSin = sin( dfAngle * _PI_ / 180.0);

			#define ERROR_TERM  0.001

			double dfErr = (dfCos >= 0) ? ERROR_TERM : -ERROR_TERM;
			ptPos.x = pt.x + (int )(dfCos * dfLength + dfErr);

			dfErr = (dfSin >= 0) ? ERROR_TERM : -ERROR_TERM;
			ptPos.y = pt.y - (int )(dfSin * dfLength + dfErr);
		}
		else if (IsRectMode( iDrawingMode))
		{
			// constrain to square

			pt = m_ptLastMouseDown;
			pView->GetTrueImageCoordinates( &pt);

			i = abs( ptPos.x - pt.x);
			j = abs( ptPos.y - pt.y);

			int iMin = min( i, j);

			if (ptPos.x >= pt.x)
				ptPos.x = pt.x + iMin;
			else
				ptPos.x = pt.x - iMin;

			if (ptPos.y >= pt.y)
				ptPos.y = pt.y + iMin;
			else
				ptPos.y = pt.y - iMin;
		}
	}

	int iWidth  = pDoc->Width();
	int iHeight = pDoc->Height();

	pView->m_CurrentPosition = ptPos;

	// neal - if ctrl is down, we're an eyedropper
	// (unless the tool is a effect, in which case "apply more" instead
	int iEditMode = g_iCurrentTool;

	if ((nFlags & MK_CONTROL) != 0)
	{		
		iEditMode = EDIT_MODE_EYEDROPPER;
	}

	BOOL bValidFreehandMode = IsFreehandTool( iEditMode);

	if ((iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
	{
		if (pView->m_bRightButtonTracking)
			bValidFreehandMode = FALSE;
	}

	////////////////////////////////////////////////////
	// check the size/state of the SpotMarker buffer //
	//////////////////////////////////////////////////

	if (iMouseCode == MOUSE_BUTTON_DOWN)
	{
		int iSize = iWidth * iHeight;
		iSize = max( iSize, 64*64);

		if ((m_pbySpotMarker == NULL) || 
				(iSize > m_iSpotMarkerSize) ||
				(iSize < 4*m_iSpotMarkerSize))
		{
			DeleteSpotMarker();

			m_pbySpotMarker   = new BYTE[iSize];
			m_iSpotMarkerSize = iSize;
		}

		if (m_pbySpotMarker && ToolUsesSpotMarker( iEditMode))
			ZeroMemory( m_pbySpotMarker, iWidth * iHeight);
	}
	else if (bValidFreehandMode && (iDrawingMode != MODE_FREEHAND))
	{
		if (m_pbySpotMarker && ToolUsesSpotMarker( iEditMode))
			ZeroMemory( m_pbySpotMarker, iWidth * iHeight);
	}

	if (bValidFreehandMode)
	{
		if ((iMouseCode == MOUSE_MOVE) || 
					(iMouseCode == TIMER_MSG) ||
					(iMouseCode == MOUSE_BUTTON_UP))
		{
			if (pView->m_bLeftButtonTracking || pView->m_bRightButtonTracking)
			{
				if (iDrawingMode != MODE_FREEHAND)
				{
					CUndoState* pState = pDoc->m_DrawingModeUndo.GetLastState();
					if (pState)
					{
						pDoc->m_bFreeEffectsData = false;
						pDoc->m_DrawingModeUndo.RestoreState( pState);
						pDoc->m_bFreeEffectsData = true;

						pDoc->UpdateAllViews( pView, HINT_UPDATE_TOOL_DIBS);
					}

					BOOL bValid = FALSE;

					switch (iDrawingMode)
					{
					case MODE_LINE:
					case MODE_HOLLOW_RECT:
					case MODE_SOLID_RECT:
					case MODE_EDGED_RECT:
						bValid    = m_bLastMouseDownIsValid;
						ptLastPos = m_ptLastMouseDown;
						break;

					case MODE_POLYLINE:
						if (m_bLastMouseUpIsValid)
						{
							bValid    = m_bLastMouseUpIsValid;
							ptLastPos = m_ptLastMouseUp;
						}
						else
						{
							bValid    = m_bLastMouseDownIsValid;
							ptLastPos = m_ptLastMouseDown;
						}
						break;

					case MODE_RAY:
						bValid    = m_bLastMouseDownIsValid;
						ptLastPos = m_ptLastMouseDown;
						break;

					default:
						ASSERT( FALSE);
						break;
					}

					if (bValid)
					{
						pView->GetTrueImageCoordinates( &ptLastPos);
					}
					else
					{
						ptLastPos = ptTrail = ptPos;
					}
				}

				if (iMouseCode == MOUSE_BUTTON_UP)
				{
					m_ptLastMouseUp       = ptClientPos;    // in client coords!
					m_bLastMouseUpIsValid = TRUE;
				}
			}
		}
		else if (iMouseCode == MOUSE_BUTTON_DOWN)
		{
			if (iDrawingMode == MODE_FREEHAND)
			{
				ptLastPos = ptTrail = ptPos;
			}
			else
			{
				pDoc->m_DrawingModeUndo.SaveUndoState( pDoc, "");

				BOOL bValid = FALSE;

				switch (iDrawingMode)
				{
				case MODE_LINE:
				case MODE_HOLLOW_RECT:
				case MODE_SOLID_RECT:
				case MODE_EDGED_RECT:
					break;

				case MODE_POLYLINE:
					bValid    = m_bLastMouseUpIsValid;
					ptLastPos = m_ptLastMouseUp;
					break;

				case MODE_RAY:
					bValid    = m_bLastMouseDownIsValid;
					ptLastPos = m_ptLastMouseDown;
					break;

				default:
					ASSERT( FALSE);
					break;
				}

				if (bValid)
				{
					pView->GetTrueImageCoordinates( &ptLastPos);
				}
				else
				{
					ptLastPos = ptTrail = ptPos;
				}
			}

			if ((iDrawingMode != MODE_RAY) || (! m_bLastMouseDownIsValid))
			{
				m_ptLastMouseDown       = ptClientPos;  // in client coords!
				m_bLastMouseDownIsValid = TRUE;
			}
		}
	}
	else if (iEditMode == EDIT_MODE_SELECTION)
	{
		if ((iMouseCode == MOUSE_MOVE) || (iMouseCode == MOUSE_BUTTON_UP))
		{
			if (pView->m_bLeftButtonTracking || pView->m_bRightButtonTracking)
			{
				BOOL bValid = FALSE;

				bValid    = m_bLastMouseDownIsValid;
				ptLastPos = m_ptLastMouseDown;

				if (bValid)
				{
					pView->GetTrueImageCoordinates( &ptLastPos);
				}
				else
				{
					ptLastPos = ptTrail = ptPos;
				}

				if (iMouseCode == MOUSE_BUTTON_UP)
				{
					m_ptLastMouseUp       = ptClientPos;    // in client coords!
					m_bLastMouseUpIsValid = TRUE;
				}
			}
		}
		else if (iMouseCode == MOUSE_BUTTON_DOWN)
		{
			ptLastPos = ptTrail = ptPos;

			m_ptLastMouseDown       = ptClientPos;  // in client coords!
			m_bLastMouseDownIsValid = TRUE;
		}
	}

	// Place the current image coordinates onto the status bar
	ShowToolInfo( pView, ptPos);

	COLOR_IRGB irgbColor     = COLOR_DEBUG_PURPLE;
	int        iDrawingWidth = GetToolWidth();

	if (IsDrawingTool( iEditMode))
	{
		if (pView->m_bLeftButtonTracking)
		{
			irgbColor = g_irgbColorLeft;

			if (ToolHasCursor( iEditMode))
			{
				if ((iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
				{
					if (pDoc->m_bCloneInitialized)
					{
						if (! pDoc->m_bCloneReset)
						{
							int iOffsetX = pDoc->m_ptCloneOffset.x;
							iOffsetX += ptPos.x - pDoc->m_ptCloneStart.x;
							iOffsetX = (iOffsetX + 1000 * iWidth) % iWidth;

							int iOffsetY = pDoc->m_ptCloneOffset.y;
							iOffsetY += ptPos.y - pDoc->m_ptCloneStart.y;
							iOffsetY = (iOffsetY + 1000 * iHeight) % iHeight;

							CPoint ptClonePos( iOffsetX, iOffsetY);
							DrawBigCursor( pView, ptClonePos, iDrawingWidth, TRUE);

							pView->InvalidateRect( NULL, FALSE);
							pView->UpdateWindow();
						}
					}
				}
			}
		}
		else if (pView->m_bRightButtonTracking)
		{
			irgbColor = g_irgbColorRight;
		}
		else
		{
			// neal - just draw the big tool cursor and leave

			if (ToolHasCursor( iEditMode))
			{
				if ((iEditMode == EDIT_MODE_CLONE) || (iEditMode == EDIT_MODE_RUBBER_STAMP))
				{
					if (pDoc->m_bCloneInitialized)
					{
						if (pDoc->m_bCloneReset)
						{
							int iOffsetX = pDoc->m_ptCloneOffset.x;
							//iOffsetX += ptPos.x - m_ptCloneStart.x;
							iOffsetX = (iOffsetX + 1000 * iWidth) % iWidth;

							int iOffsetY = pDoc->m_ptCloneOffset.y;
							//iOffsetY += ptPos.y - m_ptCloneStart.y;
							iOffsetY = (iOffsetY + 1000 * iHeight) % iHeight;

							CPoint ptClonePos( iOffsetX, iOffsetY);
							DrawBigCursor( pView, ptClonePos, iDrawingWidth, TRUE);
						}
						else
						{
							int iOffsetX = pDoc->m_ptCloneOffset.x;
							iOffsetX += ptPos.x - pDoc->m_ptCloneStart.x;
							iOffsetX = (iOffsetX + 1000 * iWidth) % iWidth;

							int iOffsetY = pDoc->m_ptCloneOffset.y;
							iOffsetY += ptPos.y - pDoc->m_ptCloneStart.y;
							iOffsetY = (iOffsetY + 1000 * iHeight) % iHeight;

							CPoint ptClonePos( iOffsetX, iOffsetY);
							DrawBigCursor( pView, ptClonePos, iDrawingWidth, TRUE);
						}
						DrawBigCursor( pView, ptPos, iDrawingWidth, FALSE);
					}
					else
					{
						DrawBigCursor( pView, ptPos, iDrawingWidth, TRUE);
					}
				}
				else
				{
					DrawBigCursor( pView, ptPos, iDrawingWidth, FALSE);
				}

				pView->InvalidateRect( NULL, FALSE);
/*
				int iToolWidth = iDrawingWidth;
				DrawBigCursor( pView, ptPos, iToolWidth);

				int iZoom = pView->m_iZoomValue;

				if (iZoom == 0)
				{
					ASSERT( FALSE);
					pView->GetDocSize( NULL);
					iZoom = pView->m_iZoomValue;
				}
				iToolWidth = ((iToolWidth + 1) / 2) * iZoom;
				CRect rCursor( ptPos.x - iToolWidth, ptPos.y - iToolWidth,
								ptPos.x + iToolWidth, ptPos.y + iToolWidth);

				pView->InvalidateRect( rCursor, FALSE);
*/
				//DataModified = true;

				//UpdateDIBs();
				pView->UpdateWindow();
			}

			ClearBigCursor( pView);

			pView->SetTimer( TIMER_TOOL_CLEAR_CURSOR, 250, NULL);
			return;
		}

		pView->KillTimer( TIMER_TOOL_CLEAR_CURSOR );

		// neal - time to save undo state?
		if (iMouseCode == MOUSE_BUTTON_DOWN)
		{
			// flood fill saves an undo only if it needs to

			if (g_iCurrentTool != EDIT_MODE_FLOOD_FILL)
				pDoc->m_Undo.SaveUndoState( pDoc, GetToolName( iEditMode));

			if ((g_iCurrentTool == EDIT_MODE_SPRAY) ||
							(g_iCurrentTool == EDIT_MODE_SPRAY_RECOLOR))
			{
				pView->SetTimer( TIMER_TOOL_SPRAY, 100, NULL); // ten times/second
			}
		}
		else if (iMouseCode == MOUSE_BUTTON_UP)
		{
			pView->KillTimer( TIMER_TOOL_SPRAY );	// stop spraying (never hurts to always kill it)
		}
	}
	else if (iEditMode == EDIT_MODE_SELECTION)
	{
		if (iMouseCode == MOUSE_BUTTON_DOWN)
		{
			pDoc->m_Undo.SaveUndoState( pDoc, GetToolName( iEditMode));
		}
	}

	// neal - undo/redo frees it (so recreate it)

	if ((! pDoc->m_pEffectLayer) && (IsAnyEffect( g_iCurrentTool)))
	{
		//ASSERT( ! IsEffect( g_iCurrentTool));

		if (g_iCurrentTool == EDIT_MODE_LIGHTEN)
			pView->OnTpLighten();
		else if (g_iCurrentTool == EDIT_MODE_DARKEN)
			pView->OnTpDarken();
		else if (g_iCurrentTool == EDIT_MODE_SCRATCH)
			pView->OnTpScratch();
		else if (g_iCurrentTool == EDIT_MODE_TINT)
			pView->OnTpTint();
		else if (g_iCurrentTool == EDIT_MODE_RECOLOR)
			pView->OnTpRecolor();
//		else if (g_iCurrentTool == EDIT_MODE_SPRAY)
//			pView->OnTpSpray();
		else if (g_iCurrentTool == EDIT_MODE_SPRAY_RECOLOR)
			pView->OnTpSprayRecolor();
		else if (g_iCurrentTool == EDIT_MODE_RIVETS)
			pView->OnTpRivets();
		else if (g_iCurrentTool == EDIT_MODE_BULLET_HOLES)
			pView->OnTpBulletHoles();
		else if (g_iCurrentTool == EDIT_MODE_PATTERNED_PAINT)
			pView->OnTpPatternedPaint();
		else if (g_iCurrentTool == EDIT_MODE_DECAL)
			pView->OnTpDecal();
		else if (g_iCurrentTool == EDIT_MODE_BLEND)
			pView->OnTpBlend();
		else if (g_iCurrentTool == EDIT_MODE_CLONE)
			pView->OnTpClone();
		else if (g_iCurrentTool == EDIT_MODE_RUBBER_STAMP)
			pView->OnTpRubberStamp();
	}
	// Neal - this was resting the effects mip, which in turn
	// was causing the spray recolor "over-apply" bug
/*
	else if (iMouseCode == MOUSE_BUTTON_DOWN)
	{
		if (g_iCurrentTool == EDIT_MODE_RECOLOR)
			pView->OnTpRecolor();
//		else if (g_iCurrentTool == EDIT_MODE_SPRAY)
//			pView->OnTpSpray();
		else if (g_iCurrentTool == EDIT_MODE_SPRAY_RECOLOR)
			pView->OnTpSprayRecolor();
	}
*/

	CToolStrokeInfo Stroke;
	Stroke.Init( pView, iEditMode, iMouseCode, g_irgbColorLeft, g_irgbColorRight);
	Stroke.m_iSize    = iDrawingWidth;
	Stroke.m_ptImage  = ptPos;
	Stroke.m_ptImage2 = ptLastPos;

	switch (iEditMode)
	{
	case EDIT_MODE_SELECTION:
		if (pView->m_bLeftButtonTracking)
		{
			CSelection* pSel = pDoc->GetSelection();
			CRect       Rect = pSel->GetBoundsRect();

			Rect.top    = min( ptPos.y, ptLastPos.y);
			Rect.left   = min( ptPos.x, ptLastPos.x);
			Rect.right  = max( ptPos.x, ptLastPos.x) + 1;
			Rect.bottom = max( ptPos.y, ptLastPos.y) + 1;

			Rect.top    = max( 0,       Rect.top);
			Rect.left   = max( 0,       Rect.left);
			Rect.right  = min( iWidth,  Rect.right);
			Rect.bottom = min( iHeight, Rect.bottom);

			// Neal - TODO: add IRGB selection layer

			if (iMouseCode == MOUSE_BUTTON_DOWN)
			{
				pDoc->MergeSelectionIntoBase( "Selection");
				pDoc->m_pSelection = pDoc->GetBase();
			}

			BOOL bValid = ((Rect.Width() > 1) || (Rect.Height() > 1));

			if (! bValid)
			{
				if (iMouseCode == MOUSE_BUTTON_UP)
					pDoc->FreeCutAndPasteData();
				else
					bValid = TRUE;
			}
			if (bValid)
			{
				pDoc->GetSelection()->SetBoundsRect( Rect.left, Rect.top, Rect.right, Rect.bottom);

				if (iMouseCode == MOUSE_BUTTON_UP)
				{
					// Neal - TODO: then code movement, cut-copy-paste

					// Neal - actually create a new selection/layer
					pDoc->FreeCutAndPasteData();

					//CSelection* pSel = new CSelection;
					CSelection* pSel = pDoc->GetCutout();

					if (pSel)
					{
						CLayer* pLayer     = pSel->GetLayer();
						CLayer* pBaseLayer = pDoc->GetBase()->GetLayer();

						ASSERT( (pLayer != NULL) && (pBaseLayer != NULL));

						pSel->SetBoundsRect( Rect.left, Rect.top, Rect.right, Rect.bottom);

// Neal - TODO: Why does TGA have NumBits == 8 (instead of 24) ???

						pLayer->FreeMem();
						pLayer->DupLayer( pView, pBaseLayer);

						// Neal - TODO: set fade based on user setting
						//
						//pLayer->m_iFadeAmount = 0;

						pDoc->m_pSelection = pSel;
					}
				}
			}
			pView->InvalidateRect( NULL, FALSE);
		}
		break;

	case EDIT_MODE_ZOOM:
		{
			if (iMouseCode != MOUSE_BUTTON_DOWN)
				break;

			pView->SetZoomFactor( pView->m_bLeftButtonTracking);

			//int iMaxWH   = max( iWidth, iHeight);
			int iMaxZoom = MAX_ZOOM;
			int iMinZoom = MIN_ZOOM;

			//if (iMaxZoom * iMaxWH >= 2047)
			//	iMaxZoom = 2047 / iMaxWH;

			//
			// Neal - fixed tiled scrollbar limit bug
			//
			//iMaxZoom = min( iMaxZoom, 32767 / (iMult * pView->m_DIB[0].GetImageWidth()));
			//iMaxZoom = min( iMaxZoom, 32767 / (iMult * pView->m_DIB[0].GetImageHeight()));

			// Neal - I think the above code might be able to div-by-zero!
			//
			if ((pDoc->GetWidth() > 0) && (pDoc->GetHeight() > 0))
			{
				iMaxZoom = min( iMaxZoom, 32767 / (iMult * pDoc->GetWidth()));
				iMaxZoom = min( iMaxZoom, 32767 / (iMult * pDoc->GetHeight()));
			}
			else
			{
				iMaxZoom = 16;
				ASSERT( pDoc->GetWidth() > 0);
				ASSERT( pDoc->GetHeight() > 0);
			}

			if (pView->m_iZoomValue > iMaxZoom)
				pView->m_iZoomValue = iMaxZoom;		
			else if (pView->m_iZoomValue < iMinZoom)
				pView->m_iZoomValue = iMinZoom;
			else
			{		
				//pView->LockWindowUpdate();
				pView->SetRedraw( FALSE);

//				pView->SetScrollBars( FALSE, FALSE);
				pView->SetScrollBars( FALSE, TRUE);		// Neal - frame flash bugfix
				CRect rClient;
				pView->GetClientRect( &rClient);

				CPoint pt;
				pt.x = ptPos.x * pView->m_iZoomValue;
				pt.y = ptPos.y * pView->m_iZoomValue;

				// centered around the zoom point
				pt.x -= ptClientPos.x;
				pt.y -= ptClientPos.y;

				DWORD dwStyle = pView->GetStyle();

				if ((dwStyle & WS_HSCROLL) == 0)
					pt.x = 0;
				if ((dwStyle & WS_VSCROLL) == 0)
					pt.y = 0;

				if ((pt.x != 0) || (pt.y != 0))
					pView->ScrollToPosition( pt);

				// Neal - this fixes the zoom-tool-doesn't-repaint-edges bug
				//pView->UnlockWindowUpdate();
				pView->SetRedraw( TRUE);
				pView->InvalidateRect( NULL, FALSE);
				pView->UpdateWindow();
			}
/*
			// Neal - no longer needed - already done in SetZoomFactor()

			{
			char szZoom[20];

//			if (pView->m_iZoomValue >= 1)
//				sprintf (szZoom, "Zoom:%2d:1", pView->m_iZoomValue);
//			else
//				sprintf (szZoom, "Zoom:1:%d", -(pView->m_iZoomValue));

			if (pView->m_iZoomValue >= 1)
				sprintf( szZoom, "Zoom: %d%%", (int )(pView->m_dfZoom * 100.0));
			else
				sprintf( szZoom, "Zoom: %1.3f%%", pView->m_dfZoom * 100.0);

			m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), szZoom, true);
			}
*/
		}
		break;

	case EDIT_MODE_EYEDROPPER:
		{
			if (iMouseCode != MOUSE_BUTTON_DOWN)
				break;

			// out of bounds?
			if ((ptPos.x < 0) || (ptPos.y < 0))
				break;

			if ((ptPos.x >= iWidth * iMult) || (ptPos.y >= iHeight * iMult))
				break;
			
			irgbColor = pDoc->GetWrappedPixel( ptPos.x, ptPos.y);

			if (pView->m_bLeftButtonTracking)
			{
				g_irgbColorLeft = irgbColor;
			}
			else if (pView->m_bRightButtonTracking)
			{
				g_irgbColorRight = irgbColor;
			}
			else
			{
				ASSERT( FALSE);
			}
			((CMainFrame* )AfxGetMainWnd())->m_wndPaletteToolBar.InvalidateRect( NULL, FALSE);
		}
		break;

	case EDIT_MODE_REPLACE:
	case EDIT_MODE_RECOLOR:
	case EDIT_MODE_PENCIL:
	case EDIT_MODE_BRUSH:
	case EDIT_MODE_ERASER:
	case EDIT_MODE_PATTERNED_PAINT:
	case EDIT_MODE_DARKEN:
	case EDIT_MODE_LIGHTEN:
	case EDIT_MODE_TINT:
	case EDIT_MODE_SCRATCH:
	case EDIT_MODE_BLEND:
//		DrawStroke( pView, ptPos, ptLastPos, iDrawingWidth, pView->m_bLeftButtonTracking);
		DrawStroke( &Stroke);
		bNeedToRebuild = TRUE;
		break;

	case EDIT_MODE_CLONE:
	case EDIT_MODE_RUBBER_STAMP:
		if (pView->m_bLeftButtonTracking)
		{
			if (iMouseCode == MOUSE_BUTTON_DOWN)
			{
				if (! pDoc->m_bCloneInitialized)
				{
					AfxMessageBox( "You must first click the right mouse button in\nthis image to set a cloning source position.");
					break;
				}

				// rubber stamp ALWAYS resets starting point

				if (pDoc->m_bCloneReset || (iEditMode == EDIT_MODE_RUBBER_STAMP))
				{
					pDoc->m_bCloneReset  = false;

					if ((iEditMode == EDIT_MODE_RUBBER_STAMP) && 
								((iDrawingMode == MODE_POLYLINE) || (iDrawingMode == MODE_RAY)))
						pDoc->m_ptCloneStart = ptLastPos;
					else
						pDoc->m_ptCloneStart = ptPos;
				}
			}
			else if (iMouseCode == MOUSE_BUTTON_UP)
			{
				if (iEditMode == EDIT_MODE_RUBBER_STAMP)
					pDoc->m_bCloneReset = true;
			}

//			DrawStroke( pView, ptPos, ptLastPos, iDrawingWidth, pView->m_bLeftButtonTracking);
			DrawStroke( &Stroke);
			bNeedToRebuild = TRUE;
		}
		else if (pView->m_bRightButtonTracking)
		{
			if (iMouseCode == MOUSE_BUTTON_DOWN)
			{
				pDoc->m_ptCloneOffset     = ptPos;
				pDoc->m_bCloneInitialized = true;
				pDoc->m_bCloneReset       = true;

				if (iEditMode == EDIT_MODE_CLONE)
					pView->OnTpClone();
				else
				{
					ASSERT (g_iCurrentTool == EDIT_MODE_RUBBER_STAMP);
					pView->OnTpRubberStamp();
				}
			}
		}
		break;

	case EDIT_MODE_DECAL:
		if ((iDrawingMode != MODE_FREEHAND) || (iMouseCode == MOUSE_BUTTON_DOWN))
		{
			if (iMouseCode == MOUSE_BUTTON_DOWN)
			{
				BOOL bWarn = FALSE;

				if (pView->m_bLeftButtonTracking)
				{
					bWarn = ! g_LeftDecalToolLayerInfo.HasData();
				}
				else
				{
					bWarn = ! g_RightDecalToolLayerInfo.HasData();
				}
				
				if (bWarn)
				{
					static BOOL bFirstWarning = TRUE;

					if (bFirstWarning)
					{
						bFirstWarning = FALSE;

						AfxMessageBox( "Decal Tool has no data source set.  You must select a left and right button decal\n"
									   "image for the tool to use (which will be restored the next time you run Wally).");
					}
					else
					{
						MessageBeep( 0);
					}
					break;
				}
			}
			if ((iDrawingMode != MODE_FREEHAND) || (iMouseCode != MOUSE_BUTTON_UP)  || 
						(ptLastPos.x != ptPos.x) || (ptLastPos.y != ptPos.y))
			{
				if (iMouseCode != MOUSE_BUTTON_UP)
				{
//					DrawStroke( pView, ptLastPos, ptPos, iDrawingWidth, pView->m_bLeftButtonTracking);
					Stroke.m_ptImage  = ptLastPos;
					Stroke.m_ptImage2 = ptPos;
					DrawStroke( &Stroke);
				}
				else
				{
					pView->BeginWaitCursor();
					m_bFinalStroke = TRUE;
//					DrawStroke( pView, ptLastPos, ptPos, iDrawingWidth, pView->m_bLeftButtonTracking);
					Stroke.m_ptImage  = ptLastPos;
					Stroke.m_ptImage2 = ptPos;
					DrawStroke( &Stroke);
					m_bFinalStroke = FALSE;
					pView->EndWaitCursor();
				}
				bNeedToRebuild = TRUE;
			}
		}
		break;

	case EDIT_MODE_SPRAY:
	case EDIT_MODE_SPRAY_RECOLOR:
		if ((iMouseCode != MOUSE_BUTTON_UP) ||  (iDrawingMode != MODE_FREEHAND))
		{
			if ((iMouseCode == MOUSE_MOVE)  && 
						(pView->m_bLeftButtonTracking || pView->m_bRightButtonTracking))
			{
				// Neal - improves spray response
				// reset the timer for another 1/10 second

				pView->SetTimer( TIMER_TOOL_SPRAY, 100, NULL); // ten times/second
			}
//			DrawStroke( pView, ptPos, ptLastPos, iDrawingWidth, pView->m_bLeftButtonTracking);
			DrawStroke( &Stroke);
			bNeedToRebuild = TRUE;
		}
		break;

	case EDIT_MODE_BULLET_HOLES:
		{
			int idx = (ptPos.x - ptLastPos.x);
			int idy = (ptPos.y - ptLastPos.y);
			int iDistanceSquared = (idx*idx) + (idy*idy);

//			int iRequiredDistanceSquared = 6 * 6;
			int w, h, iRequiredDistanceSquared;

			//int iWidth = 1;

			if (pView->m_bLeftButtonTracking)
			{
				w = g_LeftBulletLayerInfo.GetClippedWidth( g_iBulletIndex);
				h = g_LeftBulletLayerInfo.GetClippedHeight( g_iBulletIndex);

				if ((w == 0) || (h == 0))
					w = h = 4;

				iRequiredDistanceSquared = w*w + h*h;
			}
			else
			{
				//iRequiredDistanceSquared = 12 * 12;

				w = g_RightBulletLayerInfo.GetClippedWidth( g_iBulletIndex);
				h = g_RightBulletLayerInfo.GetClippedHeight( g_iBulletIndex);

				if ((w == 0) || (h == 0))
					w = h = 7;

				iRequiredDistanceSquared = w*w + h*h;

				//iWidth = 2;
			}

			if ((iMouseCode == MOUSE_BUTTON_DOWN) ||
					(iDistanceSquared >= iRequiredDistanceSquared))
			{
				CSelection* pSel = pDoc->GetSelection();

				if ((ptPos.x >= 0) && (ptPos.x < pDoc->Width() * iMult) &&
							(ptPos.y >= 0) && (ptPos.y < pDoc->Height() * iMult))
				{
//					bNeedToRebuild = DrawBulletHole( pDoc,  pView, ptPos.x, ptPos.y, iMouseCode);
					bNeedToRebuild = DrawBulletHole( Stroke);
				}
			}
		}
		break;

	case EDIT_MODE_RIVETS:
		if (iMouseCode != MOUSE_BUTTON_UP)
		{
//			bNeedToRebuild = DrawRivet( pDoc, pView, ptPos.x, ptPos.y, iMouseCode);
			bNeedToRebuild = DrawRivet( Stroke);
		}
		break;

	case EDIT_MODE_DENT:
		ASSERT( FALSE);		// not implimented yet
		break;
	case EDIT_MODE_BUMP:
		ASSERT( FALSE);		// not implimented yet
		break;

	case EDIT_MODE_DRIP:
		ASSERT( FALSE);		// not implimented yet
		break;
	case EDIT_MODE_SHARPEN:
		ASSERT( FALSE);		// not implimented yet
		break;

	case EDIT_MODE_FLOOD_FILL:
		if ((iMouseCode == MOUSE_BUTTON_DOWN) && (! g_bFloodInProgress))
		{
			pt = ptPos;

			if (pt.y < 0)
				pt.y += 1000 * iHeight;		// negative modulus problem work-around
			pt.y %= iHeight;

			if (pt.x < 0)
				pt.x += 1000 * iWidth;		// negative modulus problem work-around
			pt.x %= iWidth;

			//int iIndex = (pt.y * iWidth) + pt.x;
			//int iInteriorColor = pDoc->GetMipPixel( iIndex, 0);
			//int iInteriorColor = GetIValue( pDoc->GetPixel( pt.x, pt.y));
			COLOR_IRGB irgbInteriorColor = pDoc->GetPixel( pt.x, pt.y);

			FloodFill( pView, pt, irgbInteriorColor, irgbColor);

			bNeedToRebuild = TRUE;
		}
		break;

	default:
		ASSERT( FALSE);		// missed a new tool?
	}

	// save the last drawn position
	//
	// Bullet Holes and Rivets are an exception because 
	// they handle their own spacing (so that they don't 
	// get placed too close together)

	if ((iEditMode != EDIT_MODE_BULLET_HOLES) && 
			(iEditMode != EDIT_MODE_RIVETS))
	{
		// remember previous last pos (for scratch tool)

		if (ptLastPos != ptPos)
			ptTrail = ptLastPos;

		ptLastPos = ptPos;
	}

	if (bNeedToRebuild)
	{				
		pView->RebuildImageViews();
	}
	if (IsDrawingTool( iEditMode))
	{
		if (iMouseCode == MOUSE_BUTTON_UP)
		{
			// remip in one second
			pView->SetTimer( TIMER_TOOL_AUTO_REMIP, 1000, NULL);
		}
		else if (iMouseCode == MOUSE_BUTTON_DOWN)
		{
			// cancel remip
			pView->KillTimer( TIMER_TOOL_AUTO_REMIP );
		}
	}

	if (iMouseCode == MOUSE_BUTTON_UP)
	{

		if (iDrawingMode == MODE_LINE)
		{
			m_bLastMouseDownIsValid = FALSE;
		}
		// free mem
		pDoc->m_DrawingModeUndo.ClearUndoBuffer();
		pDoc->m_DrawingModeUndo.ClearRedoBuffer();
	}
	if (bNeedToRebuild)
	{
		pView->UpdateWindow();
	}
}

void CTool::InitTool( CWallyView* pView, int iToolMode)
{
	int iPrevTool  = g_iCurrentTool;
	g_iCurrentTool = iToolMode;

	pView->UpdateColorPalette();
	UpdateConstrainPaneText();

	if (IsAnyEffect( g_iCurrentTool))
	{
		CWallyDoc* pDoc = pView->GetDocument();

		// neal - first, free left-over mip buffer

		pDoc->FreeEffectsData();

		// neal - next, alloc mip buffer

		int iWidth  = pDoc->Width();
		int iHeight = pDoc->Height();
//		int iSize = pDoc->Width() * pDoc->Height();

		pDoc->m_pEffectLayer = new CLayer;

		// neal - then, copy current mip image there

		pDoc->CopyLayerToMip();

		// Neal - bugfix for CopyLayer assert
//		pDoc->m_pEffectLayer->Create( pDoc, iWidth, iHeight, 24);
		pDoc->m_pEffectLayer->Create( pDoc, iWidth, iHeight, pDoc->GetCurrentLayer()->GetNumBits());

		//pDoc->m_pEffectLayer->CopyLayer( pView, pDoc->GetSelection()->GetLayer());
		pDoc->m_pEffectLayer->CopyLayer( pView, pDoc->GetCurrentLayer());

		if (g_iCurrentTool == EDIT_MODE_RUBBER_STAMP)
		{
			pDoc->m_bCloneReset  = true;
		}
		else if ((g_iCurrentTool != EDIT_MODE_SELECTION) &&
					(iPrevTool == EDIT_MODE_SELECTION))
		{
			// Ty- this isn't in CWallyDoc... maybe missed with a code drop?
			// Neal - I started working again on cut-n-paste (a long while back)
			//        (I don't seem to have the code any more)  :-(
			//        It wasn't a whole lot of work, though.
			//
			pDoc->MergeSelectionIntoBase( "Tool Changed, Selection Merged");
		}
	}

	CMainFrame*    pWndMain    = (CMainFrame* )AfxGetMainWnd();
	CToolSettingsToolbar *pWndToolbar = &pWndMain->m_wndToolSettingsToolbar;
	pWndToolbar->EnableControls (IsDecalTool (g_iCurrentTool));
	pWndToolbar->EnableRightButton (g_iCurrentTool != EDIT_MODE_RIVETS);	
}
