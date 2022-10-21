// BrowseView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BrowseView.h"
#include "BrowseDoc.h"
#include "BrowserCacheList.h"
#include "DibSection.h"
#include "PrefabImages.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "PackageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBrowseView

IMPLEMENT_DYNCREATE(CBrowseView, CView)

CBrowseView::CBrowseView()
{
	m_iHorzButtons = 0;
	m_iVertButtons = 0;
	m_iNumImageRows = 0;
	m_iCurrentScrollPos = 0;
	m_iInvalidateScrollRange = 0;

	m_bInitialized = FALSE;
	m_bSized = FALSE;
	m_bScrolling = FALSE;
	m_bResetScrollbars = FALSE;
	m_bDragging	= FALSE;

	m_bCalcSizes = FALSE;

	CPrefabImage piImage;
	BYTE *pbyData = NULL;
	BYTE *pbyPalette = NULL;

	piImage.CreatePrefabImageFromID (IDR_PCX_WAD_THUMB, &pbyData, &pbyPalette);
	
	m_dsWadThumbnail.Init (piImage.GetImageWidth(), piImage.GetImageHeight(), 8, pbyPalette, TRUE);
	m_dsWadThumbnail.SetRawBits (pbyData);	

	m_ftText.CreateFont (6, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"MS Sans Serif");
}

CBrowseView::~CBrowseView()
{
	m_ftText.DeleteObject();
}


BEGIN_MESSAGE_MAP(CBrowseView, CView)
	//{{AFX_MSG_MAP(CBrowseView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()	
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_BVPOPUP_COPY, OnBvpopupCopy)
	ON_COMMAND(ID_BVPOPUP_PASTE, OnBvpopupPaste)
	ON_UPDATE_COMMAND_UI(ID_BVPOPUP_PASTE, OnUpdateBvpopupPaste)
	ON_COMMAND(ID_BVPOPUP_COPY_CLIPBOARD, OnBvpopupCopyClipboard)
	ON_COMMAND(ID_BVPOPUP_DELETE, OnBvpopupDelete)
	ON_COMMAND(ID_BVPOPUP_INFO, OnBvpopupInfo)
	ON_UPDATE_COMMAND_UI(ID_BVPOPUP_COPY, OnUpdateBvpopupCopy)
	ON_UPDATE_COMMAND_UI(ID_BVPOPUP_COPY_CLIPBOARD, OnUpdateBvpopupCopyClipboard)
	ON_UPDATE_COMMAND_UI(ID_BVPOPUP_DELETE, OnUpdateBvpopupDelete)
	ON_UPDATE_COMMAND_UI(ID_BVPOPUP_INFO, OnUpdateBvpopupInfo)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowseView drawing

void CBrowseView::OnInitialUpdate()
{
	DragAcceptFiles (true);
	m_oleDropTarget.Register (this);

	CView::OnInitialUpdate();
	CalcMaxButtons();
}

void CBrowseView::OnDraw(CDC* pDC)
{
	CBrowseDoc* pDoc = GetDocument();
//	CStringArray *pArray = pDoc->GetArray();
	
	CBrowserCacheList *pCacheList = pDoc->GetCacheList();
	CBrowserCacheItem *pItem = NULL;

	CDibSection *pDibItem = NULL;
	int iOldWidth = 0;
	int iOldHeight = 0;
	int iOldBitDepth = 0;

	int iWidth = 0;
	int iHeight = 0;
	int iBitDepth = 0;

	BYTE *pbyData = NULL;
	BYTE *pbyPalette = NULL;
	
	int j = 0;
	int k = 0;	
	int x = 0;
	int y = 0;
	int x1 = 0;
	int y1 = 0;	
	int xOffset = 0;
	int yOffset = 0;

	int iOffsetPosition = 0;
	int iCount = 0;
	int iTextX = 0;
	int iTextY = 0;
	int iTextX1 = 0;
	int iTextY1 = 0;

	CRect rButton;
	CRect rText;
	CRect rClient;
	GetClientRect( &rClient);

	int rWidth = rClient.Width();
	int rHeight = rClient.Height() + BROWSE_BUTTON_HEIGHT;
	
	CFont *pOldFont;

	CString strText("");
	BOOL bScrolling = FALSE;

	if (!m_bScrolling)
	{
		// Get the max number of buttons
		CalcMaxButtons();
	}
	else
	{
		bScrolling = TRUE;
	}

	// Erase the background

	CDC MemDC;
	CBitmap Bmp;
	
	MemDC.CreateCompatibleDC( pDC);
	Bmp.CreateCompatibleBitmap( pDC, rWidth, rHeight);
	MemDC.SelectObject( &Bmp);
	
	HBRUSH hBrush = CreateSolidBrush ( RGB (200, 181, 160));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( MemDC, hBrush);

	::PatBlt (MemDC, 0, 0, rWidth, rHeight, PATCOPY);

	SelectObject( MemDC, hOldBrush);
	DeleteObject( hBrush);

	MemDC.SetBkColor (GetSysColor (COLOR_BTNFACE));
	
	pOldFont = MemDC.SelectObject (&m_ftText);

	//  Find out where the scrollbar is at, and calculate how many "rows" down we are 
	int iRow = m_iCurrentScrollPos;

	int iStartRow = 0;
	int iEndRow = 0;

	iStartRow = bScrolling ? (m_iInvalidateScrollRange < 0 ? m_iVertButtons + m_iInvalidateScrollRange : 0) : 0;
	iEndRow = bScrolling ? (m_iInvalidateScrollRange < 0 ? m_iVertButtons + 1 : (m_iInvalidateScrollRange - 1)) : m_iVertButtons + 1;

	iStartRow = max (iStartRow, 0);
	iEndRow = min (iEndRow, m_iVertButtons + 1);

	//  Only draw the set of textures that can possibly be seen
	for (j = iStartRow; j <= iEndRow; j++)
	{
		for (k = 0; k < m_iHorzButtons; k++)
		{
			// OffsetPosition is the position of the texture we're currently at
			iOffsetPosition = (iRow * m_iHorzButtons) + (j * m_iHorzButtons) + k;

			pItem = pCacheList->GetAtPosition (iOffsetPosition);

			if (pItem)
			{
				DrawItem (pItem, &MemDC, j, k);
			}			
		}			
	}
	
	if (pDibItem)
	{
		delete pDibItem;
		pDibItem = NULL;
	}

	int rcY = 0;
	pDC->BitBlt( 0, rcY, rWidth, rHeight, &MemDC, 0, 0, SRCCOPY);


	MemDC.SelectObject (pOldFont);

	MemDC.DeleteDC();
	Bmp.DeleteObject();

	m_bScrolling = FALSE;
}

