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
	SetNext(NULL);
	SetPrevious(NULL);
}

CWndItem::~CWndItem ()
{
	m_pWnd = NULL;
	SetNext(NULL);
	SetPrevious(NULL);
}

void CWndItem::SetNext (CWndItem *pNext)
{
	m_pNext = pNext;
}

CWndItem *CWndItem::GetNext()
{
	return m_pNext;
}

void CWndItem::SetPrevious (CWndItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CWndItem *CWndItem::GetPrevious()
{
	return m_pPrevious;
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


CWndList::CWndList ()
{
	SetAll (NULL);
}

CWndList::~CWndList ()
{
	PurgeList();
}

void CWndList::AddItem (CWnd *pWnd, int iFlag)
{
	if (IsInList (pWnd))
	{
		ASSERT (false);		// This CWnd is already in the list... only register once
		return;
	}

	CWndItem *pNewItem = new CWndItem (pWnd, iFlag);

	if (!GetFirst())
	{
		SetAll (pNewItem);
	}
	else
	{
		m_pWrite->SetNext (pNewItem);
		pNewItem->SetPrevious (m_pWrite);
		m_pWrite = pNewItem;
	}

	pNewItem = NULL;
}

void CWndList::SetFirst (CWndItem *pFirst)
{
	m_pFirst = pFirst;
}

CWndItem *CWndList::GetFirst ()
{
	return m_pFirst;
}

void CWndList::SetRead (CWndItem *pRead)
{
	m_pRead = pRead;
}

CWndItem *CWndList::GetRead ()
{
	return m_pRead;
}

void CWndList::SetWrite (CWndItem *pWrite)
{
	m_pWrite = pWrite;
}

CWndItem *CWndList::GetWrite ()
{
	return m_pWrite;
}

void CWndList::SetAll (CWndItem *pWnd)
{
	SetFirst (pWnd);
	SetRead (pWnd);
	SetWrite (pWnd);
}

void CWndList::PurgeList()
{
	SetRead (GetFirst());
	CWndItem *pTemp = NULL;

	while (m_pRead)
	{
		pTemp = m_pRead->GetNext();
		delete m_pRead;
		m_pRead = pTemp;
	}

	SetAll (NULL);
}

void CWndList::InvalidateAll(int iFlag)
{
	SetRead (GetFirst());
	
	while (m_pRead)
	{		
		m_pRead->Invalidate(iFlag);
		m_pRead = m_pRead->GetNext();
	}
}

bool CWndList::IsInList (CWnd *pWnd)
{
	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->GetWnd() == pWnd)
		{
			return TRUE;
		}
		m_pRead = m_pRead->GetNext();
	}

	return FALSE;
}
