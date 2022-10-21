// SortStringArray.cpp: implementation of the CSortStringArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SortStringArray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CSortStringArray::Sort()
{ 
	BOOL bNotDone = TRUE;

	while (bNotDone)
	{
		bNotDone = FALSE;
		for (int iPos = 0; iPos < GetUpperBound(); iPos++)
		{
			bNotDone |= CompareAndSwap (iPos);
		}
	}
}

BOOL CSortStringArray::CompareAndSwap(int iPos)
{ 
	CString strTemp;
	int posFirst = iPos;
	int posNext = iPos + 1;

	if (GetAt(posFirst).CompareNoCase(GetAt(posNext)) > 0)
	{
		strTemp = GetAt (posFirst);
		SetAt(posFirst, GetAt(posNext));
		SetAt(posNext, strTemp);
		return TRUE;
	}
	return FALSE;
}  

int CSortStringArray::HasString (LPCTSTR szString)
{
	int iPosition = 0;

	while (iPosition < GetSize())
	{
		if (!GetAt(iPosition).CompareNoCase(szString))
		{
			return iPosition;
		}

		iPosition++;
	}

	return -1;
}