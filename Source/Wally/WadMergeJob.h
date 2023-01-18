// WadMergeJob.h: interface for the CWadMergeJob class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WADMERGEJOB_H__
#define _WADMERGEJOB_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImageHelper;
#include "WADList.h"
#include "WadMergeDlg.h"

#define WAD_MERGE_NONE				0x0
#define WAD_MERGE_DUP_IGNORE		0x1
#define WAD_MERGE_DUP_RENAME		0x2
#define WAD_MERGE_DUP_OVERWRITE		0x4
#define WAD_MERGE_FIRST_WAD			0x8

class CWadMergeJob  
{
private:
	CString	m_strWildcard;
	CString m_strWadFile;
	CString m_strShortWadFile;
	DWORD	m_dwMaxHeight;
	DWORD	m_dwMaxWidth;
	DWORD	m_dwMinHeight;
	DWORD	m_dwMinWidth;
	CWADList m_WADList;

public:
	CWadMergeJob();
	virtual ~CWadMergeJob();

	virtual LPSTR GetWildcard();
	virtual void SetWildcard( LPCTSTR szWildcard );
	virtual LPSTR GetShortWadFile();
	virtual LPSTR GetWadFile();
	virtual void SetWadFile( LPCTSTR szWadFile );
	virtual DWORD GetMaxHeight();
	virtual void SetMaxHeight( DWORD dwMaxHeight );
	virtual DWORD GetMaxWidth();
	virtual void SetMaxWidth( DWORD dwMaxWidth );
	virtual DWORD GetMinHeight();
	virtual void SetMinHeight( DWORD dwMinHeight );
	virtual DWORD GetMinWidth();
	virtual void SetMinWidth( DWORD dwMinWidth );

	virtual BOOL MeetsMinMaxRestrictions( DWORD dwWidth, DWORD dwHeight );
	virtual BOOL HasMinMaxRestrictions();

	virtual BOOL LoadWAD();
	virtual BOOL MergeWAD( CWADList *pWADList, DWORD dwFlags, CWadMergeDlg::LPTHREAD_PARAM lpParam );
	virtual DWORD GetNumImages();

};

#endif // #ifndef _WADMERGEJOB_H__
