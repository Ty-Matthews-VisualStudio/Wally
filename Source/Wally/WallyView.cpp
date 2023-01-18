/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyView.cpp : implementation of the CWallyView class
//
// Created by Ty Matthews, 1-16-1998
/////////////////////////////////////////////////////////////////////////////

//  GetWindowPlacement

#include "stdafx.h"
#include "Wally.h"
#include "Afxext.h"
#include <afxole.h>
#include <math.h>
#include <mmsystem.h>

#include "WallyDoc.h"
#include "MainFrm.h"
#include "Tool.h"
//#include "Layer.h"
#include "resource.h"
//#include "ReMip.h"
#include "ClipboardDIB.h"
#include "DibSection.h"
#include "GraphicsFunctions.h"
#include "CopyTiledDlg.h"
#include "RotateDlg.h"
#include "MiscFunctions.h"
#include "Filter.h"
#include "QuakeUnrealDlg.h"
#include "WallyPropertySheet.h"
#include "FreeUndoDlg.h"
#include "ColorOpt.h"
#include "PaletteEditorDlg.h"
#include "ColorTranslatorDlg.h"
#include "PasteSpecialDlg.h"
#include "DecalToolbar.h"
#include "EnlargeCanvasDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
extern CTool     g_Tool;
//CPoint g_ptLine1, g_ptLine2;

// trail is previous last point (used for improved scratch tool)
//CPoint ptLastPos, ptTrail;

//BYTE g_byEffectsIndexLookUp[3][256];
BYTE g_byCursorLookUp[256];

extern int        g_iLastLeftColor;
extern int        g_iLastRightColor;
extern COLOR_IRGB g_irgbLastLeftColor;
extern COLOR_IRGB g_irgbLastRightColor;
extern int        g_iDarkenPercent;
extern double     g_dfLightenPercent;


extern BOOL g_bConstrained;
extern BOOL g_bConstrainedHorizonal;
extern BOOL g_bFlipped;
//extern BOOL g_bToolsUseTransparency255;

//int  iMixColor                  = 255;
//int  iBulletIndex               = -1;

//#define TIMER_REMIP         1001
//#define TIMER_AUTO_REMIP    1002
//#define TIMER_SPRAY         1003
//#define TIMER_CLEAR_CURSOR  1004
//#define TIMER_ANIMATE_SELECTION  1005

#define RGB_GRAY		(RGB( 128, 128, 128))
#define RGB_RED			(RGB( 192,  32,  32))
#define RGB_BLUE		(RGB(   0,   0, 128))


//#define SPRAY_COVERAGE 25		// 25 percent coverage

//#define _PI_	3.141592654		// neal - "_PI_" is much easier to find than "PI"


#define IsKeyPressed( vk) (GetKeyState( vk) < 0)
//#define sq( x)            ((x) * (x))


UINT GetKeyFlags( void)
{
	UINT uFlags = 0;

	uFlags |= IsKeyPressed( VK_SHIFT)   ? MK_SHIFT   : 0;
	uFlags |= IsKeyPressed( VK_CONTROL) ? MK_CONTROL : 0;
	uFlags |= IsKeyPressed( VK_LBUTTON) ? MK_LBUTTON : 0;
	uFlags |= IsKeyPressed( VK_MBUTTON) ? MK_MBUTTON : 0;
	uFlags |= IsKeyPressed( VK_RBUTTON) ? MK_RBUTTON : 0;

	return (uFlags);
}

//inline int FindNearestColorByIndex( int iColorIndex, int iMult, int iDiv, int iAdd)
//{
//	int r = (quake2_pal[iColorIndex*3])     * iMult / iDiv + iAdd;
//	int g = (quake2_pal[iColorIndex*3 + 1]) * iMult / iDiv + iAdd;
//	int b = (quake2_pal[iColorIndex*3 + 2]) * iMult / iDiv + iAdd;
//
//	return (FindNearestColor( r, g, b, FALSE));
//}

/////////////////////////////////////////////////////////////////////////////
// CWallyView

IMPLEMENT_DYNCREATE(CWallyView, CScrollView)


//DWORD dwWinVer = ::GetVersion();
//static UINT uMsgMouseWheel = ::RegisterWindowMessage( MSH_MOUSEWHEEL);

CString strMouseWheelMsg( MSH_MOUSEWHEEL);

/*
// register for Windows 95 or Windows NT 3.51
static UINT uMsgMouseWheel =
   (((::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 4)) ||
	 (!(::GetVersion() & 0x80000000) && LOBYTE(LOWORD(::GetVersion()) == 3)))
	 ? ::RegisterWindowMessage( MSH_MOUSEWHEEL) : 0;
*/

BEGIN_MESSAGE_MAP(CWallyView, CScrollView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_ZOOMIN, OnEditZoomIn)
	ON_COMMAND(ID_EDIT_ZOOMOUT, OnEditZoomOut)
//	ON_COMMAND(ID_EDIT_SHIFT_UP, OnEditShiftUp)
//	ON_COMMAND(ID_EDIT_SHIFT_DOWN, OnEditShiftDown)
//	ON_COMMAND(ID_EDIT_SHIFT_LEFT, OnEditShiftLeft)
//	ON_COMMAND(ID_EDIT_SHIFT_RIGHT, OnEditShiftRight)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CWallyView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_TP_TILE, OnTpTile)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_TP_ANIMATE, OnTpAnimate)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_POPUP_COPY_AS_BITMAP, OnPopupCopyAsBitmap)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_TP_TILE, OnUpdateTpTile)
	ON_COMMAND(ID_TP_REMIP, OnTpRemip)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_TP_ZOOM, OnTpZoom)
	ON_UPDATE_COMMAND_UI(ID_TP_ZOOM, OnUpdateTpZoom)
	ON_COMMAND(ID_TP_SELECTION, OnTpSelection)
	ON_COMMAND(ID_TP_DROPPER, OnTpDropper)
	ON_UPDATE_COMMAND_UI(ID_TP_DROPPER, OnUpdateTpDropper)
	ON_COMMAND(ID_TP_PENCIL, OnTpPencil)
	ON_UPDATE_COMMAND_UI(ID_TP_PENCIL, OnUpdateTpPencil)
	ON_COMMAND(ID_TP_LIGHTEN, OnTpLighten)
	ON_UPDATE_COMMAND_UI(ID_TP_LIGHTEN, OnUpdateTpLighten)
	ON_COMMAND(ID_TP_ERASER, OnTpEraser)
	ON_UPDATE_COMMAND_UI(ID_TP_ERASER, OnUpdateTpEraser)
	ON_COMMAND(ID_TP_DARKEN, OnTpDarken)
	ON_UPDATE_COMMAND_UI(ID_TP_DARKEN, OnUpdateTpDarken)
	ON_COMMAND(ID_TP_BRUSH, OnTpBrush)
	ON_UPDATE_COMMAND_UI(ID_TP_BRUSH, OnUpdateTpBrush)
	ON_COMMAND(ID_TP_SCRATCH, OnTpScratch)
	ON_UPDATE_COMMAND_UI(ID_TP_SCRATCH, OnUpdateTpScratch)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_TP_BULLET_HOLES, OnTpBulletHoles)
	ON_UPDATE_COMMAND_UI(ID_TP_BULLET_HOLES, OnUpdateTpBulletHoles)
	ON_COMMAND(ID_TP_RIVETS, OnTpRivets)
	ON_UPDATE_COMMAND_UI(ID_TP_RIVETS, OnUpdateTpRivets)
	ON_COMMAND(ID_TP_AMOUNT, OnTpAmount)
	ON_UPDATE_COMMAND_UI(ID_TP_AMOUNT, OnUpdateTpAmount)
	ON_COMMAND(ID_TP_WIDTH, OnTpWidth)
	ON_UPDATE_COMMAND_UI(ID_TP_WIDTH, OnUpdateTpWidth)
	ON_COMMAND(ID_TP_TINT, OnTpTint)
	ON_UPDATE_COMMAND_UI(ID_TP_TINT, OnUpdateTpTint)
	ON_COMMAND(ID_TP_SHAPE, OnTpShape)
	ON_UPDATE_COMMAND_UI(ID_TP_SHAPE, OnUpdateTpShape)
	ON_COMMAND(ID_TP_REPLACE, OnTpReplace)
	ON_UPDATE_COMMAND_UI(ID_TP_REPLACE, OnUpdateTpReplace)
	ON_COMMAND(ID_TP_CLONE, OnTpClone)
	ON_UPDATE_COMMAND_UI(ID_TP_CLONE, OnUpdateTpClone)
	ON_COMMAND(ID_TP_RUBBER_STAMP, OnTpRubberStamp)
	ON_UPDATE_COMMAND_UI(ID_TP_RUBBER_STAMP, OnUpdateTpRubberStamp)
	ON_COMMAND(ID_TP_SPRAY, OnTpSpray)
	ON_UPDATE_COMMAND_UI(ID_TP_SPRAY, OnUpdateTpSpray)
	ON_COMMAND(ID_TP_FLOOD_FILL, OnTpFloodFill)
	ON_UPDATE_COMMAND_UI(ID_TP_FLOOD_FILL, OnUpdateTpFloodFill)
	ON_COMMAND(ID_EDIT_COPYTILED, OnEditCopyTiled)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_TP_PATTERNED_PAINT, OnTpPatternedPaint)
	ON_UPDATE_COMMAND_UI(ID_TP_PATTERNED_PAINT, OnUpdateTpPatternedPaint)
	ON_COMMAND(ID_IMAGE_MIRROR, OnImageMirror)
	ON_COMMAND(ID_IMAGE_ROTATE, OnImageRotate)
	ON_WM_SIZE()
	ON_COMMAND(ID_TP_RECOLOR, OnTpRecolor)
	ON_UPDATE_COMMAND_UI(ID_TP_RECOLOR, OnUpdateTpRecolor)
	ON_COMMAND(ID_TP_SPRAY_RECOLOR, OnTpSprayRecolor)
	ON_UPDATE_COMMAND_UI(ID_TP_SPRAY_RECOLOR, OnUpdateTpSprayRecolor)
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_DRAWINGMODE_FREEHAND, OnDrawingModeFreehand)
	ON_COMMAND(ID_DRAWINGMODE_LINE, OnDrawingModeLine)
	ON_COMMAND(ID_DRAWINGMODE_POLYLINE, OnDrawingModePolyline)
	ON_COMMAND(ID_DRAWINGMODE_RAY, OnDrawingModeRay)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_FREEHAND, OnUpdateDrawingModeFreehand)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_LINE, OnUpdateDrawingModeLine)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_POLYLINE, OnUpdateDrawingModePolyline)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_RAY, OnUpdateDrawingModeRay)
	ON_COMMAND(ID_DRAWINGMODE_HOLLOW_RECT, OnDrawingModeHollowRect)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_HOLLOW_RECT, OnUpdateDrawingModeHollowRect)
	ON_COMMAND(ID_DRAWINGMODE_SOLID_RECT, OnDrawingModeSolidRect)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_SOLID_RECT, OnUpdateDrawingModeSolidRect)
	ON_COMMAND(ID_DRAWINGMODE_EDGED_RECT, OnDrawingModeEdgedRect)
	ON_UPDATE_COMMAND_UI(ID_DRAWINGMODE_EDGED_RECT, OnUpdateDrawingModeEdgedRect)
	ON_COMMAND(ID_IMAGE_BRIGHTNESS_CONTRAST, OnImageBrightnessContrast)
	ON_COMMAND(ID_IMAGE_BLEND, OnImageBlend)
	ON_COMMAND(ID_IMAGE_ADD_NOISE, OnImageAddNoise)
	ON_COMMAND(ID_IMAGE_EMBOSS, OnImageEmboss)
	ON_COMMAND(ID_IMAGE_EDGE_DETECT, OnImageEdgeDetect)
	ON_COMMAND(ID_IMAGE_GENERATE_MARBLE_TEXTURE, OnImageGenrateMarbleTexture)
	ON_COMMAND(ID_IMAGE_CONVERT_TEXTURES_QUAKE_TO_FROM_UNREAL, OnImageConvertTexturesQuakeToFromUnreal)
	ON_COMMAND(ID_IMAGE_GENERATE_MARBLE_TILE_TEXTURE, OnImageGenerateMarbleTileTexture)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_EDIT_FREEUNDOMEMORY, OnFreeUndoMemory)
	ON_COMMAND(ID_IMAGE_DIFFUSE, OnImageDiffuse)
	ON_COMMAND(ID_IMAGE_FLIP_HORZ, OnImageFlipHorz)
	ON_COMMAND(ID_IMAGE_FLIP_VERT, OnImageFlipVert)	
	ON_COMMAND(ID_IMAGE_OFFSET, OnImageOffset)
	ON_COMMAND(ID_IMAGE_SHARPEN, OnImageSharpen)
	ON_COMMAND(ID_COLORS_EDIT_PALETTE, OnColorsEditPalette)
	ON_COMMAND(ID_COLORS_LOAD_PALETTE, OnColorsLoadPalette)
	ON_COMMAND(ID_COLORS_SAVE_PALETTE, OnColorsSavePalette)
	ON_COMMAND(ID_COLORS_COUNT_COLORS_USED, OnColorsCountColorsUsed)
	ON_COMMAND(ID_COLORS_TRANSLATE, OnColorsTranslate)
	ON_COMMAND(ID_IMAGE_RESIZE, OnImageResize)
	ON_UPDATE_COMMAND_UI(ID_TP_SELECTION, OnUpdateTpSelection)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_ROTATE, OnUpdateImageRotate)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_RESIZE, OnUpdateImageResize)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_OFFSET, OnUpdateImageOffset)
	ON_COMMAND(ID_SELECTION_SELECT_ALL, OnSelectionSelectAll)
	ON_COMMAND(ID_SELECTION_SELECT_NONE, OnSelectionSelectNone)
	ON_UPDATE_COMMAND_UI(ID_SELECTION_SELECT_NONE, OnUpdateSelectionSelectNone)
	ON_COMMAND(ID_IMAGE_REDUCE_COLORS, OnImageReduceColors)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_REDUCE_COLORS, OnUpdateImageReduceColors)
	ON_COMMAND(ID_REPEAT_FILTER_PICK0, OnRepeatFilterPick0)
	ON_UPDATE_COMMAND_UI(ID_REPEAT_FILTER_PICK0, OnUpdateRepeatFilterPick0)
	ON_COMMAND(ID_REPEAT_FILTER_PICK1, OnRepeatFilterPick1)
	ON_UPDATE_COMMAND_UI(ID_REPEAT_FILTER_PICK1, OnUpdateRepeatFilterPick1)
	ON_COMMAND(ID_REPEAT_FILTER_PICK2, OnRepeatFilterPick2)
	ON_UPDATE_COMMAND_UI(ID_REPEAT_FILTER_PICK2, OnUpdateRepeatFilterPick2)
	ON_COMMAND(ID_REPEAT_FILTER_PICK3, OnRepeatFilterPick3)
	ON_UPDATE_COMMAND_UI(ID_REPEAT_FILTER_PICK3, OnUpdateRepeatFilterPick3)
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_PASTE_SPECIALEFFECTS, OnEditPasteSpecialEffects)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SPECIALEFFECTS, OnUpdateEditPasteSpecialEffects)
	ON_COMMAND(ID_TP_DECAL, OnTpDecal)
	ON_UPDATE_COMMAND_UI(ID_TP_DECAL, OnUpdateTpDecal)
	ON_WM_MOUSEWHEEL()
	//ON_REGISTERED_MESSAGE(uMsgMouseWheel, OnRegisteredMouseWheel)
	ON_COMMAND(ID_TP_BLEND, OnTpBlend)
	ON_UPDATE_COMMAND_UI(ID_TP_BLEND, OnUpdateTpBlend)
	ON_COMMAND(ID_IMAGE_ENLARGE, OnImageEnlarge)
	ON_COMMAND(ID_IMAGE_TILED_TINT, OnImageTiledTint)
	ON_COMMAND(ID_IMAGE_FIX_UNEVEN_LIGHTING, OnImageFixUnevenLighting)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyView construction/destruction

CWallyView::CWallyView()
{	
	// Initialize the boolean flags
	m_bTile                = FALSE;
	m_bLeftButtonTracking  = FALSE;
	m_bRightButtonTracking = FALSE;
	//DataModified         = TRUE;
	m_iZoomValue           = g_iDefaultZoomValue;

	if (m_iZoomValue == 0)
		m_dfZoom = 1.0;
	else
		SetZoom( FALSE);

	m_iMouseWheelFactor    = 0;
	m_iMouseWheelAggregate = 0;

	m_bSettingScrollBars	= FALSE;
	m_bInitialized			= FALSE;
	m_bFirstPaint			= TRUE;
	m_bHasSubMips			= FALSE;
	m_bUnknownType			= TRUE;

	m_iLastTool          = -1;
	m_iLastLeftColor     = -1;
	m_iLastRightColor    = -1;
	g_iLastLeftColor     = -2;
	g_iLastRightColor    = -2;
	g_iDarkenPercent     = -2;
	g_dfLightenPercent   = -2.0;

//	m_iLastBigCursorSize = -1;
	m_bNeedToEraseBkgnd  = TRUE;

	m_bLastMouseDownIsValid = FALSE;
	m_bLastMouseUpIsValid   = FALSE;
	
	m_iShiftCountLeftRight = 0;
	m_iShiftCountUpDown    = 0;

	m_nClipboardFormat = ::RegisterClipboardFormat(_T("Quake2 .wal file"));	

	m_imgList.m_hImageList = NULL;
	VERIFY(m_imgList.Create(IDB_WALLYDOC_ICONS, 16, 1, RGB (255, 0, 255)));
}

CWallyView::~CWallyView()
{
	m_imgList.DeleteImageList();
}

BOOL CWallyView::PreCreateWindow( CREATESTRUCT& cs)
{	
	return CScrollView::PreCreateWindow( cs);
}

