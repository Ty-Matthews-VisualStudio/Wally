// BuildView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BuildView.h"
#include "BuildList.h"
#include "BuildDoc.h"
#include "ImageHelper.h"
#include "DibSection.h"
#include "CustomResource.h"
#include "BuildPropertiesDlg.h"
#include "GamePaletteDlg.h"
#include "PrefabImages.h"
#include "ClipboardDIB.h"
#include "ColorOpt.h"
#include "ReMip.h"
#include "WallyPal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildView

IMPLEMENT_DYNCREATE(CBuildView, CScrollView)

CBuildView::CBuildView()
{
	m_iTileNumber = 0;
}

CBuildView::~CBuildView()
{
	if (m_pdsImage)
	{
		delete m_pdsImage;
		m_pdsImage = NULL;
	}
}


BEGIN_MESSAGE_MAP(CBuildView, CScrollView)
	//{{AFX_MSG_MAP(CBuildView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_BUILDVIEW_POPUP_COPY, OnBuildviewPopupCopy)
	ON_COMMAND(ID_BUILDVIEW_POPUP_PROPERTIES, OnBuildviewPopupProperties)
	ON_COMMAND(ID_BUILDVIEW_POPUP_PASTE, OnBuildviewPopupPaste)
	ON_UPDATE_COMMAND_UI(ID_BUILDVIEW_POPUP_PASTE, OnUpdateBuildviewPopupPaste)
	ON_COMMAND(ID_BUILDVIEW_POPUP_DELETE, OnBuildviewPopupDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildView drawing

void CBuildView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CBuildDoc *pDoc = GetDocument();

	pDoc->ValidateTileNumber(&m_iTileNumber);

	CGamePaletteDlg dlgPalette;
	dlgPalette.DoModal();
	
	CCustomResource crPalette;

	crPalette.UseResourceId ("PALETTE", dlgPalette.GetGameSelection());
	memcpy (m_byPalette, crPalette.GetData(), 768);

	m_iLastWidth	= 0;
	m_iLastHeight	= 0;

	CPrefabImage piImage;
	BYTE *pbyButtonData = NULL;
	BYTE *pbyButtonPalette = NULL;
	int iWidth = 0;
	int iHeight = 0;

	piImage.CreatePrefabImage (PI_BUILDVIEW_BUTTON, &pbyButtonData, &pbyButtonPalette);
	iWidth = piImage.GetImageWidth();
	iHeight = piImage.GetImageHeight();
	
	m_dsButton.Init (iWidth, iHeight, 8, pbyButtonPalette, FALSE);
	m_dsButton.SetRawBits (pbyButtonData);

	m_pdsImage = NULL;
}

