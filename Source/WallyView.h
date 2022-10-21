/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyView.h : interface of the CWallyView class
//
// Created by Ty Matthews, 1-1-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _WALLYVIEW_H_
#define _WALLYVIEW_H_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define MOUSE_MOVE			0
#define MOUSE_BUTTON_DOWN	1
#define MOUSE_BUTTON_UP		2
#define TIMER_MSG			MOUSE_MOVE

#include "DibSection.h"

class CWallyDoc;

//DWORD __stdcall ThreadFloodFill( LPVOID pvoidWallyView);

class CWallyView : public CScrollView
{
protected: // create from serialization only
	CWallyView();
	DECLARE_DYNCREATE(CWallyView)

// Attributes
public:
	CWallyDoc* GetDocument();

// Members
public:
	BOOL m_bTile;
	BOOL m_bLeftButtonTracking;
	BOOL m_bRightButtonTracking;	
	BOOL m_bCDCisSet;
	BOOL m_bNeedToEraseBkgnd;

	int m_iLastTool;
	int m_iLastLeftColor;
	int m_iLastRightColor;


	CDibSection m_DIB[4];
	CPoint      m_ptWrapOffset[4];
	BOOL		m_bHasSubMips;
	BOOL		m_bUnknownType;
		
	int    m_iZoomValue;	// Zoom Out: 2,3,4,    Zoom In: -2,-3,-4,  100% = 1
	double m_dfZoom;		// Zoom Out: 2.0, 4.0, Zoom In: 0.5, 0.25, 100% = 1.0
	
	CSize  m_DocSize;
	CSize  m_TrueDocSize;
	CPoint m_CurrentPosition;
//	CPoint m_ptFlood;
//	CPoint m_ptLastBigCursorPos;
//	int    m_iLastBigCursorSize;
	
	CPoint m_ptLastMouseDown;		// in client coords!
	CPoint m_ptLastMouseUp;			// in client coords!
	BOOL   m_bLastMouseDownIsValid;
	BOOL   m_bLastMouseUpIsValid;
	int    m_iMouseWheelFactor;
	int    m_iMouseWheelAggregate;
	int    m_iShiftCountLeftRight;
	int    m_iShiftCountUpDown;

	CImageList  m_imgList;

// Operations
public:
	void SetScrollBars( BOOL bInitialUpdate = FALSE, BOOL bUseLockWindowUpdate = TRUE);
	void SetZoom( BOOL bPlaySound);
	void SetZoomFactor( BOOL bZoomIn);
	SIZE CalcDocSize( LPRECT lpRect, double dfZoom, int iGap);
	void GetDocSize( LPRECT lpRect = NULL);
	void GetTrueImageCoordinates( CPoint* ImagePoint);
	void PlotPixel( int iImageX, int iImageY, int ColorIndex);
	void UpdateDIBs( void);
//	void UpdateConstrainPaneText( void);
	void FreeEffectsData( void);
	void RebuildImageViews( void);
	int GetZoomValue() { return m_iZoomValue; };
	void SetZoomValue(int iZoomValue);
	BOOL ShowSubMips()
	{
		if ((m_bHasSubMips) && (g_bShowSubMips))
		{
			return TRUE;
		}
		return FALSE;
	};
	void SetHasSubMips(BOOL bHasSubMips = TRUE) { m_bHasSubMips = bHasSubMips; };

//	void InitTool( int iToolMode);
//	void UseTool( UINT nFlags, CPoint ptPos, int iMouseCode);
//	void DrawBigCursor( CPoint ptImage, int iSize);
//	void ClearBigCursor( void);
	
//	void DrawSpot( int iImageX, int iImageY, int iSize, int iColor, BOOL bHollow);
//	void SpraySpot( int iImageX, int iImageY, int iSize, int iColor, int iCoverage);
//	void ReplaceSpot( int iImageX, int iImageY, int iSize, int iColor, int iReplaceColor, BOOL bLeftButton, BOOL bHollow);
//	void PatternedSpot( int iImageX, int iImageY, int iSize, int iColor1, int iColor2, BOOL bHollow);
//	void DecalSpot( int iImageX, int iImageY, int iSize);
//	BOOL DrawBulletHole( int iImageX, int iImageY, int iMouseCode);
//	BOOL DrawRivet( int iImageX, int iImageY, int iMouseCode);

//	void DrawStroke( CPoint ptImage1, CPoint ptImage2, int iSize, BOOL bLeftButton);
//	void DrawLine( CPoint ptImage1, CPoint ptImage2, int iSize, int iColor1, int iColor2, BOOL bLeftButton);
//	void DrawSolidRectangle( CPoint ptMin, CPoint ptMax, int iColor1, int iColor2, BOOL bLeftButton);
//	void PaintLine( CPoint ptImage1, CPoint ptImage2, int iSize, int iColor);
//	void SprayLine( CPoint ptImage1, CPoint ptImage2, int iSize, int iColor, int iReplaceColor);
//	void ReplaceLine( CPoint ptImage1, CPoint ptImage2, int iSize, int iColor, int iReplaceColor, BOOL bLeftButton);
//	void PatternedLine( CPoint ptImage1, CPoint ptImage2, int iSize, int iColor1, int iColor2);
//	void DecalLine( CPoint ptImage1, CPoint ptImage2, int iSize);

//	void InternalFloodFill( int iX, int iY, int iDepth);
//	void FloodFill( CPoint ptImage, int iInteriorColor, int iColor);
	void DrawingModeRestart( void);
//	void ShowToolInfo( CPoint ptPos);