// pbyInitializationPalette is "packed" and is used a flag
// that indicates if the DIB needs to be initialized
void CWallyView::SetRawDibBits( int iDibIndex, BYTE* pbyInitializationPalette, bool bOnlyToolDirtyArea)
{
	// only DIBs 0 thru 3 are vaild
	ASSERT( (iDibIndex >= 0) && (iDibIndex < 4));

	CWallyDoc* pDoc = GetDocument();

	//m_DIB[iDibIndex].SetRawBits( pDoc->GetBits( iDibIndex));

	int iNumBits = pDoc->GetNumBits();
	int iWidth   = pDoc->MipArray[iDibIndex].width;
	int iHeight  = pDoc->MipArray[iDibIndex].height;

	if (iNumBits == 8)
	{
		if (pbyInitializationPalette)
			m_DIB[iDibIndex].Init( iWidth, iHeight, iNumBits, pbyInitializationPalette);

		m_DIB[iDibIndex].SetRawBits( pDoc->GetBits( iDibIndex));
	}
	else if ((iNumBits == 24) || (iNumBits == 32))
	{
		if (iDibIndex == 0)
		{
			if (pbyInitializationPalette)
			{
				// Neal - always use 24, even for 32 bit images
				m_DIB[iDibIndex].Init( iWidth, iHeight, 24, pbyInitializationPalette);
			}

			m_DIB[iDibIndex].SetBitsFromLayer( &pDoc->GetBase()->m_Layer, NULL, bOnlyToolDirtyArea, FALSE);

			if (pDoc->HasSelection())
			{
				CLayer* pLayer = &pDoc->GetCutout()->m_Layer;
				m_DIB[iDibIndex].SetBitsFromLayer( pLayer, pLayer->m_rBoundsRect, false, m_bTile);
			}
		}
		else
		{
			if (pbyInitializationPalette)
				m_DIB[iDibIndex].Init( iWidth, iHeight, 8, pbyInitializationPalette);

// Neal - BUGFIX for resize-smaller/undo bug ???

			ASSERT((m_DIB[iDibIndex].GetWidth() == iWidth) &&
					(m_DIB[iDibIndex].GetHeight() == iHeight));

			m_DIB[iDibIndex].SetRawBits( pDoc->GetBits( iDibIndex));
		}
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CWallyView::RebuildImageViews()
{	
	CWallyDoc* pDoc = GetDocument();
	ASSERT( pDoc);

	if (pDoc)
	{
#ifdef _DEBUG
		pDoc->AssertValid();
#endif

		pDoc->CopyLayerToMip();
		//m_DIB[0].SetRawBits( pDoc->GetBits());
		SetRawDibBits( 0, NULL, false);

		pDoc->SetModifiedFlag( TRUE);
		pDoc->UpdateAllViews( NULL, HINT_UPDATE_DIBS);
	}
}

void CWallyView::DrawGrid( CDC* pDC, const CRect& rClient, const CPoint& ptScrollPos)
{
	// *** neal - as you can see, i tried many approaches,  ***
	// *** trying to find one that was fast and looked good ***

	COLORREF rgbOldTextColor = pDC->GetTextColor();
	COLORREF rgbOldBkColor   = pDC->GetBkColor();

	CBrush  brSolidGrid, brBoldGrid;
	CBrush* pbrOld = NULL;

	int iDibWidth  = m_DIB[0].GetImageWidth();
	int iDibHeight = m_DIB[0].GetImageHeight();

	TRY
	{
		int   i, iSpacing;
		BOOL  bBrighter;
		CRect rTotal( 0, 0, 0, (int )(iDibHeight * m_dfZoom));

		if ((m_iZoomValue >= 4) && (g_bShowFineGrid))
		{
			int iRed   = 0;
			int iGreen = 0;
			int iBlue  = 0;

			if ((m_iZoomValue >= 4) && (g_bShowFineGrid))
			{
				iRed   = GetRValue( g_rgbGridColor);
				iGreen = GetGValue( g_rgbGridColor);
				iBlue  = GetBValue( g_rgbGridColor);

				// a contrasting version of grid color

				// neal - this is more efficent
				int iRG = max( iRed, iGreen);
				int iRB = max( iRed, iBlue);

				if (max( iRG, iRB) >= 85)
				{
					iRed   /= 2;	// darker
					iGreen /= 2;
					iBlue  /= 2;
				}
				else
				{
					iRed   += 85;	// lighter
					iGreen += 85;
					iBlue  += 85;

					ClampRGB( &iRed, &iGreen, &iBlue);
				}
			}

			brSolidGrid.CreateSolidBrush( RGB( iRed, iGreen, iBlue));
			pbrOld = (CBrush *)pDC->SelectObject( &brSolidGrid);

			//////////////////////////////////
			// draw the single pixel lines //
			////////////////////////////////

			iSpacing = (g_bShowCourseGrid) ? g_iGridHorizontal : 9999999;

			if (iSpacing > 1)
			{
				for (i = 1; i <= iDibWidth; i++)
				{
					bBrighter = ((i % iSpacing) == 0);
					if (! bBrighter)
					{
						rTotal.right = (int )(i * m_dfZoom);
						rTotal.left  = rTotal.right - 1;
						pDC->BitBlt( rTotal.left, rTotal.top, 1, rTotal.Height(), 
							pDC, rTotal.left, rTotal.top, PATCOPY);
					}
				}
			}

			rTotal.left  = 0;
			rTotal.right = (int )(iDibWidth * m_dfZoom);

			iSpacing = (g_bShowCourseGrid) ? g_iGridVertical : 9999999;

			if (iSpacing > 1)
			{
				for (i = 1; i <= iDibHeight; i++)
				{
					bBrighter = ((i % iSpacing) == 0);
					if (! bBrighter)
					{
						rTotal.bottom = (int )(i * m_dfZoom);
						rTotal.top    = rTotal.bottom - 1;

						pDC->BitBlt( rTotal.left, rTotal.top, rTotal.Width(), 1, 
							pDC, rTotal.left, rTotal.top, PATCOPY);
					}
				}
			}
		}

		///////////////////////////////
		// draw the bold grid lines //
		/////////////////////////////

		iSpacing = (int )(g_iGridHorizontal * m_dfZoom);
		i        = (int )(g_iGridVertical   * m_dfZoom);
		iSpacing = min( i, iSpacing);

		if (g_bShowCourseGrid && (iSpacing >= 4))
		{
			brBoldGrid.CreateSolidBrush( g_rgbGridColor);

			rTotal.top    = 0;
			rTotal.bottom = (int )(iDibHeight * m_dfZoom);

			for (i = g_iGridHorizontal; i <= iDibWidth; i += g_iGridHorizontal)
			{
				rTotal.right = (int )(i * m_dfZoom);
				rTotal.left  = rTotal.right - 1;
				pDC->FillRect( rTotal, &brBoldGrid);
			}

			rTotal.left  = 0;
			rTotal.right = (int )(iDibWidth * m_dfZoom);

			for (i = g_iGridVertical; i <= iDibHeight; i += g_iGridVertical)
			{
				rTotal.bottom = (int )(i * m_dfZoom);
				rTotal.top    = rTotal.bottom - 1;
				pDC->FillRect( rTotal, &brBoldGrid);
			}
		}
	}
	CATCH_ALL( e)
	{
		MessageBeep( MB_ICONEXCLAMATION);
		TRACE0( "CWallyView::DrawGrid - exception was thrown\n");
	}
	END_CATCH_ALL

	pDC->SelectObject( pbrOld);
	DeleteObject( brSolidGrid);
	DeleteObject( brBoldGrid);

	pDC->SetTextColor( rgbOldTextColor);
	pDC->SetBkColor( rgbOldBkColor);
}

/////////////////////////////////////////////////////////////////////////////
// CWallyView drawing

void CWallyView::OnDraw( CDC* pDC)
{
	CDC     MemDC;
	CDC    *pUseDC = NULL;		// Neal - pUseDC fixes print preview
	CBitmap Bmp;
	CBitmap BmpBackground;

	TRY
	{
		CWallyDoc* pDoc = GetDocument();

		if (m_iZoomValue == 0)
		{
			ASSERT (FALSE);
			GetDocSize (NULL);
		}

		if (pDoc->GetSelection()->GetLayer()->GetNumBits() != pDoc->GetOriginalBitDepth())
		{
			ASSERT( FALSE);
			pDoc->GetSelection()->GetLayer()->SetNumBits( pDoc->GetOriginalBitDepth());
		}

		if (m_bFirstPaint)
		{
			UpdateColorPalette();
			
			CWallyPalette* pPal = pDoc->GetPalette();

			for (int i = 0; i < 256; i++)
			{
				int iRed   = pPal->GetR( i);
				int iGreen = pPal->GetG( i);
				int iBlue  = pPal->GetB( i);

				// build a nice contrasting color
				iRed   = (3 - (iRed   / 64)) * 85;
				iGreen = (3 - (iGreen / 64)) * 85;
				iBlue  = (3 - (iBlue  / 64)) * 85;

				g_byCursorLookUp[i] = pDoc->FindNearestColor( iRed, iGreen, iBlue, FALSE);
			}	

			m_bFirstPaint = FALSE;
		}

		CRect rClient;
		GetClientRect( &rClient);

		double dfZoom = m_dfZoom;

		int iGap         = 8;
		int iPrintFactor = 1;		// Neal - multiplier for streching image

		if (pDC->IsPrinting())
		{
			iGap *= 2;				// Neal - make it wider (printer res is higher than screen)

			CRect rBounds(0, 0, pDC->GetDeviceCaps( HORZRES), pDC->GetDeviceCaps( VERTRES));
			rClient = rBounds;		// Neal - long way (for code clarity)

			rBounds.right  -= iGap;
			rBounds.bottom -= iGap;
			SIZE sizeDoc    = CalcDocSize( rBounds, 1, 0);

			iPrintFactor = rBounds.Width() / sizeDoc.cx;
			iPrintFactor = max( 1, iPrintFactor);
			dfZoom = iPrintFactor;
		}
		int iMult = (m_bTile) ? 3 : 1;
		
		int ModWidth   = (int )(m_DIB[0].GetImageWidth()  * dfZoom) * iMult;
		int ModHeight  = (int )(m_DIB[0].GetImageHeight() * dfZoom) * iMult;
		int Mod1Width  = (int )(m_DIB[1].GetImageWidth()  * dfZoom) * iMult;
		int Mod1Height = (int )(m_DIB[1].GetImageHeight() * dfZoom) * iMult;
		int Mod2Width  = (int )(m_DIB[2].GetImageWidth()  * dfZoom) * iMult;

		if (rClient.Width() >= ModWidth + iGap + Mod1Width)
		{
			// position horizontally

			m_ptWrapOffset[1].x = ModWidth   + iGap;
			m_ptWrapOffset[1].y = 0;
			m_ptWrapOffset[2].x = ModWidth   + iGap;
			m_ptWrapOffset[2].y = Mod1Height + iGap;

		}
		else	// position vertically
		{
			m_ptWrapOffset[1].x = 0;
			m_ptWrapOffset[1].y = ModHeight + iGap;
			m_ptWrapOffset[2].x = Mod1Width + iGap;
			m_ptWrapOffset[2].y = ModHeight + iGap;
		}
		m_ptWrapOffset[0].x = 0;
		m_ptWrapOffset[0].y = 0;
		m_ptWrapOffset[3].x = m_ptWrapOffset[2].x + Mod2Width + iGap;
		m_ptWrapOffset[3].y = m_ptWrapOffset[2].y;

		// Neal - use memory DC to prevent flashing

		VERIFY( MemDC.CreateCompatibleDC( pDC));

		GetDocSize( &rClient);

		// Neal - fixes printout stretched bug
		CSize sizeDoc( max(m_DocSize.cx, rClient.Width()), max(m_DocSize.cy, rClient.Height()));

		//  Neal - part of scroll bug fix
//		if (Bmp.CreateCompatibleBitmap( pDC, rClient.Width(), rClient.Height()))
		if (Bmp.CreateCompatibleBitmap( pDC, sizeDoc.cx, sizeDoc.cy))
		{
			pUseDC = &MemDC;		// Neal - pUseDC fixes print preview
			VERIFY( pUseDC->SelectObject( &Bmp));
		}
		else	// Neal - above can fail during print preview (bmp too big?)
		{
			pUseDC = pDC;
		}

		CPoint ptScrollPos( GetDeviceScrollPosition());
		pDC->SetViewportOrg( pDC->GetViewportOrg());

		if (pDC->IsPrinting())
		{
			VERIFY( pUseDC->PatBlt(0, 0, rClient.Width(), rClient.Height(), WHITENESS));
		}
		else
		{
			// Neal - diagonal stripe background

			m_bNeedToEraseBkgnd = FALSE;

			static DWORD BitsDiag[4]  = { 0x00330066, 0x00CC0099, 0x00330066, 0x00CC0099 };
			static DWORD BitsAlpha[4] = { 0x00230001, 0x00230033, 0x00890001, 0x008900FF };
			//static DWORD Bits32[4]    = { 0x005700BF, 0x00BB00CB, 0x005700D7, 0x00E300AF };

			COLORREF rgbStripeColor = RGB_GRAY;

			// Neal - TEST TEST TEST
			//static BOOL bHasAlphaChannel = FALSE;
			//static BOOL bIsAlphaChannel  = FALSE;

			if (pDoc->HasAlphaChannel())
			{
				rgbStripeColor = pDoc->IsAlphaChannelSelected() ? RGB_RED : RGB_BLUE;

				BmpBackground.CreateBitmap( 8, 8, 1, 1, &BitsAlpha);
			}
			else
			{
				BmpBackground.CreateBitmap( 8, 8, 1, 1, &BitsDiag);
			}

			COLORREF rgbOldTextColor = pUseDC->SetTextColor( rgbStripeColor);
			COLORREF rgbOldBkColor   = pUseDC->SetBkColor( RGB( 64, 64, 64));

			CBrush brBackground( &BmpBackground);

//			// Neal - don't do this (fixes scrolled background bug)
//			pUseDC->SetBrushOrg( -ptScrollPos.x, -ptScrollPos.y);
//			brBackground.UnrealizeObject();

			CRect rTotal( 0, ptScrollPos.x, rClient.Width() + ptScrollPos.x, 0);

			// neal - don't fill area under large mip (speed-up)

			rTotal.top    = iMult * (int )(m_DIB[0].GetImageHeight() * dfZoom);
			rTotal.bottom = rClient.Height() + ptScrollPos.y;
			pUseDC->FillRect( rTotal, &brBackground);

			rTotal.bottom = rTotal.top + 1;
			rTotal.top    = 0;
			rTotal.left   = iMult * (int )(m_DIB[0].GetImageWidth() * dfZoom);
			pUseDC->FillRect( rTotal, &brBackground);

			VERIFY( brBackground.DeleteObject());

			pUseDC->SetTextColor( rgbOldTextColor);
			pUseDC->SetBkColor( rgbOldBkColor);
		}

//		if (pDoc->HasSelection() && (pDoc->GetOriginalBitDepth() > 8))
//		{
//			// rebuild dib
//
//			m_DIB[0].SetBitsFromLayer( &pDoc->GetBase()->m_Layer, NULL, false);
//
//			CLayer* pLayer = &pDoc->GetCutout()->m_Layer;
//			m_DIB[0].SetBitsFromLayer( pLayer, pLayer->m_rBoundsRect, false);
//		}

		if (m_bTile)
		{
			m_DIB[0].ShowTiled( pUseDC, m_ptWrapOffset[0].x, m_ptWrapOffset[0].y, dfZoom, iMult);
			if (ShowSubMips())
			{
				m_DIB[1].ShowTiled( pUseDC, m_ptWrapOffset[1].x, m_ptWrapOffset[1].y, dfZoom, iMult);
				m_DIB[2].ShowTiled( pUseDC, m_ptWrapOffset[2].x, m_ptWrapOffset[2].y, dfZoom, iMult);
				m_DIB[3].ShowTiled( pUseDC, m_ptWrapOffset[3].x, m_ptWrapOffset[3].y, dfZoom, iMult);
			}

			// if (pDoc->ShowGridLines())
			{
				// neal - i'm not sure i *should* draw grid line in tiled mode
			}
		}
		else
		{	
			m_DIB[0].Show( pUseDC, m_ptWrapOffset[0].x, m_ptWrapOffset[0].y, dfZoom);
			if (ShowSubMips())
			{
				m_DIB[1].Show( pUseDC, m_ptWrapOffset[1].x, m_ptWrapOffset[1].y, dfZoom);
				m_DIB[2].Show( pUseDC, m_ptWrapOffset[2].x, m_ptWrapOffset[2].y, dfZoom);
				m_DIB[3].Show( pUseDC, m_ptWrapOffset[3].x, m_ptWrapOffset[3].y, dfZoom);
			}
			if (g_bShowFineGrid || g_bShowCourseGrid)
			{
				DrawGrid( pUseDC, rClient, ptScrollPos);
			}
		}

		if (pDoc->HasSelection() && ! pDC->IsPrinting())
		{
			pDoc->GetSelection()->DrawSelection( pUseDC, dfZoom, m_ptWrapOffset[0]);
		}

		if (pUseDC != pDC)		// Neal - not print preview
		{
#ifdef _DEBUG
//			ASSERT( pDC->PatBlt( ptScrollPos.x, ptScrollPos.y, rClient.Width(), rClient.Height(), BLACKNESS));
#endif
			// Neal - BitBlt sometimes fails during print preview ???

			BOOL bSuccess = pDC->BitBlt( ptScrollPos.x, ptScrollPos.y, 
					rClient.Width(), rClient.Height(),
					pUseDC, ptScrollPos.x, ptScrollPos.y, SRCCOPY);

			if (! bSuccess)
			{
				ASSERT( pDC->IsPrinting());		// Neal - only happens during print preview ???
			}
		}
		else
		{
//			ASSERT( pDC->IsPrinting());		// Neal - this should only happen during print preview
#ifdef _DEBUG
			MessageBeep(MB_ICONQUESTION);
#endif
		}

		HICON hIcon		= NULL;	
		HICON hPrevIcon	= NULL;

		if (pDoc)
		{
			int iType = pDoc->GetGameType();

			// neal - we don't want to set the icon on *every* paint

			if (iType != pDoc->m_LastGameType)
			{
				pDoc->m_LastGameType = iType;

				int iIcon = -1;

				switch (iType)
				{				
				case FILE_TYPE_QUAKE2:
					iIcon = 0;
					break;

				case FILE_TYPE_HALF_LIFE:
					iIcon = (pDoc->HasPackageDoc()) ? 1 : 2;
					break;

				case FILE_TYPE_QUAKE1:
					iIcon = (pDoc->HasPackageDoc()) ? 3 : 4;
					break;

				case FILE_TYPE_SIN:
					iIcon = 5;
					break;

				case FILE_TYPE_HERETIC2:
					iIcon = 6;
					break;

				case FILE_TYPE_TEX:
					iIcon = 7;
					break;

				case FILE_TYPE_TGA:
				case FILE_TYPE_PCX:
				case FILE_TYPE_BMP:
				case FILE_TYPE_PNG:
				case FILE_TYPE_JPG:				
					iIcon = 0;
					break;

				default:
					ASSERT (false);
					break;
				}
				if (iIcon != -1)
					hIcon = m_imgList.ExtractIcon( iIcon);

				if (hIcon)
				{
					CWnd* pParent = GetParent();
					if (pParent)
					{
						hPrevIcon = (HICON) pParent->SendMessage( WM_SETICON, 
								(WPARAM )ICON_SMALL, (LPARAM )hIcon);

						// Free the previous icon resource
						if (hPrevIcon) 
						{
							DestroyIcon(hPrevIcon);
						}
					}
				}
				else
				{
					TRACE ("Unable to create the icon!\n");
				}
			}
		}
	}
	CATCH_ALL( e)
	{
		MessageBeep( MB_ICONEXCLAMATION);
		TRACE0( "CWallyView::OnDraw - exception was thrown\n");
	}
	END_CATCH_ALL

	VERIFY(MemDC.DeleteDC());

	if (pUseDC != pDC)
		VERIFY(Bmp.DeleteObject());
#ifdef _DEBUG
	else
		ASSERT(! Bmp.DeleteObject());
#endif

	if (! pDC->IsPrinting())
		VERIFY(BmpBackground.DeleteObject());
#ifdef _DEBUG
	else
		ASSERT(! BmpBackground.DeleteObject());
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CPbcToJpgView printing

BOOL CWallyView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWallyView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// MAYBE: add extra initialization before printing
}

void CWallyView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// MAYBE: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// CWallyView diagnostics

#ifdef _DEBUG
void CWallyView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWallyView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CWallyDoc* CWallyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWallyDoc)));
	return (CWallyDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWallyView message handlers


void CWallyView::OnMouseMove( UINT nFlags, CPoint ptPos) 
{
//	TRACKMOUSEEVENT sTrackMouseEvent;
//	sTrackMouseEvent.cbSize      = sizeof( TrackMouseEvent);
//	sTrackMouseEvent.dwFlags     = TME_LEAVE;
//	sTrackMouseEvent.hwndTrack   = m_hWnd;
//	sTrackMouseEvent.dwHoverTime = HOVER_DEFAULT;
//	TrackMouseEvent( &sTrackMouseEvent);
//	// Neal - hook this up - we'd like to get WM_MOUSELEAVE msgs
	// Neal - Oops - not in Win95 - DAMN!

	// Neal - improves spray response (commented out next IF)

//	if ((g_iCurrentTool != EDIT_MODE_SPRAY) ||
//				(! (m_bLeftButtonTracking || m_bRightButtonTracking)))
	{
		static CPoint ptLast( -10000, -10000);
		CPoint        ptTest( ptPos);
		GetTrueImageCoordinates( &ptTest);

		// neal - don't make duplicate calls when zoomed
		if (ptTest != ptLast)
		{
			ptLast = ptTest;
			g_Tool.UseTool( this, nFlags, ptPos, MOUSE_MOVE);
		}
	}

	// catch missed constraining
	if (g_bConstrained)
	{
		if (! (nFlags & MK_SHIFT))
		{
			SetFocus();
			OnKeyUp( VK_SHIFT, 1, 0);
		}
	}
	else
	{
		if (nFlags & MK_SHIFT)
		{
			SetFocus();
			OnKeyDown( VK_SHIFT, 1, 0);
		}
	}

	CScrollView::OnMouseMove( nFlags, ptPos);
}

/////////////////////////////////////////////////////////////////////////////
// Left mouse button message handlers

void CWallyView::OnLButtonDown( UINT nFlags, CPoint ptPos)
{	
	// neal - only allow one button down at a time
	if (! m_bRightButtonTracking)
	{
		m_bLeftButtonTracking = TRUE;
		SetCapture();

		g_Tool.UseTool( this, nFlags, ptPos, MOUSE_BUTTON_DOWN);
	}

	CScrollView::OnLButtonDown( nFlags, ptPos);

}


void CWallyView::OnLButtonUp( UINT nFlags, CPoint ptPos)
{
	if (m_bLeftButtonTracking)
	{
		ReleaseCapture();

		g_Tool.UseTool( this, nFlags, ptPos, MOUSE_BUTTON_UP);

		m_bLeftButtonTracking = FALSE;
	}

	CScrollView::OnLButtonUp( nFlags, ptPos);

}

void CWallyView::OnLButtonDblClk(UINT nFlags, CPoint ptPos) 
{
	// neal - TODO: dbl click selection tool - select all

	OnLButtonDown( nFlags, ptPos);
}

/////////////////////////////////////////////////////////////////////////////
// Middle mouse button message handlers

/////////////////////////////////////////////////////////////////////////////
// Name:	OnMButtonDblClk
// Action:	Zoom to 1:1 (100%)
/////////////////////////////////////////////////////////////////////////////
void CWallyView::OnMButtonDblClk( UINT nFlags, CPoint ptPos)
{
	m_iZoomValue = 1;
	SetZoom( TRUE);

	SetScrollBars( FALSE);
	InvalidateRect( NULL, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// Right mouse button message handlers

void CWallyView::OnRButtonDown(UINT nFlags, CPoint ptPos)
{
	// neal - only allow one button down at a time
	if (! m_bLeftButtonTracking)
	{
		m_bRightButtonTracking = TRUE;
		SetCapture();

		g_Tool.UseTool( this, nFlags, ptPos, MOUSE_BUTTON_DOWN);
	}

	CScrollView::OnRButtonDown( nFlags, ptPos);	

}

void CWallyView::OnRButtonUp(UINT nFlags, CPoint ptPos)
{
	if (m_bRightButtonTracking)
	{
		ReleaseCapture();

		g_Tool.UseTool( this, nFlags, ptPos, MOUSE_BUTTON_UP);

		m_bRightButtonTracking = FALSE;
	}

	CScrollView::OnRButtonUp( nFlags, ptPos);

}

void CWallyView::OnRButtonDblClk(UINT nFlags, CPoint ptPos)
{
	OnRButtonDown( nFlags, ptPos);
}

BOOL CWallyView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CWallyDoc* pDoc = GetDocument();

	if (nHitTest == HTCLIENT)
	{		
		int w = pDoc->Width();
		int h = pDoc->Height();
		char szText[100];
		char szMsg[40];

		// doc title
//		m_wndStatusBar.SetPaneText(0, pDoc->GetTitle(), true);
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), pDoc->GetTitle(), true);

		// coords, etc.
		g_Tool.ShowToolInfo( this, m_CurrentPosition);

		// doc width and height
//		sprintf (sbText, "Size:%3d x %3d", w, h);

		int iBits = pDoc->GetOriginalBitDepth();

		switch (iBits)
		{
		case 8:
			strcpy_s( szMsg, sizeof(szMsg), "%d x %d, 256 color");
			break;
		case 24:
		case 32:
			sprintf_s(szMsg, sizeof(szMsg), "%%d x %%d, %d-bit", iBits);
			break;
		default:
			strcpy_s(szMsg, sizeof(szMsg), "%d x %d, <unknown bit depth>");
			break;
		}

		sprintf_s (szText, sizeof(szText), szMsg, w, h);
		m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), szText, true);

		// current zoom value
