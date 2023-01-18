/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Filter.cpp : implementation of the CFilter class
//
// Created by Neal White III, 7-25-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Wally.h"
#include "ReMip.h"
#include "Noise.h"


#define NUMPTS  512
#define P1      173
#define P2      263
#define P3      337
#define phi	0.6180339

static double pts[NUMPTS];

/////////////////////////////////////////////////////////////////////////////
// Name:		InitNoise
// Action:	Generate noise points +/- 0.5
/////////////////////////////////////////////////////////////////////////////
void InitNoise( int iSeed)
{
	InitRandom( iSeed);

	for (int i = 0; i < NUMPTS; i++)
	{
		pts[i] = Random() - 0.5;
	}
}

#define HASH(a,b,c)     (a+b+c & NUMPTS-1)

/////////////////////////////////////////////////////////////////////////////
// Name:		Noise1
// Action:	Generate 1D noise (+/- 0.5)
/////////////////////////////////////////////////////////////////////////////
inline double Noise1( Vec vPos)
{
	int    xi, yi, zi, xa, xb, ya, yb, za, zb;
	double xf, yf, zf;
	double p000, p100, p010, p110;
	double p001, p101, p011, p111;

	xf = vPos[0];
	yf = vPos[1];
	zf = vPos[2];

	xi = (int )floor( xf);
	xa = (int )floor( P1 * (xi*phi - floor(xi*phi)));
	xb = (int )floor( P1 * ((xi+1)*phi - floor((xi+1)*phi)));

	yi = (int )floor( yf);
	ya = (int )floor( P2 * (yi*phi - floor(yi*phi)));
	yb = (int )floor( P2 * ((yi+1)*phi - floor((yi+1)*phi)));

	zi = (int )floor( zf);
	za = (int )floor( P3 * (zi*phi - floor(zi*phi)));
	zb = (int )floor( P3 * ((zi+1)*phi - floor((zi+1)*phi)));

	p000 = pts[HASH(xa, ya, za)];
	p001 = pts[HASH(xa, ya, zb)];
	p010 = pts[HASH(xa, yb, za)];
	p100 = pts[HASH(xb, ya, za)];
	p011 = pts[HASH(xa, yb, zb)];
	p110 = pts[HASH(xb, yb, za)];
	p101 = pts[HASH(xb, ya, zb)];
	p111 = pts[HASH(xb, yb, zb)];

	xf = xf - xi;
	yf = yf - yi;
	zf = zf - zi;

	// Since we're just doing linear interpolation between the points
	// we need to "bend" the line a bit to fake a spline interpolation.
	// This looks almost as good as the spline interpolation but
	// is much faster.

	if (xf > 0.5)
		{
		xf = 1.0 - xf;
		xf = 2.0 * xf * xf;
		xf = 1.0 - xf;
		}
	else
		xf = 2.0 * xf * xf;
		
	if (yf > 0.5)
		{
		yf = 1.0 - yf;
		yf = 2.0 * yf * yf;
		yf = 1.0 - yf;
		}
	else
		yf = 2.0 * yf * yf;

	if (zf > 0.5)
		{
		zf = 1.0 - zf;
		zf = 2.0 * zf * zf;
		zf = 1.0 - zf;
		}
	else
		zf = 2.0 * zf * zf;

	return  p000 * (1-xf) * (1-yf) * (1-zf) +
		p001 * (1-xf) * (1-yf) * zf     +
		p010 * (1-xf) * yf     * (1-zf) +
		p100 * xf     * (1-yf) * (1-zf) +
		p011 * (1-xf) * yf     * zf     +
		p110 * xf     * yf     * (1-zf) +
		p101 * xf     * (1-yf) * zf     +
		p111 * xf     * yf     * zf;
}

/////////////////////////////////////////////////////////////////////////////
// Name:		Noise3
// Action:	Generate 3D noise (+/- 0.5)
/////////////////////////////////////////////////////////////////////////////
void Noise3( Vec vPos, Vec vDest)
{
	Vec vTemp;

	vDest[0] = Noise1( vPos);

	vTemp[0]  = vPos[1] + P1;
	vTemp[1] = vPos[0] + P2;
	vTemp[2] = vPos[2] + P3;
	vDest[1] = Noise1( vTemp);

	vTemp[0] += P2;
	vTemp[1] += P3;
	vTemp[2] += P1;
	vDest[2] = Noise1( vTemp);
}

