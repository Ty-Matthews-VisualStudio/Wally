/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ColorOpt.cpp : implementation of Color Optimization (reduction) helper
//
// Created by Neal White III, 12-24-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "ReMip.h"
#include "ColorOpt.h"
//#include "ProgressBar.h"
#include "MiscFunctions.h"

//#define HIST_BIT   (6)
#define HIST_BIT   (7)				// number of usable color bits
#define HIST_MAX   (1 << HIST_BIT)
#define R_STRIDE   (HIST_MAX * HIST_MAX)
#define G_STRIDE   (HIST_MAX)
#define HIST_CELLS (HIST_MAX * HIST_MAX * HIST_MAX)
#define HIST_SHIFT (8 - HIST_BIT)

CColorOptimizer::CColorOptimizer()
{
}

CColorOptimizer::~CColorOptimizer()
{
}

void* CColorOptimizer::SmartAlloc( UINT uSize)
{
	// neal - Or should this be "smart-aleck"?  ;-)

	if (uSize == 0)
		return NULL;

	void* p = malloc( uSize);

	if (p == NULL)
	{
		ASSERT( FALSE);
//		AfxMessageBox( "ColorOpt Error: out of memory!");
		AfxThrowMemoryException();
	}
#if OLD_METHOD
	ZeroMemory( p, uSize);
#endif
	return p;
}

//////////////////////////////////////////////////////////////////////////////
// Name:	ColorOpt
// Action:	Creates a optimized palette with iMaxColors color indexes
//
// Returns:	The number of colors actually used (or 0 for failure)
//////////////////////////////////////////////////////////////////////////////
int CColorOptimizer::Optimize( COLOR_IRGB* irgbBuffer, int iSizeX, int iSizeY, 
			  BYTE* pbyPackedPalette, int iMaxColors, BOOL bShowProgrssbar)
{
	int iNumColors = 0;

	if ((iMaxColors < 1) || (iMaxColors > 256))
	{
		ASSERT( FALSE);
		//AfxMessageBox( "Error: invalid number of destination colors!");
		iMaxColors = 256;
	}

	if ((iSizeX < 1) || (iSizeY < 1))
	{
		AfxMessageBox( "ColorOpt Error: invalid image dimension!");
	}
	else
	{
		CWaitCursor   curWait;
		CProgressBar* pctlProgress = NULL;

		if (bShowProgrssbar)
		{
			TRY
			{
				pctlProgress = new CProgressBar( "Preprocessing...", 50, 100, false, 0);
			}
			CATCH_ALL( e)
			{
			}
			END_CATCH_ALL
		}

#if OLD_METHOD

		UINT*  histogram = NULL;

		TRY
		{
			int i, j;
			histogram = (UINT *)SmartAlloc( HIST_CELLS * sizeof( UINT));

			TRACE0( "***Building histogram...\n");

			ASSERT(sizeof(DW_IRGB) == sizeof(COLOR_IRGB));
			DW_IRGB* pirgbBuf = (DW_IRGB *)irgbBuffer;

			for (j = 0; j < iSizeY; j++)
			{
				for (i = 0; i < iSizeX; i++)
				{
					int r = pirgbBuf->byRed   >> HIST_SHIFT;
					int g = pirgbBuf->byGreen >> HIST_SHIFT;
					int b = pirgbBuf->byBlue  >> HIST_SHIFT;
					pirgbBuf++;

					histogram[(r * R_STRIDE) + (g * G_STRIDE) + b]++;
				}
			}

			TRACE1( "***Quantizing to a maximum of %i colors...\n", iMaxColors);

			ZeroMemory( pbyPackedPalette, 256*3);

			VERIFY(Quantize( histogram, iMaxColors, pbyPackedPalette, &iNumColors));

			TRACE1( "***Quantized to %i colors\n", iNumColors);
		}
		CATCH_ALL( e)
		{
			AfxMessageBox( "ColorOpt Error: out of memory!");
		}
		END_CATCH_ALL

		if (histogram)
			free( histogram);
#endif

#if BETTER_METHOD
		m_pBetterHistNodeList = NULL;

		TRY
		{
			int i, j, k, m;
			int iHistSize    = 0;
			int iHistMaxSize = (iSizeX*iSizeY * sizeof( CColorNode)) / 8;

			ASSERT (sizeof( CColorNode) == 8);	// record packing is not set correctly.

			m_pBetterHistNodeList = (CColorNode *)SmartAlloc( iHistMaxSize * sizeof( CColorNode));

			TRACE0( "***Building histogram...\n");

			COLOR_IRGB* pIRGB_Buf = irgbBuffer;

			for (j = 0; j < iSizeY; j++)
			{
				for (i = 0; i < iSizeX; i++)
				{
					COLORREF rgb = *pIRGB_Buf;
					pIRGB_Buf++;

					// TODO: implement hash table speed-up

					// put it in the histogram

					m = -1;
					for (k = 0; k < iHistSize; k++)
					{
						if ((rgb & 0x00FFFFFF) == m_pBetterHistNodeList[k].GetRGB())
						{
							m = k;
							break;
						}
					}

					if (m > -1)
						m_pBetterHistNodeList[m].IncCount();
					else
					{
						if (iHistSize >= iHistMaxSize)
						{
							//AfxMessageBox( "ColorOpt Notification: memory reallocated");

							iHistMaxSize += 1024;
							m_pBetterHistNodeList = (CColorNode *)realloc( 
									(void *)m_pBetterHistNodeList, 
									iHistMaxSize * sizeof( CColorNode));

							if (m_pBetterHistNodeList == NULL)
							{
								ASSERT( FALSE);
								AfxMessageBox( "ColorOpt Error: out of memory!");
								return 0;
							}
						}
						m_pBetterHistNodeList[iHistSize].SetRGB( rgb);
						m_pBetterHistNodeList[iHistSize].SetCount( 1);
						m_pBetterHistNodeList[iHistSize].SetNextIndex( iHistSize+1);
						iHistSize++;
					}
				}

				// building histogram matrix accounts for 20%

				if (pctlProgress)
					pctlProgress->SetPos( (j * 20) / iSizeY);
			}

			// mark last node as END-OF-LIST

			m_pBetterHistNodeList[iHistSize-1].SetNextIndex( NIL_NODE);

			TRACE1( "***Quantizing to a maximum of %i colors...\n", iMaxColors);

//			iMaxColors = 20;		// TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

			VERIFY(BetterQuantize( m_pBetterHistNodeList, iMaxColors, 
							pbyPackedPalette, &iNumColors, pctlProgress));

			TRACE1( "***Quantized to %i colors\n", iNumColors);
		}
		CATCH_ALL( e)
		{
			AfxMessageBox( "ColorOpt Error: out of memory!");
		}
		END_CATCH_ALL

		if (m_pBetterHistNodeList)
			free( m_pBetterHistNodeList);

		if (m_pBetterBoxes)
			free( m_pBetterBoxes);
#endif
		if (pctlProgress)
		{
			pctlProgress->Clear();
			delete pctlProgress;
		}
	}

	// Ty- if there aren't many colors, blend the rest based on the overall color.  Produces a broader palette
	// when maybe only 20 were originally selected.

	if (iNumColors < 256)
	{
		BYTE byTempPalette[768];
		BYTE byBlendPalette[258 * 3];
		memcpy (byTempPalette, pbyPackedPalette, iNumColors * 3);
				
		float fR;
		float fG;
		float fB;
		int j = 0;
		int iValue = 0;

		// Get the average overall color
		CalcImageColor24 (iSizeX, iSizeY, irgbBuffer, &fR, &fG, &fB);

		// First palette entry is solid black, last entry solid white, middle is the average color
		memset (byBlendPalette, 0, 3);
		byBlendPalette[128 * 3 + 0] = (int)(fR * 255.0);
		byBlendPalette[128 * 3 + 1] = (int)(fG * 255.0);
		byBlendPalette[128 * 3 + 2] = (int)(fB * 255.0);
		memset (byBlendPalette + (257 * 3), 255, 3);

		// Blend from black->color and then color->white
		BlendPalette (byBlendPalette, 0, 128);
		BlendPalette (byBlendPalette, 128, 257);

		// Separation between selections from the blend palette
		double dfBlenders = 256.0 / ( 1.0 * (256 - iNumColors));
		double dfCurrent = dfBlenders;

		// Start after the last palette item currently used
		for (j = iNumColors; j < 256; j++)
		{
			iValue = (int)dfCurrent;
			
			//memcpy (byTempPalette + (j * 3), byBlendPalette + (iValue * 3), 3);
			byTempPalette[ j * 3 + 0 ] = byBlendPalette[ iValue * 3 + 0 ];
			byTempPalette[ j * 3 + 1 ] = byBlendPalette[ iValue * 3 + 1 ];
			byTempPalette[ j * 3 + 2 ] = byBlendPalette[ iValue * 3 + 2 ];
			
			dfCurrent += dfBlenders;
		}

		memcpy (pbyPackedPalette, byTempPalette, 768);
	}
	return iNumColors;
}

