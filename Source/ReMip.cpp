/////////////////////////////////////////////////////////////////////////////
// Remip.cpp
//
// Reads a mip file and recalculates the sub-mips.
// This is intended to produce higher quality in 
// the sub-mip textures.
//
// Written by Neal White III
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "ImageHelper.h"
#include "WallyPal.h"

#define WALLY 1		// in Wally?

#ifndef WALLY
	char g_szErrorString[400];
#endif

//#define BOOL  int
//#define TRUE  1
//#define FALSE 0
//
//#define BYTE  unsigned char
//#define WORD  unsigned short
//#define DWORD unsigned long

#include "ReMip.h"

#if 1
	#ifndef WALLY
		#include "c:\src\litedlx\common\cmdlib.h"
		#include "c:\src\litedlx\common\mathlib.h"
		#include "c:\src\litedlx\common\bspfile.h"
		#include "c:\src\litedlx\common\wadlib.h"
	#else		
		#include "stdlib.h"
		#include "stdio.h"
		#include "math.h"
		#include "memory.h"
	#endif
#else
	#include "common\cmdlib.h"
	#include "common\mathlib.h"
	#include "common\bspfile.h"
	#include "wadlib.h"
#endif


//------ ----- ----- #include "random.c" ----- ----- -----
// Random.c
//
// Copyright 1995, Neal White III, All Rights Reserved
//
// Very high quality ramdom number generator (from ACM Journal)
// The standard C rand() may cause visible artifacting.

//#include "random.h"

long l_Seed = 3;

long const a = 16807;
long const m = 2147483647;
long const q = 127773;     // m div a
long const r = 2836;       // m mod a

double Random()		// returns [0.0 - 1.0)  -- never returns higher than 0.999999...
{
	long hi = l_Seed / q;
	long lo = l_Seed % q;
	long test = a * lo - r * hi;

	l_Seed = test;
	if (test <= 0)
		l_Seed += m;

	return ((double )l_Seed / m);
}

void InitRandom( long lSeed)
{
	l_Seed = lSeed;
}

long GetRandomSeed()
{
	return( l_Seed);
}

//------ ----- ----- end "random.c" ----- ----- -----


// Mip stucture from common\bspfile.h
//
#if !defined MIPLEVELS
#define	MIPLEVELS	4
#endif

/*typedef struct waltex_s
{
	char		name[32];
	unsigned	width, height;
	unsigned	offsets[MIPLEVELS];		// four mip maps stored
	char		animation_name[32];
	long		flags;					// skt, slick, light, etc.
	long		contents;				// solid, water, lava, etc.
	long		light_value;			// brightness
} q2_miptex_s;*/

// Ty- fixing contention issues with RebuildWAD and multiple threads
	//COLOR_IRGB  g_rgbIntermediate[512*512];
	//COLOR_IRGB*	g_pIRGB_Intermediate = NULL;
	

#ifndef WALLY
	int			g_iNumFullBrights = NUM_FULL_BRIGHTS;	// should be read from last byte of "palette.lmp" file
	BYTE        g_byPalette[256*3];
	HSV         g_hsvPalette[256];						// palette rgb converted to hsv

	BOOL	g_bVerbose            = 0;
	BOOL	b_bBayer              = FALSE;

	#define USAGE "Usage: ReMipDLX [-verbose] [-noise] [-nosharpen] [-noweight] bspfile"
#endif

	// Ty- moved to globals.cpp
	//BOOL	g_bWeightedAverage;// = TRUE;
	//BOOL	g_bErrorDiffusion;//  = TRUE;
	//BOOL	g_bNoise;//           = FALSE;
	//int     g_iSharpen;       //  = SHARPEN_MEDIUM;
	//BOOL	g_bAutoRemip;            // = FALSE;
	//BOOL	g_bAutoRemipAfterEdit;	 // = FALSE
	//BOOL	g_bRebuildSubMipsOnSave; // = FALSE;

	// neal - used as cache for GetNearestColor (major speed-up!)
	int g_iOldColor = -1;
	int g_iOldR     = -1;
	int g_iOldG     = -1;
	int g_iOldB     = -1;

