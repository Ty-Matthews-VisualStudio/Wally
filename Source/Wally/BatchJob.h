// BatchJob.h: interface for the CBatchJob class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BATCHJOB_H__
#define _BATCHJOB_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThreadJob.h"

#define BLUE_INDEX					-1
#define DESTINATION_TEXTURES		0
#define DESTINATION_PACKAGE			1

class CBatchJob : public CThreadJob  
{
private:
	CString m_strSourceFile;
	CString m_strDestinationDirectory;
	CString m_strDestinationFile;
	CString m_strOffsetDirectory;
	
	CPackageDoc *m_pPackageDoc;
	int m_iImageType;
	int m_iDestinationType;

public:
	CBatchJob();
	CBatchJob( LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory, int iDestinationType, int iImageType, CPackageDoc *pDoc );
	virtual ~CBatchJob();

	virtual void Initialize();
	virtual void Process();
	virtual LPCTSTR GetSourceFile();
	virtual LPCTSTR GetDestinationFile();
	virtual LPCTSTR GetMessage();
};

#endif // #ifndef _BATCHJOB_H__
