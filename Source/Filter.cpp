/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Filter.cpp : implementation of the CFilter class
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "MainFrm.h"
#include "WallyDoc.h"
#include "WallyPal.h"
//#include "Remip.h"
#include "ColorOpt.h"
#include "Filter.h"
#include "AddNoiseDlg.h"
#include "BlendDlg.h"
#include "SharpenDlg.h"
#include "FilterBCDlg.h"
#include "EmbossDlg.h"
#include "EdgeDetectDlg.h"
#include "FixUnevenLightingDlg.h"
#include "MarbleDlg.h"
#include "MarbleTileDlg.h"
#include "DiffuseDlg.h"
#include "MirrorDlg.h"
#include "OffsetDlg.h"
#include "ReduceColorsDlg.h"
#include "ResizeDlg.h"
#include "2PassScale.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
extern CWallyPalette g_CurrentPalette;

#define FUDGE (8)	// general fudge factor

int   g_iWorking     = 0;
int   g_iTotalPixels = 0;
DWORD g_dwTickCount  = 0;
char* szAnimi[4]     = { "¤      ", "  ¤    ", "    ¤  ", "      ¤" };

/////////////////////////////////////////////////////////////////////////////
// CFilter

/////////////////////////////////////////////////////////////////////////////
// CFilter construction/destruction

CFilter::CFilter()
{
	m_pView                  = NULL;
	m_iFilterType            = -1;

	// Neal - sometimes we skip rebuilding as a speed-up
	//        if you do that, you need to call CWallyView::SetRawDibBits()
	//        after a filter has been performed
	m_bRebuildSubMips        = TRUE;

	m_iAddNoiseAmount        = FILTER_LIGHT;
	m_iAddNoiseDistribution  = NOISE_UNIFORM;
	m_iAddNoiseType          = NOISE_SINGLE_PIXEL;
	m_iAddNoiseMinLength     = 2;
	m_iAddNoiseMaxLength     = 20;
	m_bMonochromeAddNoise    = TRUE;

	m_iBlendAmount           = FILTER_LIGHT;
	m_iBrightnessAmount      = 0;
	m_iContrastAmount        = 0;
	m_iDiffuseAmount         = 1;
	m_iEdgeDetectAmount      = FILTER_LIGHT;
	m_iEdgeDetectColor       = 0;
	m_bEdgeDetectMonochrome  = FALSE;
	m_rgbEdgeDetectColor     = 0;
	m_iEmbossAmount          = FILTER_LIGHT;
	m_bEmbossMonochrome      = FALSE;
	m_iHistogramLow          = 0;
	m_iHistogramMed          = 128;
	m_iHistogramHigh         = 255;

	m_iMarbleSeed            = 111;
	m_iMarbleLevels          = 7;
	m_iMarbleNumThickStripes = 1;
	m_iMarbleNumThinStripes  = 3;
	m_iMarbleColorIndexBackground   = -1;
	m_iMarbleColorIndexThickStripes = -1;
	m_iMarbleColorIndexThinStripes  = -1;
	m_iMarbleColorVariance   = 0;
	m_bMarbleSeamlessTexture = TRUE;
	m_bMarbleExtraSmooth     = TRUE;
	m_dfMarbleAmplitude      = 1.5;
	m_vMarbleScale[0]        = 1.0;
	m_vMarbleScale[1]        = 1.0;
	m_vMarbleScale[2]        = 1.0;
	m_vMarbleOffset[0]       = 0.0;
	m_vMarbleOffset[1]       = 0.0;
	m_vMarbleOffset[2]       = 0.0;

	m_iMarbleTileColorGrout  = -1;
	m_iMarbleTileSeed        = 111;
	m_iMarbleTileSize        = TILE_SIZE_ANY;
	m_iMarbleTileShape       = TILE_ANY_SHAPE;
	m_iMarbleTileColor       = TILE_COLOR_ANY;
	m_iMarbleTileStyle       = TILE_STYLE_ANY;
	m_iMarbleTileGroutType   = TILE_GROUT_ANY;
	m_iMarbleTileBevelType   = TILE_BEVEL_ANY;

	m_iMirrorType            = MIRROR_LEFT_TO_RIGHT;
	m_iOffsetX               = 0;
	m_iOffsetY               = 0;

	m_iReduceColorsMaxNum    = 128;
	m_iReserveColor[0]       = -1;
	m_iReserveColor[1]       = -1;
	m_iReserveColor[2]       = -1;
	m_iReserveColor[3]       = -1;

	m_iResizeType              = 0;
	m_iResizeAmount            = 1;
	m_iResizeWidth             = 64;
	m_iResizeHeight            = 64;
	m_bResizeProportionally    = TRUE;
	m_iFixUnevenLightingAmount = FILTER_LIGHT;

	m_iSharpenAmount         = FILTER_LIGHT;

	m_bNeedsFindNearestColor = FALSE;
	m_bUseSelection          = FALSE;
	m_pIRGB_SrcBuffer        = NULL;
	m_pIRGB_DestBuffer       = NULL;

	m_strRepeatLast[0]       = "Add Noise Filter";
	m_iRepeatLastFilterID[0] = FILTER_ADD_NOISE;

	m_strRepeatLast[1]       = "Blend Filter";
	m_iRepeatLastFilterID[1] = FILTER_BLEND;

	m_strRepeatLast[2]       = "Diffuse Filter";
	m_iRepeatLastFilterID[2] = FILTER_DIFFUSE;

	m_strRepeatLast[3]       = "Sharpen Filter";
	m_iRepeatLastFilterID[4] = FILTER_SHARPEN;
};

CFilter::~CFilter()
{
	if (m_pIRGB_SrcBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_SrcBuffer);
	}
	if (m_pIRGB_DestBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_DestBuffer);
	}
};

int CFilter::PickFilter()
{
	// TODO: let the user pick a filter from a list

	return FILTER_BRIGHTNESS_CONTRAST;
};

