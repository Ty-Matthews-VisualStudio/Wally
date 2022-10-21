// Genesis3D.cpp
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "wally.h"
#include "PackageDoc.h"
#include "ColorOpt.h"
#include "WallyPal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// The Genesis3D SDK requires compiling with VC6
#if _MSC_VER >= 1200
#ifdef GENESIS_3D

#include "Genesis3D.h"

#ifdef _DEBUG
	#pragma comment (lib, "./GenesisSDK/lib/genesisd.lib")
#else
	#pragma comment (lib, "./GenesisSDK/lib/genesis.lib")
#endif

extern CWallyApp theApp;

BOOL	AddTexture(geVFile *BaseFile, const char *Path, CPackageDoc *pDoc)
{
//	geBitmap_Info	PInfo;
//	geBitmap_Info	SInfo;
	geBitmap		*Bitmap;
	geVFile			*File;
	char			FileName[_MAX_FNAME];
	char			*Name;

	geBitmap *Lock;
	gePixelFormat Format;
	geBitmap_Info info;
	HBITMAP hbm = NULL;
	CString strError("");

	Bitmap = NULL;
	File = NULL;

	_splitpath(Path, NULL, NULL, FileName, NULL);
	Name = strdup(FileName);
	if	(!Name)
	{
		strError = "Memory allocation error processing ";
		strError += Path;
		AfxMessageBox (strError);
		return FALSE;
	}

	if	(BaseFile)
		File = geVFile_Open(BaseFile, Path, GE_VFILE_OPEN_READONLY);
	else
		File = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_DOS, Path, NULL, GE_VFILE_OPEN_READONLY);

	if	(!File)
	{
		strError = "Could not open ";
		strError +=  Path;
		AfxMessageBox (strError);
		goto fail;
	}

	Bitmap = geBitmap_CreateFromFile(File);
	geVFile_Close(File);
	if	(!Bitmap)
	{
		strError = Path;
		strError += "is not a valid bitmap";
		AfxMessageBox (strError);
		goto fail;
	}

	// <> choose format to be 8,16,or 24, whichever is closest to Bitmap
	Format = GE_PIXELFORMAT_8BIT_PAL;

	if ( ! geBitmap_LockForRead(Bitmap, &Lock, 0, 0, Format, GE_FALSE,0) )
	{
		return NULL;
	}
	
	geBitmap_GetInfo(Lock, &info, NULL);

	if ( info.Format != Format )
		return NULL;

	{
		void  *bits;
		BYTE  *newbits;
		int pelbytes;
		BOOL bAlloced = FALSE;

		pelbytes = gePixelFormat_BytesPerPel(Format);
		bits = geBitmap_GetBits(Lock);
		
		int iHeight = info.Height;
		int iWidth = 0;
		
		if ( (info.Stride*pelbytes) == (((info.Stride*pelbytes)+3)&(~3)) )
		{
			iWidth = info.Stride;
			newbits = (BYTE *)bits;
		}
		else
		{
			int Stride;

			iWidth = info.Width;
			
			Stride = (((info.Width*pelbytes)+3)&(~3));
			newbits = (BYTE *)geRam_Allocate(Stride * info.Height);
			if ( newbits )
			{
				bAlloced = TRUE;
				char *newptr,*oldptr;
				int y;

				newptr = (char *)newbits;
				oldptr = (char *)bits;
				for(y=0; y<info.Height; y++)
				{
					memcpy(newptr,oldptr,(info.Width)*pelbytes);
					oldptr += info.Stride*pelbytes;
					newptr += Stride;
				}				
			}
		}

		int iSize = iWidth * iHeight;
		geBitmap_Palette *gePal = geBitmap_GetPalette(Bitmap);
		BYTE *bybmpPalette;
		BYTE byPalette[768];
		int iPalSize = 0;

		if (!gePal)
		{			
			strError = "Could not retrieve palette from ";
			strError += Path;
			AfxMessageBox (strError);
			goto fail;
		}

		geBitmap_Palette_Lock(gePal, (void **)(&bybmpPalette), NULL, &iPalSize);
				
		BYTE *byBits[4];
		int j = 0;

		for (j = 0; j < 4; j++)
		{
			byBits[j] = new BYTE[iSize];
			memcpy (byBits[j], newbits, iSize);
		}

//		BYTE byCopy;
		memset (byPalette, 0, 768);

		for (j = 0; j < iPalSize; j++)
		{
		/*	byCopy = bybmpPalette[j * 3 + 0];
			bybmpPalette[j * 3 + 0] = bybmpPalette[j * 3 + 2];
			bybmpPalette[j * 3 + 2] = byCopy;
			*/

			memcpy (byPalette + (j * 3), bybmpPalette + (j * 4), 3);
		}	
		
		CWallyPalette Palette;
		Palette.SetPalette (byPalette, 256);

		pDoc->AddImage (byBits, &Palette, Name, iWidth, iHeight, TRUE);

		geBitmap_Palette_UnLock(gePal);

		if (bAlloced)
		{
			geRam_Free(newbits);
		}

		for (j = 0; j < 4; j++)
		{
			delete []byBits[j];
			byBits[j] = NULL;
		}
	}

	if ( Lock != Bitmap )
	{
		geBitmap_UnLock (Lock);
	}

	/*
	geBitmap_GetInfo(Bitmap, &PInfo, &SInfo);
	if	(PInfo.Format != GE_PIXELFORMAT_8BIT)
	{
		NonFatalError("%s is not an 8bit bitmap", Path);
		goto fail;
	}	
	NewBitmapList = geRam_Realloc(pData->Bitmaps, sizeof(*NewBitmapList) * (pData->BitmapCount + 1));
	if	(!NewBitmapList)
	{
		NonFatalError("Memory allocation error processing %s", Path);
		goto fail;
	}

	NewBitmapList[pData->BitmapCount].Name		= Name;
	NewBitmapList[pData->BitmapCount].Bitmap	= Bitmap;
	NewBitmapList[pData->BitmapCount].WinBitmap	= NULL;
	NewBitmapList[pData->BitmapCount].Flags		= 0;
	pData->BitmapCount++;
	pData->Bitmaps = NewBitmapList;

	SendDlgItemMessage(pData->hwnd, IDC_TEXTURELIST, LB_ADDSTRING, (WPARAM)0, (LPARAM)Name);	
	*/
	
	if	(Name)
		free(Name);
	if	(Bitmap)
		geBitmap_Destroy(&Bitmap);
	return TRUE;

