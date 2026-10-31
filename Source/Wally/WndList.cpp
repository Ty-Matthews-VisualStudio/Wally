/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WndList.cpp : implementation of the CWndItem and CWndList classes
//
//  Created by Ty Matthews, 3-7-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WndList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CWndItem implementation
/////////////////////////////////////////////////////////////////////////////

CWndItem::CWndItem (CWnd *pWnd, int iFlag)
{
	m_pWnd = pWnd;
	m_iInvalidateFlag = iFlag;
}

void CWndItem::Invalidate(int iFlag)
{
	if (iFlag & m_iInvalidateFlag)
	{
		ASSERT (m_pWnd);
		
		if (::IsWindow(m_pWnd->m_hWnd))
		{
			m_pWnd->InvalidateRect (NULL, false);
		}
	}
}

CWnd *CWndItem::GetWnd()
{
	return m_pWnd;
}


/////////////////////////////////////////////////////////////////////////////
//	CWndList implementation
/////////////////////////////////////////////////////////////////////////////


void CWndList::AddItem (CWnd *pWnd, int iFlag)
{
	if (IsInList (pWnd))
	{
		ASSERT (false);		// This CWnd is already in the list... only register once
		return;
	}

	m_Items.emplace_back (pWnd, iFlag);
}

void CWndList::PurgeList()
{
	m_Items.clear();
}

void CWndList::InvalidateAll(int iFlag)
{
	for (CWndItem &item : m_Items)
	{
		item.Invalidate(iFlag);
	}
}

bool CWndList::IsInList (CWnd *pWnd)
{
	for (CWndItem &item : m_Items)
	{
		if (item.GetWnd() == pWnd)
		{
			return TRUE;
		}
	}

	return FALSE;
}