void CBrowseView::DrawItem (CBrowserCacheItem *pItem, CDC *pMemDC, int iRow, int iColumn)
{
	// Figure out the location to plot this guy... (x,y) = top left, (x1/y1) = bottom right
	int x = iColumn * BROWSE_BUTTON_WIDTH;
	int y = iRow * BROWSE_BUTTON_HEIGHT;

	int x1 = x + BROWSE_BUTTON_WIDTH - 1;
	int y1 = y + BROWSE_BUTTON_HEIGHT - 1;

	// Plop in the rectangle coords and draw the button behind the texture
	CRect rButton (x, y, x1, y1);
	CRect rSelectButton (x + 2, y + 2, x1 - 3, y1 - 3);
	pMemDC->DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH);

	COLORREF crColorBK;
	COLORREF crColorText;

	if (pItem->IsSelected())
	{
		crColorBK = pMemDC->SetBkColor ( GetSysColor (COLOR_HIGHLIGHT));
		crColorText = pMemDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT));

		//HBRUSH hBrush = CreateSolidBrush ( GetSysColor (COLOR_HIGHLIGHT));
		HBRUSH hBrush = GetSysColorBrush (COLOR_HIGHLIGHT);
		HBRUSH hOldBrush = (HBRUSH )pMemDC->SelectObject( hBrush);

		pMemDC->PatBlt (rSelectButton.left, rSelectButton.top, rSelectButton.right - rSelectButton.left, rSelectButton.bottom - rSelectButton.top, PATCOPY);

		pMemDC->SelectObject( hOldBrush);
		//DeleteObject( hBrush);
	}	

	// Draw the thumbnail
	int iWidth = pItem->GetThumbnailWidth();
	int iHeight = pItem->GetThumbnailHeight();
	int iBitDepth = 0;

	int xOffset = 0;
	int yOffset = 0;
	
	BYTE *pbyData = NULL;
	BYTE *pbyPalette = NULL;

	CDibSection DibItem;
	
	if ((iWidth == 0) || (iHeight == 0))
	{
		CImageHelper ihHelper;

		if (ihHelper.IsValidPackage (pItem->GetImageType()))
		{
			iWidth = m_dsWadThumbnail.GetWidth();
			iHeight = m_dsWadThumbnail.GetHeight();
			
			if (iWidth < BROWSE_IMAGE_WIDTH)
			{
				xOffset = (BROWSE_IMAGE_WIDTH / 2) - (iWidth / 2) + 3;
			}
			else
			{
				xOffset = 3;
			}

			if (iHeight < BROWSE_IMAGE_HEIGHT)
			{
				yOffset = (BROWSE_IMAGE_HEIGHT / 2) - (iHeight / 2) + 3;
			}
			else
			{
				yOffset = 3;
			}

			m_dsWadThumbnail.Show (pMemDC, x + xOffset, y + yOffset, 1);
		}
	}
	else
	{		
		pbyData    = pItem->GetImageData();
		pbyPalette = pItem->GetPalette();
		iBitDepth = pItem->GetBitDepth();

		// Neal - always use 24, even for 32 bit images
//		if (iBitDepth == 32)
//			iBitDepth = 24;

		if (iWidth == 94)		// TEST TEST TEST
		{
			int iBreakpoint = 0;
		}

//		if (pbyData && pbyPalette)
		if (pbyData)					// Neal - allow 24/32 bit
		{
			DibItem.Init( iWidth, iHeight, iBitDepth, pbyPalette, TRUE);
			DibItem.SetRawBits( pbyData);
		}
		else
		{
			// Neal - handle 24/32 bit

			int iSize = iHeight * PadDWORD( (iWidth * iBitDepth) / 8);

			BYTE *pbyBits = new BYTE[iSize];
			memset (pbyBits, 0, iSize);

			BYTE byPalette[768];
			memset (byPalette, 0, 768);

			DibItem.Init( iWidth, iHeight, iBitDepth, byPalette, TRUE);		
			DibItem.SetRawBits (pbyBits);

			if (pbyBits)
			{
				delete []pbyBits;
				pbyBits = NULL;
			}
		}
			
		if (iWidth < BROWSE_IMAGE_WIDTH)
		{
			xOffset = (BROWSE_IMAGE_WIDTH / 2) - (iWidth / 2) + 3;
		}
		else
		{
			xOffset = 3;
		}

		if (iHeight < BROWSE_IMAGE_HEIGHT)
		{
			yOffset = (BROWSE_IMAGE_HEIGHT / 2) - (iHeight / 2) + 3;
		}
		else
		{
			yOffset = 3;
		}
		
		DibItem.Show (pMemDC, x + xOffset, y + yOffset, 1);
	}
	
	// Set up where the text should be plotted
	int iTextX = x + 6;
	int iTextY = y + BROWSE_BUTTON_HEIGHT - 22;
	int iTextX1 = x + BROWSE_BUTTON_WIDTH - 6;
	int iTextY1 = y + BROWSE_BUTTON_HEIGHT;

	// This guy is used to clip the text, in case it's really long
	CRect rText(iTextX, iTextY, iTextX1, iTextY1);

	CString strText("");
	strText = pItem->GetFileName();
	
	// Write to the screen!
	pMemDC->ExtTextOut(iTextX, iTextY, ETO_CLIPPED, &rText, strText, strText.GetLength(), NULL);

	if (pItem->IsSelected())
	{
		pMemDC->SetBkColor ( crColorBK);
		pMemDC->SetTextColor( crColorText);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBrowseView diagnostics

#ifdef _DEBUG
void CBrowseView::AssertValid() const
{
	CView::AssertValid();
}

void CBrowseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBrowseDoc* CBrowseView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBrowseDoc)));
	return (CBrowseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBrowseView message handlers

BOOL CBrowseView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}