#if OLD_METHOD
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 *      A Practical Implementation of Xiaolin Wu's Color Quantizer
 *           (See Graphics Gems, Volume Two, Pages 126-133)
 *                     October 23, 1995 - v2
 *
 * Author:
 *
 * Rich Geldreich, Jr.
 *
 * Description:
 *
 * This module is an implementation of a high-speed, low-memory
 * and relatively easy to understand statistical color quantizer.
 * Its operation is similar to other Heckbert-style quantizers,
 * except that each box is weighted by variance (instead of by the
 * much more naive methods of weighting boxes by either population, size,
 * or a combination of the two), and each box is splitted on the
 * axis which will minimize the sum of the variances of both new boxes.
 *
 * Notes:
 *
 * (1) Int's are assumed to be at least 32-bits wide.
 *
 * (2) The area sum table approach to gathering color statistics is not
 * implemented here to conserve memory. Instead, a brute force method
 * of gathering color statistics is employed, which is surprisingly fast
 * and easily optimized in assembler.
 *
 * (3) A binary tree based priority list is employed to speed up the
 * search for the box with the largest variance. This differs from the
 * usual technique of using a simple linear search.
 *
 * (4) Although floating point math is used in the Variance() function,
 * this may be easily replaced with high-precision fixed point math on
 * machines with weak floating point math capability.
 *
 * (5) The output of this function is an array of 8-bit palette entries.
 * It is up to you to map the original image's true-color (24-bit) pixels
 * to palettized (8-bit or less) pixels.  Spencer W. Thomas's  inverse
 * colormap functions serve this purpose very well. (See INV_CMAP.C)
 *
 */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Shrinks box to minimum possible size.                                      */
/*----------------------------------------------------------------------------*/
void CColorOptimizer::ShrinkBox( int ir, int ig, int ib,
                       int jr, int jg, int jb,
                       int *lr, int *lg, int *lb,
                       int *hr, int *hg, int *hb)
{
	int r, g, b;
	UINT *rp, *gp, *bp, *s;

	s = puHist + (ir * R_STRIDE + ig * G_STRIDE + ib);

	rp = s;

	for (r = ir; r <= jr; r++)
	{
		gp = rp;

		for (g = ig; g <= jg; g++)
		{
			bp = gp;

			for (b = ib; b <= jb; b++)
			{
				if (*bp++)
				{
					*lr = r;
					goto lr_done;
				}
			}
			gp += G_STRIDE;
		}
		rp += R_STRIDE;
	}

lr_done:

	gp = s;

	for (g = ig; g <= jg; g++)
	{
		rp = gp;

		for (r = ir; r <= jr; r++)
		{
			bp = rp;

			for (b = ib; b <= jb; b++)
			{
				if (*bp++)
				{
					*lg = g; 
					goto lg_done;
				}
			}
			rp += R_STRIDE;
		}
		gp += G_STRIDE;
	}

lg_done:

	bp = s;

	for (b = ib; b <= jb; b++)
	{
		rp = bp;

		for (r = ir; r <= jr; r++)
		{
			gp = rp;

			for (g = ig; g <= jg; g++, gp += G_STRIDE)
			{
				if (*gp)
				{
					*lb = b;
					goto lb_done;
				}
			}
			rp += R_STRIDE;
		}
		bp++;
	}

lb_done:

	s = puHist + (jr * R_STRIDE + jg * G_STRIDE + jb);

	rp = s;

	for (r = jr; r >= ir; r--)
	{
		gp = rp;

		for (g = jg; g >= ig; g--)
		{
			bp = gp;

			for (b = jb; b >= ib; b--)
			{
				if (*bp--)
				{
					*hr = r;
					goto hr_done;
				}
			}
			gp -= G_STRIDE;
		}
		rp -= R_STRIDE;
	}

hr_done:

	gp = s;

	for (g = jg; g >= ig; g--)
	{
		rp = gp;

		for (r = jr; r >= ir; r--)
		{
			bp = rp;

			for (b = jb; b >= ib; b--)
			{
				if (*bp--)
				{
					*hg = g;
					goto hg_done;
				}
			}
			rp -= R_STRIDE;
		}
		gp -= G_STRIDE;
	}

hg_done:

	bp = s;

	for (b = jb; b >= ib; b--)
	{
		gp = bp;

		for (g = jg; g >= ig; g--)
		{
			rp = gp;

			for (r = jr; r >= ir; r--, rp -= R_STRIDE)
			{
				if (*rp)
				{
					*hb = b;
					goto hb_done;
				}
			}
			gp -= G_STRIDE;
		}
		bp--;
	}

hb_done:
	return;
}

