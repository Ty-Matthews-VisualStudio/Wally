/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Filter.h : interface of the CFilter class
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOISE_H_
	#include "Noise.h"
#endif

#ifndef _REMIP_H_
	#include "ReMip.h"
#endif

#ifndef _WALLYVIEW_H_
	#include "WallyView.h"
#endif

#ifndef _WALLYUNDO_H_
	#include "WallyUndo.h"
#endif

#ifndef _FILTER_H_
#define _FILTER_H_

#define FILTER_ADD_NOISE				3001
#define FILTER_BLEND					3002
#define FILTER_BRIGHTNESS_CONTRAST		3003
#define FILTER_EDGE_DETECT				3004
#define FILTER_EMBOSS					3005
#define FILTER_HISTOGRAM				3006
#define FILTER_SHARPEN					3007
#define FILTER_DIFFUSE					3008
#define FILTER_OFFSET					3009
#define FILTER_RESIZE					3010
#define FILTER_FLIP_HORIZONTAL			3011
#define FILTER_FLIP_VERTICAL			3012
#define FILTER_SKEW						3013
#define FILTER_TILED_TINT				3014
#define FILTER_FIX_UNEVEN_LIGHTING		3015

#define FILTER_ERASE_ALL				3101
#define FILTER_RECOLOR_ALL				3102
#define FILTER_COLORIZE_ALL				3103
#define FILTER_COLOR_REPLACE_ALL		3104
#define FILTER_REDUCE_COLORS			3105

#define FILTER_MIRROR					3200
#define MIRROR_LEFT_TO_RIGHT			3201
#define MIRROR_TOP_TO_BOTTOM			3202
#define MIRROR_UPPER_LEFT_TO_REST		3203

#define FILTER_MARBLE					3901
#define FILTER_MARBLE_TILE				3902
#define FILTER_NOISE					3903
#define FILTER_TURBULANCE				3904
#define FILTER_WOOD						3905
#define FILTER_CLOUD					3906
#define FILTER_WATER					3907

#define FILTER_REPEAT_LAST_0			4000
#define FILTER_REPEAT_LAST_1			4001
#define FILTER_REPEAT_LAST_2			4002
#define FILTER_REPEAT_LAST_3			4003


#define TILE_ANY			0

#define TILE_ANY_SHAPE		TILE_ANY
#define TILE_SEAMLESS		1
#define TILE_SEAMLESS_X		2
#define TILE_SEAMLESS_Y		3
#define TILE_SQUARE			10
#define TILE_RECTANGULAR	11
#define TILE_DIAMOND			12
#define TILE_HEXAGONAL		13
#define TILE_OCTAGONAL		14
#define TILE_OTHER_SHAPE	99

#define TILE_TRIANGULAR		20
										//					 --,
#define TRIANGULAR_UL		21	// upper left:  |/
										//					,--
#define TRIANGULAR_UR		22	// upper right: \|

#define TRIANGULAR_LL		23	// lower left:  |\
										//					 --'
#define TRIANGULAR_LR		24	// lower right: /|
										//					'--

#define PLAIN_ALIGNED				0
#define PLAIN_MIRRORED				1
#define PLAIN_BRICK					2

#define FANCY_STAGGERED				100
#define FANCY_PARQUET_2				101
#define FANCY_PARQUET_4				102
#define FANCY_PARQUET_8				103

#define EXTRA_FANCY_BORDERED		200
#define EXTRA_FANCY_THIN_BORDER		201
#define EXTRA_FANCY_MIXED			202	// (like my bathroom floor)

#define TILE_SIZE_ANY				TILE_ANY
#define TILE_TINY					301
#define TILE_SMALL					302
#define TILE_MEDIUM					303
#define TILE_LARGE					304

#define TILE_COLOR_ANY				TILE_ANY
#define TILE_COLOR_SOLID			501
#define TILE_COLOR_ACCENTED			502
#define TILE_COLOR_TWO_TONE			503
#define TILE_COLOR_MULTI			504

