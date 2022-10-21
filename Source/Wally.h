// Wally.h : main header file for the WALLY application

#ifndef _WALLY_H_
#define _WALLY_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
//#include "MiscFunctions.h"

class CParseCommandLine;
/////////////////////////////////////////////////////////////////////////////
// CWallyApp:
// See Wally.cpp for the implementation of this class
//

#define COLOR_DEBUG_PURPLE	(IRGB( 255, 255, 0, 255))

// used by CWallyView to regenerate the view/data
#define HINT_UPDATE_STANDARD	0
#define HINT_UPDATE_TOOL_DIBS	1
#define HINT_UPDATE_DIBS		2
#define HINT_UPDATE_PAL			3
#define HINT_UPDATE_IMAGE_SIZE	4

// neal - added g_iCurrentTool constants
#define EDIT_MODE_SELECTION		0
#define EDIT_MODE_ZOOM			1
#define EDIT_MODE_EYEDROPPER	2

#define DRAWING_TOOLS_START				100
#define FREEHAND_TOOLS_START			DRAWING_TOOLS_START

#define EDIT_MODE_PENCIL				(DRAWING_TOOLS_START + 0)

#define CURSOR_TOOLS_START				(DRAWING_TOOLS_START + 10)

#define EDIT_MODE_BRUSH					(CURSOR_TOOLS_START + 0)
#define EDIT_MODE_ERASER				(CURSOR_TOOLS_START + 1)
#define EDIT_MODE_REPLACE				(CURSOR_TOOLS_START + 2)
#define EDIT_MODE_RECOLOR				(CURSOR_TOOLS_START + 3)
#define EDIT_MODE_SPRAY					(CURSOR_TOOLS_START + 4)
#define EDIT_MODE_SPRAY_RECOLOR			(CURSOR_TOOLS_START + 5)
#define EDIT_MODE_TINT					(CURSOR_TOOLS_START + 6)

#define EFFECT_TOOLS_START				(DRAWING_TOOLS_START + 100)

#define EDIT_MODE_PATTERNED_PAINT		(EFFECT_TOOLS_START + 0)
#define EDIT_MODE_LIGHTEN				(EFFECT_TOOLS_START + 1)
#define EDIT_MODE_DARKEN				(EFFECT_TOOLS_START + 2)
#define EDIT_MODE_BLEND					(EFFECT_TOOLS_START + 3)
#define EDIT_MODE_SHARPEN				(EFFECT_TOOLS_START + 4)
#define EDIT_MODE_CLONE					(EFFECT_TOOLS_START + 5)
#define EDIT_MODE_RUBBER_STAMP			(EFFECT_TOOLS_START + 6)
//#define EDIT_MODE_REPLACE				(EFFECT_TOOLS_START + 8)
//#define EDIT_MODE_RECOLOR				(EFFECT_TOOLS_START + 9)
//#define EDIT_MODE_SPRAY				(EFFECT_TOOLS_START + 10)
//#define EDIT_MODE_SPRAY_RECOLOR		(EFFECT_TOOLS_START + 11)

//#define CURSOR_TOOLS_END				EDIT_MODE_RECOLOR
#define CURSOR_TOOLS_END				EDIT_MODE_RUBBER_STAMP

#define EDIT_MODE_DECAL					(EFFECT_TOOLS_START + 10)
#define EDIT_MODE_SCRATCH				(EFFECT_TOOLS_START + 11)

#define FREEHAND_TOOLS_END				EDIT_MODE_SCRATCH

#define EDIT_MODE_DENT					(EFFECT_TOOLS_START + 12)
#define EDIT_MODE_BUMP					(EFFECT_TOOLS_START + 13)
#define EDIT_MODE_DRIP					(EFFECT_TOOLS_START + 14)
#define EDIT_MODE_BULLET_HOLES			(EFFECT_TOOLS_START + 15)
#define EDIT_MODE_RIVETS				(EFFECT_TOOLS_START + 16)

#define EFFECT_TOOLS_END				(EDIT_MODE_RIVETS)
#define SHAPE_TOOLS_START				(DRAWING_TOOLS_START + 200)

#define EDIT_MODE_FLOOD_FILL			(SHAPE_TOOLS_START + 0)

#define SHAPE_TOOLS_END					EDIT_MODE_FLOOD_FILL
#define DRAWING_TOOLS_END				EDIT_MODE_FLOOD_FILL


