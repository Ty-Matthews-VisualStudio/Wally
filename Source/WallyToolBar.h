/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyToolBar.h : interface of the CWallyToolBar class
//
// Created by Neal White III, 10-30-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _WALLYTOOLBAR_H_
#define _WALLYTOOLBAR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CWallyToolBar : public CToolBar
{
// Constructor
public:
//	CWallyToolBar();

// Implementation
public:
//	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
};

#endif		// _WALLYTOOLBAR_H_
