/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyPal.h : interface of the CWallyPalette class
//
// Created by Neal White III
/////////////////////////////////////////////////////////////////////////////

#ifndef _WALLYPAL_H_
#define _WALLYPAL_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _REMIP_H_
	#include "remip.h"
#endif // _MSC_VER >= 1000

#ifndef _PALMAP_H_
	#include "PalMap.h"
#endif

class CWallyPalette : public CObject
{
public:
	virtual ~CWallyPalette();
	CWallyPalette();

protected:
	DECLARE_SERIAL( CWallyPalette)

// Members
public:
	const CWallyPalette& operator=( const CWallyPalette& Pal)
	{
		m_PalMap          = Pal.m_PalMap;
		m_iNumFullBrights = Pal.m_iNumFullBrights;
		m_iNumColors      = Pal.m_iNumColors;
		memcpy( m_rgbPalette, Pal.m_rgbPalette, sizeof (m_rgbPalette));
		memcpy( m_hsvPalette, Pal.m_hsvPalette, sizeof (m_hsvPalette));

		return *this;
	}

public:
	CPalMap  m_PalMap;

protected:
    WORD     m_wVersionMajor;
    WORD     m_wVersionMinor;

	int      m_iNumFullBrights;  // should be read from last byte of "palette.lmp" file
	int      m_iNumColors;       // must be 256 for now
	COLORREF m_rgbPalette[256];
	HSV      m_hsvPalette[256];  // palette rgb converted to hsv

	// neal - used as cache for FindNearestColor (major speed-up!)
	int      m_iOldColor;
	int      m_iOldR;
	int      m_iOldG;
	int      m_iOldB;

// Operations
public:
	// TODO: neal - what about undo?
	void GetPalette( BYTE* pbyPackedPalette, int iNumColors);	// R-G-B, R-G-B, ...
	void SetPalette( BYTE* pbyPackedPalette, int iNumColors);	// R-G-B, R-G-B, ...
	void ClearPalette( void);


	COLOR_IRGB GetIRGB( int iIndex) { return ((iIndex << 24) | m_rgbPalette[iIndex]); };
	COLORREF   GetRGB( int iIndex) { return m_rgbPalette[iIndex]; };
	// TODO: neal - what about undo?
	void       SetRGB( int iIndex, int iRed, int iGreen, int iBlue);

	int GetR( int iIndex) { return GetRValue( m_rgbPalette[iIndex]); };
	int GetG( int iIndex) { return GetGValue( m_rgbPalette[iIndex]); };
	int GetB( int iIndex) { return GetBValue( m_rgbPalette[iIndex]); };

	double GetH( int iIndex)   { return m_hsvPalette[iIndex].dfHue; };
	double GetS( int iIndex)   { return m_hsvPalette[iIndex].dfSaturation; };
	double GetV( int iIndex)   { return m_hsvPalette[iIndex].dfValue; };
	void   SetHSV( int iIndex, double dfHue, double dfSaturation, double dfValue);
	BOOL   IsGray( int iIndex) { return m_hsvPalette[iIndex].bIsGray; };

	int  GetNumColors( void)                  { return m_iNumColors; };
	void SetNumColors( int iNumColors)        { m_iNumColors = iNumColors; };
	int  GetNumFullBrights( void)             { return m_iNumFullBrights; };
	void SetNumFullBrights( int iFullBrights) { m_iNumFullBrights = iFullBrights; };
	BOOL IsFullBright( int iIndex)            { return (iIndex >= (m_iNumColors - m_iNumFullBrights)); };

	int FindNearestColor( int r, int g, int b, BOOL bIsFullBright);
	int FindNearestColor( COLORREF rgbColor, BOOL bIsFullBright)
		{
			return FindNearestColor( GetRValue( rgbColor), 
					GetGValue( rgbColor), GetBValue( rgbColor), bIsFullBright);
		}
	void CalcHSV( int iIndex);
	void InitHSVPalette( int iNumFullBrights = -1);

	void RebuildWAL( BYTE* pbyData);

	void Convert24BitTo256Color( COLOR_IRGB* pSrcData, 
			BYTE* byDestData, int iWidth, int iHeight, int iBuild, 
			UINT uDitherType, BOOL bSharpen);

// Overrides
	virtual void Serialize( CArchive& ar);

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext& dc) const;
#endif

};

#endif _WALLYPAL_H_