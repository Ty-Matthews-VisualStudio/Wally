/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998, 1999
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Globals.cpp
//
// Created by Ty Matthews, 11-13-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"

#include "PackageBrowseView.h"
#include "ImageHelper.h"
#include "ReMip.h"
#include "Filter.h"
#include "WndList.h"
#include "Tool.h"
#include "Layer.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
extern void WhatsNew(void);

//CString g_strProgramBuildDate = "Sep 20 2001";
CString g_strProgramBuildDate = "<not set>";

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ADD_REG_NUM(			i,		Value,	strKey)						g_rhHelper.AddItem (	&i,		i,		0,		0,		Value,	strKey)
#define ADD_REG_NUM_MIN(		i,		min,	Value, strKey)				g_rhHelper.AddItem (	&i,		i,		min,	0,		Value,	strKey, REGISTRY_MIN )
#define ADD_REG_NUM_MAX(		i,		max,	Value, strKey)				g_rhHelper.AddItem (	&i,		i,		0,		max,	Value,	strKey, REGISTRY_MAX )
#define ADD_REG_NUM_MINMAX(		i,		min, max,	Value, strKey)			g_rhHelper.AddItem (	&i,		i,		min,		max,	Value,	strKey, REGISTRY_MIN | REGISTRY_MAX )
#define ADD_REG_SZ(				str,	Value,	strKey)						g_rhHelper.AddItem (	&str,	str,					Value,	strKey)

CRegistryHelper		g_rhHelper;	

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RegisterGlobalVariables ()
{
	g_rhHelper.SetMainKey (HKEY_CURRENT_USER);
	g_rhHelper.SetBaseSubKey("Software\\Team BDP\\Wally");	// Neal - TODO: is this correct?
	
	RegisterPakVariables();
	RegisterWallyVariables();
	RegisterReMipVariables();
	RegisterImageEffectVariables();
	RegisterFilterVariables();
	RegisterBatchConversionVariables();
	RegisterPaintEffectVariables();			// Neal - BUGFIX - need to save these too!
	RegisterWADMergeVariables();
}

void ReadGlobalVariables()
{
	g_rhHelper.ReadRegistry();

	// Neal - store last program build date in registry, 
	// if different, bring up a "what's new" message box.

	if (g_strProgramBuildDate != __DATE__)
	{
		WhatsNew();

		// Neal - make sure registry is up-to-date
		g_strProgramBuildDate = __DATE__;
		g_rhHelper.WriteRegistry();
	}

	// Validation checks for non min/max items
	if ((g_iExportColorDepth != IH_8BIT) && (g_iExportColorDepth != IH_24BIT))
	{
		g_iExportColorDepth = IH_8BIT;
	}
}

