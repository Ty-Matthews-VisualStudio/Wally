// MdiClient.cpp : implementation file
//

// neal fix the non-blanking status bar bug

#include "stdafx.h"
#include "Wally.h"
#include "MdiClient.h"
#include "PrefabImages.h"
#include "DibSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStatusBar  m_wndStatusBar;

/////////////////////////////////////////////////////////////////////////////
// CWndMdiClient

CWndMdiClient::CWndMdiClient()
{
}

CWndMdiClient::~CWndMdiClient()
{
}


BEGIN_MESSAGE_MAP(CWndMdiClient, CWnd)
	//{{AFX_MSG_MAP(CWndMdiClient)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndMdiClient message handlers

void CWndMdiClient::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_wndStatusBar.SetPaneText (0,"",true);
	m_wndStatusBar.SetPaneText (1,"",true);
	m_wndStatusBar.SetPaneText (2,"",true);
	m_wndStatusBar.SetPaneText (4,"",true);
	//m_wndStatusBar.SetPaneText (5,"",true);
	
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CWndMdiClient::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	
	//return CWnd::OnEraseBkgnd(pDC);
}

void CWndMdiClient::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting
	CRect rcItem;
	GetClientRect( &rcItem);

	int rcWidth = rcItem.Width();
	int rcHeight = rcItem.Height();	
	
	CDC DC;
	DC.CreateCompatibleDC( &PaintDC );
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( &PaintDC, rcWidth, rcHeight );
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNSHADOW ));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, rcWidth, rcHeight, PATCOPY);
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);

	CPrefabImage piImage;
	LPBYTE pbyData = NULL;
	LPBYTE pbyPalette = NULL;

	piImage.CreatePrefabImage( PI_WALLY_LOGO, &pbyData, &pbyPalette, COLOR_BTNSHADOW );

	int w = piImage.GetImageWidth();
	int h = piImage.GetImageHeight();

	if( (pbyData) && (pbyPalette) )
	{		
		CDibSection dsView;	
		dsView.Init( w, h, 8, pbyPalette, FALSE );
		dsView.SetRawBits (pbyData);
		dsView.Show (&DC, (rcWidth / 2) - (w / 2), (rcHeight / 2 ) - (h / 2), 1.0 );
	}
	
	PaintDC.BitBlt( 0, 0, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);
	DC.DeleteDC();
}

void CWndMdiClient::InvalidateRect( LPCRECT lpRect, BOOL bErase /* = TRUE */ )
{
	CWnd::InvalidateRect (lpRect, FALSE);	
}

void CWndMdiClient::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanging(lpwndpos);
	
	InvalidateRect( NULL, FALSE );
	
}