//		if (m_iZoomValue >= 1)
//			sprintf (sbText, "Zoom:%2d:1", m_iZoomValue);
//		else
//			sprintf (sbText, "Zoom:1:%d", -m_iZoomValue);

			if (m_iZoomValue >= 1)
				sprintf_s( szText, sizeof(szText), "Zoom: %d%%", (int )(m_dfZoom * 100.0));
			else
				sprintf_s( szText, sizeof(szText), "Zoom: %1.3f%%", m_dfZoom * 100.0);

		m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), szText, true);

		// neal - if ctrl is down, we're an eyedropper
		// (unless the tool is a effect, in which case "apply more" instead

		int iEditMode = g_iCurrentTool;

		if (IsKeyPressed( VK_CONTROL))
		{
			if (IsDrawingTool( g_iCurrentTool) || (iEditMode == EDIT_MODE_SELECTION))
			{
				iEditMode = EDIT_MODE_EYEDROPPER;

				// neal - TODO: switch darken/lighten, blend/sharpen
				// on CTRL modifier key

				g_Tool.ClearBigCursor( this);
//UpdateWindow();
				InvalidateRect( NULL, FALSE);
UpdateWindow();
			}
		}

		switch (iEditMode)
		{
		case EDIT_MODE_ZOOM:
            ::SetCursor(AfxGetApp()->LoadCursor( IDC_ZOOM_CURSOR));
            return TRUE;
			break;

		case EDIT_MODE_EYEDROPPER:
			::SetCursor(AfxGetApp()->LoadCursor( IDC_DROPPER_CURSOR));
			return TRUE;
			break;

		default:
			if (IsDrawingTool( iEditMode))
			{
				::SetCursor(AfxGetApp()->LoadCursor( IDC_PAINT_CURSOR));
				return TRUE;
			}
			else
			{
				::SetCursor( ::LoadCursor( NULL, IDC_ARROW));
				return TRUE;
			}

			break;
		}
	}

	else	          
	{
//		m_wndStatusBar.SetPaneText(0, pDoc->GetTitle(),true);
//		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), "", true);
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), pDoc->GetTitle(),true);
		m_wndStatusBar.SetPaneText(0, "", true);
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), "", true);
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), "", true);
		return CScrollView::OnSetCursor(pWnd, nHitTest, message);
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);

}

void CWallyView::OnTpAnimate() 
{
	// TODO: Add your command handler code here
	
}


void CWallyView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (pSender != this)
	{
		switch (lHint)
		{
		case HINT_UPDATE_STANDARD:	// standard update
			SetScrollBars( FALSE);
			break;

		case HINT_UPDATE_TOOL_DIBS:		// needs to update DIBs, but only tool's dirty area
		case HINT_UPDATE_DIBS:			// needs to update DIBs
		case HINT_UPDATE_PAL:			// update DIBs and force palette update
			{
				CWallyDoc* pDoc         = GetDocument();
				BOOL       bSizeChanged = FALSE;
				BYTE       byPackedPal[3*256];

				if (pDoc->GetPalette()->GetNumColors() > 0)
				{
					pDoc->GetPalette( byPackedPal, 256);
					pDoc->CopyLayerToMip();

					for (int j = 0; j < 4; j++)
					{			
						int iWidth  = pDoc->MipArray[j].width;
						int iHeight = pDoc->MipArray[j].height;

						bSizeChanged = ((m_DIB[j].GetImageWidth() != iWidth) ||
										(m_DIB[j].GetImageHeight() != iHeight));

						// Reset the DIBSection, as the width/height may have changed with
						// an undo feature
						if (m_DIB[j].IsInit())
						{
							if (bSizeChanged)
							{							
								m_DIB[j].SetWidth( iWidth);
								m_DIB[j].SetHeight( iHeight);
								m_DIB[j].RebuildDibSection( byPackedPal);
							}
							else if (lHint == HINT_UPDATE_PAL)	// Neal - force rebuild of palette
							{
								m_DIB[j].RebuildDibSection( byPackedPal);
							}
						}

						//m_DIB[j].SetRawBits( pDoc->GetBits(j));
						//SetRawDibBits( j, NULL, (HINT_UPDATE_TOOL_DIBS == lHint));
						SetRawDibBits( j, (lHint == HINT_UPDATE_PAL) ? byPackedPal : NULL, (HINT_UPDATE_TOOL_DIBS == lHint));
					} 	

					if (bSizeChanged)
					{
						SetScrollBars( FALSE);
					}
					else
					{
						InvalidateRect( NULL, FALSE);
					}
				}
			}
			break;

		case HINT_UPDATE_IMAGE_SIZE:	// show/hide sub-mips
			m_iZoomValue = 0;			// neal - auto-zoom to fit
			SetScrollBars( FALSE);
			UpdateColorPalette();
			break;

		default:
			ASSERT( FALSE);
			break;
		}
	}
}

void CWallyView::OnInitialUpdate() 
{
	m_bInitialized = TRUE;
	UpdateDIBs();	
	SetScrollBars(TRUE);	
	CScrollView::OnInitialUpdate();
	g_Tool.UpdateConstrainPaneText();

	SetTimer( TIMER_WV_ANIMATE_SELECTION, 100, NULL);
}


BOOL CWallyView::OnEraseBkgnd( CDC* pDC)
{
	// nw - moved to OnDraw (less flicker)

	m_bNeedToEraseBkgnd = TRUE;
	return TRUE;					// ***** Neal - which is correct?  does it matter?
//	return FALSE;
}

void CWallyView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CScrollView::OnShowWindow(bShow, nStatus);				
}

void CWallyView::SetScrollBars( BOOL bInitialUpdate, BOOL bUseLockWindowUpdate)
{
	// neal - This new code keeps the scroll bars
	// from covering the image window, unless neccesary
	// (it was sizing poorly when scroll bars were present)

	m_bSettingScrollBars = TRUE;
	CFrameWnd* pWndMain  = NULL;

	CWnd* pParent = GetParent();
	if (pParent)
	{
		CRect rRect, rParent;
		pParent->GetWindowRect( &rParent);

		pWndMain = GetTopLevelFrame();
//		CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();

		if (pWndMain)
		{
			// prevent flashing during resizing
			if ((! bInitialUpdate) && bUseLockWindowUpdate)
			{
				pWndMain->LockWindowUpdate();
				//SetRedraw( FALSE);
			}

			BOOL bSuccess = pWndMain->NegotiateBorderSpace( CFrameWnd::borderGet, rRect);
			pWndMain->ClientToScreen( &rRect);

			// stretch the window to the frame's full size
			pParent->SetWindowPos( NULL, 0, 0, rRect.right  - rParent.left, 
				rRect.bottom - rParent.top, SWP_NOMOVE | SWP_NOZORDER);
		}
		GetClientRect( &rRect);
		GetDocSize( &rRect);
	}
	else
		GetDocSize( NULL);

	SetScrollSizes( MM_TEXT, m_DocSize);

	// trim off empty space
	if (! pParent->IsZoomed())
		ResizeParentToFit( TRUE);
	else
		Invalidate();

	if (pWndMain)
	{
		if ((! bInitialUpdate) && bUseLockWindowUpdate)
		{
			pWndMain->UnlockWindowUpdate();
			//SetRedraw( TRUE);
			//InvalidateRect( NULL, FALSE);
		}

		pWndMain->UpdateWindow();
	}
	m_bSettingScrollBars = FALSE;
}

// set zoom factor
void CWallyView::SetZoom( BOOL bPlaySound)
{
	// must enforce min/max zoom limits

	int iMult    = (m_bTile) ? 3 : 1;	
	int iMaxZoom = MAX_ZOOM;
	int iMinZoom = MIN_ZOOM;
	int iWidth   = m_DIB[0].GetImageWidth();
	int iHeight  = m_DIB[0].GetImageHeight();

	iWidth  = max( 1, iWidth);
	iHeight = max( 1, iHeight);

	// neal - fixed tiled scrollbar limit bug
	iMaxZoom = min( iMaxZoom, 32767 / (iMult * iWidth));
	iMaxZoom = min( iMaxZoom, 32767 / (iMult * iHeight));

	if (m_iZoomValue > iMaxZoom)
		m_iZoomValue = iMaxZoom;
	else if (m_iZoomValue < iMinZoom)
		m_iZoomValue = iMinZoom;

	//////////////////////////////
	// now set the zoom amount //
	////////////////////////////

	if (m_iZoomValue >= 2)
	{
		m_dfZoom = m_iZoomValue;
	}
	else if (m_iZoomValue <= -2)
	{
		m_dfZoom = 1.0 / (-m_iZoomValue);
	}
	else	// 100%
	{
		ASSERT( m_iZoomValue == 1);

		m_iZoomValue  = 1;
		m_dfZoom      = 1.0;

		// play an audiable clue - you're at 100 %

		if (bPlaySound)
			PlaySound( "MenuPopup", NULL, SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
	}

	// update status bar

	char szZoom[20];

//	if (m_iZoomValue >= 1)
//		sprintf( szZoom, "Zoom:%2d:1", m_iZoomValue);
//	else
//		sprintf( szZoom, "Zoom:1:%d", -m_iZoomValue);

	if (m_iZoomValue >= 1)
		sprintf_s( szZoom, sizeof(szZoom), "Zoom: %d%%", (int )(m_dfZoom * 100.0));
	else
		sprintf_s(szZoom, sizeof(szZoom), "Zoom: %1.3f%%", m_dfZoom * 100.0);

	m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), szZoom, true);
}

void CWallyView::SetZoomFactor( BOOL bZoomIn)
{
	if (bZoomIn)	// zoom in
	{
		if (m_iZoomValue == -2)
			m_iZoomValue = 1;
		else
			m_iZoomValue += 1;
	}
	else			// zoom out
	{
		if (m_iZoomValue == 1)
			m_iZoomValue = -2;
		else
			m_iZoomValue -= 1;
	}

	SetZoom( TRUE);
}

SIZE CWallyView::CalcDocSize( LPRECT lpRect, double dfZoom, int iGap)
{
	SIZE szRet;
	int  iMult = (m_bTile) ? 3 : 1;
	
	szRet.cx = m_DIB[0].GetImageWidth()  * iMult;
	szRet.cy = m_DIB[0].GetImageHeight() * iMult;

	int iModWidth  = (int )(szRet.cx * dfZoom);
	int iModHeight = (int )(szRet.cy * dfZoom);

//	if (ShowSubMips() && IsValidImageSize( m_DIB[0].GetImageWidth(), 
//				m_DIB[0].GetImageHeight(), FORCE_X_16))
	if (ShowSubMips())
	{
		int iMax = max( iModHeight/2, iModHeight/4 + iGap + iModHeight/8);
		int w    = iModWidth;
		int h    = iModHeight + iGap + iMax;

		szRet.cx = max( iModWidth, iModWidth/2 + iGap + iModWidth/4 + iGap + iModWidth/8);
		szRet.cy = h;

		// neal - allow sub-mips to position on right as well as bottom

		if (lpRect)
		{
			iMax = max( iModWidth/2, iModWidth/4 + iGap + iModWidth/8);
			w    = iModWidth + iGap + iMax;

			if (w <= lpRect->right - lpRect->left)
			{
				szRet.cx = w;
				szRet.cy = max( iModHeight, iModHeight/2 + iGap + iModHeight/4);
			}
		}
	}
	else	// don't draw sub-mips
	{
		szRet.cx = iModWidth;
		szRet.cy = iModHeight;
	}

	return szRet;
}

