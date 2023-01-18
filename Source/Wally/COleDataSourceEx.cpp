
/**************************************************************************
* @Doc
* @Module		COleDataSourceEx.cpp
* |				Copyright (c) 1998 - Keith Rule
*				You my freely use or modify this code provided this 
*				copyright is included in all versions. 
* @Ver      	1.0
* @Modified		Aug. 26, 1998 - Keith Rule
**************************************************************************/


#include "StdAfx.h"
#include "COleDataSourceEx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Forwards for the DIB stuff.
typedef BYTE * LPHUGEBYTE;
typedef BITMAPINFOHEADER *LPHUGEBITMAPINFOHEADER;  
typedef BITMAPCOREHEADER *LPHUGEBITMAPCOREHEADER;  
typedef BITMAPCOREINFO *LPHUGEBITMAPCOREINFO;
typedef BITMAPINFO *LPHUGEBITMAPINFO;
#define HDIB HANDLE
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))  
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

WORD DIBNumColors(LPHUGEBYTE lpDIB);
HDIB BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal);


/**************************************************************************
* Class		   	COleDataSourceEx
**************************************************************************/


///////////////////////////////////////////////////////////////////////////
// @Mfunc	Constructor
//
// @Rdesc	Not Applicable
//
COleDataSourceEx::COleDataSourceEx(
                                   COleDataSourceView* view,	// @Parm view initializer
                                   BOOL bDelayRendered)	// @Parm TRUE if delay rendered, FALSE if immediate rendered
								   : COleDataSource()
{
	m_pView             = view;
	m_bitmap            = NULL;
	m_bDelayRendered    = bDelayRendered;	
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Destructor
//
// @Rdesc	Not Applicable
//
COleDataSourceEx::~COleDataSourceEx()
{
	Cleanup();
	Empty();
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function preforms the delay render of a DIB.
//
// @Rdesc	BOOL - Returns TRUE if successful, FALSE otherwise.
//
BOOL COleDataSourceEx::DelayRenderDIB(
                                      LPFORMATETC lpFormatEtc,	// @Parm (Parameter description)
                                      LPSTGMEDIUM lpStgMedium)	// @Parm (Parameter description)
{
	try {
		lpStgMedium->tymed = TYMED_HGLOBAL;	
		lpStgMedium->hGlobal = RenderDIB();
		return TRUE;
	} catch (...) {}
	
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function forwards DIB requests to DelayRenderDIB, otherwise
//			it lets COleDataSource continue with the processing.
//
// @Rdesc	BOOL - Returns TRUE if successful, FALSE otherwise. 
//
BOOL COleDataSourceEx::OnRenderData(
                                    LPFORMATETC lpFormatEtc,	// @Parm (Parameter description)
                                    LPSTGMEDIUM lpStgMedium)	// @Parm (Parameter description)
{
	if (lpFormatEtc->cfFormat == CF_DIB) {
		return DelayRenderDIB(lpFormatEtc, lpStgMedium);
	} 
	return COleDataSource::OnRenderData(lpFormatEtc, lpStgMedium);
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Place the requested formats on the clipboard and forward
//			the DoDragDrop to COleDataSource.
//
// @Rdesc	DROPEFFECT - Returns result of COleDataSource::DoDragDrop().
//
DROPEFFECT COleDataSourceEx::DoDragDrop(
                                        DWORD dwEffects,	// @Parm Same as COleDataSource::DoDragDrop()
                                        LPCRECT lpRectStartDrag,	// @Parm Same as COleDataSource::DoDragDrop()
                                        COleDropSource* pDropSource) // @Parm Same as COleDataSource::DoDragDrop()
{
	InstantiateFormats();
	DROPEFFECT retval = COleDataSource::DoDragDrop(dwEffects, lpRectStartDrag, pDropSource);
	Cleanup();
	return retval;
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Internal Function which handles text and custom formats.
//
// @Rdesc	BOOL - Returns TRUE of data was rendered.
//
BOOL COleDataSourceEx::OnRenderFileData(
										LPFORMATETC lpFormatEtc, // @Parm (Parameter description)
										CFile * pFile) // @Parm (Parameter description)
{
	try {
		if (lpFormatEtc->cfFormat == ::RegisterClipboardFormat("Csv")) {
			return m_pView->WriteCsvFormat(pFile);
		} else if (lpFormatEtc->cfFormat == CF_TEXT) {
			return m_pView->WriteTextFormat(pFile);
		} else if (lpFormatEtc->cfFormat == ::RegisterClipboardFormat(CF_RTF)) {
			return m_pView->WriteRtfFormat(pFile);
		} else {
			return m_pView->WriteCustomFormat(lpFormatEtc, pFile);
		}
	} catch (...) {}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Renders the DIB format into an HGLOBAL. Used for both immediate
//			and delay rendering of the client area.
//
// @Rdesc	HGLOBAL - Returns handle to rendered DIB
//
HGLOBAL COleDataSourceEx::RenderDIB()
{
	CView*	pView = m_pView->GetView();
	if (m_bitmap != NULL) delete m_bitmap;
	m_bitmap = new CBitmap;
		
	ASSERT(pView != NULL && m_bitmap != NULL);
	CClientDC cdc(pView);	
	CDC dc;
	dc.CreateCompatibleDC(&cdc);	
				
	CRect bounds = m_pView->GetBounds();
		
	m_bitmap->CreateCompatibleBitmap(&cdc, bounds.Width(), bounds.Height());
	CBitmap* oldBitmap = dc.SelectObject(m_bitmap);	
		
	m_pView->DoPrepareDC(&dc);
		
	CBrush fill(RGB(255, 255, 255));
	dc.FillRect(bounds, &fill);	
		
	m_pView->DoDraw(&dc);	
		
	HBITMAP hBitmap = HBITMAP(*m_bitmap);
	CPalette *palette = new CPalette;
	LOGPALETTE LogPalette;
	LogPalette.palVersion = 0x300;
	LogPalette.palNumEntries = 1;
	LogPalette.palPalEntry[0].peRed = 255;
	LogPalette.palPalEntry[0].peGreen = 255;
	LogPalette.palPalEntry[0].peBlue = 255;
	LogPalette.palPalEntry[0].peFlags = PC_NOCOLLAPSE;
	if(!palette->CreatePalette( &LogPalette ))
			return NULL;
	HPALETTE hPal = HPALETTE(*palette);
	HANDLE hDIB = ::BitmapToDIB(hBitmap, hPal);
		
	dc.SelectObject(oldBitmap);
	
	return hDIB;	
}



#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////
// @Mfunc	Checks for validity of object instance.
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::AssertValid() const
{
	COleDataSource::AssertValid();
}
#endif //_DEBUG



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Removes temporary data
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::Cleanup()
{
	if (m_bitmap != NULL) {
		delete m_bitmap;
		m_bitmap = NULL;
	}
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places CSV format on the clipboard. Data is rendered during
//			immediate rendering. The data rendering is deferred with delay
//			rendering. The type of rendering is specified in the constructor.
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::CsvFormat()
{
	try {
		if (m_pView != NULL && (m_pView->CsvStatus() & FORMAT_SET)) {
			UINT	csvFormat = ::RegisterClipboardFormat("Csv");
			if (m_bDelayRendered) {
				DelayRenderFileData(csvFormat);
			} else {
				CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
				if (m_pView->WriteCsvFormat(&sf) && sf.GetLength() > 0) {
					CacheGlobalData(csvFormat, sf.Detach());
				}
			}
		}
	} catch (...) {}
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Specifies a format ID for a developer supported custom format.
//			More than one custom format may be specified by calling this 
//			function several times.
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::EnableCustomFormat(UINT format)
{
	m_customFormat.Add(format);
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places Custom format on the clipboard. Data is rendered during
//			immediate rendering. The data rendering is deferred with delay
//			rendering. The type of rendering is specified in the constructor.	
//
// @Rdesc	void - 
//
void COleDataSourceEx::CustomFormat()
{
	try {
		if (m_pView != NULL) {
			for (int i = 0; i < m_customFormat.GetSize(); i++) {
				if (m_bDelayRendered) {
					DelayRenderFileData(m_customFormat[i]);
				} else {
					CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
					FORMATETC format;

					format.cfFormat = m_customFormat[i];
					format.ptd = NULL;
					format.dwAspect = DVASPECT_CONTENT;
					format.lindex = -1;
					format.tymed = TYMED_HGLOBAL;

					if (m_pView->WriteCustomFormat(&format, &sf) && sf.GetLength() > 0) {
						CacheGlobalData(m_customFormat[i], sf.Detach());
					}
				}
			}
		}
	} catch (...) {}
}
///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places DIB format on the clipboard. Data is rendered during
//			immediate rendering. The data rendering is deferred with delay
//			rendering. The type of rendering is specified in the constructor.	
	
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::DIBFormat()
{
	try {
		if (m_pView != NULL && (m_pView->DIBStatus() & FORMAT_SET)) {
			if (m_bDelayRendered) {
				DelayRenderData(CF_DIB);
			} else {
				HGLOBAL hDIB = RenderDIB();
				if (hDIB != NULL) CacheGlobalData(CF_DIB, hDIB);
			}
		}
	} catch (...) {}
}



#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////
// @Mfunc	Dumps current data. Used for debugging.
//
// @Rdesc	void - 
//
void COleDataSourceEx::Dump(
                            CDumpContext& dc) const	// @Parm (Parameter description)
{
	COleDataSource::Dump(dc);
}
#endif //_DEBUG



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places all the specified formats on the clipboard. This function
//			is called by SetClipboard() and DoDragDrop().
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::InstantiateFormats()
{
	CsvFormat();
	TextFormat();
	RtfFormat();
	DIBFormat();
	CustomFormat();
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places RTF format on the clipboard. Data is rendered during
//			immediate rendering. The data rendering is deferred with delay
//			rendering. The type of rendering is specified in the constructor.	
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::RtfFormat()
{
	try {
		if (m_pView != NULL && (m_pView->RtfStatus() & FORMAT_SET)) {
			UINT	rtfformat = ::RegisterClipboardFormat(CF_RTF);
			if (m_bDelayRendered) {
				DelayRenderFileData(rtfformat);
			} else {
				CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
				if (m_pView->WriteRtfFormat(&sf) && sf.GetLength() > 0) {
					CacheGlobalData(rtfformat, sf.Detach());
				}
			}
		}
	} catch(...) {}
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Handles Copy/Cut placement of data on the clipboard. Data transferred
//			in this manner should probably use immediate rendering.
//
// @Rdesc	void - 
//
void COleDataSourceEx::SetClipboard() 
{
	InstantiateFormats();
	COleDataSource::SetClipboard();
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Places CF_TEXT format on the clipboard. Data is rendered during
//			immediate rendering. The data rendering is deferred with delay
//			rendering. The type of rendering is specified in the constructor.	
//
// @Rdesc	void - No Return
//
void COleDataSourceEx::TextFormat()
{
	try {
		if (m_pView != NULL && (m_pView->TextStatus() & FORMAT_SET)) {
			if (m_bDelayRendered) {
				DelayRenderFileData(CF_TEXT);
			} else {
				CSharedFile sf(GMEM_MOVEABLE | GMEM_SHARE | GMEM_ZEROINIT);
				if (m_pView->WriteTextFormat(&sf) && sf.GetLength() > 0) {
					CacheGlobalData(CF_TEXT, sf.Detach());
				}
			}
		}
	} catch(...) {}
}





/**************************************************************************
* Class		   	COleDataSourceView
**************************************************************************/


///////////////////////////////////////////////////////////////////////////
// @Mfunc	This constructor arbitrarially selects the formats enabled. Derived classes should set what's approprate for them.
//
// @Rdesc	No Applicable.
//
COleDataSourceView::COleDataSourceView()
{
	// You should enable the format you support 
	// in your derived class.
	EnableDIBFormat(0);
	EnableTextFormat(0);
	EnableRtfFormat(0);
	EnableCsvFormat(0);
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Virtual Destructor
//
// @Rdesc	 No applicable
//
COleDataSourceView::~COleDataSourceView()
{
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Called when DIB format is rendered. This should be overridden
//			by the developer if something other than the ClientRect should be
//			returned.
//
// @Rdesc	CRect - The client rect.
//
CRect COleDataSourceView::GetBounds()
{
	CRect bounds(0, 0, 0, 0);

	if (GetView() != NULL) {
		GetView()->GetClientRect(&bounds);
		return bounds;
	}

	// If you enable the DIB format you 
	// must either override this function or GetView().
	ASSERT(FALSE);
	return bounds;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Returns the current CView derived class. This should be
//			overridden when DIB format is supported.
//
// @Rdesc	CView* - 
//
CView* COleDataSourceView::GetView()
{
	// If you enable the DIB format you 
	// must either override this function GetView().
	ASSERT(FALSE); 
	return NULL;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Returns whether the CSV format is supported.
//
// @Rdesc	int - a bit-field with any of FORMAT_SET, or FORMAT_EDITABLE flags set.
//
int COleDataSourceView::CsvStatus()
{
	return m_csvFormat;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Returns whether the DIB format is supported.
//
// @Rdesc	int - a bit-field with any of FORMAT_SET, or FORMAT_EDITABLE flags set.
//
int COleDataSourceView::DIBStatus()
{
	return m_DIBFormat;
}

///////////////////////////////////////////////////////////////////////////
// @Mfunc	Returns whether the RTF format is supported.
//
// @Rdesc	int - a bit-field with any of FORMAT_SET, or FORMAT_EDITABLE flags set.
//
int COleDataSourceView::RtfStatus()
{
	return m_rtfFormat;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	Returns whether the CF_TEXT format is supported.
//
// @Rdesc	int - a bit-field with any of FORMAT_SET, or FORMAT_EDITABLE flags set. 
//
int COleDataSourceView::TextStatus()
{
	return m_textFormat;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	A placeholder to be overridden by the derived view.
//			This function routes this call to the CViews OnDraw()
//			function. This is necessary because the OnDraw() function is
//			protected in MFC.
//
// @Rdesc	void - No Return
//
void COleDataSourceView::DoDraw(
                                CDC* pDC)	// @Parm See CView::OnDraw().
{
	// If you enable the DIB format you 
	// must override this function.
	ASSERT(FALSE);
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	A placeholder to be overridden by the derived view.
//			This function routes this call to the CViews DoPrepareDC()
//			function. This is necessary because the DoPrepareDC() function is
//			protected in MFC.	
//
// @Rdesc	void - 
//
void COleDataSourceView::DoPrepareDC(
                                     CDC* pDC,	// @Parm (Parameter description)
                                     CPrintInfo* pInfo)	// @Parm (Parameter description)
{
	TRACE(_T("If you enable the DIB format, you might want to consider overriding DoPrepareDC() too.\r\n"));
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function sets the status info for the CSV format.
//			The value is a bit-field with any of FORMAT_SET, and FORMAT_EDITABLE flags set.
//
// @Rdesc	void - No return.
//
void COleDataSourceView::EnableCsvFormat(
                                         int status)	// @Parm New status
{
	m_csvFormat = status;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function sets the status info for the DIB format.
//			The value is a bit-field with any of FORMAT_SET, and FORMAT_EDITABLE flags set.
//
// @Rdesc	void - No return.
//
void COleDataSourceView::EnableDIBFormat(
                                         int status)	// @Parm New status
{
	m_DIBFormat = status;
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function sets the status info for the RTF format.
//			The value is a bit-field with any of FORMAT_SET, and FORMAT_EDITABLE flags set.
//
// @Rdesc	void - No return.
//
void COleDataSourceView::EnableRtfFormat(
                                         int status)	// @Parm New status
{
	m_rtfFormat = status;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	This function sets the status info for the CF_TEXT format.
//			The value is a bit-field with any of FORMAT_SET, and FORMAT_EDITABLE flags set.
//
// @Rdesc	void - No return.
//
void COleDataSourceView::EnableTextFormat(
                                          int status)	// @Parm New status
{
	m_textFormat = status;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	The functions is intended to be overridden by the developer in
//			a derived class. This function is used to write the CSV format to
//			the clipboard.
//
// @Rdesc	BOOL - TRUE if write successful, FALSE otherwise.
//
BOOL COleDataSourceView::WriteCsvFormat(
                                        CFile* pFile)	// @Parm Target of Write.
{
	// If you enable the CSV format you 
	// must override this function.
	ASSERT(FALSE);
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	The functions is intended to be overridden by the developer in
//			a derived class. This function is used to write the RTF format to
//			the clipboard.
//
// @Rdesc	BOOL - TRUE if write successful, FALSE otherwise. 
//
BOOL COleDataSourceView::WriteRtfFormat(
                                        CFile* pFile)	// @Parm Target of Write.
{
	// If you enable the RTF format you 
	// must override this function.
	ASSERT(FALSE);
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////
// @Mfunc	The functions is intended to be overridden by the developer in
//			a derived class. This function is used to write the CF_TEXT format to
//			the clipboard.
//
// @Rdesc	BOOL - TRUE if write successful, FALSE otherwise. 
//
BOOL COleDataSourceView::WriteTextFormat(
                                         CFile* pFile)	// @Parm Target of Write.
{
	// If you enable the Text format you 
	// must override this function.
	ASSERT(FALSE);
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////
// @Mfunc	The functions is intended to be overridden by the developer in
//			a derived class. This function is used to write the Custom formats to
//			the clipboard.
//
// @Rdesc	BOOL - TRUE if write successful, FALSE otherwise. 
//
BOOL COleDataSourceView::WriteCustomFormat(
									  LPFORMATETC lpFormat, // @Parm (Parameter description)
									  CFile* file)	// @Parm (Parameter description)
{
	// If you enable a custom format you 
	// must override this function.
	ASSERT(FALSE);
	return FALSE;
}

/////////////////////////////////////////////////////////
// Additional code to support DIB's. 
//
// I didn't write this. I believe I pulled it off the 
// net. I've long since lost any credits that went with 
// it (a big I'm sorry goes to author - whoever you are). 
// So, I make to claims on the following source. 
// - KR
/////////////////////////////////////////////////////////


WORD PaletteSize(LPHUGEBYTE lpDIB)
{ 
   WORD	size; 
   
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpDIB)) {
      size = (DIBNumColors(lpDIB) * sizeof(RGBQUAD));
   } else {
      size = (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE)); 
   }

   return size;
}

WORD DIBNumColors(LPHUGEBYTE lpDIB)
{
   WORD wBitCount;  /* DIB bit count */

   /*  If this is a Windows-style DIB, the number of colors in the
    *  color table can be less than the number of bits per pixel
    *  allows for (i.e. lpbi->biClrUsed can be set to some value).
    *  If this is the case, return the appropriate value.
    */

   if (IS_WIN30_DIB(lpDIB))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
      if (dwClrUsed)
     return (WORD)dwClrUsed;
   }

   /*  Calculate the number of colors in the color table based on
    *  the number of bits per pixel for the DIB.
    */
   if (IS_WIN30_DIB(lpDIB))
      wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

   /* return number of colors based on bits per pixel */
   switch (wBitCount)
      {
   case 1:
      return 2;

   case 4:
      return 16;

   case 8:
      return 256;

   default:
      return 0;
      }
}

HDIB BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
   BITMAP bm;                   /* bitmap structure */
   BITMAPINFOHEADER bi;         /* bitmap header */
   LPHUGEBITMAPINFOHEADER  lpbi; /* pointer to BITMAPINFOHEADER */
   DWORD dwLen;                 /* size of memory block */
   HANDLE hDIB, h;              /* handle to DIB, temp handle */
   HDC hDC;                     /* handle to DC */
   WORD biBits;                 /* bits per pixel */

   /* check if bitmap handle is valid */

   if (!hBitmap)
      return NULL;

   /* fill in BITMAP structure, return NULL if it didn't work */
   if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm))
      return NULL;

   /* if no palette is specified, use default palette */
   if (hPal == NULL)
      hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

   /* calculate bits per pixel */
   biBits = (WORD)(bm.bmPlanes * bm.bmBitsPixel);

   /* make sure bits per pixel is valid */
   if (biBits <= 1)
      biBits = 1;
   else if (biBits <= 4)
      biBits = 4;
   else if (biBits <= 8)
      biBits = 8;
   else /* if greater than 8-bit, force to 24-bit */
      biBits = 24;

   /* initialize BITMAPINFOHEADER */
   bi.biSize = sizeof(BITMAPINFOHEADER);
   bi.biWidth = bm.bmWidth;
   bi.biHeight = bm.bmHeight;
   bi.biPlanes = 1;
   bi.biBitCount = biBits;
   bi.biCompression = BI_RGB;
   bi.biSizeImage = 0;
   bi.biXPelsPerMeter = 0;
   bi.biYPelsPerMeter = 0;
   bi.biClrUsed = 0;
   bi.biClrImportant = 0;

   /* calculate size of memory block required to store BITMAPINFO */
   dwLen = bi.biSize + PaletteSize((LPHUGEBYTE)&bi);

   /* get a DC */
   hDC = GetDC(NULL);

   /* select and realize our palette */
   hPal = SelectPalette(hDC, hPal, FALSE);
   RealizePalette(hDC);

   /* alloc memory block to store our bitmap */
   hDIB = GlobalAlloc(GHND, dwLen);

   /* if we couldn't get memory block */
   if (!hDIB)
   {
      /* clean up and return NULL */
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }

   /* lock memory and get pointer to it */
   //lpbi = (VOID FAR *)GlobalLock(hDIB);
   lpbi = (LPHUGEBITMAPINFOHEADER)GlobalLock(hDIB);

   /* use our bitmap info. to fill BITMAPINFOHEADER */
   *lpbi = bi;

   /*  call GetDIBits with a NULL lpBits param, so it will calculate the
    *  biSizeImage field for us
    */
   GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, NULL, (LPHUGEBITMAPINFO)lpbi,
         DIB_RGB_COLORS);

   /* get the info. returned by GetDIBits and unlock memory block */
   bi = *lpbi;
   GlobalUnlock(hDIB);

   /* if the driver did not fill in the biSizeImage field, make one up */
   if (bi.biSizeImage == 0)
      bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

   /* realloc the buffer big enough to hold all the bits */
   dwLen = bi.biSize + PaletteSize((LPHUGEBYTE)&bi) + bi.biSizeImage;  

   h = GlobalReAlloc(hDIB, dwLen, 0);
   if (h)
      hDIB = h;
   else
   {
      /* clean up and return NULL */
      GlobalFree(hDIB);
      hDIB = NULL;
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }

   /* lock memory block and get pointer to it */  

   lpbi = (LPHUGEBITMAPINFOHEADER)GlobalLock(hDIB);

   /*  call GetDIBits with a NON-NULL lpBits param, and actualy get the
    *  bits this time
    */
   if (GetDIBits(hDC, hBitmap, 0, (WORD)bi.biHeight, (LPHUGEBYTE)lpbi + (WORD)lpbi
         ->biSize + PaletteSize((LPHUGEBYTE)lpbi), (LPHUGEBITMAPINFO)lpbi,
         DIB_RGB_COLORS) == 0)
   {
      /* clean up and return NULL */
      GlobalUnlock(hDIB);
      hDIB = NULL;
      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
   }
   bi = *lpbi;

   /* clean up */
   GlobalUnlock(hDIB);
   SelectPalette(hDC, hPal, TRUE);
   RealizePalette(hDC);
   ReleaseDC(NULL, hDC);

   /* return handle to the DIB */
   return hDIB;
}


