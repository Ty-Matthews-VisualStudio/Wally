/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1999,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// PalMap.cpp : implementation of the CPalMap class
//
// Created by Neal White III, 5-31-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "PalMap.h"
#include "WallyPal.h"

CPalMap::CPalMap()
{
	// Initializes the hash table. For best performance, the hash table size 
	// should be a prime number. To minimize collisions the size should be 
	// roughly 20 percent larger than the largest anticipated data set.

	InitHashTable( 401);	// nice and big to *really* reduce collisions
	m_nBlockSize = 16;

	COLORREF rgbBlack              = RGB( 0,0,0);
	DWORD    dwRefCountAndPalIndex = MAKELONG( rgbBlack, 256);	// 256 black values

	SetAt( rgbBlack, dwRefCountAndPalIndex);
}

CPalMap::~CPalMap()
{
	RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////

void CPalMap::FullAssertValid( CWallyPalette* pPal) const
{
	int      iTotalRefCount = 0;
	COLORREF rgbIndex;
	DWORD    dwRefCountAndPalIndex;
	POSITION Pos = GetStartPosition();

	// neal - debug
//	ASSERT( Pos != (POSITION )0xFFFFFFFF);

	// loop thru all elements

	while (Pos != NULL)
	{
		GetNextAssoc( Pos, rgbIndex, dwRefCountAndPalIndex);

		int iRefCount   = HIWORD( dwRefCountAndPalIndex);
		int iCheckIndex = LOWORD( dwRefCountAndPalIndex);

		// all ref counts >= 1?  all indexes valid?
#ifdef _PALMAP_ASSERT
		ASSERT( (iRefCount >= 1) && (iRefCount <= 256));
		ASSERT( iCheckIndex <= 255);

		// neal - hunting for rgb==0, index==255 bug
		ASSERT( (rgbIndex != 0) || (iCheckIndex != 255));
#endif

#ifdef _DEBUG
		if (pPal)
		{
			static BOOL bJustOnce = TRUE;

			if (pPal->GetRGB( iCheckIndex) != rgbIndex)
			{
				if (bJustOnce)
				{
					ASSERT( FALSE);		// colors don't match!
					bJustOnce = FALSE;
				}
			}
		}
#endif

		iTotalRefCount += iRefCount;
	}

	// total ref count == 256?

	if (Pos != NULL)
	{
		ASSERT( iTotalRefCount == 256);
	}
}

/////////////////////////////////////////////////////////////////////////////
#endif	// _DEBUG
/////////////////////////////////////////////////////////////////////////////

void CPalMap::AddRef( COLORREF rgbIndex, int iPalIndex)
{
	ASSERT( iPalIndex <= 255);

	DWORD dwRefCountAndPalIndex;

	if (Lookup( rgbIndex, dwRefCountAndPalIndex))
	{
		int iRefCount   = HIWORD( dwRefCountAndPalIndex);
		int iCheckIndex = LOWORD( dwRefCountAndPalIndex);

		ASSERT( (iRefCount >= 0) && (iRefCount <= 256));
		ASSERT( iCheckIndex <= 255);
		iCheckIndex &= 255;

		iRefCount++;

		dwRefCountAndPalIndex = MAKELONG( iCheckIndex, iRefCount);
		SetAt( rgbIndex, dwRefCountAndPalIndex);
	}
	else
	{
		dwRefCountAndPalIndex = MAKELONG( iPalIndex, 1);
		SetAt( rgbIndex, dwRefCountAndPalIndex);
	}
}

void CPalMap::Release( COLORREF rgbIndex, int iPalIndex, CWallyPalette* pPal)
{
/*
	DWORD dwRefCountAndPalIndex;

	if (Lookup( rgbIndex, dwRefCountAndPalIndex))
	{
		int iRefCount   = HIWORD( dwRefCountAndPalIndex);
		int iCheckIndex = LOWORD( dwRefCountAndPalIndex);

		ASSERT( (iRefCount >= 1) && (iRefCount <= 256));
		ASSERT( iCheckIndex <= 255);
		iCheckIndex &= 255;

		iRefCount--;

		if (iRefCount <= 0)
		{
			RemoveKey( rgbIndex);
		}
		else
		{
			rgbIndex &= 0x00FFFFFF;   // just in case...

			if (iCheckIndex == iPalIndex)
			{
				// we need to find a valid index, since we have
				// multiples and we're removing the one in use

				int iIndex = -1;

				for (int i = 0; i < 256; i++)
				{
					COLORREF rgb = pPal->GetRGB( i);

					ASSERT( (rgb & 0xFF000000) == 0);
					rgb &= 0x00FFFFFF;   // just in case...

					if ((rgbIndex == rgb) && (iCheckIndex != i))
					{
						iIndex = i;
						break;
					}
				}
				ASSERT( iIndex != -1);

				// this is the new palette index to use for this color

				iPalIndex = iIndex;

				ASSERT( iCheckIndex != iPalIndex);
			}
			// neal - hunting for rgb==0, index==255 bug
			//dwRefCountAndPalIndex = MAKELONG( iPalIndex, iRefCount);
			dwRefCountAndPalIndex = MAKELONG( iCheckIndex, iRefCount);

			SetAt( rgbIndex, dwRefCountAndPalIndex);
		}
	}
	else
	{
		ASSERT( FALSE);
	}
*/

	POSITION Pos = GetStartPosition();

	// loop thru all elements

	while (Pos != NULL)
	{
		COLORREF rgbIndex;
		DWORD    dwRefCountAndPalIndex;

		GetNextAssoc( Pos, rgbIndex, dwRefCountAndPalIndex);

		//int iRefCount   = HIWORD( dwRefCountAndPalIndex);
		int iCheckIndex = LOWORD( dwRefCountAndPalIndex);

		// remove all references to removed color index
		if (iPalIndex == iCheckIndex)
		{
			RemoveKey( rgbIndex);
		}
	}
}
