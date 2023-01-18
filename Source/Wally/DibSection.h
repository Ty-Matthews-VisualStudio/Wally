// DibSection.h: interface for the CDibSection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DIBSECTION_H__
#define _DIBSECTION_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Math.h"

#ifndef __MISC_FUNCTIONS_H_
	#include "MiscFunctions.h"
#endif

#define ROTATETO90  0
#define ROTATETO180 1
#define ROTATETO270 2

#define RGB_8BIT	1
#define RGB_24BIT	3

#define SHIFT_UP		VK_UP
#define SHIFT_DOWN		VK_DOWN
#define SHIFT_LEFT		VK_LEFT
#define SHIFT_RIGHT		VK_RIGHT

#define DIB_ERROR_START					0
#define DIB_ERROR_OUT_OF_MEMORY			(DIB_ERROR_START + 1)
#define DIB_ERROR_CREATE				(DIB_ERROR_START + 2)
#define DIB_ERROR_CLIPBOARD				(DIB_ERROR_START + 3)
#define DIB_ERROR_END					(DIB_ERROR_START + 4)

#define DIB_FLAG_FLIP_REDBLUE			0x1
#define DIB_FLAG_WITH_PADDING			0x2

typedef struct
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
} DIB_BITMAPINFO, *LPDIB_BITMAPINFO;

typedef struct
{
	// This is from the BITMAP struct declaration
	long	bmType; 
	long	bmWidth; 
	long	bmHeight; 		
	WORD	bmPlanes; 	
	WORD	bmBitsPixel; 
	BYTE	bmBitsPixelFiller[16]; 
	WORD	bmColorsUsed;
	BYTE    bmBits[6];
} DIBType, *LPDIBType;

union ClipboardDataType
{
	LPDIBType	lpDIB; 
	LPBYTE		pbyClipboardData;
};

class CLayer;

class CDibSection  
{
public:	
	HBITMAP m_BMHan;
	DIB_BITMAPINFO m_BMInfo;
	BYTE *m_byBits;

	unsigned char m_byOriginalPalette[768];

	int m_iWidth;
	int m_iHeight;
	int m_iByteWidth;

	int m_iBitCount;
	bool m_bWasInit;
	bool m_bUseGamma;

	int m_iErrorCode;
	CString m_strError;

public:	
	CDibSection();	
	virtual ~CDibSection();	
	
	BOOL Init( int iWidth, int iHeight, int iBitCount, BYTE* pbyPal = NULL, bool bUseGamma = TRUE);
	BOOL ReInit( int iWidth, int iHeight, int iBitCount, BYTE* pbyPal = NULL, bool bUseGamma = TRUE);
	void RebuildPalette( BYTE* pbyPal, BOOL bSaveAsOriginalPalette);

	operator BYTE *() { return m_byBits; }
	operator HBITMAP ()	{ return m_BMHan; }
	
protected:
	void InternalStretchBlt( HDC hDC, int X, int Y, double dfZoom, HDC hContextDC);

public:

	// Display operations
	void Show (CDC* ActiveWindow, int X, int Y, double dfZoom);	
	void ShowTiled (CDC* ActiveWindow, int X, int Y, double dfZoom, int NumTiles);
	void ShowRestricted (CDC* ActiveWindow, CRect rcClient);	
//	void Stretch (CDC* ActiveWindow, CRect m_Rect, int Width, int Height);
//	void Stretch (CPaintDC* ActiveWindow, CRect m_Rect, int Width, int Height);		

	// Clipboard operations
	BOOL InitFromClipboard(CWnd *pWnd);
	BOOL WriteToClipboard (CWnd *pWnd);
	BOOL WriteToClipboardTiled (CWnd *pWnd, int iCountX, int iCountY);

	// Bit manipulation operations
	void AddDIB (int X, int Y, int iWidth, int iHeight, BYTE *pbyAddBits);
	void MirrorBits ();
	void FlipBits ();
	void RotateBits (int iDestinationAngle);
//	void ShiftBits (int iDirection, int iFactor);
	
	void FlipWidthHeight();
	BOOL RebuildDibSection( BYTE *pbyPal = NULL);
	
	BYTE* GetBits()			{ return m_byBits;            }
	BYTE* GetPalette()		{ return m_byOriginalPalette; }
	bool  IsInit()			{ return m_bWasInit;          }
	int   GetWidth()		{ return m_iWidth;            }
	int   GetHeight()		{ return m_iHeight;           }
	int   GetImageWidth()	{ return GetWidth();          }
	int	  GetImageHeight()	{ return GetHeight();         }
	int   GetByteWidth()	{ return m_iByteWidth;        }
	int   GetByteCount()	{ return (GetBitCount() / 8); }
	int   GetBitCount()		{ return m_iBitCount;         }
//	int   GetBitCount()		{ return (m_iBitCount != 32) ?  m_iBitCount : 24; }
	int   GetErrorCode()	{ return m_iErrorCode;        }

	void SetWidth( int iWidth)
	{		
		m_iWidth = iWidth;
		SetByteWidth( PadDWORD (iWidth * GetByteCount()));
	}
	void SetByteWidth( int iByteWidth)	{ m_iByteWidth = iByteWidth; }
	void SetHeight( int iHeight)		{ m_iHeight = iHeight;       }
	void SetBitCount( int iBitCount)
	{ 
		ASSERT( (iBitCount == 8) || (iBitCount == 24) || (iBitCount == 32));
//		if (iBitCount == 32)
//			iBitCount = 24;
		m_iBitCount = iBitCount;
	}
	void SetErrorCode( int iErrorCode)	{ m_iErrorCode = iErrorCode; }
	void SetErrorString( LPCTSTR szError)	{ m_strError = szError;  }
	CString GetErrorString()				{ return m_strError;     }

	void SetRawBits (BYTE *pbyData, int iFlags = DIB_FLAG_FLIP_REDBLUE);
	void GetRawBits (BYTE *pbyData, int iFlags = DIB_FLAG_FLIP_REDBLUE);
	void SetBitsFromLayer( CLayer* pLayer, RECT *pRect, bool bOnlyToolDirtyArea, bool bTiled);
	void SetRawLiquidBits (BYTE *pbyData, int iFrame);
	void GetFlippedBits (BYTE *pbyData, int iFlags = 0);

	COLOR_IRGB GetPixelAtIndex( UINT iIndex);
	COLOR_IRGB GetPixelAtXY( UINT iX, UINT iY);

	void AddLine (int iLineNumber, int iNumBits, BYTE *pbyBits);
	void ClearBits (int iIndex);	

	CString GetError();
};

#endif // #ifndef _DIBSECTION_H__
