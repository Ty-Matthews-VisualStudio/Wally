// Package2FormListBox.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "Package2FormListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormListBox

CPackage2FormListBox::CPackage2FormListBox()
{	
	m_foDrawText.CreateFont (6, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"MS Sans Serif");
	m_brColor.CreateSolidBrush (GetSysColor (COLOR_WINDOW));
}

CPackage2FormListBox::~CPackage2FormListBox()
{	
	m_foDrawText.DeleteObject();
	m_brColor.DeleteObject();
}


BEGIN_MESSAGE_MAP(CPackage2FormListBox, CListBox)
	//{{AFX_MSG_MAP(CPackage2FormListBox)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormListBox message handlers

void CPackage2FormListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{	
	//  If nothing to draw, exit now
	if( lpDrawItemStruct->itemID == LB_ERR )
	{
		return;
	}

	CDC DC;
	DC.Attach (lpDrawItemStruct->hDC);	
	RECT rcItem;
	CBrush *pBrush = NULL;

	CFont *pOldFont = DC.SelectObject (&m_foDrawText);
	
	CString strName ("");

	//  Get RECT of item that needs updated
	//  Don't use lpDs->rcItem as that's only the text area	
	SendMessage( LB_GETITEMRECT, lpDrawItemStruct->itemID,
			  (LPARAM)&rcItem);

	char szText[_MAX_PATH];
	SendMessage( LB_GETTEXT, lpDrawItemStruct->itemID, (LPARAM)szText);
	
	strName = szText;
	CRect rcClient (rcItem);

	int rcWidth = rcClient.Width();
	int rcHeight = rcClient.Height();

	bool bIsSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);

	//#define HIGHLIGHT_BGCOLOR		RGB (0, 0, 128)
	#define HIGHLIGHT_BGCOLOR		GetSysColor (COLOR_HIGHLIGHT)
	//#define LOWLIGHT_BGCOLOR		RGB (255, 255, 255)
	#define LOWLIGHT_BGCOLOR		GetSysColor( COLOR_WINDOW)
	#define HIGHLIGHT_MIP_TEXT		GetSysColor( COLOR_HIGHLIGHTTEXT)
	#define LOWLIGHT_MIP_TEXT		GetSysColor( COLOR_WINDOWTEXT)		
	//#define HIGHLIGHT_NONMIP_TEXT	RGB (128, 128, 128)
	#define HIGHLIGHT_NONMIP_TEXT	GetSysColor (COLOR_INACTIVECAPTIONTEXT)
	//#define LOWLIGHT_NONMIP_TEXT	RGB (128, 128, 128)
	#define LOWLIGHT_NONMIP_TEXT	GetSysColor (COLOR_INACTIVECAPTION)
	
	pBrush = new CBrush ( bIsSelected ? HIGHLIGHT_BGCOLOR : LOWLIGHT_BGCOLOR);
	DC.FillRect (rcClient, pBrush);
	delete pBrush;
	pBrush = NULL;

	/*if (bIsMip)
	{
	*/
		DC.SetTextColor( bIsSelected ? HIGHLIGHT_MIP_TEXT : LOWLIGHT_MIP_TEXT);
	/*}
	else
	{
		DC.SetTextColor( bIsSelected ? HIGHLIGHT_NONMIP_TEXT : LOWLIGHT_NONMIP_TEXT);
	}
	*/
	DC.SetBkMode( TRANSPARENT );		
	
	//  and display the text
	DC.TextOut( 2, lpDrawItemStruct->rcItem.top + 1, strName, strName.GetLength());
	DC.SelectObject (pOldFont);
	
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		DC.DrawFocusRect (rcClient);
	}
	
	DC.Detach();
	
	
}

void CPackage2FormListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemWidth = 50;
	lpMeasureItemStruct->itemHeight = P2_FORM_LB_ITEMHEIGHT;
}

BOOL CPackage2FormListBox::OnEraseBkgnd(CDC* pDC) 
{
	CRect rcClient;
	GetClientRect( &rcClient);
	int iWidth = rcClient.Width();
	int iHeight = rcClient.Height();
	
	CDC DC;
	DC.CreateCompatibleDC( pDC);
	CBitmap Bmp;	
	Bmp.CreateCompatibleBitmap( pDC, iWidth, iHeight);
	DC.SelectObject( Bmp);
	
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, (HBRUSH)m_brColor);
		
	::PatBlt( DC, 0, 0, iWidth, iHeight, PATCOPY);

	// Only erase the part that won't be drawn during OnPaint()
	int iSBPos = GetScrollPos (SB_VERT);

	int iYOffset = (GetCount() - iSBPos) * P2_FORM_LB_ITEMHEIGHT;
	pDC->BitBlt (0, iYOffset, iWidth, iHeight - iYOffset, &DC, 0, 0, SRCCOPY);

	DC.SelectStockObject( NULL_BRUSH);
	SelectObject( DC, hOldBrush);

	DC.DeleteDC();
	
	//return CListBox::OnEraseBkgnd(pDC);
	return TRUE;
}