void CWallyView::GetDocSize( LPRECT lpRect)
{
	// neal - new auto-zoom code

	int iMult = (m_bTile) ? 3 : 1;	

	int iGap = 8;

	if (m_bUnknownType)
	{
		// Find out if we have sub-mips
		UINT iNumMips = GetDocument()->GetNumMips();

		if (iNumMips > 0)
		{
			m_bUnknownType = FALSE;
		
			SetHasSubMips (iNumMips > 1);			
		}	
	}

	CWallyDoc* pDoc = GetDocument();

	// now calc zoom factor...
	// (assumes window is preset to maximum allowable size)

	if (pDoc && m_iZoomValue == 0)
	{
		CRect rClient;
		GetClientRect( rClient);

		double dfZoomValueX = rClient.Width() - iGap;

		//int iWidth = m_DIB[0].GetImageWidth();
		// Neal - I think the above code might be able to div-by-zero!
		//
		int iWidth = pDoc->GetWidth();
		ASSERT( iWidth > 0);
		iWidth = max( 1, iWidth);

		if (ShowSubMips())
			dfZoomValueX /= (iWidth + iWidth/2 /*half size sub-mip*/ ) * iMult;
		else
			dfZoomValueX /= iWidth * iMult;

		// neal - fix tall skinny textures zoom bug

		double dfZoomValueY = rClient.Height() - iGap;
//		dfZoomValueY /= (m_DIB[0].GetImageHeight() + m_DIB[1].m_Height) * iMult;
//		dfZoomValueY /= m_DIB[0].GetImageHeight() * iMult;
		//
		// Neal - I think the above code might be able to div-by-zero!
		//
		int iHeight = pDoc->GetHeight();
		ASSERT( iHeight > 0);
		iHeight = max( 1, iHeight);
		dfZoomValueY /= iHeight * iMult;

		int iZoomValueX = (int )dfZoomValueX;
		int iZoomValueY = (int )dfZoomValueY;

		// Neal - need to zoom out?

		if (iZoomValueX == 0)
			iZoomValueX = -(int )ceil(1.0 / dfZoomValueX);

		if (iZoomValueY == 0)
			iZoomValueY = -(int )ceil(1.0 / dfZoomValueY);

		m_iZoomValue = min( iZoomValueX, iZoomValueY);

		int iMaxZoom = MAX_ZOOM;
		int iMinZoom = MIN_ZOOM;

		// neal - fixed tiled scrollbar limit bug
		iMaxZoom = min( iMaxZoom, 32767 / (iMult * iWidth));
		iMaxZoom = min( iMaxZoom, 32767 / (iMult * iHeight));

		m_iZoomValue = min( m_iZoomValue, iMaxZoom);
		m_iZoomValue = max( m_iZoomValue, iMinZoom);

		if ((m_iZoomValue >= -1) && (m_iZoomValue < 1))
			m_iZoomValue = 1;

		SetZoom( FALSE);
	}
/*
	m_TrueDocSize.cx = m_DIB[0].GetImageWidth()  * iMult;
	m_TrueDocSize.cy = m_DIB[0].GetImageHeight() * iMult;

	int iModWidth  = (int )(m_TrueDocSize.cx * m_dfZoom);
	int iModHeight = (int )(m_TrueDocSize.cy * m_dfZoom);

//	if (ShowSubMips() && IsValidImageSize( m_DIB[0].GetImageWidth(), 
//				m_DIB[0].GetImageHeight(), FORCE_X_16))
	if (ShowSubMips())
	{
		int iMax = max( iModHeight/2, iModHeight/4 + iGap + iModHeight/8);
		int w    = iModWidth;
		int h    = iModHeight + iGap + iMax;

		m_DocSize.cx = max( iModWidth, iModWidth/2 + iGap + iModWidth/4 + iGap + iModWidth/8);
		m_DocSize.cy = h;

		// neal - allow sub-mips to position on right as well as bottom

		if (lpRect)
		{
			iMax = max( iModWidth/2, iModWidth/4 + iGap + iModWidth/8);
			w    = iModWidth + iGap + iMax;

			if (w <= lpRect->right - lpRect->left)
			{
				m_DocSize.cx = w;
				m_DocSize.cy = max( iModHeight, iModHeight/2 + iGap + iModHeight/4);
			}
		}
	}
	else	// don't draw sub-mips
	{
		m_DocSize.cx = iModWidth;
		m_DocSize.cy = iModHeight;
	}
*/
	m_DocSize = CalcDocSize( lpRect, m_dfZoom, iGap);
}

void CWallyView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{		
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWallyView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWallyView::OnContextMenu(CWnd*, CPoint point)
{

	if (g_iCurrentTool == EDIT_MODE_SELECTION)
	{

		{
			if (point.x == -1 && point.y == -1)
			{
				//keystroke invocation
				CRect rect;
				GetClientRect(rect);
				ClientToScreen(rect);
	
				point = rect.TopLeft();
				point.Offset(5, 5);
			}

			CMenu menu;
			VERIFY(menu.LoadMenu(CG_IDR_POPUP_WALLY_VIEW));

			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			CWnd* pWndPopupOwner = this;

			while (pWndPopupOwner->GetStyle() & WS_CHILD)
				pWndPopupOwner = pWndPopupOwner->GetParent();

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
				pWndPopupOwner);
		}
	}
}

void CWallyView::OnUpdateEditCopyCut( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( IsSelected());
}

void CWallyView::OnUpdateEditPaste( CCmdUI* pCmdUI)
{
	
	pCmdUI->Enable( ::IsClipboardFormatAvailable( CF_DIB));
	//|| ::IsClipboardFormatAvailable(m_nClipboardFormat));
}

void CWallyView::OnEditCopy()
{
	CWallyDoc* pDoc = GetDocument();

	if (pDoc && pDoc->HasSelection())
	{
		CSelection*    pSel = pDoc->GetSelection();
		CWallyPalette* pPal = pDoc->GetPalette();

		if (pSel && pPal)
		{
			CLayer* pLayer = pSel->GetLayer();
			pLayer->m_Pal  = *pPal;

			if (pLayer && (! pLayer->HasData()))
			{
				// neal - TODO: move data storage from MIP to 24 bit CLayer

				int iWidth  = pDoc->Width();
				int iHeight = pDoc->Height();

				pLayer->ReCreate( pDoc, iWidth, iHeight, 24);

				for (int j = 0; j < iHeight; j++)
				{
					for (int i = 0; i < iWidth; i++)
					{
//						int iIndex = j*iWidth + i;
						//int iColor = pDoc->GetMipPixel( iIndex, 0);
						//pSel->m_pLayer->m_pIRGB_Data[iIndex] =
						//		IRGB( iColor, pPal->GetR( iColor),
						//		pPal->GetG( iColor), pPal->GetB( iColor));

						COLOR_IRGB irgbColor = pDoc->GetPixel( i, j);
//						pLayer->m_pIRGB_Data[iIndex] = irgbColor;
						pLayer->SetPixel( this, i, j, irgbColor);
					}
				}

				if (pLayer->HasData())
				{
					pLayer->WriteToClipboard( this);
					pLayer->FreeMem();
				}
			}
			else
			{
				pLayer->WriteToClipboard( this);
			}
		}
		else
		{
			ASSERT( FALSE);
		}
	}
	else
	{
		m_DIB[0].WriteToClipboard( this);
	}
}

void CWallyView::FreeEffectsData()
{
	// Clear out the effects block

	GetDocument()->FreeEffectsData();
}

void CWallyView::OnEditCut()
{
	FreeEffectsData();
	
	OnEditCopy();
	OnEditClear();
}

void CWallyView::OnEditPaste()
{
	BeginWaitCursor();
	CClipboardDIB ClipboardDIB;

	if (ClipboardDIB.InitFromClipboard( this))
	{
		BYTE* pbyClipboardData;
		int j, r, g, b;
		WORD BitsPerPixel;
		BYTE ClipboardPalette[1024];		
		// RGBQUAD QuadPalette[256];
		int iColor   = -9999;
		int iWidth   = ClipboardDIB.GetWidth();
		int iHeight  = ClipboardDIB.GetHeight();
		int iSize    = iWidth * iHeight;

		BitsPerPixel = ClipboardDIB.GetColorDepth();
		
		CWallyDoc* pDoc = GetDocument();
		pDoc->m_Undo.SaveUndoState( pDoc, "Paste");

		FreeEffectsData();

		// MARKER

		CRect rClient;
		GetClientRect( &rClient);
		GetDocSize( &rClient);

		int iMipNumber = -1;

		// Neal - bugfix - needs to start at 1, not 0, so prompt doesn't come up all the time
		//
		for (j = 1; j < 4; j++)
		{
			if ((iWidth == pDoc->GetWidth(j)) && (iHeight == pDoc->GetHeight(j)))
			{
				iMipNumber = j;
			}
		}
		BOOL bPasteSubMip = FALSE;
		
		if ((! pDoc->HasSelection()) && iMipNumber != -1)			
		{
			if (IDNO == AfxMessageBox( "Would you like to paste into the main texture (YES) or into\n"
									   "the sub-mip whose size matches the data on the clipboard (NO)?", MB_YESNO))
			{
				bPasteSubMip = TRUE;
			}
		}

		if (! bPasteSubMip)
		{
			if (! pDoc->HasSelection())
			{
//				pDoc->m_pSelection = &pDoc->m_Cutout;
//				pDoc->m_pSelection->SetBoundsRect( 0, 0, iWidth, iHeight);

				// Neal - BUGFIX - paste w/ no selection was failing
				CSelection* pSel = pDoc->GetCutout();

				if (pSel)
				{
					CLayer* pLayer     = pSel->GetLayer();
					CLayer* pBaseLayer = pDoc->GetBase()->GetLayer();

					ASSERT( (pLayer != NULL) && (pBaseLayer != NULL));

					pSel->SetBoundsRect( 0, 0, iWidth, iHeight);
					pLayer->FreeMem();
					pLayer->DupLayer( this, pBaseLayer);

					// Neal - TODO: set fade based on user setting
					//
					//pLayer->m_iFadeAmount = 0;

					pDoc->m_pSelection = pSel;
				}
			}
		}

		if (pDoc->HasSelection())
		{
			iMipNumber = 0;
		}

		// Point to the clipboard bits
		pbyClipboardData = (BYTE *)ClipboardDIB;

		int iTestSize = sizeof( COLOR_IRGB);

		BYTE*       pbyTemp8Bit = (BYTE* )malloc( iSize);
		COLOR_IRGB* pTemp24Bit  = NULL;

		ClipboardDIB.GetDIBPalette( ClipboardPalette);
		CWallyPalette *pPal = pDoc->GetPalette();		

		if (pbyTemp8Bit)
		{
			if (BitsPerPixel == 8)
			{		
				switch (g_iPaletteConversion)
				{
				case PALETTE_CONVERT_MAINTAIN:				

					switch (pDoc->GetGameType())					
					{
					case FILE_TYPE_QUAKE1:
					case FILE_TYPE_QUAKE2:
						// Just jam it in, no conversion necessary
						memcpy( pbyTemp8Bit, pbyClipboardData, iSize);
						break;

					case FILE_TYPE_TGA:
					case FILE_TYPE_BMP:
					case FILE_TYPE_PCX:
					case FILE_TYPE_PNG:
						// Neal - TODO: check for 24 bit (???)

						// drop thru

					case FILE_TYPE_SIN:
					case FILE_TYPE_HERETIC2:
					case FILE_TYPE_HALF_LIFE:

						memcpy( pbyTemp8Bit, pbyClipboardData, iSize);

						/////////////////////////////////
						// Neal - just a quick bugfix //
						///////////////////////////////
						//
						// when we're 24 bit, perhaps we won't have to prompt
						// the problem is, they want the new palette for a paste OVER
						// and they want to keep the old palette on a paste small area ON

						if ((iWidth >= pDoc->GetWidth()) && (iHeight >= pDoc->GetHeight()))
						{
							if (IDYES == MessageBox( _T("Do you wish to copy the palette from the clipboard as well?"),
										_T("Wally - Clipboard Query"), MB_YESNO))
							{
								for (j = 0; j < 256; j++)
								{
									b = ClipboardPalette[j * 4];
									g = ClipboardPalette[j * 4 + 1];
									r = ClipboardPalette[j * 4 + 2];

									pPal->SetRGB( j, r, g, b);
								}
							}
						}
						break;

					default:	
						ASSERT (FALSE);	
						break;

					}
					break;
					
				case PALETTE_CONVERT_NEAREST:
					{
/*
						switch (pDoc->GetGameType())
						{
						case FILE_TYPE_QUAKE1:
						case FILE_TYPE_QUAKE2:
*/
							pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));

							for (j = 0; j < iSize; j++) 
							{
								b = ClipboardPalette[pbyClipboardData[j] * 4 + 0];
								g = ClipboardPalette[pbyClipboardData[j] * 4 + 1];
								r = ClipboardPalette[pbyClipboardData[j] * 4 + 2];
						
								if ((r != 159) || (g != 91) || (b != 83))
									pTemp24Bit[j] = IRGB( 0, r, g, b);
								else
									pTemp24Bit[j] = IRGB( 255, r, g, b);
							}
							pPal->Convert24BitTo256Color( pTemp24Bit, 
									pbyTemp8Bit, iWidth, iHeight, 0, GetDitherType(), FALSE);
							break;
/*
						case FILE_TYPE_TGA:
						case FILE_TYPE_BMP:
						case FILE_TYPE_PCX:
						case FILE_TYPE_PNG:
							// Neal - TODO: check for 24 bit (???)

							// drop thru

						case FILE_TYPE_SIN:
						case FILE_TYPE_HERETIC2:
						case FILE_TYPE_HALF_LIFE:

							for (j = 0; j < 256; j++)
							{
								b = ClipboardPalette[j * 4];
								g = ClipboardPalette[j * 4 + 1];
								r = ClipboardPalette[j * 4 + 2];

								pPal->SetRGB( j, r, g, b);
							}

							memcpy( pbyTemp8Bit, pbyClipboardData, iSize);

							UpdateColorPalette();
							pDoc->UpdateAllViews( NULL, HINT_UPDATE_PAL);
							break;

						default:
							ASSERT (false);
							break;
						}
*/
					}
					break;

				default:
					// Unknown palette conversion option
					ASSERT (false);
					break;
				}
			}		
			else if (BitsPerPixel == 24)
			{			
				pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));

				CColorOptimizer ColorOpt;
				for (j = 0; j < iSize; j++)
				{
					b = pbyClipboardData[j * 3];
					g = pbyClipboardData[j * 3 + 1];
					r = pbyClipboardData[j * 3 + 2];

					// Neal - I think we should drop the old Q2 invisibility stuff
					//
					//if ((r != 159) || (g != 91) || (b != 83))
						pTemp24Bit[j] = IRGB( 0, r, g, b);
					//else
					//	pTemp24Bit[j] = IRGB( 255, r, g, b);
				}
				if (pDoc->GetGameType() == FILE_TYPE_HALF_LIFE)
				{
					BYTE byPalette[256*3];

					// neal - Half-Life textures with transparency should really use 255, not 256
					ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 256, TRUE);

					pDoc->SetPalette( byPalette, 256);
					UpdateColorPalette();
					pDoc->UpdateAllViews( NULL, HINT_UPDATE_PAL);
				}
/*				// Neal - no need to do this inless we're pasting into an 8 bit image
				if (pbyTemp8Bit && pTemp24Bit && (pDoc->GetNumBits() == 8))
				{
					pDoc->Convert24BitTo256Color( pTemp24Bit, pbyTemp8Bit, 
							iWidth, iHeight, 0, GetDitherType(), FALSE);

					// Neal - free it now, so we'll us 8-bit later
					if (pTemp24Bit)
					{
						free( pTemp24Bit);
						pTemp24Bit = NULL;
					}
				}
*/			}
			else
			{
				ASSERT( FALSE);
			}
		}
		else
		{
			AfxMessageBox( IDS_OUT_OF_MEM);
		}

		int iNumBits = pDoc->GetNumBits();

