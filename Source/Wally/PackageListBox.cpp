// PackageListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "PackageListBox.h"
#include "WADList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
int WM_PACKAGELISTBOX_CUSTOM = RegisterWindowMessage ("WM_PACKAGELISTBOX_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CPackageListBox

CPackageListBox::CPackageListBox()
{
	m_iBaseShiftIndex = 0;
	m_bRightButtonDown = false;
	m_bDragging = false;
	m_brColor.CreateSolidBrush (GetSysColor (COLOR_WINDOW));
}

CPackageListBox::~CPackageListBox()
{
}


BEGIN_MESSAGE_MAP(CPackageListBox, CListBox)
	//{{AFX_MSG_MAP(CPackageListBox)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()	
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageListBox message handlers

void CPackageListBox::OnRButtonDown( UINT uFlags, CPoint ptPos) 
{
	int iSelCount = GetSelCount();

	// If either the shift or the control key is down, don't act like a left-click; we're going
	// to be dragging soon
	bool bShiftKey = IsKeyPressed (VK_SHIFT);
	bool bControlKey = IsKeyPressed (VK_CONTROL);

	if ((iSelCount == 1) && (!bShiftKey) && (!bControlKey))
	{
		UINT uLeftFlags = uFlags;

		// neal - doesn't matter if right button is also down

		//uLeftFlags &= ~MK_RBUTTON;
		uLeftFlags |= MK_LBUTTON;

		LONG lPos = MAKELONG( ptPos.x, ptPos.y);

		// neal - set the selection in the listbox by faking a left-click

		SendMessage( WM_LBUTTONDOWN, uLeftFlags, lPos);
		SendMessage( WM_LBUTTONUP,   uLeftFlags, lPos);
	}
	else
	{		
	}

	// neal - now really handle the right button down

	CListBox::OnRButtonDown( uFlags, ptPos);

	m_bRightButtonDown = true;
	m_bDragging = false;
	m_ptRightButtonDown.x = ptPos.x;
	m_ptRightButtonDown.y = ptPos.y;
}

void CPackageListBox::OnRButtonUp( UINT uFlags, CPoint ptPos) 
{
	CListBox::OnRButtonUp( uFlags, ptPos);

	CWnd* pParent = GetParent();
	if (pParent)
	{
		// neal - map mouse pos to parent

		ClientToScreen( &ptPos);
		pParent->ScreenToClient( &ptPos);

		// neal - tell our parent to bring up a pop-up menu

		pParent->SendMessage( WM_RBUTTONUP, uFlags, MAKELONG( ptPos.x, ptPos.y));
	}
	m_bRightButtonDown = false;
	m_bDragging = false;
}

void CPackageListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bRightButtonDown)
	{		
		int x = m_ptRightButtonDown.x;
		int y = m_ptRightButtonDown.y;

		int x1 = point.x;
		int y1 = point.y;

		int xd = abs (x - x1);
		int yd = abs (y - y1);

		if ((xd > 5) || (yd > 5))
		{
			if (!m_bDragging)
			{				
				COleDataSource*	pSource = new COleDataSource();
				CSharedFile	sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

				CWnd *pParent = GetParent();				
				
				// Just write out our parent's address, the other packageview will take it from there
				sf.Write(&pParent, sizeof(CWnd *));
				HGLOBAL hMem = sf.Detach();	
				
				if (!hMem) return;
				pSource->CacheGlobalData(g_iPackageFormat, hMem);	
				pSource->DoDragDrop();

				if (pSource)
				{
					delete pSource;
					pSource = NULL;
				}
				m_bDragging = true;
			}
		}
	}

	
	CListBox::OnMouseMove(nFlags, point);
}

int CPackageListBox::CalcItemSize (CWADItem *pItem)
{
	ASSERT (pItem);
	
	ASSERT (pItem->IsValidMip());
	
	int iWidth = pItem->GetWidth();
	int iHeight = pItem->GetHeight();
		
	int iWidths[4];
	int iHeights[4];
	int iTotalSize = 0;

	for (int j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
		iTotalSize += (iWidths[j] * iHeights[j]);
	}
	
	return iTotalSize;
}


void CPackageListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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
	
	CWADItem *pItem = (CWADItem *)(lpDrawItemStruct->itemData);	
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

	bool bIsMip = false;
	if (pItem)
	{
		bIsMip = pItem->IsValidMip();		
	}

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

	if (bIsMip)
	{
		DC.SetTextColor( bIsSelected ? HIGHLIGHT_MIP_TEXT : LOWLIGHT_MIP_TEXT);
	}
	else
	{
		DC.SetTextColor( bIsSelected ? HIGHLIGHT_NONMIP_TEXT : LOWLIGHT_NONMIP_TEXT);
	}
	DC.SetBkMode( TRANSPARENT );		
	
	//  and display the text
	DC.TextOut( 2, lpDrawItemStruct->rcItem.top + 1, strName, strName.GetLength());
	
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		DC.DrawFocusRect (rcClient);
	}	
	DC.Detach();
	
}

UINT CPackageListBox::OnGetDlgCode() 
{
	UINT result = CListBox::OnGetDlgCode();
	result |= DLGC_WANTALLKEYS;	
	return result; 
}

void CPackageListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	// Handle those keys that aren't being passed properly.  Where the heck
	// is the default code for CListBox in the MFC source?

	// neal: the problem was that the arrow keys were set as accelerators
	// that gives the app priority over the control (for those keystrokes)
	
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	CWnd* pParent = GetParent();
	int iCount = GetCount();
	int *piIndexes = NULL;	
	
	switch (nChar)
	{
	case VK_RETURN:
		if (pParent)
		{			
			pParent->SendMessage( WM_PACKAGELISTBOX_CUSTOM, PACKAGELISTBOX_OPENTEXTURE, 0);
		}
		break;

	case VK_SHIFT:
		// Keep track of the the top index currently selected
		iCount = GetCount();
		piIndexes = new int[iCount];
		GetSelItems (iCount, piIndexes);
		m_iBaseShiftIndex = piIndexes[0];
		if (piIndexes)
		{
			delete [] piIndexes;	// neal - must use array delete to delete arrays!
			piIndexes = NULL;
		}
		break;
/*
	case VK_DOWN:
	case VK_UP:		
		{
			bool bShiftKey = IsKeyPressed( VK_SHIFT);
			int iIncrement = (nChar == VK_UP ? (-1 * nRepCnt) : nRepCnt);
			int iIndex = GetCaretIndex();
			int iCurrentIndex = iIndex;
			int iCount = GetCount();
			int iTopIndex = 0;
			int iBottomIndex = 0;
			
			iIndex += iIncrement;

			iIndex = max (iIndex, 0);
			iIndex = min (iIndex, iCount - 1);

			if (iIndex == iCurrentIndex)
			{
				// Position hasn't changed, get out!
				return;
			}
			
			SelItemRange (false, 0, iCount - 1);

			if (bShiftKey)
			{
				// Set all of the items between our top and bottom indexes
				iTopIndex = min (m_iBaseShiftIndex, iIndex);
				iBottomIndex = max (m_iBaseShiftIndex, iIndex);

				// Make sure we're in bounds
				iTopIndex		= max (iTopIndex, 0);
				iBottomIndex	= min (iBottomIndex, iCount - 1);

				SelItemRange (true, iTopIndex, iBottomIndex);
			}
			SetSel (iIndex, true);			
			SetCaretIndex (iIndex, true);
			CWnd* pParent = GetParent();
			if (pParent)
			{			
				pParent->SendMessage( WM_PACKAGELISTBOX_CUSTOM, PACKAGELISTBOX_SELCHANGE, 0);
			}
		}	
		break;
		*/
		
	default:
		// Do nothing, we don't care what key they hit
		break;
	}
}


int CPackageListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL CPackageListBox::PreTranslateMessage(MSG* pMsg) 
{	
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_DOWN) || (pMsg->wParam == VK_UP) || (pMsg->wParam == VK_RETURN))
		{
			OnKeyDown (pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
		}		
	}
	return CListBox::PreTranslateMessage(pMsg);
}

BOOL CPackageListBox::OnEraseBkgnd(CDC* pDC) 
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

	int iYOffset = (GetCount() - iSBPos) * PFORM_LB_ITEMHEIGHT;
	pDC->BitBlt (0, iYOffset, iWidth, iHeight - iYOffset, &DC, 0, 0, SRCCOPY);

	DC.SelectStockObject( NULL_BRUSH);
	SelectObject( DC, hOldBrush);

	DC.DeleteDC();
	
	//return CListBox::OnEraseBkgnd(pDC);
	return TRUE;
}

BOOL CPackageListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	return TRUE;
	//return CListBox::OnMouseWheel(nFlags, zDelta, pt);
}