void CBuildView::OnDraw(CDC* pDC)
{

	CBuildDoc *pDoc = GetDocument();
	//CDibSection *m_pdsImage = NULL;
	
	CRect rcItem;
	CRect rcImage;
	GetClientRect( &rcItem);
	int rcWidth = rcItem.Width();
	int rcHeight = rcItem.Height() + (BUILD_IMAGE_HEIGHT);

	CDC DC;
	DC.CreateCompatibleDC( pDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( pDC, rcWidth, rcHeight);
	DC.SelectObject( Bmp);


	// Figure out how many images we can display vertically and horizontally	
	div_t mDiv;

	mDiv = div (rcHeight, BUILD_IMAGE_HEIGHT);
	int iMaxVImages = mDiv.quot + 1;
	iMaxVImages = max (iMaxVImages, 1) + 1;
	
	mDiv = div (rcWidth, BUILD_IMAGE_WIDTH);
	int iMaxHImages = mDiv.quot;	
	iMaxHImages = max (iMaxHImages, 1);

	if ((iMaxHImages != m_iMaxHImages) || (iMaxVImages != m_iMaxVImages))
	{
		m_iMaxHImages = iMaxHImages;
		m_iMaxVImages = iMaxVImages;
		
		m_DocSize.cx = m_iMaxHImages * BUILD_IMAGE_WIDTH;
		mDiv = div (256 + (m_iMaxHImages - 1), m_iMaxHImages);
		
		m_DocSize.cy = mDiv.quot * BUILD_IMAGE_HEIGHT;

		CSize vertAmount;
		vertAmount.cy = BUILD_IMAGE_HEIGHT * m_iMaxVImages;
		vertAmount.cx = BUILD_IMAGE_WIDTH;
			
		SetScrollRange (SB_VERT, 0, m_DocSize.cy, false);	
		SetScrollRange (SB_HORZ, 0, m_DocSize.cx, false);	
		
		SetScrollSizes(MM_TEXT, m_DocSize, vertAmount, vertAmount);	
	}

	int j = 0;
	int k = 0;
	int iTileNumber = 0;
	
	CFont TextFont;
	CFont *pOldFont;

	DC.SetBkColor (RGB( 0, 0, 0));
	TextFont.CreateFont (18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"MS Sans Serif");
	pOldFont = DC.SelectObject (&TextFont);

	DC.SetTextColor (RGB (216, 128, 14));

	//  Find out where the scrollbar is at, and calculate how many "rows" down we are 
	int sbPosition = (GetScrollPos(SB_VERT) / BUILD_IMAGE_HEIGHT);
	int sbPositionY = GetScrollPos(SB_VERT);
	int iOffsetPosition = 0;
	int x, y;
	int TextX, TextY, TextX1, TextY1;
	CRect TextRect;
	// x1, y1;

	pDoc->ValidateTileNumber (&iTileNumber);
	int iActualTileNumber = 0;
	CBuildItem *pItem = NULL;
	BYTE *pbyData = NULL;
	int iWidth = 0;
	int iHeight = 0;
	CString strText ("");

	// clear background
	
	//DC.SetViewportOrg( pDC->GetViewportOrg());

	// clear background
	HBRUSH hBrush    = CreateSolidBrush( RGB (0, 0, 0));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	
	//x1 = rcWidth;
	//y1 = rcHeight;

	::PatBlt( DC, 0, 0, rcWidth, rcHeight, PATCOPY);
	
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);	
	
	//  Only draw the set of textures that can possibly be seen
	for (j = 0; j <= (m_iMaxVImages + 1); j++)
	{
		for (k = 0; k < m_iMaxHImages; k++)
		{
			// OffsetPosition is the position of the texture we're currently at
			iOffsetPosition = (sbPosition * m_iMaxHImages) + (j * m_iMaxHImages) + k;

			// Figure out the location to plot this guy... (x,y) = top left, (x1/y1) = bottom right
			x = k * BUILD_IMAGE_WIDTH;			
			y = j * BUILD_IMAGE_HEIGHT;

			//x1 = x + BUILD_IMAGE_WIDTH - 1;			
			//y1 = y + BUILD_IMAGE_HEIGHT - 1;

			iActualTileNumber = iTileNumber + iOffsetPosition;
			pItem = pDoc->GetBuildTile (iActualTileNumber);

			if ((iActualTileNumber <= pDoc->GetLastBuildTile()) && (iActualTileNumber >= pDoc->GetFirstBuildTile()))
			{
				m_dsButton.Show (&DC, x, y, 1.0);
				
				// Set up where the text should be plotted
				TextX = x + 56;
				TextY = y + BUILD_IMAGE_HEIGHT - 18;
				TextX1 = x + BUILD_IMAGE_WIDTH - 6;
				TextY1 = y + BUILD_IMAGE_HEIGHT;

				// This guy is used to clip the text, in case it's really long
				TextRect.SetRect (TextX, TextY, TextX1, TextY1);					

				strText.Format ("%d", iActualTileNumber);
				// Write to the screen!
				DC.ExtTextOut(TextX, TextY, ETO_CLIPPED, &TextRect, strText, strText.GetLength(), NULL);							
			}

			if (pItem)
			{
				pbyData = pItem->GetBits();
				iWidth = pItem->GetWidth();
				iHeight = pItem->GetHeight();

				if ((iWidth > 0) && (iHeight > 0))
				{
					if ((iWidth != m_iLastWidth) || (iHeight != m_iLastHeight))
					{
						if (m_pdsImage)
						{
							delete m_pdsImage;
							m_pdsImage = NULL;
						}						
					}
					
					if (!m_pdsImage)
					{					
						m_pdsImage = new CDibSection;
						m_pdsImage->Init (iWidth, iHeight, 8, m_byPalette);
					}

					m_iLastWidth = iWidth;
					m_iLastHeight = iHeight;
					
					m_pdsImage->SetRawBits (pbyData);

					if ((iWidth > (BUILD_TEXTURE_LIMIT / 2)) || (iHeight > (BUILD_TEXTURE_LIMIT / 2)))
					{
						rcImage.SetRect (x + BUILD_IMAGE_XOFFSET, y + BUILD_IMAGE_YOFFSET, 
							x + BUILD_TEXTURE_LIMIT + BUILD_IMAGE_XOFFSET, y + BUILD_TEXTURE_LIMIT + BUILD_IMAGE_YOFFSET);
						m_pdsImage->ShowRestricted (&DC, rcImage);
					}
					else
					{
						m_pdsImage->Show (&DC, x + BUILD_IMAGE_XOFFSET, y + BUILD_IMAGE_YOFFSET, 2);
					}
				}
			}		
		}
	}


	DC.SelectObject (pOldFont);
	TextFont.DeleteObject();
	
	//CPoint ptScrollPos( GetDeviceScrollPosition());
	int rcY = sbPosition * BUILD_IMAGE_HEIGHT;
	//TRACE ("sbPositionY = %d\n", sbPositionY);
	pDC->BitBlt( 0, rcY, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);
	DC.DeleteDC();
}