int CFilter::ShowSettingsDlg( CLayer* pLayer)
{
	ASSERT( pLayer != NULL);

	CWallyDoc*  pDoc  = pLayer->GetDoc();
//	CWallyView* pView = pLayer->GetView();	// neal - just for TESTING

	int  iRet             = IDCANCEL;
	BOOL bPreviewApplied  = FALSE;
	BOOL bSettingsChanged = TRUE;		// TODO: add to dlgs - (speed-up)

	switch (m_iFilterType)
	{
	case FILTER_ADD_NOISE:
		{
			CAddNoiseDlg Dlg;
			Dlg.m_pLayer                = pLayer;
			Dlg.m_iAddNoiseAmount       = m_iAddNoiseAmount;
			Dlg.m_iAddNoiseDistribution = m_iAddNoiseDistribution;
			Dlg.m_iAddNoiseType         = m_iAddNoiseType;
			Dlg.m_iAddNoiseMinLength    = m_iAddNoiseMinLength;
			Dlg.m_iAddNoiseMaxLength    = m_iAddNoiseMaxLength;
			Dlg.m_bMonochromeAddNoise   = m_bMonochromeAddNoise;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iAddNoiseAmount       = Dlg.m_iAddNoiseAmount;
				m_iAddNoiseDistribution = Dlg.m_iAddNoiseDistribution;
				m_iAddNoiseType         = Dlg.m_iAddNoiseType;
				m_iAddNoiseMinLength    = Dlg.m_iAddNoiseMinLength;
				m_iAddNoiseMaxLength    = Dlg.m_iAddNoiseMaxLength;
				m_bMonochromeAddNoise   = Dlg.m_bMonochromeAddNoise;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_BLEND:
		{
			CBlendDlg Dlg;
			Dlg.m_pLayer       = pLayer;
			Dlg.m_iBlendAmount = m_iBlendAmount;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iBlendAmount = Dlg.m_iBlendAmount;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_BRIGHTNESS_CONTRAST:
		{
			CFilterBrightnessContrastDlg Dlg;
			Dlg.m_pLayer                  = pLayer;
			Dlg.m_iFilterBrightnessAmount = m_iBrightnessAmount;
			Dlg.m_iFilterContrastAmount   = m_iContrastAmount;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iBrightnessAmount = Dlg.m_iFilterBrightnessAmount;
				m_iContrastAmount   = Dlg.m_iFilterContrastAmount;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_DIFFUSE:
		{
			CDiffuseDlg Dlg;
			Dlg.m_pLayer         = pLayer;
			Dlg.m_iDiffuseAmount = m_iDiffuseAmount;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iDiffuseAmount = Dlg.m_iDiffuseAmount;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_EMBOSS:
		{
			CEmbossDlg Dlg;
			Dlg.m_pLayer        = pLayer;
			Dlg.m_iEmbossAmount = m_iEmbossAmount;
			Dlg.m_bMonochrome   = m_bEmbossMonochrome;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iEmbossAmount     = Dlg.m_iEmbossAmount;
				m_bEmbossMonochrome = Dlg.m_bMonochrome;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_EDGE_DETECT:
		{
			CEdgeDetectDlg Dlg;
			Dlg.m_pLayer            = pLayer;
			Dlg.m_iEdgeDetectAmount = m_iEdgeDetectAmount;
			Dlg.m_bMonochrome       = m_bEdgeDetectMonochrome;
			Dlg.m_iBackgroundColor  = m_iEdgeDetectColor;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				//int r, g, b;
				COLORREF rgb = 0;

				m_iEdgeDetectAmount     = Dlg.m_iEdgeDetectAmount;
				m_bEdgeDetectMonochrome = Dlg.m_bMonochrome;
				m_iEdgeDetectColor      = Dlg.m_iBackgroundColor;

				CWallyPalette* pPal = pDoc->GetPalette();

				switch (m_iEdgeDetectColor)
				{
					case 0:
						//rgb = 0;
						break;
					case 1:
						rgb = RGB( 255, 255, 255);
						break;
					case 2:
//						r = pPal->GetR( g_iColorIndexLeft);
//						g = pPal->GetG( g_iColorIndexLeft);
//						b = pPal->GetB( g_iColorIndexLeft);
						rgb = g_irgbColorLeft & 0x00FFFFFF;
						break;
					case 3:
//						r = pPal->GetR( g_iColorIndexRight);
//						g = pPal->GetG( g_iColorIndexRight);
//						b = pPal->GetB( g_iColorIndexRight);
						rgb = g_irgbColorRight & 0x00FFFFFF;
						break;
					default: 
						//rgb = 0;
						break;
				}
//				m_rgbEdgeDetectColor = RGB( r, g, b);
				m_rgbEdgeDetectColor = rgb;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_FIX_UNEVEN_LIGHTING:
		{
			static bool bOneTimeOnly = true;

			if (bOneTimeOnly)
			{
				bOneTimeOnly = false;
				AfxMessageBox(
						"The Fix Uneven Lighting Filter is designed to smooth the lighting\n"
						"in a photographic image based texture.  If you are drawing\n"
						"textures from scratch, you don't need to use this filter.\n"
						"\n"
						"This filter is VERY slow; it's doing a LOT of work.  Be patient,\n"
						"it can take several MINUTES to process a large image.  Go get\n"
						"a snack or stretch your legs.  :-)"
						, MB_OK);
			}
			CFixUnevenLightingDlg Dlg;
			Dlg.m_pLayer                   = pLayer;
			Dlg.m_iFixUnevenLightingAmount = m_iFixUnevenLightingAmount;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iFixUnevenLightingAmount = Dlg.m_iFixUnevenLightingAmount;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_MARBLE:
		{
			if (m_iMarbleColorIndexBackground == -1)
				m_iMarbleColorIndexBackground = pDoc->FindNearestColor( 255, 255, 255, FALSE);

			if (m_iMarbleColorIndexThickStripes == -1)
				m_iMarbleColorIndexThickStripes = pDoc->FindNearestColor( 128, 128, 128, FALSE);

			if (m_iMarbleColorIndexThinStripes == -1)
				m_iMarbleColorIndexThinStripes = pDoc->FindNearestColor(  32,  32,  32, FALSE);

			CMarbleDlg Dlg;
			Dlg.m_pLayer  = pLayer;
			Dlg.m_pFilter = this;

			Dlg.m_iMarbleSeed            = m_iMarbleSeed;
			Dlg.m_iMarbleLevels          = m_iMarbleLevels;
			Dlg.m_iMarbleNumThickStripes = m_iMarbleNumThickStripes;
			Dlg.m_iMarbleNumThinStripes  = m_iMarbleNumThinStripes;
			Dlg.m_dfMarbleAmplitude      = m_dfMarbleAmplitude;
			Dlg.m_bMarbleExtraSmooth     = m_bMarbleExtraSmooth;
			Dlg.m_bMarbleSeamlessTexture = m_bMarbleSeamlessTexture;
			Dlg.m_iMarbleColorVariance   = m_iMarbleColorVariance;
			Dlg.m_vMarbleScale[0]        = m_vMarbleScale[0];
			Dlg.m_vMarbleScale[1]        = m_vMarbleScale[1];
			Dlg.m_vMarbleScale[2]        = m_vMarbleScale[2];
			Dlg.m_vMarbleOffset[0]       = m_vMarbleOffset[0];
			Dlg.m_vMarbleOffset[1]       = m_vMarbleOffset[1];
			Dlg.m_vMarbleOffset[2]       = m_vMarbleOffset[2];

			Dlg.m_iShape = TILE_SEAMLESS;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iMarbleSeed            = Dlg.m_iMarbleSeed;
				m_iMarbleLevels          = Dlg.m_iMarbleLevels;
				m_iMarbleNumThickStripes = Dlg.m_iMarbleNumThickStripes;
				m_iMarbleNumThinStripes  = Dlg.m_iMarbleNumThinStripes;
				m_dfMarbleAmplitude      = Dlg.m_dfMarbleAmplitude;
				m_bMarbleExtraSmooth     = Dlg.m_bMarbleExtraSmooth;
				m_bMarbleSeamlessTexture = Dlg.m_bMarbleSeamlessTexture;
				m_iMarbleColorVariance   = Dlg.m_iMarbleColorVariance;
				m_vMarbleScale[0]        = Dlg.m_vMarbleScale[0];
				m_vMarbleScale[1]        = Dlg.m_vMarbleScale[1];
				m_vMarbleScale[2]        = Dlg.m_vMarbleScale[2];
				m_vMarbleOffset[0]       = Dlg.m_vMarbleOffset[0];
				m_vMarbleOffset[1]       = Dlg.m_vMarbleOffset[1];
				m_vMarbleOffset[2]       = Dlg.m_vMarbleOffset[2];
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_MARBLE_TILE:
		{
			CMarbleTileDlg Dlg;

			Dlg.m_pLayer      = pLayer;
			Dlg.m_pFilter     = this;
			Dlg.m_iRandomSeed = m_iMarbleTileSeed;

			switch (m_iMarbleTileSize)
			{
			case TILE_SIZE_ANY:
				Dlg.m_iMarbleTileSize = 0;
				break;
			case TILE_TINY:
				Dlg.m_iMarbleTileSize = 1;
				break;
			case TILE_SMALL:
				Dlg.m_iMarbleTileSize = 2;
				break;
			case TILE_MEDIUM:
				Dlg.m_iMarbleTileSize = 3;
				break;
			case TILE_LARGE:
				Dlg.m_iMarbleTileSize = 4;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iMarbleTileSize = 0;
			};

			switch (m_iMarbleTileShape)
			{
			case TILE_ANY_SHAPE:
				Dlg.m_iMarbleTileShape = 0;
				break;
			case TILE_SQUARE:
				Dlg.m_iMarbleTileShape = 1;
				break;
			case TILE_RECTANGULAR:
				Dlg.m_iMarbleTileShape = 2;
				break;
			case TILE_HEXAGONAL:
				Dlg.m_iMarbleTileShape = 3;
				break;
			case TILE_OTHER_SHAPE:
				Dlg.m_iMarbleTileShape = 4;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iMarbleTileShape = 0;
			};

			switch (m_iMarbleTileColor)
			{
			case TILE_COLOR_ANY:
				Dlg.m_iMarbleTileColor = 0;
				break;
			case TILE_COLOR_SOLID:
				Dlg.m_iMarbleTileColor = 1;
				break;
			case TILE_COLOR_ACCENTED:
				Dlg.m_iMarbleTileColor = 2;
				break;
			case TILE_COLOR_TWO_TONE:
				Dlg.m_iMarbleTileColor = 3;
				break;
			case TILE_COLOR_MULTI:
				Dlg.m_iMarbleTileColor = 4;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iMarbleTileColor = 0;
			};

			switch (m_iMarbleTileStyle)
			{
			case TILE_STYLE_ANY:
				Dlg.m_iMarbleTileStyle = 0;
				break;
			case TILE_STYLE_PLAIN:
				Dlg.m_iMarbleTileStyle = 1;
				break;
			case TILE_STYLE_FANCY:
				Dlg.m_iMarbleTileStyle = 2;
				break;
			case TILE_STYLE_EXTRA_FANCY:
				Dlg.m_iMarbleTileStyle = 3;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iMarbleTileStyle = 0;
			};

			switch (m_iMarbleTileGroutType)
			{
			case TILE_GROUT_ANY:
				Dlg.m_iGroutType = 0;
				break;
			case TILE_GROUT_NONE:
				Dlg.m_iGroutType = 1;
				break;
			case TILE_GROUT_THIN_SMOOTH:
				Dlg.m_iGroutType = 2;
				break;
			case TILE_GROUT_THIN_ROUGH:
				Dlg.m_iGroutType = 3;
				break;
			case TILE_GROUT_THICK_SMOOTH:
				Dlg.m_iGroutType = 4;
				break;
			case TILE_GROUT_THICK_ROUGH:
				Dlg.m_iGroutType = 5;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iGroutType = 0;
			};

			switch (m_iMarbleTileBevelType)
			{
			case TILE_BEVEL_ANY:
				Dlg.m_iBevelType = 0;
				break;
			case TILE_BEVEL_NONE:
				Dlg.m_iBevelType = 1;
				break;
			case TILE_BEVEL_SHALLOW:
				Dlg.m_iBevelType = 2;
				break;
			case TILE_BEVEL_STANDARD:
				Dlg.m_iBevelType = 3;
				break;
			case TILE_BEVEL_DEEP:
				Dlg.m_iBevelType = 4;
				break;
			default:
				ASSERT( FALSE);
				Dlg.m_iBevelType = 0;
			};

			if (m_iMarbleTileColorGrout == -1)
				m_iMarbleTileColorGrout = pDoc->FindNearestColor( 64, 64, 64, FALSE);

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				// neal - look up tables (simplifes code)
				int iSizeArray[5] = { TILE_SIZE_ANY, TILE_TINY, 
					TILE_SMALL, TILE_MEDIUM, TILE_LARGE};
				int iShapeArray[5] = { TILE_ANY_SHAPE, TILE_SQUARE, 
					TILE_RECTANGULAR, TILE_HEXAGONAL, TILE_OTHER_SHAPE};
				int iColorArray[5] = { TILE_COLOR_ANY, TILE_COLOR_SOLID, 
					TILE_COLOR_ACCENTED, TILE_COLOR_TWO_TONE, TILE_COLOR_MULTI};
				int iStyleArray[4] = { TILE_STYLE_ANY, TILE_STYLE_PLAIN, 
					TILE_STYLE_FANCY, TILE_STYLE_EXTRA_FANCY};
				int iGroutArray[6] = { TILE_GROUT_ANY, TILE_GROUT_NONE, 
					TILE_GROUT_THIN_SMOOTH, TILE_GROUT_THIN_ROUGH, 
					TILE_GROUT_THICK_SMOOTH, TILE_GROUT_THICK_ROUGH};
				int iBevelArray[5] = { TILE_BEVEL_ANY, TILE_BEVEL_NONE, 
					TILE_BEVEL_SHALLOW, TILE_BEVEL_STANDARD, TILE_BEVEL_DEEP};

				m_iMarbleTileSeed = Dlg.m_iRandomSeed;

				if ((Dlg.m_iMarbleTileSize < 0) || (Dlg.m_iMarbleTileSize > 4))
				{
					ASSERT( FALSE);
					Dlg.m_iMarbleTileSize = 3;		// medium
				}
				if ((Dlg.m_iMarbleTileShape < 0) || (Dlg.m_iMarbleTileShape > 4))
				{
					ASSERT( FALSE);
					Dlg.m_iMarbleTileShape = 1;		// square
				}
				if ((Dlg.m_iMarbleTileColor < 0) || (Dlg.m_iMarbleTileColor > 4))
				{
					ASSERT( FALSE);
					Dlg.m_iMarbleTileColor = 1;		// solid
				}
				if ((Dlg.m_iMarbleTileStyle < 0) || (Dlg.m_iMarbleTileStyle > 3))
				{
					ASSERT( FALSE);
					Dlg.m_iMarbleTileStyle = 1;		// plain
				}
				if ((Dlg.m_iGroutType < 0) || (Dlg.m_iGroutType > 5))
				{
					ASSERT( FALSE);
					Dlg.m_iGroutType = 3;			// thin and rough
				}
				if ((Dlg.m_iBevelType < 0) || (Dlg.m_iBevelType > 4))
				{
					ASSERT( FALSE);
					Dlg.m_iBevelType = 3;			// standard bevel
				}

				m_iMarbleTileSize      = iSizeArray[ Dlg.m_iMarbleTileSize];
				m_iMarbleTileShape     = iShapeArray[ Dlg.m_iMarbleTileShape];
				m_iMarbleTileColor     = iColorArray[ Dlg.m_iMarbleTileColor];
				m_iMarbleTileStyle     = iStyleArray[ Dlg.m_iMarbleTileStyle];
				m_iMarbleTileGroutType = iGroutArray[ Dlg.m_iGroutType];
				m_iMarbleTileBevelType = iBevelArray[ Dlg.m_iBevelType];
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_MIRROR:
		{
			CMirrorDlg Dlg;
			Dlg.m_pLayer      = pLayer;
			Dlg.m_iMirrorType = m_iMirrorType;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iMirrorType = Dlg.m_iMirrorType;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_OFFSET:
		{
			COffsetDlg Dlg;
			Dlg.m_pLayer = pLayer;

			// no other settings needed - automatically
			// uses half the width and height of the image

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iOffsetX = Dlg.m_iOffsetX;
				m_iOffsetY = Dlg.m_iOffsetY;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_REDUCE_COLORS:
		{
			CReduceColorsDlg Dlg;

			Dlg.m_pLayer         = pLayer;
			Dlg.m_iMaxColors     = m_iReduceColorsMaxNum;
			Dlg.m_iReserveColor1 = m_iReserveColor[0];
			Dlg.m_iReserveColor2 = m_iReserveColor[1];
			Dlg.m_iReserveColor3 = m_iReserveColor[2];
			Dlg.m_iReserveColor4 = m_iReserveColor[3];

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iReduceColorsMaxNum = Dlg.m_iMaxColors;
				m_iReserveColor[0]    = Dlg.m_iReserveColor1;
				m_iReserveColor[1]    = Dlg.m_iReserveColor2;
				m_iReserveColor[2]    = Dlg.m_iReserveColor3;
				m_iReserveColor[3]    = Dlg.m_iReserveColor4;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_RESIZE:
		{
			CResizeDlg Dlg;
			Dlg.m_pLayer      = pLayer;
			Dlg.m_iWidth      = m_iResizeWidth;
			Dlg.m_iHeight     = m_iResizeHeight;
			Dlg.m_bProportial = m_bResizeProportionally;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iResizeType           = Dlg.m_iResizeType;
				m_iResizeWidth          = Dlg.m_iWidth;
				m_iResizeHeight         = Dlg.m_iHeight;
				m_bResizeProportionally = Dlg.m_bProportial;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_SHARPEN:
		{
			CSharpenDlg Dlg;
			Dlg.m_pLayer         = pLayer;
			Dlg.m_iSharpenAmount = m_iSharpenAmount;

			iRet = Dlg.DoModal();
			if (iRet == IDOK)
			{
				m_iSharpenAmount = Dlg.m_iSharpenAmount;
			}
			bPreviewApplied = Dlg.m_bPreviewApplied;
		}
		break;

	case FILTER_TILED_TINT:
		{
			iRet = IDOK;
		}
		break;

	case FILTER_HISTOGRAM:
	default:
		ASSERT( FALSE);
		break;
	}

	if (iRet == IDOK)
	{
		// update registry imediately (in case Wally dies later)

		CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();
		pWndMain->SaveWindowState();
	}

	if ((iRet == IDOK) && (! bSettingsChanged))
	{
		if (bPreviewApplied)
			iRet = IDCANCEL;		// we've already applied the filter
	}
	else if (bPreviewApplied)
	{
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);
	}

	return iRet;
};

void CFilter::DoFilter( CWallyView* pView, int iFilterType, BOOL bShowSettingsDlg)
{
	ASSERT( pView != NULL);
	m_pView = pView;

	CWallyDoc* pDoc = pView->GetDocument();
	ASSERT( pDoc != NULL);

	if (! pView)
	{
		MessageBeep( MB_ICONEXCLAMATION);
		return;
	}

	CSelection* pSel   = pDoc->GetSelection();
	CLayer*     pLayer = pSel->GetLayer();

	pLayer->m_pDoc  = pDoc;
//	pLayer->m_pView = pView;

	switch (iFilterType)
	{
	case FILTER_REPEAT_LAST_0:
		iFilterType = m_iRepeatLastFilterID[0];
		break;
	case FILTER_REPEAT_LAST_1:
		iFilterType = m_iRepeatLastFilterID[1];
		break;
	case FILTER_REPEAT_LAST_2:
		iFilterType = m_iRepeatLastFilterID[2];
		break;
	case FILTER_REPEAT_LAST_3:
		iFilterType = m_iRepeatLastFilterID[3];
		break;
	default:
		break;
	}

	m_iFilterType = iFilterType;

	if (bShowSettingsDlg)
		if (IDOK != ShowSettingsDlg( pLayer))
		{
			return;		// canceled by user
		}

	switch (iFilterType)
	{
	case FILTER_ADD_NOISE:
		DoAddNoise( pLayer, m_iAddNoiseAmount, m_iAddNoiseDistribution, 
			m_iAddNoiseType, m_iAddNoiseMinLength, m_iAddNoiseMaxLength, m_bMonochromeAddNoise);
		break;

	case FILTER_BLEND:
		DoBlend( pLayer, m_iBlendAmount);
		break;

	case FILTER_BRIGHTNESS_CONTRAST:
		DoBrightnessContrast( pLayer, m_iBrightnessAmount, m_iContrastAmount);
		break;

	case FILTER_EMBOSS:
		DoEmboss( pLayer, m_iEmbossAmount, m_bEmbossMonochrome);
		break;

	case FILTER_EDGE_DETECT:
		DoEdgeDetect( pLayer, m_iEdgeDetectAmount, m_rgbEdgeDetectColor, m_bEdgeDetectMonochrome);
		break;

	case FILTER_FIX_UNEVEN_LIGHTING:
		DoFixUnevenLighting( pLayer, m_iFixUnevenLightingAmount);
		break;

	case FILTER_MARBLE:
	case FILTER_MARBLE_TILE:
		{
//			Vec vScale  = { 1.0, 1.0, 1.0 };
//			Vec vOffset = { 0.0, 0.0, 0.0 };

//			g_Filter.DoMarbleTex( pDoc, m_vMarbleScale, m_vMarbleOffset,
//				m_dfMarbleAmplitude, m_iMarbleLevels, m_iMarbleNumThickStripes, 
//				m_iMarbleNumThinStripes, m_iMarbleColorVariance, m_iMarbleSeed, 
//				m_bMarbleSeamlessTexture, m_bMarbleExtraSmooth, FALSE);

			MarbleParams Params;

			Params.pLayer           = pLayer;
			Params.iRandSeed        = m_iMarbleSeed;
			Params.iLevels          = m_iMarbleLevels;
			Params.iNumThickStripes = m_iMarbleNumThickStripes;
			Params.iNumThinStripes  = m_iMarbleNumThinStripes;
			Params.iColorVariance   = m_iMarbleColorVariance;

			Params.ptOffset = CPoint( 0, 0);
			Params.sizeTile = CSize( 0, 0);

//			Params.iShape   = TILE_SQUARE;
			Params.iShape   = (iFilterType == FILTER_MARBLE) ? 
									TILE_SEAMLESS : m_iMarbleTileShape;

			Params.iStyle   = PLAIN_ALIGNED;
			Params.bMirrorX = FALSE;
			Params.bMirrorY = FALSE;

			Params.bSeamless    = m_bMarbleSeamlessTexture;
			Params.bExtraSmooth = m_bMarbleExtraSmooth;

			Params.dfAmplitude = m_dfMarbleAmplitude;

			Params.vScale[0] = m_vMarbleScale[0];
			Params.vScale[1] = m_vMarbleScale[1];
			Params.vScale[2] = m_vMarbleScale[2];

			Params.vOffset[0] = m_vMarbleOffset[0];
			Params.vOffset[1] = m_vMarbleOffset[1];
			Params.vOffset[2] = m_vMarbleOffset[2];

			Params.iARGB_PrimaryField  = NULL;
			Params.iARGB_ContrastField = NULL;
			Params.iARGB_AccentField   = NULL;
			Params.pbyBlockStencilData = NULL;

			Params.iTileRandSeed  = m_iMarbleTileSeed;
			Params.iTileSize      = m_iMarbleTileSize;

			Params.iTileColor     = m_iMarbleTileColor;
			Params.iTileStyle     = m_iMarbleTileStyle;
			Params.iTileGroutType = m_iMarbleTileGroutType;
			Params.iTileBevelType = m_iMarbleTileBevelType;

			Params.bFastPreview = FALSE;

			g_Filter.DoMarbleTex( &Params);
		}
		break;

	case FILTER_DIFFUSE:
		DoPixelFilter( pLayer, iFilterType, m_iDiffuseAmount);
		break;

	case FILTER_MIRROR:
		DoPixelFilter( pLayer, iFilterType, m_iMirrorType);
		break;

	case FILTER_OFFSET:
		DoPixelFilter( pLayer, iFilterType, m_iOffsetX, m_iOffsetY);
		break;

	case FILTER_FLIP_HORIZONTAL:
	case FILTER_FLIP_VERTICAL:
		DoPixelFilter( pLayer, iFilterType, 0);
		break;

	case FILTER_REDUCE_COLORS:
		DoReduceColors( pLayer, m_iReduceColorsMaxNum, m_iReserveColor);
		break;

	case FILTER_RESIZE:
		if (m_iResizeType == 0)		// any size
			DoArbitraryResize( pLayer, m_iResizeWidth, m_iResizeHeight);
		else
			DoResize( pLayer, m_iResizeType, m_iResizeAmount);
		break;

	case FILTER_SHARPEN:
		DoSharpen( pLayer, m_iSharpenAmount);
		break;

	case FILTER_TILED_TINT:
		DoTiledTint( pLayer );
		break;

	//case FILTER_CONVERT_TO_GRAY:
	case FILTER_HISTOGRAM:
	default:
		ASSERT( FALSE);
		break;
	}
	m_pView = NULL;
};

inline void LookupPalColor( CWallyPalette* pPal, int iIndexColor, int* piRed, int* piGreen, int* piBlue)
{
	*piRed   = pPal->GetR( iIndexColor);
	*piGreen = pPal->GetG( iIndexColor);
	*piBlue  = pPal->GetB( iIndexColor);
}

//inline void AddLookupPalColor( CWallyPalette* pPal, int iIndexColor, int iMult, int* piRed, int* piGreen, int* piBlue)
//{
//	*piRed   += iMult * pPal->GetR( iIndexColor);
//	*piGreen += iMult * pPal->GetG( iIndexColor);
//	*piBlue  += iMult * pPal->GetB( iIndexColor);
//}

inline void AddColor( COLORREF rgbColor, int iMult, int* piRed, int* piGreen, int* piBlue)
{
	*piRed   += iMult * GetRValue( rgbColor);
	*piGreen += iMult * GetGValue( rgbColor);
	*piBlue  += iMult * GetBValue( rgbColor);
}

inline int CalcGray( COLORREF rgbColor)
{
	int iRed   = GetRValue( rgbColor);
	int iGreen = GetGValue( rgbColor);
	int iBlue  = GetBValue( rgbColor);

	// standard TV definition of gray
	// (weighted based on human eye's color response)

	int iGray = (iGreen * 59 + iRed * 30 + iBlue * 11) / 100;
	return (iGray);
}

inline BOOL CFilter::WithinSelection( int /*iX*/, int /*iY*/)
{
	if (m_bUseSelection)
	{
		// Neal - TODO: implement for non-rectangular selections

		// Neal - there is no need to ensure iX,iY is in selection rect,
		//        since this funtion should only be called in the 
		//        inner-most filtering loop (which should only process
		//        the pixels within selection rect)
		return TRUE;
	}
	return TRUE;	// always TRUE if no selection
}

BOOL CFilter::PrepareSoloFilter( CLayer *pLayer, int iFilterType, LPCSTR lpszFilterName)
{
	g_iWorking               = 0;
	g_iTotalPixels           = 0;
	g_dwTickCount            = GetTickCount();
	m_bNeedsFindNearestColor = FALSE;

	if (m_pIRGB_SrcBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_SrcBuffer);
		m_pIRGB_SrcBuffer = NULL;
	}
	if (m_pIRGB_DestBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_DestBuffer);
		m_pIRGB_DestBuffer = NULL;
	}

	///////////////////////////////////////////////////////////
	// build a full IndexRedGreenBlue source for the filter //
	/////////////////////////////////////////////////////////

	int iWidth  = pLayer->GetWidth();
	int iHeight = pLayer->GetHeight();

	m_rSrc.top    = 0;
	m_rSrc.left   = 0;
	m_rSrc.right  = iWidth;
	m_rSrc.bottom = iHeight;

	m_rDest = m_rSrc;

	m_pIRGB_SrcBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	COLOR_IRGB*    pIRGB_Src     = m_pIRGB_SrcBuffer;

	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidth; i++)
		{
			*pIRGB_Src = pLayer->GetPixel( i, j);
			pIRGB_Src++;
		}
	}

	m_rSelection.SetRect( 0, 0, iWidth, iHeight);
	
	return (m_pIRGB_SrcBuffer != NULL);
}

BOOL CFilter::PrepareFilter( CLayer* pLayer, int iFilterType, LPCSTR lpszFilterName, int iUndoType)
{
	// Neal - add the filter to the most Recently Used Filters menu

	if (iFilterType < FILTER_MARBLE)
	{
		int i, j;

		// shift them up to make space in front

		for (i = 4; i >= 1; i--)
		{
			m_strRepeatLast[i]       = m_strRepeatLast[i-1];
			m_iRepeatLastFilterID[i] = m_iRepeatLastFilterID[i-1];
		}

		if ((iFilterType >= FILTER_MIRROR) && (iFilterType <= MIRROR_UPPER_LEFT_TO_REST))
		{
			m_strRepeatLast[0]       = "Mirror";
			m_iRepeatLastFilterID[0] = FILTER_MIRROR;
		}
		else 
		{
			m_strRepeatLast[0]       = lpszFilterName;
			m_iRepeatLastFilterID[0] = iFilterType;
		}

		for (i = 1; i < 4; i++)
		{
			if (m_iRepeatLastFilterID[i] == m_iRepeatLastFilterID[0])
			{
				// shift back down (don't want duplicates)

				for (j = i; j < 4; j++)
				{
					m_strRepeatLast[j]       = m_strRepeatLast[j+1];
					m_iRepeatLastFilterID[j] = m_iRepeatLastFilterID[j+1];
				}
				break;
			}
		}
	}

	// Neal - initialize and free up old memory

	g_iWorking               = 0;
	g_iTotalPixels           = 0;
	g_dwTickCount            = GetTickCount();
	m_bNeedsFindNearestColor = FALSE;

	if (m_pIRGB_SrcBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_SrcBuffer);
		m_pIRGB_SrcBuffer = NULL;
	}
	if (m_pIRGB_DestBuffer)
	{
		ASSERT( FALSE);
		free( m_pIRGB_DestBuffer);
		m_pIRGB_DestBuffer = NULL;
	}

	CWallyDoc* pDoc = NULL;
	if (pLayer)
		pDoc = pLayer->GetDoc();

	if (pDoc)
	{
#ifdef _DEBUG
		pDoc->AssertValid();
#endif
		pDoc->BeginWaitCursor();
		pDoc->m_Undo.SaveUndoState( pDoc, lpszFilterName, iUndoType);

		///////////////////////////////////////////////////////////
		// build a full IndexRedGreenBlue source for the filter //
		/////////////////////////////////////////////////////////

		int iWidth  = pDoc->Width();
		int iHeight = pDoc->Height();

		m_rSrc.top    = 0;
		m_rSrc.left   = 0;
		m_rSrc.right  = iWidth;
		m_rSrc.bottom = iHeight;

		m_rDest = m_rSrc;

		m_pIRGB_SrcBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

		COLOR_IRGB*    pIRGB_Src     = m_pIRGB_SrcBuffer;
		CWallyPalette* pPal          = pDoc->GetPalette();

		for (int j = 0; j < iHeight; j++)
		{
			for (int i = 0; i < iWidth; i++)
			{
				*pIRGB_Src = pDoc->GetPixel( i, j);
				pIRGB_Src++;
			}
		}

		// TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//		m_bUseSelection = TRUE;
//		m_rSelection.SetRect( iWidth/4, iHeight/4, iWidth*3/4, iHeight*3/4);

		m_bUseSelection = pDoc->HasSelection();
		if (m_bUseSelection)
		{
			CSelection* pSel = pDoc->GetSelection();
			m_rSelection     = pSel->GetBoundsRect();
		}
		else
		{
			m_rSelection.SetRect( 0, 0, iWidth, iHeight);
		}
	}
	else
	{
		MessageBeep( MB_ICONEXCLAMATION);
		ASSERT( FALSE);
	}

	return (m_pIRGB_SrcBuffer != NULL);
}

void CFilter::EndSoloFilter()
{
	if (m_pIRGB_SrcBuffer)
	{
		free( m_pIRGB_SrcBuffer);
		m_pIRGB_SrcBuffer = NULL;
	}

	if (m_pIRGB_DestBuffer)
	{
		free( m_pIRGB_DestBuffer);
		m_pIRGB_DestBuffer = NULL;
	}
}

void CFilter::EndFilter( CLayer* pLayer)
{
	ASSERT( pLayer);
	ASSERT( m_pIRGB_SrcBuffer != NULL);

	if (m_pIRGB_SrcBuffer)
	{
		//////////////////////////////////////////
		// convert back to 256 color (indexes) //
		////////////////////////////////////////

		int            i, j;
		COLOR_IRGB*    pIRGB_Src = NULL;
		CWallyPalette* pPal      = pLayer->GetDoc()->GetPalette();
//		BOOL           bResized  = FALSE;

		int iWidth  = m_rDest.Width();
		int iHeight = m_rDest.Height();

		// handle dest buffer size != pDoc->size

		CWallyDoc* pDoc = pLayer->GetDoc();

		// Neal - did we resize the layer?

		if (pDoc && pLayer && ((pLayer->GetWidth() != iWidth) ||
				(pLayer->GetHeight() != iHeight)))
		{
			pDoc->DeleteMips();

			TRY
			{
				// Neal - fixes resize bug

				pLayer->FreeMem();		// prevents asserts
				pDoc->SetDimensions( iWidth, iHeight);
//				pDoc->RebuildMipsArray( false);	// these will now be correct size
				pDoc->RebuildLayerAndMips( false);	// these will now be correct size

//				pDoc->UpdateAllViews( NULL, HINT_UPDATE_DIBS, NULL);	// Neal - new size - update pView->m_DIB
			}
			CATCH_ALL( e)
			{
			}
			END_CATCH_ALL

			// NEAL-TEST

//			pDoc->UpdateAllDIBs();	// TEST <<<<<<<<<<<<<<<<<<<<
//			bResized = TRUE;
		}

		// if there's a dest buffer use that instead

		if (m_pIRGB_DestBuffer)
			pIRGB_Src = m_pIRGB_DestBuffer;
		else
			pIRGB_Src = m_pIRGB_SrcBuffer;

//		CWallyView* pView = pLayer->GetView();

		for (j = 0; j < iHeight; j++)
		{
			for (i = 0; i < iWidth; i++)
			{
				if ((j >= m_rSelection.top) && (j < m_rSelection.bottom) &&
							(i >= m_rSelection.left) && (i < m_rSelection.right) &&
							(WithinSelection( i, j)))
				{
					COLOR_IRGB irgb = pIRGB_Src[(j * iWidth) + i];

					if (m_bNeedsFindNearestColor)
					{
						pDoc->SetNearestColorPixel( m_pView, i, j, irgb);
					}
					else
					{
						pDoc->SetPixel( m_pView, i, j, irgb);
					}
				}
			}
		}
		free( m_pIRGB_SrcBuffer);

		m_pIRGB_SrcBuffer = NULL;
	}
	if (m_pIRGB_DestBuffer)
	{
		free( m_pIRGB_DestBuffer);
		m_pIRGB_DestBuffer = NULL;
	}

	CWallyDoc* pDoc = pLayer->GetDoc();

	if (pDoc)
	{
#ifdef _DEBUG
		pDoc->AssertValid();
#endif
		// Neal - this fixes effects-tool-filter-draw bug
		pDoc->FreeEffectsData();

		// always rebuild the SubMips after a filter

		pDoc->SetModifiedFlag();
		pDoc->CopyLayerToMip();

		// Neal - sometimes we skip this as a speed-up
		if (m_bRebuildSubMips)
			pDoc->UpdateAllDIBs( TRUE);

		pDoc->EndWaitCursor();
	}
	else
	{
		ASSERT( FALSE);
	}

	m_wndStatusBar.SetPaneText( 0, "", true);
}

// now 24 bit
void CFilter::DoBlend( CLayer* pLayer, int iBlendAmount)
{
	PrepareFilter( pLayer, FILTER_BLEND, "Blend Filter");

	if (! pLayer)
		return;

	int iMult = 8;
	int iDist = 5;

	switch (iBlendAmount)
	{
	case FILTER_VERY_LIGHT:
		iDist = 32;
		iMult = 32;
		break;
	case FILTER_LIGHT:
		iDist = 24;
		iMult = 16;
		break;
	case FILTER_MEDIUM:
		iDist = 16;
		iMult = 8;
		break;
	case FILTER_HEAVY:
		iDist = 8;
		iMult = 4;
		break;
	case FILTER_VERY_HEAVY:
		iDist = 4;
		iMult = 2;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	int i, j, ix, iy;
	int iRed, iGreen, iBlue;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j-1;
				int iAbove = (iy >= 0) ? iy : (iHeight-1);
				iy = j+1;
				int iBelow = (iy < iHeight) ? iy : 0;

				iy = j;
				ix = i-1;
				int iLeft  = (ix >= 0) ? ix : (iWidth-1);
				ix = i+1;
				int iRight = (ix < iWidth) ? ix : 0;

				//////////////////////////////////////////////////
				// use weighted average for best looking blend //
				////////////////////////////////////////////////

				// center pixel
				ix = i;
				iy = j;
				COLORREF rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   = GetRValue( rgbColor) * iMult;
				iGreen = GetGValue( rgbColor) * iMult;
				iBlue  = GetBValue( rgbColor) * iMult;

				// horizontal and vertical neighbors
				ix = iLeft;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);

				ix = i;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);

				ix = i;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);


				// diagonal neighbors
				ix = iLeft;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ix = iLeft;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				iRed   /= iMult + 4*2 + 4;
				iGreen /= iMult + 4*2 + 4;
				iBlue  /= iMult + 4*2 + 4;

				//m_pIRGB_DestBuffer[ ((j % iHeight) * iWidth) + (i % iWidth)] = 
				//		IRGB( 0, iRed, iGreen, iBlue);
				m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}
	}

	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

// now 24 bit
void CFilter::DoSharpen( CLayer* pLayer, int iSharpenAmount)
{
	BOOL bCombinedOperation = (m_pIRGB_SrcBuffer != NULL);

	if (! bCombinedOperation)
		PrepareFilter( pLayer, FILTER_SHARPEN, "Sharpen Filter");

	if (! pLayer)
		return;

	int iWeight = -1;	// standard amount

	switch (iSharpenAmount)
	{
	case FILTER_VERY_LIGHT:
		iWeight = -1;
		break;
	case FILTER_LIGHT:
		iWeight = -2;
		break;
	case FILTER_MEDIUM:
		iWeight = -4;
		break;
	case FILTER_HEAVY:
		iWeight = -8;
		break;
	case FILTER_VERY_HEAVY:
		iWeight = -16;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	// neal - iMult MUST == iWeight * 4 for sharpen to work
	// because there are four neighbors (up, down, left, right)

	int iMult = -(iWeight * 4);		// center pixel multiplier

	int i, j, ix, iy;
	int iRed, iGreen, iBlue;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	ASSERT( m_pIRGB_DestBuffer == NULL);

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j-1;
				int iAbove = (iy >= 0) ? iy : (iHeight-1);
				iy = j+1;
				int iBelow = (iy < iHeight) ? iy : 0;

				iy = j;
				ix = i-1;
				int iLeft  = (ix >= 0) ? ix : (iWidth-1);
				ix = i+1;
				int iRight = (ix < iWidth) ? ix : 0;

				//////////////////////////////////////////////////
				// use weighted average for best looking blend //
				////////////////////////////////////////////////

				// center pixel
				ix = i;
				iy = j;
				COLORREF rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   = GetRValue( rgbColor) * iMult;
				iGreen = GetGValue( rgbColor) * iMult;
				iBlue  = GetBValue( rgbColor) * iMult;

				// horizontal and vertical neighbors
				ix = iLeft;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

				ix = i;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

				ix = i;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

				// scale down difference, so we can have "light" sharpening
				iRed   /= 8;
				iGreen /= 8;
				iBlue  /= 8;

				// add center pixel to edge difference
				ix = i;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ClampRGB( &iRed, &iGreen, &iBlue);

//				m_pIRGB_DestBuffer[ ((j % iHeight) * iWidth) + (i % iWidth)] = 
//						IRGB( 0, iRed, iGreen, iBlue);
				m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}
	}
	m_bNeedsFindNearestColor = TRUE;

	if (! bCombinedOperation)
		EndFilter( pLayer);
};

// now 24 bit
void CFilter::DoBrightnessContrast( CLayer* pLayer, 
			int iBrightenAmount, int iContrastAmount)
{
	PrepareFilter( pLayer, FILTER_BRIGHTNESS_CONTRAST, "Brightness/Contrast Filter");

	if (! pLayer)
		return;

	int    i, j;
	int    iRed, iGreen, iBlue, iColor;
	double dfContrast;

	int    iWidth  = m_rSrc.Width();
	int    iHeight = m_rSrc.Height();

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				COLOR_IRGB irgb = m_pIRGB_SrcBuffer[(j * iWidth) + i];

				iColor = GetIValue( irgb);

				if (iColor != 255)	// transparency
				{
					iRed   = GetRValue( irgb);
					iGreen = GetGValue( irgb);
					iBlue  = GetBValue( irgb);

					dfContrast = 1.0 + (iContrastAmount / 50.0);

					iRed   = 128 + (int )((iRed   - 128) * dfContrast) + iBrightenAmount;
					iGreen = 128 + (int )((iGreen - 128) * dfContrast) + iBrightenAmount;
					iBlue  = 128 + (int )((iBlue  - 128) * dfContrast) + iBrightenAmount;

					ClampRGB( &iRed, &iGreen, &iBlue);
					m_pIRGB_SrcBuffer[(j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
				}
			}
		}
	}
	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

inline int CalcNoise( int iAmount, int iDistribution)
{
	ASSERT( (iDistribution >= NOISE_UNIFORM) && (iDistribution <= NOISE_GAUSSIAN));

	int iRand = (int )((Random()+0.0001) * iAmount);

	if (iDistribution == NOISE_GAUSSIAN)
	{
		iRand += (int )((Random()+0.0001) * iAmount);
		iRand /= 2;
	}

	if (Random() < 0.5)
		iRand = -iRand;

	return iRand;
}

// now 24 bit
void CFilter::DoAddNoise( CLayer* pLayer, int iAddNoiseAmount, int iDistribution, 
			int iAddNoiseType, int iAddNoiseMinLength, int iAddNoiseMaxLength, BOOL bMonochrome)
{
	PrepareFilter( pLayer, FILTER_ADD_NOISE, "Add Noise Filter");

	if (! pLayer)
		return;

	int i, j;
	int iRed, iGreen, iBlue;
	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	if (iAddNoiseMinLength > iAddNoiseMaxLength)
	{
		i                  = iAddNoiseMinLength;   // swap 'em
		iAddNoiseMinLength = iAddNoiseMaxLength;
		iAddNoiseMaxLength = i;
	}

	int iAmount = 8;

	switch (iAddNoiseAmount)
	{
	case FILTER_VERY_LIGHT:
		iAmount = 4;
		break;
	case FILTER_LIGHT:
		iAmount = 8;
		break;
	case FILTER_MEDIUM:
		iAmount = 16;
		break;
	case FILTER_HEAVY:
		iAmount = 32;
		break;
	case FILTER_VERY_HEAVY:
		iAmount = 64;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	if (iAddNoiseType == NOISE_SINGLE_PIXEL)
	{
		iAddNoiseMinLength = iAddNoiseMaxLength = 1;
	}
	
	CPoint ptStart;
	ptStart.x = (int )(Random() * iWidth);
	ptStart.y = (int )(Random() * iHeight);

	i = ptStart.x;
	j = ptStart.y;

	CPoint ptWrap( ptStart);

	int iLen = 0;
	int iRandR, iRandG, iRandB;

	while (TRUE)
	{
		if (iLen <= 0)
		{
			iLen = iAddNoiseMinLength + (int )(Random() * 
					(iAddNoiseMaxLength - iAddNoiseMinLength));

			iRandR = CalcNoise( iAmount, iDistribution);

			if (bMonochrome)
			{
				iRandG = iRandB = iRandR;
			}
			else
			{
				iRandG = CalcNoise( iAmount, iDistribution);
				iRandB = CalcNoise( iAmount, iDistribution);
			}
		}

		if ((j >= m_rSelection.top) && (j < m_rSelection.bottom) &&
					(i >= m_rSelection.left) && (i < m_rSelection.right) &&
					(WithinSelection( i, j)))
		{
			COLOR_IRGB irgb = m_pIRGB_SrcBuffer[ j * iWidth + i];

			if (GetIValue( irgb) != 255)	// transparency
			{
				iRed   = GetRValue( irgb) + iRandR;
				iGreen = GetGValue( irgb) + iRandG;
				iBlue  = GetBValue( irgb) + iRandB;

				ClampRGB( &iRed, &iGreen, &iBlue);
				m_pIRGB_SrcBuffer[j * iWidth + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}

		iLen--;

		if (iAddNoiseType == NOISE_VERTICAL_LINE)
		{
			j++;
			if (j >= iHeight)
				j -= iHeight;

			if (j == ptWrap.y)		// start a new line?
			{
				iLen = 0;
				j    = ptWrap.y = (int )(Random() * iHeight);

				i++;
				if (i >= iWidth)
					i -= iWidth;

				if (i == ptStart.x)
					break;
			}
		}
		else
		{
			i++;
			if (i >= iWidth)
				i -= iWidth;

			if (i == ptWrap.x)		// start a new line?
			{
				iLen = 0;
				i    = ptWrap.x = (int )(Random() * iWidth);

				j++;
				if (j >= iHeight)
					j -= iHeight;

				if (j == ptStart.y)
					break;
			}
		}
	}

	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

// now 24 bit
void CFilter::DoEmboss( CLayer* pLayer, int iEmbossAmount, BOOL bMonochrome)
{
	PrepareFilter( pLayer, FILTER_EMBOSS, "Emboss Filter");

	if (! pLayer)
		return;

	double dfMult = 1.0;

	switch (iEmbossAmount)
	{
	case FILTER_VERY_LIGHT:
		dfMult = 0.5;
		break;
	case FILTER_LIGHT:
		dfMult = 1.0;
		break;
	case FILTER_MEDIUM:
		dfMult = 2.0;
		break;
	case FILTER_HEAVY:
		dfMult = 4.0;
		break;
	case FILTER_VERY_HEAVY:
		dfMult = 8.0;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	int i, j, ix, iy;
	int iRed, iGreen, iBlue, iGray;
	int iR, iG, iB;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j-1;
				int iAbove = (iy >= 0) ? iy : (iHeight-1);
				iy = j+1;
				int iBelow = (iy < iHeight) ? iy : 0;

				iy = j;
				ix = i-1;
				int iLeft  = (ix >= 0) ? ix : (iWidth-1);
				ix = i+1;
				int iRight = (ix < iWidth) ? ix : 0;

				// use neighbors to generate highlights/shadows

				ix     = iLeft;
				iy     = iAbove;
				iGray  = CalcGray( m_pIRGB_SrcBuffer[(iy * iWidth) + ix]);
				iRed   = iGreen = iBlue = iGray;

				ix     = i;
				iy     = iAbove;
				iGray  = CalcGray( m_pIRGB_SrcBuffer[(iy * iWidth) + ix]);
				iR     = iG = iB = iGray;
				iR     = (2*iR + iRed)   / 3;
				iG     = (2*iG + iGreen) / 3;
				iB     = (2*iB + iBlue)  / 3;

				// center pixel
				ix     = i;
				iy     = j;
				iGray  = CalcGray( m_pIRGB_SrcBuffer[(iy * iWidth) + ix]);
				iRed   = iGreen = iBlue = iGray;

				// neal - this is standard gray emboss
				if (bMonochrome)
				{
					iR = 128 + iRed   - iR;
					iG = 128 + iGreen - iG;
					iB = 128 + iBlue  - iB;
				}
				else
				{
					// emboss over current image (variation)

					iR = iRed   - iR;
					iG = iGreen - iG;
					iB = iBlue  - iB;

					// desaturate existing pixel value
					// (for better looking effect)

					COLORREF rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
					iRed   = GetRValue( rgbColor);
					iGreen = GetGValue( rgbColor);
					iBlue  = GetBValue( rgbColor);

					iRed   = (128 + 3 * iRed)   / 4;
					iGreen = (128 + 3 * iGreen) / 4;
					iBlue  = (128 + 3 * iBlue)  / 4;

					iR = (int )(iRed   + (dfMult * iR));
					iG = (int )(iGreen + (dfMult * iG));
					iB = (int )(iBlue  + (dfMult * iB));
				}

				ClampRGB( &iR, &iG, &iB);
				m_pIRGB_DestBuffer[ ((j % iHeight) * iWidth) + (i % iWidth)] = 
						IRGB( 0, iR, iG, iB);
			}
		}
	}
	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

// now 24 bit
void CFilter::DoEdgeDetect( CLayer* pLayer, int iAmount, COLORREF rgbBackground, BOOL bMonochrome)
{
	PrepareFilter( pLayer, FILTER_EDGE_DETECT, "Edge Detect Filter");

	if (! pLayer)
		return;

	double dfMult = 1.0;

	switch (iAmount)
	{
	case FILTER_VERY_LIGHT:
		dfMult = 0.5;
		break;
	case FILTER_LIGHT:
		dfMult = 1.0;
		break;
	case FILTER_MEDIUM:
		dfMult = 2.0;
		break;
	case FILTER_HEAVY:
		dfMult = 4.0;
		break;
	case FILTER_VERY_HEAVY:
		dfMult = 8.0;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	int i, j, ix, iy;
	int iRed, iGreen, iBlue;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	// we test against background color below
	int iBackgroundR = GetRValue( rgbBackground);
	int iBackgroundG = GetGValue( rgbBackground);
	int iBackgroundB = GetBValue( rgbBackground);

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j-1;
				int iAbove = (iy >= 0) ? iy : (iHeight-1);
				iy = j+1;
				int iBelow = (iy < iHeight) ? iy : 0;

				iy = j;
				ix = i-1;
				int iLeft  = (ix >= 0) ? ix : (iWidth-1);
				ix = i+1;
				int iRight = (ix < iWidth) ? ix : 0;

				// use horizontal neighbors to detect edges
				ix = iLeft;
				iy = iAbove;
				COLORREF rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   = GetRValue( rgbColor);
				iGreen = GetGValue( rgbColor);
				iBlue  = GetBValue( rgbColor);

				ix = i;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ix = iLeft;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -1, &iRed, &iGreen, &iBlue);

				ix = i;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -2, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -1, &iRed, &iGreen, &iBlue);

				int iR = abs( iRed);
				int iG = abs( iGreen);
				int iB = abs( iBlue);

				// use vertical neighbors to detect edges
				ix = iLeft;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   = GetRValue( rgbColor);
				iGreen = GetGValue( rgbColor);
				iBlue  = GetBValue( rgbColor);

				ix = iLeft;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 2, &iRed, &iGreen, &iBlue);

				ix = iLeft;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iAbove;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -1, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -2, &iRed, &iGreen, &iBlue);

				ix = iRight;
				iy = iBelow;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, -1, &iRed, &iGreen, &iBlue);

				iRed   = (int )(dfMult * (abs( iRed)   + iR));
				iGreen = (int )(dfMult * (abs( iGreen) + iG));
				iBlue  = (int )(dfMult * (abs( iBlue)  + iB));

				// neal - this is gray edge detect
				if (bMonochrome)
				{
					// standard b/w tv conversion to monochrome

					iRed = iGreen = iBlue = 
							(59 * iGreen + 30 * iRed + 11 * iBlue) / 100;
				}

				// invert as necessary for best contrast
				if (iBackgroundR >= 128)
					iRed = -iRed;
				iRed += iBackgroundR;

				if (iBackgroundG >= 128)
					iGreen = -iGreen;
				iGreen += iBackgroundG;

				if (iBackgroundB >= 128)
					iBlue = -iBlue;
				iBlue += iBackgroundB;

				ClampRGB( &iRed, &iGreen, &iBlue);
				m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}
	}
	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

void CFilter::DoFixUnevenLighting( CLayer* pLayer, int iFixUnevenLightingAmount)
{
	PrepareFilter( pLayer, FILTER_FIX_UNEVEN_LIGHTING, "Fix Uneven Lighting Filter");

	if (! pLayer)
		return;

	int iDist = 24;

	switch (iFixUnevenLightingAmount)
	{
	case FILTER_VERY_LIGHT:
		iDist = 40;
		break;
	case FILTER_LIGHT:
		iDist = 32;
		break;
	case FILTER_MEDIUM:
		iDist = 24;
		break;
	case FILTER_HEAVY:
		iDist = 16;
		break;
	case FILTER_VERY_HEAVY:
		iDist = 8;
		break;
	default:
		ASSERT (FALSE);
		break;
	}

	int i, j, ix, iy;
	int iRed, iGreen, iBlue;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	//////////////////////////////////////
	// Neal - special "even-up" filter //
	////////////////////////////////////

	// This is the start of a new filter that will help even out the
	// lighting of a photo-realistic texture.  Nearly all such textures
	// have a corner or edge that is darker (or ligher) than the overall
	// texture.  This new filter will fix the lighting, so that the
	// imager can be made seamless (with much less effort).
	//
	// The results of this filter is OK as long as the entire texture
	// is approximately the same shade.  To improve the results, I
	// need to move the algorithm to the HSV domain and process the
	// brigtness channel only, instead of the current RGB approach.

	int iAvgR  = 0;
	int iAvgG  = 0;
	int iAvgB  = 0;
	int iTotal = 0;
	COLORREF rgbColor;

	iRed = iGreen = iBlue = 0;

	for (iy = m_rSelection.top; iy < m_rSelection.bottom; iy++)
	{
		for (ix = m_rSelection.left; ix < m_rSelection.right; ix++)
		{
			if (WithinSelection( ix, iy))
			{
				iTotal++;

				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   += GetRValue( rgbColor);
				iGreen += GetGValue( rgbColor);
				iBlue  += GetBValue( rgbColor);
			}
		}
	}
	iAvgR  = iRed   / iTotal;
	iAvgG  = iGreen / iTotal;
	iAvgB  = iBlue  / iTotal;

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				iTotal = iRed = iGreen = iBlue = 0;

				for (int yl = -iDist+j; yl <= iDist+j; yl++)
				{
					// mirror edges

					if (yl >= m_rSelection.top)
					{
						if (yl < m_rSelection.bottom)
							iy = yl;
						else
						{
							iy = j + m_rSelection.bottom - yl - 1;
						}
					}
					else
					{
						iy = j + m_rSelection.top - yl;
					}

					ASSERT( (iy >= m_rSelection.top) && (iy < m_rSelection.bottom));

					for (int xl = -iDist+i; xl <= iDist+i; xl++)
					{
						if (xl >= m_rSelection.left)
						{
							if (xl < m_rSelection.right)
								ix = xl;
							else
							{
								ix = i + m_rSelection.right - xl - 1;
							}
						}
						else
						{
							ix = i + m_rSelection.left - xl;
						}

						ASSERT( (ix >= m_rSelection.left) && (ix < m_rSelection.right));

						iTotal++;
						rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
						iRed   += GetRValue( rgbColor);
						iGreen += GetGValue( rgbColor);
						iBlue  += GetBValue( rgbColor);
					}
				}

				iRed   /= iTotal;
				iGreen /= iTotal;
				iBlue  /= iTotal;

//				ASSERT( (iRed   >= 0) && (iRed   <= 255));
//				ASSERT( (iGreen >= 0) && (iGreen <= 255));
//				ASSERT( (iBlue  >= 0) && (iBlue  <= 255));

				ix = i;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];

				iRed   = GetRValue( rgbColor) - (iRed   - iAvgR);
				iGreen = GetGValue( rgbColor) - (iGreen - iAvgG);
				iBlue  = GetBValue( rgbColor) - (iBlue  - iAvgB);

				iRed   = max( 0, iRed);
				iRed   = min( 255, iRed);
				iGreen = max( 0, iGreen);
				iGreen = min( 255, iGreen);
				iBlue  = max( 0, iBlue);
				iBlue  = min( 255, iBlue);

				m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}
	}

	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
};