#define TILE_STYLE_ANY				TILE_ANY
#define TILE_STYLE_PLAIN			601
#define TILE_STYLE_FANCY			602
#define TILE_STYLE_EXTRA_FANCY		603

#define TILE_GROUT_ANY				TILE_ANY
#define TILE_GROUT_NONE				701
#define TILE_GROUT_THIN_SMOOTH		702
#define TILE_GROUT_THIN_ROUGH		703
#define TILE_GROUT_THICK_SMOOTH		704
#define TILE_GROUT_THICK_ROUGH		705

#define TILE_BEVEL_ANY				TILE_ANY
#define TILE_BEVEL_NONE				800
#define TILE_BEVEL_SHALLOW			801
#define TILE_BEVEL_STANDARD			802
#define TILE_BEVEL_DEEP				803

#define TILED_TINT_SHADE_ANY		TILE_ANY
#define TILED_TINT_SHADE_BRIGHTEN	901
#define TILED_TINT_SHADE_DARKEN		902
#define TILED_TINT_SHADE_RANDOM		903

#define TILED_TINT_LEVEL_ANY		TILE_ANY
#define TILED_TINT_LEVEL_LIGHT		1001
#define TILED_TINT_LEVEL_MEDIUM		1002
#define TILED_TINT_LEVEL_HEAVY		1003

#define NOISE_SINGLE_PIXEL			0
#define NOISE_HORIZONTAL_LINE		1
#define NOISE_VERTICAL_LINE			2

inline BOOL NeedToTrimCorners( int iShape)
{
	ASSERT ((iShape >= TILE_ANY_SHAPE) && (iShape <= TILE_OTHER_SHAPE));

	return ((iShape >= TILE_DIAMOND) && (iShape <= TILE_OCTAGONAL));
}

extern int GetFilterDlgAmount( CDialog *pDlg, BOOL bPreviewApplied);

typedef struct
{
	CLayer* pLayer;

	CPoint ptOffset;
	CSize  sizeTile;
	int    iShape;			// TILE_SQUARE, TILE_RECTANGULAR, TILE_DIAMOND, etc.
	int    iStyle;			// PLAIN_ALIGNED, PLAIN_BRICK, FANCY_STAGGERED, EXTRA_FANCY_BORDERED, etc.
	BOOL   bMirrorX;
	BOOL   bMirrorY;

	// to make tiles of a single marble color, all 3 of these should have same addr
	// to mix two marble colors of tiles, just Contrast and Accent should have same addr
	int*   iARGB_PrimaryField;		// array[4*256]
	int*   iARGB_ContrastField;	// array[4*256]
	int*   iARGB_AccentField;		// array[4*256]

	BYTE*  pbyBlockStencilData;	// used for hexagonal, octagonal, and other fancy shapes

	Vec    vScale;
	Vec    vOffset;
	double dfAmplitude;
	int    iLevels;
	int    iNumThickStripes;
	int    iNumThinStripes;
	int    iColorVariance;
	int    iRandSeed;
	BOOL   bSeamless;
	BOOL   bExtraSmooth;
	BOOL   bFastPreview;

	int    iTileRandSeed;
	int    iTileSize;			// TILE_TINY, TILE_SMALL, TILE_MEDIUM, TILE_LARGE
//	int    iTileShape;		// TILE_SQUARE, TILE_RECTANGULAR, TILE_HEXAGONAL, etc.
	int    iTileColor;		// TILE_COLOR_SOLID, TILE_COLOR_TWO_TONE, TILE_COLOR_MULTI
	int    iTileStyle;		// TILE_STYLE_PLAIN, TILE_STYLE_FANCY, TILE_STYLE_EXTRA_FANCY
	int    iTileGroutType;	// TILE_GROUT_NONE, TILE_GROUT_THIN_SMOOTH, TILE_GROUT_THIN_ROUGH, etc.
	int    iTileBevelType;	// TILE_BEVEL_NONE, TILE_BEVEL_SHALLOW, TILE_BEVEL_STANDARD, etc.
} MarbleParams;