#define PALETTE_CONVERT_MAINTAIN 0    // Only valid for 8-bit images
#define PALETTE_CONVERT_NEAREST  1    // Nearest color

#define SHAPE_SQUARE   501
#define SHAPE_CIRCULAR 502
#define SHAPE_DIAMOND  503

// Texture rotate defines

#define ROTATE_LEFT   0
#define ROTATE_RIGHT  1
#define ROTATE_90	  0
#define ROTATE_180	  1
#define ROTATE_270	  2

// Texture directory settings
#define BLANK_DIR	0
#define PRESET_DIR	1
#define PARENT_DIR	2

#define MAX_ZOOM				16
#define MIN_ZOOM				(-16)
#define MAX_CONVERSION_THREADS	99

#define MAX_TEXTURE_WIDTH		2048 //512
#define MAX_TEXTURE_HEIGHT		2048 //512


// File Types
#define	FILE_TYPE_PROMPT		-1
#define	FILE_TYPE_START			0
#define	FILE_TYPE_QUAKE2		(FILE_TYPE_START + 1)
#define	FILE_TYPE_QUAKE1		(FILE_TYPE_START + 2)
#define	FILE_TYPE_QUAKE1_WAD	(FILE_TYPE_START + 3)
#define	FILE_TYPE_SIN			(FILE_TYPE_START + 4)
#define	FILE_TYPE_HERETIC2		(FILE_TYPE_START + 5)
#define FILE_TYPE_HALF_LIFE		(FILE_TYPE_START + 6)
#define FILE_TYPE_HALF_LIFE_WAD	(FILE_TYPE_START + 7)
#define	FILE_TYPE_PAK			(FILE_TYPE_START + 8)
#define	FILE_TYPE_TGA			(FILE_TYPE_START + 9)
#define	FILE_TYPE_BMP			(FILE_TYPE_START + 10)
#define	FILE_TYPE_PCX			(FILE_TYPE_START + 11)
#define	FILE_TYPE_PNG			(FILE_TYPE_START + 12)
#define	FILE_TYPE_JPG			(FILE_TYPE_START + 13)
#define	FILE_TYPE_TEX			(FILE_TYPE_START + 14)
#define	FILE_TYPE_END			(FILE_TYPE_START + 15)


/////////////////////////////////////////////////////////////////////////////
class CWallyPalette;
class CPackageDoc;
class CBrowseDocument;
class CBrowseDoc;

extern CWallyPalette	g_CurrentPalette;

#define ALLOW_ANY_SIZE			0x00	// used by g_flgAllowableSizes
#define	FORCE_X_16				0x01
#define FORCE_X_POWER_OF_2		0x02

#define MODE_FREEHAND			1001
#define MODE_LINE				1002
#define MODE_POLYLINE			1003
#define MODE_RAY				1004
#define MODE_CURVE				1005
#define MODE_POLYCURVE			1006
#define MODE_RAY_CURVE			1007
#define MODE_SOLID_RECT			1101
#define MODE_HOLLOW_RECT		1102
#define MODE_EDGED_RECT			1103
#define MODE_SOLID_ELLIPSE		1104
#define MODE_HOLLOW_ELLIPSE		1105
#define MODE_EDGED_ELLIPSE		1106
#define MODE_SOLID_POLYGON		1107
#define MODE_HOLLOW_POLYGON		1108
#define MODE_EDGED_POLYGON		1109

#define	COLOR_RED	0
#define COLOR_GREEN	1
#define COLOR_BLUE	2

#define COLOR_RGB  COLORREF  // use RGB(), GetRValue(), etc.
#define COLOR_IRGB COLORREF  // use RGB(), GetRValue(), etc.
#define LPCOLOR_RGB		COLOR_RGB *
#define LPCOLOR_IRGB	COLOR_IRGB *

inline COLORREF IRGB( int i, int r, int g, int b)
{
	return ((COLORREF) ((((BYTE)(r) | ((WORD)((BYTE)(g))<<8)) | 
		(((DWORD)(BYTE)(b))<<16))) | (((DWORD)(BYTE)(i))<<24));
}
#define GetIValue(irgb)			((BYTE)((irgb) >> 24))
#define SetIValue(irgb, i)		(irgb = ( (irgb | (0xff << 24) ) ^ (0xff << 24) ) | (i << 24) )
#define SetRValue(irgb, r)		(irgb = ( (irgb | (0xff) ) ^ (0xff) ) | r )
#define SetGValue(irgb, g)		(irgb = ( (irgb | (0xff << 8) ) ^ (0xff << 8) ) | (g << 8) )
#define SetBValue(irgb, b)		(irgb = ( (irgb | (0xff << 16) ) ^ (0xff << 16) ) | (b << 16) )