void CBrowseView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (!m_bCalcSizes)
	{
		CalcMaxButtons();
	}
}

void CBrowseView::CalcMaxButtons()
{	
	// Figure out how many buttons can fit 

	CBrowseDoc* pDoc = GetDocument();

	if (!pDoc)
	{
		return;
	}

	CBrowserCacheList *pCacheList = pDoc->GetCacheList();

	int iNumItems = pCacheList->GetCount();

	m_bCalcSizes = TRUE;

	CRect crView;
	GetClientRect (&crView);
	CSize csPageAmount;
	CSize csLineAmount;
//	SCROLLINFO initScroll;

	int iVertButtons = 0;
	int iHorzButtons = 0;

	div_t dtSize;
	dtSize = div (crView.Height(), BROWSE_BUTTON_HEIGHT);
	
	iVertButtons = dtSize.quot;
	iVertButtons = max (iVertButtons, 1);

	dtSize = div (crView.Width(), BROWSE_BUTTON_WIDTH);

	iHorzButtons = dtSize.quot;
	iHorzButtons = max (iHorzButtons, 1);

	int iNumImageRows = (iNumItems + iHorzButtons) / iHorzButtons;

	// Only adjust the scrollbars if the dimensions of the window have changed	
	if (
		(iHorzButtons != m_iHorzButtons) || 
		(iVertButtons != m_iVertButtons) || 
		((iNumImageRows != m_iNumImageRows) && (iNumImageRows > iVertButtons)) ||
		m_bResetScrollbars
		)
	{	
/*		m_iVertButtons = iVertButtons;
		m_iHorzButtons = iHorzButtons;
		m_iNumImageRows = iNumImageRows;

		m_csDocSize.cx = BROWSE_BUTTON_WIDTH * m_iHorzButtons;				
		m_csDocSize.cy = m_iNumImageRows * BROWSE_BUTTON_HEIGHT;
			
		csPageAmount.cy = BROWSE_BUTTON_HEIGHT * m_iVertButtons;
		csPageAmount.cx = BROWSE_BUTTON_WIDTH;

		csLineAmount.cy = BROWSE_BUTTON_HEIGHT;
		csLineAmount.cx = BROWSE_BUTTON_WIDTH;

		//SetScrollRange (SB_VERT, 0, m_csDocSize.cy, FALSE);	
		//SetScrollRange (SB_HORZ, 0, m_csDocSize.cx, FALSE);
		*/
		
/*		initScroll.fMask = SIF_PAGE | SIF_RANGE;
		initScroll.nPage = m_csDocSize.cx;
		initScroll.nMin = 0;
		initScroll.nMax = m_csDocSize.cx;

		SetScrollInfo(SB_HORZ, &initScroll, FALSE);

		initScroll.fMask = SIF_PAGE | SIF_RANGE;
		initScroll.nPage = m_csDocSize.cy;		
		initScroll.nMin = 0;
		initScroll.nMax = m_csDocSize.cy;

		SetScrollInfo(SB_VERT, &initScroll, FALSE);
		*/

		/*SetScrollSizes(MM_TEXT, m_csDocSize, csPageAmount, csLineAmount);
		
		if (m_iNumImageRows > iVertButtons)
		{
			initScroll.fMask = SIF_POS;
			initScroll.nPos = m_csDocSize.cy;
			
			SetScrollInfo(SB_VERT, &initScroll, FALSE);
		}

		m_iCurrentScrollPos = min (m_iCurrentScrollPos, m_iNumImageRows);
		m_iCurrentScrollPos = max (m_iCurrentScrollPos, 0);

		m_bResetScrollbars = FALSE;
		*/

		m_iVertButtons = iVertButtons;
		m_iHorzButtons = iHorzButtons;
		m_iNumImageRows = iNumImageRows;
		
		SCROLLINFO initScroll;
		UINT uiStructSize = sizeof (SCROLLINFO);
		
		memset (&initScroll, 0, uiStructSize);

		initScroll.cbSize = uiStructSize;
		initScroll.fMask = SIF_PAGE | SIF_RANGE;

		initScroll.nMin = 0;
		initScroll.nMax = iNumImageRows;
		initScroll.nPage = m_iVertButtons;

		SetScrollInfo (SB_VERT, &initScroll, FALSE);

		m_iCurrentScrollPos = min (m_iCurrentScrollPos, iNumImageRows);
		m_iCurrentScrollPos = max (m_iCurrentScrollPos, 0);
		SetScrollPos (SB_VERT, m_iCurrentScrollPos, TRUE);

		m_bResetScrollbars = FALSE;
	}
	
	m_bCalcSizes = FALSE;
}

