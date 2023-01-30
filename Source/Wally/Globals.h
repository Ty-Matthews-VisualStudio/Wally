#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals.h : header file
//
// Implementation file for global variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define Globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TIMER_START						0
#define TIMER_ETV_ID					( TIMER_START + 1 )
#define TIMER_PB_ID_ANIMATE				( TIMER_START + 2 )
#define TIMER_PV_ID_LISTBOX				( TIMER_START + 3 )
#define TIMER_PV_ID_LISTBOX_SELECTION	( TIMER_START + 4 )
#define TIMER_PV_ID_FILTER				( TIMER_START + 5 )
#define TIMER_SPLASH_ID					( TIMER_START + 6 )
#define TIMER_TOOL_CLEAR_CURSOR			( TIMER_START + 7 )
#define TIMER_TOOL_SPRAY				( TIMER_START + 8 )
#define TIMER_TOOL_AUTO_REMIP			( TIMER_START + 9 )
#define TIMER_WMS_THREAD				( TIMER_START + 10 )
#define TIMER_WLY_FLICKER				( TIMER_START + 11 )
#define TIMER_WV_ANIMATE_SELECTION		( TIMER_START + 12 )
#define TIMER_WV_REMIP					( TIMER_START + 13 )
#define TIMER_WPP_CHANGE_GAMMA			( TIMER_START + 14 )
#define TIMER_PV_ID_TREECTRL			( TIMER_START + 15 )
#define TIMER_PV_ID_TREECTRL_SELECTION	( TIMER_START + 16 )

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registry Helper Global
////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern CRegistryHelper	g_rhHelper;

void RegisterGlobalVariables();
void ReadGlobalVariables();
void WriteGlobalVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern	CString		g_strBrowseCacheDirectory;
extern	CString		g_strDecalDirectory;
extern	CString		g_strTempDirectory;
extern	CString		g_strPaletteDirectory;
extern	CString		g_strJSONDirectory;
extern	int			g_iUntitledCount;

extern	int			g_iUntitledCount;
extern	CString		g_strBrowseCacheDirectory;
extern	CString		g_strDecalDirectory;
extern	CString		g_strTempDirectory;
extern	CString		g_strPaletteDirectory;
extern	CString		g_szAppDirectory;
extern	CString		g_strHalfLifeDirectory;
extern	CString		g_strModSelection;
extern	int			g_iColorDecalSelection;
extern	int			g_iColorDecalBlendRed;
extern	int			g_iColorDecalBlendGreen;
extern	int			g_iColorDecalBlendBlue;

class CWildCardList;
class CWndList;
class CProgressCtrl;
class CTool;
class CLayer;

extern	CWndList		g_InvalidateList;
extern	CProgressCtrl	g_ctProgress;
extern	CStatusBar		m_wndStatusBar;

// Editing trackers
extern	int			g_iCurrentTool;
extern	BOOL		g_bUpdateDecalPreviewWnd;
extern	CTool		g_Tool;

// Decals
extern	CLayer		g_LeftDecalToolLayerInfo;
extern	CLayer		g_RightDecalToolLayerInfo;
extern	CLayer		g_LeftPatternToolLayerInfo;
extern	CLayer		g_RightPatternToolLayerInfo;
extern	CLayer		g_LeftBulletLayerInfo;
extern	CLayer		g_RightBulletLayerInfo;
extern	CLayer		g_RivetToolLayerInfo;
extern	CLayer		g_CloneSourceLayerInfo;

// Q1/Q2 Palettes
extern	BYTE		quake2_pal[768];
extern	BYTE		quake1_pal[768];

// CWallyDoc constructor items:
extern	int			g_iDocWidth;
extern	int			g_iDocHeight;
extern	int			g_iDocColorDepth;

// Clipboard formats
extern	UINT		g_iPackageFormat;
extern	UINT		g_iBrowseCopyPasteFormat;

// Used for \n in CWnd edit boxes
extern	char CRLF[3];

// Debugging item
extern BOOL			g_bThrowAsserts;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pak editor globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern	BOOL		g_bPakQuickPlaySound;
extern	BOOL		g_bPakImportRecurseSubDirectories;
extern	BOOL		g_bPakImportReplaceExistingItems;
extern	BOOL		g_bPakImportRetainStructure;
extern	BOOL		g_bPakExportRecurseSubDirectories;
extern	BOOL		g_bPakExportReplaceExistingItems;
extern	BOOL		g_bPakExportRetainStructure;
extern	CString		g_strPakImportSourceDirectory;
extern	CString		g_strPakImportOffsetDirectory;
extern	CString		g_strPakImportWildCards;
extern	CString		g_strPakExportDestinationDirectory;
extern	CString		g_strPakExportWildCards;

void RegisterPakVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wally globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern	int			g_iDefaultZoomValue;
extern	int			g_iDefaultTextureWidth;
extern	int			g_iDefaultTextureHeight;
extern	int			g_iPreviousTextureWidth;
extern	int			g_iPreviousTextureHeight;
extern	BOOL		g_bRetainTextureSize;
			
