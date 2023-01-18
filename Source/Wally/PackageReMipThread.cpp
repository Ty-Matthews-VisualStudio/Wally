// PackageReMipThread.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PackageReMipThread.h"
#include "PackageView.h"
#include "WADList.h"
#include "PackageListBox.h"
//#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackageReMipThread

IMPLEMENT_DYNCREATE(CPackageReMipThread, CWinThread)

CPackageReMipThread::CPackageReMipThread()
{
}

CPackageReMipThread::~CPackageReMipThread()
{
}

BOOL CPackageReMipThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CPackageReMipThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPackageReMipThread, CWinThread)
	//{{AFX_MSG_MAP(CPackageReMipThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageReMipThread message handlers

void CPackageReMipThread::MainLoop( LPVOID pParam )
{
	CPackageView *pView = (CPackageView *)(pParam);

	if (!pView)
	{
		return;
	}
	
	//CProgressBar *pProgressBar = pView->GetReMipProgress();
	CPackageListBox *pListBox = pView->GetListBox();
	bool bProgressBar = FALSE;

	int iFlag = pView->GetReMipFlag();

	switch (iFlag)
	{
	case PACKAGE_REMIP_ALL:
		{
			CWADItem *pItem = NULL;

			int iCount = pListBox->GetCount();
			int j = 0;

			bProgressBar = pView->InitReMipProgress ("ReMip Deluxe  ", 60, iCount, TRUE, 0);
			//pProgressBar->Create ("ReMip Deluxe  ", 60, iCount, TRUE, 0);
				
			for (j = 0; j < iCount; j++)
			{
				pItem = (CWADItem *)(pListBox->GetItemData(j));
				pItem->RebuildSubMips();
				if (bProgressBar)
				{
					pView->StepReMipProgress();
				}
				Sleep (50);			// Don't hog the CPU
			}

			if (bProgressBar)
			{
				pView->ClearReMipProgress();
			}
		}
		break;

	case PACKAGE_REMIP_SELECTED:
		{
			CWADItem *pItem = NULL;

			int iSelCount = pListBox->GetSelCount();
			
			bProgressBar = pView->InitReMipProgress ("ReMip Deluxe  ", 60, iSelCount, TRUE, 0);
			//pProgressBar->Create ("ReMip Deluxe  ", 60, iSelCount, TRUE, 0);

			if (iSelCount != LB_ERR)
			{
				int *piIndexes = NULL;
				piIndexes = new int[iSelCount];
				pListBox->GetSelItems (iSelCount, piIndexes);

				int j = 0;		
				for (j = 0; j < iSelCount; j++)
				{
					pItem = (CWADItem *)(pListBox->GetItemData(piIndexes[j]));
					pItem->RebuildSubMips();

					if (bProgressBar)
					{
						pView->StepReMipProgress();
					}
					Sleep (50);			// Don't hog the CPU
				}

				if (piIndexes)
				{
					delete [] piIndexes;
					piIndexes = NULL;
				}
				if (bProgressBar)
				{
					pView->ClearReMipProgress();
				}
			}
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	pView->StopThread();	
	ExitThread(0);	
}