/////////////////////////////////////////////////////////////////////////////
// Name:		DNoise
// Action:	Generate perturbed 3D noise (+/- 0.5)
/////////////////////////////////////////////////////////////////////////////
inline void DNoise( Vec vSrc, Vec vDest)
{
	Vec    vTemp;
	double center;

	center = Noise1( vSrc);

	vTemp[0] = vSrc[0] + 0.1;
	vTemp[1] = vSrc[1];
	vTemp[2] = vSrc[2];
	vDest[0] = (Noise1( vTemp) - center) * 10.0;

	vTemp[0] = vSrc[0];
	vTemp[1] += 0.1;
	vDest[1] = (Noise1( vTemp) - center) * 10.0;

	vTemp[1] = vSrc[1];
	vTemp[2] += 0.1;
	vDest[2] = (Noise1( vTemp) - center) * 10.0;
}       /* end of DNoise() */


/////////////////////////////////////////////////////////////////////////////
// Name:			Turb1
// Action:		Generate 3D turbulance
//
// Parameters:
//					vPos:    input point (gets thrashed around)
//					iLevels: max number of levels */
/////////////////////////////////////////////////////////////////////////////
double Turb1( Vec vPos, int iLevels)
{
	double result = 0.0;

	switch (iLevels)
	{
		// neal - optimize for most common cases
		case 1:
			result += Noise1( vPos);
			VecS( 2.0, vPos, vPos);
			break;

		case 6:
			result += Noise1( vPos) / 32.0;
			VecS( 2.0, vPos, vPos);
			// drop thru
		case 5:
			result += Noise1( vPos) / 16.0;
			VecS( 2.0, vPos, vPos);
			// drop thru
		case 4:
			result += Noise1( vPos) / 8.0;
			VecS( 2.0, vPos, vPos);
			// drop thru
		case 3:
			result += Noise1( vPos) / 4.0;
			VecS( 2.0, vPos, vPos);
			// drop thru
		case 2:
			result += Noise1( vPos) / 2.0;
			VecS( 2.0, vPos, vPos);
			// drop thru
		//case 1:
			result += Noise1( vPos);
			VecS( 2.0, vPos, vPos);
			break;

		default:
		{
			for (int i = 0; i < iLevels; i++)
			{
				result += Noise1( vPos) / (double)(1 << i);
				VecS( 2.0, vPos, vPos);
			}
		}
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// Name:			Turb3
// Action:		Generate 3D turbulance
//
// Parameters:
//					vPos:    input point (gets thrashed around)
//					vDest:   resultant vector
//					iLevels: max number of levels */
/////////////////////////////////////////////////////////////////////////////
void Turb3( Vec vPos, Vec vDest, int iLevels)
{
	Vec vTemp;

	MakeVector( 0, 0, 0, vDest);

/*
	switch (iLevels)
	{
		// neal - optimize for most common cases
		case 1:
			//Noise3( vPos, vTemp);
			DNoise( vPos, vTemp);

			VecS( 1.0, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			//VecS( 2.0, p, p);
			break;

		case 6:
			DNoise( vPos, vTemp);

			//VecS( 1.0 / (Flt )(1 << i), vTemp, vTemp);
			//VecS( 1.0 / 32.0, vTemp, vTemp);
			VecS( 0.03125, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			// drop thru

		case 5:
			DNoise( vPos, vTemp);

			//VecS( 1.0 / 16.0, vTemp, vTemp);
			VecS( 0.0625, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			// drop thru

		case 4:
			DNoise( vPos, vTemp);

			//VecS( 1.0 / 8.0, vTemp, vTemp);
			VecS( 0.125, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			// drop thru

		case 3:
			DNoise( vPos, vTemp);

			//VecS( 1.0 / 4.0, vTemp, vTemp);
			VecS( 0.25, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			// drop thru

		case 2:
			DNoise( vPos, vTemp);

			//VecS( 1.0 / 2.0, vTemp, vTemp);
			VecS( 0.5, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			// drop thru

		//case 1:
			DNoise( vPos, vTemp);

			VecS( 1.0, vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			//VecS( 2.0, vPos, vPos);
			break;

		default:
*/
	{
		{
		for (int i = 0; i < iLevels; i++)
			{
			DNoise( vPos, vTemp);

			VecS( 1.0 / (double )(1 << i), vTemp, vTemp);
			VecAdd( vTemp, vDest, vDest);
			VecS( 2.0, vPos, vPos);
			}
		}
	}
}