//		if ( (pbyTemp8Bit && (iNumBits == 8)) ||
//				(pTemp24Bit && (iNumBits == 24)) )
		if (pbyTemp8Bit || pTemp24Bit)
		{
			if (pDoc->HasSelection())
			{
				CSelection* pSel = pDoc->GetSelection();
				ASSERT( pSel);

				int   iDocWidth  = pDoc->Width();
				int   iDocHeight = pDoc->Height();
				CRect Rect       = pSel->GetBoundsRect();

				Rect.right  = Rect.left + iWidth;
				Rect.bottom = Rect.top  + iHeight;

				pSel->SetBoundsRect( Rect);

				// neal - TODO: 24 bit modifications

//				int iTransparentColor = (g_bPasteInvisibleBackground) ? g_iColorIndexRight : -1;
				int iTransparentColor = (g_bPasteInvisibleBackground) ? GetRValue( g_irgbColorRight) : -1;
				int iAmount           = ToolAmountToPercent( EDIT_MODE_SELECTION);

				for (int j = 0; j < Rect.Height(); j++)
				{
					for (int i = 0; i < Rect.Width(); i++)
					{
						if (pTemp24Bit)
						{
							COLOR_IRGB irgb = pTemp24Bit[j*iWidth + i];
							r = GetRValue( irgb);
							g = GetGValue( irgb);
							b = GetBValue( irgb);

							if (iAmount == 100)
								iColor = pPal->FindNearestColor( r, g, b, FALSE);
						}
						else
						{
							iColor = pbyTemp8Bit[j*iWidth + i];
							b      = ClipboardPalette[iColor * 4];
							g      = ClipboardPalette[iColor * 4 + 1];
							r      = ClipboardPalette[iColor * 4 + 2];
						}

						if (iAmount < 100)
						{
//							int iTexColor = pDoc->GetWrappedMipPixel( 
//										i+Rect.left, j+Rect.top, 0);
//
//							r = ((r * iAmount) + (pPal->GetR( iTexColor) * (100-iAmount))) / 100;
//							g = ((g * iAmount) + (pPal->GetG( iTexColor) * (100-iAmount))) / 100;
//							b = ((b * iAmount) + (pPal->GetB( iTexColor) * (100-iAmount))) / 100;

							COLOR_IRGB irgbTexColor = pDoc->GetWrappedPixel( 
									i+Rect.left, j+Rect.top);

							r = ((r * iAmount) + (GetRValue( irgbTexColor) * (100-iAmount))) / 100;
							g = ((g * iAmount) + (GetGValue( irgbTexColor) * (100-iAmount))) / 100;
							b = ((b * iAmount) + (GetBValue( irgbTexColor) * (100-iAmount))) / 100;

							iColor = pPal->FindNearestColor( r, g, b, 
									(iColor >= 255 - pPal->GetNumFullBrights()));
						}

						if (iColor != iTransparentColor)
						{
							if (m_bTile)
							{
//								iColor = pPal->FindNearestColor( r, g, b, 
//										(iColor >= 255 - pPal->GetNumFullBrights()));
//
//								pDoc->SetWrappedMipPixel( this,
//											i+Rect.left, j+Rect.top, iColor, 0);

//								pDoc->SetNearestColorWrappedPixel( this,
//									i+Rect.left, j+Rect.top, RGB( r, g, b), FALSE);

								pDoc->SetWrappedPixel( this,
									i+Rect.left, j+Rect.top, IRGB( iColor, r, g, b));
							}
							else if ((i < iWidth) && (j < iHeight))
							{
								//int iIndex = (j+Rect.top)*iDocWidth + i+Rect.left;

								if ((i >= 0) && (j >= 0) && 
											(i+Rect.left < iDocWidth) && 
											(j+Rect.top  < iDocHeight))
								{
//									iColor = pPal->FindNearestColor( r, g, b, 
//											(iColor >= 255 - pPal->GetNumFullBrights()));

									//pDoc->SetMipPixel( this, iIndex, iColor, 0);
									pDoc->SetPixel( this, i+Rect.left, j+Rect.top, 
											IRGB( iColor, r, g, b));
								}

								//m_DIB[MipNumber].Bits[iIndex] = 128;		// TEST ***
							}
						}
					}
				}
			}
			else
			{
				// neal - TODO: needs to use code above
				//        (so that tranparency and amount are supported)

				ASSERT( FALSE);

				m_DIB[iMipNumber].SetRawBits( pbyTemp8Bit);
			}
			pDoc->SetModifiedFlag( TRUE);

			// Only rebuild the submips if we've pasted into the first mip

			if (g_bAutoRemip && iMipNumber == 0)
			{
				pDoc->RebuildSubMips();
				pDoc->UpdateSubMipDIBs();
			}
			//Invalidate();

			pDoc->UpdateAllViews( NULL, HINT_UPDATE_DIBS);
		}
		else
		{
			ASSERT( FALSE);		// not yet implemented or out-of-mem
		}

		// Neal - this is correct, we only freed the 8-bit dest case earlier
		if (pTemp24Bit)
		{
			free( pTemp24Bit);
			pTemp24Bit = NULL;
		}
		if (pbyTemp8Bit)
		{
			free( pbyTemp8Bit);
			pbyTemp8Bit = NULL;
		}
	}  				
	EndWaitCursor();
}

BOOL CWallyView::IsSelected()
{


	// TODO: Add/Replace code to selectively enable and disable the copy 
	//       and cut menu items. Return TRUE when one or more items are
	//       selected, and return FALSE when no items are selected.

	return TRUE;
}

void CWallyView::DoCutCopyPaste(CArchive &ar, BOOL bCut)
{
	// TODO: Add/Replace code to copy the currently selected data to or
	//       from the clipboard, via the CArchive object. Note: 
	//       ar.IsStoring() set to TRUE indicates a copy or cut operation.

	CWallyDoc* pDoc = GetDocument();

//	pDoc->LoadBitmap(ar);

	if (bCut)
	{
		ASSERT(ar.IsStoring()); // no cutting on paste operation

		// TODO: Add/Replace code to delete the currently selected items.

		pDoc->DeleteContents();
	}

	// updated view and flag as modified if cut or paste operation
	if (!(ar.IsStoring() && !bCut))
	{
		pDoc->UpdateAllViews(NULL);
		pDoc->SetModifiedFlag();
	}
}

void CWallyView::OnPopupCopyAsBitmap() 
{

	{
		CSharedFile	memFile;
		CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);

		// serialize data to archive object
		DoCutCopyPaste(ar, FALSE);

		ar.Flush();

		HGLOBAL hData = memFile.Detach();
		if (OpenClipboard())
		{
			::SetClipboardData(CF_BITMAP, hData);
			CloseClipboard();
		}
		else
			AfxMessageBox(CG_IDS_CANNOT_OPEN_CLIPBOARD);
	}	
}

void CWallyView::GetTrueImageCoordinates( CPoint* ImagePoint)
{		
	if (m_iZoomValue == 0)
	{
		ASSERT( FALSE);
		GetDocSize( NULL);
	}

	ImagePoint->x += GetScrollPos( SB_HORZ);
	ImagePoint->y += GetScrollPos( SB_VERT);

//	ImagePoint->x /= iZoom;
//	ImagePoint->y /= iZoom;

	if (m_dfZoom == 0)
	{
		ASSERT( FALSE);

		m_dfZoom = 1.0;
	}

	ImagePoint->x = (int )(ImagePoint->x / m_dfZoom);
	ImagePoint->y = (int )(ImagePoint->y / m_dfZoom);
}

void CWallyView::OnTpTile() 
{
	m_iZoomValue = 0;			// neal - auto-zoom to fit

	m_bTile = ! m_bTile;

	SetScrollBars( FALSE);

	UpdateColorPalette();
}

void CWallyView::OnUpdateTpTile( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bTile);
}

void CWallyView::OnTpRemip() 
{
	CWallyDoc* pDoc = GetDocument();
	pDoc->m_Undo.SaveUndoState( pDoc, "ReMip");

	//pDoc->RebuildSubMips();
	pDoc->UpdateAllDIBs( TRUE);
	pDoc->SetModifiedFlag( TRUE);
}


void CWallyView::OnEditClear() 
{
	CWallyDoc* pDoc = GetDocument();
	pDoc->m_Undo.SaveUndoState( pDoc, "Clear");
//	pDoc->ClearMipArray( g_iColorIndexRight, 0);	
	//pDoc->ClearMipArray( GetIValue( g_irgbColorRight), 0);	
	pDoc->ClearMipArray( g_irgbColorRight, 0);	
	FreeEffectsData();

//	if (g_bAutoRemip)
//	{		
//		pDoc->RebuildSubMips();		
//		//pDoc->UpdateSubMipDIBs();
//	}
	pDoc->UpdateAllDIBs( g_bAutoRemip);	
	pDoc->SetModifiedFlag( pDoc->HasSelection());
	RebuildImageViews();
}

//////////////////////////////////////////////////////////////////////////////
//	Name:	UpdateDIBs
//	Action:	This function reads the data from the document, and fills the Bits
//			pointer for each DIB with the corresponding document submip data
//////////////////////////////////////////////////////////////////////////////
void CWallyView::UpdateDIBs()
{
	CWallyDoc* pDoc = GetDocument();

	BYTE byPackedPal[3*256];
	pDoc->GetPalette( byPackedPal, 256);

	for (int j = 0; j < 4; j++)
	{
		SetRawDibBits( j, byPackedPal, false);
	}
}

void CWallyView::OnEditUndo() 
{
	CWallyDoc* pDoc = GetDocument();

	// nw - reduce flashing
	int iWidth  = pDoc->Width();
	int iHeight = pDoc->Height();

	pDoc->m_Undo.DoUndo();
	
	// Ty - need to reset the bars in case the width/height are switched from
	//		undoing/redoing a rotate
	if ((pDoc->Width() != iWidth) || (pDoc->Height() != iHeight))
		SetScrollBars( FALSE);
	
	// cancel remip
	KillTimer( TIMER_TOOL_AUTO_REMIP);
	UpdateColorPalette();
}

void CWallyView::OnUpdateEditUndo( CCmdUI* pCmdUI) 
{
	CWallyDoc* pDoc = GetDocument();

	pCmdUI->Enable( pDoc->m_Undo.CanUndo());
	pCmdUI->SetText( pDoc->m_Undo.GetUndoString());	
}

void CWallyView::OnEditRedo() 
{
	CWallyDoc* pDoc = GetDocument();

	// nw - reduce flashing
	int iWidth  = pDoc->Width();
	int iHeight = pDoc->Height();
		
	pDoc->m_Undo.DoRedo();

	// Ty - need to reset the bars in case the width/height are switched from
	//		undoing/redoing a rotate
	if ((pDoc->Width() != iWidth) || (pDoc->Height() != iHeight))
		SetScrollBars( FALSE);
	
	// cancel remip
	KillTimer( TIMER_TOOL_AUTO_REMIP);
	UpdateColorPalette();
}

void CWallyView::OnUpdateEditRedo( CCmdUI* pCmdUI) 
{
	CWallyDoc* pDoc = GetDocument();

	pCmdUI->Enable( pDoc->m_Undo.CanRedo());
	pCmdUI->SetText( pDoc->m_Undo.GetRedoString());
	
}

void CWallyView::OnTpZoom() 
{
	g_Tool.InitTool( this, EDIT_MODE_ZOOM);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnEditZoomIn()
{
	int iZoomVal = m_iZoomValue;

	SetZoomFactor( TRUE);			// zoom IN

	if (iZoomVal != m_iZoomValue)	// did zoom amount change?
	{
		SetScrollBars( FALSE);
		InvalidateRect( NULL, FALSE);
	}
}

void CWallyView::OnEditZoomOut()
{
	int iZoomVal = m_iZoomValue;

	SetZoomFactor( FALSE);			// zoom OUT

	if (iZoomVal != m_iZoomValue)	// did zoom amount change?
	{
		SetScrollBars( FALSE);
		InvalidateRect( NULL, FALSE);
	}
}

void CWallyView::SetZoomValue(int iZoomValue)
{
	int iZoomVal = m_iZoomValue;

	if (iZoomVal != iZoomValue)
	{
		m_iZoomValue = iZoomValue;
		SetZoom( FALSE);

		SetScrollBars( FALSE);
		InvalidateRect( NULL, FALSE);
	}
}

void CWallyView::OnUpdateTpZoom( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_ZOOM);
}

void CWallyView::ShiftImage( int iDirection)
{
	CWallyDoc* pDoc = GetDocument();

	const char* szUndo = NULL;

	switch (iDirection)
	{
	case SHIFT_UP:
		szUndo = "Shift Image Up";
		break;
	case SHIFT_DOWN:
		szUndo = "Shift Image Down";
		break;
	case SHIFT_LEFT:
		szUndo = "Shift Image Left";
		break;
	case SHIFT_RIGHT:
		szUndo = "Shift Image Right";
		break;
	default:
		ASSERT( FALSE);
		MessageBeep( MB_ICONEXCLAMATION);
		return;
	}

	pDoc->m_Undo.SaveUndoState( pDoc, szUndo);

	FreeEffectsData();
	pDoc->SetModifiedFlag( TRUE);

	int iMult = 1;
	if (IsKeyPressed( VK_CONTROL))
		iMult= 16;
	else if (IsKeyPressed( VK_SHIFT))
		iMult = 8;

	//m_DIB[0].ShiftBits( iDirection, i);		// Neal - TODO - remove it
	//m_DIB[0].GetRawBits( pDoc->GetBits());
	//pDoc->CopyMipToLayer();

	int iXdir = 0;
	int iYdir = 0;
	switch (iDirection)
	{
		case (SHIFT_UP):
			iYdir = -1;
			break;
		case (SHIFT_DOWN):
			iYdir = 1;
			break;
		case (SHIFT_LEFT):
			iXdir = -1;
			break;
		case (SHIFT_RIGHT):
			iXdir = 1;
			break;
		default:
			ASSERT( FALSE);		// invalid direction
	}

	BOOL bRebuildSubMips       = g_Filter.m_bRebuildSubMips;
	g_Filter.m_bRebuildSubMips = FALSE;

	g_Filter.DoPixelFilter( pDoc->GetCurrentLayer(), FILTER_OFFSET, 
			iXdir*iMult, iYdir*iMult);

	g_Filter.m_bRebuildSubMips = bRebuildSubMips;

	SetRawDibBits( 0, NULL, false);		// Neal - just update main mip
	InvalidateRect( NULL, FALSE);

	// neal - rebuild sub-mips only when the user is done
	SetTimer( TIMER_WV_REMIP, 1000, NULL);
}


void CWallyView::OnTpSelection() 
{
	g_Tool.InitTool( this, EDIT_MODE_SELECTION);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpSelection( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_SELECTION);
}

void CWallyView::OnTpDropper() 
{
	g_Tool.InitTool( this, EDIT_MODE_EYEDROPPER);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpDropper( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_EYEDROPPER);
}

void CWallyView::OnTpPencil() 
{
	g_Tool.InitTool( this, EDIT_MODE_PENCIL);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpPencil( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_PENCIL);
}

void CWallyView::OnTpLighten() 
{
	g_Tool.InitTool( this, EDIT_MODE_LIGHTEN);

//	CWallyDoc*     pDoc = GetDocument();
//	CWallyPalette* pPal = pDoc->GetPalette();
//
//	int i, iRed, iGreen, iBlue;
//
//	// neal - initialize the palette index transform tables
//
//	double dfLightenPercent = ToolAmountToPercent( g_iCurrentTool);
//
//	if ((m_iLastTool != g_iCurrentTool) || (g_dfLightenPercent != dfLightenPercent))
//	{
//		g_dfLightenPercent = dfLightenPercent;
//
//		// a little bit lighter
//		for (i = 0; i < 256; i++)
//		{
//			iRed   = (int )((255*dfLightenPercent/100) + pPal->GetR( i) * (100-dfLightenPercent) / 100);
//			iGreen = (int )((255*dfLightenPercent/100) + pPal->GetG( i) * (100-dfLightenPercent) / 100);
//			iBlue  = (int )((255*dfLightenPercent/100) + pPal->GetB( i) * (100-dfLightenPercent) / 100);
//
//			g_byEffectsIndexLookUp[0][i] = 
//					pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//		}
//
//		dfLightenPercent += 10;
//
//		// a little bit lighter
//		for (i = 0; i < 256; i++)
//		{
//			iRed   = (int )((255*dfLightenPercent/100) + pPal->GetR( i) * (100-dfLightenPercent) / 100);
//			iGreen = (int )((255*dfLightenPercent/100) + pPal->GetG( i) * (100-dfLightenPercent) / 100);
//			iBlue  = (int )((255*dfLightenPercent/100) + pPal->GetB( i) * (100-dfLightenPercent) / 100);
//
//			g_byEffectsIndexLookUp[1][i] = 
//					pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//		}
//	}
	g_dfLightenPercent = ToolAmountToPercent( g_iCurrentTool);
	m_iLastTool        = g_iCurrentTool;
}

void CWallyView::OnUpdateTpLighten( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_LIGHTEN);
}

void CWallyView::OnTpEraser() 
{
	g_Tool.InitTool( this, EDIT_MODE_ERASER);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpEraser( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_ERASER);
}

void CWallyView::OnTpDarken() 
{
	g_Tool.InitTool( this, EDIT_MODE_DARKEN);

//	CWallyDoc*     pDoc = GetDocument();
//	CWallyPalette* pPal = pDoc->GetPalette();
//
//	int i, iRed, iGreen, iBlue;
//
//	// neal - initialize the palette index transform tables
//
//	int iDarkenPercent = ToolAmountToPercent( g_iCurrentTool);
//
//	if ((m_iLastTool != g_iCurrentTool) || (g_iDarkenPercent != iDarkenPercent))
//	{
//		g_iDarkenPercent = iDarkenPercent;
//
//		// a little bit darker
//		for (i = 0; i < 256; i++)
//		{
//			iRed   = pPal->GetR( i) * iDarkenPercent / 100;
//			iGreen = pPal->GetG( i) * iDarkenPercent / 100;
//			iBlue  = pPal->GetB( i) * iDarkenPercent / 100;
//
//			g_byEffectsIndexLookUp[0][i] = 
//					pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//		}
//
//		iDarkenPercent -= 10;
//
//		// a lot darker
//		for (i = 0; i < 256; i++)
//		{
//			iRed   = pPal->GetR( i) * iDarkenPercent / 100;
//			iGreen = pPal->GetG( i) * iDarkenPercent / 100;
//			iBlue  = pPal->GetB( i) * iDarkenPercent / 100;
//
//			g_byEffectsIndexLookUp[1][i] = 
//					pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//		}
//	}

	g_iDarkenPercent = ToolAmountToPercent( g_iCurrentTool);
	m_iLastTool      = g_iCurrentTool;
}

void CWallyView::OnUpdateTpDarken( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_DARKEN);
}

void CWallyView::OnTpBrush() 
{
	g_Tool.InitTool( this, EDIT_MODE_BRUSH);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpBrush( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_BRUSH);
}

void CalcTintLookup( CWallyPalette* pPal, int iColor, int iTintAmount, int iTableIndex)
{
	// blend in a little bit of iColor

//	for (int i = 0; i < 256; i++)
//	{
//		int iRed   = (pPal->GetR( i)      * (100 - iTintAmount) / 100) + 
//					 (pPal->GetR( iColor) * iTintAmount / 100);
//		int iGreen = (pPal->GetG( i)      * (100 - iTintAmount) / 100) + 
//					 (pPal->GetG( iColor) * iTintAmount / 100);
//		int iBlue  = (pPal->GetB( i)      * (100 - iTintAmount) / 100) + 
//					 (pPal->GetB( iColor) * iTintAmount / 100);
//
//		g_byEffectsIndexLookUp[iTableIndex][i] = 
//				pPal->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//	}
}

/*void CalcHSVLookup( CWallyPalette* pPal, int iColor, int iTableIndex)
{
	int iRed   = pPal->GetR( iColor);
	int iGreen = pPal->GetG( iColor);
	int iBlue  = pPal->GetB( iColor);

	double dfSrcHue, dfSrcSaturation, dfSrcValue;
	RGBtoHSV( iRed, iGreen, iBlue, &dfSrcHue, &dfSrcSaturation, &dfSrcValue);

	for (int i = 0; i < 256; i++)
	{
		iRed   = pPal->GetR( i);
		iGreen = pPal->GetG( i);
		iBlue  = pPal->GetB( i);

		double dfHue, dfSaturation, dfValue;

		RGBtoHSV( iRed, iGreen, iBlue, &dfHue, &dfSaturation, &dfValue);
		HSVtoRGB( dfSrcHue, dfSrcSaturation, dfValue, &iRed, &iGreen, &iBlue);

		g_byEffectsIndexLookUp[iTableIndex][i] = 
				pPal->FindNearestColor( iRed, iGreen, iBlue, i == 255);
	}
}
*/