// now 24 bit
void CFilter::DoPixelFilter( CLayer* pLayer, int iFilterType, int iAmount, int iAmount2)
{
	// Neal - this is usually done in PrepareFilter(), but we need to
	//        us it below (BUGFIX)
	//
	CWallyDoc* pDoc = pLayer->GetDoc();
	m_bUseSelection = pDoc->HasSelection();

	CString strName( "<Unknown>");

	switch (iFilterType)
	{
	case FILTER_DIFFUSE:
		strName = "Diffuse";

		switch (iAmount)
		{
		case 0:
			iAmount = 1;	// ± 1
			break;
		case 1:
		default:
			iAmount = 2;	// ± 2
			break;
		case 2:
			iAmount = 4;	// ± 4
			break;
		case 3:
			iAmount = 8;	// ± 8
			break;
		}
		break;

	case FILTER_MIRROR:
//		if (m_bUseSelection)
//		{
//			MessageBeep( 0);
//			ASSERT( FALSE);		// neal - TODO: implement
//			return;
//		}

		switch (iAmount)
		{
		case MIRROR_TOP_TO_BOTTOM:
			strName = "Mirror Top --> Bottom";
			break;
		case MIRROR_UPPER_LEFT_TO_REST:
			strName = "Mirror Upper Left --> Rest";
			break;
		default:
			ASSERT( FALSE);
			// drop thru
		case MIRROR_LEFT_TO_RIGHT:
			strName = "Mirror Left --> Right";
			break;
		}
		// mirror type must differ from diffuse for code below to work
		ASSERT( iAmount != FILTER_DIFFUSE);

		iFilterType = iAmount;
		break;

	case FILTER_OFFSET:
		if (m_bUseSelection)
		{
			MessageBeep( 0);
			ASSERT( FALSE);		// not supported (should be grayed out)
			return;
		}
		strName = "Offset";
		break;

	case FILTER_FLIP_HORIZONTAL:
		strName = "Horizontal Flip";
		break;

	case FILTER_FLIP_VERTICAL:
		strName = "Vertical Flip";
		break;

	case FILTER_SKEW:
		ASSERT( FALSE);		// not supported yet
		strName = "Skew";
		break;

	default:
		ASSERT( FALSE);
	}

	strName += " Filter";

	PrepareFilter( pLayer, iFilterType, strName);

	if (! pLayer)
		return;

	int i, j, ix, iy;
	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	if (iFilterType == FILTER_OFFSET)
	{
		// need to invert offsets so that the directions work as expected
		iAmount  = -iAmount;
		iAmount2 = -iAmount2;

		while (iAmount >= iWidth)
			iAmount -= iWidth;
		while (iAmount < 0)
			iAmount += iWidth;

		while (iAmount2 >= iHeight)
			iAmount2 -= iHeight;
		while (iAmount2 < 0)
			iAmount2 += iHeight;
	}

	COLOR_IRGB irgb = 0;

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j;

				switch (iFilterType)
				{
				case FILTER_DIFFUSE:
					ix = i + (int )(Random() * (iAmount*2+1)) - iAmount;
					if (ix >= iWidth)
						ix -= iWidth;
					else if (ix < 0)
						ix += iWidth;

					iy = j + (int )(Random() * (iAmount*2+1)) - iAmount;
					if (iy >= iHeight)
						iy -= iHeight;
					else if (iy < 0)
						iy += iHeight;
					break;

				case FILTER_OFFSET:
					ix += iAmount;
					if (ix >= iWidth)
						ix -= iWidth;

					iy += iAmount2;
					if (iy >= iHeight)
						iy -= iHeight;
					break;

				case FILTER_FLIP_HORIZONTAL:
					ix = m_rSelection.left + (m_rSelection.right-1) - i;
					break;

				case FILTER_FLIP_VERTICAL:
					iy = m_rSelection.top + (m_rSelection.bottom-1) - j;
					break;

				case MIRROR_LEFT_TO_RIGHT:
					if (i >= m_rSelection.left + m_rSelection.Width()/2)
						ix = m_rSelection.left + (m_rSelection.right-1) - i;
					break;

				case MIRROR_TOP_TO_BOTTOM:
					if (j >= m_rSelection.top + m_rSelection.Height()/2)
						iy = m_rSelection.top + (m_rSelection.bottom-1) - j;
					break;

				case MIRROR_UPPER_LEFT_TO_REST:
					if (i >= m_rSelection.left + m_rSelection.Width()/2)
						ix = m_rSelection.left + (m_rSelection.right-1) - i;

					if (j >= m_rSelection.top + m_rSelection.Height()/2)
						iy = m_rSelection.top + (m_rSelection.bottom-1) - j;
					break;

				default:
					break;
				}

				ASSERT( (iy >= 0) && (ix >= 0) && (iy < iHeight) && (ix < iWidth));
				irgb = m_pIRGB_SrcBuffer[ (iy * iWidth) + ix];

				m_pIRGB_DestBuffer[ (j * iWidth) + i] = irgb;
			}
		}
	}
	m_bNeedsFindNearestColor = FALSE;
	EndFilter( pLayer);
}