/////////////////////////////////////////////////////////////////////////////
// CBuildView diagnostics

#ifdef _DEBUG
void CBuildView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CBuildView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBuildView message handlers

BOOL CBuildView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	//return CScrollView::OnEraseBkgnd(pDC);
}

int CBuildView::GetTileNumber(CPoint ptMouse)
{
	CBuildDoc *pDoc = GetDocument();
	int x, y, iRow, iColumn, iOffset;

	if ((ptMouse.x > m_iMaxHImages * BUILD_IMAGE_WIDTH) || (ptMouse.y > (m_iMaxVImages + 1) * BUILD_IMAGE_HEIGHT))
	{		
		return -1;
	}
	
	//  Find out where the scrollbar is at
	y = GetScrollPos(SB_VERT) + ptMouse.y;
	x = GetScrollPos(SB_HORZ) + ptMouse.x;

	iColumn = x / BUILD_IMAGE_WIDTH;
	iRow = y / BUILD_IMAGE_HEIGHT;

	iOffset = iColumn + (iRow * m_iMaxHImages) + pDoc->GetFirstBuildTile();

	if (iOffset <= pDoc->GetLastBuildTile())
	{
		return iOffset;
	}
	
	return -1;	
}

void CBuildView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CScrollView::OnLButtonDown(nFlags, point);

	/*int iTileNumber = GetTileNumber (point);
	
	CBuildDoc *pDoc = GetDocument();
	CBuildItem *pItem = pDoc->GetBuildTile (iTileNumber);
	
	if (pItem)
	{
		CString strText("");
		int iFlags = pItem->GetFlags();
		int iAnimateNumber = iFlags & 0x7f;			// First 6 bits are the animate number
		int iAnimateType = (iFlags >> 6) & 3;		// Bits 7 & 8 are the animate type
		char xOffset = (iFlags >> 8) & 255;			// Bits 9 -> 16 are the X Offset
		char yOffset = (iFlags >> 16) & 255;		// Bits 17 -> 24 are the Y Offset;
		int iAnimateSpeed = (iFlags >> 24) & 15;	// Bits 25 -> 28 are the animate speed
		strText.Format ("Animate number = %d\nAnimate type = %d", iAnimateNumber, iAnimateType);
		AfxMessageBox (strText);
	}
	*/

}

