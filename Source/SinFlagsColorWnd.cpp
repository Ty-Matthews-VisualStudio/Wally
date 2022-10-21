// SinFlagsColorWnd.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "SinFlagsColorWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_SINFLAGSWND_CUSTOM = RegisterWindowMessage ("WM_SINFLAGSWND_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CSinFlagsColorWnd

CSinFlagsColorWnd::CSinFlagsColorWnd()
{
	SetColor (1.0, 1.0, 1.0);
	m_bLeftButtonDown = FALSE;
}

CSinFlagsColorWnd::~CSinFlagsColorWnd()
{
}


BEGIN_MESSAGE_MAP(CSinFlagsColorWnd, CWnd)
	//{{AFX_MSG_MAP(CSinFlagsColorWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSinFlagsColorWnd message handlers

void CSinFlagsColorWnd::OnPaint() 
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
//	HBRUSH hBrush    = CreateSolidBrush( GetSysColor( COLOR_BTNFACE));
	HBRUSH hBrush    = GetSysColorBrush( COLOR_BTNFACE);
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, 10000, 10000, PATCOPY);
	
	int iColor = 0;

	CPen PenEdge( PS_SOLID, 1, GetSysColor( COLOR_BTNFACE));
	DC.SelectObject( PenEdge);

	SelectObject( DC, hOldBrush);
//	DeleteObject( hBrush);

	CRect rButton (0, 0, COLOR_BUTTON_WIDTH + 4, COLOR_BUTTON_HEIGHT + 4);

	if (m_bLeftButtonDown)
	{
		DC.DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
	}
	else
	{
		DC.DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH);
	}	

	hBrush = CreateSolidBrush( RGB( m_byRed, m_byGreen, m_byBlue));
	SelectObject( DC, hBrush);
	
	DC.Rectangle (3, 3, COLOR_BUTTON_WIDTH, COLOR_BUTTON_HEIGHT);

	DC.SelectStockObject( NULL_BRUSH);
	CRect Rect;
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);

	PaintDC.BitBlt( 0, 0, 10000, 10000, &DC, 0, 0, SRCCOPY);

	DC.DeleteDC();	

}

void CSinFlagsColorWnd::Update()
{
	InvalidateRect (NULL, false);
}

void CSinFlagsColorWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_bLeftButtonDown = TRUE;	
	CWnd::OnLButtonDown(nFlags, point);
	Update();
}

void CSinFlagsColorWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bLeftButtonDown = FALSE;	
	CWnd::OnLButtonUp(nFlags, point);
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
			pParent->SendMessage( WM_SINFLAGSWND_CUSTOM, SINFLAGS_WND_LBUTTON_SELECT, 0);
		}
	}	

}