	void ShiftImage (int iDirection);

//	LPCSTR GetToolName( int iEditMode);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

protected:
	void DrawGrid( CDC* pDC, const CRect& rClient, const CPoint& ptScrollPos);

// Implementation
public:
	afx_msg void OnUpdateEditCopyCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditZoomIn();
	afx_msg void OnEditZoomOut();
//	afx_msg void OnEditShiftUp();
//	afx_msg void OnEditShiftDown();
//	afx_msg void OnEditShiftLeft();
//	afx_msg void OnEditShiftRight();
	BOOL IsSelected();
	void DoCutCopyPaste(CArchive &ar, BOOL bCut);
	UINT m_nClipboardFormat;
	virtual ~CWallyView();

	void UpdateColorPalette();

	// pbyInitializationPalette is "packed" and is used a flag
	// that indicates if the DIB needs to be initialized
	void SetRawDibBits( int iDibIndex, BYTE* pbyInitializationPalette, bool bOnlyToolDirtyArea);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
//	int  m_iFloodColor;		// saves stack space
//	int  m_iFloodInterior;
	BOOL m_bInitialized;
	BOOL m_bFirstPaint;
	BOOL m_bSettingScrollBars;	

// Generated message map functions
public:	
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CWallyView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTpTile();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTpAnimate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPopupCopyAsBitmap();	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateTpTile(CCmdUI* pCmdUI);
	afx_msg void OnTpRemip();
	afx_msg void OnEditClear();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTpZoom();
	afx_msg void OnUpdateTpZoom(CCmdUI* pCmdUI);
	afx_msg void OnTpSelection();
	afx_msg void OnTpDropper();
	afx_msg void OnUpdateTpDropper(CCmdUI* pCmdUI);
	afx_msg void OnTpPencil();
	afx_msg void OnUpdateTpPencil(CCmdUI* pCmdUI);
	afx_msg void OnTpLighten();
	afx_msg void OnUpdateTpLighten(CCmdUI* pCmdUI);
	afx_msg void OnTpEraser();
	afx_msg void OnUpdateTpEraser(CCmdUI* pCmdUI);
	afx_msg void OnTpDarken();
	afx_msg void OnUpdateTpDarken(CCmdUI* pCmdUI);
	afx_msg void OnTpBrush();
	afx_msg void OnUpdateTpBrush(CCmdUI* pCmdUI);
	afx_msg void OnTpScratch();
	afx_msg void OnUpdateTpScratch(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnTpBulletHoles();
	afx_msg void OnUpdateTpBulletHoles(CCmdUI* pCmdUI);
	afx_msg void OnTpRivets();
	afx_msg void OnUpdateTpRivets(CCmdUI* pCmdUI);
	afx_msg void OnTpAmount();
	afx_msg void OnUpdateTpAmount(CCmdUI* pCmdUI);
	afx_msg void OnTpWidth();
	afx_msg void OnUpdateTpWidth(CCmdUI* pCmdUI);
	afx_msg void OnTpTint();
	afx_msg void OnUpdateTpTint(CCmdUI* pCmdUI);
	afx_msg void OnTpShape();
	afx_msg void OnUpdateTpShape(CCmdUI* pCmdUI);
	afx_msg void OnTpReplace();
	afx_msg void OnUpdateTpReplace(CCmdUI* pCmdUI);
	afx_msg void OnTpClone();
	afx_msg void OnUpdateTpClone(CCmdUI* pCmdUI);
	afx_msg void OnTpRubberStamp();
	afx_msg void OnUpdateTpRubberStamp(CCmdUI* pCmdUI);
	afx_msg void OnTpSpray();
	afx_msg void OnUpdateTpSpray(CCmdUI* pCmdUI);
	afx_msg void OnTpFloodFill();
	afx_msg void OnUpdateTpFloodFill(CCmdUI* pCmdUI);
	afx_msg void OnEditCopyTiled();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTpPatternedPaint();
	afx_msg void OnUpdateTpPatternedPaint(CCmdUI* pCmdUI);
	afx_msg void OnImageMirror();
	afx_msg void OnImageRotate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTpRecolor();
	afx_msg void OnUpdateTpRecolor(CCmdUI* pCmdUI);
	afx_msg void OnTpSprayRecolor();
	afx_msg void OnUpdateTpSprayRecolor(CCmdUI* pCmdUI);
	afx_msg void OnCancelMode();
	afx_msg void OnDrawingModeFreehand();
	afx_msg void OnDrawingModeLine();
	afx_msg void OnDrawingModePolyline();
	afx_msg void OnDrawingModeRay();
	afx_msg void OnUpdateDrawingModeFreehand(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawingModeLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawingModePolyline(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawingModeRay(CCmdUI* pCmdUI);
	afx_msg void OnDrawingModeHollowRect();
	afx_msg void OnUpdateDrawingModeHollowRect(CCmdUI* pCmdUI);
	afx_msg void OnDrawingModeSolidRect();
	afx_msg void OnUpdateDrawingModeSolidRect(CCmdUI* pCmdUI);
	afx_msg void OnDrawingModeEdgedRect();
	afx_msg void OnUpdateDrawingModeEdgedRect(CCmdUI* pCmdUI);
	afx_msg void OnImageBrightnessContrast();
	afx_msg void OnImageBlend();
	afx_msg void OnImageAddNoise();
	afx_msg void OnImageEmboss();
	afx_msg void OnImageEdgeDetect();
	afx_msg void OnImageGenrateMarbleTexture();
	afx_msg void OnImageConvertTexturesQuakeToFromUnreal();
	afx_msg void OnImageGenerateMarbleTileTexture();
	afx_msg void OnViewOptions();
	afx_msg void OnFreeUndoMemory();
	afx_msg void OnImageDiffuse();
	afx_msg void OnImageFlipHorz();
	afx_msg void OnImageFlipVert();	
	afx_msg void OnImageOffset();
	afx_msg void OnImageSharpen();
	afx_msg void OnColorsEditPalette();
	afx_msg void OnColorsLoadPalette();
	afx_msg void OnColorsSavePalette();
	afx_msg void OnColorsCountColorsUsed();
	afx_msg void OnColorsTranslate();
	afx_msg void OnImageResize();
	afx_msg void OnUpdateTpSelection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageRotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageResize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImageOffset(CCmdUI* pCmdUI);
	afx_msg void OnSelectionSelectAll();
	afx_msg void OnSelectionSelectNone();
	afx_msg void OnUpdateSelectionSelectNone(CCmdUI* pCmdUI);
	afx_msg void OnImageReduceColors();
	afx_msg void OnUpdateImageReduceColors(CCmdUI* pCmdUI);
	afx_msg void OnRepeatFilterPick0();
	afx_msg void OnUpdateRepeatFilterPick0(CCmdUI* pCmdUI);
	afx_msg void OnRepeatFilterPick1();
	afx_msg void OnUpdateRepeatFilterPick1(CCmdUI* pCmdUI);
	afx_msg void OnRepeatFilterPick2();
	afx_msg void OnUpdateRepeatFilterPick2(CCmdUI* pCmdUI);
	afx_msg void OnRepeatFilterPick3();
	afx_msg void OnUpdateRepeatFilterPick3(CCmdUI* pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditPasteSpecialEffects();
	afx_msg void OnUpdateEditPasteSpecialEffects(CCmdUI* pCmdUI);
	afx_msg void OnTpDecal();
	afx_msg void OnUpdateTpDecal(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnRegisteredMouseWheel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTpBlend();
	afx_msg void OnUpdateTpBlend(CCmdUI* pCmdUI);
	afx_msg void OnImageEnlarge();
	afx_msg void OnImageTiledTint();
	afx_msg void OnImageFixUnevenLighting();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WallyView.cpp
inline CWallyDoc* CWallyView::GetDocument()
   { return (CWallyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif		// _WALLYVIEW_H_
