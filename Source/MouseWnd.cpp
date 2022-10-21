// MouseWnd.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "MouseWnd.h"
#include "ImageHelper.h"
#include "PrefabImages.h"
#include "DibSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_MOUSE_WND_CUSTOM = RegisterWindowMessage ("WM_MOUSE_WND_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CMouseWnd

CMouseWnd::CMouseWnd()
{
	m_iButtonState = 0;
	SetButtonState (MOUSE_STATE_NORMAL);
}

CMouseWnd::~CMouseWnd()
{
}


BEGIN_MESSAGE_MAP(CMouseWnd, CWnd)
	//{{AFX_MSG_MAP(CMouseWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMouseWnd message handlers

void CMouseWnd::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting

	CRect rcItem;	
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

	CPrefabImage piImage;
	unsigned char *pbyData = NULL;
	unsigned char *pbyPalette = NULL;

	if (IsDisabled())
	{
		piImage.CreatePrefabImage (MOUSE_STATE_DISABLED, &pbyData, &pbyPalette);
	}
	else
	{
		if (IsRightButtonDisabled())
		{
			if (IsLeftButtonDown())
			{
				piImage.CreatePrefabImage (PI_MOUSE_LEFT_RBUTTON_DISABLED, &pbyData, &pbyPalette);
			}
			else
			{
				piImage.CreatePrefabImage (PI_MOUSE_RBUTTON_DISABLED, &pbyData, &pbyPalette);
			}
		}
		else
		{
			if (IsLeftButtonDown())
			{
				piImage.CreatePrefabImage (PI_MOUSE_LEFT, &pbyData, &pbyPalette);
			}
			else
			{
				if (IsRightButtonDown())
				{
					piImage.CreatePrefabImage (PI_MOUSE_RIGHT, &pbyData, &pbyPalette);
				}
				else
				{
					piImage.CreatePrefabImage (PI_MOUSE, &pbyData, &pbyPalette);
				}
			}
		}

	}
	
	int w = piImage.GetImageWidth();
	int h = piImage.GetImageHeight();

	if ((pbyData) && (pbyPalette))
	{		
		CDibSection dsView;	
		dsView.Init (w, h, 8, pbyPalette, FALSE);
		dsView.SetRawBits (pbyData);
		dsView.Show (&DC, 0, 0, 1.0);
	}

	PaintDC.BitBlt( 0, 0, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);
	DC.DeleteDC();

	// Do not call CWnd::OnPaint() for painting messages
}


void CMouseWnd::HandleLButtonDown (CPoint point)
{
	SetCapture ();
	SetButtonState (MOUSE_STATE_LBUTTON_DOWN);
	Update();	
}

void CMouseWnd::HandleRButtonDown (CPoint point)
{
	SetCapture ();
	SetButtonState (MOUSE_STATE_RBUTTON_DOWN);
	Update();	
}

void CMouseWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CWnd::OnLButtonDown(nFlags, point);
	HandleLButtonDown (point);
}

void CMouseWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CWnd::OnLButtonDblClk(nFlags, point);
	HandleLButtonDown (point);
}

void CMouseWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CWnd::OnRButtonDown(nFlags, point);
	HandleRButtonDown (point);
}

void CMouseWnd::OnRButtonDblClk(UINT nFlags, CPoint point) 
{	
	CWnd::OnRButtonDblClk(nFlags, point);
	HandleRButtonDown (point);
}


void CMouseWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	RemoveButtonState (MOUSE_STATE_LBUTTON_DOWN);
	CWnd::OnLButtonUp(nFlags, point);
	Update();

	CRect rcItem;	
	GetClientRect( &rcItem);

	if ((point.x < rcItem.left) || (point.x > rcItem.right) || (point.y < rcItem.top) || (point.y > rcItem.bottom))
	{
		// Point is off our button		
	}
	else
	{		
		CWnd* pParent = GetParent();	
		if (pParent)
		{			
			pParent->SendMessage( WM_MOUSE_WND_CUSTOM, MOUSE_WND_LBUTTON_SELECT, 0);
		}
	}

}

void CMouseWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	ReleaseCapture();
	RemoveButtonState (MOUSE_STATE_RBUTTON_DOWN);
	CWnd::OnRButtonUp(nFlags, point);
	Update();

	CRect rcItem;	
	GetClientRect( &rcItem);

	if ((point.x < rcItem.left) || (point.x > rcItem.right) || (point.y < rcItem.top) || (point.y > rcItem.bottom))
	{
		// Point is off our button		
	}
	else
	{		
		CWnd* pParent = GetParent();	
		if (pParent)
		{			
			pParent->SendMessage( WM_MOUSE_WND_CUSTOM, MOUSE_WND_RBUTTON_SELECT, 0);
		}
	}
}

void CMouseWnd::Update()
{
	if (m_hWnd)
		InvalidateRect (NULL, false);
}

void CMouseWnd::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
		
	if (bEnable)
	{	
		SetButtonState (MOUSE_STATE_NORMAL);
	}
	else
	{
		SetButtonState (MOUSE_STATE_DISABLED);
	}	
	Update();
}