#ifndef WALLY
/////////////////////////////////////////////////////////////////////////////
// Name:        Help
// Action:      Displays info for -help command line parameter
/////////////////////////////////////////////////////////////////////////////
void Help( void)
{
	printf ("************ HELP ************\n");
	printf( USAGE "\n\n");

	printf( "-noise             use white-noise dithering (original technique)\n");
	printf( "-nosharpen         turns off sharpening\n");
	printf( "-noweight          turns off weighted average (uses plain average)\n");
	printf( "-numfullbrights #  set number of fullbrishts in palette (default = 32)\n");
	printf( "-verbose           displays extra info\n");
	printf( "-help              shows these help pages\n");

	printf( "====================\n");
	printf( "New Features (0.93): added Quake2 WAL file type (uses Q2Pal.lmp)\n");
	printf( "Improvements:        uses Floyd-Steinberg 'error-diffusion' dithering\n");
	printf( "                     use -noise to use old 'white noise' dither technique\n");
	printf( "                     added -nosharpen to turn off sharpening\n");
	printf( "                     added -noweight to turn off weighted average\n");
	printf( "New Features (0.92): added -numfullbrights parameter\n");
	printf( "Improvements:        added white-noise dithering\n");
	printf( "                     tweaked full-bright and gray pixel handling (again)\n");
	printf( "New Features (0.91): added WAD, MIP file types\n");
	printf( "Improvements         better full-bright and gray pixel handling\n");
	printf( "New features (0.90): -verbose is OFF by default\n");
	printf( "Bug fixes:           sup-MIP textures look even better\n");
	printf( "====================\n");
	printf( "\n");
	printf( "This program can read WAD (default), BSP, and MIP files.\n");
	printf( "It rebuilds each MIP it finds and will not touch the original 1:1 bitmaps.\n");
	printf( "\n");
	printf( "I will be adding more details here, as soon as I get around to it.  ;-)\n");


	printf( "\n--- Press ENTER to continue ---\n");
	getchar();
	exit( 1);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Name:        RGBtoHSV
// Action:      Converts red-green-blue color to hue-saturation-value
//
// Notes:		r, g, b are 0..255
//				pH is 0..360, pS and pV are 0.0..1.0
/////////////////////////////////////////////////////////////////////////////
inline void RGBtoHSV( double r, double g, double b, double* pH, double* pS, double* pV)
{
	double fmin, fmax, h, s, v;

	r = min( r, 255.0);
	r = max( r, 0.0) / 255.0;
	g = min( g, 255.0);
	g = max( g, 0.0) / 255.0;
	b = min( b, 255.0);
	b = max( b, 0.0) / 255.0;

	// calc brightness - v
	fmin = min( r, g);
	fmin = min( fmin, b);

	fmax = max( r, g);
	fmax = max( fmax, b);

	v = fmax;

	// calc saturation - s
	if (fmax != 0)
		s = (fmax - fmin) / fmax;
	else
		s = 0.0;

	if (s == 0.0)		// no color (monochromatic grayscale)
	{
		h = 0.0;		// h is actually UNDEFINED
	}
	else				// has color
	{
		double fdelta = fmax - fmin;

		if (r == fmax)
			h = (g - b) / fdelta;	// color is between yellow and magenta
		else if (g == fmax)
			h = 2.0 + (b - r) / fdelta;	// color is between cyan and yellow
		else //if (r == fmax)
			h = 4.0 + (r - g) / fdelta;	// color is between magenta and cyan

		h *= 60.0;

		if (h < 0.0)
			h += 360.0;
	}

	*pH = h;
	*pS = s;
	*pV = v;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        CRGBtoHSV
// Action:      Converts red-green-blue color to hue-saturation-value
//
// Notes:		r, g, b are 0..255
//				pH is 0..360, pS and pV are 0.0..1.0
/////////////////////////////////////////////////////////////////////////////
void CRGBtoHSV( COLORREF rgbColor, double* pH, double* pS, double* pV)
{
	double h, s, v;

	int r = GetRValue( rgbColor);
	int g = GetGValue( rgbColor);
	int b = GetBValue( rgbColor);

	// calc brightness - v
	int imin = min( r, g);
	imin     = min( imin, b);

	int imax = max( r, g);
	imax     = max( imax, b);

	v = imax / 255.0;

	// calc saturation - s
	if (imax != 0)
		s = (imax - imin) / (double )imax;
	else
		s = 0.0;

	if (s == 0.0)		// no color (monochromatic grayscale)
	{
		h = 0.0;		// h is actually UNDEFINED
	}
	else				// has color
	{
		double fdelta = (double )(imax - imin);

		if (r == imax)					// color is between yellow and magenta
			h = (g - b) / fdelta;
		else if (g == imax)				// color is between cyan and yellow
			h = 2.0 + (b - r) / fdelta;
		else //if (r == fmax)			// color is between magenta and cyan
			h = 4.0 + (r - g) / fdelta;

		h *= 60.0;

		if (h < 0.0)
			h += 360.0;
	}

	*pH = h;
	*pS = s;
	*pV = v;
}

/////////////////////////////////////////////////////////////////////////////
// Name:        HSVtoRGB
// Action:      Converts hue-saturation-value to red-green-blue color
//
// Notes:		r, g, b are 0..255
//				pH is 0..360, pS and pV are 0.0..1.0
/////////////////////////////////////////////////////////////////////////////
inline void HSVtoRGB( double dfH, double dfS, double dfV, int* pR, int* pG, int* pB)
{
	if (dfS == 0.0)		// monochromatic (gray) color
	{
		*pR = *pG = *pB = (int )(dfV * 255.0);
	}
	else				// chromatic (has color)
	{
		if (dfH >= 360.0)
			dfH -= 360.0;
		else if (dfH < 0.0)
			dfH += 360.0;

		dfH /= 60.0;
		int    iFloor  = (int )floor( dfH);
		double dfFract = dfH - iFloor;
		double dfP = dfV * (1.0 - dfS);
		double dfQ = dfV * (1.0 - (dfS * dfFract));
		double dfT = dfV * (1.0 - (dfS * (1.0 - dfFract)));

		switch (iFloor)
		{
		case 0:
			*pR = (int )(dfV * 255.0);
			*pG = (int )(dfT * 255.0);
			*pB = (int )(dfP * 255.0);
			break;
		case 1:
			*pR = (int )(dfQ * 255.0);
			*pG = (int )(dfV * 255.0);
			*pB = (int )(dfP * 255.0);
			break;
		case 2:
			*pR = (int )(dfP * 255.0);
			*pG = (int )(dfV * 255.0);
			*pB = (int )(dfT * 255.0);
			break;
		case 3:
			*pR = (int )(dfP * 255.0);
			*pG = (int )(dfQ * 255.0);
			*pB = (int )(dfV * 255.0);
			break;
		case 4:
			*pR = (int )(dfT * 255.0);
			*pG = (int )(dfP * 255.0);
			*pB = (int )(dfV * 255.0);
			break;
		case 5:
			*pR = (int )(dfV * 255.0);
			*pG = (int )(dfP * 255.0);
			*pB = (int )(dfQ * 255.0);
			break;
		default:
			ASSERT( FALSE);
			*pR = 255;			// lavender is not in quake's palette
			*pG = 255;
			*pB = 128;
		}
	}
}

UINT GetDitherType( void)
{
	if (g_bErrorDiffusion)
		return DITHER_ERROR_DIFFUSION;
	else if (g_bNoise)
		return DITHER_WHITE_NOISE;
	else
		return DITHER_NONE;
}

#ifndef WALLY
	/////////////////////////////////////////////////////////////////////////////
	// Name:        FindNearestColor
	// Action:      Converts a 24 bit RGB value into a 256 color index
	//              based on the palette.lmp file
	/////////////////////////////////////////////////////////////////////////////
	inline int FindNearestColor( int r, int g, int b, BOOL bIsFullBright)
	{
		// neal - cache last color found - major speed-up!

		if ((r == g_iOldR) && (g == g_iOldG) && (b == g_iOldB))
			return (g_iOldColor);

		int    i;
		int    iFound   = 255;

		//double distance = 99999999.0;
		//double dr, dg, db, tdist;

		// nw - longs will be faster (and they're big enough)
		long distance = 99999999;
		long dr, dg, db, tdist;

		double dHue, dSaturation, dValue;

		RGBtoHSV( r, g, b, &dHue, &dSaturation, &dValue);

		// find nearest color loop

		if (bIsFullBright)
		{
			for (i = 256-g_iNumFullBrights; i < 255; i++)
			{
	#if USE_RGB
				// use RGB method
				dr = r - g_byPalette[i*3];
				dg = g - g_byPalette[i*3 + 1];
				db = b - g_byPalette[i*3 + 2];

				tdist = dr*dr + dg*dg + db*db;
	#else
				// use HSV method
				dr = g_byPalette[i*3];
				dg = g_byPalette[i*3 + 1];
				db = g_byPalette[i*3 + 2];

				double dHue2, dSaturation2, dValue2;

				// r, g, b are 0..255
				// h is 0..360, s and v are 0.0..1.0
				//RGBtoHSV( dr, dg, db, &dHue2, &dSaturation2, &dValue2);

				// speed up - use pre-built look up table
				dHue2        = g_hsvPalette[i].dfHue;
				dSaturation2 = g_hsvPalette[i].dfSaturation;
				dValue2      = g_hsvPalette[i].dfValue;

				dHue2        -= dHue;
				dSaturation2 -= dSaturation;
				dValue2      -= dValue;

				// rescale
				dHue2        *= 2.0;		// really 360*2 = 720
				dSaturation2 *= 100.0;
				dValue2      *= 400.0;		//was 100.0

				tdist = dHue2*dHue2 + dSaturation2*dSaturation2 + dValue2*dValue2;
	#endif
				if (tdist < distance)
				{
					distance = tdist;
					iFound   = i;

					if (tdist == 0)
						break;			// we found an exact match!
				}
			}
		}
		else
		{
			//BOOL bIsGray = ((r == g) && (r == b));
			//BOOL bIsGray = (dSaturation <= 0.075);
			BOOL bIsGray = (dSaturation <= 0.0156);

	TryAgain:

			for (i = 0; i < 256-g_iNumFullBrights; i++)
			{
				// if pixel should be gray, don't match any nearby COLORS
				if (bIsGray && (! g_hsvPalette[i].bIsGray))
					continue;
	#if USE_RGB
				// use RGB method
				dr = r - g_byPalette[i*3];
				dg = g - g_byPalette[i*3 + 1];
				db = b - g_byPalette[i*3 + 2];

				tdist = dr*dr + dg*dg + db*db;
	#else
				// use HSV method
				dr = g_byPalette[i*3];
				dg = g_byPalette[i*3 + 1];
				db = g_byPalette[i*3 + 2];

				double dHue2, dSaturation2, dValue2;

				// r, g, b are 0..255
				// h is 0..360, s and v are 0.0..1.0
				//RGBtoHSV( dr, dg, db, &dHue2, &dSaturation2, &dValue2);

				// speed up - use pre-built look up table
				dHue2        = g_hsvPalette[i].dfHue;
				dSaturation2 = g_hsvPalette[i].dfSaturation;
				dValue2      = g_hsvPalette[i].dfValue;

				dHue2        -= dHue;
				dSaturation2 -= dSaturation;
				dValue2      -= dValue;

				// rescale
				dHue2        *= 2.0;		// really 360*2 = 720
				dSaturation2 *= 100.0;
				dValue2      *= 400.0;
				tdist = dHue2*dHue2 + dSaturation2*dSaturation2 + dValue2*dValue2;
	#endif
				if (tdist < distance)
				{
					distance = tdist;
					iFound   = i;

					if (tdist == 0)
						break;			// we found an exact match!
				}
			}

			if (bIsGray)		// check if nearest gray is a good match
			{
				dr = r - g_byPalette[iFound*3];
				dg = g - g_byPalette[iFound*3 + 1];
				db = b - g_byPalette[iFound*3 + 2];

				tdist = dr*dr + dg*dg + db*db;

				if (tdist > (3*16*16))
				{
					// not a close enough match
					bIsGray = FALSE;
					goto TryAgain;
				}
			}
		}
		g_iOldColor = iFound;
		g_iOldR     = r;
		g_iOldG     = g;
		g_iOldB     = b;

		return iFound;
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// Name:        ReMip24
// Action:      Builds one 24 bit sub-MIP image
/////////////////////////////////////////////////////////////////////////////
void ReMip24( CWallyPalette* pPal, q2_miptex_s *pMipHeader, BYTE* byData, int iOriginal, int iBuild, COLOR_IRGB* pIRGB_Intermediate)
{
	int i, j, x, y;
	int iOffset      = 0;
	int iOffsetOrig  = pMipHeader->offsets[iOriginal];// - pMipHeader->offsets[0];
	int iOffsetBuild = pMipHeader->offsets[iBuild];// - pMipHeader->offsets[0];
	int iPowOrig     = (int )pow( 2.0, iOriginal);
	int iPowBuild    = (int )pow( 2.0, iBuild);
	int iWidthOrig   = pMipHeader->width / iPowOrig;		// is this correct ????
	int iHeightOrig  = pMipHeader->height / iPowOrig;
	int iWidth       = pMipHeader->width / iPowBuild;		// is this correct ????
	int iHeight      = pMipHeader->height / iPowBuild;
	int iMult        = iPowBuild / iPowOrig;

	for (j = 0; j < iHeight; j++)
	{
		for (i = 0; i < iWidth; i++)
		{
			//////////////////////////////////////
			// average all the pixels approach //
			////////////////////////////////////
			{
				int   r     = 0, g     = 0, b     = 0;		// non-full-bright rgb values
				int   rFull = 0, gFull = 0, bFull = 0;		// full-bright rgb values
				int   rT    = 0, gT    = 0, bT    = 0;

				int   iBrightness        = 0;
				int   iMax               = 0;	// maximum texture index seen in non-full-brights
				int   iMaxBrightness     = 0;
				int   iMin               = 255;	// minimum texture index seen in non-full-brights
				int   iMinBrightness     = 255;
				int   iMaxFull           = 0;	// maximum texture index seen in full-brights
				int   iMaxFullBrightness = 0;
				int   iMinFull           = 255;	// minimum texture index seen in full-brights
				int   iMinFullBrightness = 255;

				int   iTex     = 0;
				int   iPop     = 0;			// population count
				int   iPopFull = 0;
				int   k             = j*iWidthOrig*iMult + i*iMult;
				BOOL  bIsFullBright = FALSE;

				double dfRand;

				double dfPercent = 0.10;

				// iSymWeight and iSymCutoff are used with iEnhance <below>, which
				// helps fix problem textures (small and symetrical like sliplite)
				// by increasing weight of bottom and right edges

				int iSymWeight     = 1;		// weighting for enhanced area (lower right)
				int iSymCutoff     = 1;		// cutoff determines if it is lower right corner

				// iSymFullBright introduces non-full-bright pixels into areas 
				// that are mostly (but not entirely) full-bright pixels
				// (this improves small light sources such as "ceil1_1")

				int iSymFullBright = 1;		// cutoff area (a square: iSymFullBright X iSymFullBright)

				// these numbers have been tweaked "by eye"
				// for best results at each magnificaion level

				if (g_bWeightedAverage)
				{
					switch (iMult)
					{
						case 2:
							iSymWeight     = 1;
							iSymCutoff     = 2;
							iSymFullBright = 2;
							dfPercent      = 0.35;	// was 0.35
							break;
						case 4:
							iSymWeight     = 2;		// was 5,4,8
							iSymCutoff     = 2;
							iSymFullBright = 3;
							dfPercent      = 0.26;	// was 0.28
							break;
						case 8:
							iSymWeight     = 4;		 // was 10, 16;
							iSymCutoff     = 3;
							iSymFullBright = 3;
							dfPercent      = 0.20;
							break;
						default:
							break;
					}
				}

				///////////////////////////
				// inner summation loop //
				/////////////////////////
				for (y = 0; y < iMult; y++)
				{
					// this helps fix problem textures (small and symetrical like sliplite)
					// by increasing weight of bottom and right edges
					int iEnhance = 1;

					for (x = 0; x < iMult; x++)
					{
						// is it on the left or top?
						if ((y < iMult-iSymCutoff) || (x < iMult-iSymCutoff))
							iEnhance = 1;
						else
							iEnhance = iSymWeight;		// bottom right, need to enhance

						// get the texture pixel (texel) value
						iTex = byData[iOffsetOrig + k + y*iWidthOrig + x];

						// now convert iTex into an RGB value

						COLORREF rgb = pPal->GetRGB( iTex);
						rT = iEnhance * GetRValue( rgb);		// r, g, b
						gT = iEnhance * GetGValue( rgb);
						bT = iEnhance * GetBValue( rgb);

						// calc true brightness (not index value)!
						// based on standard TV method (from Foley vanDam)
						iBrightness = (59*gT + 30*rT + 11*bT) / 100;

						////////////////////////
						// NOT a full-bright //
						//////////////////////
						if (iTex < (256-pPal->GetNumFullBrights()))
						{
							iPop += iEnhance;

							r += rT;		// r, g, b
							g += gT;
							b += bT;

							// update max (brightest texel found so far)
							if (iMaxBrightness < iBrightness)
							{
								iMaxBrightness = iBrightness;
								iMax           = iTex;
							}

							// update min (darkest texel found so far)
							if (iMinBrightness > iBrightness)
							{
								iMinBrightness = iBrightness;
								iMin           = iTex;
							}
						}
						///////////////////////////////
						// else it IS a full-bright //
						/////////////////////////////
						else
						{
							// special case
							// it must exist in the bottom right area to be considered
							// as a full-bright (improves small light sources such as "ceil1_1")
							if ((y >= iMult-iSymFullBright) && (x >= iMult-iSymFullBright))
								bIsFullBright = TRUE;

 							iPopFull += iEnhance;

							rFull += rT;		// r, g, b
							gFull += gT;
							bFull += bT;

							// update max (brightest texel found so far)
							if (iMaxFullBrightness < iBrightness)
							{
								iMaxFullBrightness = iBrightness;
								iMaxFull           = iTex;
							}

							// update min (darkest texel found so far)
							if (iMinFullBrightness > iBrightness)
							{
								iMinFullBrightness = iBrightness;
								iMinFull           = iTex;
							}
						}
					}
				}

				// special case small light sources such as "ceil1_1"

				if (iPopFull)				// are there any full-bright pixels at all?
				{
					if ((iMult == 2) || (iWidthOrig >= 32) || (iWidthOrig >= 32))
						bIsFullBright = TRUE;
					else	// smaller sub-mips (iMult == 4 or 8) with dimensions of 16x__ or __x16
					{
						// see if we should let the pixel be a full-bright after all
						if ((j < iHeight-1) && (i < iWidth-1))
							bIsFullBright = TRUE;
					}
				}

				///////////////////////////////
				// Calc final 24 bit result //
				/////////////////////////////

				// compute the average r,g,b

				if (bIsFullBright)
				{
					if (iPopFull)
					{
						r = rFull / iPopFull;
						g = gFull / iPopFull;
						b = bFull / iPopFull;
					}
					iMax = iMaxFull;	// transfer to primary vars
					iMin = iMinFull;

					dfPercent *= 1.50;		// was 1.65, 1.75, 2.0, 1.5
				}
				else
				{
					if (iPop)
					{
						r /= iPop;
						g /= iPop;
						b /= iPop;
					}
				}

				////////////////////////////////
				// ERROR DIFFUSION DITHERING //
				//////////////////////////////
				if (g_bErrorDiffusion)
				{
					// this is Floyd-Steinberg "error-diffusion" dithering
					// (this helps approximate the average color of a texture)

					// don't need to do anything yet
				}

				////////////////////////////
				// WHITE NOISE DITHERING //
				//////////////////////////
				else if (g_bNoise)
				{
					// this is a form of "white-noise" dithering
					// (this helps to bring out details and reduce "flat" areas)
					// and yes, I AM using my own random number generator.
					// (the original C function is not as good and may cause artifacting)
					dfRand = Random();

					if (! bIsFullBright)
					{
						if (dfRand < 0.50)
						{
							// use brightest pixel found?

							COLORREF rgb;

							//if ((dfRand < 0.25) || bIsFullBright)
							if (dfRand < 0.25)
							{
								rgb = pPal->GetRGB( iMax);
							}
							else	// use darkest pixel found
							{
								rgb = pPal->GetRGB( iMin);
							}

							// average in a portion of brightness

							r = (int )((1.0 - dfPercent) * r + dfPercent * GetRValue( rgb));
							g = (int )((1.0 - dfPercent) * g + dfPercent * GetGValue( rgb));
							b = (int )((1.0 - dfPercent) * b + dfPercent * GetBValue( rgb));
						}
						// else 
							// just use existing average of all non-full-bright pixels
					}
				}
/*
				//////////////////////
				// BAYER DITHERING //
				////////////////////
				else if (b_bBayer)
				{
					// this is a form of Bayer "ordered" dithering
					// (this helps to bring out details and reduce "flat" areas)
					// and preserves the over-all coloring better than "white-noise" above

					if (! bIsFullBright)
					{
						//static int iLookUpTable[4] = { -3,1, 3,-1 };
						static int iLookUpTable[4] = { -2,0, 0,2 };

						int i2x2dithered = (i & 1) + ((j & 1) * 2);	// range of 0..3

						r += i2x2dithered;
						r = max( 0, r);
						r = min( r, 255);

						g += i2x2dithered;
						g = max( 0, g);
						g = min( g, 255);

						b += i2x2dithered;
						b = max( 0, b);
						b = min( b, 255);
					}
				}
*/
				//g_rgbIntermediate[j*iWidth + i].byFullBright = bIsFullBright;
				//g_rgbIntermediate[j*iWidth + i].byRed   = r;
				//g_rgbIntermediate[j*iWidth + i].byGreen = g;
				//g_rgbIntermediate[j*iWidth + i].byBlue  = b;

				//g_pIRGB_Intermediate[j*iWidth + i] = IRGB( bIsFullBright, r, g, b);
				pIRGB_Intermediate[j*iWidth + i] = IRGB( bIsFullBright, r, g, b);

				iOffset++;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Name:        Convert24BitTo256Color
// Action:      Converts 24 bit image to 256 color
//
// Parameters:	pSrcData:		24 bit original image
//				byDestData:		Preallocated destination buffer
//				iWidth, iHeight: Duh...
//				iBuild:			Sub-MIP index (0: full size, 1: one-half, 
//								2: one-quarter, 3: one-eighth)
//				bSharpen:		TRUE to allow it if (g_iSharpen != SHARPEN_NONE)
/////////////////////////////////////////////////////////////////////////////
void Convert24BitTo256Color( CWallyPalette* pPal, COLOR_IRGB* pSrcData, 
						BYTE* byDestData, int iWidth, int iHeight, int iBuild, 
						UINT uDitherType, BOOL bSharpen)
{
	int         i, j;
	int         r, g, b;
	int         iOffset       = 0;
	BOOL        bIsFullBright = FALSE;
	COLOR_IRGB* pPixel        = NULL;

	ASSERT( (uDitherType >= DITHER_NONE) && (uDitherType <= DITHER_WHITE_NOISE));

	BOOL bErrorDiffusion = (uDitherType == DITHER_ERROR_DIFFUSION);
	BOOL bNoise          = (uDitherType == DITHER_WHITE_NOISE);

	int iErrorR  = 0, iErrorG  = 0, iErrorB  = 0;
	int iErrorR1 = 0, iErrorG1 = 0, iErrorB1 = 0;
	int iErrorR2 = 0, iErrorG2 = 0, iErrorB2 = 0;
	int iErrorR3 = 0, iErrorG3 = 0, iErrorB3 = 0;
	int iErrorR4 = 0, iErrorG4 = 0, iErrorB4 = 0;

	int static iErrorTermsR[MAX_MIP_SIZE+2];	// holds error terms for error difusion
	int static iErrorTermsG[MAX_MIP_SIZE+2];
	int static iErrorTermsB[MAX_MIP_SIZE+2];
	int* pErrorR;
	int* pErrorG;
	int* pErrorB;

	int static iErrorBelowR[MAX_MIP_SIZE+2];	// holds error terms for error difusion
	int static iErrorBelowG[MAX_MIP_SIZE+2];
	int static iErrorBelowB[MAX_MIP_SIZE+2];
	int* pErrorBelowR;
	int* pErrorBelowG;
	int* pErrorBelowB;

	// init before copying below
	for (i = 0; i < iWidth+2; i++)
	{
		iErrorBelowR[i] = 0;
		iErrorBelowG[i] = 0;
		iErrorBelowB[i] = 0;
	}

	// neal - used as cache for GetNearestColor (major speed-up!)
	g_iOldColor = -1;
	g_iOldR     = -1;
	g_iOldG     = -1;
	g_iOldB     = -1;

	// now convert the pixel to 256 color

	for (j = 0; j < iHeight; j++)
	{
		if (bErrorDiffusion)
		{
			// copy diffusion error terms up one line
			// then clear next line of error terms

			for (i = 0; i < iWidth+2; i++)
			{
				iErrorTermsR[i] = iErrorBelowR[i];
				iErrorBelowR[i] = 0;

				iErrorTermsG[i] = iErrorBelowG[i];
				iErrorBelowG[i] = 0;

				iErrorTermsB[i] = iErrorBelowB[i];
				iErrorBelowB[i] = 0;
			}
			pErrorR = &iErrorTermsR[1];
			pErrorG = &iErrorTermsG[1];
			pErrorB = &iErrorTermsB[1];

			pErrorBelowR = &iErrorBelowR[1];
			pErrorBelowG = &iErrorBelowG[1];
			pErrorBelowB = &iErrorBelowB[1];
		}

		//pErrorR = &iErrorTermsR[1];
		//pErrorG = &iErrorTermsG[1];
		//pErrorB = &iErrorTermsB[1];

		//pErrorBelowR = &iErrorBelowR[1];
		//pErrorBelowG = &iErrorBelowG[1];
		//pErrorBelowB = &iErrorBelowB[1];

		for (i = 0; i < iWidth; i++)
		{
			COLOR_IRGB* pTop    = NULL;
			COLOR_IRGB* pLeft   = NULL;
			COLOR_IRGB* pRight  = NULL;
			COLOR_IRGB* pBottom = NULL;
			int         iWeight = -1;		// standard is -1

			// go right-to-left on odd rows (looks better)

			pPixel        = &pSrcData[j*iWidth + i];
			//bIsFullBright = (pPixel->byFullBright != 0);
			bIsFullBright = (GetIValue( *pPixel) != 0);

//			int iFirstFullBright = (256-pPal->GetNumFullBrights());

			// don't sharpen lights

			if ((! bIsFullBright) && bSharpen)
			{
				// Find top neighbor

				if (j > 0)
					pTop = &pSrcData[(j-1)*iWidth + i];
				else	// j == 0
					pTop = &pSrcData[(iHeight-1)*iWidth + i];

				if (GetIValue( *pTop) != bIsFullBright)	// ignore non-matching full-brights
					pTop = pPixel;

				// Find left neighbor

				if (i > 0)
					pLeft = &pSrcData[j*iWidth + (i-1)];
				else	// j == 0
					pLeft = &pSrcData[j*iWidth + (iWidth-1)];

				if (GetIValue( *pLeft) != bIsFullBright)	// ignore non-matching full-brights
					pLeft = pPixel;

				// Find right neighbor

				if (i < (iWidth-1))
					pRight = &pSrcData[j*iWidth + (i+1)];
				else	// j == 0
					pRight = &pSrcData[j*iWidth + 0];

				if (GetIValue( *pRight) != bIsFullBright)	// ignore non-matching full-brights
					pRight = pPixel;

				// Find bottom neighbor

				if (j < (iHeight-1))
					pBottom = &pSrcData[(j+1)*iWidth + i];
				else	// j == 0
					pBottom = &pSrcData[0*iWidth + i];

				if (GetIValue( *pBottom) != bIsFullBright)	// ignore non-matching full-brights
					pBottom = pPixel;

				// start with weighted top neighbor

				r = iWeight * GetRValue( *pTop);
				g = iWeight * GetGValue( *pTop);
				b = iWeight * GetBValue( *pTop);

				// add in weighted left neighbor

				r += iWeight * GetRValue( *pLeft);
				g += iWeight * GetGValue( *pLeft);
				b += iWeight * GetBValue( *pLeft);

				// add in weighted right neighbor

				r += iWeight * GetRValue( *pRight);
				g += iWeight * GetGValue( *pRight);
				b += iWeight * GetBValue( *pRight);

				// add in weighted bottom neighbor

				r += iWeight * GetRValue( *pBottom);
				g += iWeight * GetGValue( *pBottom);
				b += iWeight * GetBValue( *pBottom);

				// add in weighted center pixel

				r = (4 * GetRValue( *pPixel)) + r;  // 4 + -4 == 0
				g = (4 * GetGValue( *pPixel)) + g;
				b = (4 * GetBValue( *pPixel)) + b;

				// clamp max difference
				// by limiting to no more than +/- iSymScale (1..3)
				// (we don't want it to be too sharp)
				{
					int iSymScale = min( iBuild, 2);

					if (g_iSharpen == SHARPEN_HEAVY)
						iSymScale *= 3;
					else if (g_iSharpen == SHARPEN_LIGHT)
						iSymScale /= 2;

					r = min( r, iSymScale);
					r = max( r, -iSymScale);
					g = min( g, iSymScale);
					g = max( g, -iSymScale);
					b = min( b, iSymScale);
					b = max( b, -iSymScale);
				}

				// add weighted pixel rgb values

				r += GetRValue( *pPixel);
				g += GetGValue( *pPixel);
				b += GetBValue( *pPixel);
			}
			else	// don't sharpen it
			{
				r = GetRValue( *pPixel);	// just copy it for use below
				g = GetGValue( *pPixel);
				b = GetBValue( *pPixel);

				//ASSERT( (r==0) && (g==0) && (b==0));	// TEST TEST TEST
			}

			if (bErrorDiffusion)
			{
				r += *pErrorR;
				g += *pErrorG;
				b += *pErrorB;
			}

			// make sure it stays in range

			ClampRGB( &r, &g, &b);

			// set the pixel

			byDestData[iOffset] = 
					pPal->FindNearestColor( r, g, b, bIsFullBright);

			if (bErrorDiffusion)
			{
				// this is Floyd-Steinberg "error-diffusion" dithering
				// (this helps approximate the average color of a texture)
				// It MUST be done down here because the error terms are
				// based on the conversion to 256 colors.

				// Floyd-Steinberg is what Quake 1 and 2 use

				//k = pSrcData[iOffset];

				//iErrorR = r - g_byPalette[ k*3];
				//iErrorG = g - g_byPalette[ k*3 + 1];
				//iErrorB = b - g_byPalette[ k*3 + 2];

				iErrorR = r - GetRValue( pSrcData[iOffset]);
				iErrorG = g - GetGValue( pSrcData[iOffset]);
				iErrorB = b - GetBValue( pSrcData[iOffset]);

				// 7/16 of error to right

				iErrorR1 = iErrorR * 7 / 16;
				iErrorG1 = iErrorG * 7 / 16;
				iErrorB1 = iErrorB * 7 / 16;

				*(pErrorR+1) += iErrorR1;
				*(pErrorG+1) += iErrorG1;
				*(pErrorB+1) += iErrorB1;

				// 3/16 below left

				iErrorR2 = iErrorR * 3 / 16;
				iErrorG2 = iErrorG * 3 / 16;
				iErrorB2 = iErrorB * 3 / 16;

				*(pErrorBelowR-1) += iErrorR2;
				*(pErrorBelowG-1) += iErrorG2;
				*(pErrorBelowB-1) += iErrorB2;

				// 5/16 below

				iErrorR3 = iErrorR * 5 / 16;
				iErrorG3 = iErrorG * 5 / 16;
				iErrorB3 = iErrorB * 5 / 16;

				*pErrorBelowR += iErrorR3;
				*pErrorBelowG += iErrorG3;
				*pErrorBelowB += iErrorB3;

				// 1/16 below right (without any roundoff)

				iErrorR4 = iErrorR - (iErrorR1 + iErrorR2 + iErrorR3);
				iErrorG4 = iErrorG - (iErrorG1 + iErrorG2 + iErrorG3);
				iErrorB4 = iErrorB - (iErrorB1 + iErrorB2 + iErrorB3);

				*(pErrorBelowR+1) += iErrorR4;
				*(pErrorBelowG+1) += iErrorG4;
				*(pErrorBelowB+1) += iErrorB4;

				// next...

				pErrorR++;
				pErrorG++;
				pErrorB++;

				pErrorBelowR++;
				pErrorBelowG++;
				pErrorBelowB++;
			}
			iOffset++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:        ReMip
// Action:      Builds one 256 color sub-MIP image
/////////////////////////////////////////////////////////////////////////////
void ReMip( CWallyPalette* pPal, q2_miptex_s* pMipHeader, BYTE* byData, int iOriginal, int iBuild, BOOL bSharpen, COLOR_IRGB* pIRGB_Intermediate)
{
	//int iOffsetOrig  = pMipHeader->offsets[iOriginal];// - pMipHeader->offsets[0];
	int iOffsetBuild = pMipHeader->offsets[iBuild];// - pMipHeader->offsets[0];

	int iPowBuild    = (int )pow( 2.0, iBuild);
	int iWidth       = pMipHeader->width / iPowBuild;		// is this correct ????
	int iHeight      = pMipHeader->height / iPowBuild;

#ifndef WALLY
	if (g_bVerbose)
		printf (".");		// MIP status counter
#endif

	//ReMip24( pPal, pMipHeader, byData, iOriginal, iBuild);
	ReMip24( pPal, pMipHeader, byData, iOriginal, iBuild, pIRGB_Intermediate);

	// neal - KEEP NEXT LINES - it causes a real cool bug (great special effect)
	//Convert24BitTo256Color( byData + iOffsetOrig, byData + iOffsetBuild, 
	//				    iWidth, iHeight, iBuild, g_bErrorDiffusion, TRUE);

	//Convert24BitTo256Color( pPal, g_pIRGB_Intermediate, byData + iOffsetBuild, 
	//				   iWidth, iHeight, iBuild, GetDitherType(), bAllowSharpen);
	
	Convert24BitTo256Color( pPal, pIRGB_Intermediate, byData + iOffsetBuild, 
				   iWidth, iHeight, iBuild, GetDitherType(), bSharpen);	
}

/////////////////////////////////////////////////////////////////////////////
// Name:        RebuildWAL
// Action:      Builds all three 256 color sub-MIP images
/////////////////////////////////////////////////////////////////////////////
char* RebuildWAL( CWallyPalette* pPal, BYTE* pbyData)
{
	int          i;
	q2_miptex_s  MipHeader;
	int          iDataSize = 0;
	q2_miptex_s* pWalHeader;

	// init HSV palette

	pWalHeader = (q2_miptex_s* )pbyData;

	// clear out mip header
	memset( &MipHeader, 0, sizeof( MipHeader));

	// init 'placeholder' mip header
	MipHeader.width      = pWalHeader->width;
	MipHeader.height     = pWalHeader->height;
	MipHeader.offsets[0] = pWalHeader->offsets[0];
	MipHeader.offsets[1] = pWalHeader->offsets[1];
	MipHeader.offsets[2] = pWalHeader->offsets[2];
	MipHeader.offsets[3] = pWalHeader->offsets[3];

	// clear out old data (debug)
//	memset( &pbyData[MipHeader.width*MipHeader.height] + sizeof( q2_miptex_s), 200, 
//			(MipHeader.width/2) * (MipHeader.height/2) +
//			(MipHeader.width/4) * (MipHeader.height/4) +
//			(MipHeader.width/8) * (MipHeader.height/8));

	// Ty - fix contention issue with multiple threads
	//ASSERT( g_pIRGB_Intermediate == NULL);
	
	COLOR_IRGB* pIRGB_Intermediate = NULL;

	HGLOBAL hIRGB_Intermediate = GlobalAlloc( GMEM_MOVEABLE, 
				pWalHeader->width * pWalHeader->height * sizeof( COLOR_IRGB));

	if (hIRGB_Intermediate)
	{
		//g_pIRGB_Intermediate = (COLOR_IRGB *)GlobalLock( hIRGB_Intermediate);
		pIRGB_Intermediate = (COLOR_IRGB *)GlobalLock( hIRGB_Intermediate);

		// redo sub-mips
		for (i = 1; i < MIPLEVELS; i++)
		{
			// remap from 1:1 master (looks better)
			ReMip( pPal, &MipHeader, pbyData, i-1, i, (g_iSharpen != SHARPEN_NONE), pIRGB_Intermediate);
		}

		GlobalUnlock( hIRGB_Intermediate);
		GlobalFree( hIRGB_Intermediate);
		hIRGB_Intermediate   = NULL;
		//g_pIRGB_Intermediate = NULL;
		pIRGB_Intermediate = NULL;
	}

	return NULL;	// everything is fine
}



















/// New stuff

/////////////////////////////////////////////////////////////////////////////
// Name:        ReMip24
// Action:      Builds one 24 bit sub-MIP image
/////////////////////////////////////////////////////////////////////////////
void NewReMip24( CWallyPalette* pPal, LPREMIP_HEADER pMipHeader, BYTE* byData, int iOriginal, int iBuild, COLOR_IRGB* pIRGB_Intermediate)
{
	int i, j, x, y;
	int iOffset      = 0;
	int iOffsetOrig  = pMipHeader->iOffsets[iOriginal];// - pMipHeader->offsets[0];
	int iOffsetBuild = pMipHeader->iOffsets[iBuild];// - pMipHeader->offsets[0];
	int iPowOrig     = (int )pow( 2.0, iOriginal);
	int iPowBuild    = (int )pow( 2.0, iBuild);
	
	int iWidthOrig   = pMipHeader->iWidths[0] / iPowOrig;		// is this correct ????
	int iHeightOrig  = pMipHeader->iHeights[0] / iPowOrig;
	int iWidth       = pMipHeader->iWidths[0] / iPowBuild;		// is this correct ????
	int iHeight      = pMipHeader->iHeights[0] / iPowBuild;
	int iMult        = iPowBuild / iPowOrig;

	for (j = 0; j < iHeight; j++)
	{
		for (i = 0; i < iWidth; i++)
		{
			//////////////////////////////////////
			// average all the pixels approach //
			////////////////////////////////////
			{
				int   r     = 0, g     = 0, b     = 0;		// non-full-bright rgb values
				int   rFull = 0, gFull = 0, bFull = 0;		// full-bright rgb values
				int   rT    = 0, gT    = 0, bT    = 0;

				int   iBrightness        = 0;
				int   iMax               = 0;	// maximum texture index seen in non-full-brights
				int   iMaxBrightness     = 0;
				int   iMin               = 255;	// minimum texture index seen in non-full-brights
				int   iMinBrightness     = 255;
				int   iMaxFull           = 0;	// maximum texture index seen in full-brights
				int   iMaxFullBrightness = 0;
				int   iMinFull           = 255;	// minimum texture index seen in full-brights
				int   iMinFullBrightness = 255;

				int   iTex     = 0;
				int   iPop     = 0;			// population count
				int   iPopFull = 0;
				int   k             = j*iWidthOrig*iMult + i*iMult;
				BOOL  bIsFullBright = FALSE;

				double dfRand;

				double dfPercent = 0.10;

				// iSymWeight and iSymCutoff are used with iEnhance <below>, which
				// helps fix problem textures (small and symetrical like sliplite)
				// by increasing weight of bottom and right edges

				int iSymWeight     = 1;		// weighting for enhanced area (lower right)
				int iSymCutoff     = 1;		// cutoff determines if it is lower right corner

				// iSymFullBright introduces non-full-bright pixels into areas 
				// that are mostly (but not entirely) full-bright pixels
				// (this improves small light sources such as "ceil1_1")

				int iSymFullBright = 1;		// cutoff area (a square: iSymFullBright X iSymFullBright)

				// these numbers have been tweaked "by eye"
				// for best results at each magnificaion level

				if (g_bWeightedAverage)
				{
					switch (iMult)
					{
						case 2:
							iSymWeight     = 1;
							iSymCutoff     = 2;
							iSymFullBright = 2;
							dfPercent      = 0.35;	// was 0.35
							break;
						case 4:
							iSymWeight     = 2;		// was 5,4,8
							iSymCutoff     = 2;
							iSymFullBright = 3;
							dfPercent      = 0.26;	// was 0.28
							break;
						case 8:
							iSymWeight     = 4;		 // was 10, 16;
							iSymCutoff     = 3;
							iSymFullBright = 3;
							dfPercent      = 0.20;
							break;
						default:
							break;
					}
				}

				///////////////////////////
				// inner summation loop //
				/////////////////////////
				for (y = 0; y < iMult; y++)
				{
					// this helps fix problem textures (small and symetrical like sliplite)
					// by increasing weight of bottom and right edges
					int iEnhance = 1;

					for (x = 0; x < iMult; x++)
					{
						// is it on the left or top?
						if ((y < iMult-iSymCutoff) || (x < iMult-iSymCutoff))
							iEnhance = 1;
						else
							iEnhance = iSymWeight;		// bottom right, need to enhance

						// get the texture pixel (texel) value
						iTex = byData[iOffsetOrig + k + y*iWidthOrig + x];

						// now convert iTex into an RGB value

						COLORREF rgb = pPal->GetRGB( iTex);
						rT = iEnhance * GetRValue( rgb);		// r, g, b
						gT = iEnhance * GetGValue( rgb);
						bT = iEnhance * GetBValue( rgb);

						// calc true brightness (not index value)!
						// based on standard TV method (from Foley vanDam)
						iBrightness = (59*gT + 30*rT + 11*bT) / 100;

						////////////////////////
						// NOT a full-bright //
						//////////////////////
						if (iTex < (256-pPal->GetNumFullBrights()))
						{
							iPop += iEnhance;

							r += rT;		// r, g, b
							g += gT;
							b += bT;

							// update max (brightest texel found so far)
							if (iMaxBrightness < iBrightness)
							{
								iMaxBrightness = iBrightness;
								iMax           = iTex;
							}

							// update min (darkest texel found so far)
							if (iMinBrightness > iBrightness)
							{
								iMinBrightness = iBrightness;
								iMin           = iTex;
							}
						}
						///////////////////////////////
						// else it IS a full-bright //
						/////////////////////////////
						else
						{
							// special case
							// it must exist in the bottom right area to be considered
							// as a full-bright (improves small light sources such as "ceil1_1")
							if ((y >= iMult-iSymFullBright) && (x >= iMult-iSymFullBright))
								bIsFullBright = TRUE;

 							iPopFull += iEnhance;

							rFull += rT;		// r, g, b
							gFull += gT;
							bFull += bT;

							// update max (brightest texel found so far)
							if (iMaxFullBrightness < iBrightness)
							{
								iMaxFullBrightness = iBrightness;
								iMaxFull           = iTex;
							}

							// update min (darkest texel found so far)
							if (iMinFullBrightness > iBrightness)
							{
								iMinFullBrightness = iBrightness;
								iMinFull           = iTex;
							}
						}
					}
				}

				// special case small light sources such as "ceil1_1"

				if (iPopFull)				// are there any full-bright pixels at all?
				{
					if ((iMult == 2) || (iWidthOrig >= 32) || (iWidthOrig >= 32))
						bIsFullBright = TRUE;
					else	// smaller sub-mips (iMult == 4 or 8) with dimensions of 16x__ or __x16
					{
						// see if we should let the pixel be a full-bright after all
						if ((j < iHeight-1) && (i < iWidth-1))
							bIsFullBright = TRUE;
					}
				}

				///////////////////////////////
				// Calc final 24 bit result //
				/////////////////////////////

				// compute the average r,g,b

				if (bIsFullBright)
				{
					if (iPopFull)
					{
						r = rFull / iPopFull;
						g = gFull / iPopFull;
						b = bFull / iPopFull;
					}
					iMax = iMaxFull;	// transfer to primary vars
					iMin = iMinFull;

					dfPercent *= 1.50;		// was 1.65, 1.75, 2.0, 1.5
				}
				else
				{
					if (iPop)
					{
						r /= iPop;
						g /= iPop;
						b /= iPop;
					}
				}

				////////////////////////////////
				// ERROR DIFFUSION DITHERING //
				//////////////////////////////
				if (g_bErrorDiffusion)
				{
					// this is Floyd-Steinberg "error-diffusion" dithering
					// (this helps approximate the average color of a texture)

					// don't need to do anything yet
				}

				////////////////////////////
				// WHITE NOISE DITHERING //
				//////////////////////////
				else if (g_bNoise)
				{
					// this is a form of "white-noise" dithering
					// (this helps to bring out details and reduce "flat" areas)
					// and yes, I AM using my own random number generator.
					// (the original C function is not as good and may cause artifacting)
					dfRand = Random();

					if (! bIsFullBright)
					{
						if (dfRand < 0.50)
						{
							// use brightest pixel found?

							COLORREF rgb;

							//if ((dfRand < 0.25) || bIsFullBright)
							if (dfRand < 0.25)
							{
								rgb = pPal->GetRGB( iMax);
							}
							else	// use darkest pixel found
							{
								rgb = pPal->GetRGB( iMin);
							}

							// average in a portion of brightness

							r = (int )((1.0 - dfPercent) * r + dfPercent * GetRValue( rgb));
							g = (int )((1.0 - dfPercent) * g + dfPercent * GetGValue( rgb));
							b = (int )((1.0 - dfPercent) * b + dfPercent * GetBValue( rgb));
						}
						// else 
							// just use existing average of all non-full-bright pixels
					}
				}
/*
				//////////////////////
				// BAYER DITHERING //
				////////////////////
				else if (b_bBayer)
				{
					// this is a form of Bayer "ordered" dithering
					// (this helps to bring out details and reduce "flat" areas)
					// and preserves the over-all coloring better than "white-noise" above

					if (! bIsFullBright)
					{
						//static int iLookUpTable[4] = { -3,1, 3,-1 };
						static int iLookUpTable[4] = { -2,0, 0,2 };

						int i2x2dithered = (i & 1) + ((j & 1) * 2);	// range of 0..3

						r += i2x2dithered;
						r = max( 0, r);
						r = min( r, 255);

						g += i2x2dithered;
						g = max( 0, g);
						g = min( g, 255);

						b += i2x2dithered;
						b = max( 0, b);
						b = min( b, 255);
					}
				}
*/
				//g_rgbIntermediate[j*iWidth + i].byFullBright = bIsFullBright;
				//g_rgbIntermediate[j*iWidth + i].byRed   = r;
				//g_rgbIntermediate[j*iWidth + i].byGreen = g;
				//g_rgbIntermediate[j*iWidth + i].byBlue  = b;

				//g_pIRGB_Intermediate[j*iWidth + i] = IRGB( bIsFullBright, r, g, b);
				pIRGB_Intermediate[j*iWidth + i] = IRGB( bIsFullBright, r, g, b);

				iOffset++;
			}
		}
	}
}









/////////////////////////////////////////////////////////////////////////////
// Name:        NewReMip
// Action:      Builds one 256 color sub-MIP image
/////////////////////////////////////////////////////////////////////////////
void NewReMip( CWallyPalette* pPal, LPREMIP_HEADER pMipHeader, BYTE* byData, int iOriginal, int iBuild, BOOL bSharpen, COLOR_IRGB* pIRGB_Intermediate)
{
	int iOffsetBuild = pMipHeader->iOffsets[iBuild];// - pMipHeader->offsets[0];
	
	int iWidth       = (int)(pMipHeader->iWidths[0] / pow( 2.0, iBuild));
	int iHeight      = (int )(pMipHeader->iHeights[0] / pow( 2.0, iBuild));

	if ((iWidth != 0) && (iHeight != 0))
	{
		NewReMip24( pPal, pMipHeader, byData, iOriginal, iBuild, pIRGB_Intermediate);
		Convert24BitTo256Color( pPal, pIRGB_Intermediate, byData + iOffsetBuild, 
				   iWidth, iHeight, iBuild, GetDitherType(), bSharpen);	
	}	
}

char* RebuildMips( CWallyPalette* pPal, BYTE* pbyData, int iNumSubmips, int iStartMip /* = 1 */)
{
	int				i;
	REMIP_HEADER	MipHeader;
	int				iDataSize = 0;
	LPREMIP_HEADER	pMipHeader;

	// init HSV palette

	pMipHeader = (LPREMIP_HEADER)pbyData;

	// clear out mip header
	memset( &MipHeader, 0, sizeof( MipHeader));

	// init 'placeholder' mip header
	memcpy ( &MipHeader, pMipHeader, sizeof( MipHeader));
	
	COLOR_IRGB* pIRGB_Intermediate = NULL;

	HGLOBAL hIRGB_Intermediate = GlobalAlloc( GMEM_MOVEABLE, 
				MipHeader.iWidths[0] * MipHeader.iHeights[0] * sizeof( COLOR_IRGB));

	if (hIRGB_Intermediate)
	{
		//g_pIRGB_Intermediate = (COLOR_IRGB *)GlobalLock( hIRGB_Intermediate);
		pIRGB_Intermediate = (COLOR_IRGB *)GlobalLock( hIRGB_Intermediate);

		// redo sub-mips
		for (i = iStartMip; i < iNumSubmips; i++)
		{
			// remap from 1:1 master (looks better)
			NewReMip( pPal, &MipHeader, pbyData, 0, i, (g_iSharpen != SHARPEN_NONE), pIRGB_Intermediate);
		}

		GlobalUnlock( hIRGB_Intermediate);
		GlobalFree( hIRGB_Intermediate);
		hIRGB_Intermediate   = NULL;		
		pIRGB_Intermediate = NULL;
	}

	return NULL;	// everything is fine
}