void WriteGlobalVariables()
{
	g_rhHelper.WriteRegistry();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int			g_iUntitledCount				= 1;
CString		g_strBrowseCacheDirectory		= "";
CString		g_strDecalDirectory				= "";
CString		g_strTempDirectory				= "";
CString		g_strPaletteDirectory			= "";
CString		g_szAppDirectory				= "";

CWndList		g_InvalidateList;
CProgressCtrl	g_ctProgress;
CStatusBar		m_wndStatusBar;

// Editing trackers
int			g_iCurrentTool     = 0;
BOOL		g_bUpdateDecalPreviewWnd	= TRUE;
CTool		g_Tool;

// Decals
CLayer		g_LeftDecalToolLayerInfo;
CLayer		g_RightDecalToolLayerInfo;
CLayer		g_LeftPatternToolLayerInfo;
CLayer		g_RightPatternToolLayerInfo;
CLayer		g_LeftBulletLayerInfo;
CLayer		g_RightBulletLayerInfo;
CLayer		g_RivetToolLayerInfo;
CLayer		g_CloneSourceLayerInfo;

// Q1/Q2 Palettes
BYTE		quake2_pal[768];
BYTE		quake1_pal[768];

// CWallyDoc constructor items:
int			g_iDocWidth;
int			g_iDocHeight;
int			g_iDocColorDepth;

// Clipboard formats
UINT		g_iPackageFormat;
UINT		g_iBrowseCopyPasteFormat;

// Used for \n in CWnd edit boxes
char CRLF[3] = {0x0d, 0x0a, 0x0};

// Debugging item
extern BOOL			g_bThrowAsserts = FALSE;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pak editor globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL		g_bPakQuickPlaySound				= TRUE;
BOOL		g_bPakImportRecurseSubDirectories	= TRUE;
BOOL		g_bPakImportReplaceExistingItems	= FALSE;
BOOL		g_bPakImportRetainStructure			= TRUE;
BOOL		g_bPakExportRecurseSubDirectories	= TRUE;
BOOL		g_bPakExportReplaceExistingItems	= FALSE;
BOOL		g_bPakExportRetainStructure			= TRUE;
CString		g_strPakImportSourceDirectory		= "";
CString		g_strPakImportOffsetDirectory		= "";
CString		g_strPakImportWildCards				= "*.*";
CString		g_strPakExportDestinationDirectory  = "";
CString		g_strPakExportWildCards				= "*.*";


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pak editor registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
void RegisterPakVariables()
{
	CString strPakKey ("Pak Editor");
	
	ADD_REG_NUM(	g_bPakQuickPlaySound,				"QuickPlay Sound",					strPakKey);
	ADD_REG_NUM(	g_bPakImportRecurseSubDirectories,	"Import Recurse SubDirectories",	strPakKey);
	ADD_REG_NUM(	g_bPakImportReplaceExistingItems,	"Import Replace Existing Items",	strPakKey);
	ADD_REG_NUM(	g_bPakImportRetainStructure,		"Import Retain Structure",			strPakKey);
	ADD_REG_NUM(	g_bPakExportRecurseSubDirectories,	"Export Recurse SubDirectories",	strPakKey);
	ADD_REG_NUM(	g_bPakExportReplaceExistingItems,	"Export Replace Existing Items",	strPakKey);
	ADD_REG_NUM(	g_bPakExportRetainStructure,		"Export Retain Structure",			strPakKey);

	ADD_REG_SZ(		g_strPakImportSourceDirectory,		"Import Source Directory",			strPakKey);
	ADD_REG_SZ(		g_strPakImportOffsetDirectory,		"Import Offset Directory",			strPakKey);
	ADD_REG_SZ(		g_strPakImportWildCards,			"Import WildCards",					strPakKey);
	ADD_REG_SZ(		g_strPakExportDestinationDirectory,	"Export Destination Directory",		strPakKey);
	ADD_REG_SZ(		g_strPakExportWildCards,			"Export WildCards",					strPakKey);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wally globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int			g_iDefaultZoomValue				= 0;	// auto-zoom
int			g_iDefaultTextureWidth			= 64;
int			g_iDefaultTextureHeight			= 64;
int			g_iPreviousTextureWidth			= 64;
int			g_iPreviousTextureHeight		= 64;
BOOL		g_bRetainTextureSize			= FALSE;
			
int			g_iFileTypeDefault				= FILE_TYPE_PROMPT;
int			g_iFileTypeLastUsed				= FILE_TYPE_TGA;
int			g_iFileTypePaste				= FILE_TYPE_PROMPT;
int         g_iNumHorizontalTiles			= 2;
int         g_iNumVerticalTiles				= 2;
int			g_iPackageViewMode				= PACKAGE_BROWSE_VIEWMODE_BROWSE;
int			g_iPackageThumbnailSize			= 4;
int			g_iLastToolSettingsTab			= 0;

//int		g_iColorIndexLeft				= 0;
//int		g_iColorIndexRight				= 0;
int			g_iColorTolerance				= 0;
COLOR_IRGB  g_irgbColorLeft                 = 0;	//COLOR_DEBUG_PURPLE;
COLOR_IRGB  g_irgbColorRight                = 0;	//IRGB(255,255,192,255);

int			g_iPaletteConversion			= 1;
int			g_iTextureNameSetting			= BLANK_DIR;
int			g_iExportColorDepth				= IH_8BIT;
int			g_iBrowserCacheFileGroom		= 7;
			
BOOL		g_bEnlargeCenterVertically		= FALSE;
BOOL		g_bEnlargeCenterHorizontally	= FALSE;
BOOL		g_bAllowNon16					= FALSE;
			
double		g_dfGamma						= 1.5;
BOOL		g_bGammaCorrectPaletteToolbar	= TRUE;
BOOL		g_bCompressUndo					= TRUE;

DWORD       g_flgAllowableSizes				= ALLOW_ANY_SIZE;
BOOL		g_bShowSubMips					= TRUE;

BOOL        g_bShowFineGrid					= FALSE;
BOOL        g_bShowCourseGrid				= FALSE;
int         g_iGridHorizontal				= 8;
int         g_iGridVertical					= 8;
COLORREF    g_rgbGridColor					= RGB( 12, 0, 102);
			
int			DefaultImportImageExtension		= 1;
int			g_iOpenPaletteExtension			= 1;
int			g_iFileOpenExtension			= 1;
int			g_iFileConvertExtension			= 1; 
int			g_iFileExportExtension			= 1;
int			g_iFileSaveExtension			= 1;
			
CString		g_strDefaultTextureName			= "";
CString		DefaultImportImageDirectory		= "";
CString		g_strOpenPaletteDirectory		= "";
CString		g_strFileSaveDirectory			= "";
CString		g_strBrowseDirectory			= "";
CString		g_strFileConvertDirectory		= "";
CString		g_strFileOpenDirectory			= "";
CString		g_strHalfLifeDirectory			= "";
CString		g_strModSelection				= "";
int			g_iColorDecalSelection			= 0;
int			g_iColorDecalBlendRed			= 255;
int			g_iColorDecalBlendGreen			= 0;
int			g_iColorDecalBlendBlue			= 255;

CString		g_strQuake2Palette				= "";
CString		g_strQuake1Palette				= "";
int			g_iUseDefaultQ2Palette			= TRUE;
int			g_iUseDefaultQ1Palette			= TRUE;
CString		g_strDefaultEditingPalette		= "";
BOOL		g_bBuildOptimizedPalette		= FALSE;


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wally registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterWallyVariables()
{
	CString strWallyKey ("Settings");
	
	ADD_REG_NUM(		g_iDefaultZoomValue,				"DefaultZoomValue",					strWallyKey);
	ADD_REG_NUM(		g_iNumHorizontalTiles,				"NumHorizTiles",					strWallyKey);
	ADD_REG_NUM(		g_iNumVerticalTiles,				"NumVertTiles",						strWallyKey);
	ADD_REG_NUM(		g_iPackageViewMode,					"Package View Mode",				strWallyKey);
	ADD_REG_NUM(		g_iPackageThumbnailSize,			"Package View Thumbnail Size",		strWallyKey);
	ADD_REG_NUM(		g_iLastToolSettingsTab,				"Last Tool Settings Tab",			strWallyKey);
	
	ADD_REG_NUM_MINMAX(	g_iDefaultTextureWidth,		1,					MAX_TEXTURE_WIDTH,	"DefaultTextureWidth",		strWallyKey);
	ADD_REG_NUM_MINMAX(	g_iDefaultTextureHeight,	1,					MAX_TEXTURE_HEIGHT,	"DefaultTextureHeight",		strWallyKey);
	ADD_REG_NUM_MINMAX(	g_iPreviousTextureWidth,	1,					MAX_TEXTURE_WIDTH,	"PreviousTextureWidth",		strWallyKey);
	ADD_REG_NUM_MINMAX(	g_iPreviousTextureHeight,	1,					MAX_TEXTURE_WIDTH,	"PreviousTextureHeight",	strWallyKey);
	
	ADD_REG_NUM_MINMAX(	g_iFileTypeDefault,			FILE_TYPE_PROMPT,	FILE_TYPE_END,		"Default File Type",		strWallyKey);
	ADD_REG_NUM_MINMAX(	g_iFileTypeLastUsed,		FILE_TYPE_PROMPT,	FILE_TYPE_END,		"Last Used File Type",		strWallyKey);
	ADD_REG_NUM_MINMAX(	g_iFileTypePaste,			FILE_TYPE_PROMPT,	FILE_TYPE_END,		"DefaultGameTypePaste",		strWallyKey);
	
//	ADD_REG_NUM_MINMAX(	g_iColorIndexLeft,			0,					255,				"GlobalColorIndexLeft",		strWallyKey);
//	ADD_REG_NUM_MINMAX(	g_iColorIndexRight,			0,					255,				"GlobalColorIndexRight",	strWallyKey);
	ADD_REG_NUM(		g_irgbColorLeft,					"GlobalColorLeft",						strWallyKey);
	ADD_REG_NUM(		g_irgbColorRight,					"GlobalColorRight",						strWallyKey);
	
	ADD_REG_NUM(		g_iPaletteConversion,				"PaletteConversion",					strWallyKey);
	ADD_REG_NUM(		g_bRetainTextureSize,				"RetainTextureSize",					strWallyKey);
	ADD_REG_NUM(		g_iTextureNameSetting,				"TextureNameSetting",					strWallyKey);	
	ADD_REG_NUM(		g_iExportColorDepth,				"ExportColorDepth",						strWallyKey);
	
	ADD_REG_NUM(		g_iBrowserCacheFileGroom,			"Browser Cache Groom",					strWallyKey);	
	ADD_REG_NUM(		g_bEnlargeCenterHorizontally,		"Center Horizontally on Enlarge",		strWallyKey);
	ADD_REG_NUM(		g_bEnlargeCenterVertically,			"Center Vertically on Enlarge",			strWallyKey);
	ADD_REG_NUM(		g_bAllowNon16,						"Allow non-16 game sizes",				strWallyKey);
	
	

	ADD_REG_NUM(		g_dfGamma,							"Gamma",								strWallyKey);
	ADD_REG_NUM(		g_bGammaCorrectPaletteToolbar,		"GammaCorrectPaletteToolbar",			strWallyKey);
	ADD_REG_NUM(		g_bCompressUndo,					"CompressUndo",							strWallyKey);
	ADD_REG_NUM(		g_flgAllowableSizes,				"AllowableSizes",						strWallyKey);
	ADD_REG_NUM(		g_bShowSubMips,						"ShowSubMips",							strWallyKey);

	ADD_REG_NUM(		g_bShowFineGrid,					"ShowFineGrid",							strWallyKey);
	ADD_REG_NUM(		g_bShowCourseGrid,					"ShowCourseGrid",						strWallyKey);
	ADD_REG_NUM(		g_iGridHorizontal,					"GridHorizontal",						strWallyKey);
	ADD_REG_NUM(		g_iGridVertical,					"GridVertical",							strWallyKey);
	ADD_REG_NUM(		g_rgbGridColor,						"GridColor",							strWallyKey);
	ADD_REG_NUM(		g_iColorTolerance,					"Color Tolerance",						strWallyKey);

	ADD_REG_NUM(		DefaultImportImageExtension,		"DefaultImportImageExtension",			strWallyKey);
	ADD_REG_NUM(		g_iOpenPaletteExtension,			"DefaultOpenPaletteExtension",			strWallyKey);
	ADD_REG_NUM(		g_iFileOpenExtension,				"DefaultFileOpenExtension",				strWallyKey);
	ADD_REG_NUM(		g_iFileConvertExtension,			"DefaultFileConvertExtension",			strWallyKey);
	ADD_REG_NUM(		g_iFileExportExtension,				"DefaultFileExportExtension",			strWallyKey);
	ADD_REG_NUM(		g_iFileSaveExtension,				"DefaultFileSaveExtension",				strWallyKey);
	
	ADD_REG_SZ(			g_strDefaultTextureName,			"DefaultTextureNameDirectory",			strWallyKey);
	ADD_REG_SZ(			DefaultImportImageDirectory,		"DefaultImportImageDirectory",			strWallyKey);
	ADD_REG_SZ(			g_strOpenPaletteDirectory,			"g_strOpenPaletteDirectory",			strWallyKey);
	ADD_REG_SZ(			g_strFileSaveDirectory,				"DefaultFileSaveDirectory",				strWallyKey);
	ADD_REG_SZ(			g_strBrowseDirectory,				"DefaultBrowseDirectory",				strWallyKey);
	ADD_REG_SZ(			g_strFileConvertDirectory,			"DefaultFileConvertDirectory",			strWallyKey);
	ADD_REG_SZ(			g_strFileOpenDirectory,				"DefaultFileOpenDirectory",				strWallyKey);
	ADD_REG_SZ(			g_strHalfLifeDirectory,				"Half-Life Directory",					strWallyKey);
	ADD_REG_SZ(			g_strModSelection,					"Color Decal Mod Directory",			strWallyKey);
	ADD_REG_NUM(		g_iColorDecalSelection,				"Color Decal Options",					strWallyKey);
	ADD_REG_NUM(		g_iColorDecalBlendRed,				"Color Decal Blend Red Color",			strWallyKey);
	ADD_REG_NUM(		g_iColorDecalBlendGreen,			"Color Decal Blend Green Color",		strWallyKey);
	ADD_REG_NUM(		g_iColorDecalBlendBlue,				"Color Decal Blend Blue Color",			strWallyKey);
	
	ADD_REG_SZ(			g_strQuake2Palette,					"Quake2 Palette",						strWallyKey);
	ADD_REG_SZ(			g_strQuake1Palette,					"Quake1 Palette",						strWallyKey);
	ADD_REG_NUM(		g_iUseDefaultQ2Palette,				"Use Default Q2",						strWallyKey);
	ADD_REG_NUM(		g_iUseDefaultQ1Palette,				"Use Default Q1",						strWallyKey);
	ADD_REG_SZ(			g_strDefaultEditingPalette,			"Default Editing Palette",				strWallyKey);
	ADD_REG_NUM(		g_bBuildOptimizedPalette,			"Build Optimized Palette",				strWallyKey);
	
	ADD_REG_NUM(		g_bThrowAsserts,					"Throw ASSERTs",						strWallyKey);
	ADD_REG_SZ(			g_strProgramBuildDate,				"Program Build Date",					strWallyKey);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReMip globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int			g_bRebuildSubMipsOnSave			= FALSE;
int			g_bAutoRemip					= FALSE;
int			g_bAutoRemipAfterEdit			= FALSE;
int			g_bErrorDiffusion				= TRUE;
int			g_bNoise						= FALSE;
int			g_iSharpen						= SHARPEN_MEDIUM;
int			g_bWeightedAverage				= TRUE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ReMip registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterReMipVariables()
{
	CString strReMipKey ("ReMipDLX");

	ADD_REG_NUM(		g_bRebuildSubMipsOnSave,			"RebuildSubMipsOnSave",					strReMipKey);
	ADD_REG_NUM(		g_bAutoRemip,						"AutoRemipOnImport",					strReMipKey);
	ADD_REG_NUM(		g_bAutoRemipAfterEdit,				"AutoRemipAfterEdit",					strReMipKey);
	ADD_REG_NUM(		g_bErrorDiffusion,					"ErrorDiffusion",						strReMipKey);
	ADD_REG_NUM(		g_bNoise,							"WhiteNoise",							strReMipKey);
	ADD_REG_NUM(		g_iSharpen,							"Sharpen",								strReMipKey);
	ADD_REG_NUM(		g_bWeightedAverage,					"WeightedAverage",						strReMipKey);	

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Paint Effect globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int			g_iPasteAmount					= 4;
int			g_bPasteInvisibleBackground		= 1;
int			g_iLightenAmount				= 2;
int			g_iDarkenAmount					= 2;
int			g_iDecalAmount					= 4;
int			g_iCloneAmount					= 4;
int			g_iScratchAmount				= 2;
int			g_iTintAmount					= 2;
int			g_iBlendAmount					= 2;
int			g_iSharpenAmount				= 2;
int			g_iSprayAmount					= 2;
int			g_iSprayRecolorAmount			= 2;
int			g_iRecolorAmount				= 2;
int			g_iPatternPaintAmount			= 2;
int			g_iBulletHoleAmount				= 2;
int			g_iRivetDistance				= 8;
int			g_iRivetAmount					= 2;
int			g_iDrawingWidth					= 5;
int			g_iDrawingMode					= MODE_FREEHAND;
int			g_iBrushShape					= SHAPE_CIRCULAR;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Paint Effect registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterPaintEffectVariables()
{
	CString strPaintEffectsKey ("PaintEffects");
	
	ADD_REG_NUM(		g_iPasteAmount,									"PasteAmount",					strPaintEffectsKey);
	ADD_REG_NUM(		g_bPasteInvisibleBackground,					"PasteInvisibleBackground",		strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iLightenAmount,				0,		4,		"LightenAmount",				strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iDarkenAmount,				0,		4,		"DarkenAmount",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iDecalAmount,									"DecalAmount",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iCloneAmount,									"CloneAmount",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iScratchAmount,								"ScratchAmount",				strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iTintAmount,					0,		4,		"TintAmount",					strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iBlendAmount,					0,		4,		"BlendAmount",					strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iSharpenAmount,				0,		4,		"SharpenAmount",				strPaintEffectsKey);
	ADD_REG_NUM_MINMAX(	g_iSprayAmount,					0,		4,		"SprayAmount",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iSprayRecolorAmount,							"SprayRecolorAmount",			strPaintEffectsKey);
	ADD_REG_NUM(		g_iRecolorAmount,								"RecolorAmount",				strPaintEffectsKey);
	ADD_REG_NUM(		g_iPatternPaintAmount,							"PatternPaintAmount",			strPaintEffectsKey);
	ADD_REG_NUM(		g_iBulletHoleAmount,							"BulletHoleAmount",				strPaintEffectsKey);
	ADD_REG_NUM(		g_iRivetDistance,								"RivetDistance",				strPaintEffectsKey);
	ADD_REG_NUM(		g_iRivetAmount,									"RivetAmount",					strPaintEffectsKey);

	ADD_REG_NUM(		g_iDrawingWidth,								"DrawingWidth",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iDrawingMode,									"DrawingMode",					strPaintEffectsKey);
	ADD_REG_NUM(		g_iBrushShape,									"BrushShape",					strPaintEffectsKey);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int			g_iRotateDirection				= ROTATE_LEFT;
int			g_iRotateDegrees				= ROTATE_90;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterImageEffectVariables()
{
	CString strImageEffectsKey ("ImageEffects");

	// Image Effect settings
	ADD_REG_NUM(		g_iRotateDirection,								"RotateDirection",				strImageEffectsKey);
	ADD_REG_NUM(		g_iRotateDegrees,								"RotateDegrees",				strImageEffectsKey);
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filter globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFilter		g_Filter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Image Effect registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterFilterVariables()
{
	CString strFilterKey ("FilterSettings");

	ADD_REG_SZ(			g_Filter.m_strRepeatLast[0],					"RepeatLast0",					strFilterKey);
	ADD_REG_SZ(			g_Filter.m_strRepeatLast[1],					"RepeatLast1",					strFilterKey);
	ADD_REG_SZ(			g_Filter.m_strRepeatLast[2],					"RepeatLast2",					strFilterKey);
	ADD_REG_SZ(			g_Filter.m_strRepeatLast[3],					"RepeatLast3",					strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iRepeatLastFilterID[0],				"RepeatLastID0",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iRepeatLastFilterID[1],				"RepeatLastID1",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iRepeatLastFilterID[2],				"RepeatLastID2",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iRepeatLastFilterID[3],				"RepeatLastID3",				strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iAddNoiseAmount,						"AddNoiseAmount",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iAddNoiseDistribution,				"AddNoiseDistribution",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iAddNoiseType,						"AddNoiseType",					strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iAddNoiseMinLength,					"AddNoiseMinLength",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iAddNoiseMaxLength,					"AddNoiseMaxLength",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_bMonochromeAddNoise,					"MonochromeAddNoise",			strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iBlendAmount,						"BlendAmount",					strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iBrightnessAmount,					"BrightnessAmount",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iContrastAmount,						"ContrastAmount",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iDiffuseAmount,						"DiffuseAmount",				strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iEdgeDetectAmount,					"EdgeDetectAmount",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iEdgeDetectColor,					"EdgeDetectColor",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_bEdgeDetectMonochrome,				"EdgeDetectMonochrome",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_rgbEdgeDetectColor,					"rgbEdgeDetectColor",			strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iEmbossAmount,						"EmbossAmount",					strFilterKey);
	ADD_REG_NUM(		g_Filter.m_bEmbossMonochrome,					"EmbossMonochrome",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMirrorType,							"MirrorType",					strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iMarbleSeed,							"MarbleSeed",                   strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleLevels,						"MarbleLevels",                 strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleNumThickStripes,				"MarbleNumThickStripes",        strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleNumThinStripes,				"MarbleNumThinStripes",         strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleColorIndexBackground,			"MarbleColorIndexBackground",   strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleColorIndexThickStripes,		"MarbleColorIndexThickStripes", strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleColorIndexThinStripes,		"MarbleColorIndexThinStripes",  strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleColorVariance,				"MarbleColorVariance",          strFilterKey);
	ADD_REG_NUM(		g_Filter.m_bMarbleSeamlessTexture,				"MarbleSeamlessTexture",        strFilterKey);
	ADD_REG_NUM(		g_Filter.m_bMarbleExtraSmooth,					"MarbleExtraSmooth",            strFilterKey);
	ADD_REG_NUM(		g_Filter.m_dfMarbleAmplitude,					"dfMarbleAmplitude",            strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleScale[0],						"vMarbleScale.0",               strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleScale[1],						"vMarbleScale.1",               strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleScale[2],						"vMarbleScale.2",               strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleOffset[0],					"vMarbleOffset.0",              strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleOffset[1],					"vMarbleOffset.1",              strFilterKey);
	ADD_REG_NUM(		g_Filter.m_vMarbleOffset[2],					"vMarbleOffset.2",              strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iMarbleTileColorGrout,				"MarbleTileColorGrout",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileSeed,						"MarbleTileSeed",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileSize,						"MarbleTileSize",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileShape,					"MarbleTileShape",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileColor,					"MarbleTileColor",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileStyle,					"MarbleTileStyle",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileGroutType,				"MarbleTileGroutType",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iMarbleTileBevelType,				"MarbleTileBevelType",			strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iReduceColorsMaxNum,					"ReduceColorsMaxNum",			strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iReserveColor[0],					"ReserveColor.0",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iReserveColor[1],					"ReserveColor.1",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iReserveColor[2],					"ReserveColor.2",				strFilterKey);
	ADD_REG_NUM(		g_Filter.m_iReserveColor[3],					"ReserveColor.3",				strFilterKey);

	ADD_REG_NUM(		g_Filter.m_iSharpenAmount,						"SharpenAmount",				strFilterKey);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch Conversion globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////

CString		g_szSourceConvertDirectory		= "";
CString		g_szDestinationConvertDirectory = "";
CString		g_szConvertWildCard				= "";
int			g_iMaxConversionThreads			= 10;
BOOL		g_bOverWriteFiles				= FALSE;
BOOL		g_bDisplayExportSummary			= TRUE;
BOOL		g_bRecurseSubdirectories		= FALSE;
BOOL		g_bRetainDirectoryStructure		= FALSE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch Conversion registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterBatchConversionVariables()
{
	CString strBatchConversionKey ("BatchSettings");

	g_iMaxConversionThreads = std::thread::hardware_concurrency(); // Use number of cores as the default

	ADD_REG_SZ(			g_szSourceConvertDirectory,						"SourceDirectory",				strBatchConversionKey);
	ADD_REG_SZ(			g_szDestinationConvertDirectory,				"DestinationDirectory",			strBatchConversionKey);
	ADD_REG_SZ(			g_szConvertWildCard,							"WildCard",						strBatchConversionKey);
	ADD_REG_NUM_MIN(	g_iMaxConversionThreads,	1,					"MaxThreads",					strBatchConversionKey);
	ADD_REG_NUM(		g_bOverWriteFiles,								"OverWriteFiles",				strBatchConversionKey);
	ADD_REG_NUM(		g_bRecurseSubdirectories,						"Recurse Subdirectories",		strBatchConversionKey);	
	ADD_REG_NUM(		g_bRetainDirectoryStructure,					"Retain Structure",				strBatchConversionKey);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WAD Merge globals
////////////////////////////////////////////////////////////////////////////////////////////////////////////


CString		g_strMergeWildCard				= "*";
CString		g_strMergeDestinationWAD		= "";
int			g_iMergeDestinationChoice		= IDC_RADIO_EXISTING_WAD;
int			g_iMergeDuplicationOption		= IDC_RADIO_RENAME;
int			g_iMergeNewWADType				= WAD3_TYPE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WAD Merge registration
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RegisterWADMergeVariables()
{
	CString strBatchConversionKey ("WADMerge");
	
	ADD_REG_SZ(			g_strMergeWildCard,								"WildCard",						strBatchConversionKey);
	ADD_REG_SZ(			g_strMergeDestinationWAD,						"Destination WAD",				strBatchConversionKey);
	ADD_REG_NUM(		g_iMergeDestinationChoice,						"Destination Choice",			strBatchConversionKey);
	ADD_REG_NUM(		g_iMergeDuplicationOption,						"Duplication Option",			strBatchConversionKey);
	ADD_REG_NUM(		g_iMergeNewWADType,								"New WAD Type",					strBatchConversionKey);
}
