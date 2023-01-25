// ClipboardDIB.h: interface for the CClipboardDIB class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CLIPBOARDDIB_H_
#define _CLIPBOARDDIB_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _REMIP_H_
	#include "ReMip.h"
#endif

#if !defined GET_RGB
	#define GET_RGB 1
#endif

#if !defined GET_INDEX
	#define GET_INDEX 0
#endif

typedef struct
{  // This is from the BITMAP struct declaration
		long	bmType; 
		long	bmWidth; 
		long	bmHeight; 		
		WORD	bmPlanes; 
		//byte   bmPlanesFiller; 
		WORD	bmBitsPixel; 
		BYTE	bmBitsPixelFiller[16]; 
		WORD	bmColorsUsed;
		BYTE    bmBits[6]; 
} CDDIBType, *LPCDDIBType;

union CDClipboardDataType
{
	LPCDDIBType DIB; 
	unsigned char* p_ClipboardData;
};

class CWallyPalette;

class CClipboardDIB  
{
// Members
	HGLOBAL hg;	
	CDClipboardDataType m_pData;
	unsigned char *m_ClipboardPalette; // For 8-bit images	
	unsigned char* p_BitsArray;	

// Methods
public:
	CClipboardDIB();
	virtual ~CClipboardDIB();
	bool InitFromClipboard(CWnd* p_Wnd);
	void GetDIBPalette(unsigned char* p_Palette);	
	void WriteToClipboard( CWnd* p_Wnd, CWallyPalette* pPal, BYTE* pbyBits, int iWidth, int iHeight);

	inline operator unsigned char* ()
	{
		return p_BitsArray;
	}
	
	inline long GetWidth()
	{
#ifdef _DEBUG
	if (m_pData.DIB == 0)
		AfxMessageBox( "Error: ClipboardDIB::GetWidth() - NULL DIB pointer!");

	if ((m_pData.DIB->bmWidth <= 0) || (m_pData.DIB->bmWidth > MAX_MIP_SIZE))
	{
		CString strMsg;
		strMsg.Format( "Error: ClipboardDIB::GetWidth() - Invalid Width == %d", m_pData.DIB->bmWidth);
		AfxMessageBox( strMsg);
	}
#endif
		return m_pData.DIB->bmWidth;
	}

	inline long GetHeight()
	{
#ifdef _DEBUG
	if (m_pData.DIB == 0)
		AfxMessageBox( "Error: ClipboardDIB::GetHeight() - NULL DIB pointer!");

	if ((m_pData.DIB->bmHeight <= 0) || (m_pData.DIB->bmHeight > MAX_MIP_SIZE))
	{
		CString strMsg;
		strMsg.Format( "Error: ClipboardDIB::GetHeight() - Invalid Height == %d", m_pData.DIB->bmHeight);
		AfxMessageBox( strMsg);
	}
#endif
		return m_pData.DIB->bmHeight;
	}

	inline WORD GetColorDepth()
	{
#ifdef _DEBUG
	if (m_pData.DIB == 0)
		AfxMessageBox( "Error: ClipboardDIB::GetColorDepth() - NULL DIB pointer!");

	if ((m_pData.DIB->bmBitsPixel != 8) && (m_pData.DIB->bmBitsPixel != 24))
	{
		CString strMsg;
		strMsg.Format( "Error: ClipboardDIB::GetColorDepth() - Invalid BitsPerPixel == %d", m_pData.DIB->bmBitsPixel);
		AfxMessageBox( strMsg);
	}
#endif
		return m_pData.DIB->bmBitsPixel;
	}

	inline WORD GetColorsUsed()
	{
#ifdef _DEBUG
	if (m_pData.DIB == 0)
		AfxMessageBox( "Error: ClipboardDIB::GetWidth() - NULL DIB pointer!");

	if ((m_pData.DIB->bmColorsUsed < 0) || (m_pData.DIB->bmColorsUsed > 256))
	{
		CString strMsg;
		strMsg.Format( "Error: ClipboardDIB::GetWidth() - Invalid NumberOfColors == %d", m_pData.DIB->bmColorsUsed);
		AfxMessageBox( strMsg);
	}
#endif
		return m_pData.DIB->bmColorsUsed;
	}
};

#endif // _CLIPBOARDDIB_H_
