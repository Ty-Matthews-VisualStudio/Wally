#include "stdafx.h"
#include "Wally.h"
#include "test_delete.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void BugFix()
{
	CFileFind fFinder;
	CString strWildCard ("c:\\temp\\mg\\pcx_source\\*.pcx");

	BOOL bFound = fFinder.FindFile (strWildCard);

	while (bFound)
	{
		bFound = fFinder.FindNextFile ();

		if (!fFinder.IsDirectory() && !fFinder.IsDots())
		{
			CImageHelper ihHelper;
			CImageHelper ihWriteHelper;

			CString strDestination ("c:\\temp\\mg\\pcx_destination\\");			
			CString strFileName (fFinder.GetFilePath());
			ihHelper.LoadImage (strFileName);

			if (ihHelper.GetErrorCode() == IH_SUCCESS)
			{
				BYTE byHold;
				BYTE *pbyPalette = ihHelper.GetPalette();
				for (int j = 0; j < 256; j++)
				{
					byHold = pbyPalette[j * 3 + 2];
					pbyPalette[j * 3 + 2] = pbyPalette[j * 3 + 0];
					pbyPalette[j * 3 + 0] = pbyPalette[j * 3 + 1];					
					pbyPalette[j * 3 + 1] = byHold;

					/*
					byHold = pbyPalette[j * 3 + 1];
					pbyPalette[j * 3 + 1] = pbyPalette[j * 3 + 2];
					pbyPalette[j * 3 + 2] = byHold;					
					*/
				}

				strDestination += fFinder.GetFileName();

				ihWriteHelper.SaveImage (IH_8BIT, strDestination, ihHelper.GetBits(), pbyPalette, ihHelper.GetImageWidth(), ihHelper.GetImageHeight());
			}
		}
	}
}