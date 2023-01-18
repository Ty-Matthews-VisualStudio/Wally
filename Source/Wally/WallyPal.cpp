/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyPal.cpp : Defines the class behaviors for CWallyPalette.
//
// Created by Neal White III, 12-16-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyPal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWallyPalette

IMPLEMENT_SERIAL( CWallyPalette, CObject, 101)

CWallyPalette::CWallyPalette()
{
    m_wVersionMajor = m_wVersionMinor = 0;

	ClearPalette();
}

CWallyPalette::~CWallyPalette()
{
#ifdef _DEBUG
	m_PalMap.FullAssertValid( this);
#endif
}

void CWallyPalette::ClearPalette( void)
{
	// should be read from last byte of "palette.lmp" file

	m_iNumFullBrights = NUM_FULL_BRIGHTS;
	m_iNumColors      = 256;

	m_PalMap.RemoveAll();

	for (int i = 0; i < m_iNumColors; i++)
	{
		m_rgbPalette[i] = 0;

		// update palette map (used for fast look-ups
		m_PalMap.AddRef( 0, 0);

		m_hsvPalette[i].dfHue        = 0.0;
		m_hsvPalette[i].dfSaturation = 0.0;
		m_hsvPalette[i].dfValue      = 0.0;
		m_hsvPalette[i].bIsGray      = TRUE;
	}

	// neal - fixes "black flash" palette bug ???
	// ((m_iNumColors == 0) indicates uninitialized palette)
	m_iNumColors = 0;

	// neal - used as cache for FindNearestColor (major speed-up!)
	m_iOldColor = -1;
	m_iOldR     = -1;
	m_iOldG     = -1;
	m_iOldB     = -1;
}

/////////////////////////////////////////////////////////////////////////////
// CWallyPalette serialization

