/////////////////////////////////////////////////////////////////////////////
// Layer.h : Helper class - represents a single bitmap layer
//           To be used for tools, cut-outs, selections, etc.

#ifndef _LAYER_H_
#define _LAYER_H_

#ifndef _WALLY_H_
	#include "Wally.h"
#endif

//#ifndef _WALLYDOC_H_
//	#include "WallyDoc.h"
//#endif

#ifndef _WALLYPAL_H_
	#include "WallyPal.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CWallyDoc;
class CWallyView;
class CDibSection;

// these are bit flags
#define FLAG_DECAL_USE_DRAWING_COLORS	0x0001	// map to left and right colors
#define FLAG_DECAL_CLEAR_BACKGROUND		0x1000
#define FLAG_DECAL_USE_EFFECT_BUFFER	0x2000


class CLayer
{
// Construction
public:
	CLayer();
	~CLayer();

	void Init();
	void FreeMem();
	void Clear( CWallyView* pWallyView, COLOR_IRGB irgbColor);
	void CopyLayer( CWallyView* pWallyView, CLayer* pLayer);
	void DupLayer( CWallyView* pWallyView, CLayer* pLayer);

// Implementation
public:
	CWallyDoc*    m_pDoc;		// can be NULL (neal - TODO: needs testing)
//	CWallyView*   m_pView;		// can be NULL (neal - TODO: needs testing)

	int           m_iFadeAmount;
//	CPoint        m_ptOffset;				// Neal - TODO: move to CSelection ???
	BYTE*         m_pbyAlphaChannel_Data;
	CWallyPalette m_Pal;
	CRect         m_rBoundsRect;			// Neal - TODO: is there a better way?

protected:
	COLOR_IRGB*   m_pIRGB_Data;
	int           m_iNumBits;	// 8 or 24
	int           m_iWidth;
	int           m_iHeight;

public:
	void Serialize( CArchive& ar, BOOL bSavePalette);
	BOOL LoadFromDibSection( CDibSection *pDIB);
	BOOL LoadFromClipboard( BOOL bAllow24Bit, CWnd* pWnd);
	void WriteToClipboard( CWnd* p_Wnd);
	void WriteToClipboardTiled( CWnd* p_Wnd, int iCountX, int iCountY);

	BOOL DrawDecal( CWallyDoc* pDoc, int iImageX, int iImageY, int iMaxSize, 
					int iPercent, DWORD dwFlags, int* piIndex);

	BOOL           HasData()  { return (m_pIRGB_Data != NULL); };
	COLOR_IRGB*    GetData()  { return m_pIRGB_Data; };

	void Create( void)
		{
			ASSERT( m_pIRGB_Data == NULL);
			FreeMem();

			m_pIRGB_Data = new COLOR_IRGB[m_iWidth*m_iHeight];
		};

	void Create( CWallyDoc* pDoc, int iWidth, int iHeight, int iNumBits)
		{
			ASSERT( pDoc != NULL);
			m_pDoc = pDoc;

			ASSERT( (iWidth > 0) && (iWidth < MAX_MIP_SIZE));
			ASSERT( (iHeight > 0) && (iHeight < MAX_MIP_SIZE));
			ASSERT( (iNumBits == 8) || (iNumBits == 24) || (iNumBits == 32));

			SetNumBits( iNumBits);
			SetWidth( iWidth);
			SetHeight( iHeight);

			Create();
		};

	void ReCreate( CWallyDoc* pDoc, int iWidth, int iHeight, int iNumBits)
		{
			ASSERT( (iNumBits == 8) || (iNumBits == 24) || (iNumBits == 32));

			FreeMem();
			Create( pDoc, iWidth, iHeight, iNumBits);
		}

	CWallyDoc*     GetDoc()      { return m_pDoc;     };
//	CWallyView*    GetView()     { return m_pView;    };
	int            GetWidth()    { return m_iWidth;   };
	int            GetHeight()   { return m_iHeight;  };
	UINT           GetNumBits()  { return m_iNumBits; };
	CWallyPalette* GetPalette()  { return &m_Pal;     };

// Ty- trying something here...
//protected:
public:
	void SetWidth( int iWidth)   { m_iWidth = iWidth;   };
	void SetHeight( int iHeight) { m_iHeight = iHeight; };

public:
	void SetNumBits( int iNumBits)
		{
			ASSERT( (iNumBits == 8) || (iNumBits == 24) || (iNumBits == 32));

			m_iNumBits = iNumBits;
		};

	void GetClipBounds( CRect* pRect, int iIndex);
	int  GetClippedWidth( int iIndex);
	int  GetClippedHeight( int iIndex);

	BOOL GetAppliedDecalPixelRGB( CWallyDoc* pDoc, CWallyPalette* pPal, COLORREF* pRGB, int x, int y, int iPercent, DWORD dwFlags);

	///////////////////////////////////
	// coordinate wrapping routines //
	/////////////////////////////////

	// neal - inlined for speed

