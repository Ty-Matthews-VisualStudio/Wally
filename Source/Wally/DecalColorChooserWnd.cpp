// DecalColorChooserWnd.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "DecalColorChooserWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_DECALCOLORCHOOSERWND_CUSTOM = RegisterWindowMessage( "WM_DECALCOLORCHOOSERWND_CUSTOM" );

/* Static variables */
int CDecalColorChooserWnd::DECAL_COLOR_CHOOSER_BUTTON_WIDTH = 40;
int CDecalColorChooserWnd::DECAL_COLOR_CHOOSER_BUTTON_HEIGHT = 15;

/////////////////////////////////////////////////////////////////////////////
// CDecalColorChooserWnd

CDecalColorChooserWnd::CDecalColorChooserWnd() : m_bLeftButtonDown( FALSE )
{	
}

CDecalColorChooserWnd::~CDecalColorChooserWnd()
{
}


BEGIN_MESSAGE_MAP(CDecalColorChooserWnd, CWnd)
	//{{AFX_MSG_MAP(CDecalColorChooserWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDecalColorChooserWnd message handlers

void CDecalColorChooserWnd::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	SetCapture();
	m_bLeftButtonDown = TRUE;	
	CWnd::OnLButtonDown( nFlags, point );
	Update();
}

void CDecalColorChooserWnd::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ReleaseCapture();	
	CWnd::OnLButtonUp( nFlags, point );

	if( !m_bLeftButtonDown )
	{
		return;
	}

	m_bLeftButtonDown = FALSE;	
	Update();
	CRect rcItem;
	GetClientRect( &rcItem);

	if (
		(rcItem.left < point.x) && 
		(rcItem.right > point.x) &&
		(rcItem.top < point.y) &&
		(rcItem.bottom > point.y)
		)
	{
		CWnd* pParent = GetParent();	
		if (pParent)
		{			
			pParent->SendMessage( WM_DECALCOLORCHOOSERWND_CUSTOM, DECAL_COLOR_CHOOSER_WND_LBUTTON_SELECT, 0 );
		}
	}	
}

void CDecalColorChooserWnd::Update()
{
	InvalidateRect( NULL, false );
}

void CDecalColorChooserWnd::SetColor( BYTE byRed, BYTE byGreen, BYTE byBlue )
{
	m_byRed = byRed;
	m_byGreen = byGreen;
	m_byBlue = byBlue;
}

void CDecalColorChooserWnd::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting
	
	CRect rcItem;
	GetClientRect( &rcItem);

	CDC DC;
	DC.CreateCompatibleDC( &PaintDC);
	CBitmap Bmp;	
	Bmp.CreateCompatibleBitmap( &PaintDC, rcItem.Width(), rcItem.Height());
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, 10000, 10000, PATCOPY);
	
	int iColor = 0;

	CPen PenEdge( PS_SOLID, 1, GetSysColor( COLOR_BTNFACE ));
	DC.SelectObject( PenEdge );

	SelectObject( DC, hOldBrush );

	CRect rButton( 0, 0, DECAL_COLOR_CHOOSER_BUTTON_WIDTH + 2, DECAL_COLOR_CHOOSER_BUTTON_HEIGHT + 2 );

	if( m_bLeftButtonDown )
	{
		DC.DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
	}
	else
	{
		DC.DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH);
	}	

	hBrush = CreateSolidBrush( RGB( m_byRed, m_byGreen, m_byBlue));
	SelectObject( DC, hBrush);
	
	DC.Rectangle( 1, 1, DECAL_COLOR_CHOOSER_BUTTON_WIDTH, DECAL_COLOR_CHOOSER_BUTTON_HEIGHT );
	DC.SelectStockObject( NULL_BRUSH );	
	SelectObject( DC, hOldBrush );
	DeleteObject( hBrush );

	PaintDC.BitBlt( 0, 0, 10000, 10000, &DC, 0, 0, SRCCOPY );

	DC.DeleteDC();	
}