/*----------------------------------------------------------------------------*/
/* Standard binary tree based priorty queue manipulation functions.           */
/*----------------------------------------------------------------------------*/
void CColorOptimizer::down_heap(void)
{
	UINT i, j, q;
	box *p;

	p = pHeap[1];
	q = p->uVariance;

	for (i = 1; ; )
	{
		if ((j = i << 1) > uHeapSize)
		break;

		if (j < uHeapSize)
		{
			if (pHeap[j]->uVariance < pHeap[j + 1]->uVariance)
				j++;
		}

		if (q >= pHeap[j]->uVariance)
			break;

		pHeap[i] = pHeap[j];
		i = j;
	}

	pHeap[i] = p;
}

/*----------------------------------------------------------------------------*/
void CColorOptimizer::insert_heap(box *p)
{
	UINT i, j, q;

	q = p->uVariance;
	j = ++uHeapSize;

	for ( ; ; )
	{
		if (((i = j >> 1) == 0) || (pHeap[i]->uVariance >= q))
			break;

		pHeap[j] = pHeap[i];
		j = i;
	}
	pHeap[j] = p;
}

/*----------------------------------------------------------------------------*/
/* Calculate statistics over the specified box. This is an implementation of  */
/* the "brute force" method of gathering statistics described earlier.        */
/*----------------------------------------------------------------------------*/
void CColorOptimizer::Sum( int ir, int ig, int ib,
		int jr, int jg, int jb,
		UINT *uTotalWeight,
		UINT *tt_sum, UINT *t_ur, UINT *t_ug, UINT *t_ub)
{
	int i, j, r, g, b;
	UINT rs, ts;
	UINT w, tr, tg, tb;
	UINT *rp, *gp, *bp;

	j = 0;

	tr = tg = tb = i = 0;

	rp = puHist + ((ir * R_STRIDE) + (ig * G_STRIDE) + ib);

	for (r = ir; r <= jr; r++)
	{
		rs = r * r;
		gp = rp;

		for (g = ig; g <= jg; g++)
		{
			ts = rs + (g * g);
			bp = gp;

			for (b = ib; b <= jb; b++)
			{
				if (*bp++)					// was this cell used at all?
				{
					w   = *(bp - 1);		// update statistics
					j  += w;
					tr += r * w;
					tg += g * w;
					tb += b * w;
					i  += (ts + b * b) * w;
				}
			}
			gp += G_STRIDE;
		}
	rp += R_STRIDE;
	}

	*uTotalWeight = j;
	*tt_sum       = i;
	*t_ur         = tr;
	*t_ug         = tg;
	*t_ub         = tb;
}

