// STabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "STabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabDlgList


CTabDlgItem::CTabDlgItem (CWnd *pParent, int iResourceID, int iTabNumber)
{
	m_pWnd = NULL;
	m_pParent = pParent;
	m_iResourceID = iResourceID;
	m_iTabNumber = iTabNumber;
	m_bAlwaysHide = FALSE;
}

CTabDlgItem::CTabDlgItem (CWnd *pWnd, int iTabNumber)
{
	m_pWnd = pWnd;
	m_pParent = NULL;
	m_iResourceID = 0;
	m_iTabNumber = iTabNumber;
	m_bAlwaysHide = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CTabDlgList

BOOL CTabDlgList::AddItem(CWnd *pParent, int iResourceID, int iTabNumber)
{
	m_Items.emplace_back (pParent, iResourceID, iTabNumber);
	return TRUE;
}

BOOL CTabDlgList::AddItem(CWnd *pWnd, int iTabNumber)
{
	m_Items.emplace_back (pWnd, iTabNumber);
	return TRUE;
}

void CTabDlgList::NeverShowControl (int iID)
{
	for (CTabDlgItem &item : m_Items)
	{
		if (item.GetResourceID() == iID)
		{
			item.SetAlwaysHide (TRUE);
		}
	}
}

void CTabDlgList::HideShowItems (int iTabNumber)
{
	for (CTabDlgItem &item : m_Items)
	{
		CWnd *pParent = item.GetParent();
		CWnd *pItem = pParent ? pParent->GetDlgItem (item.GetResourceID()) : item.GetWnd();

		if (pItem && ::IsWindow(pItem->m_hWnd))
		{
			pItem->ShowWindow( ((item.GetTabNumber() == iTabNumber) && (!item.AlwaysHide())) ? SW_SHOW : SW_HIDE);
			pItem->InvalidateRect( NULL, FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSTabCtrl

CSTabCtrl::CSTabCtrl()
{	
}

CSTabCtrl::~CSTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CSTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CSTabCtrl)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTabCtrl message handlers

int CSTabCtrl::SetCurSel( int nItem )
{
	/*NMHDR _nDummyNMHDR;
	LRESULT _nDummyLRESULT;

	OnSelchange(&_nDummyNMHDR,&_nDummyLRESULT);*/
	m_TabItemList.HideShowItems (nItem);

	return CTabCtrl::SetCurSel(nItem);
}

void CSTabCtrl::NeverShowControl (int iID)
{
	m_TabItemList.NeverShowControl (iID);
}

BOOL CSTabCtrl::AttachControlToTab(CWnd *pParent, INT iID, INT iTabNum)
{
	return m_TabItemList.AddItem (pParent, iID, iTabNum);
	
	/*
	CWnd* pWnd = pParent->GetDlgItem( iID);

	if (pWnd)
	{
		return AttachControlToTab(pWnd, _nTabNum);
	}

	return FALSE;
	*/
}

BOOL CSTabCtrl::AttachControlToTab(CWnd *pWnd, INT iTabNum)
{
	return m_TabItemList.AddItem (pWnd, iTabNum);
}

/*
BOOL CSTabCtrl::AttachControlToTab(CWnd * _pControl,
									INT _nTabNum)
{

	if(_nTabNum >= GetItemCount())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CList <CWnd *, CWnd *> * _pCtrlList = NULL;

	if(!m_PageToCtrlListMap.Lookup(_nTabNum,_pCtrlList) || !_pCtrlList)
	{
		_pCtrlList = new CList <CWnd *, CWnd *>;
		m_PageToCtrlListMap.SetAt(_nTabNum, _pCtrlList);
	}

	_pCtrlList -> AddTail(_pControl);

	return TRUE;
}
*/

BOOL CSTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
	int iCurSel = GetCurSel();

	m_TabItemList.HideShowItems (iCurSel);

	*pResult = 0;
	return FALSE;
}

/*
BOOL CSTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	INT _nCurrSel = GetCurSel();

	// show controls on current page.

	POSITION _rPos[2] = { m_PageToCtrlListMap.GetStartPosition(), NULL };
	CList <CWnd *, CWnd *> * _pCtrlList = NULL;
	CWnd * _pCtrl = NULL;
	INT _rKey (0);

	while(_rPos[0])
	{
		m_PageToCtrlListMap.GetNextAssoc(_rPos[0],_rKey,_pCtrlList);

		_rPos[1] = _pCtrlList -> GetHeadPosition();

		while(_rPos[1])
		{
			_pCtrl = _pCtrlList -> GetNext(_rPos[1]);

			HWND hWnd = _pCtrl -> GetSafeHwnd();

			if (hWnd)		// BoundsChecker bitched about hWnd == NULL
				::ShowWindow( hWnd, _nCurrSel == _rKey ? SW_SHOW : SW_HIDE );
			// cannot use because using this call with the MS WebBrowser
			// control destroys the control's window.
			//_pCtrl -> ShowWindow(_nCurrSel == _rKey);
		}
	}

	*pResult = 0;

	return FALSE;	// allow control to handle as well.
}
*/


void CSTabCtrl::OnDestroy() 
{
	CTabCtrl::OnDestroy();
	
	// clean up map.
	POSITION _rPos = m_PageToCtrlListMap.GetStartPosition();
	CList <CWnd *, CWnd *> * _pValue = NULL;
	INT _rKey;

	while(_rPos)
	{
		m_PageToCtrlListMap.GetNextAssoc(_rPos,_rKey,_pValue);
		_pValue -> RemoveAll();
		delete _pValue;
	}
	m_PageToCtrlListMap.RemoveAll();
}