void CBuildView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CScrollView::OnRButtonDown(nFlags, point);

	m_iPopupMenuTile = GetTileNumber (point);
	
	if (m_iPopupMenuTile == -1)
	{
		return;
	}

	// Need this point to position the menu, relative to the screen.  The passed-in point 
	// is relative to the view window
	CPoint ptScreenPos;
	GetCursorPos( &ptScreenPos);
		
	

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_BUILDVIEW_POPUP));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptScreenPos.x, ptScreenPos.y,
		pWndPopupOwner);
	
}

void CBuildView::OnBuildviewPopupCopy() 
{
	if (m_iPopupMenuTile == -1)
	{
		return;
	}

	CBuildDoc *pDoc = GetDocument();
	CBuildItem *pItem = pDoc->GetBuildTile (m_iPopupMenuTile);
	
	if (pItem)
	{
		int iWidth = pItem->GetWidth();
		int iHeight = pItem->GetHeight();		
		BYTE *pbyData = pItem->GetBits();

		if ((iWidth > 0) && (iHeight > 0))
		{
			CDibSection dsItem;
			dsItem.Init (iWidth, iHeight, 8, m_byPalette);
			dsItem.SetRawBits (pbyData);
			dsItem.WriteToClipboard (this);
		}
	}
}

void CBuildView::OnBuildviewPopupProperties() 
{
	CBuildPropertiesDlg dlg;
	
	CBuildDoc *pDoc = GetDocument();
	CBuildItem *pItem = pDoc->GetBuildTile (m_iPopupMenuTile);
	
	if (pItem)
	{
		CString strText("");
		int iFlags = pItem->GetFlags();
		int iHoldFlags = iFlags;

		int iAnimateNumber = iFlags & 0x3f;			// First 6 bits are the animate number
		int iAnimateType = (iFlags >> 6) & 3;		// Bits 7 & 8 are the animate type
		signed char xOffset = (iFlags >> 8) & 0xff;		// Bits 9 -> 16 are the X Offset
		signed char yOffset = (iFlags >> 16) & 0xff;	// Bits 17 -> 24 are the Y Offset;
		int iAnimateSpeed = (iFlags >> 24) & 15;	// Bits 25 -> 28 are the animate speed

		dlg.SetTileNumber (m_iPopupMenuTile);
		dlg.SetAnimateType (iAnimateType);
		dlg.SetAnimateSpeed (iAnimateSpeed);
		dlg.SetNumAnimateTiles (iAnimateNumber);
		dlg.SetXOffset (xOffset);
		dlg.SetYOffset (yOffset);
		if (dlg.DoModal() == IDOK)
		{
			iAnimateNumber = dlg.GetNumAnimateTiles();
			iAnimateType = dlg.GetAnimateType();
			iAnimateSpeed = dlg.GetAnimateSpeed();
			xOffset = dlg.GetXOffset();
			yOffset = dlg.GetYOffset();

			/*iAnimateType = iAnimateType << 5;
			int ixOffset = (int)xOffset;
			ixOffset = ixOffset << 7;

			int iyOffset = (int)yOffset;
			iyOffset = iyOffset << 15;

			iAnimateSpeed = iAnimateSpeed << 23;*/

			iFlags = 0;
			iFlags |= iAnimateNumber;
			iFlags |= iAnimateType << 6;
			iFlags |= ((int)xOffset) << 8;
			iFlags |= ((int)yOffset) << 16;
			iFlags |= iAnimateSpeed << 24;
			
			if (iFlags != iHoldFlags)
			{
				pItem->SetFlags (iFlags);
				pDoc->SetModifiedFlag (TRUE);
			}			
		}
	}	
}

