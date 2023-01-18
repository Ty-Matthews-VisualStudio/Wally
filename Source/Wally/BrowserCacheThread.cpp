// BrowserCacheThread.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BrowserCacheThread.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserCacheThread

IMPLEMENT_DYNCREATE(CBrowserCacheThread, CWinThread)

CBrowserCacheThread::CBrowserCacheThread()
{
}

CBrowserCacheThread::~CBrowserCacheThread()
{
}

BOOL CBrowserCacheThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CBrowserCacheThread::ExitInstance()
{	
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CBrowserCacheThread, CWinThread)
	//{{AFX_MSG_MAP(CBrowserCacheThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserCacheThread message handlers

void CBrowserCacheThread::CleanupProc (CBrowserCacheList *pCacheList)
{
	CString strSourceCacheFile("");
	CString strTempCacheFile("");
	FILE *rp = NULL;
	FILE *wp = NULL;

	BYTE *pbySourceData = NULL;
	int iSourceLength = 0;
	int iPosition = 0;
	int iTotalItemSize = 0;
	int iNewNumThumbnails = 0;
	UINT j = 0;

	int iMessage = 0;

	strSourceCacheFile = pCacheList->GetCacheFileName();
	LPCACHE_HEADER lpSourceHeader = NULL;
	LPCACHE_ITEM lpCacheItem = NULL;

	// Come up with a unique filename for the temp cache file
	
	strTempCacheFile = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + pCacheList->GenerateRandomFileName (8);
	
	errno_t err = fopen_s (&wp, strTempCacheFile, "r");	
	while (err == 0)
	{
		fclose (wp);
		strTempCacheFile = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + pCacheList->GenerateRandomFileName (8);
		err = fopen_s(&wp, strTempCacheFile, "r");
	}

	err = fopen_s(&wp, strTempCacheFile, "wb");	
	if (err != 0)
	{		
		return;
	}

	err = fopen_s(&rp, strSourceCacheFile, "rb");
	if (err != 0)
	{
		return;
	}

	if (pCacheList->GetThreadMessage() != CACHE_THREAD_MESSAGE_NONE)
	{
		fclose (rp);
		fclose (wp);

		remove (strTempCacheFile);
		return;
	}

	// Build the source buffer
	iSourceLength = GetFileLength (rp);

	pbySourceData = new BYTE[iSourceLength];

	if (!pbySourceData)
	{
		fclose (rp);
		fclose (wp);

		remove (strTempCacheFile);
		return;
	}

	memset (pbySourceData, iSourceLength, 0);
	fread (pbySourceData, iSourceLength, 1, rp);
	fclose (rp);

	// Point at the header, make sure our first entry is correct
	lpSourceHeader = (LPCACHE_HEADER)pbySourceData;
	lpSourceHeader->iFirstItemOffset = lpSourceHeader->iHeaderSize;

	// Copy over the header to start
	fwrite (lpSourceHeader, 1, lpSourceHeader->iHeaderSize, wp);

	iPosition = lpSourceHeader->iFirstItemOffset;

	if (pCacheList->GetThreadMessage() != CACHE_THREAD_MESSAGE_NONE)
	{
		if (pbySourceData)
		{
			delete []pbySourceData;
			pbySourceData = NULL;
		}
		
		fclose (wp);
		remove (strTempCacheFile);
		return;
	}

	for (j = 0; j < lpSourceHeader->iNumThumbnails; j++)
	{
		lpCacheItem = (LPCACHE_ITEM)(pbySourceData + iPosition);

		iTotalItemSize = lpCacheItem->iHeaderSize;
		iTotalItemSize += lpCacheItem->iDataSize;
		iTotalItemSize += (lpCacheItem->iNumPaletteColors * 3);

		if ((lpCacheItem->iFlags & CIFLAG_DELETE) == 0)
		{
			// It's good, so copy it over
			fwrite (lpCacheItem, 1, iTotalItemSize, wp);

			iNewNumThumbnails++;
		}

		iPosition += iTotalItemSize;

		if (pCacheList->GetThreadMessage() != CACHE_THREAD_MESSAGE_NONE)
		{
			if (pbySourceData)
			{
				delete []pbySourceData;
				pbySourceData = NULL;
			}
			
			fclose (wp);
			remove (strTempCacheFile);
			return;
		}
		
		// Don't hog the CPU time
		Sleep (200);
	}

	lpSourceHeader->iNumThumbnails = iNewNumThumbnails;
	fseek (wp, 0, SEEK_SET);
	fwrite (lpSourceHeader, 1, lpSourceHeader->iHeaderSize, wp);
	fclose (wp);

	lpSourceHeader = NULL;

	if (pbySourceData)
	{
		delete []pbySourceData;
		pbySourceData = NULL;
	}

	// Delete the original cache file
	remove (strSourceCacheFile);
}

