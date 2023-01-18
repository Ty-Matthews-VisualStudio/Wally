// DecalListBox.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "DecalListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
int WM_DECALLISTBOX_CUSTOM = RegisterWindowMessage ("WM_DECALLISTBOX_CUSTOM");

#define IsKeyPressed( vk) (GetKeyState( vk) < 0)

/////////////////////////////////////////////////////////////////////////////
// CDecalListBox

CDecalListBox::CDecalListBox()
{
}

CDecalListBox::~CDecalListBox()
{
}


BEGIN_MESSAGE_MAP(CDecalListBox, CListBox)
	//{{AFX_MSG_MAP(CDecalListBox)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecalListBox message handlers

void CDecalListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	

	CWnd* pParent = GetParent();

	switch (nChar)
	{
	case VK_RETURN:			
		if (pParent)
		{
			pParent->SendMessage( WM_DECALLISTBOX_CUSTOM, DECALLISTBOX_ENTER, 0);			
		}
		break;

	// Ty- no longer need to trap this; it's duplicating keystrokes
	/*case VK_DOWN:
	case VK_UP:		
		{			
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
			
			SetCurSel (iIndex);			
			CWnd* pParent = GetParent();
			if (pParent)
			{			
				pParent->SendMessage( WM_DECALLISTBOX_CUSTOM, DECALLISTBOX_SELCHANGE, 0);
			}
		}	
		break;*/
		
	default:		
		CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
		break;
	}
}

BOOL CDecalListBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			OnKeyDown (pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
		}		
	}	
	return CListBox::PreTranslateMessage(pMsg);
}
