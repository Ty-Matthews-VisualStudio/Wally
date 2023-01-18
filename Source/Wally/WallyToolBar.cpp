/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyToolBar.cpp : implementation of the CWallyToolBar class
//
// Created by Neal White III, 10-30-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallyToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSize CWallyToolBar::CalcDynamicLayout( int nLength, DWORD nMode)
{	
	if ((nMode & (LM_HORZDOCK | LM_VERTDOCK)) != 0)
	{
		if (m_nMRUWidth == 32767)
		{
			m_nMRUWidth = 46;		// two columns wide by default
		}
		else if (m_nMRUWidth < 46)
		{
			// Neal - is screen smaller than 1024x768?
			if (GetSystemMetrics( SM_CYSCREEN) < 768)
				m_nMRUWidth = 46;	// force to two columns wide
		}
		nMode |= LM_MRUWIDTH;
	}

	return CToolBar::CalcDynamicLayout( nLength, nMode);
}
