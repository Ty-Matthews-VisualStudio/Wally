/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1999,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  PcxHelper.cpp : PCX file i/o for the CImageHelper class
//
//  Created by Neal White III, 9-5-1999
/////////////////////////////////////////////////////////////////////////////

#pragma warning( error : 0)

This file is not yet finished and should not be built

#include "stdafx.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"

/////////////////////////////////////////////////////////////////////////////

#define DECODE_BUFF_LINES	1
#define ENCODE_BUFF_SIZE	4096

#define PAL_256_KEYWORD  12

// in runcomp.asm
int FAR PASCAL UncompressPCX( LPSTR, PSTR, int, int, PINT, PINT);

/////////////////////////////////////////////////////////////////////////////
extern WORD Splitto1By4(LPSTR, LPSTR, WORD, WORD);

static void  SetHeaderPalElement( PCXHEADER*, int, DWORD);
static int   Write256Palette( CImageHelper*, int);
static BOOL  SavePcx8Bit( HWND, CImageHelper*, PINT, PSTR, PRECT, OFSTRUCT *);

/////////////////////////////////////////////////////////////////////////////
// Name:	IsPcxHeaderValid
// Action:	Determines if the pcx file header looks OK
/////////////////////////////////////////////////////////////////////////////
inline BOOL IsPcxHeaderValid( PCXHEADER* pHdr)
{
	return ((pHdr->byManufacturer == 10) && (pHdr->byEncoding == 1));
}