void CBrowseView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CView::OnVScroll(nSBCode, nPos, pScrollBar);

	int iScrollBarPos = m_iCurrentScrollPos;
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		{
			iScrollBarPos -= 1;
		}
		break;

	case SB_LINEDOWN:
		{
			iScrollBarPos += 1;
		}
		break;

	case SB_PAGEUP:
		{
			iScrollBarPos -= m_iVertButtons;
		}
		break;

	case SB_PAGEDOWN:
		{
			iScrollBarPos += m_iVertButtons;
		}
		break;

	case SB_BOTTOM:
		{
			iScrollBarPos = m_iNumImageRows;
		}
		break;

	case SB_TOP:
		{
			iScrollBarPos = 0;
		}
		break;

	case SB_THUMBPOSITION:
		{
			iScrollBarPos = nPos;
		}
		break;

	case SB_THUMBTRACK:
		{
			iScrollBarPos = nPos;
		}
		break;

	case SB_ENDSCROLL:
		{
			m_bScrolling = FALSE; 
		}
		break;
	}

	iScrollBarPos = min (iScrollBarPos, m_iNumImageRows);
	iScrollBarPos = max (iScrollBarPos, 0);

	if (iScrollBarPos != m_iCurrentScrollPos)
	{
		m_bScrolling = TRUE;
		m_iInvalidateScrollRange = m_iCurrentScrollPos - iScrollBarPos;

		m_iCurrentScrollPos = iScrollBarPos;		

		if (abs (m_iInvalidateScrollRange) < m_iVertButtons)
		{
			ScrollWindow (0, (m_iInvalidateScrollRange * BROWSE_BUTTON_HEIGHT), NULL, NULL);
			UpdateWindow();
		}
		else
		{
			InvalidateRect (NULL, FALSE);
		}
		
		//TRACE ("m_iCurrentScrollPos = %d\n", m_iCurrentScrollPos);
		SetScrollPos (SB_VERT, m_iCurrentScrollPos, TRUE);		
	}	
}

void CBrowseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CView::OnLButtonDblClk(nFlags, point);

	int iItemNumber = GetItemNumber (point);
	
	if (iItemNumber != -1)
	{
		CBrowseDoc *pDoc = GetDocument();
		CBrowserCacheList *pCacheList = pDoc->GetCacheList();
		CBrowserCacheItem *pCacheItem = pCacheList->GetAtPosition (iItemNumber);
		CString strFullPath ("");

		strFullPath.Format ("%s\\%s", TrimSlashes(pCacheList->GetDirectory()), pCacheItem->GetFileName());

		theApp.OpenImage (strFullPath);	
	}
}

void CBrowseView::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	CView::OnRButtonUp(nFlags, point);
	
	m_bRButtonDown = FALSE;
	ReleaseCapture();
	
	int iItemNumber = GetItemNumber (point);

	if (iItemNumber == m_iRButtonItem)
	{		
		CBrowserCacheItem *pCacheItem = GetCurrentCacheItem (MB_RIGHT_BUTTON);

		if (pCacheItem)
		{
			if (!pCacheItem->IsSelected())
			{
				CBrowseDoc* pDoc = GetDocument();
				CBrowserCacheList *pCacheList = pDoc->GetCacheList();

				pCacheList->ResetAllSelections();
				pCacheItem->SetSelected();
				InvalidateRect (NULL, FALSE);
			}
		}
		
		// Need this point to position the menu, relative to the screen.  The passed-in point 
		// is relative to the view window
		CPoint ptScreenPos;
		GetCursorPos( &ptScreenPos);
		
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_BROWSE_POPUP));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptScreenPos.x, ptScreenPos.y,
			pWndPopupOwner);	
	}	
}

void CBrowseView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CView::OnRButtonDown(nFlags, point);
	
	m_bRButtonDown = TRUE;
	m_iRButtonItem = GetItemNumber (point);
	SetCapture();	
}

void CBrowseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	
	m_ptLButtonDown = point;
	m_iLButtonItem = GetItemNumber (point);
	m_bLButtonDown = TRUE;
	m_bDragging = FALSE;
	
	SetCapture();
}

void CBrowseView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnLButtonUp(nFlags, point);

	m_bLButtonDown = FALSE;
	m_bDragging = FALSE;

	ReleaseCapture();

	int iItemNumber = GetItemNumber (point);

	if (iItemNumber == m_iLButtonItem)
	{
		CBrowserCacheItem *pCacheItem = GetCurrentCacheItem(MB_LEFT_BUTTON);

		if (pCacheItem)
		{
			BOOL bControlKey = IsKeyPressed( VK_CONTROL);

			if (!bControlKey)
			{
				CBrowseDoc* pDoc = GetDocument();
			
				CBrowserCacheList *pCacheList = pDoc->GetCacheList();
				pCacheList->ResetAllSelections();

				pCacheItem->SetSelected();
			}
			else
			{
				pCacheItem->SetSelected(!pCacheItem->IsSelected());
			}
			InvalidateRect (NULL, FALSE);			
		}
	}
}