void CWallyPalette::Serialize( CArchive& ar)
{
    #define VER_MAJOR   1
	#define VER_MAJOR1  VER_MAJOR

	#define VER_MINOR   1

	m_wVersionMajor = VER_MAJOR;
	m_wVersionMinor = VER_MINOR;

	#define SIGNATURE "WallyPal"
	CString strFileSignature( SIGNATURE);

	

	// sanity check

	if (ar.IsStoring())	
	{
		ar << strFileSignature;
		ar << m_wVersionMajor;
		ar << m_wVersionMinor;
	}
	else
	{
		ar >> strFileSignature;

		if (strFileSignature != SIGNATURE)
		{
			TRACE0( _T("WallyPal load failed - 'signature' not found\n"));
			AfxThrowArchiveException( CArchiveException::badClass);
		}

		ar >> m_wVersionMajor;
		ar >> m_wVersionMinor;

		if (m_wVersionMajor != VER_MAJOR)
		{
			TRACE0( _T("WallyPal load failed - unrecognized file version\n"));
			AfxThrowArchiveException( CArchiveException::badClass);
		}

		//if ((m_wVersionMinor > VER_MINOR) || (m_wVersionMinor < VER_MINOR1))
		if (m_wVersionMinor != VER_MINOR)
		{
			TRACE0( _T("WallyPal load failed - unrecognized file version\n"));
			AfxThrowArchiveException( CArchiveException::badClass);
		}
	}

	// actual save / load

	DWORD dwNumColors = 256;

	if (ar.IsStoring())
	{
		ar << dwNumColors;
		ar << m_iNumFullBrights;

		ar.Write( m_rgbPalette, sizeof( m_rgbPalette));
	}
	else	// loading
	{
		ar >> dwNumColors;

		if (dwNumColors != 256)
		{
			TRACE0( _T("WallyPal load failed - number of colors != 256\n"));
			AfxThrowArchiveException( CArchiveException::badClass);
		}

		ar >> m_iNumFullBrights;

		UINT uSize = ar.Read( m_rgbPalette, sizeof( m_rgbPalette));

		if (uSize != sizeof( m_rgbPalette))
		{
			TRACE0( _T("WallyPal load failed - end-of-file reached\n"));
			AfxThrowArchiveException( CArchiveException::badClass);
		}

		// neal - remove the cache, we're about to over-write 
		//        the matching palette (this prevents asserts)
		m_PalMap.RemoveAll();

		// neal - no need to save the HSV data - just rebuild it

		//InitHSVPalette( dwNumColors);
		InitHSVPalette( m_iNumFullBrights);  // neal - MAJOR bugfix - undos were screwed up!

		// neal - update cache (bugfix)

		if (m_iOldColor != -1)
		{
			m_iOldR = GetR( m_iOldColor);
			m_iOldG = GetG( m_iOldColor);
			m_iOldB = GetB( m_iOldColor);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWallyPalette diagnostics

#ifdef _DEBUG
void CWallyPalette::AssertValid() const
{
	CObject::AssertValid();
}

void CWallyPalette::Dump( CDumpContext& dc) const
{
	CObject::Dump( dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

void CWallyPalette::CalcHSV( int iIndex)
{	
	COLORREF rgb = m_rgbPalette[iIndex];
	int r = GetRValue( rgb);
	int g = GetGValue( rgb);
	int b = GetBValue( rgb);
	double dfr = r;
	double dfg = g;
	double dfb = b;

	// r, g, b are 0..255
	// h is 0..360, s and v are 0.0..1.0
	RGBtoHSV( dfr, dfg, dfb, &(m_hsvPalette[iIndex].dfHue), 
			&(m_hsvPalette[iIndex].dfSaturation), &(m_hsvPalette[iIndex].dfValue));

	m_hsvPalette[iIndex].bIsGray = ((r == g) && (r == b));
}

void CWallyPalette::InitHSVPalette( int iNumFullBrights)
{	
	//m_iNumFullBrights = NUM_FULL_BRIGHTS;

	if (iNumFullBrights != -1)
		m_iNumFullBrights = iNumFullBrights;

//#ifdef _DEBUG
//	m_PalMap.FullAssertValid( this);
//#endif

	m_PalMap.RemoveAll();

	for (int i = 0; i < 256; i++)
	{
		CalcHSV( i);

		// update palette map (used for fast look-ups

		m_PalMap.AddRef( m_rgbPalette[i], i);
	}

#ifdef _DEBUG
	m_PalMap.FullAssertValid( this);
#endif
}

// TODO - what about UNDO ???

void CWallyPalette::SetRGB( int iIndex, int iRed, int iGreen, int iBlue)
{
	ASSERT( (iIndex >= 0) && (iIndex <= 255));

	if ((iIndex < 0) || (iIndex > 255))
	{
		AfxMessageBox( "CWallyPalette::SetRGB - invalid iIndex");
		return;
	}

//#ifdef _DEBUG
//	m_PalMap.FullAssertValid( this);
//#endif

	ASSERT( (iRed   >= 0) && (iRed   <= 255));
	ASSERT( (iGreen >= 0) && (iGreen <= 255));
	ASSERT( (iBlue  >= 0) && (iBlue  <= 255));

	ClampRGB( &iRed, &iGreen, &iBlue);

	COLORREF rgbOld = m_rgbPalette[iIndex];
	COLORREF rgbNew = RGB( iRed, iGreen, iBlue);

	// Neal - hunting for palette cache bug
	//m_rgbPalette[iIndex] = rgbNew;

	// now update palette map (used for fast look-ups

	if (rgbNew != rgbOld)
	{
		// neal - hunting for rgb==0, index==255 bug
//		ASSERT( (rgbOld != 0) || (iIndex != 255));

		m_rgbPalette[iIndex] = rgbNew;

//		if (m_PalMap.GetCount())
			m_PalMap.Release( rgbOld, iIndex, this);

		m_PalMap.AddRef(  rgbNew, iIndex);
	}
	else
	{
		ASSERT( m_rgbPalette[iIndex] == rgbNew);
	}

#ifdef _DEBUG
	m_PalMap.FullAssertValid( this);
#endif

	// neal - update cache (bugfix)

	if (m_iOldColor == iIndex)
	{
		m_iOldR = iRed;
		m_iOldG = iGreen;
		m_iOldB = iBlue;
	}
	CalcHSV( iIndex);
}

void CWallyPalette::SetHSV( int iIndex, double dfHue, double dfSaturation, double dfValue)
{
	ASSERT( (iIndex >= 0) && (iIndex <= 255));

	if ((iIndex < 0) || (iIndex > 255))
	{
		AfxMessageBox( "CWallyPalette::SetHSV - invalid iIndex");
		return;
	}

	if (dfHue < 0.0)
		dfHue += 360.0;
	else if (dfHue > 360.0)
		dfHue -= 360.0;

	ASSERT( (dfHue        >= 0.0) && (dfHue        <= 360.0));
	ASSERT( (dfSaturation >= 0.0) && (dfSaturation <= 1.0));
	ASSERT( (dfValue      >= 0.0) && (dfValue      <= 1.0));

//#ifdef _DEBUG
//	m_PalMap.FullAssertValid( this);
//#endif

	dfHue        = max( dfHue,        0.0);
	dfHue        = min( dfHue,        360.0);
	dfSaturation = max( dfSaturation, 0.0);
	dfSaturation = min( dfSaturation, 1.0);
	dfValue      = max( dfValue,      0.0);
	dfValue      = min( dfValue,      1.0);

	// r, g, b are 0..255
	// h is 0..360, s and v are 0.0..1.0

	m_hsvPalette[iIndex].dfHue        = dfHue;
	m_hsvPalette[iIndex].dfSaturation = dfSaturation;
	m_hsvPalette[iIndex].dfValue      = dfValue;

	int iRed, iGreen, iBlue;
	HSVtoRGB( dfHue, dfSaturation, dfValue, &iRed, &iGreen, &iBlue);

	COLORREF rgbOld = m_rgbPalette[iIndex];
	COLORREF rgbNew = RGB( iRed, iGreen, iBlue);

	m_rgbPalette[iIndex]         = rgbNew;
	m_hsvPalette[iIndex].bIsGray = ((iRed == iGreen) && (iRed == iGreen));

	// now update palette map (used for fast look-ups

	if (rgbNew != rgbOld)
	{
		m_PalMap.Release( rgbOld, iIndex, this);
		m_PalMap.AddRef(  rgbNew, iIndex);
	}

#ifdef _DEBUG
	m_PalMap.FullAssertValid( this);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Name:        GetPal
// Action:      Copies our COLORREF palette into packed R-G-B byte palette
/////////////////////////////////////////////////////////////////////////////
void CWallyPalette::GetPalette( BYTE* pbyPackedPalette, int iNumColors)
{
	ASSERT( iNumColors == 256);				// must be 256 for now
	//ASSERT( m_iNumColors == iNumColors);	// the sizes *better* match

	ASSERT( (m_iNumColors == iNumColors) ||	// the sizes *better* match
			(m_iNumColors == 0));			// OR an empty palette

	for (int i = 0; i < iNumColors; i++)
	{
		COLORREF rgb = GetRGB( i);
		int iRed   = GetRValue( rgb);
		int iGreen = GetGValue( rgb);
		int iBlue  = GetBValue( rgb);

		pbyPackedPalette[i*3]     = iRed;
		pbyPackedPalette[i*3 + 1] = iGreen;
		pbyPackedPalette[i*3 + 2] = iBlue;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:        SetPal
// Action:      Copies packed R-G-B byte palette into our COLORREF palette
/////////////////////////////////////////////////////////////////////////////

// TODO: neal - what about undo?
void CWallyPalette::SetPalette( BYTE* pbyPackedPalette, int iNumColors)
{
	if (this == NULL)
	{
		AfxMessageBox( "Error: CWallyPalette::SetPalette() - this object is NULL");
		return;
	}

	if (iNumColors != 256)
	{
		ASSERT( iNumColors == 256);		// must be 256 for now

		CString strTemp;
		strTemp.Format( "Error: CWallyPalette::SetPalette() - Number of colors "
				"is %d instead of 256!", iNumColors);
		AfxMessageBox( strTemp);

		iNumColors = min( 256, iNumColors);
	}

//#ifdef _DEBUG
//	m_PalMap.FullAssertValid( this);
//#endif

	//ClearPalette();		// Neal - reinit things properly
	//m_PalMap.RemoveAll();	// Neal - reinit things properly

	m_iNumColors = iNumColors;

	for (int i = 0; i < iNumColors; i++)
	{
		int iRed   = pbyPackedPalette[i*3];
		int iGreen = pbyPackedPalette[i*3 + 1];
		int iBlue  = pbyPackedPalette[i*3 + 2];

		SetRGB( i, iRed, iGreen, iBlue);
	}

#ifdef _DEBUG
	m_PalMap.FullAssertValid( this);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Name:        FindNearestColor
// Action:      Converts a 24 bit RGB value into a 256 color index
//              based on the palette.lmp file
/////////////////////////////////////////////////////////////////////////////
int CWallyPalette::FindNearestColor( int r, int g, int b, BOOL bIsFullBright)
{
	// neal - cache last color found - major speed-up!

	if ((r == m_iOldR) && (g == m_iOldG) && (b == m_iOldB))
		return (m_iOldColor);

	// Neal - leave metrics code here for speed testing

//	static DWORD dwTotalCountFAST = 0;
//	static DWORD dwTotalCountSLOW = 0;

#if 1		// Neal - TODO - FIX BLACK-IS-255-INSTEAD-OF-ZERO BUG
		// neal - hunt for rgb==0, index==255 bug

	int iPalIndex = m_PalMap.LookUpColorIndex( RGB( r,g,b));
	if (iPalIndex != -1)
	{
//		dwTotalCountFAST++;

		return iPalIndex;
	}
#endif

//	dwTotalCountSLOW++;

	int i;
	int iFound = 255;

	// nw - longs are faster than doubles (and they're big enough)

	long distance = 99999999;
	long dr, dg, db, tdist;

	double dHue, dSaturation, dValue;

	RGBtoHSV( r, g, b, &dHue, &dSaturation, &dValue);

	// find nearest color loop

	if (bIsFullBright)
	{
		for (i = 256-m_iNumFullBrights; i < 255; i++)
		{
			// use RGB method

			dr = r - GetR( i);
			dg = g - GetG( i);
			db = b - GetB( i);

			tdist = dr*dr + dg*dg + db*db;

			if (tdist < distance)
			{
				distance = tdist;
				iFound   = i;

				if (tdist == 0)
				{
					// we found an exact match!

					ASSERT( FALSE);  // should have been caught by CMap above
					break;
				}
			}
		}
	}
	else
	{
		BOOL bIsGray = (dSaturation <= 0.0156);

TryAgain:

		for (i = 0; i < 256-m_iNumFullBrights; i++)
		{
			// if pixel should be gray, don't match any nearby COLORS

			if (bIsGray && (! m_hsvPalette[i].bIsGray))
				continue;

			// use RGB method

			dr = r - GetR( i);
			dg = g - GetG( i);
			db = b - GetB( i);

			tdist = dr*dr + dg*dg + db*db;

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
			dr = r - GetR( iFound);
			dg = g - GetG( iFound);
			db = b - GetB( iFound);

			tdist = dr*dr + dg*dg + db*db;

			if (tdist > (3*16*16))
			{
				// not a close enough match

				bIsGray = FALSE;
				goto TryAgain;
			}
		}
	}
	m_iOldColor = iFound;
	m_iOldR     = r;
	m_iOldG     = g;
	m_iOldB     = b;

	return iFound;
}

void CWallyPalette::RebuildWAL( BYTE* pbyMipData)
{
	::RebuildWAL( this, pbyMipData);
}

void CWallyPalette::Convert24BitTo256Color( COLOR_IRGB* pSrcData, 
			BYTE* byDestData, int iWidth, int iHeight, int iBuild, 
			UINT uDitherType, BOOL bSharpen)
{
	::Convert24BitTo256Color( this, pSrcData, byDestData, 
			iWidth, iHeight, iBuild, uDitherType, bSharpen);
}