void CBrowserCacheThread::GroomCacheFiles (CBrowserCacheList *pCacheList)
{
	// Go out, find those cache files that haven't been accessed in g_iBrowserCacheFileGroom days,
	// and delete em!

	if (g_iBrowserCacheFileGroom <= 0)
	{
		// Don't delete anything
		return;
	}

	time_t osBinaryTime;
	time( &osBinaryTime );
	time_t lAccessTime;
	
	CString strSearch("");
	CFileFind fFinder;
	CStringArray saCacheFiles;
	CString strCacheFileName;
	
	// Go find all the cache files
	strSearch = TrimSlashes (g_strBrowseCacheDirectory) + "\\*.*";
	
	BOOL bWorking = fFinder.FindFile(strSearch);
	FILE *fp = NULL;
	errno_t err = 0;
	LPCACHE_HEADER lpHeader = NULL;
	BYTE byTempData[16];
	BYTE *pbyHeader = NULL;
	int iHeaderSize = 0;
		
	while (bWorking)
	{
		bWorking = fFinder.FindNextFile();
		if ( !fFinder.IsDirectory() && !fFinder.IsDots() )
		{
			strCacheFileName = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + fFinder.GetFileName();
			saCacheFiles.Add (strCacheFileName);			
		}
	}

	for (int j = 0; j < saCacheFiles.GetSize(); j++)
	{		
		strCacheFileName = saCacheFiles.GetAt(j);

		err = fopen_s( &fp, strCacheFileName, "rb");
		if (err == 0)
		{
			if (GetFileLength (fp) > 16)
			{
				fread ((BYTE *)byTempData, 1, 16, fp);
				lpHeader = (LPCACHE_HEADER)byTempData;
				
				if (!strcmp (lpHeader->szId, CACHE_SZID))
				{
					switch (lpHeader->iVersion)
					{
					case 1:
						{					
							iHeaderSize = lpHeader->iHeaderSize;														
							pbyHeader = new BYTE[iHeaderSize];
							
							lpHeader = (LPCACHE_HEADER)pbyHeader;
							fseek (fp, 0, SEEK_SET);

							if (GetFileLength (fp) >= iHeaderSize)
							{	
								fread (pbyHeader, 1, iHeaderSize, fp);

								lAccessTime = lpHeader->lAccessTime;

								if ( ((ULONG)osBinaryTime - (ULONG)lAccessTime) > 
									(ULONG)(g_iBrowserCacheFileGroom * 24 * 60 * 60))
								{
									// File is old... delete it!
									fclose (fp);
									fp = NULL;

									remove (strCacheFileName);
								}
							}

							lpHeader = NULL;
							if (pbyHeader)
							{
								delete []pbyHeader;
								pbyHeader = NULL;
							}
						}
						break;

					default:
						break;
					}		// switch (cHeader.iVersion)
				}			// if (!strcmp (cHeader.szId, CACHE_SZID))
			}				// if (GetFileLength (fp) > 16)
		}					// if (fp)

		if (fp)
		{
			fclose (fp);
			fp = NULL;
		}

		if (pCacheList->GetThreadMessage() != CACHE_THREAD_MESSAGE_NONE)
		{			
			return;
		}
		
		// Don't hog the CPU time
		Sleep (200);
	}						// for (int j = 0; j < saCacheFiles.GetCount(); j++)
}

void CBrowserCacheThread::MainLoop (LPVOID pParam)
{
	CBrowserCacheList *pCacheList = (CBrowserCacheList *)pParam;

	int iMessage = 0;
	BOOL bDone = FALSE;
	
	pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_IDLE);

	while (!bDone)
	{
		iMessage = pCacheList->GetThreadMessage();

		switch (iMessage)
		{
		case CACHE_THREAD_MESSAGE_CLEANUP:
			{
				pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_CLEANING);

				CleanupProc (pCacheList);
				
				iMessage = pCacheList->GetThreadMessage();

				// We were paused
				if (iMessage == CACHE_THREAD_MESSAGE_PAUSE)
				{
					pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				}
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_IDLE);
			}
			break;

		case CACHE_THREAD_MESSAGE_GROOM:
			{
				pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_GROOMING);
				
				GroomCacheFiles (pCacheList);
				
				iMessage = pCacheList->GetThreadMessage();

				// We were paused
				if (iMessage == CACHE_THREAD_MESSAGE_PAUSE)
				{
					pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				}
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_IDLE);
			}

		case CACHE_THREAD_MESSAGE_STOP:
			{
				pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_STOPPED);
				bDone = TRUE;
				continue;
			}
			break;

		case CACHE_THREAD_MESSAGE_PAUSE:
			{
				pCacheList->SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
				pCacheList->SetThreadStatus (CACHE_THREAD_STATUS_IDLE);
			}
			break;

		default:
			// This thread is a low priority
			Sleep (1000);
			break;
		}
	}
	ExitThread (0);
}
