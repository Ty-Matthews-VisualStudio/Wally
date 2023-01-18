// PackagePreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PackagePreviewWnd.h"
#include "DibSection.h"
#include "Layer.h"
//#include "WallyPal.h"
#include "WndList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackagePreviewWnd

CPackagePreviewWnd::CPackagePreviewWnd()
{
	SetWidth (0);
	SetHeight (0);
	SetPreviewWidth (256);
	SetPreviewHeight (256);

	m_byBits = NULL;
	m_byDecalBits = NULL;
	memset (m_byPalette, 0, 768);

	SetNonImageText ("This item is not a texture.");
	SetNonImageLocation (25, 115);
	SetType (PP_TYPE_NORMAL);	

	m_bNotRegistered = TRUE;
}

CPackagePreviewWnd::~CPackagePreviewWnd()
{
	if (m_byBits)
	{
		delete [] m_byBits;		// neal - must use array delete to delete arrays!
		m_byBits = NULL;
	}
}


BEGIN_MESSAGE_MAP(CPackagePreviewWnd, CWnd)
	//{{AFX_MSG_MAP(CPackagePreviewWnd)
	ON_WM_PAINT()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPackagePreviewWnd message handlers

void CPackagePreviewWnd::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting

	CRect rcItem;
	CRect rcButton;
	CRect rcDIB;
	GetClientRect( &rcItem);	

	int rcWidth = rcItem.Width();
	int rcHeight = rcItem.Height();	
	
	CDC DC;
	DC.CreateCompatibleDC( &PaintDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( &PaintDC, rcWidth, rcHeight);
	DC.SelectObject( Bmp);
	
	// clear background
//	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, rcWidth, rcHeight, PATCOPY);
	SelectObject( DC, hOldBrush);
//	DeleteObject( hBrush);	

	int iPreviewWidth = GetPreviewWidth();
	int iPreviewHeight = GetPreviewHeight();

	rcButton.SetRect (0, 0, iPreviewWidth + 4, iPreviewHeight + 4);
	DC.DrawFrameControl (&rcButton, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);

	int w = GetWidth();
	int h = GetHeight();

	BYTE byPackedPal[3*256];

	switch (GetType())
	{
	// neal - this method gives both cases a "This item is not a texture" msg

	case PP_TYPE_NORMAL:
		{
			// Can't use the palette from g_CurrentPalette because some other doc
			// might have the focus... only when we're a Decal preview wnd do we 
			// want that palette.
			memcpy (byPackedPal, m_byPalette, 256 * 3);
		}
		break;

	case PP_TYPE_DECAL:
		{
			g_CurrentPalette.GetPalette( byPackedPal, 256);

			if (m_bNotRegistered)
			{
				g_InvalidateList.AddItem (this, INVALIDATE_PALETTE_CHANGE | INVALIDATE_AMOUNT_CHANGE | INVALIDATE_DECAL_SELECTION_CHANGE);
				m_bNotRegistered = FALSE;
			}
		}
		break;

	default:
		ASSERT (false);
		break;
	}

	if (m_byBits && (w > 0) && (h > 0))		// Neal - Quake1 fix?
	{		
		rcDIB.SetRect (2, 2, iPreviewWidth + 2, iPreviewHeight + 2);				
		
		CDibSection dsView;	
		dsView.Init (w, h, 8, byPackedPal);
				
		if (GetType() == PP_TYPE_DECAL)
		{
			// Ty - speed up... with multiple pages now, don't recalc the decal bits
			// unless something has changed (image selection, left/right colors, etc)
			if (g_bUpdateDecalPreviewWnd)
			{
				dsView.SetRawBits (GetBits());

				CWallyPalette Palette;
				Palette.SetPalette( GetPalette(), 256);
				//Palette = g_CurrentPalette;
				
				CLayer Layer;
				BYTE*  pbyTexels = dsView.GetBits();
				int    iPercent  = ToolAmountToPercent( g_iCurrentTool);

				DWORD dwFlags = FLAG_DECAL_CLEAR_BACKGROUND;

				// TODO - implement
				// if (this decal usese left/right button colors)
					dwFlags |= FLAG_DECAL_USE_DRAWING_COLORS;

				for (int j = 0; j < h; j++)
				{
					for (int i = 0; i < w; i++)
					{
						int iTexel = pbyTexels[j*w + i];	// a byte

						COLOR_IRGB irgbTexel = Palette.GetRGB( iTexel);

						//TRACE1( "%6X ", irgbTexel);

	//							Layer.GetAppliedDecalPixelRGB( NULL, &g_CurrentPalette, 
						Layer.GetAppliedDecalPixelRGB( NULL, &Palette, 
								&irgbTexel, i, j, iPercent, dwFlags);
						//TRACE1( "%6X ", irgbTexel);

						int r = GetRValue( irgbTexel);
						int g = GetGValue( irgbTexel);
						int b = GetBValue( irgbTexel);

	//							pbyTexels[j*w + i] = GetIValue( irgbTexel);
						pbyTexels[j*w + i] = g_CurrentPalette.FindNearestColor( r, g, b,
	//									//(iTexel >= 256 - Palette.GetNumFullBrights()));
								FALSE);
						//TRACE1( "%6X ", g_CurrentPalette.GetRGB( pbyTexels[j*w + i]));
					}
					//TRACE0( "\n");
				}
				//BYTE byPackedPal[3*256];
				//g_CurrentPalette.GetPalette( byPackedPal, 256);
				//dsView.RebuildPalette( byPackedPal);

				// Store these away now
				dsView.GetRawBits (m_byDecalBits);
				g_bUpdateDecalPreviewWnd = FALSE;
			}
			else
			{
				dsView.SetRawBits (m_byDecalBits);				
			}

			if ((w < 48) && (h < 48))
			{
				dsView.Show (&DC, 2, 2, 4.0);
			}
			else
			{	
				dsView.ShowRestricted (&DC, rcDIB);
			}

		}			// if (GetType() == PP_TYPE_DECAL)
		else
		{			
			ASSERT (FALSE);	// The WAD viewer is not using this anymore; just trying to see where it 
							// might be picked up

			// We're a WAD preview wnd
			dsView.SetRawBits (GetBits());

			if ((w < 64) && (h < 64))
			{
				dsView.Show (&DC, 2, 2, 4.0);
			}
			else
			{	
				dsView.ShowRestricted (&DC, rcDIB);
			}
		}		
	}
	else
	{
		CFont TextFont, *pOldFont;
		
		DC.SetBkColor (GetSysColor( COLOR_BTNFACE));
		TextFont.CreateFont (24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS,
			"MS Sans Serif");		
		
		pOldFont = DC.SelectObject (&TextFont);
		CString strText (GetNonImageText());

		CPoint ptLoc (GetNonImageLocation());
		
		// Write to the screen!
		DC.ExtTextOut(ptLoc.x, ptLoc.y, ETO_CLIPPED, &rcButton, strText, strText.GetLength(), NULL);
		DC.SelectObject (pOldFont);
		TextFont.DeleteObject();
	}

	PaintDC.BitBlt( 0, 0, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);
	DC.DeleteDC();
	// Do not call CWnd::OnPaint() for painting messages
}