#define NEIGHBOR_HORZ  1
#define NEIGHBOR_VERT  2

// now 24 bit
void CFilter::SpecialSharpenForReduce( CLayer* pLayer, int iWeight, DWORD dwNeighborFlags)
{
	if (! pLayer)
		return;

//	int iWeight = -1;	// standard amount
//
//	switch (iSharpenAmount)
//	{
//	case FILTER_VERY_LIGHT:
//		iWeight = -1;
//		break;
//	case FILTER_LIGHT:
//		iWeight = -2;
//		break;
//	case FILTER_MEDIUM:
//		iWeight = -4;
//		break;
//	case FILTER_HEAVY:
//		iWeight = -8;
//		break;
//	case FILTER_VERY_HEAVY:
//		iWeight = -16;
//		break;
//	default:
//		ASSERT (FALSE);
//		break;
//	}

	// neal - iMult MUST == iWeight * 4 for sharpen to work
	// because there are four neighbors (up, down, left, right)
	// or *2 when there are only two neighbors

	// center pixel multiplier
	int iMult = -(iWeight * ((dwNeighborFlags == 3) ? 4 : 2));

	int i, j, ix, iy;
	int iRed, iGreen, iBlue;

	int iWidth  = m_rSrc.Width();
	int iHeight = m_rSrc.Height();

	ASSERT( m_pIRGB_DestBuffer == NULL);

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	for (j = m_rSelection.top; j < m_rSelection.bottom; j++)
	{
		for (i = m_rSelection.left; i < m_rSelection.right; i++)
		{
			if (WithinSelection( i, j))
			{
				ix = i;
				iy = j-1;
				int iAbove = (iy >= 0) ? iy : (iHeight-1);
				iy = j+1;
				int iBelow = (iy < iHeight) ? iy : 0;

				iy = j;
				ix = i-1;
				int iLeft  = (ix >= 0) ? ix : (iWidth-1);
				ix = i+1;
				int iRight = (ix < iWidth) ? ix : 0;

				//////////////////////////////////////////////////
				// use weighted average for best looking blend //
				////////////////////////////////////////////////

				// center pixel
				ix = i;
				iy = j;
				COLORREF rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				iRed   = GetRValue( rgbColor) * iMult;
				iGreen = GetGValue( rgbColor) * iMult;
				iBlue  = GetBValue( rgbColor) * iMult;

				// horizontal neighbors

				if (dwNeighborFlags & NEIGHBOR_HORZ)
				{
					ix = iLeft;
					iy = j;
					rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
					AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

					ix = iRight;
					iy = j;
					rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
					AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);
				}

				// vertical neighbors

				if (dwNeighborFlags & NEIGHBOR_VERT)
				{
					ix = i;
					iy = iAbove;
					rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
					AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);

					ix = i;
					iy = iBelow;
					rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
					AddColor( rgbColor, iWeight, &iRed, &iGreen, &iBlue);
				}

				// scale down difference, so we can have "light" sharpening
				iRed   /= iMult*2;
				iGreen /= iMult*2;
				iBlue  /= iMult*2;

				// add center pixel to edge difference
				ix = i;
				iy = j;
				rgbColor = m_pIRGB_SrcBuffer[(iy * iWidth) + ix];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				ClampRGB( &iRed, &iGreen, &iBlue);

				m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
			}
		}
	}
};

// resize helper
void CFilter::Reduce( CLayer* pLayer, int iDivX, int iDivY, BOOL bFirstCall)
{
	int iOriginalDivX = iDivX;
	int iOriginalDivY = iDivY;

	if ((iDivX > 2) || (iDivY > 2))
	{
		if (iDivX > 1)
			iDivX /= 2;
		if (iDivY > 1)
			iDivY /= 2;

		Reduce( pLayer, iDivX, iDivY, FALSE);

		// copy dest to source and repeat

		ASSERT( m_pIRGB_DestBuffer != NULL);
		free( m_pIRGB_SrcBuffer);

		m_pIRGB_SrcBuffer  = m_pIRGB_DestBuffer;
		m_pIRGB_DestBuffer = NULL;
	}

	int iDivCount = iDivX * iDivY;

	if (bFirstCall)
	{
		// do a light sharpen to improve results

		//DoSharpen( pDoc, (iDivCount == 4) ? FILTER_LIGHT : FILTER_VERY_LIGHT);
		SpecialSharpenForReduce( pLayer, 
				((iOriginalDivX == 4) || (iOriginalDivY == 4)) ? -4 : -2,
				((iDivX != 1) ? NEIGHBOR_HORZ : 0) | 
				((iDivY != 1) ? NEIGHBOR_VERT : 0));

		ASSERT( m_pIRGB_DestBuffer != NULL);

		if (m_pIRGB_DestBuffer != NULL)
		{
			free( m_pIRGB_SrcBuffer);
			m_pIRGB_SrcBuffer  = m_pIRGB_DestBuffer;
			m_pIRGB_DestBuffer = NULL;
		}
	}

	int iSrcWidth  = m_rSelection.Width();
	int iSrcHeight = m_rSelection.Height();
	int iWidth     = iSrcWidth  / iDivX;
	int iHeight    = iSrcHeight / iDivY;

	m_rSrc.top    /= iDivY;
	m_rSrc.left   /= iDivX;
	m_rSrc.right  /= iDivX;
	m_rSrc.bottom /= iDivY;

	m_rDest.top    = 0;
	m_rDest.left   = 0;
	m_rDest.right  = iWidth;
	m_rDest.bottom = iHeight;

	m_rSelection.top    /= iDivY;
	m_rSelection.left   /= iDivX;
	m_rSelection.right  /= iDivX;
	m_rSelection.bottom /= iDivY;

	ASSERT( m_pIRGB_DestBuffer == NULL);
	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	int      i, j;
	int      iRed, iGreen, iBlue;
	COLORREF rgbColor = 0;

	for (j = 0; j < iHeight; j++)
	{
		for (i = 0; i < iWidth; i++)
		{
			COLORREF rgbColor = m_pIRGB_SrcBuffer[j*iDivY*iSrcWidth + i*iDivX];
			iRed   = GetRValue( rgbColor);
			iGreen = GetGValue( rgbColor);
			iBlue  = GetBValue( rgbColor);

			if (iDivX > 1)		// reduce width
			{
				rgbColor = m_pIRGB_SrcBuffer[j*iDivY*iSrcWidth + i*iDivX+1];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);
			}
			if (iDivY > 1)		// reduce height
			{
				rgbColor = m_pIRGB_SrcBuffer[(j*iDivY+1)*iSrcWidth + i*iDivX];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);
			}
			if (iDivCount > 2)	// reduce both directions
			{
				rgbColor = m_pIRGB_SrcBuffer[(j*iDivY+1)*iSrcWidth + i*iDivX+1];
				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);
			}

			iRed   /= iDivCount;
			iGreen /= iDivCount;
			iBlue  /= iDivCount;

			m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
		}
	}
}