void CBuildView::OnBuildviewPopupPaste() 
{
	CBuildDoc *pDoc = GetDocument();
	CBuildItem *pItem = pDoc->GetBuildTile (m_iPopupMenuTile);
	
	if (pItem)
	{
		int iReturn = IDYES;

		if (pItem->HasValidData())
		{
			iReturn = AfxMessageBox ("Are you sure you want to replace this tile?", MB_YESNO);
		}

		if (iReturn == IDYES)
		{
			CClipboardDIB ClipboardDIB;
			if (ClipboardDIB.InitFromClipboard(this))
			{		
				BYTE *pbyDocumentData;
				BYTE *pbyClipboardData;
				CWallyPalette Palette;
				BYTE byClipboardPalette[1024];

				int iWidth	= 0;
				int iHeight = 0;
				int iSize	= 0;
				int iDepth	= 0;

				int r, g, b, j;

				iWidth = ClipboardDIB.GetWidth();
				iHeight = ClipboardDIB.GetHeight();
				iSize = iWidth * iHeight;
				iDepth = ClipboardDIB.GetColorDepth();

				// Point to the clipboard bits
				pbyClipboardData = (unsigned char *)ClipboardDIB;

				pbyDocumentData = new BYTE[iSize];
				
				if (!pbyDocumentData)
				{
					ASSERT (FALSE);
					AfxMessageBox( "Out of memory during paste operation.", MB_ICONSTOP);
					return;
				}
				memset (pbyDocumentData, 0, iSize);

				COLOR_IRGB* pTemp24Bit = new COLOR_IRGB [iSize * sizeof( COLOR_IRGB)];
				if (pTemp24Bit == NULL)
				{
					ASSERT (FALSE);
					AfxMessageBox( "Out of memory during paste operation.", MB_ICONSTOP);
					
					if (pbyDocumentData)
					{
						delete [] pbyDocumentData;
						pbyDocumentData = NULL;
					}
					return;
				}

				Palette.SetPalette (m_byPalette, 256);
		
				switch (iDepth)
				{
				case 8:
					{					
						ClipboardDIB.GetDIBPalette( byClipboardPalette);

						for (j = 0; j < iSize; j++) 
						{
							b = byClipboardPalette[pbyClipboardData[j] * 4 + 0];
							g = byClipboardPalette[pbyClipboardData[j] * 4 + 1];
							r = byClipboardPalette[pbyClipboardData[j] * 4 + 2];
					
							pTemp24Bit[j] = IRGB( 0, r, g, b);
						}
					}
					break;

				case 24:
					{					
						for (j = 0; j < iSize; j++) 
						{
							b = pbyClipboardData[j * 3 + 0];
							g = pbyClipboardData[j * 3 + 1];
							r = pbyClipboardData[j * 3 + 2];
					
							pTemp24Bit[j] = IRGB( 0, r, g, b);
						}
					}
					break;

				default:
					ASSERT (FALSE);
					AfxMessageBox ("Unsupported bit depth");
					if (pTemp24Bit)
					{
						delete [] pTemp24Bit;
						pTemp24Bit = NULL;
					}
					return;
					break;
				}
									
				Palette.Convert24BitTo256Color( pTemp24Bit, 
						pbyDocumentData, iWidth, iHeight, 0, GetDitherType(), FALSE);

				pItem->ReplaceData (pbyDocumentData, iWidth, iHeight);
				InvalidateRect (NULL, FALSE);
				pDoc->SetModifiedFlag (TRUE);

				if (pbyDocumentData)
				{
					delete [] pbyDocumentData;
					pbyDocumentData = NULL;
				}

				if (pTemp24Bit)
				{
					delete [] pTemp24Bit;
					pTemp24Bit = NULL;
				}
			}
		}
	}
}

void CBuildView::OnUpdateBuildviewPopupPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));	
}

void CBuildView::OnBuildviewPopupDelete() 
{
	CBuildDoc *pDoc = GetDocument();
	CBuildItem *pItem = pDoc->GetBuildTile (m_iPopupMenuTile);
	
	if (pItem)		
	{
		if (AfxMessageBox ("Are you sure you want to delete this tile?", MB_YESNO) == IDYES)
		{
			pItem->ReplaceData (NULL, 0, 0);
			InvalidateRect (NULL, FALSE);
		}
	}

}