typedef struct
{
	int		iTileSize;		// TILE_TINY, TILE_SMALL, TILE_MEDIUM, TILE_LARGE
	int		iTintShade;		// TILED_TINT_SHADE_BRIGHTEN, TILED_TINT_SHADE_DARKEN, TILED_TINT_SHADE_RANDOM
	int		iTintLevel;		// TILED_TINT_LEVEL_LIGHT, TILED_TINT_LEVEL_MEDIUM, TILED_TINT_LEVEL_HEAVY
} TiledTintParams, *LPTiledTintParams;

typedef struct
{
	int		iX;
	int		iY;
	int		iWidth;
	int		iHeight;
} TiledTintBlock, *LPTiledTintBlock;


//class CWallyDoc;
class CWallyView;

class CFilter
{
public:
	CWallyView* m_pView;

	int      m_iFilterType;

	int      m_iAddNoiseAmount;
	int      m_iAddNoiseDistribution;
	int      m_iAddNoiseType;
	int      m_iAddNoiseMinLength;
	int      m_iAddNoiseMaxLength;
	BOOL     m_bMonochromeAddNoise;

	int      m_iBlendAmount;
	int      m_iBrightnessAmount;
	int      m_iContrastAmount;
	int      m_iDiffuseAmount;

	int      m_iEdgeDetectAmount;
	int      m_iEdgeDetectColor;
	BOOL     m_bEdgeDetectMonochrome;
	COLORREF m_rgbEdgeDetectColor;

	int      m_iEmbossAmount;
	BOOL     m_bEmbossMonochrome;

	int      m_iHistogramLow;
	int      m_iHistogramMed;
	int      m_iHistogramHigh;

	int      m_iMarbleSeed;
	int      m_iMarbleLevels;
	int      m_iMarbleNumThickStripes;
	int      m_iMarbleNumThinStripes;
	int      m_iMarbleColorIndexBackground;
	int      m_iMarbleColorIndexThickStripes;
	int      m_iMarbleColorIndexThinStripes;
	int      m_iMarbleColorVariance;
	BOOL     m_bMarbleSeamlessTexture;
	BOOL     m_bMarbleExtraSmooth;
	double   m_dfMarbleAmplitude;
	Vec      m_vMarbleScale;
	Vec      m_vMarbleOffset;

	int      m_iMarbleTileSeed;
	int      m_iMarbleTileColorGrout;
	int      m_iMarbleTileSize;			// TILE_TINY, TILE_SMALL, TILE_MEDIUM, TILE_LARGE
	int      m_iMarbleTileShape;		// TILE_SQUARE, TILE_RECTANGULAR, TILE_HEXAGONAL, etc.
 	int      m_iMarbleTileColor;		// TILE_COLOR_SOLID, TILE_COLOR_TWO_TONE, TILE_COLOR_MULTI
	int      m_iMarbleTileStyle;		// TILE_STYLE_PLAIN, TILE_STYLE_FANCY, TILE_STYLE_EXTRA_FANCY
	int      m_iMarbleTileGroutType;	// TILE_GROUT_NONE, TILE_GROUT_THIN_SMOOTH, TILE_GROUT_THIN_ROUGH, etc.
	int      m_iMarbleTileBevelType;	// TILE_BEVEL_NONE, TILE_BEVEL_SHALLOW, TILE_BEVEL_STANDARD, etc.

	int      m_iMirrorType;
	int      m_iOffsetX;
	int      m_iOffsetY;

	int      m_iReduceColorsMaxNum;
	int      m_iReserveColor[4];
	int      m_iResizeType;
	int      m_iResizeAmount;
	int      m_iResizeWidth;				// ***** save in registry
	int      m_iResizeHeight;				// ***** save in registry
	int      m_iFixUnevenLightingAmount;	// ***** save in registry

	int      m_iSharpenAmount;

	BOOL        m_bNeedsFindNearestColor;
	BOOL        m_bUseSelection;
	BOOL        m_bResizeProportionally;

	// Neal - sometimes we skip rebuilding as a speed-up
	//        if you do that, you need to call CWallyView::SetRawDibBits()
	//        after a filter has been performed
	BOOL        m_bRebuildSubMips;			// Neal - usually TRUE