// resize helper
void CFilter::Enlarge( CLayer* pLayer, int iMultX, int iMultY)
{
	if ((iMultX > 2) || (iMultY > 2))
	{
		if (iMultX > 1)
			iMultX /= 2;
		if (iMultY > 1)
			iMultY /= 2;

		Enlarge( pLayer, iMultX, iMultY);

		// copy dest to source and repeat

		ASSERT( m_pIRGB_DestBuffer != NULL);
		free( m_pIRGB_SrcBuffer);

		m_pIRGB_SrcBuffer  = m_pIRGB_DestBuffer;
		m_pIRGB_DestBuffer = NULL;
	}

	int iSrcWidth  = m_rSelection.Width();
	int iSrcHeight = m_rSelection.Height();
	int iWidth     = iSrcWidth  * iMultX;
	int iHeight    = iSrcHeight * iMultY;

	m_pIRGB_DestBuffer = (COLOR_IRGB *)malloc( iWidth * iHeight * sizeof( COLOR_IRGB));

	m_rDest.top    = 0;
	m_rDest.left   = 0;
	m_rDest.right  = iWidth;
	m_rDest.bottom = iHeight;

	m_rSelection.top    *= iMultY;
	m_rSelection.left   *= iMultX;
	m_rSelection.right  *= iMultX;
	m_rSelection.bottom *= iMultY;

	int      i, j;
	int      iRed, iGreen, iBlue;
	COLORREF rgbColor = 0;

	for (j = 0; j < iHeight; j++)
	{
		for (i = 0; i < iWidth; i++)
		{
			COLORREF rgbColor = m_pIRGB_SrcBuffer[
					j/iMultY*iSrcWidth + i/iMultX];

			iRed   = GetRValue( rgbColor);
			iGreen = GetGValue( rgbColor);
			iBlue  = GetBValue( rgbColor);

			if (((i & 1) == 0) && ((j & 1) == 0))
			{
			}
			else if (((i & 1) == 1) && ((j & 1) == 0))
			{
				rgbColor = m_pIRGB_SrcBuffer[
					j/iMultY*iSrcWidth + (((i/iMultX)+(iMultX/2)) % iSrcWidth)];

				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				iRed   /= 2;
				iGreen /= 2;
				iBlue  /= 2;
			}
			else if (((i & 1) == 0) && ((j & 1) == 1))
			{
				rgbColor = m_pIRGB_SrcBuffer[
					(((j/iMultY)+(iMultY/2)) % iSrcHeight)*iSrcWidth + i/iMultX];

				AddColor( rgbColor, 1, &iRed, &iGreen, &iBlue);

				iRed   /= 2;
				iGreen /= 2;
				iBlue  /= 2;
			}
			else   // if ((i & 1) == 1) && ((j & 1)) == 1)
			{
				iRed   *= 40;
				iGreen *= 40;
				iBlue  *= 40;

				rgbColor = m_pIRGB_SrcBuffer[
					j/iMultY*iSrcWidth + (((i/iMultX) +
					(iMultX/2)) % iSrcWidth)];

				AddColor( rgbColor, 20, &iRed, &iGreen, &iBlue);

				rgbColor = m_pIRGB_SrcBuffer[
					(((j/iMultY)+(iMultY/2)) % iSrcHeight)*iSrcWidth + 
					i/iMultX];

				AddColor( rgbColor, 20, &iRed, &iGreen, &iBlue);

				rgbColor = m_pIRGB_SrcBuffer[
					(((j/iMultY)+(iMultY/2)) % iSrcHeight)*iSrcWidth +
					(((i/iMultX)+(iMultX/2)) % iSrcWidth)];

				AddColor( rgbColor, 15, &iRed, &iGreen, &iBlue);

				iRed   /= 95;
				iGreen /= 95;
				iBlue  /= 95;
			}

			m_pIRGB_DestBuffer[ (j * iWidth) + i] = IRGB( 0, iRed, iGreen, iBlue);
		}
	}
}

void CFilter::DoReduceColors( CLayer* pLayer, int iReduceColorsMaxNum, int iReserveColor[4])
{
	if ((! pLayer) || (iReduceColorsMaxNum == 256))
		return;

	if ((iReduceColorsMaxNum < 1) || (iReduceColorsMaxNum > 256))
	{
		ASSERT( FALSE);
		//AfxMessageBox( "Error: invalid number of destination colors!");
		return;
	}

	PrepareFilter( pLayer, FILTER_REDUCE_COLORS, "Reduce Colors Filter", UNDO_DOC_AND_PAL);

	CWallyPalette* pOldPal = pLayer->GetDoc()->GetPalette();
	BYTE           PackedPal[3*256];

	pLayer->GetDoc()->GetPalette( PackedPal, 256);

	CColorOptimizer ColorOpt;
	int iNumActualColors = ColorOpt.Optimize( m_pIRGB_SrcBuffer, 
			pLayer->GetWidth(), pLayer->GetHeight(), 
			PackedPal, iReduceColorsMaxNum, TRUE);

	if (iNumActualColors <= 0)
	{
		ASSERT( FALSE);
		return;
	}

	CWallyPalette NewPal;
	NewPal.SetPalette( PackedPal, 256);

	int i, j;
	int r, g, b;
	int iWidth  = pLayer->GetWidth();
	int iHeight = pLayer->GetHeight();

	// neal - fix sorting order problem

	for (i = 0; i < 4; i++)
		if (iReserveColor[i] == -1)
			(iReserveColor[i] = 9999);

	///////////////////////////////////
	// build look-up table speed-up //
	/////////////////////////////////

	BYTE       byLookUp[256];
	COLOR_IRGB irgb;

	for (i = 0; i < 256; i++)
	{
		r = pOldPal->GetR( i);
		g = pOldPal->GetG( i);
		b = pOldPal->GetB( i);

		j = NewPal.FindNearestColor( r, g, b, FALSE);

		// don't let black match out-of-range color

		if (j >= iNumActualColors)
			j = 0;

		byLookUp[i] = j;
	}

	///////////////////////////////////
	// calc average color (for fog) //
	/////////////////////////////////

	int iAvgR = 0;
	int iAvgG = 0;
	int iAvgB = 0;

	for (i = 0; i < iNumActualColors; i++)
	{
		iAvgR += PackedPal[i*3 + 0];
		iAvgG += PackedPal[i*3 + 1];
		iAvgB += PackedPal[i*3 + 2];
	}
	iAvgR /= iNumActualColors;
	iAvgG /= iNumActualColors;
	iAvgB /= iNumActualColors;

	/////////////////////////
	// sort iReserveColor //
	///////////////////////

	BOOL bSwapped = TRUE;

	while (bSwapped)
	{
		bSwapped = FALSE;

		for (i = 1; i < 4; i++)
		{
			if (iReserveColor[i-1] > iReserveColor[i])
			{
				bSwapped           = TRUE;
				j                  = iReserveColor[i-1];
				iReserveColor[i-1] = iReserveColor[i];
				iReserveColor[i]   = j;
			}
		}
	}

	///////////////////////////////////////////
	// shift to reserve color palette slots //
	/////////////////////////////////////////

	int iNumReserved = 0;

	for (j = 0; j < 4; j++)
	{
		if ((iReserveColor[j] >= 0) && (iReserveColor[j] < 255))
		{
			iNumReserved++;

			for (i = 254; i >= iReserveColor[j]; i--)
			{
				PackedPal[((i+1) * 3) + 0] = PackedPal[(i * 3) + 0];
				PackedPal[((i+1) * 3) + 1] = PackedPal[(i * 3) + 1];
				PackedPal[((i+1) * 3) + 2] = PackedPal[(i * 3) + 2];
			}
			for (i = 0; i < 256; i++)
			{
				if (byLookUp[i] >= iReserveColor[j])
					byLookUp[i]++;
			}
		}
	}

#ifdef _DEBUG
	for (j = 0; j < 4; j++)
	{
		if ((iReserveColor[j] >= 0) && (iReserveColor[j] <= 255))
		{
			for (i = 0; i < 256; i++)
			{
				ASSERT( byLookUp[i] != iReserveColor[j]);
			}
		}
	}
#endif

	if (iNumReserved == 1)
	{
		if (iReserveColor[0] == 255)			// Half-Life
		{
			PackedPal[(255 * 3) + 0] = 0;		// transparency color (BLUE)
			PackedPal[(255 * 3) + 1] = 0;
			PackedPal[(255 * 3) + 2] = 255;
		}
		else if (iReserveColor[0] == 255)		// Unreal
		{
			PackedPal[(0 * 3) + 0] = 0;			// transparency color (BLACK)
			PackedPal[(0 * 3) + 1] = 0;
			PackedPal[(0 * 3) + 2] = 0;
		}
	}
	else if (iNumReserved == 2)
	{
		if ((iReserveColor[0] == 3) && (iReserveColor[1] == 4))   // Half-Life liquid
		{
			PackedPal[(3 * 3) + 0] = (BYTE )iAvgR;		// fog color
			PackedPal[(3 * 3) + 1] = (BYTE )iAvgG;
			PackedPal[(3 * 3) + 2] = (BYTE )iAvgB;

			PackedPal[(4 * 3) + 0] = 128;				// red is fog density
			PackedPal[(4 * 3) + 1] = 0;
			PackedPal[(4 * 3) + 2] = 0;
		}
	}

	NewPal.SetPalette( PackedPal, 256);

	///////////////////
	// remap colors //
	/////////////////

	for (j = 0; j < iHeight; j++)
	{
		for (i = 0; i < iWidth; i++)
		{
			irgb = m_pIRGB_SrcBuffer[j*iWidth+i];

			int iOldIndex = GetIValue( irgb);
			int iNewIndex = byLookUp[iOldIndex];

			r = NewPal.GetR( iNewIndex);
			g = NewPal.GetG( iNewIndex);
			b = NewPal.GetB( iNewIndex);

			m_pIRGB_SrcBuffer[j*iWidth+i] = IRGB( iNewIndex, r, g, b);
		}
	}

	pLayer->GetDoc()->SetPalette( PackedPal, 256, TRUE);

	m_bNeedsFindNearestColor = FALSE;
	EndFilter( pLayer);

	g_CurrentPalette = NewPal;

	((CMainFrame* )AfxGetMainWnd())->m_wndPaletteToolBar.Update( pLayer->GetDoc(), FALSE);

	// neal - fix sorting order problem

	for (i = 0; i < 4; i++)
		if (iReserveColor[i] == 9999)
			(iReserveColor[i] = -1);
}


void CFilter::DoResize( CLayer* pLayer, int iResizeType, int iResizeAmount)
{
	PrepareFilter( pLayer, FILTER_RESIZE, "Resize");

	if (! pLayer)
		return;

	int iSrcWidth  = m_rSrc.Width();
	int iSrcHeight = m_rSrc.Height();

	int iMultX = 1;
	int iDivX  = 1;
	int iMultY = 1;
	int iDivY  = 1;

	BOOL bReduce = TRUE;

	switch (iResizeType)
	{
	case 1:		// quad size
		iMultX = 4;
		iMultY = 4;
		break;
	case 2:		// double size
		iMultX = 2;
		iMultY = 2;
		break;
	case 3:		// half size
		iDivX = 2;
		iDivY = 2;
		break;
	case 4:		// quarter size
		iDivX = 4;
		iDivY = 4;
		break;

	case 5:		// quad width
		iMultX = 4;
		break;
	case 6:		// double width
		iMultX = 2;
		break;
	case 7:		// half width
		iDivX = 2;
		break;
	case 8:		// quarter width
		iDivX = 4;
		break;

	case 9:		// quad height
		iMultY = 4;
		break;
	case 10:		// double height
		iMultY = 2;
		break;
	case 11:		// half height
		iDivY = 2;
		break;
	case 12:		// quarter height
		iDivY = 4;
		break;

	default:
		ASSERT( FALSE);
		break;
	}

	ASSERT( (iMultX == 1) || (iDivX == 1));
	ASSERT( (iMultY == 1) || (iDivY == 1));

	if ( (iMultX == 1) && (iDivX == 1) &&
			(iMultY == 1) && (iDivY == 1))
	{
		ASSERT( FALSE);		// no resize (100%)
		return;
	}

	bReduce = ((iDivX > 1) || (iDivY > 1));

	if (bReduce)
	{
		Reduce( pLayer, iDivX, iDivY, TRUE);
	}
	else
	{
		Enlarge( pLayer, iMultX, iMultY);
	}

	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
}

void CFilter::DoArbitraryResize( CLayer* pLayer, int iNewWidth, int iNewHeight)
{
	PrepareFilter( pLayer, FILTER_RESIZE, "Resize");

	if (! pLayer)
		return;

	int iSrcWidth  = m_rSrc.Width();
	int iSrcHeight = m_rSrc.Height();
//	int iSrcWidth  = m_rSelection.Width();
//	int iSrcHeight = m_rSelection.Height();

	C2PassScale <CBilinearFilter> ScaleEngine;

	m_pIRGB_DestBuffer = ScaleEngine.AllocAndScale( m_pIRGB_SrcBuffer,
			iSrcWidth, iSrcHeight, iNewWidth, iNewHeight);

	if (NULL == m_pIRGB_SrcBuffer)
	{
		// Handle errors here

		m_pIRGB_DestBuffer = m_pIRGB_SrcBuffer;
	}
	else
	{
		m_rDest.top    = 0;
		m_rDest.left   = 0;
		m_rDest.right  = iNewWidth;
		m_rDest.bottom = iNewHeight;

//		m_rSelection.top    *= iMultY;
//		m_rSelection.left   *= iMultX;
//		m_rSelection.right  *= iMultX;
//		m_rSelection.bottom *= iMultY;
		m_rSelection.top    = m_rSelection.top    * iNewHeight / iSrcHeight;
		m_rSelection.left   = m_rSelection.left   * iNewWidth  / iSrcWidth;
		m_rSelection.right  = m_rSelection.right  * iNewWidth  / iSrcWidth;
		m_rSelection.bottom = m_rSelection.bottom * iNewHeight / iSrcHeight;
	}
	m_bNeedsFindNearestColor = TRUE;
	EndFilter( pLayer);
}

inline void SetFieldColorValue( CWallyPalette* pPal, int* iARGB_Field, int iIndex, int iColor)
{
	if (iColor != 255)		// not transparent
	{
		iARGB_Field[iIndex*4]   = 255;					// Alpha
		iARGB_Field[iIndex*4+1] = pPal->GetR( iColor);	// Red channel
		iARGB_Field[iIndex*4+2] = pPal->GetG( iColor);	// Green channel
		iARGB_Field[iIndex*4+3] = pPal->GetB( iColor);	// Blue channel
	}
	else	// 255 is transparency color index
	{
		iARGB_Field[iIndex*4]   = 0;		// Alpha
		iARGB_Field[iIndex*4+1] = 0;		// Red channel
		iARGB_Field[iIndex*4+2] = 0;		// Green channel
		iARGB_Field[iIndex*4+3] = 0;		// Blue channel
	}
}

