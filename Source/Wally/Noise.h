/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// Noise.h : interface of the Noise Helper Functions
//
// Created by Neal White III, 7-25-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _NOISE_H_
#define _NOISE_H_

#ifdef	HUGE
#undef	HUGE
#endif

#define HUGE    (1.0e8)

/*
	generic helpful macros
*/

#define ABS(a)		(((a)<0)?(-(a)):(a))
#define FLOOR(a)	((a)>0?(int)(a):-(int)(a))
#define CEILING(a)	((a)==(int)(a)?(a):(a)>0?1+(int)(a):-(1+(int)(-a))
#define ROUND(a)	((a)>0?(int)((a)+0.5):-(int)(0.5-a))

#define MIN(a,b)	(((a)<(b))?(a):(b))
#define MAX(a,b)	(((a)>(b))?(a):(b))

/*
	typedefs for the world
*/

typedef double	Vec[3];
typedef Vec	Point;
typedef Vec	Color;
typedef double	Matrix[4][4];

/*----------------------------------------------------------------------*/

#ifndef DUMB_CPP 

#define MakeVector(x, y, z, v)		(v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecNegate(a)	(a)[0]=(-(a)[0]);\
			(a)[1]=(-(a)[1]);\
			(a)[2]=(-(a)[2]);
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecLen(a)	(sqrt(VecDot(a,a)))
#define VecCopy(a,b)	(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];
#define VecAdd(a,b,c)	(c)[0]=(a)[0]+(b)[0];\
			(c)[1]=(a)[1]+(b)[1];\
			(c)[2]=(a)[2]+(b)[2]
#define VecSub(a,b,c)	(c)[0]=(a)[0]-(b)[0];\
			(c)[1]=(a)[1]-(b)[1];\
			(c)[2]=(a)[2]-(b)[2]
#define VecComb(A,a,B,b,c)	(c)[0]=(A)*(a)[0]+(B)*(b)[0];\
				(c)[1]=(A)*(a)[1]+(B)*(b)[1];\
			 	(c)[2]=(A)*(a)[2]+(B)*(b)[2]
#define VecS(A,a,b)		(b)[0]=(A)*(a)[0];\
				(b)[1]=(A)*(a)[1];\
				(b)[2]=(A)*(a)[2]
#define VecAddS(A,a,b,c)	(c)[0]=(A)*(a)[0]+(b)[0];\
				(c)[1]=(A)*(a)[1]+(b)[1];\
				(c)[2]=(A)*(a)[2]+(b)[2]
#define VecMul(a,b,c)		(c)[0]=(a)[0]*(b)[0];\
				(c)[1]=(a)[1]*(b)[1];\
				(c)[2]=(a)[2]*(b)[2]
#define VecCross(a,b,c)		(c)[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1];\
				(c)[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2];\
				(c)[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0]
#define VecZero(v)		(v)[0]=0.0; (v)[1]=0.0; v[2]=0.0
#define VecPrint(msg,v)		fprintf(stderr, "%s: %g %g %g\n", msg,\
					(v)[0],(v)[1],(v)[2])

#define matrix_copy(msrc,mdest)	_fmemcpy( (mdest), (msrc), sizeof( Matrix))

#endif /* not DUMB_CPP */

/* surface structure */

typedef struct t_surface {
	Color   diff;                   /* diffuse */
//	Texmap  *tm_diff;
	Color	spec;			/* specular (reflected) */
//	Texmap  *tm_spec;
	Color	amb;			/* ambient */
//	Texmap  *tm_amb;
	double  shine;                  /* specular spot exponent */
	Color   cshine;                 /* spec spot color */
	Color   trans;                  /* transparency */
//	Texmap  *tm_trans;
	double  ior;			/* index of refraction */
	double  fuzz;                   /* surface fuzz */
	double  flags;                  /* is this surface valid for shadow caching */
	struct t_texture	*tex;	/* ptr for color texture */
	struct t_bump           *bump;  /* ptr for surface normal texture */
	Matrix  matrix;                 /* transformation matrix */
} Surface;

/* surface flags */

typedef struct t_turbulence {
	int     terms;          /* # of terms in the series */
	Vec     trans;          /* pre-process numbers */
	Vec     scale;
	double  amp;            /* post turbulence amplification */
} Turbulence;

typedef struct t_wave {
	Vec    center;
	double wavelength,
		amp,		/* should be about 0 to .6 */
		damp,		/* damping per wavelength */
		phase;		/* wavelength offset */
	struct t_wave	*next;	/* next wave in line */
} Wave;

#if 0

typedef struct t_texture {
	Flt	(*func)();	/* returns 0.0..1.0 */
	Flt	blur;		/* % blending between layers */
	Flt	fuzz;		/* white noise blending */
	Flt     r1, r2;         /* just because */
	int     terms;          /* for tex_noise() */
	Vec     trans,
		scale;
	Surface *surf[2];
	Turbulence      *turbulence;
	Wave	*waves;
} Texture;
#endif

#ifndef PI
#define PI 		(3.14159265358979323844)
#endif /* PI */

#define degtorad(x) (((Flt)(x))*PI/180.0)

// helper funtions
void   InitNoise( int iSeed);
double Noise1( Vec vPos);
void   Noise3( Vec vPos, Vec vDest);
double Turb1( Vec vPos, int nLevels);
void   Turb3( Vec vPos, Vec vDest, int nLevels);

#endif	// _NOISE_H_
