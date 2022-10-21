/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// Layer.cpp : Helper class - represents a single bitmap layer
//             To be used for tools, cut-outs, selections, etc.
//
// Created by Neal White III, 2-21-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Layer.h"
#include "WallyDoc.h"
#include "ClipboardDIB.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CLayer::CLayer()
{
	m_pIRGB_Data           = NULL;
	m_pbyAlphaChannel_Data = NULL;

	Init();
}

CLayer::~CLayer()
{
	FreeMem();
}

void CLayer::Init()
{
	m_pDoc        = NULL;
	m_iNumBits    = 8;	// 8 or 24
	m_iWidth      = 0;
	m_iHeight     = 0;
	m_iFadeAmount = 0;

//	m_ptOffset.x  = 0;
//	m_ptOffset.y  = 0;

	FreeMem();
}

void CLayer::FreeMem()
{
	if (m_pIRGB_Data)
	{
		delete [] m_pIRGB_Data;
		m_pIRGB_Data = NULL;
	}
	if (m_pbyAlphaChannel_Data)
	{
		delete [] m_pbyAlphaChannel_Data;
		m_pbyAlphaChannel_Data = NULL;
	}
}

void CLayer::Clear( CWallyView* pWallyView, COLOR_IRGB irgbColor)
{
	for (int j = 0; j < m_iHeight; j++)
	{
		for (int i = 0; i < m_iWidth; i++)
		{
			SetPixel( pWallyView, i, j, irgbColor);
		}
	}
}

void CLayer::CopyLayer( CWallyView* pWallyView, CLayer* pLayer)
{
	ASSERT( pLayer != NULL);
	ASSERT( pLayer->GetWidth()   == GetWidth());
	ASSERT( pLayer->GetHeight()  == GetHeight());
	ASSERT( pLayer->GetNumBits() == GetNumBits());
	ASSERT( pLayer->HasData() && HasData());

	ASSERT( (GetNumBits() == 8) || (GetNumBits() == 24) || (GetNumBits() == 32));

	for (int j = 0; j < m_iHeight; j++)
	{
		for (int i = 0; i < m_iWidth; i++)
		{
			SetPixel( pWallyView, i, j, pLayer->GetPixel( i, j));
		}
	}
}

void CLayer::DupLayer( CWallyView* pWallyView, CLayer* pLayer)
{
	ASSERT( pLayer != NULL);

	if (pLayer != NULL)
	{
		m_pDoc        = pLayer->m_pDoc;
		m_iNumBits    = pLayer->m_iNumBits;
		m_iWidth      = pLayer->m_iWidth;
		m_iHeight     = pLayer->m_iHeight;
		m_iFadeAmount = pLayer->m_iFadeAmount;

//		m_ptOffset.x  = pLayer->m_ptOffset.x;
//		m_ptOffset.y  = pLayer->m_ptOffset.y;

		Create();

		if (pLayer->HasData())
		{
			CopyLayer( pWallyView, pLayer);
		}
		else
		{
			ASSERT( FALSE);		// duping an empty layer ???

			Clear( pWallyView, COLOR_DEBUG_PURPLE);  // a highly visible color
		}
	}
}


void CLayer::GetClipBounds( CRect* pRect, int iIndex)
{
	if (pRect == NULL)
	{
		ASSERT( FALSE);
		return;
	}

	pRect->SetRect( 1048576, 1048576, -1, -1);

	int    iWidth  = GetWidth();
	int    iHeight = GetHeight();

	int iStripStart = 0;
	int iStripWidth = iWidth;

	COLOR_IRGB* pData = NULL;

	if (HasData())
	{
		pData   = GetData();
		iWidth  = GetWidth();
		iHeight = GetHeight();

		iStripWidth = iWidth;

		// is it a horizontal strip?  (select one randomly)
		if ((iWidth > iHeight) && ((iWidth % iHeight) == 0))
		{
//			iStripStart = (int )(Random() * (iWidth / iHeight));
			iStripStart = iIndex;		// measure this item in strip

			if (iStripStart >= (iWidth / iHeight))
				iStripStart = 0;		// error - measure first item
			else if (iStripStart < 0)
				iStripStart = 0;		// error - measure first item

			iWidth = iHeight;

			iStripStart *= iWidth;
		}
	}
	else
	{
		//ASSERT( FALSE);
		pRect->SetRect( 0, 0, 0, 0);
		return;
	}

	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidth; i++)
		{
			if (pData)
			{
				COLOR_IRGB irgb = pData[j*iStripWidth + i+iStripStart];
				int iDecalG = GetGValue( irgb);

				if (iDecalG == 255)		// invisble
				{
					continue;
				}
				// set the bounds

				pRect->top    = min( pRect->top,    j);
				pRect->left   = min( pRect->left,   i);
				pRect->right  = max( pRect->right,  i);
				pRect->bottom = max( pRect->bottom, j);
			}
			else
			{
				ASSERT( FALSE);
				pRect->SetRect( 0, 0, 0, 0);
			}
		}
	}
	if (pRect->right < 0)
		pRect->SetRect( 0, 0, 0, 0);
}