void CFilter::GenerateMarbleColorField( CLayer* pLayer, int iARGB_Field[4*256], 
			int iBackground, int iThickStripes, int iThinStripes,
			int iNumThickStripes, int iNumThinStripes, int iColorVariance,
			int iRandSeed, BOOL bExtraSmooth)
{
	if (! pLayer)
		return;

	int i, j, n, m;
	int iRandR, iRandG, iRandB;
	int iLeft, iRight;

	int iARGB_Background[4];
	int iARGB_ThickStripe[4];
	int iARGB_ThinStripe[4];
	int iARGB_VarianceStripe[4];

	int iARGB_SmoothField[4*256];

	CWallyPalette* pPal = pLayer->GetDoc()->GetPalette();

	InitRandom( iRandSeed);
	InitNoise( iRandSeed);

	////////////////////
	// check limits! //
	//////////////////

	iNumThickStripes = min( iNumThickStripes, 10);
	iNumThickStripes = max( iNumThickStripes, 0);

	iNumThinStripes = min( iNumThinStripes, 20);
	iNumThinStripes = max( iNumThinStripes, 0);

	// special case? (both zero means random selection)

	if ((iNumThickStripes == 0) && (iNumThinStripes == 0))
	{
		iNumThickStripes = (int )(Random() * 5);
		iNumThinStripes  = (int )(Random() * 10);
	}

	///////////////////////////////////////
	// init Alpha-Red-Green-Blue colors //
	/////////////////////////////////////

	SetFieldColorValue( pPal, iARGB_Background,  0, iBackground);
	SetFieldColorValue( pPal, iARGB_ThickStripe, 0, iThickStripes);
	SetFieldColorValue( pPal, iARGB_ThinStripe,  0, iThinStripes);

	///////////////////////////
	// fill with background //
	/////////////////////////

	for (i = 0; i < 256; i++)
	{
		CopyMemory( &iARGB_Field[i*4], iARGB_Background, sizeof( iARGB_Background));
	}

	////////////////////////
	// add thick stripes //
	//////////////////////

	for (j = 0; j < iNumThickStripes; j++)
	{
		n = (int )(Random() * 256);
		m = n + 12 + (int )(Random()*8);

		// random number in range: -variance .. +variance
		iRandR = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;
		iRandG = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;
		iRandB = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;

		// skip it if it's transparent
		if (iARGB_ThickStripe[0] == 255)
		{
			iARGB_VarianceStripe[0] = iARGB_ThickStripe[0];
			iARGB_VarianceStripe[1] = iARGB_ThickStripe[1] + iRandR;
			iARGB_VarianceStripe[2] = iARGB_ThickStripe[2] + iRandG;
			iARGB_VarianceStripe[3] = iARGB_ThickStripe[3] + iRandB;

			ClampRGB( &iARGB_VarianceStripe[1], &iARGB_VarianceStripe[2], &iARGB_VarianceStripe[3]);
		}
		else
		{
			CopyMemory( iARGB_VarianceStripe, iARGB_ThickStripe, sizeof( iARGB_ThickStripe));
		}

		for (i = n+1; i < m; i++)
		{
			// add a thick stripe

			CopyMemory( &iARGB_Field[(i&255)*4], iARGB_VarianceStripe, sizeof( iARGB_VarianceStripe));
		}

		// blended left edges

		n &= 255;

		iARGB_Field[n * 4]   = (iARGB_Background[0] + iARGB_VarianceStripe[0]) / 2;
		iARGB_Field[n * 4+1] = (iARGB_Background[1] + iARGB_VarianceStripe[1]) / 2;
		iARGB_Field[n * 4+2] = (iARGB_Background[2] + iARGB_VarianceStripe[2]) / 2;
		iARGB_Field[n * 4+3] = (iARGB_Background[3] + iARGB_VarianceStripe[3]) / 2;

		// blended right edge

		m &= 255;

		iARGB_Field[m * 4]   = (iARGB_Background[0] + iARGB_VarianceStripe[0]) / 2;
		iARGB_Field[m * 4+1] = (iARGB_Background[1] + iARGB_VarianceStripe[1]) / 2;
		iARGB_Field[m * 4+2] = (iARGB_Background[2] + iARGB_VarianceStripe[2]) / 2;
		iARGB_Field[m * 4+3] = (iARGB_Background[3] + iARGB_VarianceStripe[3]) / 2;
	}

	// smooth seams (first long pass for thick stripes)

	n = (bExtraSmooth) ? 8 : 2;
	for (j = 0; j < n; j++)
	{
		for (i = 0; i < 256; i++)
		{
			iLeft  = (i-1) & 255;
			iRight = (i+1) & 255;

			// alpha
			iARGB_SmoothField[i*4] = (iARGB_Field[iLeft*4] + 
					(2*iARGB_Field[i*4]) + iARGB_Field[iRight*4]) / 4;

			// red
			iARGB_SmoothField[i*4+1] = (iARGB_Field[iLeft*4+1] + 
					(2*iARGB_Field[i*4+1]) + iARGB_Field[iRight*4+1]) / 4;

			// green
			iARGB_SmoothField[i*4+2] = (iARGB_Field[iLeft*4+2] + 
					(2*iARGB_Field[i*4+2]) + iARGB_Field[iRight*4+2]) / 4;

			// blue
			iARGB_SmoothField[i*4+3] = (iARGB_Field[iLeft*4+3] + 
					(2*iARGB_Field[i*4+3]) + iARGB_Field[iRight*4+3]) / 4;
		}

		// copy back

		CopyMemory( iARGB_Field, iARGB_SmoothField, sizeof( iARGB_Field));
	}

	// we want thin stripes to be repeatable
	// (regardless of number of thick stripes)
	InitRandom( iRandSeed);

	///////////////////////
	// add thin stripes //
	/////////////////////

	for (j = 0; j < iNumThinStripes; j++)
	{
		n = (int )(Random() * 256);
		m = n + 4 + (int )(Random()*8);

		iRandR = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;
		iRandG = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;
		iRandB = (int )(Random() * (iColorVariance*2+1)) - iColorVariance;

		// skip it if it's transparent
		if (iARGB_ThinStripe[0] == 255)
		{
			iARGB_VarianceStripe[0] = iARGB_ThinStripe[0];
			iARGB_VarianceStripe[1] = iARGB_ThinStripe[1] + iRandR;
			iARGB_VarianceStripe[2] = iARGB_ThinStripe[2] + iRandG;
			iARGB_VarianceStripe[3] = iARGB_ThinStripe[3] + iRandB;

			ClampRGB( &iARGB_VarianceStripe[1], &iARGB_VarianceStripe[2], &iARGB_VarianceStripe[3]);
		}
		else
		{
			CopyMemory( iARGB_VarianceStripe, iARGB_ThinStripe, sizeof( iARGB_ThinStripe));
		}

		for (i = n+1; i < m; i++)
		{
			// add a thin stripe

			CopyMemory( &iARGB_Field[(i&255)*4], iARGB_VarianceStripe, sizeof( iARGB_VarianceStripe));
		}

		// blended left edges

		iARGB_Field[(n & 255) * 4]   = (iARGB_Background[0] + iARGB_VarianceStripe[0]) / 2;
		iARGB_Field[(n & 255) * 4+1] = (iARGB_Background[1] + iARGB_VarianceStripe[1]) / 2;
		iARGB_Field[(n & 255) * 4+2] = (iARGB_Background[2] + iARGB_VarianceStripe[2]) / 2;
		iARGB_Field[(n & 255) * 4+3] = (iARGB_Background[3] + iARGB_VarianceStripe[3]) / 2;

		// blended right edge

		iARGB_Field[(m & 255) * 4]   = (iARGB_Background[0] + iARGB_VarianceStripe[0]) / 2;
		iARGB_Field[(m & 255) * 4+1] = (iARGB_Background[1] + iARGB_VarianceStripe[1]) / 2;
		iARGB_Field[(m & 255) * 4+2] = (iARGB_Background[2] + iARGB_VarianceStripe[2]) / 2;
		iARGB_Field[(m & 255) * 4+3] = (iARGB_Background[3] + iARGB_VarianceStripe[3]) / 2;
	}

  // smooth seams again (short pass for thin stripes)

	n = (bExtraSmooth) ? 2 : 1;
	for (j = 0; j < n; j++)
	{
		for (i = 0; i < 256; i++)
		{
			iLeft  = (i-1) & 255;
			iRight = (i+1) & 255;

			// alpha
			iARGB_SmoothField[i*4] = (iARGB_Field[iLeft*4] + 
					(2*iARGB_Field[i*4]) + iARGB_Field[iRight*4]) / 4;

			// red
			iARGB_SmoothField[i*4+1] = (iARGB_Field[iLeft*4+1] + 
					(2*iARGB_Field[i*4+1]) + iARGB_Field[iRight*4+1]) / 4;

			// green
			iARGB_SmoothField[i*4+2] = (iARGB_Field[iLeft*4+2] + 
					(2*iARGB_Field[i*4+2]) + iARGB_Field[iRight*4+2]) / 4;

			// blue
			iARGB_SmoothField[i*4+3] = (iARGB_Field[iLeft*4+3] + 
					(2*iARGB_Field[i*4+3]) + iARGB_Field[iRight*4+3]) / 4;
		}

		// copy back

		CopyMemory( iARGB_Field, iARGB_SmoothField, sizeof( iARGB_Field));
	}
}

// now 24 bit
void CFilter::GenerateMarbleTexture( CLayer* pLayer, int iARGB_Field[4*256], 
			LPRECT lpRect, BYTE* pbyBlockStencilData,
			Vec vScale, Vec vOffset, double dfAmplitude, 
			int iLevels, int iRandSeed, BOOL bSeamless, int iTileGroutType, 
			BOOL bMirrorX, BOOL bMirrorY, BOOL bFastPreview)
{
	if (! pLayer)
		return;

	int w = m_rSrc.Width();
	int h = m_rSrc.Height();

	CWallyPalette* pPal = pLayer->GetDoc()->GetPalette();
	COLOR_IRGB     irgb;
	CRect          rBounds;

	if (lpRect)
		rBounds = lpRect;
	else
		rBounds.SetRect( 0, 0, w, h);

	int i, j, m, n, x, y;
	int iAlpha, iRed, iGreen, iBlue, iGray, iColor;
	int iTileW = rBounds.Width();
	int iTileH = rBounds.Height();

	////////////////////
	// check limits! //
	//////////////////

	iLevels = min( iLevels, 10);
	iLevels = max( iLevels, 1);

	if (vScale[0] == 0.0)
		vScale[0] = 1.0;
	if (vScale[1] == 0.0)
		vScale[1] = 1.0;
	if (vScale[2] == 0.0)
		vScale[2] = 1.0;

	#define ANTIALIAS 3
	int iAntialias = (bFastPreview) ? 1 : ANTIALIAS;

	CWallyDoc* pDoc = pLayer->GetDoc();

	for (y = 0; y < iTileH; y++)
	{
		//j = (bMirrorY) ? ((h-1) - y) : y;
		j = (bMirrorY) ? ((iTileH-1) - y) : y;

		for (x = 0; x < iTileW; x++)
		{
			//i = (bMirrorX) ? ((w-1) - x) : x;
			i = (bMirrorX) ? ((iTileW-1) - x) : x;

//			int iIndex = (((y+rBounds.top) % h) * w) + ((x+rBounds.left) % w);
			CPoint pt;
			pt.x = (x+rBounds.left) % w;
			pt.y = (y+rBounds.top) % h;
			int iIndex = (pt.y * w) + pt.x;

			ASSERT( (iIndex >= 0) && (iIndex < w*h));

			if (! m_rSelection.PtInRect( pt))
				continue;

			Vec vNoise, vP, vT;
			Vec vSeamless, vSeamlessX;

/* neal - save - good start for liquid tex

			// vP is x,y,z coord
			vP[0] = (double )i / w * 8;
			vP[1] = (double )j / h * 8;
			vP[2] = 0.5;

			//Flt f = 0.5 + noise1( vP);
			Flt f = 0.5 + turb1( vP, iLevels);

			iRed   = (int )(f * 255.0);
			iGreen = (int )(f * 255.0);
			iBlue  = (int )(f * 255.0);
*/
			iAlpha = iRed = iGreen = iBlue = iGray = 0;

			// used for shadow effects
			int iStencil;

			if (pbyBlockStencilData)
				iStencil = ((signed char *)pbyBlockStencilData)[y*w + x];
			else
				iStencil = 0;

			if (iStencil == 127)		// transparent?
				continue;

			//////////////////
			// draw morter //
			////////////////

			if (iStencil == 126)		// grout color?
			{
				iGray = 0;

				if ((iTileGroutType == TILE_GROUT_THIN_ROUGH) ||
							(iTileGroutType == TILE_GROUT_THICK_ROUGH))
				{
					iGray = 32 - (int )(Random() * 65.0);
				}

				iRed   = pPal->GetR( m_iMarbleTileColorGrout) + iGray;
				iGreen = pPal->GetG( m_iMarbleTileColorGrout) + iGray;
				iBlue  = pPal->GetB( m_iMarbleTileColorGrout) + iGray;

				ClampRGB( &iRed, &iGreen, &iBlue);

				//iColor = pDoc->FindNearestColor( iRed, iGreen, iBlue, FALSE);
				iColor = pPal->FindNearestColor( iRed, iGreen, iBlue, FALSE);

				// neal - this is done so we can see our progress on-screen
//				pDoc->SetMipPixel( NULL, iIndex, iColor, 0);
//
//				m_pIRGB_SrcBuffer[iIndex] = IRGB( iColor, iRed, iGreen, iBlue);

				irgb = IRGB( iColor, iRed, iGreen, iBlue);
///				pDoc->SetPixel( NULL, x, y, irgb);
				pDoc->SetPixel( NULL, pt.x, pt.y, irgb);

				m_pIRGB_SrcBuffer[iIndex] = irgb;
				continue;
			}

 			for (m = 0; m < iAntialias; m++)
			{
				for (n = 0; n < iAntialias; n++)
				{
					// vP is x,y,z coord
					vP[0] = (i + vOffset[0]) / (w * vScale[0]);				// X
					vP[1] = (j - vOffset[1]) / (h * vScale[1]);				// Y
					vP[2] = (0 + vOffset[2]) / (max( w, h) * vScale[2]);	// Z

					vP[0] += n / (iAntialias * w * vScale[0]);		// X
					vP[1] += m / (iAntialias * h * vScale[1]);		// Y

					CopyMemory( vSeamless, vP, sizeof( Vec));

					Turb3( vP, vT, iLevels);
					Noise3( vT, vNoise);

					double f = vNoise[0] + vNoise[1] + vNoise[2];

					if (bSeamless)
					{
/*						int    iWdiv2    = w / 2;
						int    iHdiv2    = h / 2;
						double dfAmountX = (double )(i - iWdiv2) / w;
						double dfAmountY = (double )(j - iHdiv2) / h;

						dfAmountX *= dfAmountX;		// squared for faster ramp
						dfAmountY *= dfAmountY;

						// if seamless, blend in an area from somewhere else

						// need to mirror based on quadrant:
						//
						// Destination:			Source:
						//  _________            ____
						// |A   |   A|				|A   |
						// |  1 | 2  |				|    |
						// |___\|/___|				|___\|
						// |   /|\   |
						// |  3 | 4  |
						// |A___|___A|
						//
						int ix = 111;
						int iy = 9;

						if ((i < iWdiv2) && (j < iHdiv2))	// first quadrant
						{
							ix += i;
							iy += j;
						}
						else if (j < iHdiv2)					// second quadrant
						{
							ix += -(iWdiv2 - i);
							iy += j;
						}
						else if (i < iWdiv2)					// third quadrant
						{
							ix += i;
							iy += -(iHdiv2 - j);
						}
						else										// fourth quadrant
						{
							ix += -(iWdiv2 - i);
							iy += -(iHdiv2 - j);
						}

						vSeamless[0] = (ix + vOffset[0]) / (w * vScale[0]);	// X
						vSeamless[0] += m / (iAntialias * w * vScale[0]);

						vSeamless[1] = (iy + vOffset[1]) / (h * vScale[1]);	// Y
						vSeamless[1] += n / (iAntialias * h * vScale[1]);

						Turb3( vSeamless, vT, iLevels);
						Noise3( vT, vSeamless);

						dfAmountX = fabs( dfAmountX);
						dfAmountY = fabs( dfAmountY);
						double dfAmount = max( dfAmountX, dfAmountY);

						//f *= 1.0 - dfAmount;
						f = 0.0;		//************** T E S T **************
						f += (vSeamless[0] + vSeamless[1] + vSeamless[2]) * dfAmount;
*/
						/////////////////
						// horizontal //
						///////////////
						double dfAmountX = (double )i / (w-1);
						double dfAmountY = (double )j / (h-1);
						dfAmountX = dfAmountX * dfAmountX;	// TODO: fails ???
						dfAmountY = dfAmountY * dfAmountY;

						vSeamlessX[0] = (i - w + vOffset[0]) / (w * vScale[0]);				// X
						vSeamlessX[1] = (j - vOffset[1]) / (h * vScale[1]);				// Y
						vSeamlessX[2] = vSeamless[2];

						vSeamlessX[0] += n / (iAntialias * w * vScale[0]);		// X
						vSeamlessX[1] += m / (iAntialias * h * vScale[1]);		// Y

						Turb3( vSeamlessX, vT, iLevels);
						Noise3( vT, vSeamlessX);

						f *= (1.0 - dfAmountX);
						f += (vSeamlessX[0] + vSeamlessX[1] + vSeamlessX[2]) * dfAmountX;

						///////////////
						// vertical //
						/////////////

						vSeamlessX[0] = (i + vOffset[0]) / (w * vScale[0]);				// X
						vSeamlessX[1] = (j - h - vOffset[1]) / (h * vScale[1]);				// Y
						vSeamlessX[2] = vSeamless[2];

						vSeamlessX[0] += n / (iAntialias * w * vScale[0]);		// X
						vSeamlessX[1] += m / (iAntialias * h * vScale[1]);		// Y

						Turb3( vSeamlessX, vT, iLevels);
						Noise3( vT, vSeamlessX);

						double dfAbove = vSeamlessX[0] + vSeamlessX[1] + vSeamlessX[2];

						// create seamless strip from area above texture

						vSeamlessX[0] = (i - w + vOffset[0]) / (w * vScale[0]);				// X
						vSeamlessX[1] = (j - h - vOffset[1]) / (h * vScale[1]);				// Y
						vSeamlessX[2] = vSeamless[2];

						vSeamlessX[0] += n / (iAntialias * w * vScale[0]);		// X
						vSeamlessX[1] += m / (iAntialias * h * vScale[1]);		// Y

						Turb3( vSeamlessX, vT, iLevels);
						Noise3( vT, vSeamlessX);

						dfAbove *= (1.0 - dfAmountX);		// this is correct!!!
						dfAbove += (vSeamlessX[0] + vSeamlessX[1] + vSeamlessX[2]) * dfAmountX;

						// now blend in both strips

						f *= (1.0 - dfAmountY);
						f += dfAbove * dfAmountY;

					}

					f = 0.5 + (f * dfAmplitude);

					// f must be positive for MOD operation to work

					if (f < 0.0)
						f += 10000 * 256;

					int iLookup = (int )(f * 255.0) % 256;

					iAlpha += iARGB_Field[ iLookup*4];
					iRed   += iARGB_Field[ iLookup*4 + 1];
					iGreen += iARGB_Field[ iLookup*4 + 2];
					iBlue  += iARGB_Field[ iLookup*4 + 3];
				}
			}

			/////////////////////////////////
			// status bar progress update //
			///////////////////////////////

			if ((x & 31) == 0)
			{
				DWORD dwTick = GetTickCount();

				if (dwTick - g_dwTickCount > 250)	// up to 4 times a second
				{
					// give the app time to breathe

					if (g_iWorking & 1)		// do this every-other time (twice a second)
					{
						MSG WindowMsg;
						HWND hWndMain = AfxGetMainWnd()->GetSafeHwnd();

						pDoc->UpdateAllViews( NULL, HINT_UPDATE_DIBS);
						PeekMessage( &WindowMsg, hWndMain, WM_KEYDOWN, WM_KEYUP, PM_NOREMOVE);
						UpdateWindow( hWndMain);
					}

					g_dwTickCount = dwTick;

					char szWorking[80];
					sprintf( szWorking, "%s Working...  (%d pixels completed)", szAnimi[g_iWorking&3], g_iTotalPixels);
					g_iWorking++;

					m_wndStatusBar.SetPaneText( 0, szWorking, true);
					m_wndStatusBar.UpdateWindow();
				}
			}

			/////////////////////////
			// generate the pixel //
			///////////////////////

			// full color, antialiased rendering?

			if (! bFastPreview)
			{
				iAlpha /= (iAntialias * iAntialias);
				iRed   /= (iAntialias * iAntialias);
				iGreen /= (iAntialias * iAntialias);
				iBlue  /= (iAntialias * iAntialias);
			}

			// add shadow effects
			iRed   += iStencil;
			iGreen += iStencil;
			iBlue  += iStencil;

			if (iAlpha > 245)			// completely opaque?
			{
				ClampRGB( &iRed, &iGreen, &iBlue);

				// neal - this is done so we can see our progress on-screen
				iColor = pDoc->FindNearestColor( iRed, iGreen, iBlue, FALSE);
//				pDoc->SetMipPixel( NULL, iIndex, iColor, 0);
//
//				m_pIRGB_SrcBuffer[iIndex] = IRGB( iColor, iRed, iGreen, iBlue);

				irgb = IRGB( iColor, iRed, iGreen, iBlue);
///				pDoc->SetPixel( NULL, x, y, irgb);
				pDoc->SetPixel( NULL, pt.x, pt.y, irgb);
				//pDoc->SetNearestColorPixel( NULL, x, y, RGB( iRed, iGreen, iBlue), FALSE);

				m_pIRGB_SrcBuffer[iIndex] = irgb;
			}
			else if (iAlpha > 10)	// not invisible?
			{
				// need to normalize RGB compontents if transparent

				double dfNormalize = 255.0 / iAlpha;

				iRed   = (int )(iRed   * dfNormalize);
				iGreen = (int )(iGreen * dfNormalize);
				iBlue  = (int )(iBlue  * dfNormalize);

				ClampRGB( &iRed, &iGreen, &iBlue);

				// now blend in original pixel color

//				iColor = pDoc->GetMipPixel( iIndex, 0);
//
//				iRed   = (iRed   + pPal->GetR( iColor)) / 2;
//				iGreen = (iGreen + pPal->GetG( iColor)) / 2;
//				iBlue  = (iBlue  + pPal->GetB( iColor)) / 2;
				
				irgb   = pDoc->GetPixel( x, y);
				iRed   = (iRed   + GetRValue( irgb)) / 2;
				iGreen = (iGreen + GetGValue( irgb)) / 2;
				iBlue  = (iBlue  + GetBValue( irgb)) / 2;

				// neal - this is done so we can see our progress on-screen
				iColor = pDoc->FindNearestColor( iRed, iGreen, iBlue, FALSE);
//				pDoc->SetMipPixel( NULL, iIndex, iColor, 0);
//
//				m_pIRGB_SrcBuffer[iIndex] = IRGB( iColor, iRed, iGreen, iBlue);

				irgb = IRGB( iColor, iRed, iGreen, iBlue);
///				pDoc->SetPixel( NULL, x, y, irgb);
				pDoc->SetPixel( NULL, pt.x, pt.y, irgb);
				//pDoc->SetNearestColorPixel( NULL, x, y, RGB( iRed, iGreen, iBlue), FALSE);

				m_pIRGB_SrcBuffer[iIndex] = irgb;
			}
			g_iTotalPixels++;
		}
	}
}

