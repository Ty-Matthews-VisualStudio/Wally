// CustomResource.cpp: implementation of the CCustomResource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "CustomResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomResource::CCustomResource()
{
	m_hInst		= AfxGetResourceHandle();
	m_hMem		= NULL;
	m_pbyData	= NULL;
	m_hResItem	= NULL;
}

CCustomResource::~CCustomResource()
{	
	FreeResources();
}

BYTE *CCustomResource::GetData()
{	
	ASSERT (m_pbyData);
	return m_pbyData;
}

void CCustomResource::UseResourceId (LPCTSTR szResource, int iResourceId)
{
	ASSERT (szResource);

	FreeResources();
	m_hResItem = FindResource( m_hInst, MAKEINTRESOURCE( iResourceId), szResource);

	if (m_hResItem)
	{
		m_hMem = LoadResource( m_hInst, m_hResItem);

		if (m_hMem)
		{		
			LPBYTE pbyData = (BYTE *)LockResource( m_hMem);

			if (pbyData)
			{
				m_dwDataSize = SizeofResource( m_hInst, m_hResItem);
				m_pbyData = new BYTE[m_dwDataSize];

				if (m_pbyData)
				{
					memcpy( m_pbyData, pbyData, m_dwDataSize);
				}
			}

			pbyData = NULL;
			m_hMem = NULL;
		}

		m_hResItem = NULL;
	}
}

void CCustomResource::FreeResources()
{
	if (m_pbyData)
	{
		delete []m_pbyData;
		m_pbyData = NULL;
	}	
}

DWORD CCustomResource::GetDataSize()
{	
	return m_dwDataSize;
}