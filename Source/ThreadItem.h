// ThreadItem.h: interface for the CThreadItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _THREADITEM_H_
#define _THREADITEM_H_

#ifndef _WALLYPAL_H_
	#include "WallyPal.h"
#endif

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Status Messages
#define THREADJOB_MESSAGES			0
#define THREADJOB_NOMESSAGE			(THREADJOB_MESSAGES + 1)
#define THREADJOB_STOP				(THREADJOB_MESSAGES + 2)
#define THREADJOB_START				(THREADJOB_MESSAGES + 3)
#define THREADJOB_MESSAGES_END		(THREADJOB_MESSAGES + 4)

// Status Codes
#define THREADJOB_STATUS_CODES		1000
#define THREADJOB_WAITING			(THREADJOB_STATUS_CODES + 1)
#define THREADJOB_STARTED			(THREADJOB_STATUS_CODES + 2)

// Job finished codes
#define THREADJOB_DONE					(THREADJOB_STATUS_CODES + 3)
#define THREADJOB_BAD_SOURCE_FILE		(THREADJOB_DONE + 1)
#define THREADJOB_BAD_DEST_FILE			(THREADJOB_DONE + 2)
#define THREADJOB_IH_ERROR				(THREADJOB_DONE + 3)
#define THREADJOB_NOT_DIV_16			(THREADJOB_DONE + 4)
#define THREADJOB_OUT_OF_MEMORY			(THREADJOB_DONE + 5)
#define THREADJOB_UNSUPPORTED_BIT_DEPTH	(THREADJOB_DONE + 6)
#define THREADJOB_STOPPED				(THREADJOB_DONE + 7)
#define THREADJOB_SUCCESS				(THREADJOB_DONE + 8)
#define THREADJOB_END_THREAD			(THREADJOB_DONE + 9)

class CThreadJob;
class CPackageDoc;
class CThreadItem  
{
// Members
public:
	CString m_strSourceFile;
	CString m_strDestinationDirectory;
	CString m_strDestinationFile;
	CString m_strOffsetDirectory;
	CString m_strErrorMessage;	

	CThreadJob *m_pThreadJob;	
	CThreadItem *m_pNext;
	CThreadItem *m_pPrevious;

private:	
	bool CheckForStopMessage();
	int m_iMessage;
	int m_iJobStatus;
	int m_iDestinationType;
	int m_iImageType;

	CPackageDoc *m_pPackage;
	LPQ2_MIP_S m_WalHeader;
	CString m_strPackageName;
	unsigned char *m_pbyData;
	unsigned char m_byPalette[768];
	CWallyPalette m_Palette;
	CImageHelper m_ihHelper;

public:
	CThreadItem(LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory, int iDestinationType, int iImageType, CPackageDoc *pDoc = NULL);
	virtual ~CThreadItem();

	void StartJob();
	void StopJob();
	void SetJobStatus (int iStatus);
	int GetJobStatus ();
	void SetMessage (int iMessage);
	int GetMessage ();
	void ClearMessage();

	void SetNext(CThreadItem *pItem);
	CThreadItem *GetNext();
	void SetPrevious (CThreadItem *pPrevious);
	CThreadItem *GetPrevious();

	void ConvertImage();
	int GetDestinationType ()
	{
		return m_iDestinationType;
	}

	unsigned char *GetBits (int iMipNumber);
	
	CWallyPalette* GetPalette( void)
	{
		return &m_Palette;
	}

	int GetWidth( void)
	{	
		ASSERT (m_WalHeader);
		return m_WalHeader->width;
	}

	int GetHeight( void)
	{	
		ASSERT (m_WalHeader);
		return m_WalHeader->height;
	}

	CString GetName( void)
	{
		return m_strPackageName;
	}

	void SetImageType (int iType)
	{
		m_iImageType = iType;
	}
	int GetImageType ()
	{
		return m_iImageType;
	}
	
};

#endif // _THREADITEM_H_