void CFilter::GenerateMarbleTileLine( int iRow, int iX, int iY, int iNumTiles, 
			int iIncrementX, int iIncrementY, MarbleParams* pParams)
{
/*
	for (j = 0; j < iNumTilesY; j++)
	{
		for (i = 0; i < iNumTilesX; i++)
		{
			int* pField = iARGB_Field;

			// checkerboard pattern
			if (((i ^ j) & 1) == 1)		// ^ is XOR
			{
				if (! bSolidColor)
				{
					if (bUseAccentColor && ((i&1) == 0))
						pField = iARGB_AccentField;
					else if (bUseContrastColor)
						pField = iARGB_ContrastField;
				}
				if (bMirror)
					vOffset[2] += dfThickness;
			}
			vOffset[0] += (i/2) * dfThickness;
			vOffset[1] += (j/2) * dfThickness;

			if (bMirror)
				vOffset[2] += j;
			else
				vOffset[2] += (j*2+i)*dfThickness;

			//int k = (bStagger) ? ((j & 1) * iStagger) : 0;
			int k = j * iStagger;

			rTile.SetRect( i*(iTileW + iMorterWidth) + k, j*(iTileH + iMorterWidth), 
				(i+1)*(iTileW + iMorterWidth) + k - iMorterWidth,
				(j+1)*(iTileH + iMorterWidth) - iMorterWidth);

			GenerateMarbleTexture( pDoc, pField, &rTile, pbyBlockStencilData, 
					vScale, vOffset, dfAmplitude, iLevels, iRandSeed, 
					bSeamless, bMirror&i&1, bMirror&j&1, bFastPreview);

			// REQUIRED -- restore offset vector
			CopyMemory( vOffset, vSaveOffset, sizeof( Vec));
		}
	}
*/
}

void CFilter::GenerateMarbleSubtile( int iRow, int iColumn, MarbleParams* pParams)
{
	int i, j;

	if ((pParams == NULL) || (pParams->pLayer == NULL))
	{
		ASSERT( FALSE);
		MessageBeep( MB_ICONEXCLAMATION);
		return;
	}

	CLayer* pLayer = pParams->pLayer;

	Vec vSaveOffset;
	CopyMemory( vSaveOffset, pParams->vOffset, sizeof( Vec));

	CRect rTile;
	int w = m_rSrc.Width();
	int h = m_rSrc.Height();

	int iNumTilesX, iNumTilesY;

	switch (pParams->iTileSize)
	{
	case TILE_TINY:
		iNumTilesX = 16;
		break;
	case TILE_SMALL:
		iNumTilesX = 8;
		break;
	case TILE_MEDIUM:
		iNumTilesX = 4;
		break;
	case TILE_LARGE:
		iNumTilesX = 2;
		break;
	default:
		ASSERT( FALSE);
		iNumTilesX = 4;
	}

	// the following code fixes "square tiles not always square" bug

	if (w > h)
	{
		i = w / iNumTilesX;	// size in pixels
		i = max( i, 4);		// no smaller than 4 pixels on a side

		iNumTilesY = h / i;
	}
	else if (h > w)
	{
		iNumTilesY = iNumTilesX;

		i = h / iNumTilesY;	// size in pixels
		i = max( i, 4);		// no smaller than 4 pixels on a side

		iNumTilesX = w / i;
	}
	else // they're equal
	{
		iNumTilesY = iNumTilesX;
	}

	// this is CORRECT - we're using the TILE random number sequence

	InitRandom( pParams->iTileRandSeed);

	BOOL bFancyColor        = (pParams->iTileColor == TILE_COLOR_MULTI);
	BOOL bUseContrastColor  = bFancyColor || (pParams->iTileColor == TILE_COLOR_TWO_TONE);
	BOOL bUseAccentColor    = bFancyColor || (pParams->iTileColor == TILE_COLOR_ACCENTED);
	BOOL bSolidColor        = ! (bUseContrastColor || bUseAccentColor);

	BOOL bGrid         = (Random() < 0.5);
	BOOL bVariation    = (Random() < 0.5);
	BOOL bStagger      = (pParams->iTileStyle >= TILE_STYLE_FANCY);
	BOOL bCheckerBoard = (Random() < 0.5);
	BOOL bMirror       = (Random() < 0.5);

	if ((pParams->iShape != TILE_SQUARE) || bUseAccentColor || bStagger)
		bMirror = FALSE;

	if (bCheckerBoard)
	{
		pParams->bMirrorX = bMirror;
		pParams->bMirrorY = bMirror;
	}
	else if (bSolidColor)
	{
		pParams->bMirrorX = (Random() < 0.5) ? bMirror : FALSE;
		pParams->bMirrorY = (Random() < 0.5) ? bMirror : FALSE;
	}
	else
		bMirror = FALSE;

	if (! bMirror)
	{
		pParams->bMirrorX = FALSE;
		pParams->bMirrorY = FALSE;
	}

	// TODO: other syle selections (???)

	// done "outside if" so random sequence is invariant
	i = (int )(Random()*2.0) + 1;	// i == 1 or 2

	if (pParams->iShape == TILE_RECTANGULAR)
	{
		i += i;		// i == 2 or 4

		if (Random() < 0.5)
			iNumTilesX *= i;
		else
			iNumTilesY *= i;
	}

	// no smaller than 4 pixels on a side

	iNumTilesX = min( iNumTilesX, w/4);
	iNumTilesY = min( iNumTilesY, h/4);

	int iBevelDepth = 1;		// 0, 1, or 2

	switch (pParams->iTileBevelType)
	{
	case TILE_BEVEL_NONE:
			iBevelDepth = 0;
			break;
	case TILE_BEVEL_SHALLOW:
	case TILE_BEVEL_STANDARD:
			iBevelDepth = 1;
			break;
	case TILE_BEVEL_DEEP:
			iBevelDepth = 2;
			break;
	default:
		ASSERT( FALSE);
	}

	int iMorterWidth = 0;		// 0, 1, or 2

	switch (pParams->iTileGroutType)
	{
	case TILE_GROUT_NONE:
			iMorterWidth = 0;
			break;
	case TILE_GROUT_THIN_SMOOTH:
	case TILE_GROUT_THIN_ROUGH:
			iMorterWidth = 1;
			break;
	case TILE_GROUT_THICK_SMOOTH:
	case TILE_GROUT_THICK_ROUGH:
			iMorterWidth = 2;
			break;
	default:
		ASSERT( FALSE);
	}

	// don't let grout get too thick

	if (iMorterWidth > 0)
	{
		if ((w/iNumTilesX < 4) || (h/iNumTilesY < 4))
			iMorterWidth = 0;
		else if ((w/iNumTilesX < 8) || (h/iNumTilesY < 8))
			iMorterWidth = 1;
	}

	int    iSpacingW   = (w / iNumTilesX);
	int    iSpacingH   = (h / iNumTilesY);
	int    iTileW      = iSpacingW - iMorterWidth;
	int    iTileH      = iSpacingH - iMorterWidth;
	double dfThickness = 1001.0;	// 4.0 looks good, too

	int iStagger = 0;
	
	if (bStagger)
	{
		if ((iNumTilesY <= 2) || (Random() < 0.5))
			iStagger = (iTileW + iMorterWidth) / 2;	// 1/2 brick width 
		else
			iStagger = (iTileW + iMorterWidth) / 4;	// 1/4 brick width
	}

	BYTE* pbyBlockStencilData = (BYTE *)malloc( w*h);

	// fill with invisible color
	FillMemory( pbyBlockStencilData, w*h, 127);

	int iCorner = 3;		// used for hexagonal/octagonal tiles

	for (j = 0; j < iTileH; j++)
	{
		for (i = 0; i < iTileW; i++)
		{
			int  iDelta       = 0;		// no-change by default (0)
			BOOL bTransparent = FALSE;

			if (NeedToTrimCorners( pParams->iShape))
			{
				// chop off corners for hexagonal / octagonal
				if (-i > j-iCorner)								// upper left
					bTransparent = TRUE;
				else if (i-iTileW > j-iCorner-1)				// upper right
					bTransparent = TRUE;
				if (-i > (iTileH-j)-iCorner)					// lower left
					bTransparent = TRUE;
				else if (i-iTileW > (iTileH-j)-iCorner-1)	// lower right
					bTransparent = TRUE;
			}

			if ((! bTransparent) && (iBevelDepth > 0))
			{
				#define SHALLOW (12)
				#define DEEP    (2*SHALLOW)

				if (j == 0)
				{
					iDelta += DEEP;	// brighten
				}

				// NOT else if!  (we want to combine effect on corners)
				if (i == 0)
				{
					iDelta += SHALLOW;	// brighten a bit
				}
				else if (i == iTileW-1)
				{
					iDelta -= SHALLOW;	// darken a bit
				}

				// NOT else if!  (we want to combine effect on corners)
				if (j == iTileH-1)
				{
					iDelta -= DEEP;	// darken
				}

				// extra deep bevel
				if (iBevelDepth > 1)
				{
					int  iDepth   = iBevelDepth-1;
					BOOL bInsideW = (i >= 1) && (i <= iTileW-2);
					BOOL bInsideH = (j >= 1) && (j <= iTileH-2);

					if ((j == 0) || ((j == 1) && bInsideW))
					{
						if (j == 0)
							iDelta += DEEP*iDepth;					// brighten
						else
							iDelta += DEEP*iDepth + SHALLOW;		// brighten more
					}

					// NOT else if!  (we want to combine effect on corners)
					if ((i == 0) || ((i == 1) && bInsideH))
					{
						iDelta += SHALLOW*iDepth;	// brighten a bit
					}
					else if ((i == iTileW-1) || ((i == iTileW-2) && bInsideH))
					{
						iDelta -= SHALLOW*iDepth;	// darken a bit
					}

					// NOT else if!  (we want to combine effect on corners)
					if ((j == iTileH-1) || ((j == iTileH-2) && (i >= 1) && (i <= iTileW-2)))
					{
						iDelta -= DEEP*iDepth;		// darken
					}

					// darken thin seams
					if ((j == 0) || (i == 0))
					{
						iDelta -= SHALLOW + DEEP;	// darken a bit more
					}
					else if ((i == iTileW-1) || (j == iTileH-1))
					{
						iDelta -= SHALLOW;			// darken
					}
				}
			}

			if (pParams->iTileBevelType == TILE_BEVEL_STANDARD)
				iDelta *= 2;

			iDelta = max( -125, iDelta);
			iDelta = min(  125, iDelta);

			if (bTransparent)
				iDelta = 127;	// invisible

			pbyBlockStencilData[j*w + i] = iDelta;
		}
	}

	//////////////////
	// calc morter //
	////////////////

	if (iMorterWidth && (m_iMarbleTileColorGrout != 255))
	{
		for (j = 0; j < iSpacingH; j++)
		{
			for (i = 0; i < iSpacingW; i++)
			{
				BOOL bNeedMorter = FALSE;
				//int iSeam = 0;

				//for (int y = 0; y < iTileH+iMorterWidth; y++)
				{
					//for (int x = -1; x < iTileW+iMorterWidth; x++)
					int x = i;
					int y = j;
					{
						//int ixm = (x + iTileW) % iTileW;
						int ixm = (x + iSpacingW) % iSpacingW;

// neal - TODO: fix "invisible grout" bug (next line) BUGBUG

						// invisible?
						if (pbyBlockStencilData[y*w + ixm] == 127)
						{
//							BOOL bNeedMorter = FALSE;

							//int iX = (x > 0) ? (x-1) : (w-1);
							int iX = ixm-1;
							if (iX < 0)
								iX += w;

							//int iY = (y > 0) ? (y-1) : (h-1);
							int iY = (y > 0) ? (y-1) : (iTileH-1);

							int iR = ixm+1;
							if (iR >= w)
								iR -= w;

							if (pbyBlockStencilData[(iY*w) + ixm] != 127)		// UP visible?
								bNeedMorter = TRUE;
							else if (pbyBlockStencilData[y*w + iX] != 127)	// LEFT visible?
								bNeedMorter = TRUE;
							else if (pbyBlockStencilData[y*w + iR] != 127)	// RIGHT visible?
								bNeedMorter = TRUE;

//							else if (pbyBlockStencilData[(((y-1)*w)%h) + ((x-1)%w)] != 127)	// UP-LEFT visible?
//								bNeedMorter = TRUE;
							else if (iMorterWidth > 1)
							{
								iX = ixm-iMorterWidth;
								if (iX < 0)
									iX += w;

								iY = y-iMorterWidth;
								if (iY < 0)
									//iY += h;
									iY += iTileH;

								if (pbyBlockStencilData[(iY*w) + ixm] != 127)		// UP visible?
									bNeedMorter = TRUE;
								else if (pbyBlockStencilData[y*w + iX] != 127)	// LEFT visible?
									bNeedMorter = TRUE;
								else
								{
									// BUGFIX: when iMorterWidth > 1 and tiles have
									// diagonal corners, some morter pixels are skipped

//									iY = (y > 0) ? (y-1) : (h-1);
									iX = ixm+iMorterWidth;
									if (iX >= w)
										iX -= w;

									if (pbyBlockStencilData[(y*w) + iX] != 127)	// RIGHT visible?
										bNeedMorter = TRUE;
								}
							}

// grout colored already?
//if (pbyBlockStencilData[(y*w)+ixm] == 126)
//	int z = 0;

							if (bNeedMorter)
							{
//if (((y*w)+ixm) == 0)
//	int zz = 0;

								pbyBlockStencilData[(y*w)+ixm] = 126;	// grout colored
								//pbyBlockStencilData[(j*w)+i] = 126;	// grout colored
							}
						}
					}
				}

// grout colored already?
//if (pbyBlockStencilData[(j*w)+i] == 126)
//	int z = 0;

				if (bNeedMorter)
				{
//if (((j*w)+i) == 0)
//	int zz = 0;

					pbyBlockStencilData[(j*w)+i] = 126;	// grout colored
				}

			}
		}
	}

	// now switch to MARBLE random number sequence
	InitRandom( pParams->iRandSeed);

	//////////////////////
	// Build the tiles //
	////////////////////

	for (j = 0; j < iNumTilesY; j++)
	{
		for (i = 0; i < iNumTilesX; i++)
		{
			int* pField = pParams->iARGB_PrimaryField;

			if (bStagger)
			{
				BOOL bMiddle = (j == (iNumTilesY/2-1));
				BOOL bBottom = (j == (iNumTilesY-1));

				// add extra stripes ?

				if (bGrid)
				{
					if (bVariation)
						bMiddle = (j & 1);			// lots (every-other)
					else
						bMiddle = ((j & 3) == 3);	// every fourth
				}

				if (bBottom)			// a bottom stripe
				{
					if (bUseAccentColor && (! bFancyColor))
						pField = pParams->iARGB_AccentField;
					else
						pField = pParams->iARGB_ContrastField;
				}
				else if (bMiddle)		// a middle stripe
				{
					if (bCheckerBoard)
					{
						if (bFancyColor)
						{
							if (((i ^ j) & 1) == 1)
								pField = pParams->iARGB_AccentField;
							else
								pField = pParams->iARGB_ContrastField;
						}
						else if (((i ^ j) & 1) == 1)
						{
							if (bUseAccentColor)
								pField = pParams->iARGB_AccentField;
							else
								pField = pParams->iARGB_ContrastField;
						}
					}
					else
						pField = pParams->iARGB_AccentField;
				}
			}
			else if (bCheckerBoard)
			{
				// checkerboard pattern

				// "^" is XOR
//				int iCheck = (bFancyColor) ? ((i+(i >> 2)) ^ (j+(j >> 2))) : (i ^ j);
				int iCheck = (bGrid) ? ((i+(i >> 2)) ^ (j+(j >> 2))) : (i ^ j);

				if ((iCheck & 1) == 1)
				{
					if (bUseAccentColor && (! bFancyColor))
						pField = pParams->iARGB_AccentField;
					else
						pField = pParams->iARGB_ContrastField;

					if (bMirror)
						pParams->vOffset[2] += dfThickness;
				}
				if (bFancyColor)
				{
					if (bGrid)
					{	
						if (((i&3) == 0) || ((j&3) == 0))
							pField = pParams->iARGB_AccentField;
					}
					else if (((i&3) == 0) && ((j&3) == 0))
						pField = pParams->iARGB_AccentField;
				}
			}
			else if (bFancyColor)
			{
				if (((i&3) == 0) || ((j&3) == 0))
				{
					if (bGrid)
					{
						if (((i&3) == 0) && ((j&3) == 0))
							pField = pParams->iARGB_ContrastField;
						else
							pField = pParams->iARGB_AccentField;
					}
					else
					{
						if (((i&3) == 0) && ((j&3) == 0))
							pField = pParams->iARGB_AccentField;
						else
							pField = pParams->iARGB_ContrastField;
					}
				}
			}
			else if (bUseAccentColor)
			{
				if (bGrid)
				{
					if (((i&3) == 0) || ((j&3) == 0))
						pField = pParams->iARGB_AccentField;
				}
				else if (((i&3) == 0) && ((j&3) == 0))
					pField = pParams->iARGB_AccentField;
			}
			else if (bUseContrastColor)
			{
				if (bGrid)
				{
					if (((i&3) == 0) || ((j&3) == 0))
						pField = pParams->iARGB_ContrastField;
				}
				else if (((i&3) == 0) && ((j&3) == 0))
					pField = pParams->iARGB_ContrastField;
			}

			pParams->vOffset[0] += (i/2) * dfThickness;
			pParams->vOffset[1] += (j/2) * dfThickness;

			if (bMirror)
				pParams->vOffset[2] += j;
			else
				pParams->vOffset[2] += (j*2+i)*dfThickness;

			int k = j * iStagger;

			rTile.SetRect( i*iSpacingW + k, j*iSpacingH, 
					(i+1)*iSpacingW + k, (j+1)*iSpacingH);

			if ((pParams->iShape == TILE_HEXAGONAL) && (iCorner != 0))
					rTile.OffsetRect( -iCorner*i, (iTileH-iCorner)*i);

			GenerateMarbleTexture( pLayer, pField, &rTile, pbyBlockStencilData, 
					pParams->vScale, pParams->vOffset, pParams->dfAmplitude, 
					pParams->iLevels, pParams->iRandSeed, 
					pParams->bSeamless, pParams->iTileGroutType, 
					pParams->bMirrorX&i&1, pParams->bMirrorY&j&1, pParams->bFastPreview);

			// REQUIRED -- restore offset vector
			CopyMemory( pParams->vOffset, vSaveOffset, sizeof( Vec));
		}
	}
	free( pbyBlockStencilData);
	pbyBlockStencilData = NULL;
}