int CLayer::GetClippedWidth( int iIndex)
{
	CRect rBounds;
	GetClipBounds( &rBounds, iIndex);

	return (rBounds.Width());
}

int CLayer::GetClippedHeight( int iIndex)
{
	CRect rBounds;
	GetClipBounds( &rBounds, iIndex);

	return (rBounds.Height());
}

void CLayer::Serialize( CArchive& ar, BOOL bSavePalette)
{
	WORD wVer = 1;
	BOOL bPaletteSaved = FALSE;

	if (ar.IsStoring())
	{
		if (! HasData())
		{
			wVer = 0;
			ar << wVer;
		}
		else
		{
			ar << wVer;
			ar << m_iNumBits;	// 8 or 24
			ar << m_iWidth;
			ar << m_iHeight;
			ar << m_iFadeAmount;

			ar << m_rBoundsRect;
//			ar << m_ptOffset.x;
//			ar << m_ptOffset.y;

			if (bSavePalette)
			{
				bPaletteSaved = TRUE;

				ar << bPaletteSaved;
				m_Pal.Serialize( ar);
			}
			else
			{
				ar << bPaletteSaved;
			}

			ar.Write( m_pIRGB_Data, m_iWidth * m_iHeight * sizeof( COLOR_IRGB));
		}
	}
	else	// loading
	{
		ar >> wVer;

		if (wVer != 0)
		{
			int iOldWidth  = m_iWidth;
			int iOldHeight = m_iHeight;

			ar >> m_iNumBits;	// 8 or 24
			ar >> m_iWidth;
			ar >> m_iHeight;
			ar >> m_iFadeAmount;

			ar >> m_rBoundsRect;
//			ar >> m_ptOffset.x;
//			ar >> m_ptOffset.y;

			ar >> bPaletteSaved;

			if (bPaletteSaved)
			{
				m_Pal.Serialize( ar);
			}

			TRY
			{
				// Neal - speed up - only do a new if size changed

				//ASSERT( m_pIRGB_Data == NULL);

				if ((m_pIRGB_Data == NULL) || (iOldWidth != m_iWidth) || 
						(iOldHeight != m_iHeight))
				{
					FreeMem();
					m_pIRGB_Data = new COLOR_IRGB[m_iWidth*m_iHeight];
				}

				// Neal - TODO: serialize alpha channel
			}
			CATCH_ALL( e)
			{
				AfxMessageBox( IDS_OUT_OF_MEM);
				return;
			}
			END_CATCH_ALL

			ar.Read( m_pIRGB_Data, m_iWidth * m_iHeight * sizeof( COLOR_IRGB));
		}
	}
}

BOOL CLayer::LoadFromDibSection( CDibSection *pDIB)
{
	ASSERT (pDIB);

	UINT iWidth		= pDIB->GetWidth();
	UINT iHeight	= pDIB->GetHeight();
	UINT iNumBits	= pDIB->GetBitCount();
	BYTE *pbyData	= NULL;

	SetWidth( iWidth);
	SetHeight( iHeight);
	SetNumBits( iNumBits);

	UINT iX = 0;
	UINT iY = 0;
	UINT iOffset = 0;
		
	TRY
	{
		FreeMem();
		Create();
	}
	CATCH_ALL( e)
	{
		AfxMessageBox( IDS_OUT_OF_MEM);
		return FALSE;
	}
	END_CATCH_ALL

	switch (iNumBits)
	{
	case 8:
		{
			GetPalette()->SetPalette( pDIB->GetPalette(), 256);
		}
		break;

	default:		
		break;
	}

	for (iY = 0; iY < iHeight; iY++)
	{				
		for (iX = 0; iX < iWidth; iX++)
		{
			iOffset = iY * iWidth + iX;

			m_pIRGB_Data[iOffset] = pDIB->GetPixelAtXY( iX, iY);
		}
	}
	
	return TRUE;
}