void CPackagePreviewWnd::SetWidth (int iWidth)
{
	m_iWidth = iWidth;
}

void CPackagePreviewWnd::SetHeight (int iHeight)
{
	m_iHeight = iHeight;
}

int CPackagePreviewWnd::GetWidth()
{
	return m_iWidth;
}	

int CPackagePreviewWnd::GetHeight()
{
	return m_iHeight;
}

void CPackagePreviewWnd::SetPreviewWidth (int iWidth)
{
	m_iPreviewWidth = iWidth;
}

void CPackagePreviewWnd::SetPreviewHeight (int iHeight)
{
	m_iPreviewHeight = iHeight;
}

int CPackagePreviewWnd::GetPreviewWidth ()
{
	return m_iPreviewWidth;
}

int CPackagePreviewWnd::GetPreviewHeight ()
{
	return m_iPreviewHeight;
}

BYTE *CPackagePreviewWnd::GetBits()
{
	return m_byBits;
}

BYTE *CPackagePreviewWnd::GetDecalBits()
{
	return m_byDecalBits;
}

void CPackagePreviewWnd::SetNonImageText (LPCTSTR szText)
{
	m_strNonImageText = szText;
}

CString CPackagePreviewWnd::GetNonImageText ()
{
	return m_strNonImageText;
}

void CPackagePreviewWnd::SetNonImageLocation (int x, int y)
{
	m_ptNonImageLocation.x = x;
	m_ptNonImageLocation.y = y;
}

CPoint CPackagePreviewWnd::GetNonImageLocation ()
{
	return m_ptNonImageLocation;
}

unsigned char *CPackagePreviewWnd::GetPalette()
{
	return m_byPalette;
}

void CPackagePreviewWnd::InitImage	(int iWidth, int iHeight, unsigned char *pbyBits, unsigned char *pbyPalette)
{
/*#ifdef _DEBUG
	ASSERT (pbyBits);
	ASSERT (pbyPalette);
	ASSERT ((iWidth > 0) && (iHeight > 0));
#endif*/

	SetWidth (iWidth);
	SetHeight (iHeight);

	if (m_byBits)
	{
		delete [] m_byBits;		// neal - must use array delete to delete arrays!
		m_byBits = NULL;
	}

	if ((GetType() == PP_TYPE_DECAL) && (m_byDecalBits))
	{
		delete []m_byDecalBits;
		m_byDecalBits = NULL;
	}

//	if (!pbyBits)
	if (!pbyBits && (iWidth != 0) && (iHeight != 0))		// Neal - fix Quake1?
	{
		// Item is not a MIP, so don't build the mip buffer
	}
	else
	{	
		int iSize = iWidth * iHeight;
		m_byBits = new BYTE[iSize];	
	
		memcpy (m_byBits, pbyBits, iSize);
		memcpy (m_byPalette, pbyPalette, 768);

		if (GetType() == PP_TYPE_DECAL)
		{
			m_byDecalBits = new BYTE[iSize];

			// Flag it so we rebuild the decal image
			g_bUpdateDecalPreviewWnd = TRUE;
		}
	}
}



void CPackagePreviewWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	CWnd::OnRButtonUp(nFlags, point);
	
	CWnd* pParent = GetParent();
	if (pParent)
	{
		// neal - map mouse pos to parent

		ClientToScreen( &point);
		pParent->ScreenToClient( &point);

		// neal - tell our parent to bring up a pop-up menu

		pParent->SendMessage( WM_RBUTTONUP, nFlags, MAKELONG( point.x, point.y));
	}	
}

void CPackagePreviewWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CWnd::OnLButtonDblClk(nFlags, point);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->SendMessage( WM_LBUTTONDBLCLK, nFlags, MAKELONG( point.x, point.y));
	}	
}

void CPackagePreviewWnd::SetType (int iType)
{
	m_iPreviewType = iType;
}

int CPackagePreviewWnd::GetType ()
{
	return m_iPreviewType;
}


BOOL CPackagePreviewWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CPackagePreviewWnd::OnNcPaint() 
{
	
	// Do not call CWnd::OnNcPaint() for painting messages
}