void CWallyView::OnTpSpray() 
{
	g_Tool.InitTool( this, EDIT_MODE_SPRAY);
/*
	CWallyDoc*     pDoc = GetDocument();
	CWallyPalette* pPal = pDoc->GetPalette();

	// neal - initialize the palette index transform tables

	int iTintAmount = (int )(ToolAmountToPercent( g_iCurrentTool) * 1.5);

	// a little bit of left button color
	int iColor = g_iColorIndexLeft;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastLeftColor != iColor))
	{
		g_iLastLeftColor = m_iLastLeftColor = iColor;
		CalcTintLookup( pPal, iColor, iTintAmount, 0);
	}

	// a little bit of right button color
	iColor = g_iColorIndexRight;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastRightColor != iColor))
	{
		g_iLastRightColor = m_iLastRightColor = iColor;
		CalcTintLookup( pPal, iColor, iTintAmount, 1);
	}
*/
//	g_iLastLeftColor  = m_iLastLeftColor  = g_iColorIndexLeft;
//	g_iLastRightColor = m_iLastRightColor = g_iColorIndexRight;

	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpSpray( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_SPRAY);
}

void CWallyView::OnTpReplace() 
{
	g_Tool.InitTool( this, EDIT_MODE_REPLACE);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpReplace( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_REPLACE);	
}

void CWallyView::OnTpClone() 
{
	g_Tool.InitTool( this, EDIT_MODE_CLONE);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpClone( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_CLONE);	
}

void CWallyView::OnTpRubberStamp() 
{
	g_Tool.InitTool( this, EDIT_MODE_RUBBER_STAMP);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpRubberStamp( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_RUBBER_STAMP);	
}

void CWallyView::OnTpFloodFill() 
{
	g_Tool.InitTool( this, EDIT_MODE_FLOOD_FILL);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpFloodFill( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_FLOOD_FILL);
}

void CWallyView::OnTpScratch() 
{
	g_Tool.InitTool( this, EDIT_MODE_SCRATCH);

//	CWallyDoc*     pDoc = GetDocument();
//	CWallyPalette* pPal = pDoc->GetPalette();
//
//	int i, iRed, iGreen, iBlue;
//
//	// neal - initialize the palette index transform tables
//
//	int iAmount = 100 - ToolAmountToPercent( g_iCurrentTool);
//
//	// a bit darker
//	for (i = 0; i < 256; i++)
//	{
//		iRed   = pPal->GetR( i) * iAmount / 100;
//		iGreen = pPal->GetG( i) * iAmount / 100;
//		iBlue  = pPal->GetB( i) * iAmount / 100;
//
//		g_byEffectsIndexLookUp[0][i] = 
//				pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//	}
//
//	// a little bit lighter
//	double dfAmount = ToolAmountToPercent( g_iCurrentTool) / 3;
//
//	// a little bit lighter
//	for (i = 0; i < 256; i++)
//	{
//		iRed   = (int )((255*dfAmount/100) + pPal->GetR( i) * (100-dfAmount) / 100);
//		iGreen = (int )((255*dfAmount/100) + pPal->GetG( i) * (100-dfAmount) / 100);
//		iBlue  = (int )((255*dfAmount/100) + pPal->GetB( i) * (100-dfAmount) / 100);
//
//		g_byEffectsIndexLookUp[1][i] = 
//				pDoc->FindNearestColor( iRed, iGreen, iBlue, i == 255);
//	}

	m_iLastTool       = g_iCurrentTool;
	//m_iLastLeftColor  = g_iColorIndexLeft;
	//m_iLastRightColor = g_iColorIndexRight;
}

void CWallyView::OnUpdateTpScratch( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_SCRATCH);
}

void CWallyView::OnTpTint() 
{
	g_Tool.InitTool( this, EDIT_MODE_TINT);
/*
	CWallyDoc*     pDoc = GetDocument();
	CWallyPalette* pPal = pDoc->GetPalette();

	// neal - initialize the palette index transform tables

	int iTintAmount = (int )(ToolAmountToPercent( g_iCurrentTool) * 1.5);

	// a little bit of left button color
	int iColor = g_iColorIndexLeft;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastLeftColor != iColor))
	{
		g_iLastLeftColor = m_iLastLeftColor = iColor;
		CalcTintLookup( pPal, iColor, iTintAmount, 0);
	}

	// a little bit of right button color
	iColor = g_iColorIndexRight;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastRightColor != iColor))
	{
		g_iLastRightColor = m_iLastRightColor = iColor;
		CalcTintLookup( pPal, iColor, iTintAmount, 1);
	}
*/
//	g_iLastLeftColor  = m_iLastLeftColor  = g_iColorIndexLeft;
//	g_iLastRightColor = m_iLastRightColor = g_iColorIndexRight;

	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpTint( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_TINT);
}

void CWallyView::OnTpBulletHoles() 
{
	g_Tool.InitTool( this, EDIT_MODE_BULLET_HOLES);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpBulletHoles( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_BULLET_HOLES);
}

void CWallyView::OnTpRivets() 
{
	g_Tool.InitTool( this, EDIT_MODE_RIVETS);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpRivets( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iCurrentTool == EDIT_MODE_RIVETS);
}

// neal - we need this one to trap "undocumented" virtual key code
// after Windows has turned it into a regular char
void CWallyView::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//TRACE2( "OnChar Key = 0x%04X  (%c)\n", nChar, nChar);

	BOOL bNeedToRedrawPalette = FALSE;
	int  iColor = 255;

	if (nChar == '[')
	{
//		g_iColorIndexLeft    = (g_iColorIndexLeft - 1) & 0xFF;
		iColor               = GetIValue( g_irgbColorLeft);
		iColor               = (iColor - 1) & 0xFF;
		g_irgbColorLeft      = g_CurrentPalette.GetIRGB( iColor);
		bNeedToRedrawPalette = TRUE;
	}
	else if (nChar == ']')
	{
//		g_iColorIndexLeft    = (g_iColorIndexLeft + 1) & 0xFF;
		iColor               = GetIValue( g_irgbColorLeft);
		iColor               = (iColor + 1) & 0xFF;
		g_irgbColorLeft      = g_CurrentPalette.GetIRGB( iColor);
		bNeedToRedrawPalette = TRUE;
	}
	else if (nChar == '{')
	{
//		g_iColorIndexRight   = (g_iColorIndexRight - 1) & 0xFF;
		iColor               = GetIValue( g_irgbColorRight);
		iColor               = (iColor - 1) & 0xFF;
		g_irgbColorRight     = g_CurrentPalette.GetIRGB( iColor);
		bNeedToRedrawPalette = TRUE;
	}
	else if (nChar == '}')
	{
//		g_iColorIndexRight   = (g_iColorIndexRight + 1) & 0xFF;
		iColor               = GetIValue( g_irgbColorRight);
		iColor               = (iColor + 1) & 0xFF;
		g_irgbColorRight     = g_CurrentPalette.GetIRGB( iColor);
		bNeedToRedrawPalette = TRUE;
	}
	else
	{
		CScrollView::OnChar( nChar, nRepCnt, nFlags);
	}

	if (bNeedToRedrawPalette)
		((CMainFrame* )AfxGetMainWnd())->
					m_wndPaletteToolBar.InvalidateRect( NULL, FALSE);
}

void CWallyView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bConstrainUpdate = FALSE;

	nChar = toupper( nChar);
//	TRACE1( "Virt Key code = 0x%04X\n", nChar);

	if (nChar == VK_CONTROL)
	{
		// neal - toggle eyedropper cursor immediately

		if ((nFlags & KF_REPEAT) == 0)
			OnSetCursor( this, HTCLIENT, WM_MOUSEMOVE);
	}
	else if (nChar == VK_SHIFT)
	{
//		if (nRepCnt <= 1)
		if ((nFlags & KF_REPEAT) == 0)
		{
			g_bConstrained   = TRUE;
			bConstrainUpdate = TRUE;

			g_Tool.UpdateConstrainPaneText();

			if (GetDrawingMode( g_iCurrentTool) != MODE_FREEHAND)
			{
				CPoint ptPos = GetMessagePos();
				ScreenToClient( &ptPos);

				g_Tool.UseTool( this, GetKeyFlags(), ptPos, MOUSE_MOVE);
			}
		}
	}
	else if ((nChar >= VK_LEFT) && (nChar <= VK_DOWN))
	{
		switch (nChar)
		{
		case VK_UP:
			ShiftImage( SHIFT_UP);
			break;
		case VK_DOWN:
			ShiftImage( SHIFT_DOWN);
			break;
		case VK_LEFT:
			ShiftImage( SHIFT_LEFT);
			break;
		case VK_RIGHT:
			ShiftImage( SHIFT_RIGHT);
			break;
		default:
			ASSERT( FALSE);
			break;
		}
	}
	else if (nChar == VK_SPACE)
	{
		if (GetDrawingMode( g_iCurrentTool) == MODE_FREEHAND)
		{
			g_bConstrainedHorizonal = ! g_bConstrainedHorizonal;

			g_Tool.UpdateConstrainPaneText();

			if (g_bConstrained)
			{
				CPoint ptPos = GetMessagePos();
				SetCursorPos( ptPos.x, ptPos.y);
				GetCursorPos( &ptPos);
				ScreenToClient( &ptPos);

				// improve constrain direction toggling behavior
				g_Tool.UseTool( this, GetKeyFlags(), ptPos, MOUSE_MOVE);

				bConstrainUpdate = TRUE;
			}
		}
	}
	else if (nChar == 'F')		// Flip?
	{
		g_bFlipped = ! g_bFlipped;
	}
	else if (nChar == VK_RETURN)		// new drawing mode line origin?
	{
		DrawingModeRestart();
	}
	else if (nChar == VK_ESCAPE)		// cancel current line in progress?
	{
		OnCancelMode();
	}

	if (bConstrainUpdate)
	{
		if (GetDrawingMode( g_iCurrentTool) == MODE_FREEHAND)
		{
			CPoint ptPos = GetMessagePos();
			CRect rRect( ptPos.x, ptPos.y, ptPos.x+1, ptPos.y+1);

			if (g_bConstrainedHorizonal)
			{
				rRect.left  = 0;
				rRect.right = 10000;
			}
			else
			{
				rRect.top    = 0;
				rRect.bottom = 10000;
			}
			ClipCursor( &rRect);
		}
		else
		{
			ClipCursor( NULL);		// line or box constrain
		}
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWallyView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// neal - toggle eyedropper cursor immediately
	if (nChar == VK_CONTROL)
	{
		OnSetCursor( this, HTCLIENT, WM_MOUSEMOVE);

		// neal - immediately redraw big cursor
		if (IsDrawingTool( g_iCurrentTool))
		{
			if (!( m_bLeftButtonTracking || m_bRightButtonTracking))
			{
				if (ToolHasCursor( g_iCurrentTool))
				{
					CPoint ptPos = GetMessagePos();
					ScreenToClient( &ptPos);
					GetTrueImageCoordinates( &ptPos);

					int iToolWidth = GetToolWidth();
					g_Tool.DrawBigCursor( this, ptPos, iToolWidth, FALSE);
/*
					if (m_iZoomValue == 0)
					{
						ASSERT( FALSE);
						GetDocSize( NULL);
					}
					iToolWidth = ((iToolWidth + 1) / 2) * m_iZoomValue;
					CRect rCursor( ptPos.x - iToolWidth, ptPos.y - iToolWidth,
									ptPos.x + iToolWidth, ptPos.y + iToolWidth);

					InvalidateRect( rCursor, FALSE);
*/
					InvalidateRect( NULL, FALSE);
					//DataModified = true;
					UpdateWindow();
					g_Tool.ClearBigCursor( this);
					SetTimer( TIMER_TOOL_CLEAR_CURSOR, 250, NULL);
				}
			}
		}
	}
	else if (nChar == VK_SHIFT)
	{
		g_bConstrained = FALSE;

		ClipCursor( NULL);
		g_Tool.UpdateConstrainPaneText();

		if (GetDrawingMode( g_iCurrentTool) != MODE_FREEHAND)
		{
			CPoint ptPos = GetMessagePos();
			ScreenToClient( &ptPos);

			g_Tool.UseTool( this, GetKeyFlags(), ptPos, MOUSE_MOVE);
		}
	}

	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CWallyView::OnTimer( UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_TOOL_SPRAY:
		{
			CPoint ptPos = GetMessagePos();
			ScreenToClient( &ptPos);

			g_Tool.UseTool( this, GetKeyFlags(), ptPos, TIMER_MSG);
		}
		break;

	case TIMER_TOOL_CLEAR_CURSOR:
		{
			CPoint ptPos = GetMessagePos();
			ScreenToClient( &ptPos);

			HDC hdc = ::GetDC( m_hWnd);

			if (hdc)
			{
				if (! PtVisible( hdc, ptPos.x, ptPos.y))
				{
					g_Tool.ClearBigCursor( this);
					InvalidateRect( NULL, FALSE);

					KillTimer( TIMER_TOOL_CLEAR_CURSOR);
				}
				::ReleaseDC( m_hWnd, hdc);
			}
		}
		break;

	case TIMER_WV_ANIMATE_SELECTION:
		{
			CWallyDoc*  pDoc     = GetDocument();
			CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();

//			CWallyDoc* pActiveDoc = (CWallyDoc* )pWndMain->GetActiveDocument();
//			if (pDoc == pActiveDoc)

			if (pDoc && pDoc->HasSelection())
//			if (pDoc)			// ************* TEST <<<<<<<<<<<<<<<<<<<<<<<<
			{
				if (m_iZoomValue == 0)
				{
					ASSERT( FALSE);
					GetDocSize( NULL);
				}

				// nw - new selection display

				CDC* pDC = GetDC();
				if (pDC)
				{
					CSelection* pSel = pDoc->GetSelection();

					if (GetFocus() == this)
						pSel->StepAnimation();

					CPoint ptOffset( GetDeviceScrollPosition());
					ptOffset.x = -ptOffset.x;
					ptOffset.y = -ptOffset.y;

//					if (m_bTile)
//					{
//						ptOffset.x += m_DIB[0].GetImageWidth()  * m_iZoomValue;
//						ptOffset.y += m_DIB[0].GetImageHeight() * m_iZoomValue;
//					}

					pSel->DrawSelection( pDC, m_dfZoom, ptOffset);
					ReleaseDC( pDC);
				}
			}
		}
		break;

	case TIMER_TOOL_AUTO_REMIP:
		KillTimer( nIDEvent);

		if (g_bAutoRemipAfterEdit)
		{
			//GetDocument()->RebuildSubMips();				
			
			// Ty - replaced call to UpdateDIBs() (and Invalidate()) with a call 			
			// to the document's UpdateAllDIBs(), which should search out and 
			// pick out all viewsof the doc
			
			// UpdateDIBs();			
			// Invalidate();
			
			GetDocument()->UpdateAllDIBs( TRUE);
		}
		break;

	case TIMER_WV_REMIP:
		KillTimer( nIDEvent);

		//GetDocument()->RebuildSubMips();			
		GetDocument()->UpdateAllDIBs( TRUE);
		break;

	default:
		CScrollView::OnTimer( nIDEvent);
	}
}

void CWallyView::OnTpWidth() 
{
	CPoint ptScreenPos;
	GetCursorPos( &ptScreenPos);

	if ((ptScreenPos.x == -1) && (ptScreenPos.y == -1))
	{
		//keystroke invocation
		CRect rect;
		GetClientRect( rect);
		ClientToScreen( rect);

		ptScreenPos = rect.TopLeft();
		ptScreenPos.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu( CG_IDR_POPUP_DRAWING_WIDTH));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
		ptScreenPos.x, ptScreenPos.y, pWndPopupOwner);
}

void CWallyView::OnTpAmount() 
{
	CPoint ptScreenPos;
	GetCursorPos( &ptScreenPos);

	if ((ptScreenPos.x == -1) && (ptScreenPos.y == -1))
	{
		//keystroke invocation
		CRect rect;
		GetClientRect( rect);
		ClientToScreen( rect);

		ptScreenPos = rect.TopLeft();
		ptScreenPos.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu( CG_IDR_POPUP_AMOUNT));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
		ptScreenPos.x, ptScreenPos.y, pWndPopupOwner);
}

void CWallyView::OnTpShape() 
{
	CPoint ptScreenPos;
	GetCursorPos( &ptScreenPos);

	if ((ptScreenPos.x == -1) && (ptScreenPos.y == -1))
	{
		//keystroke invocation
		CRect rect;
		GetClientRect( rect);
		ClientToScreen( rect);

		ptScreenPos = rect.TopLeft();
		ptScreenPos.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu( CG_IDR_POPUP_SHAPE));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
		ptScreenPos.x, ptScreenPos.y, pWndPopupOwner);
}

void CWallyView::OnUpdateTpWidth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ToolHasWidth( g_iCurrentTool));
}

void CWallyView::OnUpdateTpAmount(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ToolHasAmount( g_iCurrentTool));
}

void CWallyView::OnUpdateTpShape(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ToolHasShape( g_iCurrentTool));
}

void CWallyView::OnEditCopyTiled()
{
	CCopyTiledDlg GetNumber;

	GetNumber.m_iHorizontalTiles = g_iNumHorizontalTiles;
	GetNumber.m_iVerticalTiles   = g_iNumVerticalTiles;

	if (GetNumber.DoModal() == IDOK)
	{
		g_iNumHorizontalTiles = GetNumber.m_iHorizontalTiles;
		g_iNumVerticalTiles   = GetNumber.m_iVerticalTiles;

		// update registry imediately (in case Wally dies later)

		CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();
		pWndMain->SaveWindowState();

		//m_DIB[0].WriteToClipboardTiled( this, g_iNumHorizontalTiles, g_iNumVerticalTiles);

		CWallyDoc* pDoc = GetDocument();

		if (pDoc && pDoc->HasSelection())
		{
			CSelection*    pSel = pDoc->GetSelection();
			CWallyPalette* pPal = pDoc->GetPalette();

			if (pSel && pPal)
			{
				CLayer* pLayer = pSel->GetLayer();
				pLayer->m_Pal  = *pPal;

				if (pLayer && (! pLayer->HasData()))
				{
					// neal - TODO: move data storage from MIP to 24 bit CLayer

					int iWidth  = pDoc->Width();
					int iHeight = pDoc->Height();

					pLayer->ReCreate( pDoc, iWidth, iHeight, 24);

//					pLayer->m_Pal = *pDoc->GetPalette();
//					pLayer->SetNumBits( 24);
//					pLayer->SetWidth( iWidth);
//					pLayer->SetHeight( iHeight);
//
//					pLayer->m_pIRGB_Data = (COLOR_IRGB *)malloc( 
//							iWidth * iHeight * sizeof( COLOR_IRGB));

					for (int j = 0; j < iHeight; j++)
					{
						for (int i = 0; i < iWidth; i++)
						{
//							int iIndex = j*iWidth + i;
							//int iColor = pDoc->GetMipPixel( iIndex, 0);
							//pSel->m_pLayer->m_pIRGB_Data[iIndex] =
							//		IRGB( iColor, pPal->GetR( iColor),
							//		pPal->GetG( iColor), pPal->GetB( iColor));

							COLOR_IRGB irgbColor = pDoc->GetPixel( i, j);
//							pLayer->m_pIRGB_Data[iIndex] = irgbColor;
							pLayer->SetPixel( this, i, j, irgbColor);
						}
					}

					if (pLayer->HasData())
					{
						pLayer->WriteToClipboardTiled( this, 
								g_iNumHorizontalTiles, g_iNumVerticalTiles);


//						free( pLayer->m_pIRGB_Data);
//						pLayer->m_pIRGB_Data = NULL;
						pLayer->FreeMem();
					}
				}
				else
				{
					pLayer->WriteToClipboardTiled( this, 
							g_iNumHorizontalTiles, g_iNumVerticalTiles);
				}
			}
			else
			{
				ASSERT( FALSE);
			}
		}
		else
		{
			m_DIB[0].WriteToClipboardTiled( this, g_iNumHorizontalTiles, g_iNumVerticalTiles);
		}
	}
}