BOOL CLayer::LoadFromClipboard( BOOL bAllow24Bit, CWnd* pWnd)
{
	CWaitCursor   WaitCursor;
	CClipboardDIB ClipboardDIB;

	if (ClipboardDIB.InitFromClipboard( pWnd))
	{
		BYTE* pbyClipboardData;
		int i, j, r, g, b;
		BYTE ClipboardPalette[1024];		

		m_iWidth   = ClipboardDIB.GetWidth();
		m_iHeight  = ClipboardDIB.GetHeight();
		//int iSize  = m_iWidth * m_iHeight;

		if ((m_iWidth > MAX_MIP_SIZE) || (m_iHeight > MAX_MIP_SIZE))
		{
			AfxMessageBox( "Paste image too big -- CLayer has a 2048 x 2048 maximum size.");
			return FALSE;
		}

		m_iNumBits = ClipboardDIB.GetColorDepth();
		
		TRY
		{
			FreeMem();
			Create();
		}
		CATCH_ALL( e)
		{
			AfxMessageBox( IDS_OUT_OF_MEM);
			return FALSE;
		}
		END_CATCH_ALL

		// Point to the clipboard bits

		pbyClipboardData = (BYTE *)ClipboardDIB;

		ClipboardDIB.GetDIBPalette( ClipboardPalette);
	
		if (m_iNumBits == 8)
		{
			// set our palette to match clipboard

			int iNumColors = ClipboardDIB.GetColorsUsed();

			for (j = 0; j < iNumColors; j++)
			{
				b = ClipboardPalette[j * 4];
				g = ClipboardPalette[j * 4 + 1];
				r = ClipboardPalette[j * 4 + 2];

				m_Pal.SetRGB( j, r, g, b);
			}

			// now copy the data from the clipboard

			for (j = 0; j < m_iHeight; j++)
			{
				int iDataLine = PadDWORD( j * m_iWidth);

				for (i = 0; i < m_iWidth; i++)
				{
					int iIndex = pbyClipboardData[iDataLine+i];

					r = m_Pal.GetR( iIndex);
					g = m_Pal.GetG( iIndex);
					b = m_Pal.GetB( iIndex);

					m_pIRGB_Data[j*m_iWidth+i] = IRGB( iIndex, r, g, b);
				}
			}

			return TRUE;
		}		
		else if (m_iNumBits == 24)
		{
			if (bAllow24Bit)
			{
				AfxMessageBox( "Not Yet Implemented: Paste into CLayer does not support 24 bit.\n"
						"Try converting your source to 256 colors and repeat.");
			}
			else
			{
				AfxMessageBox( "This operation does not support 24 bit data.");
			}
		}
		else
		{
			ASSERT( FALSE);
		}
	}
	return FALSE;
}