//void CFilter::DoMarbleTex( CLayer* pLayer, Vec vScale, Vec vOffset, 
//			double dfAmplitude, int iLevels, int iNumThickStripes, 
//			int iNumThinStripes, int iColorVariance, int iRandSeed, 
//			BOOL bSeamless, BOOL bExtraSmooth, BOOL bFastPreview)
void CFilter::DoMarbleTex( MarbleParams* pParams)
{
	CLayer* pLayer = pParams->pLayer;

#ifdef _DEBUG
	#undef  MAX_GUARD
	#define MAX_GUARD 256

	DWORD dwGuard[MAX_GUARD];

	for (int i = 0; i < MAX_GUARD; i++)
	{
		dwGuard[i] = 0x12345678;
	}
#endif

	if (! pLayer)
		return;

	PrepareFilter( pLayer, FILTER_MARBLE, "Create Marble Texture");

	int iARGB_Field[4*256];

	GenerateMarbleColorField( pLayer, iARGB_Field, m_iMarbleColorIndexBackground, 
			m_iMarbleColorIndexThickStripes, m_iMarbleColorIndexThinStripes,
			pParams->iNumThickStripes, pParams->iNumThinStripes, pParams->iColorVariance, 
			pParams->iRandSeed, pParams->bExtraSmooth);

#ifdef _DEBUG
	for (i = 0; i < MAX_GUARD; i++)
	{
		ASSERT( dwGuard[i] == 0x12345678);
	}
#endif

	//pLayer->GetDoc()->AssertValid();

	MarbleParams Params;

	Params.pLayer = pLayer;

	Params.ptOffset.x = 0;
	Params.ptOffset.y = 0;

//	Params.sizeTile.cx =;
//	Params.sizeTile.cy =;

	// TILE_SEAMLESS, TILE_SQUARE, TILE_RECTANGULAR, TILE_DIAMOND, etc.
	//Params.iShape = (pParams->bSeamless) ? TILE_SEAMLESS : TILE_SQUARE;
	Params.iShape = pParams->iShape;
	Params.iStyle = PLAIN_ALIGNED;	// PLAIN_BRICK, FANCY_STAGGERED, EXTRA_FANCY_BORDERED, etc.

	// to make tiles of a single marble color, all 3 of these should have same addr
	// to mix two marble colors of tiles, just Contrast and Accent should have same addr
	Params.iARGB_PrimaryField  = iARGB_Field;
	Params.iARGB_AccentField   = iARGB_Field;
	Params.iARGB_ContrastField = iARGB_Field;

	if (Params.iShape == TILE_SEAMLESS)
	{
		///////////////////
		// plain marble //
		/////////////////

		GenerateMarbleTexture( pLayer, iARGB_Field, NULL, NULL,
				pParams->vScale, pParams->vOffset, pParams->dfAmplitude, 
				pParams->iLevels, pParams->iRandSeed, 
				pParams->bSeamless, pParams->iTileGroutType, 
				FALSE, FALSE, pParams->bFastPreview);
	}
	else
	{
		////////////////////////////
		// generate marble tiles //
		//////////////////////////

		Params.vScale[0]        = pParams->vScale[0];
		Params.vScale[1]        = pParams->vScale[1];
		Params.vScale[2]        = pParams->vScale[2];
		Params.vOffset[0]       = pParams->vOffset[0];
		Params.vOffset[1]       = pParams->vOffset[1];
		Params.vOffset[2]       = pParams->vOffset[2];
		Params.dfAmplitude      = pParams->dfAmplitude;
		Params.iLevels          = pParams->iLevels;
		Params.iNumThickStripes = pParams->iNumThickStripes;
		Params.iNumThinStripes  = pParams->iNumThinStripes;
		Params.iColorVariance   = pParams->iColorVariance;
		Params.iRandSeed        = pParams->iRandSeed;
		Params.bSeamless        = pParams->bSeamless;
		Params.bExtraSmooth     = pParams->bExtraSmooth;
		Params.bFastPreview     = pParams->bFastPreview;

		Params.iTileRandSeed    = pParams->iTileRandSeed;
		Params.iTileBevelType   = pParams->iTileBevelType;
		Params.iTileGroutType   = pParams->iTileGroutType;
		Params.iTileSize        = pParams->iTileSize;
		Params.iShape           = pParams->iShape;
		Params.iTileColor       = pParams->iTileColor;
		Params.iTileStyle       = pParams->iTileStyle;

		InitRandom( Params.iTileRandSeed);

		// we want the random number sequence to be invariant
		double dfRand = Random();

		if (Params.iTileSize == TILE_SIZE_ANY)
			Params.iTileSize = TILE_TINY + (int )
					(dfRand*(1+TILE_LARGE-TILE_TINY));

		dfRand = Random();

		if (Params.iTileBevelType == TILE_BEVEL_ANY)
			Params.iTileBevelType = TILE_BEVEL_NONE + (int )
					(dfRand*(1+TILE_BEVEL_DEEP-TILE_BEVEL_NONE));

		dfRand = Random();

		if (Params.iTileGroutType == TILE_GROUT_ANY)
			Params.iTileGroutType = TILE_GROUT_NONE + (int )
					(dfRand*(1+TILE_GROUT_THICK_ROUGH-TILE_GROUT_NONE));

		dfRand = Random();

		if (Params.iShape == TILE_ANY_SHAPE)
			Params.iShape = TILE_SQUARE + (int )
					(dfRand*(1+TILE_RECTANGULAR-TILE_SQUARE));

// neal - DEBUG TEST --------------------------------------------
//Params.iShape = TILE_HEXAGONAL;

		dfRand = Random();

		if (Params.iTileColor == TILE_COLOR_ANY)
			Params.iTileColor = TILE_COLOR_SOLID + (int )
					(dfRand*(1+TILE_COLOR_MULTI-TILE_COLOR_SOLID));

		dfRand = Random();

		if (Params.iTileStyle == TILE_STYLE_ANY)
			Params.iTileStyle = TILE_STYLE_PLAIN + (int )
					(dfRand*(1+TILE_STYLE_EXTRA_FANCY-TILE_STYLE_PLAIN));

		// double check settings
		ASSERT( (Params.iTileBevelType >= TILE_BEVEL_NONE) &&
				(Params.iTileBevelType <= TILE_BEVEL_DEEP));

		ASSERT( (Params.iTileGroutType >= TILE_GROUT_NONE) &&
				(Params.iTileGroutType <= TILE_GROUT_THICK_ROUGH));

		ASSERT( (Params.iTileSize >= TILE_TINY) &&
				(Params.iTileSize <= TILE_LARGE));

		ASSERT( (Params.iShape >= TILE_SEAMLESS) &&
				(Params.iShape <= TILE_OTHER_SHAPE));

		ASSERT( (Params.iTileColor >= TILE_COLOR_SOLID) &&
				(Params.iTileColor <= TILE_COLOR_MULTI));

		ASSERT( (Params.iTileStyle >= TILE_STYLE_PLAIN) &&
				(Params.iTileStyle <= TILE_STYLE_EXTRA_FANCY));

		////////////////////////////////////////
		// Build the aux marble color fields //
		//////////////////////////////////////

		int iARGB_ContrastField[4*256];
		int iARGB_AccentField[4*256];

		// create contrasting color field for "black" tiles

		BOOL bFancyColor        = (Params.iTileColor == TILE_COLOR_MULTI);
		BOOL bUseContrastColor  = bFancyColor || (Params.iTileColor == TILE_COLOR_TWO_TONE);
		BOOL bUseAccentColor    = bFancyColor || (Params.iTileColor == TILE_COLOR_ACCENTED);

		if (bUseContrastColor)
		{
			GenerateMarbleColorField( pLayer, iARGB_ContrastField, m_iMarbleColorIndexThickStripes, 
					m_iMarbleColorIndexThickStripes, m_iMarbleColorIndexThinStripes,
					Params.iNumThickStripes, Params.iNumThinStripes, Params.iColorVariance, 
					Params.iRandSeed, Params.bExtraSmooth);
		}

		// create contrasting color field for "accent" tiles

		if (bUseAccentColor)
		{
			GenerateMarbleColorField( pLayer, iARGB_AccentField, m_iMarbleColorIndexThinStripes, 
					m_iMarbleColorIndexThinStripes, m_iMarbleColorIndexThickStripes,
					Params.iNumThickStripes, Params.iNumThinStripes, Params.iColorVariance, 
					Params.iRandSeed, Params.bExtraSmooth);
		}

		if (bUseContrastColor)
		{
			Params.iARGB_AccentField   = iARGB_ContrastField;
			Params.iARGB_ContrastField = iARGB_ContrastField;
		}

		if (bUseAccentColor)
			Params.iARGB_AccentField = iARGB_AccentField;

		GenerateMarbleSubtile( 0, 0, &Params);
	}
	m_bNeedsFindNearestColor = FALSE;
	EndFilter( pLayer);
};


void CFilter::DoTiledTint( CLayer *pLayer )
{
	PrepareFilter( pLayer, FILTER_TILED_TINT, "Tiled Tint Filter" );

	/*
	#define TILE_SIZE_ANY				TILE_ANY
	#define TILE_TINY					301
	#define TILE_SMALL					302
	#define TILE_MEDIUM					303
	#define TILE_LARGE					304

	#define TILED_TINT_SHADE_ANY		TILE_ANY
	#define TILED_TINT_SHADE_BRIGHTEN	901
	#define TILED_TINT_SHADE_DARKEN		902
	#define TILED_TINT_SHADE_RANDOM		903

	#define TILED_TINT_LEVEL_ANY		TILE_ANY
	#define TILED_TINT_LEVEL_LIGHT		1001
	#define TILED_TINT_LEVEL_MEDIUM		1002
	#define TILED_TINT_LEVEL_HEAVY		1003
	*/
	
	int iWidth		= m_rSelection.Width();
	int iHeight		= m_rSelection.Height();
	int iSelX		= m_rSelection.TopLeft().x;
	int iSelY		= m_rSelection.TopLeft().y;
	int iImageWidth		= m_rSrc.Width();
	int iImageHeight	= m_rSrc.Height();	

	float fColumns	= ( iWidth * 1.0 ) / 16.0;
	int iColumns	= 0;
	float fRows		= ( iHeight * 1.0 ) / 16.0;
	int iRows		= 0;
	float fAdj		= 0.0;
	div_t dt;
	int w			= 0;
	float fW		= 0.0;
	int	h			= 0;
	float fH		= 0.0;
	int j			= 0;
	int iNumBlocks	= 0;
	int iBlockSize  = 0;
	int iIndex		= 0;

	DWORD RTotal	= 0;
	DWORD GTotal	= 0;
	DWORD BTotal	= 0;
	float RAvg		= 0.0;
	float GAvg		= 0.0;
	float BAvg		= 0.0;
	float fMax		= 0.0;
	BYTE RBlock		= 0;
	BYTE GBlock		= 0;
	BYTE BBlock		= 0;
	
	LPTiledTintBlock	BlockList = NULL;

	// Make sure we line up on even boundaries
	dt = div( iWidth, (int)( fColumns + 0.5 ) );
	iColumns = dt.quot;
	
	if( dt.rem != 0 )
	{
		iColumns += 1;
	}	

	dt = div( iHeight, (int)( fRows + 0.5 ) );
	iRows = dt.quot;
	
	if( dt.rem != 0 )
	{
		iRows += 1;
	}

	iNumBlocks = iColumns * iRows;

	BlockList = new TiledTintBlock[ iNumBlocks ];

	// Figure out the width of each column, rounding to the nearest int as we go along
	fAdj = (iWidth * 1.0 ) / (iColumns * 1.0);	
	for( w = iSelX, fW = (iSelX * 1.0); w < iColumns; w++, fW += fAdj )
	{		
		for( h = 0; h < iRows; h++ )
		{
			BlockList[w + (h * iColumns)].iX = (int)( fW + 0.5 );
			BlockList[w + (h * iColumns)].iWidth = (int)( (fW + fAdj) + 0.5 ) - BlockList[w + (h * iColumns)].iX;
		}
	}
	
	// Do the same for the height
	fAdj = (iHeight * 1.0 ) / (iRows * 1.0);	
	for( h = iSelY, fH = (iSelY * 1.0); h < iRows; h++, fH += fAdj )
	{		
		for( w = 0; w < iColumns; w++ )
		{
			BlockList[w + (h * iColumns)].iY = (int)( fH + 0.5 );
			BlockList[w + (h * iColumns)].iHeight = (int)( (fH + fAdj) + 0.5 ) - BlockList[w + (h * iColumns)].iY;
		}		
	}
	
#ifdef _DEBUG
	for( j = 0; j < iNumBlocks; j++ )
	{		
		TRACE( "Block[%lu]  X = %lu  Y = %lu  W = %lu  H = %lu\n", j, BlockList[j].iX, BlockList[j].iY, BlockList[j].iWidth, BlockList[j].iHeight );
	}
#endif

	// Now loop through all of the blocks, calculating their color, and adjusting the pixels based on 
	// that color
	for( j = 0; j < iNumBlocks; j++ )
	{		
		// Sanity check
		ASSERT( (BlockList[j].iX + BlockList[j].iWidth) <= (iSelX + iWidth) );
		ASSERT( (BlockList[j].iY + BlockList[j].iHeight) <= (iSelY + iHeight) );

		RTotal		= 0;
		GTotal		= 0;
		BTotal		= 0;		
		
		for( w = BlockList[j].iX; w < (BlockList[j].iX + BlockList[j].iWidth); w++ )
		{
			for( h = BlockList[j].iY; h < (BlockList[j].iY + BlockList[j].iHeight); h++ )
			{
				iIndex = w + ( h * iImageWidth );
				RTotal += GetRValue( m_pIRGB_SrcBuffer[iIndex] );
				GTotal += GetGValue( m_pIRGB_SrcBuffer[iIndex] );
				BTotal += GetBValue( m_pIRGB_SrcBuffer[iIndex] );
			}
		}

		iBlockSize = BlockList[j].iWidth * BlockList[j].iHeight;

		// Get the average color of this block
		RAvg = (RTotal * 1.0) / (iBlockSize * 1.0);
		GAvg = (GTotal * 1.0) / (iBlockSize * 1.0);
		BAvg = (BTotal * 1.0) / (iBlockSize * 1.0);

		// Normalize it
		/*
		fMax = max( RAvg, GAvg );
		fMax = max( fMax, BAvg );

		RAvg /= fMax;
		GAvg /= fMax;
		BAvg /= fMax;
		*/

		// Do we lighten, darken, or randomize?
		// switch on LPTiledTintParams->iTintShade

		// Lighten:
		RAvg *= 1.25;
		GAvg *= 1.25;
		BAvg *= 1.25;

		// Now average between the calculated color and the actual color
		for( w = BlockList[j].iX; w < (BlockList[j].iX + BlockList[j].iWidth); w++ )
		{
			for( h = BlockList[j].iY; h < (BlockList[j].iY + BlockList[j].iHeight); h++ )
			{
				iIndex = w + ( h * iImageWidth );

				m_pIRGB_SrcBuffer[ iIndex ] = IRGB( 0, 
					min( 255, (int)( ( (GetRValue( m_pIRGB_SrcBuffer[ iIndex ] ) * 2.0) + RAvg) / 3.0) ),
					min( 255, (int)( ( (GetGValue( m_pIRGB_SrcBuffer[ iIndex ] ) * 2.0) + GAvg) / 3.0) ),
					min( 255, (int)( ( (GetBValue( m_pIRGB_SrcBuffer[ iIndex ] ) * 2.0) + BAvg) / 3.0) ) );
			}
		}
	}

	LPVOID lpFile = MakeFile( "c:\\temp\\raw.dat", iImageWidth * iImageHeight * 3 );
	LPBYTE pbyFile = static_cast< LPBYTE>( lpFile );
	for( w = 0; w < (iImageWidth * iImageHeight); w++ )
	{
		pbyFile[ w * 3 + 0 ] = GetRValue( m_pIRGB_SrcBuffer[ w ] );
		pbyFile[ w * 3 + 1 ] = GetGValue( m_pIRGB_SrcBuffer[ w ] );
		pbyFile[ w * 3 + 2 ] = GetBValue( m_pIRGB_SrcBuffer[ w ] );		
	}
	CloseFile( lpFile );
	
	if( BlockList )
	{
		delete []BlockList;
		BlockList = NULL;
	}

	EndFilter( pLayer );
}