int CBrowseView::GetItemNumber (CPoint pt)
{
	CBrowseDoc *pDoc = GetDocument();

	if (!pDoc)
	{
		return -1;
	}

	int x, y;
	int iRow = 0;
	int iColumn = 0;
	int iOffset = 0;

	if ((pt.x > m_iHorzButtons * BROWSE_BUTTON_WIDTH) || (pt.y > (m_iVertButtons + 1) * BROWSE_BUTTON_HEIGHT))
	{
		return -1;
	}
	
	//  Find out where the scrollbar is at
	y = (m_iCurrentScrollPos * BROWSE_BUTTON_HEIGHT) + pt.y;
	x = pt.x;

	iColumn = x / BROWSE_BUTTON_WIDTH;
	iRow = y / BROWSE_BUTTON_HEIGHT;

	iOffset = iColumn + (iRow * m_iHorzButtons);

	if (iOffset < pDoc->GetItemCount())
	{
		return iOffset;
	}

	return -1;	
}

void CBrowseView::OnBvpopupCopy() 
{
	CString strFileName("");
	
	int iSelCount = GetSelectedList();

	if (iSelCount)
	{	
		HGLOBAL hGlobal;
		
		// allocate space for DROPFILE structure plus the number of file and one extra byte for final NULL terminator
		hGlobal = GlobalAlloc (GHND | GMEM_SHARE | GMEM_ZEROINIT, (DWORD) ( sizeof(DROPFILES) + (_MAX_PATH) * iSelCount + 1) );
		
		if (!hGlobal)
		{
			return;
		}

		LPDROPFILES pDropFiles;
		
		pDropFiles = (LPDROPFILES)GlobalLock(hGlobal);
		
		// set the offset where the starting point of the files start
		pDropFiles->pFiles = sizeof(DROPFILES);
		
		// file contains wide characters
		pDropFiles->fWide = FALSE;

		int iCurPosition = sizeof(DROPFILES);
		
		for (int i = 0; i < iSelCount; i++)
		{
			strFileName = m_saSelectedItems.GetAt(i);
			
			// copy the file into global memory
			lstrcpy( (LPSTR) ((LPSTR)(pDropFiles) + iCurPosition), TEXT(strFileName));
			
			// move the current position beyond the file name copied
			// don't forget the Null terminator (+1)
			iCurPosition += strFileName.GetLength() + 1;
		 }
		
		// final null terminator as per CF_HDROP Format specs.
		((LPSTR) pDropFiles) [iCurPosition] = 0;
		
		GlobalUnlock(hGlobal);			

		CWnd *pWnd = (CWnd *)this;
		
		if (::OpenClipboard(pWnd->GetSafeHwnd()))
		{
			::EmptyClipboard();		
			::SetClipboardData(CF_HDROP, hGlobal);
			CloseClipboard();
		}
		else
		{
			AfxMessageBox(CG_IDS_CANNOT_OPEN_CLIPBOARD);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 
////	PasteFile
////   
////	Takes as parameters the file name to be copied, and a BOOL as to whether a prompt should be given
////	to the user if the file can't be copied about whether or not to continue copying files.  If we're 
////	working on the last file in a set, no need to prompt them.  Returns the value of their selection- TRUE 
////	if they want to continue, FALSE if they do not
//// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		
BOOL CBrowseView::PasteFile(LPCTSTR szFileName, BOOL bPrompt)
{
	// Let's just make sure the source file is still there.

	FILE *fp = fopen (szFileName, "rb");
	FILE *rp = NULL;

	CString strDestinationFile ("");
	CString strNewName ("");
	CString strRawName (GetRawFileNameWExt (szFileName));
	int iCopyCounter = 1;

	if (fp)
	{
		fclose (fp);

		// Let's see if the same file name exists in our current directory
		strNewName = strRawName;
		strDestinationFile.Format ("%s%s", GetCurrentDirectory(), strNewName);

		rp = fopen (strDestinationFile, "rb");

		// It exists, let's come up with a unique name
		while (rp)
		{
			fclose (rp);

			if (iCopyCounter == 1)
			{
				strNewName.Format ("Copy of %s", strRawName);
			}
			else
			{
				strNewName.Format ("Copy (%d) of %s", iCopyCounter, strRawName);
			}
			iCopyCounter++;
			
			strDestinationFile.Format ("%s%s", GetCurrentDirectory(), strNewName);
			rp = fopen (strDestinationFile, "rb");
		}		// while (rp)
		
		CopyFile( szFileName, strDestinationFile, TRUE);
		
		CBrowseDoc *pDoc = GetDocument();
	
		if (pDoc)
		{
			CBrowserCacheList *pCacheList = pDoc->GetCacheList();
			
			if (pCacheList)
			{
				CBrowserCacheItem *pNewItem = new CBrowserCacheItem;
				
				if (pNewItem->Create (strDestinationFile))
				{
					pCacheList->AddItem(pNewItem);
					InvalidateRect(NULL, FALSE);
				}
			}
		}		// if (pDoc)

		fclose (fp);
	}			// if (fp)	
	else
	{
		CString strError("");

		if (bPrompt)
		{
			strError.Format ("Could not copy %s\nWould you like to continue copying the remaining files?", szFileName);
			if (AfxMessageBox (strError, MB_YESNO) == IDYES)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			strError.Format ("Could not copy %s", szFileName);
			AfxMessageBox (strError, MB_ICONSTOP);
		}
	}
	
	return TRUE;
}

void CBrowseView::OnBvpopupPaste() 
{
	CWnd *pWnd = (CWnd *)this;
	
	if (::IsClipboardFormatAvailable(CF_HDROP))
	{
		if (pWnd->OpenClipboard())
		{
			HGLOBAL hgData = ::GetClipboardData(CF_HDROP);
			if (!hgData)
			{
				AfxMessageBox ("Error reading from clipboard", MB_ICONSTOP);
				return;
			}

			// Determine the data size
			int iDataSize = GlobalSize(hgData);
						
			// Alloc memory
			BYTE *pbyData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

			if (!pbyData)
			{
				AfxMessageBox ("Ran out of memory trying to access clipboard", MB_ICONSTOP);
				return;
			}

			// Grab the data					
			CopyMemory(pbyData, GlobalLock(hgData), iDataSize); 

			// Get out as fast as we can from the clipboard
			GlobalUnlock(hgData);
			CloseClipboard();

			LPDROPFILES pDropFiles;
		
			pDropFiles = (LPDROPFILES)pbyData;

			// find the offset where the starting point of the files start
			int iCurOffset = pDropFiles->pFiles;

			// copy the filename
			CString strFileName("");
			BOOL bDone = FALSE;

			while (!bDone)
			{
				if (*(pbyData + iCurOffset) == 0)
				{
					bDone = TRUE;
				}
				else
				{
					if (pDropFiles->fWide)
					{
						wchar_t *szWideString = (wchar_t *)(pbyData + iCurOffset);
						strFileName = szWideString;

						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += (strFileName.GetLength() + 1) * 2;						
					}
					else
					{
						strFileName = (LPSTR) ((LPSTR)(pDropFiles) + iCurOffset);
						
						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += strFileName.GetLength() + 1;
					}
					
					// Go add it to the cache
					bDone = !PasteFile(strFileName, (*(pbyData + iCurOffset) == 0) ? FALSE : TRUE);					
				}
			}			
						
			if (pbyData)
			{
				GlobalFree ((HGLOBAL) pbyData);
				pbyData = NULL;
			}			
		}					// if (pWnd->OpenClipboard())
	}						// if (::IsClipboardFormatAvailable(g_iBrowseCopyPasteFormat))
}

void CBrowseView::OnUpdateBvpopupPaste(CCmdUI* pCmdUI) 
{
	//pCmdUI->Enable (::IsClipboardFormatAvailable(g_iBrowseCopyPasteFormat));
	pCmdUI->Enable (::IsClipboardFormatAvailable(CF_HDROP));
}

void CBrowseView::OnBvpopupCopyClipboard() 
{
	CBrowserCacheItem *pCacheItem = GetCurrentCacheItem(MB_RIGHT_BUTTON);
	
	if (pCacheItem)
	{
		CString strFileName("");		
		strFileName = GetCurrentDirectory() + pCacheItem->GetFileName();

		CImageHelper ihHelper;

		ihHelper.LoadImage (strFileName, IH_LOAD_ONLYIMAGE);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;
		}

		CDibSection dsClip;
		dsClip.Init (ihHelper.GetImageWidth(), ihHelper.GetImageHeight(), ihHelper.GetColorDepth() == IH_8BIT ? 8 : 24, ihHelper.GetPalette(), FALSE);
		dsClip.SetRawBits (ihHelper.GetBits());
		dsClip.WriteToClipboard ((CWnd*)this);	
	}	
}

void CBrowseView::OnUpdateBvpopupCopyClipboard(CCmdUI* pCmdUI) 
{
	CImageHelper ihHelper;
	CBrowserCacheItem *pItem = GetCurrentCacheItem(MB_RIGHT_BUTTON);
	
	BOOL bEnable = ( (pItem != NULL) && (pItem != NULL ? ihHelper.IsValidImage (pItem->GetImageType()) : FALSE) );
	pCmdUI->Enable (bEnable);
}

void CBrowseView::DeleteSelectedItems() 
{
	int iSelCount = GetSelectedList();
	CString strFileName("");
	CString strMessage("");
	
	if (iSelCount)
	{		
		strMessage.Format ("Are you sure you want to delete %s?", 
			iSelCount > 1 ? "these items" : "this item"		
			);
		int iReturn = AfxMessageBox (strMessage, MB_YESNO);

		if (iReturn != IDYES)
		{
			return;
		}
	}
	else
	{
		return;
	}
	
	CBrowseDoc *pDoc = GetDocument();				
	if (!pDoc)
	{
		return;
	}

	CBrowserCacheList *pCacheList = pDoc->GetCacheList();		
	if (!pCacheList)
	{
		return;
	}

	CBrowserCacheItem *pCacheItem = pCacheList->GetFirstSelectedItem();
	CBrowserCacheItem *pTemp = NULL;

	while (pCacheItem)
	{		
		strFileName = GetCurrentDirectory();
		strFileName += pCacheItem->GetFileName();
		remove (strFileName);
		
		pTemp = pCacheItem;
		pCacheItem = pCacheList->GetNextSelectedItem();					
		pCacheList->RemoveItem (pTemp);
	}

	ResetScrollbars (TRUE);
	InvalidateRect (NULL, TRUE);
}

void CBrowseView::OnEditClear() 
{
	DeleteSelectedItems();	
}

void CBrowseView::OnBvpopupDelete() 
{
	DeleteSelectedItems();	
}

void CBrowseView::OnBvpopupInfo() 
{	
	CBrowserCacheItem *pCacheItem = GetCurrentCacheItem(MB_RIGHT_BUTTON);

	if (pCacheItem)
	{	
		CString strFullPath (GetCurrentDirectory());
		strFullPath += pCacheItem->GetFileName();

		CWildCardItem *pItem = g_WildCardList.GetAt (pCacheItem->GetImageType());

		CString strDescription = pItem->GetDescription();
		CString strText("");

		strText.Format ("%s%s\n%s%d\n%s%d\n%s%d",
			"Image type: ", strDescription,
			"Width: ", pCacheItem->GetWidth(),
			"Height: ", pCacheItem->GetHeight(),
			"Bit depth: ", pCacheItem->GetBitDepth());

		AfxMessageBox (strText);
	}
}

CBrowserCacheItem *CBrowseView::GetCurrentCacheItem(int iMouseButton)
{
	CBrowseDoc *pDoc = GetDocument();
	CBrowserCacheItem *pCacheItem = NULL;

	if (pDoc)
	{
		CBrowserCacheList *pCacheList = pDoc->GetCacheList();
		
		if (pCacheList)
		{
			pCacheItem = pCacheList->GetAtPosition (iMouseButton == MB_RIGHT_BUTTON ? m_iRButtonItem : m_iLButtonItem);
		}
	}
	return pCacheItem;
}

CString CBrowseView::GetCurrentDirectory()
{
	CBrowseDoc *pDoc = GetDocument();
	CString strDirectory ("");
	
	if (pDoc)
	{
		CBrowserCacheList *pCacheList = pDoc->GetCacheList();
		
		if (pCacheList)
		{
			strDirectory.Format ("%s\\", TrimSlashes(pCacheList->GetDirectory()));
		}
	}

	return strDirectory;
}

void CBrowseView::OnUpdateBvpopupCopy(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetCurrentCacheItem(MB_RIGHT_BUTTON) != NULL);
	pCmdUI->Enable (bEnable);
}

void CBrowseView::OnUpdateBvpopupDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetCurrentCacheItem(MB_RIGHT_BUTTON) != NULL);
	pCmdUI->Enable (bEnable);
}