BOOL CLayer::GetAppliedDecalPixelRGB( CWallyDoc* pDoc, 
		CWallyPalette* pPal, COLORREF* pRGB, 
		int x, int y, int iPercent, DWORD dwFlags)
{
	
	ASSERT( (iPercent >= 0) && (iPercent <= 100));

	if ((pRGB == NULL) || (pPal == NULL))
	{
		ASSERT( FALSE);
		return FALSE;
	}
	int r = 0;
	int g = 0;
	int b = 0;
	int t = 0;
	int iDecalR = GetRValue( *pRGB);
	int iDecalG = GetGValue( *pRGB);
	int iDecalB = GetBValue( *pRGB);

	COLOR_IRGB irgbColor  = 0;

	if (dwFlags & FLAG_DECAL_USE_DRAWING_COLORS)
	{
		if ((iDecalR == iDecalG) && (iDecalR == iDecalB))
		{
			// gray means shadow / highlight
			t = iDecalR - 128;

			if (t)
			{
				// scale range by AMOUNT

				t = (t * 2 * iPercent) / 100;

				if (pDoc)
				{
					if (dwFlags & FLAG_DECAL_USE_EFFECT_BUFFER)
					{
						irgbColor = pDoc->m_pEffectLayer->GetWrappedPixel( x, y);
					}
					else
					{
						irgbColor = pDoc->GetWrappedPixel( x, y);
					}
					r = GetRValue( irgbColor);
					g = GetGValue( irgbColor);
					b = GetBValue( irgbColor);
				}
				else
				{
					r = g = b = 128;
				}

				r += t;
				g += t;
				b += t;
			}
		}
		else if (iDecalG == 255)		// invisble
		{
			if (dwFlags & FLAG_DECAL_CLEAR_BACKGROUND)
			{
				// need to clear any left-over pixels
				// (happens with random strip rivets)

				if (pDoc)
				{
					if (dwFlags & FLAG_DECAL_USE_EFFECT_BUFFER)
					{
						irgbColor = pDoc->m_pEffectLayer->GetWrappedPixel( x, y);
					}
					else
					{
						irgbColor = pDoc->GetWrappedPixel( x, y);
					}
					r = GetRValue( irgbColor);
					g = GetGValue( irgbColor);
					b = GetBValue( irgbColor);
				}
				else
				{
					r = g = b = 128;
				}
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			int iShade       = 0;
			int iLeftAmount  = iDecalR * 200 / 255;
			int iRightAmount = iDecalB * 200 / 255;

			int iLeftR  = 0;
			int iLeftG  = 0;
			int iLeftB  = 0;
			int iRightR = 0;
			int iRightG = 0;
			int iRightB = 0;
			
			if (iLeftAmount > 0)
			{
//				iLeftR = g_CurrentPalette.GetR( g_iColorIndexLeft);
//				iLeftG = g_CurrentPalette.GetG( g_iColorIndexLeft);
//				iLeftB = g_CurrentPalette.GetB( g_iColorIndexLeft);
				iLeftR = GetRValue( g_irgbColorLeft);
				iLeftG = GetGValue( g_irgbColorLeft);
				iLeftB = GetBValue( g_irgbColorLeft);

				if (iLeftAmount > 255)
				{
					iLeftAmount /= 2;
					iShade       = iLeftAmount;
				}
			}
			
			if (iRightAmount > 0)
			{
//				iRightR = g_CurrentPalette.GetR( g_iColorIndexRight);
//				iRightG = g_CurrentPalette.GetG( g_iColorIndexRight);
//				iRightB = g_CurrentPalette.GetB( g_iColorIndexRight);
				iRightR = GetRValue( g_irgbColorRight);
				iRightG = GetGValue( g_irgbColorRight);
				iRightB = GetBValue( g_irgbColorRight);

				if (iRightAmount > 255)
				{
					iRightAmount /= 2;
					iShade       += iRightAmount;
				}
			}

			r = ((iLeftR*iLeftAmount) + (iRightR*iRightAmount) + iShade) / 100;
			g = ((iLeftG*iLeftAmount) + (iRightG*iRightAmount) + iShade) / 100;
			b = ((iLeftB*iLeftAmount) + (iRightB*iRightAmount) + iShade) / 100;

			int iR, iG, iB;

			if (pDoc)
			{
					if (dwFlags & FLAG_DECAL_USE_EFFECT_BUFFER)
					{
						irgbColor = pDoc->m_pEffectLayer->GetWrappedPixel( x, y);
					}
					else
					{
						irgbColor = pDoc->GetWrappedPixel( x, y);
					}
					iR = GetRValue( irgbColor);
					iG = GetGValue( irgbColor);
					iB = GetBValue( irgbColor);
			}
			else
			{
				iR = iG = iB = 128;
			}

			t = iDecalG * 100 / 255;
			r = (r * (100-t) + iR*t) / 100;
			g = (g * (100-t) + iG*t) / 100;
			b = (b * (100-t) + iB*t) / 100;
		}
	}
	else	// standard - just use colors in decal palette (no remapping)
	{
		// color 255 is invisible - TODO - should it be right-btn color instead?
				// scale range by AMOUNT

		int iColor = GetIValue( *pRGB);

		if (iColor != 255)
		{
			if (iPercent == 100)	// speed-up
			{
				r = iDecalR;
				g = iDecalG;
				b = iDecalB;

				goto CalcColor;
			}

			if (pDoc)
			{
				if (dwFlags & FLAG_DECAL_USE_EFFECT_BUFFER)
				{
					irgbColor = pDoc->m_pEffectLayer->GetWrappedPixel( x, y);
				}
				else
				{
					irgbColor = pDoc->GetWrappedPixel( x, y);
				}
				r = GetRValue( irgbColor);
				g = GetGValue( irgbColor);
				b = GetBValue( irgbColor);
			}
			else
			{
				r = g = b = 128;
			}
			r = (r * (100-iPercent) + iDecalR*iPercent) / 100;
			g = (g * (100-iPercent) + iDecalG*iPercent) / 100;
			b = (b * (100-iPercent) + iDecalB*iPercent) / 100;
		}
		else  // it's invisble
		{
			if (dwFlags & FLAG_DECAL_CLEAR_BACKGROUND)
			{
				// need to clear any left-over pixels
				// (happens with random strip rivets)

				if (pDoc)
				{
					if (dwFlags & FLAG_DECAL_USE_EFFECT_BUFFER)
					{
						*pRGB = pDoc->m_pEffectLayer->GetWrappedPixel( x, y);
					}
					else
					{
						*pRGB = pDoc->GetWrappedPixel( x, y);
					}
					return TRUE;
				}
				else
				{
					r = g = b = 128;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	ClampRGB( &r, &g, &b);

CalcColor:
//	iColor = pPal->FindNearestColor( r, g, b, FALSE);
//	*pIRGB = IRGB( iColor, r, g, b);

	// Neal - this is a MAJOR speed up (skip FindNearestColor();
	*pRGB = RGB( r, g, b);

	return TRUE;
}

BOOL CLayer::DrawDecal( CWallyDoc* pDoc, int iImageX, int iImageY, int iMaxSize, 
						int iPercent, DWORD dwFlags, int* piIndex)
{
	CWallyPalette* pPal    = pDoc->GetPalette();
	int            iWidth  = pDoc->Width();
	int            iHeight = pDoc->Height();
	CPoint         ptTemp( iImageX, iImageY);

	// neal - BUGFIX - don't draw rivets when pos is out-of-bounds

	int iStripStart = 0;
	int iStripWidth = iWidth;

	COLOR_IRGB* pData = NULL;

	if (HasData())
	{
		pData   = GetData();
		iWidth  = GetWidth();
		iHeight = GetHeight();

		iStripWidth = iWidth;

		// is it a horizontal strip?  (select one randomly)

		if ((iWidth > iHeight) && ((iWidth % iHeight) == 0))
		{
			//iStripStart = (int )(Random() * (iWidth / iHeight));

			if (piIndex)
				iStripStart = *piIndex;		// measure this item in strip

			if ((piIndex == NULL) || (iStripStart == -1))
			{
				// iIndex == -1 implies pack a random one

				iStripStart = (int )(Random() * (iWidth / iHeight));

				if (piIndex)
					*piIndex = iStripStart;
			}
			else if (iStripStart < 0)
			{
				iStripStart = 0;		// error - measure first item
			}
			else if (iStripStart >= (iWidth / iHeight))
			{
				iStripStart = 0;		// error - measure first item
			}

			iWidth      = iHeight;
			iStripStart *= iWidth;
		}
	}
	else
	{
		ASSERT( FALSE);
		return FALSE;
	}

	int iOffsetX      = iWidth  / 2;
	int iOffsetY      = iHeight / 2;
	int w             = iWidth;
	int h             = iHeight;
	int iDecalOffsetX = 0;
	int iDecalOffsetY = 0;

	if (iMaxSize > 0) 
	{
		if (iMaxSize < iWidth)
		{
			w             = iMaxSize;
			iDecalOffsetX = (iWidth - iMaxSize) / 2;
			iStripStart  += iDecalOffsetX;
		}
		if (iMaxSize < iWidth)
		{
			h             = iMaxSize;
			iDecalOffsetY = (iHeight - iMaxSize) / 2;
		}
	}

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			if (pData)
			{
				COLOR_IRGB irgb = pData[(j+iDecalOffsetY)*iStripWidth + i+iStripStart];

				if (! GetAppliedDecalPixelRGB( pDoc, pPal, &irgb, 
							ptTemp.x + i + iDecalOffsetX - iOffsetX, 
							ptTemp.y + j + iDecalOffsetY - iOffsetY,
							iPercent, dwFlags))
					continue;

				int r = GetRValue( irgb);
				int g = GetGValue( irgb);
				int b = GetBValue( irgb);
				int iColor = pPal->FindNearestColor( r, g, b, FALSE);
				irgb = IRGB( iColor, r, g, b);

				pDoc->SetWrappedPixel( NULL, 
						ptTemp.x + i + iDecalOffsetX - iOffsetX, 
						ptTemp.y + j + iDecalOffsetY - iOffsetY, irgb);
			}
			else
			{
				ASSERT( FALSE);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//  Name:	WriteToClipboard
//	Action:	As the name implies, this will copy the DIB to the clipboard, giving 
//			ownership then to the CWnd passed in.  BPP is used for the
//			bits per pixel value; the values 8BIT or 24BIT can be passed 
/////////////////////////////////////////////////////////////////////////////
void CLayer::WriteToClipboard( CWnd* p_Wnd)
{
	BYTE byPalette[256*3];
	m_Pal.GetPalette( byPalette, 256);

	int iWidth  = m_rBoundsRect.Width();
	int iHeight = m_rBoundsRect.Height();

//#define NUM_BITS 8 // 24 - neal - TESTED AND WORKS in both 8 and 24 bits
//	m_iNumBits = NUM_BITS;

	CDibSection Dib;
	Dib.Init( iWidth, iHeight, m_iNumBits, byPalette);

	if (m_pIRGB_Data)
	{
		int iNumBytes = m_iNumBits / 8;

		for (int j = 0; j < iHeight; j++)
		{
			int k = (j * PadDWORD( iWidth * iNumBytes));

			for (int i = 0; i < iWidth; i++)
			{
				//COLOR_IRGB irgb = m_pIRGB_Data[j*m_iWidth + i];
				COLOR_IRGB irgb = m_pIRGB_Data[
						(j+m_rBoundsRect.top)*m_iWidth + i + m_rBoundsRect.left];

				if (m_iNumBits == 24)
				{
//					for (int k = 0; k < iNumBytes; k++)
//					{
//						Dib.m_byBits[((j*iWidth) + i) * iNumBytes + k] =
//								(BYTE )(irgb & 0xFF);
//						// TEST *********************************
//						//Dib.m_byBits[((j*m_iWidth) + i) * iNumBytes + k] = 255;
//						irgb >>= 8;
//					}
					//int k = ((j*iWidth) + i) * iNumBytes;

					// Neal - this swaps Red & Blue (DIBs are backwards)
					Dib.m_byBits[k+2] = (BYTE )(irgb & 0xFF);
					Dib.m_byBits[k+1] = (BYTE )((irgb >> 8) & 0xFF);
					Dib.m_byBits[k+0] = (BYTE )((irgb >> 16) & 0xFF);

					k += 3;
				}
				else	// 8 bit == indexed 256 color
				{
					Dib.m_byBits[(j*PadDWORD(iWidth)) + i] = GetIValue( irgb);
				}
			}
		}
		Dib.WriteToClipboard( p_Wnd);
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CLayer::WriteToClipboardTiled( CWnd* p_Wnd, int iCountX, int iCountY)
{
	BYTE byPalette[256*3];
	m_Pal.GetPalette( byPalette, 256);

	int iWidth  = m_rBoundsRect.Width();
	int iHeight = m_rBoundsRect.Height();

//#define NUM_BITS 8 // 24 - neal - TESTED AND WORKS in both 8 and 24 bits
//	m_iNumBits = NUM_BITS;

	CDibSection Dib;
	Dib.Init( iWidth, iHeight, m_iNumBits, byPalette);

	if (m_pIRGB_Data)
	{
		int iNumBytes = m_iNumBits / 8;

		for (int j = 0; j < iHeight; j++)
		{
			for (int i = 0; i < iWidth; i++)
			{
				COLOR_IRGB irgb = m_pIRGB_Data[
						(j+m_rBoundsRect.top)*m_iWidth + i + m_rBoundsRect.left];

				int t = (j * PadDWORD( iWidth * iNumBytes)) + (i * iNumBytes);

				if (m_iNumBits == 24)
				{
//					for (int k = 0; k < iNumBytes; k++)
//					{
//						Dib.m_byBits[t + k] = (BYTE )(irgb & 0xFF);
//
//						irgb >>= 8;
//					}
					// Neal - this swaps Red & Blue (DIBs are backwards)
					Dib.m_byBits[t+2] = (BYTE )(irgb & 0xFF);
					Dib.m_byBits[t+1] = (BYTE )((irgb >> 8) & 0xFF);
					Dib.m_byBits[t+0] = (BYTE )((irgb >> 16) & 0xFF);
				}
				else	// 8 bit == indexed 256 color
				{
					Dib.m_byBits[t] = GetIValue( irgb);
				}
			}
		}
		Dib.WriteToClipboardTiled( p_Wnd, iCountX, iCountY);
	}
	else
	{
		ASSERT( FALSE);
	}
}
