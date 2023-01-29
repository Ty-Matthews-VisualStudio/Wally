// ReMip.h

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _REMIP_H_
#define _REMIP_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WALLY_H_
	#include "Wally.h"
#endif // _MSC_VER >= 1000


//#define MAX_MIP_SIZE      (1024)
#define MAX_MIP_SIZE      (2048)
//#define MAX_MIP_DATA_SIZE (64 * MAX_MIP_SIZE)
#define NUM_FULL_BRIGHTS  (1)//(32)


// 512 is to reserve room for 512 byte header (max)
#define MAX_DATA_SIZE (MAX_MIP_SIZE * MAX_MIP_SIZE + 2048)

#define USE_RGB  1

#ifndef FILE_UNKNOWN
#define FILE_UNKNOWN	0
#endif
#define FILE_WAD		1
#define FILE_BSP		2
#define FILE_MIP		3
#define FILE_PAK		4
#define FILE_WAL		5

#define SHARPEN_HEAVY  101
#define SHARPEN_MEDIUM 102
#define SHARPEN_LIGHT  103
#define SHARPEN_NONE   104

#define DITHER_NONE				201
#define DITHER_ERROR_DIFFUSION	202
#define DITHER_WHITE_NOISE		203


typedef struct
{
	double red;
	double green;
	double blue;
} RGB_T;

typedef struct
{
	int    pindex;
	double red;
	double green;
	double blue;
} IRGB_T;

// Neal - used by ColorOpt
typedef struct
{
	BYTE byRed;
	BYTE byGreen;
	BYTE byBlue;
	BYTE byFullBright;		// if non-zero, this is the palette index of the full-bright color
} DW_IRGB;

typedef struct
{
	double dfHue;
	double dfSaturation;
	double dfValue;
	BOOL   bIsGray;
} HSV;

class CWallyPalette;

typedef struct
{	
	int iWidths[16];
	int iHeights[16];
	int iOffsets[16];
} REMIP_HEADER, *LPREMIP_HEADER;


#ifdef __cplusplus

	extern "C" void InitRandom( long lSeed);
	extern "C" long GetRandomSeed( void);

	// returns [0.0 - 1.0)  -- never returns higher than 0.999999...
	extern "C" double Random();

	extern "C" char* RebuildWAL( CWallyPalette* pPal, BYTE* pbyData);
	extern "C" char* RebuildMips( CWallyPalette* pPal, BYTE* pbyData, int iNumSubmips, int iStartMip = 1);

	extern "C" void  Convert24BitTo256Color( CWallyPalette* pPal, COLOR_IRGB* pSrcData, 
			BYTE* byDestData, int iWidth, int iHeight, int iBuild, 
			UINT uDitherType, BOOL bSharpen);

	//extern "C" int FindNearestColor( int r, int g, int b, BOOL bIsFullBright);
	extern "C" void RGBtoHSV( double r, double g, double b, double* pH, double* pS, double* pV);
	extern "C" void CRGBtoHSV( COLORREF rgbColor, double* pH, double* pS, double* pV);
	extern "C" void HSVtoRGB( double dfH, double dfS, double dfV, int* r, int* g, int* b);
	extern "C" UINT GetDitherType( void);

/////////////////////////////////////////////////////////////////////////////
// Name:        GetV
// Action:      Converts red-green-blue color to hue-saturation-value
//
// Returns:		The Value portion of HSV
/////////////////////////////////////////////////////////////////////////////
inline double GetV( COLOR_IRGB irgbColor)
{
	int r = GetRValue( irgbColor);
	int g = GetGValue( irgbColor);
	int b = GetBValue( irgbColor);

	// calc brightness - v
	int imin = min( r, g);
	imin     = min( imin, b);

	int imax = max( r, g);
	imax     = max( imax, b);

	return (imax / 255.0);	// V of HSV
}

/*
	extern "C" void InitHSVPalette (BYTE *Palette);
	extern "C" BYTE g_byPalette[256*3];
	extern "C" HSV  g_hsvPalette[256];			// palette rgb converted to hsv

	extern "C" BOOL	g_bVerbose;
	extern "C" BOOL	b_bBayer;
	//extern "C" BOOL	g_bSharpen;

	// Ty- moved to globals.h
	extern "C" BOOL	g_bWeightedAverage;
	extern "C" BOOL	g_bErrorDiffusion;
	extern "C" BOOL	g_bNoise;
	extern "C" int  g_iSharpen;
	extern "C" BOOL g_bAutoRemip;
	extern "C" BOOL g_bAutoRemipAfterEdit;
	extern "C" BOOL g_bRebuildSubMipsOnSave;
	*/
#endif


#endif	// _REMIP_H_