//////////////////////////////////////////////////////////////////////////////
// Name:	SplitBox
// Action:	Splits box along the axis which will minimize the two new
//			boxes overall variance. A search on each axis (r, g, and b)
//			is used to locate the optimum split point.
//////////////////////////////////////////////////////////////////////////////
void CColorOptimizer::SplitBox( box* pOldBox)
{
	int i;

	UINT uTotalWeight;
	UINT tt_sum, t_ur, t_ug, t_ub;
	int  ir, ig, ib, jr, jg, jb;

	UINT uTotalWeight1;
	UINT tt_sum1, t_ur1, t_ug1, t_ub1;
	int  ir1, ig1, ib1, jr1, jg1, jb1;

	UINT uTotalWeight2;
	UINT tt_sum2, t_ur2, t_ug2, t_ub2;
	int  ir2, ig2, ib2, jr2, jg2, jb2;

	UINT uTotalWeight3;
	UINT tt_sum3, t_ur3, t_ug3, t_ub3;

	UINT lowest_variance, variance_r, variance_g, variance_b;
	int  pick_r, pick_g, pick_b;

	box* pNewBox = pBoxes + iNumBoxes;
	iNumBoxes++;

	uTotalWeight          = pOldBox->uTotalWeight;
	tt_sum                = pOldBox->tt_sum;
	t_ur                  = pOldBox->t_ur;
	t_ug                  = pOldBox->t_ug;
	t_ub                  = pOldBox->t_ub;
	ir                    = pOldBox->ir;
	ig                    = pOldBox->ig;
	ib                    = pOldBox->ib;
	jr                    = pOldBox->jr;
	jg                    = pOldBox->jg;
	jb                    = pOldBox->jb;

	// left box's initial statistics

	uTotalWeight1         = 0;
	tt_sum1               = 0;
	t_ur1                 = 0;
	t_ug1                 = 0;
	t_ub1                 = 0;

	// right box's initial statistics

	uTotalWeight2         = uTotalWeight;
	tt_sum2               = tt_sum;
	t_ur2                 = t_ur;
	t_ug2                 = t_ug;
	t_ub2                 = t_ub;

	// Note: One useful optimization has been purposefully omitted from the
	// following loops. The variance function is always called twice per
	// iteration to calculate the new total variance. This is a waste of time
	// in the possibly common case when the new split point did not shift any
	// new points from one box into the other. A simple test can be added to
	// remove this inefficiency.

	// locate optimum split point on red axis

	variance_r = 0xFFFFFFFF;

	for (i = ir; i < jr; i++)
	{
		UINT total_variance;

		/* calculate the statistics for the area being taken
		 * away from the right box and given to the left box
		 */

		Sum( i, ig, ib, i, jg, jb,
			&uTotalWeight3, &tt_sum3, &t_ur3, &t_ug3, &t_ub3);

		ASSERT(uTotalWeight3 < uTotalWeight);

		// update left and right box's statistics

		uTotalWeight1 += uTotalWeight3;
		tt_sum1       += tt_sum3;
		t_ur1         += t_ur3;
		t_ug1         += t_ug3;
		t_ub1         += t_ub3;

		uTotalWeight2 -= uTotalWeight3;
		tt_sum2       -= tt_sum3;
		t_ur2         -= t_ur3;
		t_ug2         -= t_ug3;
		t_ub2         -= t_ub3;

		if ((uTotalWeight1 + uTotalWeight2) != uTotalWeight)
			ASSERT( FALSE);

		// calc left and right box's overall variance

		total_variance = Variance( uTotalWeight1, tt_sum1, t_ur1, t_ug1, t_ub1) +
						 Variance( uTotalWeight2, tt_sum2, t_ur2, t_ug2, t_ub2);

		// found better split point? if so, remember it

		if (total_variance < variance_r)
		{
			variance_r = total_variance;
			pick_r = i;
		}
	}

	// left box's initial statistics

	uTotalWeight1         = 0;
	tt_sum1               = 0;
	t_ur1                 = 0;
	t_ug1                 = 0;
	t_ub1                 = 0;

	// right box's initial statistics

	uTotalWeight2         = uTotalWeight;
	tt_sum2               = tt_sum;
	t_ur2                 = t_ur;
	t_ug2                 = t_ug;
	t_ub2                 = t_ub;

	// locate optimum split point on green axis

	variance_g = 0xFFFFFFFF;

	for (i = ig; i < jg; i++)
	{
		UINT total_variance;

		// calc the statistics for the area being taken
		// away from the right box and given to the left box

		Sum( ir, i, ib, jr, i, jb,
			&uTotalWeight3, &tt_sum3, &t_ur3, &t_ug3, &t_ub3);

		if (uTotalWeight3 > uTotalWeight)
			ASSERT( FALSE);

		// update left and right box's statistics

		uTotalWeight1 += uTotalWeight3;
		tt_sum1       += tt_sum3;
		t_ur1         += t_ur3;
		t_ug1         += t_ug3;
		t_ub1         += t_ub3;

		uTotalWeight2 -= uTotalWeight3;
		tt_sum2       -= tt_sum3;
		t_ur2         -= t_ur3;
		t_ug2         -= t_ug3;
		t_ub2         -= t_ub3;

		if ((uTotalWeight1 + uTotalWeight2) != uTotalWeight)
			ASSERT( FALSE);

		// calc left and right box's overall variance

		total_variance = Variance( uTotalWeight1, tt_sum1, t_ur1, t_ug1, t_ub1) +
						 Variance( uTotalWeight2, tt_sum2, t_ur2, t_ug2, t_ub2);

		// found better split point? if so, remember it

		if (total_variance < variance_g)
		{
			variance_g = total_variance;
			pick_g = i;
		}
	}

	// left box's initial statistics

	uTotalWeight1         = 0;
	tt_sum1               = 0;
	t_ur1                 = 0;
	t_ug1                 = 0;
	t_ub1                 = 0;

	// right box's initial statistics

	uTotalWeight2         = uTotalWeight;
	tt_sum2               = tt_sum;
	t_ur2                 = t_ur;
	t_ug2                 = t_ug;
	t_ub2                 = t_ub;

	variance_b = 0xFFFFFFFF;

	// locate optimum split point on blue axis

	for (i = ib; i < jb; i++)
	{
		UINT total_variance;

		// calc the statistics for the area being taken
		// away from the right box and given to the left box

		Sum( ir, ig, i, jr, jg, i,
				&uTotalWeight3, &tt_sum3, &t_ur3, &t_ug3, &t_ub3);

		if (uTotalWeight3 > uTotalWeight)
			ASSERT( FALSE);

		// update left and right box's statistics

		uTotalWeight1 += uTotalWeight3;
		tt_sum1       += tt_sum3;
		t_ur1         += t_ur3;
		t_ug1         += t_ug3;
		t_ub1         += t_ub3;

		uTotalWeight2 -= uTotalWeight3;
		tt_sum2       -= tt_sum3;
		t_ur2         -= t_ur3;
		t_ug2         -= t_ug3;
		t_ub2         -= t_ub3;

		if ((uTotalWeight1 + uTotalWeight2) != uTotalWeight)
			ASSERT( FALSE);

		// calculate left and right box's overall variance

		total_variance = Variance( uTotalWeight1, tt_sum1, t_ur1, t_ug1, t_ub1) +
						 Variance( uTotalWeight2, tt_sum2, t_ur2, t_ug2, t_ub2);

		// found better split point? if so, remember it

		if (total_variance < variance_b)
		{
			variance_b = total_variance;
			pick_b = i;
		}
	}

	// now find out which axis should be split (r, g, or b)

	lowest_variance = variance_r;
	i = 0;

	if (variance_g < lowest_variance)
	{
		lowest_variance = variance_g;
		i = 1;
	}

	if (variance_b < lowest_variance)
	{
		lowest_variance = variance_b;
		i = 2;
	}

	// split box on the selected axis

	ir1 = ir; ig1 = ig; ib1 = ib;
	jr2 = jr; jg2 = jg; jb2 = jb;

	switch (i)
	{
	case 0:
		jr1 = pick_r + 0; jg1 = jg; jb1 = jb;
		ir2 = pick_r + 1; ig2 = ig; ib2 = ib;
		break;

	case 1:
		jr1 = jr; jg1 = pick_g + 0; jb1 = jb;
		ir2 = ir; ig2 = pick_g + 1; ib2 = ib;
		break;

	case 2:
		jr1 = jr; jg1 = jg; jb1 = pick_b + 0;
		ir2 = ir; ig2 = ig; ib2 = pick_b + 1;
		break;

	default:
		ASSERT( FALSE);
	}

	// shrink the new boxes to their minimum possible sizes

	ShrinkBox( ir1, ig1, ib1, jr1, jg1, jb1,
			&ir1, &ig1, &ib1, &jr1, &jg1, &jb1);

	ShrinkBox( ir2, ig2, ib2, jr2, jg2, jb2,
			&ir2, &ig2, &ib2, &jr2, &jg2, &jb2);

	// update statistics

	Sum( ir1, ig1, ib1, jr1, jg1, jb1,
			&uTotalWeight1, &tt_sum1, &t_ur1, &t_ug1, &t_ub1);

	uTotalWeight2         = uTotalWeight - uTotalWeight1;
	tt_sum2               = tt_sum - tt_sum1;
	t_ur2                 = t_ur - t_ur1;
	t_ug2                 = t_ug - t_ug1;
	t_ub2                 = t_ub - t_ub1;

	// create the new boxes

	pOldBox->uVariance    = Variance( uTotalWeight1, tt_sum1, t_ur1, t_ug1, t_ub1);
	pOldBox->uTotalWeight = uTotalWeight1;
	pOldBox->tt_sum       = tt_sum1;
	pOldBox->t_ur         = t_ur1;
	pOldBox->t_ug         = t_ug1;
	pOldBox->t_ub         = t_ub1;
	pOldBox->ir           = ir1;
	pOldBox->ig           = ig1;
	pOldBox->ib           = ib1;
	pOldBox->jr           = jr1;
	pOldBox->jg           = jg1;
	pOldBox->jb           = jb1;

	pNewBox->uVariance    = Variance( uTotalWeight2, tt_sum2, t_ur2, t_ug2, t_ub2);
	pNewBox->uTotalWeight = uTotalWeight2;
	pNewBox->tt_sum       = tt_sum2;
	pNewBox->t_ur         = t_ur2;
	pNewBox->t_ug         = t_ug2;
	pNewBox->t_ub         = t_ub2;
	pNewBox->ir           = ir2;
	pNewBox->ig           = ig2;
	pNewBox->ib           = ib2;
	pNewBox->jr           = jr2;
	pNewBox->jg           = jg2;
	pNewBox->jb           = jb2;

	// enter all splittable boxes into the priory queue

	i = 0;
	if ((jr1 - ir1) + (jg1 - ig1) + (jb1 - ib1))
		i = 2;
	if ((jr2 - ir2) + (jg2 - ig2) + (jb2 - ib2))
		i++;

	switch (i)
	{
	case 0:
		pHeap[1] = pHeap[uHeapSize];
		uHeapSize--;

		if (uHeapSize)
			down_heap();
		break;

	case 1:
		pHeap[1] = pNewBox;
		down_heap();
		break;

	case 2:
		down_heap();
		break;

	case 3:
		down_heap();
		insert_heap( pNewBox);
		break;

	default:
		ASSERT( FALSE);
		break;
	}
}

