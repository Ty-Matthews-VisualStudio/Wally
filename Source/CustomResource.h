// CustomResource.h: interface for the CCustomResource class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CUSTOM_RESOURCE_H_
#define _CUSTOM_RESOURCE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCustomResource  
{
public:
	CCustomResource();
	virtual ~CCustomResource();

	void UseResourceId (LPCTSTR szResource, int iResourceId);
	BYTE *GetData();
	DWORD GetDataSize();
	void FreeResources();

private:
	HINSTANCE	m_hInst;	
	HGLOBAL		m_hMem;
	LPBYTE		m_pbyData;
	HRSRC		m_hResItem;	
	DWORD		m_dwDataSize;
};

#endif // ifndef _CUSTOM_RESOURCE_H_