#define IsKeyPressed( vk) (GetKeyState( vk) < 0)

/////////////////////////////////////////////////////////////////////////////
// Name:	ClampColorComponent
// Action:	Clamps a single R-G-B component to a range of 0..255
/////////////////////////////////////////////////////////////////////////////
inline void ClampColorComponent( int* piComponent)
{
	*piComponent = min( *piComponent, 255);
	*piComponent = max( *piComponent, 0);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	ClampRGB
// Action:	Clamps each R-G-B component to a range of 0..255
//			It replaces the old LimitRange() function
/////////////////////////////////////////////////////////////////////////////
inline void ClampRGB( int* pR, int* pG, int* pB)
{
	ClampColorComponent( pR);
	ClampColorComponent( pG);
	ClampColorComponent( pB);
}


// Tool related stuff
extern int	g_iCurrentTool;
extern int	g_iDrawingMode;

extern int IndexToBrushSize( int iIndex);
extern int IndexToBrushSizeId( int iIndex);
extern int BrushSizeIdToIndex( int iSize);
extern int BrushSizeToIndex( int iSize);
extern int IndexToBrushShape( int iIndex);
extern int IndexToBrushShapeId( int iIndex);
extern int BrushShapeToIndex( int iShape);
extern int BrushShapeIdToIndex( int iShape);
extern int IndexToToolAmountId( int iIndex);
extern int ToolToAmountIndex( int iToolEditMode);
extern int ToolAmountToPercent( int iToolEditMode);


inline BOOL IsDrawingTool( int iTool)
{
	return ((iTool >= DRAWING_TOOLS_START) && (iTool <= DRAWING_TOOLS_END));
}

inline BOOL IsFreehandTool( int iTool)
{
	return ((iTool >= FREEHAND_TOOLS_START) && (iTool <= FREEHAND_TOOLS_END));
}

inline BOOL ToolHasConstrain( int iTool)
{
	return (IsFreehandTool( iTool) || (iTool == EDIT_MODE_SELECTION));
}

//inline BOOL IsLineTool( int iTool)
//{
//	return (IsFreehandTool( iTool) && (iTool != EDIT_MODE_ERASER));
//}
//
//inline BOOL IsRectTool( int iTool)
//{
//	return (IsLineTool( iTool) || (iTool == EDIT_MODE_SELECTION));
//}

inline BOOL ToolHasCursor( int iTool)
{
	return ((iTool >= CURSOR_TOOLS_START) && (iTool <= CURSOR_TOOLS_END));
}

inline BOOL IsTintEffect( int iTool)
{
	return ((iTool == EDIT_MODE_TINT) ||
			(iTool == EDIT_MODE_SPRAY_RECOLOR) || (iTool == EDIT_MODE_RECOLOR));
}

// uses effects buffer?
inline BOOL IsBufferEffect( int iTool)
{
	return (IsTintEffect( iTool) || 
			((iTool >= EFFECT_TOOLS_START) && (iTool <= EFFECT_TOOLS_END)));
}

inline BOOL IsAnyEffect( int iTool)
{
//	return (IsBufferEffect( iTool) || Is_Future_Effect( iTool));
	return (IsBufferEffect( iTool));
}

inline BOOL ToolHasWidth( int iTool)
{
	return ((ToolHasCursor( iTool) && (iTool != EDIT_MODE_ERASER)) ||
			(iTool == EDIT_MODE_RIVETS) || (iTool == EDIT_MODE_SELECTION));
}

inline BOOL UsesColors( int iTool)
{
	return (((iTool >= DRAWING_TOOLS_START) && (iTool <= EDIT_MODE_PATTERNED_PAINT)) ||
			((iTool >= EDIT_MODE_DRIP) && (iTool <= EDIT_MODE_RIVETS)) ||
			(iTool == EDIT_MODE_SELECTION) || (iTool == EDIT_MODE_FLOOD_FILL) || (iTool == EDIT_MODE_EYEDROPPER));
}

inline int GetToolWidth( void)
{
	int iDrawingWidth = ToolHasWidth( g_iCurrentTool) ? g_iDrawingWidth : 1;

	if ((g_iCurrentTool == EDIT_MODE_PENCIL) || 
				(g_iDrawingMode == MODE_SOLID_RECT))
	{
		if ((g_iCurrentTool == EDIT_MODE_CLONE) || (g_iCurrentTool == EDIT_MODE_RUBBER_STAMP))
			iDrawingWidth = 5;
		else
			iDrawingWidth = 1;
	}
	else if (g_iCurrentTool == EDIT_MODE_ERASER)
	{
		iDrawingWidth = 16;
	}
	else if (g_iCurrentTool == EDIT_MODE_RIVETS)
	{
		iDrawingWidth = g_iRivetDistance;
	}

	return iDrawingWidth;
}
// same as above, but assume freehand drawing mode
inline int GetFreehandToolWidth( void)
{
	int iDrawingWidth = ToolHasWidth( g_iCurrentTool) ? g_iDrawingWidth : 1;

	if (g_iCurrentTool == EDIT_MODE_SELECTION) 
	{
		iDrawingWidth = g_bPasteInvisibleBackground;
	}
	else if (g_iCurrentTool == EDIT_MODE_PENCIL) 
	{
		iDrawingWidth = 1;
	}
	else if (g_iCurrentTool == EDIT_MODE_ERASER)
	{
		iDrawingWidth = 16;
	}
	else if (g_iCurrentTool == EDIT_MODE_RIVETS)
	{
		iDrawingWidth = g_iRivetDistance;
	}

	return iDrawingWidth;
}

// does this tool have any drawing modes at all?
inline BOOL HasDrawingModes( int iTool)
{
	return ((iTool >= FREEHAND_TOOLS_START) && (iTool <= FREEHAND_TOOLS_END));
}

inline BOOL IsLineMode( int iMode)
{
	return ((iMode >= MODE_LINE) && (iMode <= MODE_RAY));
}

inline BOOL IsRectMode( int iMode)
{
	return ((iMode >= MODE_SOLID_RECT) && (iMode <= MODE_EDGED_RECT));
}

inline BOOL IsFilledMode( int iMode)
{
	return ((iMode == MODE_SOLID_RECT) || (iMode == MODE_EDGED_RECT));
}

// does this tool offer a particular drawing mode?
inline BOOL HasDrawingMode( int iTool, int iMode)
{
	BOOL bRet = HasDrawingModes( iTool);

	if ((iTool == EDIT_MODE_SELECTION))
	{
		bRet = (iMode == MODE_HOLLOW_RECT);
	}
	else if (iTool == EDIT_MODE_DECAL)
	{
		bRet = (! IsFilledMode( iMode));
	}
	else if ((iTool == EDIT_MODE_ERASER) || (iTool == EDIT_MODE_BULLET_HOLES) ||
				(iTool == EDIT_MODE_RIVETS))
	{
		bRet = (iMode == MODE_FREEHAND);
	}
	return bRet;
}

inline int GetDrawingMode( int iTool)
{
	int iMode = (HasDrawingModes( iTool) ? 
			g_iDrawingMode : -1);

	if ((iTool == EDIT_MODE_SELECTION))
		iMode = MODE_HOLLOW_RECT;
	else if ((iTool == EDIT_MODE_ERASER) || (iTool == EDIT_MODE_BULLET_HOLES) ||
				(iTool == EDIT_MODE_RIVETS))
		iMode = MODE_FREEHAND;
	else if ((iTool == EDIT_MODE_DECAL) && IsFilledMode( iMode))
	{
		if (IsRectMode( iMode))
			iMode = MODE_HOLLOW_RECT;	// override (no filled decal modes)
		else
		{
			ASSERT( FALSE);				// do we have some new filled modes?
			iMode = MODE_FREEHAND;		// override
		}
	}

	return iMode;
}

inline BOOL ToolHasAmount( int i)
{
	return (((i >= EDIT_MODE_TINT) && (i <= EDIT_MODE_RUBBER_STAMP)) ||
			((i >= EDIT_MODE_DENT) && (i <= EDIT_MODE_RIVETS)) ||
			(i == EDIT_MODE_SELECTION) || 
			(i == EDIT_MODE_SPRAY) || 
			(i == EDIT_MODE_SPRAY_RECOLOR) ||
			(i == EDIT_MODE_SCRATCH) || 
			(i == EDIT_MODE_DECAL)
			);
}

inline BOOL ToolHasShape( int i)
{
	return ((i >= CURSOR_TOOLS_START) && (i <= CURSOR_TOOLS_END));
}

inline BOOL IsDecalTool( int i)
{
	return ((i == EDIT_MODE_BULLET_HOLES) || (i == EDIT_MODE_RIVETS) ||
			(i == EDIT_MODE_PATTERNED_PAINT) || (i == EDIT_MODE_DECAL));
}

BOOL IsPowerOf2( int iVal);
BOOL IsValidImageSize( int iWidth, int iHeight, DWORD flgAllowableSizes = g_flgAllowableSizes);

inline BOOL ShowSubMips( void)  { return g_bShowSubMips; };


//extern void LzrwCompress( const BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len);
//extern void LzrwDecompress( BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len);
HGLOBAL CompressMemory( HGLOBAL* phSrc, DWORD dwOriginalSize, DWORD* pdwSize, BOOL bFreeSrc);
HGLOBAL DecompressMemory( HGLOBAL* phCompressedSrc, BOOL bFreeSrc);

BOOL LoadDefaultEditingPalette( LPBYTE pbyPalette, CWallyPalette *pPalette, int iNumColors);

typedef double	Vec[3];

class CWallyApp : public CWinApp
{
public:
	CWallyApp();
	void LoadPalette (int iGameType);
	void OpenNonWalFile (LPCTSTR szFileName);
	
	CMultiDocTemplate* WallyDocTemplate;
	CMultiDocTemplate* BrowseDocTemplate;
	CMultiDocTemplate* PackageDocTemplate;
	CMultiDocTemplate* BuildDocTemplate;
	CMultiDocTemplate* WLYDocTemplate;
	CMultiDocTemplate* PakDocTemplate;
	CMultiDocTemplate* DebugDibTemplate;
	
	CBrowseDoc *m_pBrowseDoc;
	BOOL m_bBrowseOpen;
	
	//CParseCommandLine CommandLineParser;

	double GetProfileDouble( LPCSTR lpszSection, LPCSTR lpszEntry, double dfDefaultValue);
	BOOL   WriteProfileDouble( LPCSTR lpszSection, LPCSTR lpszEntry, double dfValue);

public:
	void OnProperties();
	void UpdateAllDocs( LPARAM lHint = HINT_UPDATE_PAL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void SetProgressBar (int iPosition);
	void SetProgressBarInfo (int iRangeLow, int iRangeHigh, int iStep);
	void StepProgressBar (int iAmount = 1);
	CView *GetActiveView();	
	CPackageDoc *CreatePackageDoc ();
	void OpenImage (LPCTSTR szFileName);
	
	BOOL GetBackgroundWindowPlacement (LPWINDOWPLACEMENT lpWindow);	
	
// Members
public:	
	RGBQUAD current_palette[768];
	UINT    m_nClipboardFormat;
	BOOL    m_bProgressCreated;	

private:	
	void ReturnErrorOnFileNew();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyApp)
	public:
	virtual BOOL InitInstance();
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWallyApp)
	afx_msg void OnAppAbout();	
	afx_msg void OnEditPasteAsNewImage();
	afx_msg void OnUpdateEditPasteAsNewImage(CCmdUI* pCmdUI);
	afx_msg void OnViewOptions();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnFileBrowse();
	afx_msg void OnFileConvert();
	afx_msg void OnFileBatchConversion();
	afx_msg void OnFileLoadQ2palette();
	afx_msg void OnFileLoadQ1palette();
	afx_msg void OnEditPasteSpecialEffects();
	afx_msg void OnUpdateEditPasteSpecialEffects(CCmdUI* pCmdUI);
	afx_msg void OnViewRules();
	afx_msg void OnFileTestTxl();
	afx_msg void OnUpdateFileTestTxl(CCmdUI* pCmdUI);
	afx_msg void OnFileDebug();
	afx_msg void OnUpdateFileDebug(CCmdUI* pCmdUI);
	afx_msg void OnWizardColorDecal();
	afx_msg void OnWizardWadmerge();
	afx_msg void OnWizardTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WALLY_H__C80E39C5_8F31_11D1_8068_5C2203C10627__INCLUDED_)