/*----------------------------------------------------------------------------*/
/* Creates new colormap.                                                      */
/*----------------------------------------------------------------------------*/
void CColorOptimizer::MakeColorMap( BYTE* pbyColorMap)
{
	int i, j, k, m, r, g, b;
	box* p;
	UINT uTotalWeight;

	p = pBoxes;

	for (i = 0; i < iNumBoxes; i++, p++)
	{
		k = 3 * i;
		uTotalWeight = p->uTotalWeight;

		if (uTotalWeight)
		{
			pbyColorMap[k]  = (BYTE)(((p->t_ur << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
			pbyColorMap[k] += pbyColorMap[k] >> HIST_BIT;		// neal - round up

			pbyColorMap[k+1] = (BYTE)(((p->t_ug << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
			pbyColorMap[k+1] += pbyColorMap[k+1] >> HIST_BIT;		// neal - round up

			pbyColorMap[k+2]  = (BYTE)(((p->t_ub << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
			pbyColorMap[k+2] += pbyColorMap[k+2] >> HIST_BIT;		// neal - round up
		}
		else
			pbyColorMap[k] = pbyColorMap[k+1] = pbyColorMap[k+2] = 0;
	}

	// neal - sort palette by brightness (for neatness sake)
	BOOL bStillSwapping = FALSE;

	do
	{
		bStillSwapping = FALSE;

		for (i = 0; i < iNumBoxes-1; i++)
		{
			k = 3 * i;
			r = pbyColorMap[k];
			g = pbyColorMap[k+1];
			b = pbyColorMap[k+2];
			int iGrayI = (r*30 + g*59 + b*11) / 100;

			for (j = i+1; j < iNumBoxes; j++)
			{
				m = 3 * j;
				r = pbyColorMap[m];
				g = pbyColorMap[m+1];
				b = pbyColorMap[m+2];
				int iGrayJ = (r*30 + g*59 + b*11) / 100;

				if (iGrayJ < iGrayI)	// swap 'em?
				{
					bStillSwapping = TRUE;

					int t = pbyColorMap[k];
					pbyColorMap[k] = pbyColorMap[m];
					pbyColorMap[m] = t;

					t = pbyColorMap[k+1];
					pbyColorMap[k+1] = pbyColorMap[m+1];
					pbyColorMap[m+1] = t;

					t = pbyColorMap[k+2];
					pbyColorMap[k+2] = pbyColorMap[m+2];
					pbyColorMap[m+2] = t;
				}
			}
		}
	} while (bStillSwapping);
}

//////////////////////////////////////////////////////////////////////////////
// Name:	Initialize
// Action:	Create initial box, initialize heap
//
// Returns:	TRUE for success, FALSE for failure
//////////////////////////////////////////////////////////////////////////////
BOOL CColorOptimizer::Initialize( int colors)
{
	UINT uTotalWeight;
	UINT tt_sum, t_ur, t_ug, t_ub;
	int ir, ig, ib, jr, jg, jb;

	if ((pHeap = (box **)malloc(sizeof(box *) * (colors + 1))) == NULL)
		return FALSE;

	if ((pBoxes = (box *)malloc(sizeof(box) * colors)) == NULL)
		return FALSE;

	/* shrink initial box to minimum possible size */
	ShrinkBox(0, 0, 0, HIST_MAX - 1, HIST_MAX - 1, HIST_MAX - 1, &ir, &ig, &ib, &jr, &jg, &jb);

	/* calculate the initial box's statistics */
	Sum( ir, ig, ib, jr, jg, jb, &uTotalWeight, &tt_sum, &t_ur, &t_ug, &t_ub);

	pBoxes->uTotalWeight = uTotalWeight;
	pBoxes->uVariance    = 1;
	pBoxes->tt_sum       = tt_sum;
	pBoxes->t_ur         = t_ur;
	pBoxes->t_ug         = t_ug;
	pBoxes->t_ub         = t_ub;
	pBoxes->ir           = ir;
	pBoxes->ig           = ig;
	pBoxes->ib           = ib;
	pBoxes->jr           = jr;
	pBoxes->jg           = jg;
	pBoxes->jb           = jb;

	/* enter box into heap if it's splittable */

	iNumBoxes           = 1;
	uHeapSize           = 0;

	if ((jr - ir) + (jg - ig) + (jb - ib))
	{
		pHeap[1] = pBoxes;
		uHeapSize = 1;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Name:	Quantize
// Action:	Creates a quantized palette with iMaxColors color indexes
//
// Returns:	TRUE for success, FALSE for failure
//			piNumColors will hold the number of colors actually used
//////////////////////////////////////////////////////////////////////////////
BOOL CColorOptimizer::Quantize( UINT* uHistogram, int iMaxColors, BYTE* pbyPackedPalette, int* piNumColors)
{
	BOOL bStatus = FALSE;
	box *p;

	pHeap  = NULL;
	pBoxes = NULL;

	puHist = uHistogram;

	bStatus = Initialize( iMaxColors);
	if (! bStatus)
	{
		*piNumColors = 0;
		goto ReduceError;
	}
	while (iNumBoxes < iMaxColors)
	{
		if ((p = worst_box()) == NULL)
			 break;

		SplitBox( p);
	}

	MakeColorMap( pbyPackedPalette);

	*piNumColors = iNumBoxes;

ReduceError:

	free( pHeap);
	free( pBoxes);

	return bStatus;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if BETTER_METHOD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
CBetterBox::CBetterBox()
{
	Init();
}

void CBetterBox::Init( void)
{
	m_uVariance    = 1;
	m_uTotalWeight = 0;		// total weight
	m_uSum = 0;				// tt_sum += r*r+g*g+b*b*weight over entire box
	m_uR   = 0;				// t_ur += r*weight over entire box
	m_uG   = 0;				// t_ug += g*weight over entire box
	m_uB   = 0;				// t_ub += b*weight over entire box

	m_uUpperR = 0;			// upper and lower bounds
	m_uUpperG = 0;
	m_uUpperB = 0;
	m_uLowerR = 0;
	m_uLowerG = 0;
	m_uLowerB = 0;

	m_uNodeIndex = NIL_NODE;
	m_pColorOpt  = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Name:	ShrinkBox
// Action:	Shrinks box to minimum possible size.
//////////////////////////////////////////////////////////////////////////////
void CBetterBox::BetterShrinkBox( void)
{
	// now includes CalcSum code, too

	m_uTotalWeight     = 0;
	m_uSum             = 0;
	m_uR = m_uG = m_uB = 0;

	UINT uNodeIndex = GetHead();
	ASSERT( uNodeIndex != NIL_NODE);
	CColorNode* pNode = GetNode( uNodeIndex);

	UINT uNumNodes = 0;

	if (pNode)
	{
		COLORREF rgb = pNode->GetRGB();
		UINT     r   = GetRValue( rgb);
		UINT     g   = GetGValue( rgb);
		UINT     b   = GetBValue( rgb);

		m_uUpperR = m_uLowerR = r;
		m_uUpperG = m_uLowerG = g;
		m_uUpperB = m_uLowerB = b;

		// scan all the nodes in this box

		while (uNodeIndex != NIL_NODE)
		{
			rgb = pNode->GetRGB();
			r   = GetRValue( rgb);
			g   = GetGValue( rgb);
			b   = GetBValue( rgb);

			m_uUpperR = max( r, m_uUpperR);
			m_uUpperG = max( g, m_uUpperG);
			m_uUpperB = max( b, m_uUpperB);

			m_uLowerR = min( r, m_uLowerR);
			m_uLowerG = min( g, m_uLowerG);
			m_uLowerB = min( b, m_uLowerB);

			UINT uWeight = pNode->GetCount();

			m_uR += r * uWeight;
			m_uG += g * uWeight;
			m_uB += b * uWeight;

			m_uSum         += (r*r + g*g + b*b) * uWeight;
			m_uTotalWeight += uWeight;

			uNodeIndex = GetNextIndex( uNodeIndex);
			pNode      = GetNode( uNodeIndex);
			uNumNodes++;
		}
	}
	if (uNumNodes == 1)
		m_uVariance = 0;
}

void CBetterBox::BetterCalcSum( void)
{
	m_uTotalWeight     = 0;
	m_uSum             = 0;
	m_uR = m_uG = m_uB = 0;

	UINT uNodeIndex = GetHead();
	ASSERT( uNodeIndex != NIL_NODE);

	// scan all the nodes in this box

	while (uNodeIndex != NIL_NODE)
	{
		CColorNode* pNode = GetNode( uNodeIndex);

		COLORREF rgb = pNode->GetRGB();
		UINT     r   = GetRValue( rgb);

		if ((r >= m_uLowerR) && (r <= m_uUpperR))
		{
			UINT g = GetGValue( rgb);

			if ((g >= m_uLowerG) && (g <= m_uUpperG))
			{
				UINT b = GetBValue( rgb);

				if ((b >= m_uLowerB) && (b <= m_uUpperB))
				{
					UINT uWeight = pNode->GetCount();

					m_uR += r * uWeight;
					m_uG += g * uWeight;
					m_uB += b * uWeight;

					m_uSum         += (r*r + g*g + b*b) * uWeight;
					m_uTotalWeight += uWeight;
				}
			}
		}
		uNodeIndex = GetNextIndex( uNodeIndex);
	}
}

// locate optimum split on R, G, or B axis
UINT CBetterBox::FindBestSplit( int iSplit, UINT* uVariance) const
{
	UINT       uPick = -1;
	CBetterBox boxLeft, boxRight, boxSlice;

	boxRight   = *this;
	*uVariance = 0xFFFFFFFF;

	UINT  uLower  = 0;
	UINT  uUpper  = 0;
	UINT* puLower = NULL;
	UINT* puUpper = NULL;

	switch (iSplit)
	{
	case SPLIT_RED:
		uLower = m_uLowerR;
		uUpper = m_uUpperR;
		puLower = &boxSlice.m_uLowerR;
		puUpper = &boxSlice.m_uUpperR;
		break;

	case SPLIT_GREEN:
		uLower = m_uLowerG;
		uUpper = m_uUpperG;
		puLower = &boxSlice.m_uLowerG;
		puUpper = &boxSlice.m_uUpperG;
		break;

	case SPLIT_BLUE:
		uLower = m_uLowerB;
		uUpper = m_uUpperB;
		puLower = &boxSlice.m_uLowerB;
		puUpper = &boxSlice.m_uUpperB;
		break;

	default:
		ASSERT( FALSE);
		break;
	}

	for (UINT i = uLower; i <= uUpper; i++)
	{
		UINT uTotalVariance;

		// calc the statistics for the "slice" (which is the area
		// being removed from the right box and added to the left one

		boxSlice = boxRight;
/*
		switch (iSplit)
		{
		case SPLIT_RED:
			boxSlice.m_uLowerR = boxSlice.m_uUpperR = i;
			break;

		case SPLIT_GREEN:
			boxSlice.m_uLowerG = boxSlice.m_uUpperG = i;
			break;

		case SPLIT_BLUE:
			boxSlice.m_uLowerB = boxSlice.m_uUpperB = i;
			break;
		}
*/
		// neal - speed up (skips switch statement)
		*puLower = i;
		*puUpper = i;

		boxSlice.BetterCalcSum();

		ASSERT( boxSlice.m_uTotalWeight <= m_uTotalWeight);

		// update left and right boxes
		boxLeft  = boxLeft  + boxSlice;
		boxRight = boxRight - boxSlice;

		ASSERT( (boxLeft.m_uTotalWeight + boxRight.m_uTotalWeight) == m_uTotalWeight);

		uTotalVariance = Variance( boxLeft.m_uTotalWeight, boxLeft.m_uSum, 
				boxLeft.m_uR, boxLeft.m_uG, boxLeft.m_uB) + 
				Variance( boxRight.m_uTotalWeight, boxRight.m_uSum, 
				boxRight.m_uR, boxRight.m_uG, boxRight.m_uB);

		// did we find a better split?  if so, save it

		if (uTotalVariance < *uVariance)
		{
			*uVariance = uTotalVariance;
			uPick = i;
		}
	}

	ASSERT( (uPick >= 0) && (uPick <= 255));
	return uPick;
}

void inline CBetterBox::AddNode( UINT uNodeIndex)
{
	if (uNodeIndex != NIL_NODE)
	{
		UINT uHeadIndex = m_uNodeIndex;

		GetNode( uNodeIndex)->SetNextIndex( uHeadIndex);

		m_uNodeIndex = uNodeIndex;
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CBetterBox::BetterSplitBox( void)
{
	UINT uVarianceR, uVarianceG, uVarianceB;

	UINT uPickR = FindBestSplit( SPLIT_RED,   &uVarianceR);
	UINT uPickG = FindBestSplit( SPLIT_GREEN, &uVarianceG);
	UINT uPickB = FindBestSplit( SPLIT_BLUE,  &uVarianceB);

	int  iBestSplit      = SPLIT_RED;
	UINT uLowestVariance = uVarianceR;

	if (uVarianceG < uLowestVariance)
	{
		uLowestVariance = uVarianceG;
		iBestSplit      = SPLIT_GREEN;
	}
	if (uVarianceB < uLowestVariance)
	{
		uLowestVariance = uVarianceB;
		iBestSplit      = SPLIT_BLUE;
	}

	// fill in new box

	CBetterBox* pBoxNew = &m_pColorOpt->m_pBetterBoxes[m_pColorOpt->m_iNumBoxes];
	m_pColorOpt->m_iNumBoxes++;

	*pBoxNew = *this;

	switch (iBestSplit)
	{
	case SPLIT_RED:
		pBoxNew->m_uUpperR = uPickR;
		m_uLowerR          = uPickR+1;

		ASSERT( (pBoxNew->m_uUpperR >= pBoxNew->m_uLowerR) && (m_uUpperR >= m_uLowerR));
		break;

	case SPLIT_GREEN:
		pBoxNew->m_uUpperG = uPickG;
		m_uLowerG          = uPickG+1;

		ASSERT( (pBoxNew->m_uUpperG >= pBoxNew->m_uLowerG) && (m_uUpperG >= m_uLowerG));
		break;

	case SPLIT_BLUE:
		pBoxNew->m_uUpperB = uPickB;
		m_uLowerB          = uPickB+1;

		ASSERT( (pBoxNew->m_uUpperB >= pBoxNew->m_uLowerB) && (m_uUpperB >= m_uLowerB));
		break;

	default:
		ASSERT( FALSE);
		break;
	}

	// split the node list between the current box and the new one

	UINT uList            = m_uNodeIndex;
	m_uNodeIndex          = NIL_NODE;
	pBoxNew->m_uNodeIndex = NIL_NODE;

	while (uList != NIL_NODE)
	{
		UINT uNext = GetNextIndex( uList);

		COLORREF rgb = GetNode( uList)->GetRGB();
		UINT     r   = GetRValue( rgb);
		UINT     g   = GetGValue( rgb);
		UINT     b   = GetBValue( rgb);

		if ((r >= m_uLowerR) && (r <= m_uUpperR) && (g >= m_uLowerG) &&
				(g <= m_uUpperG) && (b >= m_uLowerB) && (b <= m_uUpperB))
		{
			AddNode( uList);
		}
		else
		{
			ASSERT( (r >= pBoxNew->m_uLowerR) && (r <= pBoxNew->m_uUpperR) &&
					(g >= pBoxNew->m_uLowerG) && (g <= pBoxNew->m_uUpperG) && 
					(b >= pBoxNew->m_uLowerB) && (b <= pBoxNew->m_uUpperB));

			pBoxNew->AddNode( uList);
		}
		uList = uNext;
	}

	// both boxes should have some items in their lists
	ASSERT( (m_uNodeIndex != NIL_NODE) && (pBoxNew->m_uNodeIndex != NIL_NODE));

	// shrink the new boxes to their minimum possible sizes

	BetterShrinkBox();
	pBoxNew->BetterShrinkBox();

	// update statistics

	m_uVariance          = Variance( m_uTotalWeight, m_uSum, m_uR, m_uG, m_uB);
	pBoxNew->m_uVariance = Variance( pBoxNew->m_uTotalWeight, pBoxNew->m_uSum, 
				pBoxNew->m_uR, pBoxNew->m_uG, pBoxNew->m_uB);
}

//////////////////////////////////////////////////////////////////////////////
// Name:	Initialize
// Action:	Create initial box, initialize heap
//
// Returns:	TRUE for success, FALSE for failure
//////////////////////////////////////////////////////////////////////////////
BOOL CColorOptimizer::BetterInitialize( int iNumColors)
{
	m_pBetterBoxes = NULL;

	m_pBetterBoxes = (CBetterBox *)malloc( sizeof( CBetterBox) * iNumColors);
	if (m_pBetterBoxes == NULL)
		return FALSE;

	for (int i = 0; i < iNumColors; i++)
	{
		m_pBetterBoxes[i].Init();
		m_pBetterBoxes[i].m_pColorOpt = this;
	}

	m_iNumBoxes                    = 1;
	m_pBetterBoxes[0].m_uNodeIndex = 0;

	// shrink initial box to minimum possible size

	m_pBetterBoxes[0].BetterShrinkBox();

	return TRUE;
}

/*----------------------------------------------------------------------------*/
/* Returns "worst" box, or NULL if no more splittable boxes remain. The worst */
/* box is the box with the largest variance.                                  */
/*----------------------------------------------------------------------------*/
CBetterBox* CColorOptimizer::BetterWorstBox(void)
{
	if (m_iNumBoxes != 0)
	{
		CBetterBox* pBox = &m_pBetterBoxes[0];
		UINT        uVar = pBox->m_uVariance;

		for (int i = 1; i < m_iNumBoxes; i++)
		{
			if (m_pBetterBoxes[i].m_uVariance > uVar)
			{
				pBox = &m_pBetterBoxes[i];
				uVar = pBox->m_uVariance;
			}
		}
		if (uVar)
			return pBox;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*/
/* Creates new colormap.                                                      */
/*----------------------------------------------------------------------------*/
void CColorOptimizer::BetterMakeColorMap( BYTE* pbyColorMap)
{
	int i, j, k, m;
	CBetterBox* pBox;
	UINT uTotalWeight;

	pBox = m_pBetterBoxes;

	for (i = 0; i < m_iNumBoxes; i++)
	{
		k = 3 * i;
		uTotalWeight = pBox->m_uTotalWeight;

		// neal - don't delete!!! - used for validity checking

//		pbyColorMap[k]  = (BYTE)(((pBox->m_uR << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
//		pbyColorMap[k] += pbyColorMap[k] >> HIST_BIT;		// neal - round up
//
//		pbyColorMap[k+1] = (BYTE)(((pBox->m_uG << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
//		pbyColorMap[k+1] += pbyColorMap[k+1] >> HIST_BIT;		// neal - round up
//
//		pbyColorMap[k+2]  = (BYTE)(((pBox->m_uB << HIST_SHIFT) + (uTotalWeight >> 1)) / uTotalWeight);
//		pbyColorMap[k+2] += pbyColorMap[k+2] >> HIST_BIT;		// neal - round up

		if (uTotalWeight)
		{
			pbyColorMap[k]   = (BYTE)(((pBox->m_uR) + (uTotalWeight >> 1)) / uTotalWeight);
			pbyColorMap[k+1] = (BYTE)(((pBox->m_uG) + (uTotalWeight >> 1)) / uTotalWeight);
			pbyColorMap[k+2] = (BYTE)(((pBox->m_uB) + (uTotalWeight >> 1)) / uTotalWeight);
		}
		else
		{
			pbyColorMap[k] = pbyColorMap[k+1] = pbyColorMap[k+2] = 0;
		}

		pBox++;
	}

	////////////////////////////////////////////////////////////
	// neal - sort palette by brightness (for neatness sake) //
	//////////////////////////////////////////////////////////

	BOOL bStillSwapping = FALSE;

	do
	{
		bStillSwapping = FALSE;

		for (i = 0; i < m_iNumBoxes-1; i++)
		{
			k = 3 * i;
			int r = pbyColorMap[k];
			int g = pbyColorMap[k+1];
			int b = pbyColorMap[k+2];
			int iGrayI = (r*30 + g*59 + b*11) / 100;

			for (j = i+1; j < m_iNumBoxes; j++)
			{
				m = 3 * j;
				r = pbyColorMap[m];
				g = pbyColorMap[m+1];
				b = pbyColorMap[m+2];
				int iGrayJ = (r*30 + g*59 + b*11) / 100;

				if (iGrayJ < iGrayI)	// swap 'em?
				{
					bStillSwapping = TRUE;

					int t = pbyColorMap[k];
					pbyColorMap[k] = pbyColorMap[m];
					pbyColorMap[m] = t;

					t = pbyColorMap[k+1];
					pbyColorMap[k+1] = pbyColorMap[m+1];
					pbyColorMap[m+1] = t;

					t = pbyColorMap[k+2];
					pbyColorMap[k+2] = pbyColorMap[m+2];
					pbyColorMap[m+2] = t;
				}
			}
		}
	} while (bStillSwapping);
}

//////////////////////////////////////////////////////////////////////////////
// Name:	Quantize
// Action:	Creates a quantized palette with iMaxColors color indexes
//
// Returns:	TRUE for success, FALSE for failure
//			piNumColors will hold the number of colors actually used
//////////////////////////////////////////////////////////////////////////////
BOOL CColorOptimizer::BetterQuantize( CColorNode* pNodeList, 
		int iMaxColors, BYTE* pbyPackedPalette, int* piNumColors,
		CProgressBar* pctlProgress)
{
	if (iMaxColors <= 0)
	{
		ASSERT( FALSE);
		iMaxColors = 1;
	}

	CBetterBox* pBox    = NULL;
	BOOL        bStatus = BetterInitialize( iMaxColors);

	ZeroMemory( pbyPackedPalette, 256*3);

	if (! bStatus)
	{
		*piNumColors = 0;
		goto ReduceError;
	}

	while (m_iNumBoxes < iMaxColors)
	{
		pBox = BetterWorstBox();

		if (pBox == NULL)
			 break;

		pBox->BetterSplitBox();

		// building boxes accounts for 80%

		if (pctlProgress)
			pctlProgress->SetPos( 20 + ((m_iNumBoxes * 80) / iMaxColors));
	}

	BetterMakeColorMap( pbyPackedPalette);

	*piNumColors = m_iNumBoxes;

ReduceError:

	if (pctlProgress)
		pctlProgress->SetPos( 100);

	return bStatus;
}

#endif