void CBrowseView::OnUpdateBvpopupInfo(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (GetCurrentCacheItem(MB_RIGHT_BUTTON) != NULL);
	pCmdUI->Enable (bEnable);
}

void CBrowseView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CView::OnMouseMove(nFlags, point);

	if (m_bLButtonDown)
	{		
		int x = m_ptLButtonDown.x;
		int y = m_ptLButtonDown.y;

		int x1 = point.x;
		int y1 = point.y;

		int xd = abs (x - x1);
		int yd = abs (y - y1);

		if ((xd > 5) || (yd > 5))
		{
			if (!m_bDragging)
			{
#if 1
				CBrowserCacheItem *pCacheItem = GetCurrentCacheItem(MB_LEFT_BUTTON);

				if (pCacheItem)
				{
					// If the item isn't selected, go select it
					if (!pCacheItem->IsSelected())
					{
						BOOL bControlKey = IsKeyPressed( VK_CONTROL);
						
						if (!bControlKey)
						{
							CBrowseDoc* pDoc = GetDocument();
		
							CBrowserCacheList *pCacheList = pDoc->GetCacheList();
							pCacheList->ResetAllSelections();							
						}

						pCacheItem->SetSelected();
						InvalidateRect (NULL, FALSE);
					}

					CString strFileName("");
		
					int iSelCount = GetSelectedList();

					if (iSelCount)
					{	
						COleDataSource*	pSource = new COleDataSource();
						CSharedFile	sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
						
						DWORD iDataSize = sizeof(DROPFILES) + ( (_MAX_PATH) * iSelCount) + 1;
						BYTE *pbyData = new BYTE[iDataSize];

						if (!pbyData)
						{
							return;
						}				
						memset (pbyData, 0, iDataSize);
						
						LPDROPFILES pDropFiles;					
						pDropFiles = (LPDROPFILES)pbyData;
						
						// set the offset where the starting point of the files start
						pDropFiles->pFiles = sizeof(DROPFILES);
						
						// file contains wide characters
						pDropFiles->fWide = FALSE;

						int iCurPosition = sizeof(DROPFILES);
						
						for (int i = 0; i < iSelCount; i++)
						{
							strFileName = m_saSelectedItems.GetAt(i);
							
							// copy the file into global memory
							lstrcpy( (LPSTR) ((LPSTR)(pDropFiles) + iCurPosition), TEXT(strFileName));
							
							// move the current position beyond the file name copied
							// don't forget the Null terminator (+1)
							iCurPosition += strFileName.GetLength() + 1;
						 }
						
						// final null terminator as per CF_HDROP Format specs.
						((LPSTR) pDropFiles) [iCurPosition] = 0;
						
						sf.Write(pbyData, iDataSize);

						HGLOBAL hMem = sf.Detach();	
					
						if (!hMem)
						{
							if (pbyData)
							{
								delete []pbyData;
								pbyData = NULL;
							}
							return;
						}

						pSource->CacheGlobalData(CF_HDROP, hMem);
						pSource->DoDragDrop();

						if (pSource)
						{
							delete pSource;
							pSource = NULL;
						}

						if (pbyData)
						{
							delete []pbyData;
							pbyData = NULL;
						}
						m_bDragging = true;					
						
					}
				}			// if (pCacheItem)

#else
				int iItemNumber = GetItemNumber (m_ptLButtonDown);
				CBrowseDoc *pDoc = GetDocument();
				
				if (!pDoc)
				{
					return;
				}

				CBrowserCacheList *pCacheList = pDoc->GetCacheList();
					
				if (!pCacheList)
				{
					return;
				}
				
				CBrowserCacheItem *pCacheItem = pCacheList->GetAtPosition (iItemNumber);

				if (!pCacheItem)
				{
					return;
				}

				COleDataSource*	pSource = new COleDataSource();
				CSharedFile	sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

				BYTE *pbyData = NULL;
				int iDataSize = 0;

				CString strFileName("");

				strFileName = GetCurrentDirectory() + pCacheItem->GetFileName();
				int iLength = strFileName.GetLength();

				// The NULL character is accounted for, as we're putting the first char of the string
				// in BROWSE_CLIPBOARD_S->szFileName
				iDataSize = iLength + BROWSE_CLIPBOARD_SIZE + BC_HEADER_SIZE;

				pbyData = new BYTE[iDataSize];

				if (!pbyData)
				{
					return;
				}				
				memset (pbyData, 0, iDataSize);

				LPBC_HEADERS_S lpHeader = (LPBC_HEADERS_S)(pbyData);

				// For now, supports only one.  TODO:  add some mechanism for multiple selections
				lpHeader->iNumSelections = 1;
				lpHeader->iFirstEntryOffset = BC_HEADER_SIZE;

				LPBROWSE_CLIPBOARD_S lpClip = (LPBROWSE_CLIPBOARD_S)(pbyData + lpHeader->iFirstEntryOffset);

				lpClip->iFileNameLength = iLength;
				strcpy ((char *)(&lpClip->szFileName), strFileName.GetBuffer(iLength));
								
				sf.Write(pbyData, iDataSize);
				HGLOBAL hMem = sf.Detach();	
				
				if (!hMem)
				{
					if (pbyData)
					{
						delete []pbyData;
						pbyData = NULL;
					}
					return;
				}

				pSource->CacheGlobalData(g_iBrowseCopyPasteFormat, hMem);
				pSource->DoDragDrop();

				if (pSource)
				{
					delete pSource;
					pSource = NULL;
				}

				if (pbyData)
				{
					delete []pbyData;
					pbyData = NULL;
				}
				m_bDragging = true;
#endif
			}		// if (!m_bDragging)
		}			// if ((xd > 5) || (yd > 5))
	}				// if (m_bLButtonDown)
}