/////////////////////////////////////////////////////////////////////////////
// Name:	IfGlobalFree
// Action:	Unlocks and frees global memory if appropriate
/////////////////////////////////////////////////////////////////////////////
inline HGLOBAL IfGlobalFree( HGLOBAL hBuff, LPVOID lpBuff)
{
	if (hBuff)
	{
		if (lpBuff)
			GlobalUnlock( hBuff);

		return (GlobalFree( hBuff));
	}
	return (NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	IfLocalFree
// Action:	Unlocks and frees local memory if appropriate
/////////////////////////////////////////////////////////////////////////////
inline HLOCAL IfLocalFree( HLOCAL hBuff, LPVOID lpBuff)
{
	if (hBuff)
	{
		if (lpBuff)
			LocalUnlock( hBuff);

		return (LocalFree( hBuff));
	}
	return (NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	IfDeleteObject
// Action:	Deleted a GDI object handle if appropriate
/////////////////////////////////////////////////////////////////////////////
BOOL IfDeleteObject( HGDIOBJ hGdiObj)
{
	if (hGdiObj)
		return (DeleteObject( hGdiObj));
	else
		return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Name:	SetHeaderPalElement
// Action:	Sets a palette element in the PCX header
//
// Note:	Saves code space (only used by SetupPcxHeader below)
/////////////////////////////////////////////////////////////////////////////
static void SetHeaderPalElement( PCXHEADER *pHdr, int iColor, DWORD dwRGB)
{
	pHdr->byPal[3 * iColor]     = GetRValue( dwRGB);
	pHdr->byPal[3 * iColor + 1] = GetGValue( dwRGB);
	pHdr->byPal[3 * iColor + 2] = GetBValue( dwRGB);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	CalcBytesPerLine
// Action:	Rreturn the number of bytes per Line (per plane), WORD aligned
/////////////////////////////////////////////////////////////////////////////
static int CalcBytesPerLine( int iWidth, int iBitsPerPixelPlane)
{
	// bytes per line per plane in picture
	int iBytesPerLine = (WORD )(((iWidth * iBitsPerPixelPlane)+7) / 8);

	// bytes per line per plane in picture (WORD aligned)
	iBytesPerLine = (WORD )((iBytesPerLine + 1) & ~1);

	return (iBytesPerLine);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	SetupPcxHeader
// Action:	Initializes the header of a PCX file, prior to saving it
//
// Parameters:	pHeader:	The header to fill
//				pImage:		The image we're saving
//				iHres:		X size of image (width in pixels)
//				iVres:		Y size of image (height in pixels)
/////////////////////////////////////////////////////////////////////////////
void SetupPcxHeader( PCXHEADER *pHdr, CImageHelper* pImage, int iHRes, int iVRes)
{
	int i;

	pHdr->byManufacturer = 10;	// keyword (means ZSoft)
	pHdr->byHardware     = 5;	// hardware series (PCX file version)
	pHdr->byEncoding     = 1;	// 1 means run length encoded

	pHdr->byBitsPerPixelPlane = pImage->GetNumBitsPerPixelPlane();
	pHdr->byNumPlanes         = pImage->GetNumPlanes();
	int iColors               = pImage->GetNumColors();

	// bytes per line per plane in picture (WORD aligned)
	pHdr->wBytesPerLine = (WORD )CalcBytesPerLine( pImage->GetImageWidth(), pHdr->byBitsPerPixelPlane);

	pHdr->wX1 = 0;				// picture dimensions (in pixels, inclusive)
	pHdr->wY1 = 0;
	pHdr->wX2 = (WORD )(iHRes-1);
	pHdr->wY2 = (WORD )(iVRes-1);

	pHdr->wHdpi = 96;		// dpi of image (creating device), screen is 96 dpi
	pHdr->wVdpi = 96;

	pHdr->wHScreenSize = (WORD )GetDeviceCaps( NULL, HORZRES);	// screen size
	pHdr->wVScreenSize = (WORD )GetDeviceCaps( NULL, VERTRES);

	ZeroMemory( &(pHdr->byPal[0]), 3*16);	// clear internal palette to zero

	if (2 == iColors)
	{
		// special case 2 colors (B & W)

		SetHeaderPalElement( pHdr, 0, RGB(   0,   0,   0));
		SetHeaderPalElement( pHdr, 1, RGB( 255, 255, 255));
	}
	else if (iColors <= 16)
	{
		// special case palette included in header

		for (i = 0; i < iColors; i++)
		{
			SetHeaderPalElement( pHdr, i, pImage->GetPaletteRGB( i));
		}
	}

	pHdr->byVMode  = 0;		// monitor (usually ignored)
	pHdr->wPalInfo = 1;		// 1 if color or b\w; 2 if grayscale

	for (i = 0; i < PCXFILLER; i++)
		pHdr->byFiller[i] = 0;		// the remainder of the 128 byte hdr isn't used
}


/////////////////////////////////////////////////////////////////////////////
// Name:	FillRawBuf
// Action:	Fills a buffer with raw data from the file
//
// Parameters:	iImageFile:	The image file
//				pRawBuff:	destination buffer
//				iBuffSize:	how much data to read
//
// Returns:	Pointer to data if successful, else NULL
/////////////////////////////////////////////////////////////////////////////
static BYTE* FillRawBuf( int iImageFile, BYTE* pRawBuff, int iBuffSize)
{
	if (_lread( iImageFile, pRawBuff, iBuffSize))
		return (pRawBuff);
	else
		return (NULL);
}


/////////////////////////////////////////////////////////////////////////////
// Name:	SavePcx8Bit
// Action:	Saves a PCX image file
//
// Parameters:	hWnd:			Parent window (so we can show an error box)
//				pImage:			Image to save
//				piImageFile:	Buffered file handle
//				pszName:		Name of file to save
//				pRect:			Rectangular portion to save
//				pReOpenBuff:	OpenFile buffer
//
// Returns:	TRUE for success, else FALSE
/////////////////////////////////////////////////////////////////////////////
static BOOL SavePcx8Bit( HWND hWnd, CImageHelper* pImage, 
		PINT piImageFile, LPSTR pszName, RECT* pRect, OFSTRUCT *pReOpenBuff)
{
	int       t, nbuftotal, nypos;
	PCXHEADER hdr;					// pcx file header
	HANDLE    hband    = 0;			// small graphics bitmap
	HANDLE    hinbuf   = 0;			// zruncomp input buffer
	LPSTR     lpinbuf  = NULL;		// locked pointer for same
	LPSTR     lpoutbuf = NULL;      // output of zruncomp 
	HANDLE    houtbuf  = 0;
//	HDC       hbanddc  = 0;
//	HDC       himagedc = 0;
	int       nlastline;
	int       bytelen;					// uncompressed scanline size in bytes
	int       bandline;
	BOOL      bResult = FALSE;
	int       noutsize;
	int       ninheight;
	int       nplaneoffset;
	int       ninsize = 0;
	WORD      wxfer;
	//int     npercent = 0;
	int       iErrorMsg = 0;
	int       nlinelength;
	HANDLE    hsafetybuff = 0;
	BYTE*     lpsafetybuff = NULL;
	BOOL      bIsBW;

	int iHres = 1 + pRect->right - pRect->left;
	int iVres = 1 + pRect->bottom - pRect->top;

	// input length for getbitmapbits
	nlinelength = CalcBytesPerLine( iHres, pImage->GetNumBitsPerPixelPlane());

	// create image file header
	SetupPcxHeader( &hdr, pImage, iHres, iVres);		// above

	wxfer = _lwrite( *piImageFile, (PSTR )&hdr, sizeof( PCXHEADER));
	if (sizeof( PCXHEADER) != wxfer)
	{
		//ImageIOError( ERROR_FWRITE, pszName);
		pImage->SetErrorCode( IH_PCX_MALFORMED);
		goto SAVEPCXERROREXIT;
	}

	// uncompressed scanline byte length
	bytelen = pImage->GetNumPlanes() * hdr.wBytesPerLine * 
				((IH_24BIT == pImage->GetColorDepth()) ? 3 : 1);

	if (! (hsafetybuff = GlobalAlloc( GMEM_MOVEABLE, 16000)))
		goto FINI0;

	if (!(lpsafetybuff = (BYTE *)GlobalLock( hsafetybuff)))
		goto FINI0;

	GlobalUnlock( hsafetybuff);
	lpsafetybuff = NULL;

			/* allocate single scanline bitmap */
	ninheight = (int )(40000L / bytelen);		/* double normal */
	ninheight = min( ninheight, iVres * 2);

	bIsBW = (IH_1BIT == pImage->GetColorDepth());

	do
		{
		IfGlobalFree( houtbuf, lpoutbuf);
		IfGlobalFree( hinbuf, lpinbuf);
		IfDeleteObject( hband);
		houtbuf = hinbuf = hband = 0;
		lpoutbuf = lpinbuf = 0;

		ninheight /= 2;				/* this is the reduction */
		if (! ninheight)
			break;			/* whoops! */

		if (bIsBW)
			hband = CreateBitmap( iHres, ninheight,
					(BYTE )pImage->GetNumPlanes(),
					(BYTE )pImage->GetNumBitsPerPixelPlane(), 0L);	/* uninitialized */
		
		nplaneoffset = ninheight * nlinelength * hdr.byNumPlanes;
		if (pImage->GetNumPlanes() > 1)
			ninsize = ninheight * nlinelength * pImage->GetNumPlanes();
		else
			ninsize = ninheight * (nlinelength+1) * 
					((IH_24BIT == pImage->GetColorDepth()) ? 3 : 1);

			/* zruncomp input band buffer (single scanline) */
			/* add 16 per plane for plane rounding */
		hinbuf = GlobalAlloc( GMEM_MOVEABLE, (LONG )ninsize);

		if (hinbuf)
			lpinbuf = (char *)GlobalLock( hinbuf);

			/* zruncomp output buffer (length is a guess) */
		if (houtbuf = GlobalAlloc( GMEM_MOVEABLE, (long )(ninheight * 2) * bytelen))
			noutsize = (ninheight + 2) * bytelen;

		if (houtbuf)
			lpoutbuf = (char *)GlobalLock( houtbuf);
		}
	while (!lpoutbuf || !houtbuf || !lpinbuf || !hinbuf || (bIsBW && !hband));

	if (! ninheight)
	{
		//iErrorMsg = IDS_BANDERROR;
		iErrorMsg = IH_PCX_MALFORMED;
		goto FINI4;
	}

	// free buffers
	IfGlobalFree( hsafetybuff, lpsafetybuff);
	hsafetybuff  = 0;		// so we don't do it again
	lpsafetybuff = NULL;

	// get dc for blting image buf to band
	if (bIsBW)
	{
		// Neal - TODO - implement save as B/W sometime

		iErrorMsg = IH_UNSUPPORTED_IMAGE;
		goto FINI5;

//		hbanddc  = CreateImageDC (pImage->GetNumBitsPerPixelPlane(), pImage->GetNumPlanes());
//		himagedc = CreateImageDC( PbEnviron.pImage->GetNumBitsPerPixelPlane(),
//						 	PbEnviron.pImage->GetNumPlanes());
//
//		if ( !hbanddc || !himagedc)
//		{
//			//iErrorMsg = IDS_OUTBUFERROR;
//			iErrorMsg = IH_OUT_OF_MEMORY;
//			goto FINI5;
//		}
//		SelectObject( hbanddc, hband);
	}

	nlastline = iVres - 1;

	//PaintDoneInfoLine( 0, pszName, FILEsave);

	nbuftotal = 0;
	for (nypos = bandline = 0; nypos <= nlastline; nypos += ninheight)
	{
		ninheight = min( ninheight, 1 + nlastline - nypos);

		// fill band buffer from image buffer

		switch ( pImage->GetColorDepth())
		{
//			case IH_1BIT:
///				bResult = VirtBitBltFrom( hbanddc, 0, 0, iHres, ninheight,
//						pImage->hBitHandle, himagedc, pRect->left, pRect->top + nypos,
//						SRCCOPY);
//				// transfer band buf to zruncomp input buf
//				if ( bResult)
//					bResult = (0L != GetBitmapBits( hband, (long )ninsize, lpinbuf));
//				break;

			case IH_8BIT:
//				bResult = VirtGetPelArray( pImage->hBitHandle,
//						0, pRect->left, (WORD )(pRect->top + nypos),	
//						(WORD )(pRect->left + nlinelength - 1),
//						(WORD )(pRect->top + nypos + ninheight - 1), lpinbuf);
				lpinbuf = m_pDecodedData + ?
				break;

			case IH_24BIT:
				bResult = VirtGetLineArray( pImage->hBitHandle,
						pRect->left, pRect->top + nypos,
						pRect->left + nlinelength - 1, pRect->top + nypos + ninheight - 1,
						lpinbuf);
				break;

			default:
				ASSERT( FALSE);
				iErrorMsg = IH_UNSUPPORTED_IMAGE;
				goto FINI5;
				break;
		}

		if (! bResult)		/* we failed to get the data! */
		{
			iErrorMsg = IH_OUT_OF_MEMORY;
			goto FINISHUP;
		}
		bResult = FALSE;			/* prime for next set */

		nbuftotal = zRunComp( lpoutbuf, lpinbuf,
					ninheight, hdr.wBytesPerLine, nlinelength, 
					pImage->GetNumPlanes() * ((IH_24BIT == pImage->GetColorDepth()) ? 3 : 1),
					nlinelength);

//		if (nlastline != 0)
//			t = (int )((nypos * 100L) / nlastline);
//		else
//			t = 100;
//
//		if (t != npercent)
//		{
//			//PaintDoneInfoLine( t, pszName, FILEsave);
//			npercent = t;
//			if (!(AbortFileIO(pnImageFile, pReOpenBuff, OF_WRITE)))
//				{			/* user canceled it */
//				MessageBeep(0);
//				//SetDirty( pImage, FALSE, 0);
//				goto FINI5;
//				}
//		}

		wxfer = _lwrite( *pnImageFile, lpoutbuf, nbuftotal);
		if ((WORD)nbuftotal != wxfer)
		{
			//iErrorMsg = ERROR_DISKFULL;
			iErrorMsg = IH_ERROR_WRITING_FILE;
			goto FINI5;
		}
	}		// end line loop

	// write 256 color palette at end of file
	if (pImage->hPalette && (IH_8BIT == pImage->GetColorDepth()))
	{
		if (iErrorMsg = write_256Palette( pImage, *pnImageFile))
			goto FINISHUP;
	}

	bResult = TRUE;   // everything seems to be ok

FINISHUP:
	//PaintDoneInfoLine( 100, pszName, FILEsave);
FINI5:
	IfDeleteDC( hbanddc);
	IfDeleteDC( himagedc);
FINI4:
	IfGlobalFree( houtbuf, lpoutbuf);
	IfGlobalFree( hinbuf, lpinbuf);
	IfDeleteObject( hband);
FINI0:

	if (iErrorMsg && !bResult)
	{
		//MessageBox( hDlg, pszName, GetResourceString(iErrorMsg), MB_OK | MB_ICONEXCLAMATION);
		pImage->SetErrorCode( iErrorMsg);
	}

SAVEPCXERROREXIT:
	IfGlobalFree( hsafetybuff, lpsafetybuff);
	return (bResult);
}

/////////////////////////////////////////////////////////////////////////////
// Name:	LoadDiffPCX
// Action:	Loads a PCX image file (a different image format than 8 bit planer)
//
// Parameters:	pHdr:			PCX file header
//				hWnd:			Parent window (so we can show an error box)
//				pImage:			Image to save
//				piImageFile:	Buffered file handle
//				pszName:		Name of file to save
//				pReOpenBuff:	OpenFile buffer
//				bWarn:			Warning flag
//
// Returns:	TRUE for success, else FALSE
/////////////////////////////////////////////////////////////////////////////
static BOOL LoadDiffPCX( PCXHEADER *pHdr, HWND hWnd, CImageHelper* pImage,
						 PINT pnImageFile, PSTR pszName,
						 OFSTRUCT *pReOpenBuff, BOOL bWarn)
{
int t, j;
PSTR psz;
PSTR pszend;
HANDLE hinbuf = 0;	/* input decoding buffer */
LPSTR lpinbuf = NULL;/* locked pointers for same */
LPSTR lpoutbuf = NULL;	/* associated data pointer */
HANDLE hband = 0;		/* small graphics bitmap */
BITMAP bitmap;			/* struct for small bitmap */
HANDLE hrawbuf = 0;	/* "buffered" file i/o */
PSTR pszrawbuf = NULL;	/* locked pointer for same */
HDC hbanddc = 0, himageDC = 0;
WORD wxsize, wysize, wplanes, wbits;	/* specs of new image buffer */
int nbtcnt;
int ntotalc;
WORD wypos, wbufline;
int ndecodelines, nbufsize;
int npercent = 0;
BOOL bResult = FALSE, bspeedup = FALSE;
int iErrorMsg = 0;

	ndecodelines = 1;		/* easiest case to handle, and most appropriate */

		/* find size of newly created image  - 
		   depth = either monochrome or match the display */
	wxsize = 1 + pHdr->wX2 - pHdr->wX1;
	wysize = 1 + pHdr->wY2 - pHdr->wY1;

	wbits   = pHdr->byBitsPerPixelPlane;
	wplanes = pHdr->byNumPlanes;

	if ((8 == wbits) && (1 == wplanes))
		bspeedup = TRUE;

			/* allocate small band bitmap */
	hband = CreateBitmap(wxsize, ndecodelines, 1, 8, 0L);	/* uninitialized */
	if (!hband)
		{		// leave image allocated, but empty
		//iErrorMsg = IDS_BANDERROR;
		iErrorMsg = IH_PCX_MALFORMED;
		goto DONEREADING;
		}

	// raw file read buffer
	nbufsize = ENCODEBUFSIZE;		  /* *** THIS SHOULD BE MIN( XSIZE, ENCODEBUFSIZE) */
	while (!(hrawbuf = LocalAlloc( LMEM_MOVEABLE, nbufsize)))
		{
		nbufsize /= 2;
		if (nbufsize <= 20)
			{
			iErrorMsg = IH_OUT_OF_MEMORY;
			goto PCXERR1;
			}
		}
	if (!(pszrawbuf = LocalLock( hrawbuf)))
			{
			iErrorMsg = IH_OUT_OF_MEMORY;
			goto PCXERR1;
			}
	pszend = pszrawbuf + nbufsize;		// where the buffer ends

	// get specs of band bitmap, to match it
	GetObject( hband, sizeof(BITMAP), (LPSTR )&bitmap);

	// decoding buffer. add 64 for scanline encoding wrap
	wypos = pHdr->wBytesPerLine * pHdr->byNumPlanes * ndecodelines + 64;

	if (! (hinbuf = GlobalAlloc( GMEM_MOVEABLE,
						(long )(wypos + bitmap.bmWidthBytes * ndecodelines))))
		{
		iErrorMsg = IDS_INBUFERROR;
		goto PCXERR1;
		}

			/* lock buffers */
	if (!(lpinbuf = GlobalLock( hinbuf)))
		{
		iErrorMsg = IDS_DECBUFERROR;
		goto PCXERR2;
		}

	lpoutbuf = lpinbuf + wypos;	/* outbuf buffer for combining */

			/* input number of bytes per scan line */
	ntotalc = pHdr->wBytesPerLine * pHdr->byNumPlanes;	/* total raw bytes */

			/* get dc's for blting band to image buf */
	himageDC = CreateImageDC( 8, 1);
	if ( !himageDC)
		{
		iErrorMsg = IDS_DECBUFERROR;
		goto PCXERR3;
		}
	hbanddc = CreateCompatibleDC( himageDC);

	if ( !hbanddc)
		{
		iErrorMsg = IDS_DECBUFERROR;
		goto PCXERR3;
		}

	SelectObject (hbanddc, hband);

	//PaintDoneInfoLine( 0, pszName, FILEopen);
	nbtcnt = wypos = wbufline = 0;
	psz = pszend;		/* so will read first time */

	while (wypos < wysize) 
		{
		if (psz >= pszend)
			if (!(psz = fill_RawBuf( *pnImageFile, pszrawbuf, nbufsize)))
				{
				iErrorMsg = IH_OUT_OF_MEMORY;
				goto PCXERR3;
				}
		if (0xc0 == (0xc0 & (t = *psz++)))
			{		/* it was a count, so read the data */
			j = 0x3f & t;
			if (psz >= pszend)
				if (!(psz = fill_RawBuf( *pnImageFile, pszrawbuf, nbufsize)))
					{
					iErrorMsg = IH_OUT_OF_MEMORY;
					goto PCXERR3;
					}
			t = *psz++;
			}
		else
			j = 1;

		if ((j > 2) && ((j + nbtcnt) <= ntotalc))
			{
			FillMemory( lpinbuf + nbtcnt, j, (BYTE )t);
			nbtcnt += j;
			}
		else
			while (j--)
				*(lpinbuf + (nbtcnt++)) = (BYTE )t;

		if (nbtcnt >= ntotalc)
			{		/* now the data is in scan line(s), and we need to transfer
						it to the image */
			t = (int )((wypos * 100L) / wysize);
			if (t != npercent)
				{
				//PaintDoneInfoLine( t, pszName, FILEopen);
				npercent = t;
			if (!(AbortFileIO(pnImageFile, pReOpenBuff, OF_READ)))
					{			/* user canceled it */
					MessageBeep(0);
					goto PCXERR3;
					}
				}
			ConvertToPelArray(lpoutbuf, lpinbuf, 0, bitmap.bmWidthBytes,
				ndecodelines, pHdr->wBytesPerLine, pHdr->byNumPlanes, pHdr->byBitsPerPixelPlane);
					/* transfer input buf to small band buf */
//			if (bspeedup)
				{
					/* next line speedup for 8 bit 1 plane only */
				if (!VirtPutPelArray( pImage->hBitHandle, himageDC, 0, wypos,
						wxsize-1, wypos + ndecodelines-1, lpoutbuf, wxsize))
					{
					iErrorMsg = IH_OUT_OF_MEMORY;
					goto PCXERR3;
					}
				}
			wypos += ndecodelines;
			if (nbtcnt == ntotalc)
				nbtcnt = 0;		/* don't reset to 0 if wrapped, need it below */
			if(nbtcnt > ntotalc)
				{		/* special situation of scanline wrap */
						/* preset the count, move data from overflow area to beginning */
				nbtcnt -= ntotalc;
				RepeatMove( lpinbuf, lpinbuf+ntotalc, nbtcnt);
				}
			}
		}	/* end char decode loop */

				/* more full lines remaining in output buffer */
		if (nbtcnt && (wypos < wysize))
			{
			wysize = nbtcnt / (wplanes * bitmap.bmWidthBytes);

			ConvertToPelArray(lpoutbuf, lpinbuf, 0, bitmap.bmWidthBytes,
				ndecodelines, pHdr->wBytesPerLine, pHdr->byNumPlanes, pHdr->byBitsPerPixelPlane);

			if (!VirtPutPelArray( pImage->hBitHandle, himageDC, 0, wypos,
						wxsize-1, wypos + wysize - 1, lpoutbuf, wxsize))
				{
				iErrorMsg = IH_OUT_OF_MEMORY;
				goto PCXERR3;
				}

			wypos += wbufline+1;
			wbufline = 0;
			}

	bResult = TRUE;

	//PaintDoneInfoLine( 100, pszName, FILEopen);

PCXERR3:
	IfDeleteDC(hbanddc);
	IfDeleteDC(himageDC);
	GlobalUnlock( hinbuf);
PCXERR2:
	GlobalFree(hinbuf);
PCXERR1:
	IfLocalFree( hrawbuf, pszrawbuf);
	DeleteObject(hband);

DONEREADING:
	if ( iErrorMsg && ! bResult && bWarn)
		MessageBox (hDlg, pszName, GetResourceString( iErrorMsg),	MB_OK | MB_ICONEXCLAMATION);

	return(bResult);		/* success */
}

/////////////////////////////////////////////////////////////////////////////
// Name:	LoadSamePCX
//
// Action:	Loads a PCX image file (8 bit planer)
//
//Parameters:	pHdr:			PCX file header
//				hWnd:			Parent window - so we can bring up an error box
//				pImage:			Image to load
//				piImageFile:	Buffered file handle
//				pszName:		Name of file to save
//				pReOpenBuff:	OpenFile buffer
//				b24Bit:			is it a 24 bit image?
//				bWarn:			Warning flag
//
// Returns:	TRUE for success, else FALSE
/////////////////////////////////////////////////////////////////////////////
static BOOL LoadSamePCX( PCXHEADER *pHdr, HWND hWnd, CImageHelper* pImage,
						int* piImageFile, LPTSTR pszName,
						OFSTRUCT *pReOpenBuff, BOOL b24Bit,
						BOOL bWarn)
{
int t, j;
PSTR psz;
HANDLE hinbuf = 0;			/* input decoding buffer */
LPSTR lpinbuf = NULL;/* locked pointers for same */
HANDLE hband = 0;			/* small graphics bitmap */
BITMAP bitmap;			/* struct for small bitmap */
HANDLE hrawbuf = 0;/* "buffered" file i/o */
PSTR pszrawbuf = NULL;	/* locked pointer for same */
HDC hbanddc = 0, himagedc = 0;
WORD wxsize, wysize, wplanes, wbits;	/* specs of new image buffer */
int nbtcnt;
int ntotalc;
int ntotalout;
WORD wypos, wbufline;
WORD wdecodelines;
int nbufsize;
int npercent = 0;
BOOL bResult = FALSE;
BOOL bspeedup;
BOOL bIsBW;
int iErrorMsg = 0;
WORD wblock;
int navail; 			/* bytes in input buffer */
WORD wytotal;
int nate, nbuilt;

	wdecodelines = 1;		/* easiest case to handle, and most appropriate */

		/* find size of newly created image  - 
		   depth = either monochrome or match the display */
	wxsize = 1 + pHdr->wX2 - pHdr->wX1;
	wysize = 1 + pHdr->wY2 - pHdr->wY1;

	wbits = pHdr->byBitsPerPixelPlane;
	wplanes = pHdr->byNumPlanes;
	bspeedup = ((8==wbits && 1==wplanes ) || b24Bit);
	bIsBW = ( 1 == wbits && 1 == wplanes);

					/* allocate small band bitmap */
	wdecodelines = 30000 / max( 1, ((wbits*wxsize*wplanes)/8));
	wdecodelines = min( wdecodelines, wysize);

	if (bIsBW)
		{
		do
			{
			hband = CreateBitmap(wxsize, wdecodelines, (BYTE)wplanes, (BYTE)wbits, 0L);	/* uninitialized */
			if (! hband)
				wdecodelines /= 2;
			}
	  	while ( (wdecodelines > 0) && !hband);

		if (! hband)
			{		/* leave image allocated, but empty */
			iErrorMsg = NOMEM_BANDERROR;
			goto DONEREADING;
			}
				/* get specs of band bitmap, to match it */
		GetObject( hband, sizeof(BITMAP), (LPSTR )&bitmap);
		}
	else
		{
		bitmap.bmWidthBytes = pHdr->wBytesPerLine;
		hband = 0;
		}

			/* raw file read buffer */
	nbufsize = ENCODEBUFSIZE;
	while (!(hrawbuf = LocalAlloc( LMEM_MOVEABLE, nbufsize)))
		{
		nbufsize /= 2;
		if (nbufsize <= 20)
			{
			iErrorMsg = NOMEM_INBUFLALLOC;
			goto PCXERR1;
			}
		}
	if (!(pszrawbuf = LocalLock( hrawbuf)))
		{
		iErrorMsg = NOMEM_INBUFLLOCK;
		goto PCXERR1;
		}
			/* output buffer */
	ntotalout = bitmap.bmWidthBytes * wplanes;
	ntotalc = pHdr->wBytesPerLine * pHdr->byNumPlanes;	/* total raw bytes */

			/* decoding buffer. add 64 for scanline encoding wrap */
	if (! (hinbuf = GlobalAlloc( GMEM_MOVEABLE,
							wdecodelines*(DWORD )max(ntotalc, ntotalout) + 64) ))
		{
		//iErrorMsg = NOMEM_DECBUFGLOCK;
		iErrorMsg = IH_OUT_OF_MEMORY;
		goto PCXERR1;
		}
			/* lock buffers */
	if (!(lpinbuf = GlobalLock( hinbuf)))
		{
		iErrorMsg = IH_OUT_OF_MEMORY;
		goto PCXERR2;
		}

	if (bIsBW)
		{				/* get dc's for blting band to image buf */
		hbanddc = CreateImageDC (wbits, wplanes);		/* for band bitmap */
		himagedc = CreateImageDC( PbEnviron.pImage->GetNumBitsPerPixelPlane(),
						 	PbEnviron.pImage->GetNumPlanes());

		if (! hbanddc || !himagedc)
			{
			//iErrorMsg = NOMEM_DECBUFERROR;
			iErrorMsg = IH_OUT_OF_MEMORY;
			goto PCXERR3;
			}
		SetTextColor( himagedc, RGB( 0, 0, 0));
		SetBkColor( himagedc, RGB( 255, 255, 255));

		SelectObject( hbanddc, hband);
		}
	else hbanddc = himagedc = 0;

	//PaintDoneInfoLine( 0, pszName, FILEopen);
	nbtcnt = wypos = wbufline = 0;
	psz = pszrawbuf;		/* so will read first time */

	wblock = min( wdecodelines, wysize);
	for ( wytotal = navail = 0; wypos < wysize; )
		{
		nbuilt = 0;
reread:
		UncompressPCX( lpinbuf + wytotal * ntotalout + nbuilt, psz,
					ntotalc - nbuilt, navail, &j, &nate);
		nbuilt += j;
		navail -= nate; 	/* available input minus amt eaten by uncomp */
		psz += nate;

		if ( nbuilt != ntotalc)
			{			/* didn't get enough input data */
			if ( navail == nbufsize)
				{
				//iErrorMsg = NOMEM_PB4NOMEM;
				iErrorMsg = IH_OUT_OF_MEMORY;
				goto PCXERR3;
				}
			if ( navail > 0)		/* move previous data down in buffer */
				memcpy( pszrawbuf, psz, navail);
									/* read some more data in */
			if (!(t = _lread( *pnImageFile, pszrawbuf + navail, nbufsize - navail)))
				{
				iErrorMsg = IH_OUT_OF_MEMORY;
				goto PCXERR3;
				}
			navail += t;
			psz = pszrawbuf;
			goto reread;
			}

		wytotal++;			/* got another one */

		if (wytotal == wblock)		/* filled enough scan lines */
			{						/* transfer it to the image */
			t = (int )((wypos * 100L) / wysize);
			if (t != npercent)
				{
				//PaintDoneInfoLine( t, pszName, FILEopen);
				npercent = t;
				if (!(AbortFileIO(pnImageFile, pReOpenBuff, OF_READ)))
					{			/* user canceled it */
					MessageBeep(0);
					goto PCXERR3;
					}
				}

			if (bspeedup)
				{
				if (b24Bit)
					{			/* process 24 bit as R,G,B lines */
					if (!VirtPutLineArray( pImage->hBitHandle, 0, wypos, wxsize - 1,
						wypos + wytotal - 1, lpinbuf, pHdr->wBytesPerLine))
						{
						//iErrorMsg = NOMEM_PB4NOMEM;
						iErrorMsg = IH_OUT_OF_MEMORY;
						goto PCXERR3;
						}
					}
				else
					{				/* next line speedup for 8 bit 1 plane only */
					if (!VirtPutPelArray( pImage->hBitHandle, 0, 0, wypos,
							wxsize-1, wypos + wytotal - 1, lpinbuf, ntotalout))
						{
						iErrorMsg = IH_OUT_OF_MEMORY;
						goto PCXERR3;
						}
					}
				}
			else
				{
				if (!SetBitmapBits(hband, wytotal * (DWORD )ntotalout, lpinbuf))
					{
					iErrorMsg = IH_OUT_OF_MEMORY;
					goto PCXERR3;
					}
				if (!VirtBitBltTo (pImage->hBitHandle, himagedc,
							0, wypos, wxsize, wytotal,
						  hbanddc, 0, 0, SRCCOPY))
					{
					iErrorMsg = IH_OUT_OF_MEMORY;
					goto PCXERR3;
					}
				}
			wypos += wytotal;
			wblock = min( wdecodelines, wysize - wypos);
			wytotal = 0;
			}
		}	/* end char decode loop */

	bResult = TRUE;

	//PaintDoneInfoLine( 100, pszName, FILEopen);

PCXERR3:
	IfDeleteDC(hbanddc);
	IfDeleteDC(himagedc);
PCXERR2:
	IfGlobalFree(hinbuf, lpinbuf);
PCXERR1:
	IfLocalFree( hrawbuf, pszrawbuf);
	IfDeleteObject(hband);

DONEREADING:
	if ( iErrorMsg && ! bResult && bWarn)
		OutOfMemIndex( iErrorMsg);  /* RC */

	return(bResult);		/* success */
}

/////////////////////////////////////////////////////////////////////////////
// Name:	Write256Palette
// Action:	Writes a 256 color palette (used only if 256 color or gray scale image)
//
// Parameters:	pImage: Image we are saving
//				iImageFile: Buffered file handle
//
// Returns:	Error string index if error, else 0
/////////////////////////////////////////////////////////////////////////////
static int Write256Palette( CImageHelper* pImage, int iImageFile)
{
	int i;
	PALETTEENTRY scolor;

	struct
	{
		BYTE cpassword;
		BYTE cpaldata[256][3];
	} pcxpal;

	// write a byte flag to indicate palette exists

	pcxpal.cpassword = PAL256KEYWORD;

	for (i = 0; i < 256; i++)
		{
		GetZPalEntry( pImage->hPalette, i, &scolor);
		pcxpal.cpaldata[i][0] = scolor.peRed;
		pcxpal.cpaldata[i][1] = scolor.peGreen;
		pcxpal.cpaldata[i][2] = scolor.peBlue;
		}

	// make sure we're really at end of file

	_llseek( nImageFile, 0L, 2 /*SEEK_END*/);
	
	if (! _lwrite( nImageFile, (PSTR )&pcxpal, sizeof(pcxpal)))
	{
			//return (ERROR_DISKFULL);
			return (IH_ERROR_WRITING_FILE);
	}
	return (0);		// success
}

//Neal - need to implement this if we ever want to save non-byte oriented formats (16 color, etc.)

BOOL FAR PASCAL SavePcx16( HWND hWnd, CImageHelper* pImage, PINT pnImageFile,
									  PSTR pszFilename, PRECT pRect, OFSTRUCT *pOf )
{
/* 
int i;
int ni;
int nwidth, nheight;
int nlinelength;
int nbuftotal;
long loffset;
int ncomp;
BOOL bsaveok = FALSE;
WORD wscanline;
LPSTR lpunc = NULL;
LPSTR lpcmp = NULL;
HVIRTBIT hv16, hvsave=0;
HPALETTE hpal, hpalsave=0;
PCXHEADER ph;
VIRTBITACCESS vb;

	memset( &vb, 0, sizeof( VIRTBITACCESS));

	nwidth = pRect->right - pRect->left + 1;
	nheight = pRect->bottom - pRect->top + 1;

	hv16 = CreateVirtBitmap( nwidth, nheight, 1, 8, NULL);
	if (IH_24BIT == pImage->GetColorDepth())
		hpal = Create16ColorPalette( 16);

	if (!hv16 || !hpal)
		goto ENDSAVE;

	ConvertBitmapToBitmap( hv16, 0, 0, 0, nwidth,
		nheight, pImage->hBitHandle, 0, pRect->left, pRect->top, pImage->hPalette,
		hpal, &PbDither[DITH_IMPORT]);

	hvsave = pImage->hBitHandle;
	hpalsave = pImage->hPalette;
	pImage->hBitHandle = hv16;
	pImage->hPalette = hpal;

	SetRect( pRect, 0, 0, nwidth - 1, nheight - 1);
	
	ph.byManufacturer = 10;
	ph.byHardware = 5;
	ph.byEncoding = 1;
	ph.byBitsPerPixelPlane = 1;
	ph.X1 = ph.Y1 = 0;
	ph.X2 = nwidth - 1;
	ph.Y2 = nheight - 1;
	ph.wHdpi = (WORD )pImage->wOrigHRes;		// dpi of image (creating device)
	ph.wVdpi = (WORD )pImage->wOrigVRes;
	for (i = 0; i < 16; i++)
		set_HdrPalElement( &ph, i, GetZPalColor( pImage->hPalette, i));
	ph.Vmode   = 0;
	ph.byNumPlanes  = 4;
	ph.wBytesPerLine   = CalcBytesPerLine( nwidth, 1);
	ph.Palinfo = 1;
	ph.Hscreensize = (WORD )GetDevCaps( HORZRES);		// screen size
	ph.Vscreensize = (WORD )GetDevCaps( VERTRES);
	RepeatFill( ph.Xtra, 0, PCXFILLER);

	if (sizeof( PCXHEADER) !=
			_lwrite( *pnImageFile, (PSTR )&ph, sizeof( PCXHEADER)))
	{
		//ImageIOError( ERROR_FWRITE, pszFilename);
		pImage->SetErrorCode( IH_ERROR_WRITING_FILE);
		goto ENDSAVE;
	}

	if (OpenVirtBitmap( &vb, pImage->hBitHandle, VBIT_READ))
		goto ENDSAVE;

	wscanline = ((nwidth + 7) / 8) * 4;
	nlinelength = wscanline / 4;

	lpunc = GlobalAllocPtr( GMEM_MOVEABLE, ph.wBytesPerLine * 4);
	if (!lpunc)
		goto ENDSAVE;
	ncomp = 4000l + ph.wBytesPerLine;
	lpcmp = GlobalAllocPtr( GMEM_MOVEABLE, ncomp);
	if (!lpcmp)
		goto ENDSAVE;


	for (nbuftotal = ni = 0; ni < nheight; ni++)
		{
		if (GetVirtBitmapPointer( &vb, 0, ni, 0))
			goto ENDSAVE;
		loffset = (long )ni * ph.wBytesPerLine;

		Splitto1By4( lpunc, vb.lpBits, vb.bmWidthBytes, ph.wBytesPerLine);

		nbuftotal += zRunComp( lpcmp + nbuftotal,
				lpunc, 1, ph.wBytesPerLine, ph.wBytesPerLine * 4, 4, ph.wBytesPerLine);

		if ( ((ncomp - nbuftotal) < 2 * nlinelength * 4) ||
				ni == (nheight - 1))
			{
			if (nbuftotal != _lwrite( *pnImageFile, lpcmp, nbuftotal))
				{
				//ImageIOError( ERROR_FWRITE, pszFilename);
				pImage->SetErrorCode( IH_ERROR_WRITING_FILE);
				goto ENDSAVE;
				}
			nbuftotal = 0;
			}
		}

	bsaveok = TRUE;

ENDSAVE:

	CloseVirtBitmap( &vb);
	FreeZPal( hpal);
	IfDeleteVirtBitmap( hv16);
	pImage->hBitHandle = hvsave;
	pImage->hPalette = hpalsave;
	if (lpunc)
		GlobalFreePtr( lpunc);
	if (lpcmp)
		GlobalFreePtr( lpcmp);

	return (bsaveok);
*/
	ASSERT( FALSE);
	return (FALSE);
}


// ----------- Global Functions -----------


/*-DOC----------------------------------------------------------------------
Procedure Name: LoadPcx

Action: Loads a PCX image file (8 bit planer)

Parameters: hDlg:				Parent window - so we can bring up an error box
				pImage:			Image to load
				pnImageFile:	Buffered file handle
				pReOpenBuff:	OpenFile buffer
				bWarn:			Warning flag

Returns: TRUE for success, else FALSE
---ENDDOC-----------------------------------------------------------------*/

BOOL FAR PASCAL LoadPcx( HWND hDlg, CImageHelper* pImage, PINT pnImageFile,
								 OFSTRUCT *pReOpenBuff, BOOL bWarn)
{
int			t, j;
int			nc;
PCXHEADER	hdr;		/* pcx file header */
WORD			wxsize, wysize, wplanes, wbits;	/* specs of new image buffer */
BOOL			bsimilar;	/* true if the buffer agrees with screen */
long			loldpos=0;
HDC			hdc;
PALETTEENTRY scolor;
PSTR			pszname;
BOOL			b24bit = FALSE;

//char winpal16[48] = {0,0,0,		0,0,128,			0,128,0,			0,128,128,
//						128,0,0,			128,0,128,		128,128,0,		128,128,128,
//						192,192,192,	0,0,255,			0,255,0,			0,255,255,
//						255,0,0,			255,0,255,		255,255,0,		255,255,255};
//char pal16[48] = {0,0,0,			128,0,0,			0,128,0,			128,128,0,
//						0,0,128,			128,0,128,		0,128,128,		192,192,192,
//						128,128,128,	255,0,0,			0,255,0,			255,255,0,
//						0,0,255,			255,0,255,		0,255,255,		255,255,255};

	pszname = pImage->szImageName;

		/* it will use a smaller size if that's not available */
	if (!(AbortFileIO(pnImageFile, pReOpenBuff, OF_READ)))
		{			/* user canceled it */
		MessageBeep(0);
		goto LOADPCXERROREXIT;
		}

	if (sizeof( PCXHEADER) != _lread( *pnImageFile, (char *)&hdr, sizeof( PCXHEADER)))
		{
		if (bWarn)
		{
			//ImageIOError( ERROR_NOTPIC, pszname);
			pImage->SetErrorCode( IH_BMP_MALFORMED);
		}
		goto LOADPCXERROREXIT;
		}

	if (! valid_PCXHdr( &hdr))
		{
		if (bWarn)
		{
			//ImageIOError( ERROR_NOTPIC, pszname);
			pImage->SetErrorCode( IH_BMP_MALFORMED);
		}
		goto LOADPCXERROREXIT;
		}

		/* find size of newly created image  - 
		   depth = either monochrome or match the display */
	wxsize = hdr.wX2 - hdr.wX1 + 1;
	wysize = hdr.wY2 - hdr.wY1 + 1;

	wbits = 8;			/* until we get a real way to do this */
	wplanes=1;

	b24bit = (24 == (hdr.byNumPlanes * hdr.byBitsPerPixelPlane));

	if ( ((wbits == hdr.byBitsPerPixelPlane) && (wplanes == hdr.byNumPlanes)) || b24bit)
		bsimilar = TRUE;
	else
		bsimilar = FALSE;

	if (hdc = GetDC( PbEnviron.hWndMain) )
		{
		t = QueryImageTranslate(hdc, pImage, wxsize,
				wysize, b24bit ? 1 : hdr.byNumPlanes, hdr.byBitsPerPixelPlane, b24bit ? 3 : 1);
		ReleaseDC(PbEnviron.hWndMain, hdc);
		if (!t)
			goto LOADPCXERROREXIT;
		}

						/* *****  WRITE PALETTE FROM PCX HEADER TO IMAGE STRUCT **** */

	t = (1 << (hdr.byNumPlanes * hdr.byBitsPerPixelPlane));			/* input colors */
	if (b24bit)
		goto DONEREADPAL;

						/* seek 0 relative to current place to get position */
	loldpos = _llseek( *pnImageFile, 0L, 1 			/*SEEK_CUR*/);

	if ((hdr.byHardware != 5) || (t <= 2))			/* no palette in header or b/w */
		{
		if (t <= 2)
			{
			pImage->hPalette = CreateZPal( 2);
			scolor.peRed = scolor.peGreen = scolor.peBlue = 0;
			SetZPalEntry( pImage->hPalette, 0, scolor);
			scolor.peRed = scolor.peGreen = scolor.peBlue = 0xFF;
			SetZPalEntry( pImage->hPalette, 1, scolor);
			}
		else if (16 == t)
			{
			pImage->hPalette = Create16ColorPalette( t);
			}
		else if (hdc = GetDC( PbEnviron.hWndMain))
			{
			pImage->hPalette = CreateScreenZPal( hdc);
			ReleaseDC( PbEnviron.hWndMain, hdc);
			}
		goto DONEREADPAL;
		}

	if (t > 16)		/* header can only handle sixteen */
		{		/* seek to 256 color palette at end of file */
		if (!_llseek( *pnImageFile, (long )-((t*3)+1), 2 /*SEEK_END*/))
			goto DONEREADPAL;		/* seek failed, forget the palette */
				/* check for keyword */
		if ((!_lread( *pnImageFile, (char *)&nc, 1)) ||
							(PAL256KEYWORD != (0xFF & nc)))
				goto DONEREADPAL;
		}

	pImage->hPalette = CreateZPal( t);
	scolor.peFlags = 0;
	if (pImage->hPalette)
		{
		if (t <= 16)
			{
			for (j = 0; j < t; j++)
				{
				scolor.peRed   = hdr.byPal[j*3];
				scolor.peGreen = hdr.byPal[j*3+1];
				scolor.peBlue  = hdr.byPal[j*3+2];
				SetZPalEntry( pImage->hPalette, j, scolor);
				}
			}
		else
		for (j = 0; j < t; j++)
			{
		if (3  != _lread( *pnImageFile, (char *)&scolor.peRed, 3))
				goto LOADPCXERROREXIT;
			SetZPalEntry( pImage->hPalette, j, scolor);
			}
		}

DONEREADPAL:
	if ((t > 16) && (!b24bit))
		_llseek( *pnImageFile, loldpos, 0 /*SEEK_SET*/);

	if (bsimilar || 1 == hdr.byNumPlanes * hdr.byBitsPerPixelPlane)
		j = LoadSamePCX( &hdr, hDlg, pImage, pnImageFile, pszname, pReOpenBuff,
								b24bit, bWarn);
	else
		j = LoadDiffPCX( &hdr, hDlg, pImage, pnImageFile, pszname, pReOpenBuff,
								bWarn);

	if (! j)		// was there an error?
		return (FALSE);

//	if ( bsimilar && pImage->flags.bSetPal && pImage->hPalette)
//		InvalidateRect( PbEnviron.hWndPattern, (LPRECT )NULL, TRUE);

	// save image dpi if they are reasonable values
/*
	pImage->wOrigHRes = hdr.wHdpi;
	pImage->wOrigVRes = hdr.wVdpi;
	switch (pImage->wOrigHRes)
		{
		case 320:
			if (pImage->wOrigVRes == 200)
				{
				pImage->wOrigHRes = GetBaseResX();
				pImage->wOrigVRes = GetBaseResY();
				}
			break;
		case 640:
			if (pImage->wOrigVRes == 200 || pImage->wOrigVRes == 350 ||
				pImage->wOrigVRes == 400 || pImage->wOrigVRes == 480)
				{
				pImage->wOrigHRes = GetBaseResX();
				pImage->wOrigVRes = GetBaseResY();
				}
			break;
		case 720:
			if (pImage->wOrigVRes == 348)
				{
				pImage->wOrigHRes = GetBaseResX();
				pImage->wOrigVRes = GetBaseResY();
				}
			break;
		case 800:
			if (pImage->wOrigVRes == 600)
				{
				pImage->wOrigHRes = GetBaseResX();
				pImage->wOrigVRes = GetBaseResY();
				}
			break;
		case 1024:
			if (pImage->wOrigVRes == 768)
				{
				pImage->wOrigHRes = GetBaseResX();
				pImage->wOrigVRes = GetBaseResY();
				}
			break;
		default:
			break;
		}
*/
	return (TRUE);

LOADPCXERROREXIT:
	return (FALSE);

}


/*-DOC----------------------------------------------------------------------
Procedure Name: SavePcx

Action: Saves a PCX image file

Parameters: hDlg: Parent window - so we can bring up an error box
				pImage: Image to save
				pnImageFile: Buffered file handle
				pszName: Name of file to save
				pRect: Retangular portion to save
				pReOpenBuff: OpenFile buffer
				wFileSubType: NOT USED

Returns: TRUE for success, else FALSE
---ENDDOC-----------------------------------------------------------------*/

BOOL FAR PASCAL SavePcx( HWND hDlg, CImageHelper* pImage, PINT pnImageFile,
		PSTR pszName, PRECT pRect, OFSTRUCT *pReOpenBuff, WORD wFileSubType)
{
	if (IH_4BIT == wFileSubType)
		return (SavePcx16( hDlg, pImage, pnImageFile, pszName, pRect, pReOpenBuff));
	else
		return (SavePcx8Bit( hDlg, pImage, pnImageFile, pszName, pRect, pReOpenBuff));
}


/*-DOC----------------------------------------------------------------------
Procedure Name: ReadPCXInfo

Action: Loads a PCX image file header - checks if it is OK

Parameters: pPbImageInfo: Image info structure to fill with image data
				nFileHandle: Buffered file handle

Returns: Error code or 0 for sucess
---ENDDOC-----------------------------------------------------------------*/
/*
WORD FAR PASCAL ReadPCXInfo( ZIMAGEINFO *pPbImageInfo, int nFileHandle)
{
PCXHEADER hdr;

	if (sizeof (PCXHEADER) > _lread( nFileHandle, (LPSTR )&hdr, sizeof(PCXHEADER)))
		return (IDFERR_BADHEADER);

	if ( 10 != hdr.byManufacturer || (!( (5 == hdr.byHardware ) || (3 == hdr.byHardware))))
		return (IDFERR_BADHEADER);

	pPbImageInfo->nHeight = (WORD )(1 + hdr.Y2 - hdr.Y1);
	pPbImageInfo->nWidth = (WORD )(1 + hdr.X2 - hdr.X1);
	pPbImageInfo->nBitsPerPixel = hdr.byBitsPerPixelPlane;
	pPbImageInfo->nPlanes = hdr.Nplane;
	pPbImageInfo->wFileSubType = 0;
	strcpy( pPbImageInfo->szFormatName, GetResourceString( IDS_PCX));
	
	return (0);
}
*/
