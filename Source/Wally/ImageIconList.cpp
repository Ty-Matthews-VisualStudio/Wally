// ImageIconList.cpp: implementation of the CImageIconItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "ImageIconList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CImageIconList Class
//////////////////////////////////////////////////////////////////////

int CImageIconList::FindMatch (int iSystemIcon)
{
	auto it = m_mapSystemToListIcon.find (iSystemIcon);

	return (it != m_mapSystemToListIcon.end()) ? it->second : -1;
}

int CImageIconList::Add( CBitmap* pbmImage, CBitmap* pbmMask, int iSystemIcon /* = -1 */)
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (pbmImage, pbmMask);
	}

	int iListIcon = CImageList::Add (pbmImage, pbmMask);
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}

int CImageIconList::Add( CBitmap* pbmImage, COLORREF crMask, int iSystemIcon /* = -1 */ )
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (pbmImage, crMask);
	}

	int iListIcon = CImageList::Add (pbmImage, crMask);
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}

int CImageIconList::Add( HICON hIcon, int iSystemIcon /* = -1 */ )
{
	if (iSystemIcon != -1)
	{
		int iMatch = FindMatch (iSystemIcon);
		
		if (iMatch != -1)
		{
			return iMatch;
		}
	}
	else
	{
		// We don't care what the system icon is, seeing how we didn't pass it in
		return CImageList::Add (hIcon);
	}

	int iListIcon = CImageList::Add (hIcon);	
	AddToList (iSystemIcon, iListIcon);

	return iListIcon;
}


void CImageIconList::AddToList (int iSystemIcon, int iListIcon)
{
	m_mapSystemToListIcon.emplace (iSystemIcon, iListIcon);
}