	CRect       m_rSelection;
	COLOR_IRGB* m_pIRGB_SrcBuffer;
	COLOR_IRGB* m_pIRGB_DestBuffer;
	CRect       m_rSrc;
	CRect       m_rDest;

	CString     m_strRepeatLast[5];        // Neal - has an extra temporary cell at end
	int         m_iRepeatLastFilterID[5];  // (must be 1 larger than "pick" items in menu)

	CFilter();
	~CFilter();

	int  PickFilter();
	int  ShowSettingsDlg( CLayer* pLayer);

	BOOL WithinSelection( int iX, int iY);
	BOOL PrepareSoloFilter( CLayer *pLayer, int iFilterType, LPCSTR lpszFilterName);
	BOOL PrepareFilter( CLayer* pLayer, int iFilterType, LPCSTR lpszFilterName, int iUndoType = UNDO_DOCUMENT);
	void EndSoloFilter();
	void EndFilter( CLayer* pLayer);

	void DoFilter( CWallyView* pView, int iFilterType, BOOL bShowSettingsDlg);
	void DoAddNoise( CLayer* pLayer, int iAddNoiseAmount, int iDistribution, 
			int iAddNoiseType, int iAddNoiseMinLength, int iAddNoiseMaxLength, BOOL bMonochrome);
	void DoBlend( CLayer* pLayer, int iBlendAmount);
	void DoSharpen( CLayer* pLayer, int iSharpenAmount);
	void DoBrightnessContrast( CLayer* pLayer, int iBrightenAmount, int iContrastAmount);
	void DoEmboss( CLayer* pLayer, int iEmbossAmount, BOOL bMonochrome);
	void DoEdgeDetect( CLayer* pLayer, int iAmount, COLORREF rgbBackground, BOOL bMonochrome);
	void DoFixUnevenLighting( CLayer* pLayer, int iFixUnevenLightingAmount);
	void DoPixelFilter( CLayer* pLayer, int iFilterType, int iAmount, int iAmount2 = 0);
	void DoTiledTint( CLayer *pLayer );

	void SpecialSharpenForReduce( CLayer* pLayer, int iWeight, DWORD dwNeighborFlags);
	void DoReduceColors( CLayer* pLayer, int iReduceColorsMaxNum, int iReserveColor[8]);
	void Reduce( CLayer* pLayer, int iDivX, int iDivY, BOOL bFirstCall);
	void Enlarge( CLayer* pLayer, int iMultX, int iMultY);
	void DoResize( CLayer* pLayer, int iResizeType, int iResizeAmount);
	void DoArbitraryResize( CLayer *pLayer, int iResizeWidth, int iResizeHeight);

	void GenerateMarbleColorField( CLayer* pLayer, int iARGB_Field[4*256], 
			int iBackground, int iThickStripes, int iThinStripes,
			int iNumThickStripes, int iNumThinStripes, int iColorVariance,
			int iRandSeed, BOOL bExtraSmooth);

	void GenerateMarbleTexture( CLayer* pLayer, int iARGB_Field[4*256], 
			LPRECT lpRect, BYTE* pbyBlockStencilData,
			Vec vScale, Vec vOffset, double dfAmplitude, 
			int iLevels, int iRandSeed, BOOL bSeamless, int iTileGroutType, 
			BOOL bMirrorX, BOOL bMirrorY, BOOL bFastPreview);

	void GenerateMarbleTileLine( int iRow, int iX, int iY, int iNumTiles, 
			int iIncrementX, int iIncrementY, MarbleParams* pParams);

	void GenerateMarbleSubtile( int iRow, int iColumn, MarbleParams* pParams);

//	void DoMarbleTex( CLayer* pLayer, Vec vScale, Vec vOffset, 
//			double dfAmplitude, int iLevels, int m_iMarbleNumThickStripes,
//			int m_iMarbleNumThinStripes, int iColorVariance, int iRandSeed,
//			BOOL bSeamless, BOOL bExtraSmooth, BOOL bFastPreview);

	void DoMarbleTex( MarbleParams* pParams);
};

#endif	// _FILTER_H_