DROPEFFECT CBrowseView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (pDataObject->IsDataAvailable(CF_HDROP)) 
	{
		return DROPEFFECT_COPY;		
	}
	
	if (pDataObject->IsDataAvailable(g_iPackageFormat)) 
	{		
		return DROPEFFECT_COPY;
	}
	
	return DROPEFFECT_NONE;
	
	//return CView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CBrowseView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	bool bShiftKey = IsKeyPressed( VK_SHIFT);
	bool bControlKey = IsKeyPressed( VK_CONTROL);

	if (pDataObject->IsDataAvailable(g_iPackageFormat)) 
	{
		HGLOBAL hmem = pDataObject->GetGlobalData(g_iPackageFormat);
		CMemFile sf((BYTE*) ::GlobalLock(hmem), ::GlobalSize(hmem));

		DWORD dwSize = ::GlobalSize(hmem);
		
		BYTE *pbyDragData = new BYTE[dwSize];

		if (!pbyDragData)
		{
			return FALSE;
		}
		
		sf.Read(pbyDragData, dwSize);

		::GlobalUnlock(hmem);

		CPackageView *pPackageView = NULL;
		
		memcpy (&pPackageView, pbyDragData, sizeof (CPackageView *));
				
		pPackageView->DoPackageExport (GetCurrentDirectory());

		pPackageView = NULL;

		if (pbyDragData)
		{
			delete []pbyDragData;
			pbyDragData = NULL;
		}

		// Control key handled first... if user has both SHIFT and CONTROL, copy
		// instead of move
/*
		if (bControlKey)
		{
			m_iDragType = PV_DRAG_COPY;
			FinishDragDrop();
			return TRUE;
		}

		if (bShiftKey)
		{
			m_iDragType = PV_DRAG_MOVE;
			FinishDragDrop();
			return TRUE;
		}		

		if (!bShiftKey && !bControlKey)
		{
			m_iDragType = PV_DRAG_UNKNOWN;

			CPoint ptScreenPos( point);
			ClientToScreen( &ptScreenPos);
				
			CMenu Menu;
			VERIFY( Menu.LoadMenu( IDR_PACKAGE_DRAG_POPUP));

			CMenu* pPopup = Menu.GetSubMenu( 0);
			ASSERT( pPopup != NULL);
	
			pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
				ptScreenPos.x, ptScreenPos.y, this);
		}
		*/

		return TRUE;	
	}

	return FALSE;
	
	//return CView::OnDrop(pDataObject, dropEffect, point);
}

BOOL CBrowseView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	//return CView::OnMouseWheel(nFlags, zDelta, pt);
	return TRUE;
}

int CBrowseView::GetSelectedList()
{
	m_saSelectedItems.RemoveAll();

	CBrowseDoc *pDoc = GetDocument();
				
	if (!pDoc)
	{
		return 0;
	}

	CBrowserCacheList *pCacheList = pDoc->GetCacheList();
		
	if (!pCacheList)
	{
		return 0;
	}

	CBrowserCacheItem *pItem = pCacheList->GetFirstSelectedItem();
	CString strFileName("");
	int iCount = 0;

	while (pItem)
	{
		strFileName = GetCurrentDirectory();
		strFileName += pItem->GetFileName();
		
		m_saSelectedItems.Add (strFileName);
		iCount++;

		pItem = pCacheList->GetNextSelectedItem();
	}

	return iCount;
}

