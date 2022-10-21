/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1999,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// PalMap.h : interface of the CPalMap class
//
// Created by Neal White III, 5-31-1999
/////////////////////////////////////////////////////////////////////////////

#ifndef _PALMAP_H_
#define _PALMAP_H_

#ifndef __AFXTEMPL_H__
	#include <afxtempl.h>
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWallyPalette;

typedef	CMap <COLORREF, COLORREF&, DWORD, DWORD&> PalMapType;

class CPalMap : public PalMapType
{
public:
	CPalMap();
	~CPalMap();

// Attributes
public:

// Members
public:

// Operations
public:
	int LookUpColorIndex( COLORREF rgbIndex)
	{
		DWORD dwRefCountAndPalIndex;

		if (Lookup( rgbIndex, dwRefCountAndPalIndex))
		{
			ASSERT( (dwRefCountAndPalIndex & 0x0000FFFF) <= 255);

			return (dwRefCountAndPalIndex & 255);
		}
		else
		{
			return -1;
		}
	}

	int CPalMap::GetRefCount( COLORREF rgbIndex)
	{
		DWORD dwRefCountAndPalIndex;

		if (Lookup( rgbIndex, dwRefCountAndPalIndex))
		{
			return (HIWORD( dwRefCountAndPalIndex));
		}
		else
		{
			ASSERT( FALSE);
			return 0;
		}
	}

	void AddRef( COLORREF rgbIndex, int iPalIndex);
	void Release( COLORREF rgbIndex, int iPalIndex, CWallyPalette* pPal);

	const CPalMap& operator=( const CPalMap& Map)
	{
#ifdef _DEBUG
		Map.FullAssertValid();
#endif
		RemoveAll();

		COLORREF rgbIndex;
		DWORD    dwRefCountAndPalIndex;
		POSITION Pos = Map.GetStartPosition();

		// loop thru all elements

		int iTest = 0;

		while (Pos != NULL)
		{
			iTest++;
			Map.GetNextAssoc( Pos, rgbIndex, dwRefCountAndPalIndex);

			// copy it
			SetAt( rgbIndex, dwRefCountAndPalIndex);
		}

		// neal - hunting for rgb==0, index==255 bug
		//ASSERT( iTest == 256);

#ifdef _DEBUG
		FullAssertValid();
#endif
		return *this;
	}

// Overrides
protected:

// Implementation
public:

#ifdef _DEBUG
	virtual void FullAssertValid( CWallyPalette* pPal = NULL) const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
};

/////////////////////////////////////////////////////////////////////////////

#endif		// _PALMAP_H_