void CWallyView::OnKillFocus( CWnd* pNewWnd)
{
	CScrollView::OnKillFocus( pNewWnd);

	// remove cursor constraining
	ClipCursor( NULL);	
}

void CWallyView::UpdateColorPalette()
{
	CMainFrame*           pWndMain      = (CMainFrame* )AfxGetMainWnd();
	CColorPalette*        pWndPalette   = &pWndMain->m_wndPaletteToolBar;
	CToolSettingsToolbar* pToolSettings = &pWndMain->m_wndToolSettingsToolbar;
	CWallyDoc*            pDoc          = GetDocument();
	CWallyPalette*        pPal          = pDoc->GetPalette();

	if (pPal->GetNumColors() > 0)
		g_CurrentPalette = *(pPal);

	pWndPalette->Update( pDoc, m_iLastTool != g_iCurrentTool);
	pToolSettings->Update( m_iLastTool != g_iCurrentTool);
}

void CWallyView::OnTpPatternedPaint()
{
	g_Tool.InitTool( this, EDIT_MODE_PATTERNED_PAINT);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpPatternedPaint( CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_PATTERNED_PAINT);
}

void CWallyView::OnTpDecal()
{
	g_Tool.InitTool( this, EDIT_MODE_DECAL);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpDecal( CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_DECAL);
}

void CWallyView::OnTpBlend() 
{
	g_Tool.InitTool( this, EDIT_MODE_BLEND);
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpBlend(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_BLEND);
}

void CWallyView::OnImageMirror() 
{
	g_Filter.DoFilter( this, FILTER_MIRROR, TRUE);
}

void CWallyView::OnImageOffset() 
{
	g_Filter.DoFilter( this, FILTER_OFFSET, TRUE);
}

void CWallyView::OnUpdateImageOffset(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ! GetDocument()->HasSelection());
}

void CWallyView::OnImageFlipHorz() 
{
	FreeEffectsData();
	g_Filter.DoFilter( this, FILTER_FLIP_HORIZONTAL, 0);
}

void CWallyView::OnImageFlipVert() 
{
	FreeEffectsData();
	g_Filter.DoFilter( this, FILTER_FLIP_VERTICAL, 0);
}

void CWallyView::OnImageResize()
{
	g_Filter.DoFilter( this, FILTER_RESIZE, TRUE);
}

void CWallyView::OnUpdateImageResize( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( ! GetDocument()->HasSelection());
}

void CWallyView::OnImageEnlarge() 
{
	// Neal: TODO: enlarge the selection instead/or/enlarge-both

	CWallyDoc* pDoc = GetDocument();
	CEnlargeCanvasDlg EnlargeDlg;

	int iOriginalWidth = pDoc->GetWidth();
	int iOriginalHeight = pDoc->GetHeight();

	EnlargeDlg.SetDimensions (iOriginalWidth, iOriginalHeight);

	if (EnlargeDlg.DoModal() == IDOK)
	{
		int iWidth = 0;
		int iHeight = 0;
		int iSize = 0;
		int j = 0;
		int iXSource = 0;
		int iYSource = 0;
		int iXDestination = 0;
		int iYDestination = 0;
		int wS = 0;
		int hS = 0;
		int wD = 0;
		int hD = 0;

		EnlargeDlg.GetDimensions (&iWidth, &iHeight);

		if ((iOriginalWidth == iWidth) && (iOriginalHeight == iHeight))
		{
			// No sense continuing if the size hasn't changed...
			return;
		}
		
		BYTE *pbySource      = NULL;
		BYTE *pbyDestination = NULL;

		iSize = iOriginalWidth * iOriginalHeight;
		pbySource = new BYTE[iSize];

		if (!pbySource)
		{
			AfxMessageBox ("Out of memory during Enlarge Canvas", MB_ICONSTOP);
			return;
		}

//		memset (pbySource, 0, iSize);
//		memcpy (pbySource, pDoc->GetBits(), iSize);
		
		pDoc->m_Undo.SaveUndoState( pDoc, "Enlarge Canvas");

		FreeEffectsData();

		// Neal - fixes trying to resize selection instead of base bug
		//
		pDoc->MergeSelectionIntoBase( NULL);

		// Neal - fixes 24-bit image is cleared bug
		//
		CLayer* pLayer = new CLayer;
		CLayer* pBase  = pDoc->GetBase()->GetLayer();
		ASSERT( pBase != NULL);		// Neal - it damn well better be valid!

		if (pLayer)
		{
			pLayer->DupLayer( this, pBase);
		}
		else
		{
			ASSERT( FALSE);		// mem alloc failed
			MessageBeep( MB_ICONEXCLAMATION);
		}

		pBase->FreeMem();		// prevents asserts
		pDoc->SetDimensions( iWidth, iHeight);
		pDoc->RebuildLayerAndMips( true);

		pbyDestination = pDoc->GetBits();

		g_bEnlargeCenterHorizontally = EnlargeDlg.CenterHorizontally();
		g_bEnlargeCenterVertically   = EnlargeDlg.CenterVertically();

		// Neal - this does the same thing as the original code, but is far more efficient
		//
		if (g_bEnlargeCenterHorizontally)
		{
			if (iWidth > iOriginalWidth)
			{
				iXDestination = (iWidth - iOriginalWidth) / 2;
			}
			else
			{
				iXSource = (iOriginalWidth - iWidth) / 2;
			}
		}
		if (g_bEnlargeCenterVertically)
		{
			if (iHeight > iOriginalHeight)
			{
				iXDestination = (iHeight - iOriginalHeight) / 2;
			}
			else
			{
				iXSource = (iOriginalHeight - iHeight) / 2;
			}
		}

//		for (hD = iYDestination, hS = iYSource; (hD < iHeight) && (hS < iOriginalHeight); hD++, hS++)
//		{
//			memcpy (pbyDestination + (hD * iWidth) + iXDestination, 
//					pbySource + (hS * iOriginalWidth) + iXSource, 
//					iWidth > iOriginalWidth ? iOriginalWidth : iWidth);
//		}

		// Neal - fixes 24-bit image is cleared bug
		//
		if (pLayer)
		{
			for (int j = 0; j < iHeight; j++)
			{
				if (j < iOriginalHeight)
				{
					for (int i = 0; i < iWidth; i++)
					{
						if (i < iOriginalWidth)
						{
							COLOR_IRGB irgb = pLayer->GetPixel( i, j);
							pBase->SetPixel( this, i + iXSource, j + iYSource, irgb);
						}
					}
				}
			}
			delete pLayer;
			pLayer = NULL;

			pDoc->CopyLayerToMip( NULL);
		}

		if (pbySource)
		{
			delete []pbySource;
			pbySource = NULL;
		}
		
//		// Yank out the selection, if any
//		if (pDoc->HasSelection())
//		{
//			pDoc->m_pSelection = NULL;
//		}
		
		pDoc->SetModifiedFlag( TRUE);
		pDoc->UpdateAllDIBs( TRUE);
	}
}

void CWallyView::OnImageRotate() 
{
	CRotateDlg Rotate;
	CWallyDoc* pDoc = GetDocument();
	int DestinationAngle;

	if (Rotate.DoModal() == IDOK)
	{			
		FreeEffectsData();

		pDoc->m_Undo.SaveUndoState( pDoc, "Rotate");
		pDoc->SetModifiedFlag(true);

		// Figure out the true ending position, regardless of direction
		if (g_iRotateDegrees == ROTATE_180)
			DestinationAngle = ROTATETO180;

		if  (
			 ((g_iRotateDegrees == ROTATE_90)  && (g_iRotateDirection == ROTATE_LEFT)) 
			||
			 ((g_iRotateDegrees == ROTATE_270) && (g_iRotateDirection == ROTATE_RIGHT))
		    )
			DestinationAngle = ROTATETO270;

		if  (
			 ((g_iRotateDegrees == ROTATE_90)  && (g_iRotateDirection == ROTATE_RIGHT)) 
			||
			 ((g_iRotateDegrees == ROTATE_270) && (g_iRotateDirection == ROTATE_LEFT))
		    )
			DestinationAngle = ROTATETO90;
		
		int iWidth = pDoc->GetWidth();
		int iHeight = pDoc->GetHeight();
		
		for (int j = 0; j < 4; j++)
		{
			m_DIB[j].RotateBits( DestinationAngle);
			//m_DIB[j].GetRawBits( pDoc->GetBits(j));
		}		

		CLayer* pLayer   = pDoc->GetBase()->GetLayer();
		pLayer->LoadFromDibSection( &m_DIB[0]);

		// If the height/width are now different, reset the dimensions
		if ((DestinationAngle != ROTATETO180) && (iWidth != iHeight))
		{
			int iSwap = iWidth;
			iWidth    = iHeight;
			iHeight   = iSwap;
			
			int     iNumBits = pLayer->GetNumBits();
			//pLayer->FreeMem();				// prevents asserts
			pDoc->SetDimensions( iWidth, iHeight);
			//pLayer->Create( pDoc, iWidth, iHeight, iNumBits);
		}
		//pDoc->CopyMipToLayer();
		
		pDoc->UpdateAllDIBs( TRUE);
	}
}

void CWallyView::OnUpdateImageRotate( CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ! GetDocument()->HasSelection());
}

void CWallyView::OnSize( UINT nType, int cx, int cy)
{
	// neal - do we need to layout the mips again?
	if ((! m_bSettingScrollBars) && m_bInitialized)
	{
		CRect rClient;
		GetClientRect( &rClient);
		GetDocSize( &rClient);
		SetScrollSizes( MM_TEXT, m_DocSize);
	}

	CScrollView::OnSize( nType, cx, cy);
}

void CWallyView::OnTpRecolor() 
{
	g_Tool.InitTool( this, EDIT_MODE_RECOLOR);
/*
	CWallyDoc*     pDoc = GetDocument();
	CWallyPalette* pPal = pDoc->GetPalette();

	// neal - initialize the palette index transform tables

	// a little bit of left button color
	int iColor = g_iColorIndexLeft;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastLeftColor != iColor))
	{
		g_iLastLeftColor = m_iLastLeftColor = iColor;
		CalcHSVLookup( pPal, iColor, 0);
	}

	// a little bit of right button color
	iColor = g_iColorIndexRight;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastRightColor != iColor))
	{
		g_iLastRightColor = m_iLastRightColor = iColor;
		CalcHSVLookup( pPal, iColor, 1);
	}
*/
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpRecolor(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_RECOLOR);
}

void CWallyView::OnTpSprayRecolor() 
{
	g_Tool.InitTool( this, EDIT_MODE_SPRAY_RECOLOR);
/*
	CWallyDoc*     pDoc = GetDocument();
	CWallyPalette* pPal = pDoc->GetPalette();

	// neal - initialize the palette index transform tables

	// a little bit of left button color
	int iColor = g_iColorIndexLeft;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastLeftColor != iColor))
	{
		g_iLastLeftColor = m_iLastLeftColor = iColor;
		CalcHSVLookup( pPal, iColor, 0);
	}

	// a little bit of right button color
	iColor = g_iColorIndexRight;

	if ((m_iLastTool != g_iCurrentTool) || (g_iLastRightColor != iColor))
	{
		g_iLastRightColor = m_iLastRightColor = iColor;
		CalcHSVLookup( pPal, iColor, 1);
	}
*/
	m_iLastTool = g_iCurrentTool;
}

void CWallyView::OnUpdateTpSprayRecolor(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (g_iCurrentTool == EDIT_MODE_SPRAY_RECOLOR);
}

void CWallyView::DrawingModeRestart()
{
	UpdateColorPalette();
	g_Tool.UpdateConstrainPaneText();

	if (! (m_bLeftButtonTracking || m_bRightButtonTracking))
	{
		CPoint ptPos = GetMessagePos();
		ScreenToClient( &ptPos);

		g_Tool.UseTool( this, GetKeyFlags(), ptPos, MOUSE_BUTTON_UP);
	}
	g_Tool.m_bLastMouseDownIsValid = FALSE;
	g_Tool.m_bLastMouseUpIsValid   = FALSE;
}

void CWallyView::OnCancelMode() 
{
	CScrollView::OnCancelMode();

	CWallyDoc* pDoc = GetDocument();

	if (pDoc)
	{
		if (m_bLeftButtonTracking || m_bRightButtonTracking)
		{
			CUndoState* pState = pDoc->m_DrawingModeUndo.GetLastState();
			if (pState)
				pDoc->m_DrawingModeUndo.RestoreState( pState);

			pDoc->UpdateAllDIBs( FALSE);

			m_bLeftButtonTracking  = FALSE;
			m_bRightButtonTracking = FALSE;

			ReleaseCapture();
		}
		else if (pDoc->HasSelection())
		{
			OnSelectionSelectNone();
		}
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CWallyView::OnDrawingModeFreehand()
{
	g_iDrawingMode = MODE_FREEHAND;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModeLine()
{
	g_iDrawingMode = MODE_LINE;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModePolyline()
{
	g_iDrawingMode = MODE_POLYLINE;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModeRay()
{
	g_iDrawingMode = MODE_RAY;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModeHollowRect()
{
	g_iDrawingMode = MODE_HOLLOW_RECT;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModeSolidRect()
{
	g_iDrawingMode = MODE_SOLID_RECT;
	DrawingModeRestart();
}

void CWallyView::OnDrawingModeEdgedRect()
{
	g_iDrawingMode = MODE_EDGED_RECT;
	DrawingModeRestart();
}


void CWallyView::OnUpdateDrawingModeFreehand( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_FREEHAND));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_FREEHAND);
}

void CWallyView::OnUpdateDrawingModeLine( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_LINE));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_LINE);
}

void CWallyView::OnUpdateDrawingModePolyline( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_POLYLINE));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_POLYLINE);
}

void CWallyView::OnUpdateDrawingModeRay( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_RAY));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_RAY);
}

void CWallyView::OnUpdateDrawingModeHollowRect( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_HOLLOW_RECT));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_HOLLOW_RECT);
}

void CWallyView::OnUpdateDrawingModeSolidRect( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_SOLID_RECT));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_SOLID_RECT);
}

void CWallyView::OnUpdateDrawingModeEdgedRect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( HasDrawingMode( g_iCurrentTool, MODE_EDGED_RECT));
	pCmdUI->SetRadio( GetDrawingMode( g_iCurrentTool) == MODE_EDGED_RECT);
}

void CWallyView::OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CScrollView::OnActivateView( bActivate, pActivateView, pDeactiveView);

//	neal - the following is not allowed for an MDI app
//	CMainFrame* pMainFrame = (CMainFrame* )AfxGetMainWnd(); 
//
//	if (bActivate)
//	{
//		UpdateColorPalette();
//		if (pMainFrame)
//			pMainFrame->SetActiveView( this, FALSE);
//	}
//	else
//	{
//		if (pMainFrame)
//			pMainFrame->SetActiveView( NULL, FALSE);
//	}

	// Neal - fixes switch-views-palette-not-updated bug
	if (bActivate)
		UpdateColorPalette();
}

void CWallyView::OnImageAddNoise() 
{
	g_Filter.DoFilter( this, FILTER_ADD_NOISE, TRUE);
}

void CWallyView::OnImageBlend() 
{
	g_Filter.DoFilter( this, FILTER_BLEND, TRUE);
}

void CWallyView::OnImageSharpen() 
{
	g_Filter.DoFilter( this, FILTER_SHARPEN, TRUE);
}

void CWallyView::OnImageBrightnessContrast() 
{
	g_Filter.DoFilter( this, FILTER_BRIGHTNESS_CONTRAST, TRUE);
}

void CWallyView::OnImageEmboss() 
{
	g_Filter.DoFilter( this, FILTER_EMBOSS, TRUE);
}

void CWallyView::OnImageEdgeDetect() 
{
	g_Filter.DoFilter( this, FILTER_EDGE_DETECT, TRUE);
}

void CWallyView::OnImageDiffuse() 
{
	g_Filter.DoFilter( this, FILTER_DIFFUSE, TRUE);
}

void CWallyView::OnImageGenrateMarbleTexture() 
{
	g_Filter.DoFilter( this, FILTER_MARBLE, TRUE);
}

void CWallyView::OnImageConvertTexturesQuakeToFromUnreal() 
{
	CConvertQuakeUnrealDlg Dlg;

	if (Dlg.DoModal() == IDOK)
	{
		CWallyDoc* pDoc = GetDocument();

		if (pDoc)
		{
			int iWidth =  pDoc->Width();
			int iHeight = pDoc->Height();

			// for every pixel...

			for (int j = 0; j < iHeight; j++)
			{
				for (int i = 0; i < iWidth; i++)
				{
					int iIndex = j*iWidth+i;
					//int iPixel = pDoc->GetMipPixel( iIndex, 0);
					COLOR_IRGB irgbColor = pDoc->GetPixel( i, j);
					int        iPixel    = GetIValue( irgbColor);

					////////////////////////////////////////////
					// if it is the transparency color index //
					// for either Quake or Unreal, swap 'em //
					/////////////////////////////////////////

					if ((iPixel == 0) || (iPixel == 255))
					{
						iPixel = (~iPixel) & 255;
						//pDoc->SetMipPixel( this, iIndex, iPixel, 0);
						pDoc->SetIndexColorPixel( this, i, j, iPixel);
					}
				}
			}

			pDoc->SetModifiedFlag( TRUE);		
			pDoc->RebuildSubMips();
			pDoc->UpdateSubMipDIBs();
			Invalidate();
		}
		else
		{
			// how did we get here without doc?

			MessageBeep( MB_ICONEXCLAMATION);
			ASSERT( FALSE);
		}
	}
}