fail:
	if	(Name)
		free(Name);
	if	(Bitmap)
		geBitmap_Destroy(&Bitmap);
	return FALSE;
}

void LoadTXLFile ()
{
	geVFile *VFS;
	geVFile_Finder *Finder;

	CString strWildCard ("Genesis3D texture packages (*.txl)|*.txl|");
	
	CFileDialog dlgTXL (true, ".txl", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, NULL);

	dlgTXL.m_ofn.lpstrTitle = "Open TXL package";	
	
	CString strFileName("");
	CString strError("");
	
	if (dlgTXL.DoModal() == IDOK)
	{		
		strFileName = dlgTXL.GetPathName();	

		CPackageDoc *pDoc = theApp.CreatePackageDoc();
		pDoc->SetWADType (WAD3_TYPE);
		
		VFS = geVFile_OpenNewSystem(NULL, GE_VFILE_TYPE_VIRTUAL, strFileName, NULL, GE_VFILE_OPEN_READONLY | GE_VFILE_OPEN_DIRECTORY);
		if	(!VFS)
		{
			strError = "Could not open file " + strFileName;
			AfxMessageBox (strError, MB_ICONSTOP);
			return;
		}
		
		Finder = geVFile_CreateFinder(VFS, "*.*");
		if	(!Finder)
		{
			strError = "Could not load textures from " + strFileName;
			AfxMessageBox (strError, MB_ICONSTOP);
			geVFile_Close(VFS);
			return;
		}
		
		while	(geVFile_FinderGetNextFile(Finder) != GE_FALSE)
		{
			geVFile_Properties	Properties;
			geVFile_FinderGetProperties(Finder, &Properties);
				
			if	(!AddTexture(VFS, Properties.Name, pDoc))
			{
				geVFile_Close(VFS);
				return;
			}
			
		}		

		geVFile_DestroyFinder(Finder);
		geVFile_Close(VFS);
	}
}

#endif		// #ifdef GENESIS_3D
#endif		// if _MSC_VER >= 1200