	int GetWrappedIndex( int iX, int iY)
		{
			int iWidth  = GetWidth();
			int iHeight = GetHeight();

			if (iY < 0)
				iY += 1000 * iHeight;	// negative modulus problem work-around
			iY %= iHeight;

			if (iX < 0)
				iX += 1000 * iWidth;	// negative modulus problem work-around
			iX %= iWidth;

			return (iY * iWidth) + iX;
		};	

	int GetWrappedX( int iX)
		{
			int iWidth = GetWidth();
			ASSERT( iWidth > 0);
			iWidth = max( iWidth, 1);	// Neal - BUGFIX - don't div by zero

			if (iX < 0)
				iX += 1000 * iWidth;	// negative modulus problem work-around
			iX %= iWidth;

			return iX;
		};

	int GetWrappedY( int iY)
		{
			int iHeight = GetHeight();
			ASSERT( iHeight > 0);
			iHeight = max( iHeight, 1);	// Neal - BUGFIX - don't div by zero

			if (iY < 0)
				iY += 1000 * iHeight;	// negative modulus problem work-around
			iY %= iHeight;

			return iY;
		};

	void SetPixel( CWallyView* pWallyView, int iX, int iY, COLOR_IRGB irgbColor)
	{
		// Neal - TODO: check view 
		//        What are we editing? alpha, RGB data, R,G, or B channel?

		int iWidth = GetWidth();
		int iIndex = (iY * iWidth) + iX;

		if (m_pIRGB_Data && (iX >= 0) && (iY >= 0) && (iX < iWidth) && (iY < GetHeight()))
		{
			// neal - speed up (only draw if we need to)

			if (m_pIRGB_Data[iIndex] == irgbColor)
				return;

			m_pIRGB_Data[iIndex] = irgbColor;
		}
		else
		{
#ifdef _DEBUG
			// Neal - can be set to FALSE in the debugger
			//
			static bool bOneTime = TRUE;

			if (bOneTime)
			{
				ASSERT( FALSE); // Gone outta bounds, better check out why
			}
#endif
		}
	}

	COLOR_IRGB GetPixel( int iX, int iY)
	{
		int iWidth = GetWidth();

		if (m_pIRGB_Data &&
				(iX >= 0) && (iY >= 0) && (iX < iWidth) && (iY < GetHeight()))
		{
			return (m_pIRGB_Data[(iY * iWidth) + iX]);
		}
		else
		{
#ifdef _DEBUG
			// Neal - can be set to FALSE in the debugger
			//
			static bool bOneTime = TRUE;

			if (bOneTime)
			{
				ASSERT( FALSE); // Gone outta bounds, better check out why
			}
#endif
			return (IRGB( 255, 255, 255, 255));
		}
	}

	////////////////////////////////
	// New pixel access routines //
	//////////////////////////////
public:
	COLOR_IRGB GetPixel( int iIndex)
	{
		if (m_pIRGB_Data && (iIndex >= 0) && (iIndex < GetWidth()*GetHeight()))
		{
			return (m_pIRGB_Data[iIndex]);
		}
		else
		{
#ifdef _DEBUG
			// Neal - can be set to FALSE in the debugger
			//
			static bool bOneTime = TRUE;

			if (bOneTime)
			{
				ASSERT( FALSE); // Gone outta bounds, better check out why
			}
#endif
			return (IRGB( 255, 255, 255, 255));
		}
	}

	COLOR_IRGB GetWrappedPixel( int iX, int iY)
		{
			return GetPixel( GetWrappedX( iX), GetWrappedY( iY));
		};

	// use these if you have the full IRGB value
	void SetWrappedPixel( CWallyView* pWallyView, int iX, int iY, COLOR_IRGB irgbColor)
		{
			SetPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), irgbColor);
		};

	// use these if you just have the color palette index
	void SetIndexColorPixel( CWallyView* pWallyView, int iX, int iY, int iColor)
		{
			int r = m_Pal.GetR( iColor);
			int g = m_Pal.GetG( iColor);
			int b = m_Pal.GetB( iColor);
			COLOR_IRGB irgbColor = IRGB( iColor, r, g, b);

			SetPixel( pWallyView, iX, iY, irgbColor);
		};
	void SetIndexColorWrappedPixel( CWallyView* pWallyView, int iX, int iY, int iColor)
		{
			SetIndexColorPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), iColor);
		};

	// use these if you just have the RGB color and need to calc index color too
	void SetNearestColorPixel( CWallyView* pWallyView, int iX, int iY, COLORREF rgbColor, BOOL bIsFullBright = FALSE)
		{
			int iColor = m_Pal.FindNearestColor( rgbColor, bIsFullBright);
			int r      = GetRValue( rgbColor);
			int g      = GetGValue( rgbColor);
			int b      = GetBValue( rgbColor);

			SetPixel( pWallyView, iX, iY, IRGB( iColor, r, g, b));
		};
	void SetNearestColorWrappedPixel( CWallyView* pWallyView, int iX, int iY, COLORREF rgbColor, BOOL bIsFullBright = FALSE)
		{
			SetNearestColorPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), rgbColor, bIsFullBright);
		};
};

#endif _LAYER_H_