void CWallyView::OnImageGenerateMarbleTileTexture()
{
	g_Filter.DoFilter( this, FILTER_MARBLE_TILE, TRUE);
}

void CWallyView::OnImageReduceColors()
{
	g_Filter.DoFilter( this, FILTER_REDUCE_COLORS, TRUE);
}

void CWallyView::OnUpdateImageReduceColors( CCmdUI* pCmdUI)
{
	CWallyDoc* pDoc = GetDocument();

	if (pDoc)
		pCmdUI->Enable( (pDoc->HasAdjustablePalette()) && 
					(! pDoc->HasSelection()));
}

void CWallyView::OnViewOptions() 
{
	// neal - yes, this function IS supposed to be in Wally.cpp also

	CWallyPropertySheet propSheet;

	propSheet.m_Page1.m_pDoc = GetDocument();

	propSheet.DoModal();
	theApp.UpdateAllDocs();
}

void CWallyView::OnFreeUndoMemory() 
{
	CFreeUndoDlg Dlg;

	Dlg.m_pDoc = GetDocument();

	Dlg.DoModal();
}

void CWallyView::OnColorsEditPalette()
{
	CWallyDoc *pDoc = GetDocument();
	CWallyPalette *pPal = pDoc->GetPalette();

	CPaletteEditorDlg dlgEditPalette;
	dlgEditPalette.SetPalette (pPal);

	if (dlgEditPalette.DoModal() == IDOK)
	{	
		pDoc->m_Undo.SaveUndoState( pDoc, "Load Palette", UNDO_PALETTE);
		
//		pDoc->GetPalette()->m_PalMap.RemoveAll();	// Neal - reinit things properly

		BYTE *pbyPalette = dlgEditPalette.GetPalette();
		pDoc->SetPalette (pbyPalette, 256, true);
		if (g_bAutoRemip)
		{
			pDoc->RebuildSubMips();
			pDoc->UpdateSubMipDIBs();
		}
		pDoc->SetModifiedFlag (true);
		FreeEffectsData();
		UpdateColorPalette();
	}
}

void CWallyView::OnColorsLoadPalette()
{
	CString strWildCard ("All Files (*.*)|*.*|");
	CImageHelper ihHelper;
	CString strAddString = ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath;	

	
	if (dlgPalette.DoModal() == IDOK)
	{
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		g_iOpenPaletteExtension   = dlgPalette.m_ofn.nFilterIndex;
		strSourceFilePath         = dlgPalette.GetPathName();		

		ihHelper.LoadImage (strSourceFilePath);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;
		}

		unsigned char *pbyPalette = NULL;
		unsigned char byCurrentPalette[768];

		if (!ihHelper.IsValidPalette())
		{
			if (ihHelper.IsValidImage())
			{
				if (ihHelper.GetColorDepth() == IH_8BIT)
				{
					pbyPalette = ihHelper.GetPalette();
				}
				else
				{
					AfxMessageBox ("Image does not have a palette", MB_ICONSTOP);
					return;
				}
			}
			else
			{
				AfxMessageBox ("Unsupported palette file", MB_ICONSTOP);
				return;
			}
		}
		else
		{
			pbyPalette = ihHelper.GetBits();
		}		

		CWallyDoc *pDoc = GetDocument();
		pDoc->m_Undo.SaveUndoState( pDoc, "Load Palette", UNDO_DOC_AND_PAL);
		pDoc->GetPalette (byCurrentPalette, 256);
		
		BYTE* pDocBits  = pDoc->GetBits();
		int   iSize     = pDoc->GetSize();
		int   iWidth    = pDoc->Width();
		int   iHeight   = pDoc->Height();
		int   iGameType = pDoc->GetGameType();
		int   j, r, g, b;

		COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
		for (j = 0; j < iSize; j++) 
		{
			r = byCurrentPalette[pDocBits[j] * 3 + 0];
			g = byCurrentPalette[pDocBits[j] * 3 + 1];
			b = byCurrentPalette[pDocBits[j] * 3 + 2];
	
			//pTemp24Bit[j].byRed   = r;
			//pTemp24Bit[j].byGreen = g;
			//pTemp24Bit[j].byBlue  = b;

			if (iGameType == FILE_TYPE_QUAKE2)
			{			
				//pTemp24Bit[j].byFullBright = ((r == 159) && (g == 91) && (b == 83));

				if ((r != 159) || (g != 91) || (b != 83))
					pTemp24Bit[j] = IRGB( 0, r, g, b);
				else
					pTemp24Bit[j] = IRGB( 255, r, g, b);
			}
			else
			{
				//pTemp24Bit[j].byFullBright = 0;

				pTemp24Bit[j] = IRGB( 0, r, g, b);
			}
		}

		pDoc->SetPalette (pbyPalette, 256, true);

		// Neal - fixes 24 bit image, load color palette, converts to 256 color bug
		//
		if (pDoc->GetOriginalBitDepth() <= 8)
		{
			pDoc->Convert24BitTo256Color( pTemp24Bit, 
					pDocBits, iWidth, iHeight, 0, GetDitherType(), FALSE);

			SetRawDibBits( 0, NULL, false);
			pDoc->CopyMipToLayer();

			if (g_bAutoRemip)
			{
				pDoc->RebuildSubMips();
				pDoc->UpdateSubMipDIBs();
			}
		}
		pDoc->SetModifiedFlag( TRUE);		
		UpdateColorPalette();
		FreeEffectsData();
		UpdateDIBs();

		if (pTemp24Bit)
		{
			free (pTemp24Bit);
			pTemp24Bit = NULL;
		}
	}

}

void CWallyView::OnColorsSavePalette() 
{
	CWallyDoc *pDoc = GetDocument();
	CWallyPalette *pPal = pDoc->GetPalette();

	CImageHelper ihHelper;
	CString strWildCard ("");
	
	strWildCard = ihHelper.GetSupportedPaletteList();
	CFileDialog dlgPalette (false, ".pal", NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Save Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension == 1 ? 1 : g_iOpenPaletteExtension - 1;	
	CString strSourceFilePath ("");
	CString strAddExtension("");	
	
	if (dlgPalette.DoModal() == IDOK)
	{	
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		strSourceFilePath         = dlgPalette.GetPathName();
		g_iOpenPaletteExtension = dlgPalette.m_ofn.nFilterIndex + 1;
				
		strAddExtension = GetWildCardExtension (strWildCard, g_iOpenPaletteExtension - 2);		
				
		int iExtensionMarker = dlgPalette.m_ofn.nFileExtension;
		int iFileNameLength = strSourceFilePath.GetLength();

		// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
		// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
		// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
		// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
		// We have to check for each instance and add the extension, as required.

		if ((strSourceFilePath.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
		{
			strSourceFilePath += ".";
		}

		if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
		{				
			// User didn't type in the extension			
			strSourceFilePath += strAddExtension;
		}		

		unsigned char byPalette[768];
		pPal->GetPalette (byPalette, 256);

		ihHelper.SaveImage (IH_8BIT, strSourceFilePath, NULL, byPalette, 0, 0);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());			
		}		
	}
	pDoc = NULL;
	pPal = NULL;
}

void CWallyView::OnColorsCountColorsUsed() 
{
	// Neal - TODO: update this to work for for 24-bit color

	BYTE byColors[256];
	memset( byColors, 0, 256);

	CWallyDoc* pDoc       = GetDocument();
	BYTE*      pbyDocBits = pDoc->GetBits();
	int        iColors    = 0;
	int        iSize      = pDoc->GetSize();
	int        j;

	for (j = 0; j < iSize; j++)
	{
		byColors[pbyDocBits[j]] = 1;
	}

	for (j = 0; j < 256; j++)
	{
		iColors += byColors[j];
	}

	CString strText ("");
	strText.Format ("There are %d unique colors in this texture.", iColors);
	AfxMessageBox (strText, MB_ICONINFORMATION);
}

void CWallyView::OnColorsTranslate() 
{
	CColorTranslatorDlg	dlgTranslate;	
	int j;
	unsigned char byPalette[768];

	CWallyDoc *pDoc = GetDocument();
	pDoc->GetPalette (byPalette, 256);

//	dlgTranslate.SetSourceIndex (g_iColorIndexLeft);
//	dlgTranslate.SetDestinationIndex (g_iColorIndexRight);
	dlgTranslate.SetSourceIndex( GetIValue( g_irgbColorLeft));
	dlgTranslate.SetDestinationIndex( GetIValue( g_irgbColorRight));
	dlgTranslate.SetPalette (byPalette);

	if (dlgTranslate.DoModal() == IDOK)
	{
		bool bSwapIndexes = dlgTranslate.SwapIndexes();
		pDoc->m_Undo.SaveUndoState( pDoc, "Translate Colors", bSwapIndexes ? UNDO_DOC_AND_PAL : UNDO_DOCUMENT);
		int iTranslateChoice = dlgTranslate.GetChoice();		
		int iDest = dlgTranslate.GetDestinationIndex();
		LPBYTE pDocBits = pDoc->GetBits(0);
		
		switch (iTranslateChoice)
		{
		case COLOR_TRANSLATE_BYINDEX:
			{				
				int iSource = dlgTranslate.GetSourceIndex();			
				
				if (iSource == iDest)
				{
					return;
				}			

				if (bSwapIndexes)
				{
					BYTE r1 = byPalette[iSource * 3 + 0];
					BYTE g1 = byPalette[iSource * 3 + 1];
					BYTE b1 = byPalette[iSource * 3 + 2];

					BYTE r2 = byPalette[iDest * 3 + 0];
					BYTE g2 = byPalette[iDest * 3 + 1];
					BYTE b2 = byPalette[iDest * 3 + 2];

					byPalette[iSource * 3 + 0] = r2;
					byPalette[iSource * 3 + 1] = g2;
					byPalette[iSource * 3 + 2] = b2;

					byPalette[iDest * 3 + 0] = r1;
					byPalette[iDest * 3 + 1] = g1;
					byPalette[iDest * 3 + 2] = b1;
				}
				
				int iSize = pDoc->GetSize();

				for (j = 0; j < iSize; j++)
				{					
					if (pDocBits[j] == iSource)
					{
						pDocBits[j] = iDest;
					}
					else
					{
						if ( (pDocBits[j] == iDest) && (bSwapIndexes) )
						{
							pDocBits[j] = iSource;
						}
					}
				}
				
			}
			break;

		case COLOR_TRANSLATE_BYCOLOR:
			{
				COLORREF rgb = dlgTranslate.GetRGB();
				int r, g, b;
				unsigned char *pDocBits = pDoc->GetBits(0);
				int iSize = pDoc->GetSize();
				int iFirstIndex = -1;

				r = GetRValue (rgb);
				g = GetGValue (rgb);
				b = GetBValue (rgb);

				for (j = 0; j < iSize; j++)
				{
					if (
						(byPalette[pDocBits[j] * 3 + 0] == r) &&
						(byPalette[pDocBits[j] * 3 + 1] == g) &&
						(byPalette[pDocBits[j] * 3 + 2] == b)
					   )
					{
						if (iFirstIndex == -1)
						{
							iFirstIndex = pDocBits[j];
						}
						pDocBits[j] = iDest;
					}
				}
				
				if (bSwapIndexes)
				{					
					for (j = 0; j < 256; j++)
					{
						if (
							(byPalette[j * 3 + 0] == r) &&
							(byPalette[j * 3 + 1] == g) &&
							(byPalette[j * 3 + 2] == b)
						   )						   
						{
							//memcpy (byPalette + (j * 3), byPalette + (iDest * 3), 3);
							byPalette[ (j * 3) + 0 ] = byPalette[ (iDest * 3) + 0 ];
							byPalette[ (j * 3) + 1 ] = byPalette[ (iDest * 3) + 1 ];
							byPalette[ (j * 3) + 2 ] = byPalette[ (iDest * 3) + 2 ];
						}						
					}

					byPalette[iDest * 3 + 0] = r;
					byPalette[iDest * 3 + 1] = g;
					byPalette[iDest * 3 + 2] = b;
				
					// Move iDest index to the first index
					for (j = 0; j < iSize; j++)
					{
						if (pDocBits[j] == iDest)
						{
							pDocBits[j] = iFirstIndex;
						}						
					}
				}

			}
			break;

		default:
			ASSERT (false);		// Unhandled option!
			break;
		}
		
		if (g_bAutoRemip)
		{
			pDoc->RebuildSubMips();
			pDoc->UpdateSubMipDIBs();
		}
		
		pDoc->CopyMipToLayer();		
		pDoc->SetModifiedFlag (true);

		// This must occur after CopyMipToLayer();
		if (bSwapIndexes)
		{
			pDoc->SetPalette (byPalette, 256, true);			
		}
		
		UpdateColorPalette();		
		FreeEffectsData();
		UpdateDIBs();
		
	}
}

void CWallyView::OnSelectionSelectAll() 
{
	CWallyDoc* pDoc = GetDocument();

	pDoc->m_Undo.SaveUndoState( pDoc, "Select All");
	pDoc->m_pSelection = pDoc->GetBase();
	pDoc->m_pSelection->SetBoundsRect( 0, 0, pDoc->Width(), pDoc->Height());

	pDoc->UpdateAllViews(NULL);
	pDoc->SetModifiedFlag();
}

void CWallyView::OnSelectionSelectNone() 
{
	GetDocument()->MergeSelectionIntoBase( "Select None");
}

void CWallyView::OnUpdateSelectionSelectNone( CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetDocument()->HasSelection());
}

void CWallyView::OnRepeatFilterPick0()
{
	g_Filter.DoFilter( this, FILTER_REPEAT_LAST_0, FALSE);
}
void CWallyView::OnRepeatFilterPick1()
{
	g_Filter.DoFilter( this, FILTER_REPEAT_LAST_1, FALSE);
}
void CWallyView::OnRepeatFilterPick2()
{
	g_Filter.DoFilter( this, FILTER_REPEAT_LAST_2, FALSE);
}
void CWallyView::OnRepeatFilterPick3()
{
	g_Filter.DoFilter( this, FILTER_REPEAT_LAST_3, FALSE);
}

void CWallyView::OnUpdateRepeatFilterPick0( CCmdUI* pCmdUI)
{
	pCmdUI->SetText( g_Filter.m_strRepeatLast[0]);
}
void CWallyView::OnUpdateRepeatFilterPick1( CCmdUI* pCmdUI)
{
	pCmdUI->SetText( g_Filter.m_strRepeatLast[1]);
}
void CWallyView::OnUpdateRepeatFilterPick2( CCmdUI* pCmdUI)
{
	pCmdUI->SetText( g_Filter.m_strRepeatLast[2]);
}
void CWallyView::OnUpdateRepeatFilterPick3( CCmdUI* pCmdUI)
{
	pCmdUI->SetText( g_Filter.m_strRepeatLast[3]);
}


void CWallyView::OnEditPasteSpecialEffects() 
{
#ifdef _DEBUG

	static BOOL bOnlyOnce = TRUE;

	if (bOnlyOnce)
	{
		bOnlyOnce = FALSE;

		AfxMessageBox( "Remember to remove auto menu delete from release\n"
						"builds when it is ready for public release !!!");
	}
#endif

	CPasteSpecialDlg Dlg;

	if (Dlg.DoModal() == IDOK)
	{
		// TODO: neal - handle the paste operation

		g_RivetToolLayerInfo.LoadFromClipboard( FALSE, this);
	}
}

void CWallyView::OnUpdateEditPasteSpecialEffects( CCmdUI* pCmdUI)
{
	// neal - TODO: remove auto menu delete from release builds
	//              when it is ready for public release

#define BETA_TEST 0

#if BETA_TEST
	pCmdUI->Enable( ::IsClipboardFormatAvailable( CF_DIB));
#else
	#ifdef _DEBUG

	pCmdUI->Enable( ::IsClipboardFormatAvailable( CF_DIB));

	#else

	if (pCmdUI->m_pMenu)
	{
		pCmdUI->m_pMenu->DeleteMenu( ID_EDIT_PASTE_SPECIALEFFECTS, MF_BYCOMMAND);
	}
	#endif
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Name:	OnMouseWheel
// Action:	Handles a mouse wheel event (zooms in/out)
/////////////////////////////////////////////////////////////////////////////
BOOL CWallyView::OnMouseWheel( UINT /*uFlags*/, short zDelta, CPoint /*pt*/)
{
	int iSign      = (m_iMouseWheelAggregate >= 0) ? 1 : -1;
	int iSignDelta = (zDelta >= 0) ? 1 : -1;

	if ((m_iMouseWheelAggregate == 0) || (iSign == iSignDelta))
	{
		// nw - accumulation test
		//#undef  WHEEL_DELTA
		//#define WHEEL_DELTA (120+40)

		m_iMouseWheelAggregate += zDelta;

		int iIncr = m_iMouseWheelAggregate / WHEEL_DELTA;

		if (iIncr != 0)
		{
			m_iMouseWheelFactor    += iIncr;
			m_iMouseWheelAggregate -= iIncr * WHEEL_DELTA;

			if (iIncr > 0)
			{
				OnEditZoomIn();
			}
			else //if (iIncr < 0)
			{
				OnEditZoomOut();
			}
		}
	}
	else
	{
		m_iMouseWheelAggregate = 0;

//		TRACE0( "MouseWheel: direction change\n");
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name:	OnRegisteredMouseWheel
// Action:	Handles a mouse wheel event (for Win95/98)
/////////////////////////////////////////////////////////////////////////////
/*
// This event handler was deprecated
LRESULT CWallyView::OnRegisteredMouseWheel(WPARAM wParam, LPARAM lParam)
{
	return CWnd::OnRegisteredMouseWheel( wParam, lParam);

}
*/

void CWallyView::OnImageTiledTint() 
{
	g_Filter.DoFilter( this, FILTER_TILED_TINT, TRUE );	
}

void CWallyView::OnImageFixUnevenLighting() 
{
	g_Filter.DoFilter( this, FILTER_FIX_UNEVEN_LIGHTING, TRUE );	
}