extern	int			g_iFileTypeDefault;
extern	int			g_iFileTypeLastUsed;
extern	int			g_iFileTypePaste;
extern	int         g_iNumHorizontalTiles;
extern	int         g_iNumVerticalTiles;
extern	int			g_iPackageViewMode;
extern	int			g_iPackageThumbnailSize;
extern	int			g_iLastToolSettingsTab;

extern	COLORREF	g_irgbColorLeft;
extern	COLORREF	g_irgbColorRight;
//extern	int		g_iColorIndexLeft;
//extern	int		g_iColorIndexRight;
extern	int			g_iColorTolerance;

extern	int			g_iPaletteConversion;
extern	int			g_iTextureNameSetting;
extern	int			g_iExportColorDepth;
extern	int			g_iBrowserCacheFileGroom;
			
extern	BOOL		g_bEnlargeCenterVertically;
extern	BOOL		g_bEnlargeCenterHorizontally;
extern	BOOL		g_bAllowNon16;
			
extern	double		g_dfGamma;
extern	BOOL		g_bGammaCorrectPaletteToolbar;
extern	BOOL		g_bCompressUndo;

extern	DWORD       g_flgAllowableSizes;
extern	BOOL		g_bShowSubMips;

extern	BOOL        g_bShowFineGrid;
extern	BOOL        g_bShowCourseGrid;
extern	int         g_iGridHorizontal;
extern	int         g_iGridVertical;
extern	COLORREF    g_rgbGridColor;
			
extern	int			DefaultImportImageExtension;
extern	int			g_iOpenPaletteExtension;
extern	int			g_iFileOpenExtension;
extern	int			g_iFileConvertExtension;
extern	int			g_iFileExportExtension;
extern	int			g_iFileSaveExtension;
			
extern	CString		g_strDefaultTextureName;
extern	CString		DefaultImportImageDirectory;
extern	CString		g_strOpenPaletteDirectory;
extern	CString		g_strFileSaveDirectory;
extern	CString		g_strBrowseDirectory;
extern	CString		g_strFileConvertDirectory;
extern	CString		g_strFileOpenDirectory;
extern	CString		g_strHalfLifeDirectory;

extern	CString		g_strQuake2Palette;
extern	CString		g_strQuake1Palette;
extern	int			g_iUseDefaultQ2Palette;
extern	int			g_iUseDefaultQ1Palette;
extern	CString		g_strDefaultEditingPalette;
extern	BOOL		g_bBuildOptimizedPalette;

void RegisterWallyVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReMip globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern	int			g_bRebuildSubMipsOnSave;
extern	int			g_bAutoRemip;
extern	int			g_bAutoRemipAfterEdit;
extern	int			g_bErrorDiffusion;
extern	int			g_bNoise;
extern	int			g_iSharpen;
extern	int			g_bWeightedAverage;

void RegisterReMipVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Paint Effect globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern	int			g_iPasteAmount;
extern	int			g_bPasteInvisibleBackground;
extern	int			g_iLightenAmount;
extern	int			g_iDarkenAmount;
extern	int			g_iDecalAmount;
extern	int			g_iCloneAmount;
extern	int			g_iScratchAmount;
extern	int			g_iTintAmount;
extern	int			g_iBlendAmount;
extern	int			g_iSharpenAmount;
extern	int			g_iSprayAmount;
extern	int			g_iSprayRecolorAmount;
extern	int			g_iRecolorAmount;
extern	int			g_iPatternPaintAmount;
extern	int			g_iBulletHoleAmount;
extern	int			g_iRivetDistance;
extern	int			g_iRivetAmount;
extern	int			g_iDrawingWidth;
extern	int			g_iDrawingMode;
extern	int			g_iBrushShape;

void RegisterPaintEffectVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern	int			g_iRotateDirection;
extern	int			g_iRotateDegrees;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterImageEffectVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filter globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CFilter;
extern CFilter g_Filter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterFilterVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch Conversion globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern	CString		g_szSourceConvertDirectory;
extern	CString		g_szDestinationConvertDirectory;
extern	CString		g_szConvertWildCard;
extern	int			g_iMaxConversionThreads;
extern	BOOL		g_bOverWriteFiles;
extern	BOOL		g_bDisplayExportSummary;
extern	BOOL		g_bRecurseSubdirectories;
extern	BOOL		g_bRetainDirectoryStructure;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch Conversion registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterBatchConversionVariables();

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WAD Merge globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern	CString		g_strMergeWildCard;
extern	CString		g_strMergeDestinationWAD;
extern	int			g_iMergeDestinationChoice;
extern	int			g_iMergeDuplicationOption;
extern	int			g_iMergeNewWADType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WAD Merge registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterWADMergeVariables();

#endif