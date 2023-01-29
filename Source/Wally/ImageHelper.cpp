/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  ImageHelper.cpp : implementation of the CImageHelper class
//
//  Created by Ty Matthews, 8-17-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "Wally.h"
#include "TextureFlags.h"
#include "WallyPal.h"
#include "ColorOpt.h"
#include "WADList.h"
#include "RegistryHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

////////////////////////////////////////////////////////////////////////////////////////
//  g_strTypeDescriptions is used by CWildCardList and CWildCardItem to fill in
//  a linked-list of currently supported file types.  The following string array defines
//  many things, from descriptions as they appear in Wally, to file extensions, to
//  registry keys, et al.  The order in which they appear *directly* correlates to
//  the #define statements located in ImageHelper.h.  If the order or the value
//  assigned to any IH_XXX_TYPE changes, you must adjust this string table accordingly.
//
//	Below is a description of each column:
//
//	Column 1:	This is the description as used in combo boxes; it is not intended
//				for use in CFileDialog wildcard lists.  Every file type must include
//				a description.
//
//  Column 2:	This is the wildcard as used in a CFileDialog.  You can append
//				mutliple wildcards together, and add a '|' to the end to close the
//				wildcard list.  Every file type must include a wildcard member.
//
//	Column 3:	This is the pure extension of the image type, with no leading period.  
//				This column is required on all file types.
//
//	Column 4:	This is the registry key name for the source palette file to be 
//				used when loading this image type.  Column 6 defines whether or 
//				not to actually use this file.  Only needed for image types (game.)
//
//	Column 5:	This is the registry key name for the destination palette file to be
//				used when saving this image type.  Column 7 defines whether or not
//				to actually use this file.  Only needed for image types.
//
//	Column 6:	This is the registry key name for whether or not to use the source
//				palette when loading.  Only needed for image types (game.)
//
//	Column 7:	This is the registry key name for whether or not to use the destination
//				palette when saving.  Only needed for image types.


CString g_strTypeDescriptions[WILDCARD_LIST_LENGTH] =
{
	"HereticII Texture (*.m8)",			"HereticII Texture (*.m8)|*.m8|",					"m8",	"",						"strDestPalette_SWL",	"",								"UseCurrentDestPalette_SWL",	
	"JPEG - JFIF (*.jpg)",				"JPEG - JFIF (*.jpg)|*.jpg|",						"jpg",	"",						"",						"",								"",
	"Portable Network Graphics (*.png)","Portable Network Graphics (*.png)|*.png|",			"png",	"",						"strDestPalette_PNG",	"",								"UseCurrentDestPalette_PNG",
	"Quake2 Texture (*.wal)",			"Quake2 Texture (*.wal)|*.wal|",					"wal",	"strSourcePalette_WAL",	"strDestPalette_WAL",	"UseCurrentSourcePalette_WAL",	"UseCurrentDestPalette_WAL",	
	"Quake1 Texture (*.mip)",			"Quake1 Texture (*.mip)|*.mip|",					"mip",	"strSourcePalette_MIP",	"strDestPalette_MIP",	"UseCurrentSourcePalette_MIP",	"UseCurrentDestPalette_MIP",	
	"Serious Sam Texture (*.tex)",		"Serious Sam Texture (*.tex)|*.tex|",				"tex",	"",						"",						"",								"",
	"SiN Texture (*.swl)",				"SiN Texture (*.swl)|*.swl|",						"swl",	"",						"strDestPalette_SWL",	"",								"UseCurrentDestPalette_SWL",
	"Truevision Targa (*.tga)",			"Truevision Targa (*.tga)|*.tga|",					"tga",	"",						"strDestPalette_TGA",	"",								"UseCurrentDestPalette_TGA",
	"Windows Bitmap (*.bmp)",			"Windows Bitmap (*.bmp)|*.bmp|",					"bmp",	"",						"strDestPalette_BMP",	"",								"UseCurrentDestPalette_BMP",	
	"ZSoft PC Paintbrush (*.pcx)",		"ZSoft PC Paintbrush (*.pcx)|*.pcx|",				"pcx",	"",						"strDestPalette_PCX",	"",								"UseCurrentDestPalette_PCX",
	
	
#ifdef _TEST
	"Wally (*.wly)",					"Wally (*.wly)|*.wly|",								"wly",	"",						"strDestPalette_WLY",	"",								"UseCurrentDestPalette_WLY",
	"CompuServe GIF (*.gif)",			"CompuServe Graphics Interchange (*.gif)|*.gif|",	"gif",	"",						"strDestPalette_GIF",	"",								"UseCurrentDestPalette_GIF",
#endif		// #ifdef _TEST

	"Quake1 Package (*.wad)",			"Quake1 Package (*.wad)|*.wad|",		"wad",	"","","","",
	"Half-Life Package (*.wad)",		"Half-Life Package (*.wad)|*.wad|",		"wad",	"","","","",
	"JASC Palette (*.pal)",				"JASC Palette (*.pal)|*.pal|",			"pal",	"","","","",
	"Lump Palette (*.lmp)",				"Lump Palette (*.lmp)|*.lmp|",			"lmp",	"","","","",
	"PhotoShop Color Table (*.act)",	"PhotoShop Color Table (*.act)|*.act|",	"act",	"","","","",
	"Pack File (*.pak)",				"Pack File (*.pak)|*.pak|",				"pak",	"","","",""
};

CWildCardList	g_WildCardList;

////////////////////////////////////////////////////////////////////////////////
//   CWildCardItem
////////////////////////////////////////////////////////////////////////////////


CWildCardItem::CWildCardItem (LPCTSTR szDescription, LPCTSTR szWildCard, LPCTSTR szWildCardExtension, LPCTSTR szSourceRegKey, LPCTSTR szDestRegKey, LPCTSTR szUseCurrentSourceRegKey, LPCTSTR szUseCurrentDestRegKey, int iImageType)
{
	ASSERT (szDescription);
	ASSERT (szWildCard);

	m_strDescription = szDescription;
	m_strWildCard = szWildCard;
	m_strWildCardExtension = szWildCardExtension;
	m_strSourceRegKey = szSourceRegKey;
	m_strDestRegKey = szDestRegKey;
	m_strUseCurrentSourceRegKey = szUseCurrentSourceRegKey;
	m_strUseCurrentDestRegKey = szUseCurrentDestRegKey;

	m_iImageType = iImageType;
	
	SetNext (NULL);
	SetPrevious (NULL);	
	m_bInChangeMode = FALSE;

	FindAssociatedProgram();
}

CWildCardItem::~CWildCardItem ()
{
	SetNext (NULL);
	SetPrevious (NULL);
}

CWildCardItem *CWildCardItem::GetNext()
{
	return m_pNext;
}


void CWildCardItem::SetNext (CWildCardItem *pNext)
{
	m_pNext = pNext;
}

CWildCardItem *CWildCardItem::GetPrevious()
{
	return m_pPrevious;
}

void CWildCardItem::SetPrevious (CWildCardItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CString CWildCardItem::GetDescription()
{
	return m_strDescription;
}

CString CWildCardItem::GetWildCard()
{
	return m_strWildCard;
}

CString CWildCardItem::GetWildCardExtension()
{
	return m_strWildCardExtension;
}

int CWildCardItem::GetImageType()
{	
	return m_iImageType;
}

CString CWildCardItem::GetSourcePaletteFile()
{
	CString strReturn ("");

	if (m_bInChangeMode)
	{
		strReturn = m_strTempSourcePaletteFile;
	}
	else
	{
		strReturn = m_strSourcePaletteFile;
	}

	return strReturn;
}

void CWildCardItem::SetSourcePaletteFile(LPCTSTR szDirectory)
{
	if (m_bInChangeMode)
	{
		m_strTempSourcePaletteFile = szDirectory;
	}
	else
	{
		m_strSourcePaletteFile = szDirectory;
	}
}

CString CWildCardItem::GetDestPaletteFile()
{
	CString strReturn ("");

	if (m_bInChangeMode)
	{
		strReturn = m_strTempDestPaletteFile;
	}
	else
	{
		strReturn = m_strDestPaletteFile;
	}

	return strReturn;
}

void CWildCardItem::SetDestPaletteFile(LPCTSTR szDirectory)
{
	if (m_bInChangeMode)
	{
		m_strTempDestPaletteFile = szDirectory;
	}
	else
	{
		m_strDestPaletteFile = szDirectory;	
	}
}

CString CWildCardItem::GetSourceRegKey()
{
	return m_strSourceRegKey;
}

CString CWildCardItem::GetDestRegKey()
{
	return m_strDestRegKey;
}

BOOL CWildCardItem::SupportsSourcePalette()
{
	return (GetSourceRegKey() != "");
}

BOOL CWildCardItem::UseCurrentSourcePalette()
{
	BOOL bValue = FALSE;

	if (m_bInChangeMode)
	{
		bValue = m_bTempUseCurrentSourcePalette;
	}
	else
	{
		bValue = m_bUseCurrentSourcePalette;
	}

	return bValue;
}

BOOL CWildCardItem::UseCurrentDestPalette()
{
	BOOL bValue = FALSE;

	if (m_bInChangeMode)
	{
		bValue = m_bTempUseCurrentDestPalette;
	}
	else
	{
		bValue = m_bUseCurrentDestPalette;
	}

	return bValue;
}

CString CWildCardItem::GetUseCurrentSourceRegKey()
{
	return m_strUseCurrentSourceRegKey;
}

CString CWildCardItem::GetUseCurrentDestRegKey()
{
	return m_strUseCurrentDestRegKey;
}

BOOL CWildCardItem::GetSourcePalette(BYTE *pbyPalette)
{
	ASSERT (pbyPalette);		// Better have room for this!

	if (!UseCurrentSourcePalette())
	{
		CImageHelper ihHelper;

		ihHelper.LoadImage (GetSourcePaletteFile(), IH_LOAD_ONLYPALETTE);
		if (ihHelper.GetErrorCode() == IH_SUCCESS)
		{
			memcpy (pbyPalette, ihHelper.GetBits(), 768);
			return TRUE;
		}
		return FALSE;		
	}


	switch (GetImageType())
	{
	case IH_WAL_TYPE:
		memcpy (pbyPalette, quake2_pal, 768);
		return TRUE;
		break;

	case IH_MIP_TYPE:
		memcpy (pbyPalette, quake1_pal, 768);
		return TRUE;
		break;

	default:
		ASSERT (FALSE);		// Only TRUE game types have defined palettes!
		memset (pbyPalette, 0, 768);
		break;
	}

	return FALSE;
	
}

BOOL CWildCardItem::GetDestPalette(BYTE *pbyPalette)
{
	ASSERT (pbyPalette);		// Better have room for this!

	if (!UseCurrentDestPalette())
	{
		CImageHelper ihHelper;

		ihHelper.LoadImage (GetDestPaletteFile(), IH_LOAD_ONLYPALETTE);
		if (ihHelper.GetErrorCode() == IH_SUCCESS)
		{
			memcpy (pbyPalette, ihHelper.GetBits(), 768);
			return TRUE;
		}
		return FALSE;		
	}


	switch (GetImageType())
	{
	case IH_WAL_TYPE:
		memcpy (pbyPalette, quake2_pal, 768);
		return TRUE;
		break;

	case IH_MIP_TYPE:
		memcpy (pbyPalette, quake1_pal, 768);
		return TRUE;
		break;

	default:
		ASSERT (FALSE);		// Only TRUE game types have defined palettes!
		memset (pbyPalette, 0, 768);
		break;
	}

	return FALSE;
}

void CWildCardItem::SolidifyChanges()
{
	m_strSourcePaletteFile = m_strTempSourcePaletteFile;
	m_strDestPaletteFile = m_strTempDestPaletteFile;
	m_bUseCurrentSourcePalette = m_bTempUseCurrentSourcePalette;
	m_bUseCurrentDestPalette = m_bTempUseCurrentDestPalette;
	m_bInChangeMode = FALSE;
}

void CWildCardItem::PrepareForChanges()
{
	m_strTempSourcePaletteFile = m_strSourcePaletteFile;
	m_strTempDestPaletteFile = m_strDestPaletteFile;
	m_bTempUseCurrentSourcePalette = m_bUseCurrentSourcePalette;
	m_bTempUseCurrentDestPalette = m_bUseCurrentDestPalette;
	m_bInChangeMode = TRUE;
}

void CWildCardItem::CancelChanges()
{
	m_bInChangeMode = FALSE;
}

void CWildCardItem::FindAssociatedProgram()
{
	CRegistryHelper rhHelper;
	rhHelper.SetMainKey (HKEY_CLASSES_ROOT);

	CString strExtension("");
	strExtension.Format (".%s", GetWildCardExtension());

	rhHelper.AddItem( &m_strAssociatedAppRegKey, "", "", strExtension);
	rhHelper.ReadRegistry();

	if (m_strAssociatedAppRegKey != "")
	{
		rhHelper.PurgeList();
		rhHelper.AddItem( &m_strAssociatedProgram, "", "", m_strAssociatedAppRegKey);
		rhHelper.ReadRegistry();
	}
	else
	{
		m_strAssociatedProgram = "";
	}
}

CString CWildCardItem::GetAssociatedProgram()
{
	return m_strAssociatedProgram;
}

void CWildCardItem::AssociateWithWally()
{
	// First off, see if something is already being associated.  If so, store it away so 
	// we can later put it back the way it was.  The first app may not be there, but oh well...
	// we're putting things back the way they were, and that's all you can expect to do.

	CRegistryHelper rhHelper;
	CString strExtension("");
	strExtension.Format (".%s", GetWildCardExtension());

	if (m_strAssociatedAppRegKey != "")
	{		
		rhHelper.SetMainKey( HKEY_CURRENT_USER);
		rhHelper.SetBaseSubKey( "Software\\Team BDP\\Wally\\Old File Assocations");

		rhHelper.AddItem( &m_strAssociatedAppRegKey, "", strExtension, strExtension);
		rhHelper.WriteRegistry();
		rhHelper.PurgeList();
		rhHelper.SetBaseSubKey("");
	}	
	
	rhHelper.SetMainKey (HKEY_CLASSES_ROOT);
	m_strAssociatedAppRegKey = "Wally.Document";

	rhHelper.AddItem( &m_strAssociatedAppRegKey, "", "", strExtension);
	rhHelper.WriteRegistry();

	m_strAssociatedProgram = "Wally Document";	
}

void CWildCardItem::RemoveIfAssociatedWithWally()
{
	CRegistryHelper rhHelper;
	rhHelper.SetMainKey( HKEY_CLASSES_ROOT);

	CString strRegApp("");
	CString strExtension("");
	strExtension.Format (".%s", GetWildCardExtension());

	rhHelper.AddItem (&strRegApp, "", "", strExtension);
	rhHelper.ReadRegistry();

	if (strRegApp == "Wally.Document")
	{
		// Let's go see if we saved it off at some point
		rhHelper.SetMainKey( HKEY_CURRENT_USER);
		rhHelper.SetBaseSubKey( "Software\\Team BDP\\Wally\\Old File Assocations");
		rhHelper.PurgeList();

		CString strAssociation("");

		// Go get it
		rhHelper.AddItem( &strAssociation, "", strExtension, strExtension);
		rhHelper.ReadRegistry();
	
		// If it's not there, we're "" anyways, so we're okay.  Otherwise, it's in the string
		rhHelper.SetMainKey( HKEY_CLASSES_ROOT);
		rhHelper.SetBaseSubKey("");
		rhHelper.PurgeList();
		rhHelper.AddItem( &strAssociation, "", "", strExtension);
		rhHelper.WriteRegistry();
		
		FindAssociatedProgram();
	}
}


////////////////////////////////////////////////////////////////////////////////
//   CWildCardList
////////////////////////////////////////////////////////////////////////////////

CWildCardList::CWildCardList()
{
	SetAll (NULL);
	int iIndex = 0;
	int iImageType = 0;

	for (; iIndex < WILDCARD_LIST_LENGTH; iIndex += WILDCARD_LIST_ITEM_COUNT, iImageType++)
	{
		AddItem (g_strTypeDescriptions[iIndex], 
			g_strTypeDescriptions[iIndex + 1], 
			g_strTypeDescriptions[iIndex + 2], 
			g_strTypeDescriptions[iIndex + 3],
			g_strTypeDescriptions[iIndex + 4],
			g_strTypeDescriptions[iIndex + 5],
			g_strTypeDescriptions[iIndex + 6],
			iImageType);
	}
}

CWildCardList::~CWildCardList()
{
	PurgeList();
}

void CWildCardList::SetAll(CWildCardItem *pItem)
{
	SetFirst (pItem);
	SetWrite (pItem);
	SetRead (pItem);
}

void CWildCardList::PurgeList()
{
	CWildCardItem *pItem = NULL;
	m_pRead = GetFirst();

	while (m_pRead)
	{
		pItem = m_pRead->GetNext();
		delete m_pRead;
		m_pRead = pItem;
	}
	SetAll (NULL);
}

void CWildCardList::AddItem (LPCTSTR szDescription, LPCTSTR szWildCard, LPCTSTR szWildCardExtension, LPCTSTR szSourceRegKey, LPCTSTR szDestRegKey, LPCTSTR szUseCurrentSourceRegKey, LPCTSTR szUseCurrentDestRegKey, int iImageType)
{
	CWildCardItem *pNewItem = new CWildCardItem (szDescription, szWildCard, szWildCardExtension, szSourceRegKey, szDestRegKey, szUseCurrentSourceRegKey, szUseCurrentDestRegKey, iImageType);
	if (!pNewItem)
	{
		ASSERT (FALSE);
		return;
	}

	if (!GetFirst())
	{
		SetAll (pNewItem);
	}
	else
	{
		m_pWrite->SetNext (pNewItem);
		pNewItem->SetPrevious (m_pWrite);
		m_pWrite = pNewItem;	
	}

	pNewItem = NULL;
}

int CWildCardList::GetCount()
{
	int iCount = 0;
	m_pRead = GetFirst();

	while (m_pRead)
	{
		iCount++;
		m_pRead = m_pRead->GetNext();
	}
	return iCount;
}

CWildCardItem *CWildCardList::GetAt(int iPosition)
{
	int j = 0;

	m_pRead = GetFirst();

	for (j = 0; j < iPosition; j++)
	{
		m_pRead = m_pRead->GetNext();
	}
	
	return m_pRead;
}

CWildCardItem *CWildCardList::GetFirstSupportedImage()
{
	m_pRead = GetFirst();
	return m_pRead;
}

CWildCardItem *CWildCardList::GetNextSupportedImage()
{	
	if (m_pRead)
	{	
		m_pRead = m_pRead->GetNext();
	}
	return m_pRead;
}


	
CWildCardItem *CWildCardList::GetFirst()
{
	return m_pFirst;
}

void CWildCardList::SetFirst(CWildCardItem *pFirst)
{
	m_pFirst = pFirst;
}

CWildCardItem *CWildCardList::GetWrite()
{
	return m_pWrite;
}

void CWildCardList::SetWrite(CWildCardItem *pWrite)
{
	m_pWrite = pWrite;
}

CWildCardItem *CWildCardList::GetRead()
{
	return m_pRead;
}

void CWildCardList::SetRead(CWildCardItem *pRead)
{
	m_pRead = m_pRead;
}

void CWildCardList::ReadRegistry()
{
	CString strBatchRegistryKey ("BatchSettings");
	CWildCardItem *pItem = GetFirstSupportedImage();
	int iCheck = 0;
	
	while (pItem)
	{
		if (pItem->GetSourceRegKey() != "")
		{
			pItem->SetSourcePaletteFile (	theApp.GetProfileString	(strBatchRegistryKey,    pItem->GetSourceRegKey(),		""));
		}

		if (pItem->GetDestRegKey() != "")
		{
			pItem->SetDestPaletteFile	(	theApp.GetProfileString	(strBatchRegistryKey,    pItem->GetDestRegKey(),		""));
		}

		if (pItem->GetUseCurrentSourceRegKey() != "")
		{
			iCheck = ( theApp.GetProfileInt	(strBatchRegistryKey,    pItem->GetUseCurrentSourceRegKey(),		1));
			pItem->m_bUseCurrentSourcePalette = iCheck ? TRUE : FALSE;			
		}

		if (pItem->GetUseCurrentDestRegKey() != "")
		{
			iCheck = ( theApp.GetProfileInt	(strBatchRegistryKey,    pItem->GetUseCurrentDestRegKey(),		1));
			pItem->m_bUseCurrentDestPalette = iCheck ? TRUE : FALSE;			
		}		
		pItem = GetNextSupportedImage();

	}
}

void CWildCardList::WriteRegistry()
{
	CString strBatchRegistryKey ("BatchSettings");
	CWildCardItem *pItem = GetFirstSupportedImage();
	
	while (pItem)
	{
		if (pItem->GetSourceRegKey() != "")
		{
			theApp.WriteProfileString	(strBatchRegistryKey,  pItem->GetSourceRegKey(),	pItem->GetSourcePaletteFile());
		}

		if (pItem->GetDestRegKey() != "")
		{
			theApp.WriteProfileString	(strBatchRegistryKey,  pItem->GetDestRegKey(),		pItem->GetDestPaletteFile());
		}

		if (pItem->GetUseCurrentSourceRegKey() != "")
		{
			theApp.WriteProfileInt	(strBatchRegistryKey,    pItem->GetUseCurrentSourceRegKey(),		pItem->m_bUseCurrentSourcePalette == TRUE ? 1 : 0);
		}

		if (pItem->GetUseCurrentDestRegKey() != "")
		{
			theApp.WriteProfileInt	(strBatchRegistryKey,    pItem->GetUseCurrentDestRegKey(),			pItem->m_bUseCurrentDestPalette == TRUE ? 1 : 0);
		}

		pItem = GetNextSupportedImage();
	}

}

void CWildCardList::SolidifyAllChanges()
{
	CWildCardItem *pItem = GetFirst();

	while (pItem)
	{
		pItem->SolidifyChanges();
		pItem = pItem->GetNext();
	}
}

void CWildCardList::PrepareForChanges()
{
	CWildCardItem *pItem = GetFirst();

	while (pItem)
	{
		pItem->PrepareForChanges();
		pItem = pItem->GetNext();
	}
}

void CWildCardList::CancelAllChanges()
{
	CWildCardItem *pItem = GetFirst();

	while (pItem)
	{
		pItem->CancelChanges();
		pItem = pItem->GetNext();
	}
}


//////////////////////////////////////////////////////////////////////////////
// ImageHelper construction/destruction
//////////////////////////////////////////////////////////////////////////////

CImageHelper::CImageHelper () 
{
	Initialize();
}

void CImageHelper::Initialize()
{
	m_pbyEncodedData	= NULL;
	m_pbyDecodedData	= NULL;	
	//m_pBMPPalette		= NULL;
	m_psfFile			= NULL;
	m_parFile			= NULL;
	m_pbyAlphaChannel	= NULL;
	m_bUseArchive		= FALSE;

	m_pPcxHeader      = NULL;
	m_pBmfHeader      = NULL;
	m_pBmfInfo        = NULL;

	m_iWidth			= 0;
	m_iHeight			= 0;

	m_pQ2ExportHeader = NULL;
	m_pQ1ExportHeader = NULL;
	m_lpSinExportHeader	= NULL;
	m_lpM8ExportHeader = NULL;
	
	m_iDataSize		= 0;
	m_iFileLength	= 0;
	m_iSpecialFlag	= 0;
	
	m_strErrorText	= "";
	m_strFileName = "";

	SetErrorCode (IH_SUCCESS);
	SetImageType (IMAGE_TYPE_START);	
	SetColorDepth (IH_8BIT);	
	memset (m_byPalette, 0, 768);	

	m_iPNGReadPosition		= 0;
	m_iPNGWritePosition		= 0;
	m_fpPNGOutput			= NULL;
}	

CImageHelper::~CImageHelper()
{
	m_pbyEncodedData		= NULL;	
	
	m_pPcxHeader   = NULL;
	m_pBmfHeader   = NULL;
	m_pBmfInfo     = NULL;

	if (m_psfFile)
	{
		delete m_psfFile;
		m_psfFile		= NULL;
	}

	if (m_pbyDecodedData)
	{
		delete [] m_pbyDecodedData;
		m_pbyDecodedData	= NULL;
	}

//	if (m_pBMPPalette)
//	{
//		delete m_pBMPPalette;
//		m_pBMPPalette	= NULL;
//	}

	if( m_pbyAlphaChannel )
	{
		delete []m_pbyAlphaChannel;
		m_pbyAlphaChannel = NULL;
	}

	if (m_pQ1ExportHeader)
	{
		delete m_pQ1ExportHeader;
		m_pQ1ExportHeader = NULL;
	}

	if (m_pQ2ExportHeader)
	{
		delete m_pQ2ExportHeader;
		m_pQ2ExportHeader = NULL;
	}

	if (m_lpSinExportHeader)
	{
		delete m_lpSinExportHeader;
		m_lpSinExportHeader = NULL;
	}

	if (m_lpM8ExportHeader)
	{
		delete m_lpM8ExportHeader;
		m_lpM8ExportHeader = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////////
// ImageHelper Get/Set functions

void CImageHelper::SetErrorText (LPCTSTR szText)
{
	m_strErrorText = szText;
}

CString CImageHelper::GetErrorText( int iCode /* = IH_NONE */ )
{
	if( iCode == IH_NONE )
	{
		iCode = GetErrorCode();	
	}

	switch( iCode )
	{
	case IH_SUCCESS:
		ASSERT (FALSE);		// There is no error, why are you here?
		break;

	case IH_UNSUPPORTED_IMAGE:
		m_strErrorText =	"Unsupported image type.  Supported types are:\n\n";
		m_strErrorText +=	"\tQuake2 Texture (WAL)\n";
		m_strErrorText +=	"\tQuake1 Texture (MIP)\n";
		m_strErrorText +=	"\tSiN Texture (SWL)\n";
		m_strErrorText +=	"\tSerious Sam Texture (TEX)\n";
		m_strErrorText +=	"\tZSoft PC Paintbrush (PCX)\n";
		m_strErrorText +=	"\tWindows Bitmap (BMP)\n";
		m_strErrorText +=	"\tTruevision Targa (TGA)\n";
		m_strErrorText +=	"\tJPEG - JFIF (JPG)\n";
		m_strErrorText +=	"\tPortable Network Graphic (PNG)\n";		
		break;

	case IH_UNSUPPORTED_PACKAGE:
		m_strErrorText =		"Unsupported package type.  Supported types are:\n\n";
		m_strErrorText +=	"\tHalf-Life WAD3\n";
		m_strErrorText +=	"\tQuake1 WAD2\n";
		break;

	case IH_UNSUPPORTED_PALETTE:
		m_strErrorText =		"Unsupported palette type.  Supported types are:\n\n";
		m_strErrorText +=	"\tJASC PAL\n";
		m_strErrorText +=	"\tQuake LMP\n";
		m_strErrorText +=	"\tPhotoshop ACT\n";
		break;

	case IH_UNSUPPORTED_FILE:
		m_strErrorText =		"Unsupported file type.\n";		
		break;

	case IH_ERROR_READING_FILE:	
		{
			m_strErrorText = "Error reading file ";
			if( m_psfFile )
			{
				m_strErrorText += m_psfFile->GetErrorText();
			}			
		}
		break;

	case IH_ERROR_WRITING_FILE:
		m_strErrorText = "Error opening file ";
		m_strErrorText += m_strFileName;
		m_strErrorText += " for write access";
		break;
	
	case IH_OUT_OF_MEMORY:
		m_strErrorText = "Could not allocate space in memory.";
		break;

	case IH_NOT_DIV16:
		m_strErrorText = "The dimensions of this image are not evenly divisible by 16";
		break;

	case IH_FILE_NOT_FOUND:
		m_strErrorText = "File not found (";
		m_strErrorText += m_strFileName;
		m_strErrorText += ")";
		break;

	case IH_CANT_CREATE_UNIQUE:
		m_strErrorText += m_strFileName;
		m_strErrorText += ":  can't create unique filename";
		break;	
	
	case IH_BMP_UNSUPPORTED_COMPRESSION:
		m_strErrorText = "BMP Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nCompression not supported.  Supported compression is BI_RGB";
		break;

	case IH_BMP_UNSUPPORTED_VERSION:
		m_strErrorText = "BMP Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nUnsupported version.";
		break;

	case IH_PAL_MALFORMED:
	case IH_PCX_MALFORMED:
	case IH_BMP_MALFORMED:
	case IH_PNG_MALFORMED:
	case IH_TGA_MALFORMED:
	case IH_JPG_MALFORMED:
	case IH_TEX_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nImage is malformed";
		break;

	case IH_TGA_UNSUPPORTED_BIT_DEPTH:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nUnsupported bit depth.";
		break;

	case IH_TGA_UNSUPPORTED_PALETTE:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nUnsupported palette bit depth.";
		break;

	case IH_PAL_INVALID_TYPE:
		m_strErrorText = "Unsupported PAL format";
		break;

	case IH_PAL_WRONG_NUM_COLORS:
		m_strErrorText = "Only 256-color PAL files are supported";
		break;
	
	case IH_WAL_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\n.wal header is malformed";
		break;

	case IH_WAD_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nPackage file is malformed";
		break;
		
	case IH_WAD_NO_IMAGES:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nPackage file has no items";
		break;

	case IH_WAD_UNSUPPORTED_VERSION:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nUnsupported WAD type";
		break;

	case IH_MIP_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\n.mip header is malformed";
		break;

	case IH_M8_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nTexture is malformed";
		break;

	case IH_SWL_MALFORMED:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nTexture is malformed";
		break;

	case IH_PNG_READ_ERROR:
	case IH_PNG_WRITE_ERROR:
	case IH_PNG_CUSTOM_ERROR:
	case IH_JPG_CUSTOM_ERROR:
		// Text was set manually
		break;

	case IH_PNG_UNSUPPORTED_VERSION:
		m_strErrorText = "Error with ";
		m_strErrorText += m_strFileName;
		m_strErrorText += "\n\nUnsupported version.";
		break;

	case IH_TEX_UNSUPPORTED_VERSION:
		{
			m_strErrorText = "Error with ";
			m_strErrorText += m_strFileName;
			m_strErrorText += "\n\nUnsupported version.";
		}
		break;

	case IH_TEX_TOO_MANY_FRAMES:
		{
			m_strErrorText += m_strFileName;
			m_strErrorText += " contains too many frames.\n\n";
			m_strErrorText += "Wally supports a maximum of one frame per TEX file right\n";
			m_strErrorText += "now.  We are working on the next release which will\n";
			m_strErrorText += "support multiple frames.";
		}
		break;

	case IH_TEX_ANIMATION_DATA:
		{
			m_strErrorText += m_strFileName;
			m_strErrorText += " contains animation data.\n\n";
			m_strErrorText += "Wally does not fully support the TEX file format right\n";
			m_strErrorText += "now.  We are working on the next release which will\n";
			m_strErrorText += "support animation frames.";
		}
		break;

	default:
		ASSERT (FALSE);		// There has to be an error for you to be here!
		break;
	}

	return m_strErrorText;
}


int CImageHelper::GetImageFlag()
{
	return m_iSpecialFlag;	
}

void CImageHelper::SetImageType (int iType)
{
	m_iImageType = iType;
}


int CImageHelper::GetImageType ()
{
	return m_iImageType;
}
	
LPBYTE CImageHelper::GetBits()
{	
	return m_pbyDecodedData;
}

LPBYTE CImageHelper::GetPalette()
{
	return m_byPalette;
}

LPBYTE CImageHelper::GetAlphaChannel ()
{
	return m_pbyAlphaChannel;
}

void CImageHelper::SetErrorCode (int iCode)
{
	m_iErrorCode = iCode;
}

int CImageHelper::GetErrorCode ()
{
	return m_iErrorCode;
}

void CImageHelper::SetImageWidth(int iWidth)
{
	m_iWidth = iWidth;
}

int CImageHelper::GetImageWidth()
{
	return m_iWidth;
}

void CImageHelper::SetImageHeight(int iHeight)
{
	m_iHeight = iHeight;
}

int CImageHelper::GetImageHeight()
{
	return m_iHeight;
}

int CImageHelper::GetFileLength ()
{
	return( m_psfFile->GetDataSize() );
}


BOOL CImageHelper::SetFileName(LPCTSTR szFileName)
{	
	m_strFileName = szFileName;
	if (m_strFileName == "")
	{
		SetErrorCode (IH_UNSUPPORTED_FILE);
		return FALSE;
	}

	CString szExtension = GetExtension (szFileName);

	// TODO:  this code really should not care about the file extension, 
	// it should run through a series of checks on the header information
	// to determine the exact format of the file being loaded.
	if (szExtension == ".pcx")
	{
		SetImageType (IH_PCX_TYPE);
		return TRUE;
	}

	if (szExtension == ".bmp")
	{
		SetImageType (IH_BMP_TYPE);
		return TRUE;
	}

	if (szExtension == ".lmp")
	{
		SetImageType (IH_LMP_TYPE);
		return TRUE;
	}
	
	if (szExtension == ".pal")
	{
		SetImageType (IH_PAL_TYPE);
		return TRUE;
	}	
	
	if (szExtension == ".act")
	{
		SetImageType (IH_ACT_TYPE);
		return TRUE;
	}

	if (szExtension == ".mip")
	{
		SetImageType (IH_MIP_TYPE);
		return TRUE;
	}

	if (szExtension == ".wal")
	{
		SetImageType (IH_WAL_TYPE);
		return TRUE;
	}

	if (szExtension == ".wad")
	{
		// Have to determine what version of WAD
		if (DetermineWADType())
		{
			return TRUE;
		}
		else
		{
			SetErrorCode (IH_WAD_UNSUPPORTED_VERSION);
			return FALSE;
		}
	}

	if (szExtension == ".swl")
	{
		SetImageType (IH_SWL_TYPE);
		return TRUE;
	}

	if (szExtension == ".m8")
	{
		SetImageType (IH_M8_TYPE);
		return TRUE;
	}

	if (szExtension == ".tga")
	{
//SetErrorCode (IH_WAD_UNSUPPORTED_VERSION);		// TEST TEST TEST
//return FALSE;
		SetImageType (IH_TGA_TYPE);
		return TRUE;
	}

	if (szExtension == ".png")
	{
		SetImageType (IH_PNG_TYPE);
		return TRUE;
	}

	if (szExtension == ".art")
	{
		SetImageType (IH_ART_TYPE);
		return TRUE;
	}

	if (szExtension == ".jpg")
	{
		SetImageType (IH_JPG_TYPE);
		return TRUE;
	}

	if (szExtension == ".tex")
	{
		SetImageType (IH_TEX_TYPE);
		return TRUE;
	}

#ifdef _DEBUG
	/*
	if (szExtension == ".gif")
	{
		SetImageType (IH_GIF_TYPE);
		return TRUE;
	}

	if (szExtension == ".wly")
	{
		SetImageType (IH_WLY_TYPE);
		return TRUE;
	}
	*/
#endif
	
	if (GetErrorCode() == IH_SUCCESS)
	{
		SetErrorCode (IH_UNSUPPORTED_FILE);
	}
	return FALSE;
}

PCXHEADER* CImageHelper::GetPCXHeader()
{
	return m_pPcxHeader;	
}

BITMAPFILEHEADER* CImageHelper::GetBMPHeader()
{
	return m_pBmfHeader;
}

BITMAPINFO* CImageHelper::GetBMPInfo()
{
	return m_pBmfInfo;
}

void CImageHelper::SetColorDepth(int iDepth)
{
	m_iColorDepth = iDepth;
}

int CImageHelper::GetColorDepth()
{
	return m_iColorDepth;
}

//int CImageHelper::PadDWORD (int x)
//{
//	div_t xdiv = div ((x + 3), 4);	
//	return (4 * xdiv.quot);
//}


CWildCardItem *CImageHelper::GetFirstSupportedImage()
{
	CWildCardItem *pItem = g_WildCardList.GetFirstSupportedImage();

	while (pItem)
	{
		if (IsValidImage (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}

	return NULL;
}

CWildCardItem *CImageHelper::GetNextSupportedImage()
{
	CWildCardItem *pItem = g_WildCardList.GetNextSupportedImage();

	while (pItem)
	{
		if (IsValidImage (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}
	return NULL;
}

CWildCardItem *CImageHelper::GetFirstSupportedPackage()
{
	CWildCardItem *pItem = g_WildCardList.GetFirstSupportedImage();

	while (pItem)
	{
		if (IsValidPackage (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}

	return NULL;
}

CWildCardItem *CImageHelper::GetNextSupportedPackage()
{
	CWildCardItem *pItem = g_WildCardList.GetNextSupportedImage();

	while (pItem)
	{
		if (IsValidPackage (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}
	return NULL;
}

CWildCardItem *CImageHelper::GetFirstSupportedPak()
{
	CWildCardItem *pItem = g_WildCardList.GetFirstSupportedImage();

	while (pItem)
	{
		if (IsValidPak (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}

	return NULL;
}

CWildCardItem *CImageHelper::GetNextSupportedPak()
{
	CWildCardItem *pItem = g_WildCardList.GetNextSupportedImage();

	while (pItem)
	{
		if (IsValidPak (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}
	return NULL;
}

CWildCardItem *CImageHelper::GetFirstSupportedPalette()
{
	CWildCardItem *pItem = g_WildCardList.GetFirstSupportedImage();

	while (pItem)
	{
		if (IsValidPalette (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}

	return NULL;
}

CWildCardItem *CImageHelper::GetWildCardAt (int iPosition)
{
	return g_WildCardList.GetAt (iPosition);
}

CWildCardItem *CImageHelper::GetNextSupportedPalette()
{
	CWildCardItem *pItem = g_WildCardList.GetNextSupportedImage();

	while (pItem)
	{
		if (IsValidPalette (pItem->GetImageType()))
		{
			return pItem;
		}
		pItem = g_WildCardList.GetNextSupportedImage();
	}
	return NULL;
}

CString CImageHelper::GetSupportedPaletteList()
{
	CString strList ("");

	if (g_WildCardList.GetCount() == 0)
	{
		return strList;
	}

	CWildCardItem *pItem = GetFirstSupportedPalette();

	while (pItem)
	{
		strList += pItem->GetWildCard();
		pItem = GetNextSupportedPalette();
	}
	
	strList += "|";
	return strList;
}

CString CImageHelper::GetNon16ImageList()
{
	CString strList ("");

	if (g_WildCardList.GetCount() == 0)
	{
		return strList;
	}

	CWildCardItem *pItem = GetFirstSupportedImage();

	while (pItem)
	{
		if (!IsGameType (pItem->GetImageType()))
		{
			strList += pItem->GetWildCard();
		}
		pItem = GetNextSupportedImage();
	}
	
	strList += "|";

	return strList;
}

CString CImageHelper::GetSupportedWADList()
{
	CString strList ("");

	if (g_WildCardList.GetCount() == 0)
	{
		return strList;
	}

	CWildCardItem *pItem = GetFirstSupportedPackage();
	while (pItem)
	{
		strList += pItem->GetWildCard();
		pItem = GetNextSupportedPackage();		
	}

	strList += "|";

	return strList;
}

CString CImageHelper::GetSupportedImageList(BOOL bIncludePackages /* = FALSE */)
{
	CString strList ("");

	if (g_WildCardList.GetCount() == 0)
	{
		return strList;
	}

	CWildCardItem *pItem = GetFirstSupportedImage();

	while (pItem)
	{
		strList += pItem->GetWildCard();
		pItem = GetNextSupportedImage();
	}

	if (bIncludePackages)
	{
		pItem = GetFirstSupportedPackage();
		while (pItem)
		{
			strList += pItem->GetWildCard();
			pItem = GetNextSupportedPackage();		
		}

		pItem = GetFirstSupportedPak();
		while (pItem)
		{
			strList += pItem->GetWildCard();
			pItem = GetNextSupportedPak();		
		}
	}
	strList += "|";

	return strList;
}

CString CImageHelper::GetWildCardExtension (int iIndex)
{
	CWildCardItem *pItem = GetFirstSupportedImage();
	int iMarker = 0;

	while (pItem)
	{		
		if (iMarker == iIndex)
		{
			return pItem->GetWildCardExtension();
		}
		pItem = GetNextSupportedImage();
		iMarker++;
	}
	return "";
}

//////////////////////////////////////////////////////////////////////////////
//  ImageHelper functions
void CImageHelper::ResetContent()
{
	SetErrorCode (IH_SUCCESS);

	m_pbyEncodedData = NULL;
	
	if (m_pbyDecodedData)
	{
		delete [] m_pbyDecodedData;
		m_pbyDecodedData	= NULL;
	}	

	if (m_psfFile)
	{
		delete m_psfFile;
		m_psfFile	= NULL;
	}

	m_WADList.PurgeList();
	
	Initialize();
}

void CImageHelper::LoadImage (CFile *cfOpen, int iFlags /* = 0*/)
{
	ASSERT (cfOpen);

	CString strFileName (cfOpen->GetFilePath());
	LoadImage (strFileName, iFlags | IH_LOAD_FILEOPEN);
}

void CImageHelper::LoadImage(LPCTSTR szFileName, int iFlags /* = 0*/)
{	
	if (szFileName[0] == NULL)
	{
		SetErrorCode (IH_FILE_NOT_FOUND);
		return;
	}

	DWORD dwDataSize = 0;
	
	if ((iFlags & IH_LOAD_FILEOPEN) == 0)
	{
		// First verify the file is even there
		FILE* fp = NULL;
		errno_t err = fopen_s(&fp, szFileName, "rb");
		if (err != 0)
		{
			m_strFileName = szFileName;
			SetErrorCode (IH_FILE_NOT_FOUND);
			return;
		}
		fclose (fp);
	}
	
	// Make sure it's one of our supported types
	if (!SetFileName(szFileName))
	{
		return;
	}	
	
	int iType = GetImageType();

	// Check to see whether the coder wants this type of file
	if (iFlags & IH_LOAD_ONLYIMAGE)
	{
		if (!IsValidImage (iType))
		{
			SetErrorCode (IH_UNSUPPORTED_IMAGE);
			return;
		}
	}

	if (iFlags & IH_LOAD_ONLYPACKAGE)
	{
		if (!IsValidPackage (iType))
		{
			SetErrorCode (IH_UNSUPPORTED_PACKAGE);
			return;
		}
	}

	if (iFlags & IH_LOAD_ONLYPALETTE)
	{
		if (!IsValidPalette (iType))
		{
			SetErrorCode (IH_UNSUPPORTED_PALETTE);
			return;
		}
	}

	//ResetContent();
	if (m_psfFile)
	{
		delete m_psfFile;
		m_psfFile	= NULL;
	}
	
	m_psfFile = new CSafeFile( szFileName );
	if( m_psfFile->GetErrorCode() != SF_SUCCESS )
	{
		SetErrorCode( IH_ERROR_READING_FILE );
		return;
	}

	dwDataSize = m_psfFile->GetDataSize();
	SetDataSize( dwDataSize );

	switch (iType)
	{
	case IH_PCX_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodePCX( iFlags );
		break;

	case IH_BMP_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeBMP( iFlags );
		break;

	case IH_TGA_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeTGA( iFlags );
		break;

#ifdef _TEST
	case IH_GIF_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeGIF( iFlags );
		break;
#endif
	
	case IH_PAL_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodePAL( iFlags );
		break;

	case IH_ACT_TYPE :
	case IH_LMP_TYPE :
		m_pbyDecodedData = new BYTE[ dwDataSize ];
		memcpy( m_pbyDecodedData, m_psfFile->GetBuffer(), dwDataSize );
		break;

	case IH_MIP_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeMIP( iFlags );
		break;

	case IH_WAL_TYPE:
		// This is used only really by the batch conversion stuff; TRUE
		// decoding with all four submips and mip header information has
		// to be handled by CWallyDoc.
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeWAL( iFlags );
		break;

	case IH_M8_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeM8( iFlags );
		break;

	case IH_SWL_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeSWL( iFlags );
		break;

	case IH_WAD2_TYPE :
	case IH_WAD3_TYPE :
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		m_WADList.Serialize (NULL, NULL, m_pbyEncodedData, FALSE);
		break;

	case IH_ART_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		m_BuildList.Serialize (NULL, NULL, m_pbyEncodedData, FALSE);
		break;

	case IH_PNG_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodePNG( iFlags );
		break;

	case IH_JPG_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeJPG( iFlags );
		break;

	case IH_TEX_TYPE:
		m_pbyEncodedData = m_psfFile->GetBuffer();
		ASSERT( m_pbyEncodedData);
		DecodeTEX( iFlags );
		break;
		
	default:
		ASSERT (FALSE);		// Unhandled type?
		break;		
	}

	m_psfFile->ReleaseFile();
}

void CImageHelper::SaveImage (int iColorDepth, CArchive *parFile, LPBYTE pbyRawData, LPBYTE pbyPalette, int iWidth, int iHeight)
{
	SetErrorCode (IH_SUCCESS);
	ASSERT (parFile);

	m_parFile = parFile;
	CString strFilePath("");
	strFilePath = parFile->GetFile()->GetFilePath();

	if (!SetFileName(strFilePath))
	{
		return;
	}

	int iType = GetImageType();
	m_bUseArchive = TRUE;

	if (!IsValidPalette())
	{
		ASSERT (pbyRawData);		// Must pass in valid data	
		ASSERT (iHeight > 0);
		ASSERT (iWidth > 0);
		ASSERT ((iColorDepth == IH_8BIT) || (iColorDepth == IH_24BIT));
		SetImageHeight (iHeight);
		SetImageWidth (iWidth);
		SetColorDepth (iColorDepth);
	}

	m_pbyDecodedData = pbyRawData;

	if (pbyPalette)
	{
		memcpy (m_byPalette, pbyPalette, 768);
	}
	
	switch (iType)
	{
	case IH_PCX_TYPE :
		EncodePCX ();
		break;

	case IH_BMP_TYPE :
		EncodeBMP ();
		break;

	case IH_TGA_TYPE:
		EncodeTGA();
		break;

	case IH_LMP_TYPE:
	case IH_ACT_TYPE:
		EncodeRawData ();
		break;

	case IH_PAL_TYPE:
		EncodePAL ();
		break;

	case IH_MIP_TYPE:
		EncodeMIP();
		break;

	case IH_WAL_TYPE:
		EncodeWAL();
		break;

	case IH_SWL_TYPE:
		EncodeSWL();
		break;

	case IH_M8_TYPE:
		EncodeM8();
		break;

	case IH_PNG_TYPE:
		EncodePNG();
		break;

	case IH_JPG_TYPE:
		EncodeJPG();
		break;

	case IH_TEX_TYPE:
		EncodeTEX();
		break;
	
	default:
		ASSERT (FALSE);		// Unhandled implementation
		break;		
	}	
	m_pbyDecodedData = NULL;
}

void CImageHelper::SaveImage(int iColorDepth, LPCTSTR szFileName, unsigned char *pbyRawData, unsigned char *pbyPalette, int iWidth, int iHeight)
{	
	ASSERT (szFileName);
	if (!SetFileName(szFileName))
	{
		return;
	}		
	SetErrorCode (IH_SUCCESS);

	int iType = GetImageType();

	if (!IsValidPalette())
	{
		ASSERT (pbyRawData);		// Must pass in valid data	
		ASSERT (iHeight > 0);
		ASSERT (iWidth > 0);
		ASSERT ((iColorDepth == IH_8BIT) || (iColorDepth == IH_24BIT));
		SetImageHeight (iHeight);
		SetImageWidth (iWidth);
		SetColorDepth (iColorDepth);
	}

	m_pbyDecodedData = pbyRawData;

	if (pbyPalette)
	{
		memcpy (m_byPalette, pbyPalette, 768);
	}
	
	switch (iType)
	{
	case IH_PCX_TYPE :
		EncodePCX ();
		break;

	case IH_BMP_TYPE :
		EncodeBMP ();
		break;

	case IH_TGA_TYPE:
		EncodeTGA();
		break;

	case IH_LMP_TYPE:
	case IH_ACT_TYPE:
		EncodeRawData ();
		break;

	case IH_PAL_TYPE:
		EncodePAL ();
		break;

	case IH_MIP_TYPE:
		EncodeMIP();
		break;

	case IH_WAL_TYPE:
		EncodeWAL();
		break;

	case IH_SWL_TYPE:
		EncodeSWL();
		break;

	case IH_M8_TYPE:
		EncodeM8();
		break;

	case IH_PNG_TYPE:
		EncodePNG();
		break;

	case IH_JPG_TYPE:
		EncodeJPG();
		break;

	case IH_TEX_TYPE:
		EncodeTEX();
		break;
	
	default:
		ASSERT (FALSE);		// Unhandled implementation
		break;		
	}	
	m_pbyDecodedData = NULL;
}

void CImageHelper::ConvertImage (int iColorDepth, LPCTSTR szDestinationDirectory, int iImageType, int iFlags /* = 0 */)
{
	ASSERT (m_pbyDecodedData);		// You must already have loaded an image
	
	BYTE *pbySourceData	 = m_pbyDecodedData;

	// Hold onto the original location of the source data, so we can point back when we're done
	BYTE *pbyHoldSourceData = m_pbyDecodedData;
	BYTE *pbySourcePalette	 = GetPalette();
	int iWidth = GetImageWidth();
	int iHeight = GetImageHeight();
	int iSize = iWidth * iHeight;
	int iSourceDepth = GetColorDepth();
	int iSourceType = GetImageType();

	// Go map the Q1/Q2 header, as appropriate
	MapGameFlags (iSourceType, m_pbyEncodedData, m_strFileName);

	// If the flags include IH_SAVE_UNIQUE_FILENAME, then the caller would like us 
	// to generate a unique filename, based on the raw file name of the original,
	// and the destination type

	CString strDestinationFileName ("");

	ASSERT (szDestinationDirectory);
	CString strDestinationDirectory ("");	
	strDestinationDirectory = TrimSlashes (szDestinationDirectory);
	strDestinationDirectory += "\\";
		
	CString strRawFileName("");
	strRawFileName = GetRawFileName (m_strFileName);
	CString strExtension = GetWildCardExtension (iImageType);	
	
	strDestinationFileName = strDestinationDirectory + strRawFileName;
	strDestinationFileName += "." + strExtension;

	UINT j = 0;	
	int r, g, b;

	if (iFlags & IH_SAVE_UNIQUE_FILENAME)
	{
		FILE* rp = NULL;
		errno_t err = fopen_s(&rp, strDestinationFileName, "r");

		while (err == 0)
		{
			strDestinationFileName.Format ("%s%s_%04d.%s", strDestinationDirectory, strRawFileName, j, strExtension);
			err = fopen_s(&rp, strDestinationFileName, "r");
			j++;
			if (j >= UINT_MAX)
			{
				SetErrorCode (IH_CANT_CREATE_UNIQUE);
				return;				
			}
		}

	}

	CWildCardItem *pItem = g_WildCardList.GetAt(iImageType);

	if (iFlags & IH_USE_BATCH_SETTINGS)
	{
		if (!pItem->UseCurrentSourcePalette())
		{
			ASSERT ((iImageType == IH_WAL_TYPE) || (iImageType == IH_MIP_TYPE));			
			pItem->GetSourcePalette (pbySourcePalette);			
		}

		if (!pItem->UseCurrentDestPalette())
		{
			// Data needs to be converted

			COLOR_IRGB* pTemp24Bit = new COLOR_IRGB [iSize * sizeof( COLOR_IRGB)];

			if (!pTemp24Bit)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				return;
			}

			switch (iSourceDepth)			
			{
			case IH_8BIT:				
				for (j = 0; j < iSize; j++)
				{
					r = pbySourcePalette[pbySourceData[j] * 3 + 0];
					g = pbySourcePalette[pbySourceData[j] * 3 + 1];
					b = pbySourcePalette[pbySourceData[j] * 3 + 2];

					pTemp24Bit[j] = IRGB( 0, r, g, b);
				}
				break;

			case IH_24BIT:
				for (j = 0; j < iSize; j++)
				{
					r = pbySourceData[j * 3 + 0];
					g = pbySourceData[j * 3 + 1];
					b = pbySourceData[j * 3 + 2];

					pTemp24Bit[j] = IRGB( 0, r, g, b);
				}
				break;
			}

			CWallyPalette Palette;
			BYTE byPalette[768];
			
			if (pItem->GetDestPalette (byPalette))
			{
				Palette.SetPalette (byPalette, 256);

				Palette.Convert24BitTo256Color( pTemp24Bit, 
					pbySourceData, iWidth, iHeight, 0, GetDitherType(), FALSE);

				memcpy (pbySourcePalette, byPalette, 768);

				// We're now down to 8-bit
				iSourceDepth = IH_8BIT;

			}

			if (pTemp24Bit)
			{
				delete [] pTemp24Bit;
				pTemp24Bit = NULL;
			}			
		}
	}

	switch (iSourceDepth)
	{
	case IH_8BIT:
		{
			switch (iColorDepth)
			{
			case IH_8BIT:
				// Our data is good to go
				SaveImage (IH_8BIT, strDestinationFileName, pbySourceData, pbySourcePalette, iWidth, iHeight);
				break;

			case IH_24BIT:
				{
					ASSERT (iImageType != IH_WAL_TYPE);
					ASSERT (iImageType != IH_MIP_TYPE);

					unsigned char *pby24BitData = new unsigned char[iSize * 3];
					if (!pby24BitData)
					{
						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}
					
					for (int j = 0; j < iSize; j++)
					{
						pby24BitData[ j * 3 + 0 ] = pbySourcePalette[ (pbySourceData[j] * 3) + 0 ];
						pby24BitData[ j * 3 + 1 ] = pbySourcePalette[ (pbySourceData[j] * 3) + 1 ];
						pby24BitData[ j * 3 + 2 ] = pbySourcePalette[ (pbySourceData[j] * 3) + 2 ];
					}

					SaveImage (IH_24BIT, strDestinationFileName, pby24BitData, NULL, iWidth, iHeight);

					if (pby24BitData)
					{
						delete [] pby24BitData;
						pby24BitData = NULL;
					}				
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}
		}
		break;

	case IH_24BIT:
		{
			switch (iColorDepth)		
			{
			case IH_8BIT:
				{
					unsigned char *pby8BitData = new unsigned char[iSize];
					if (!pby8BitData)
					{
						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}
					memset (pby8BitData, 0, iSize);

					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
					if (!pTemp24Bit)
					{
						if( pby8BitData )
						{
							delete []pby8BitData;
							pby8BitData = NULL;
						}

						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}

					BYTE byPalette[256*3];					
					CColorOptimizer ColorOpt;
					CWallyPalette Palette;
					int r, g, b;

					for (int j = 0; j < iSize; j++)
					{
						r = pbySourceData[j * 3 + 0];
						g = pbySourceData[j * 3 + 1];
						b = pbySourceData[j * 3 + 2];						

						if (iImageType == IH_WAL_TYPE)
						{
							if ((r != 159) || (g != 91) || (b != 83))
								pTemp24Bit[j] = IRGB( 0, r, g, b);
							else
								pTemp24Bit[j] = IRGB( 255, r, g, b);
						}
						else
						{
							pTemp24Bit[j] = IRGB( 0, r, g, b);
						}
					}
					
					switch (iImageType)
					{
					case IH_WAL_TYPE:
						memcpy (byPalette, quake2_pal, 768);
						Palette.SetPalette (quake2_pal, 256);
						break;

					case IH_MIP_TYPE:
						memcpy (byPalette, quake1_pal, 768);
						Palette.SetPalette (quake1_pal, 256);
						break;	

					case IH_M8_TYPE:
					case IH_SWL_TYPE:
					case IH_PCX_TYPE:
					case IH_BMP_TYPE:
					case IH_TGA_TYPE:						
						ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 256, TRUE);												
						Palette.SetPalette( byPalette, 256);						
						break;

					default:
						TRACE ("Unhandled ImageType in ImageHelper::ConvertImage()\n");
						ASSERT (FALSE);
						break;
					}

					Palette.Convert24BitTo256Color( pTemp24Bit, pby8BitData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					SaveImage (IH_8BIT, strDestinationFileName, pby8BitData, byPalette, iWidth, iHeight);

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
						pTemp24Bit = NULL;
					}
					if (pby8BitData)
					{
						delete [] pby8BitData;
						pby8BitData = NULL;
					}					
				}
				break;
						
			case IH_24BIT:
				ASSERT (iImageType != IH_WAL_TYPE);
				ASSERT (iImageType != IH_MIP_TYPE);
				ASSERT (iImageType != IH_SWL_TYPE);
				ASSERT (iImageType != IH_M8_TYPE);
				
				// Our data is good to go as-is
				SaveImage (IH_24BIT, strDestinationFileName, pbySourceData, NULL, iWidth, iHeight);				
				break;				

			default:
				ASSERT (FALSE);
				break;
			}
		}
		break;

	case IH_32BIT:
		{
			switch (iColorDepth)		
			{
			case IH_8BIT:
				{
					unsigned char *pby8BitData = new unsigned char[iSize];
					if (!pby8BitData)
					{
						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}
			
					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
					if (!pTemp24Bit)
					{
						if( pby8BitData )
						{
							delete []pby8BitData;
							pby8BitData = NULL;
						}

						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}

					BYTE byPalette[256 * 3];
					CColorOptimizer ColorOpt;
					CWallyPalette Palette;
					int r, g, b;

					for (int j = 0; j < iSize; j++)
					{
						r = pbySourceData[j * 4 + 0];
						g = pbySourceData[j * 4 + 1];
						b = pbySourceData[j * 4 + 2];						

						if (iImageType == IH_WAL_TYPE)
						{
							if ((r != 159) || (g != 91) || (b != 83))
								pTemp24Bit[j] = IRGB( 0, r, g, b);
							else
								pTemp24Bit[j] = IRGB( 255, r, g, b);
						}
						else
						{
							pTemp24Bit[j] = IRGB( 0, r, g, b);
						}
					}
					
					switch (iImageType)
					{
					case IH_WAL_TYPE:
						memcpy (byPalette, quake2_pal, 768);
						Palette.SetPalette (quake2_pal, 256);
						break;

					case IH_MIP_TYPE:
						memcpy (byPalette, quake1_pal, 768);
						Palette.SetPalette (quake1_pal, 256);
						break;	

					case IH_M8_TYPE:
					case IH_SWL_TYPE:
					case IH_PCX_TYPE:
					case IH_BMP_TYPE:
					case IH_TGA_TYPE:						
						ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 256, TRUE);												
						Palette.SetPalette( byPalette, 256);						
						break;

					default:
						TRACE ("Unhandled ImageType in ImageHelper::ConvertImage()\n");
						ASSERT (FALSE);
						break;
					}

					Palette.Convert24BitTo256Color( pTemp24Bit, pby8BitData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					SaveImage( IH_8BIT, strDestinationFileName, pby8BitData, byPalette, iWidth, iHeight );

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
						pTemp24Bit = NULL;
					}
					if (pby8BitData)
					{
						delete [] pby8BitData;
						pby8BitData = NULL;
					}					
				}
				break;
						
			case IH_24BIT:
				{
					ASSERT (iImageType != IH_WAL_TYPE);
					ASSERT (iImageType != IH_MIP_TYPE);
					ASSERT (iImageType != IH_SWL_TYPE);
					ASSERT (iImageType != IH_M8_TYPE);
				
					// Build a 24-bit array
					LPBYTE pby24BitData = new BYTE[ iSize * 3 ];
					if( !pby24BitData )
					{
						SetErrorCode (IH_OUT_OF_MEMORY);								
						return;
					}
					
					for( int j = 0; j < iSize; j++ )
					{
						pby24BitData[ j * 3 + 0 ] = pbySourceData[ j * 4 + 0 ];
						pby24BitData[ j * 3 + 1 ] = pbySourceData[ j * 4 + 1 ];
						pby24BitData[ j * 3 + 2 ] = pbySourceData[ j * 4 + 2 ];						
					}
					SaveImage( IH_24BIT, strDestinationFileName, pby24BitData, NULL, iWidth, iHeight );

					if( pby24BitData )
					{
						delete []pby24BitData;
						pby24BitData = NULL;
					}
				}
				break;				

			default:
				ASSERT (FALSE);
				break;
			}
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	m_pbyDecodedData = pbyHoldSourceData;
}


BOOL CImageHelper::DecodePAL( int iFlags /* = 0 */ )
{		
	BOOL bEOF = FALSE;
	unsigned char *p_Position = m_pbyEncodedData;
	
	m_pbyDecodedData = new BYTE[768];
	memset (m_pbyDecodedData, 0, 768);
	CString strCompare("");
	int iColors = 0;
	int iLength = GetFileLength();

	if (iLength == 768)
	{
		// Assume it's an LMP file
		memcpy (m_pbyDecodedData, m_pbyEncodedData, 768);
		return TRUE;
	}

	strCompare = "";
		
	while ((*p_Position) != 0x0d)
	{
		if ((*p_Position) != ' ')
		{
			strCompare += (*p_Position);
		}
		if (p_Position == (m_pbyEncodedData + iLength))
		{
			SetErrorCode (IH_PAL_MALFORMED);
			return FALSE;
		}
		p_Position++;
	}
	p_Position += 2;

	if (strCompare != "JASC-PAL")
	{
		SetErrorCode (IH_PAL_INVALID_TYPE);
		return FALSE;
	}
		
	// Just grab the version(?) line and ignore it
	while ((*p_Position) != 0x0d)
	{
		if (p_Position == (m_pbyEncodedData + iLength))
		{
			SetErrorCode (IH_PAL_MALFORMED);
			return FALSE;
		}
		p_Position++;
	}
	p_Position += 2;

	// Find out how many colors there are
	strCompare = "";
	while ((*p_Position) != 0x0d)
	{
		strCompare += (*p_Position);
		if (p_Position == (m_pbyEncodedData + iLength))
		{
			SetErrorCode (IH_PAL_MALFORMED);
			return FALSE;
		}
		p_Position++;
	}
	p_Position += 2;
	
	iColors = atol (strCompare);		
	if (iColors != 256)
	{
		SetErrorCode (IH_PAL_WRONG_NUM_COLORS);
		return FALSE;
	}

	// We're past all the header stuff... now it's time for the items!		
	for (int j = 0; j < 256; j++)
	{
		// Strip out any spaces at the front 
		while (*(p_Position) == ' ')
		{
			p_Position++;
		}
		strCompare = "";
		
		// Read up to the next space
		while ((*p_Position) != ' ')
		{			
			strCompare += (*p_Position);
			if (p_Position == (m_pbyEncodedData + iLength))
			{
				SetErrorCode (IH_PAL_MALFORMED);
				return FALSE;
			}
			p_Position++;
		}
		p_Position++;

		m_pbyDecodedData[j * 3] = atoi(strCompare);

		strCompare = "";		
		
		// Read up to the next space
		while ((*p_Position) != ' ')
		{			
			strCompare += (*p_Position);
			if (p_Position == (m_pbyEncodedData + iLength))
			{
				SetErrorCode (IH_PAL_MALFORMED);
				return FALSE;
			}
			p_Position++;
		}
		p_Position++;
		
		m_pbyDecodedData[j * 3 + 1] = atoi(strCompare);

		strCompare = "";		

		// Read up to the EOL
		while ((*p_Position) != 0x0d)
		{			
			strCompare += (*p_Position);
			if (p_Position == (m_pbyEncodedData + iLength))
			{
				SetErrorCode (IH_PAL_MALFORMED);
				return FALSE;
			}
			p_Position++;
		}
		p_Position += 2;
		
		m_pbyDecodedData[j * 3 + 2] = atoi(strCompare);		
	}

	return TRUE;
}

BOOL CImageHelper::EncodePAL ()
{
	if (m_bUseArchive)
	{
		CString strLine("");
		strLine.Format ("JASC-PAL\n0100\n256\n");
		
		(*m_parFile) << strLine;

		for (int j = 0; j < 256; j++)
		{			
			strLine.Format ("%d %d %d\n", m_byPalette[j * 3], m_byPalette[j * 3 + 1], m_byPalette[j * 3 + 2]);
			(*m_parFile) << strLine;
		}
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s(&fp, m_strFileName, "wt");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);		
			return FALSE;
		}
		else
		{
			fprintf (fp,"JASC-PAL\n0100\n256\n");

			for (int j = 0; j < 256; j++)
			{
				fprintf (fp, "%d %d %d\n", m_byPalette[j * 3], m_byPalette[j * 3 + 1], m_byPalette[j * 3 + 2]);
			}		
			fclose(fp);	
		}
	}

	return TRUE;
}

BOOL CImageHelper::DecodeMIP( int iFlags /* = 0 */ )
{	
	SetColorDepth (IH_8BIT);

	LPQ1_MIP_S Q1MipHeader = (LPQ1_MIP_S) m_pbyEncodedData;
	int iHeaderSize  = Q1_HEADER_SIZE;
	int iFileLength = GetFileLength();
		
	if (iFileLength < iHeaderSize)
	{
		SetErrorCode (IH_MIP_MALFORMED);		
		return FALSE;
	}

	int iWidth = Q1MipHeader->width;
	int iHeight = Q1MipHeader->height;
	int iSize = iWidth * iHeight;
	int iTotalSize = 0;
	int j = 0;

	iTotalSize = iHeaderSize + iSize;

	if (iTotalSize > iFileLength)
	{
		SetErrorCode (IH_MIP_MALFORMED);		
		return FALSE;
	}	

	SetImageHeight (iHeight);
	SetImageWidth (iWidth);

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}
	
	m_pbyDecodedData = new BYTE[iSize];
	memset (m_pbyDecodedData, 0, iSize);

	int iOffset = Q1MipHeader->offsets[0];
	
	if ( (iOffset + iSize) > iFileLength)
	{
		// Try to recover as much as possible		
		if ( iOffset < iFileLength)
		{
			memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iFileLength - iOffset);
		}
	}
	else
	{
		memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iSize);
	}

	memcpy (m_byPalette, quake1_pal, 768);
	return TRUE;
}

BOOL CImageHelper::EncodeMIP ()
{
	int iHeaderSize = Q1_HEADER_SIZE;

	ASSERT (m_pQ1ExportHeader);		// You must initialize the header first!	
	ASSERT (m_pbyDecodedData);

	if (!m_pQ1ExportHeader)
	{
		m_pQ1ExportHeader = new Q1_MIP_S;
		memset (m_pQ1ExportHeader, 0, iHeaderSize);
	}

	unsigned char *pbyPackedData		= NULL;
	unsigned char *pbyBits[4];
	LPQ1_MIP_S pQ1Header				= NULL;
	CWallyPalette Palette;

	int iWidth[4];
	int iHeight[4];	
	int iSizes[4];
	int iOffsets[4];

	int j = 0;
	int iTotalSize = iHeaderSize;
	int r, g, b;
	
	if ((!IsValidImageSize ( GetImageWidth(), GetImageHeight(), FORCE_X_16)) && (!g_bAllowNon16))
	{
		SetErrorCode (IH_NOT_DIV16);
		return FALSE;
	}

	for (j = 0; j < 4; j++)
	{
		iWidth[j]	= max ( (int)(GetImageWidth()	/ pow (2, j)), 1);
		iHeight[j]	= max ( (int)(GetImageHeight()	/ pow (2, j)), 1);
		
		iSizes[j] = iWidth[j] * iHeight[j];
				
		iOffsets[j] = m_pQ1ExportHeader->offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
		iTotalSize += iSizes[j];
	}
	
	m_pQ1ExportHeader->width = iWidth[0];
	m_pQ1ExportHeader->height = iHeight[0];
	
	pbyPackedData = new BYTE[iTotalSize];
	
	if (!pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyPackedData, 0, iTotalSize);

	pQ1Header = (LPQ1_MIP_S)pbyPackedData;
	memcpy (pQ1Header, m_pQ1ExportHeader, iHeaderSize);

	for (j = 0; j < 4; j++)
	{
		pbyBits[j] = pbyPackedData + iOffsets[j];
	}
		
	memcpy (pbyBits[0], m_pbyDecodedData, iSizes[0]);

	COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
	if (!pTemp24Bit)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}
	Palette.SetPalette (quake1_pal, 256);

	for (j = 0; j < iSizes[0]; j++) 
	{
		r = m_byPalette[pbyBits[0][j] * 3 + 0];
		g = m_byPalette[pbyBits[0][j] * 3 + 1];
		b = m_byPalette[pbyBits[0][j] * 3 + 2];
		
		pTemp24Bit[j] = IRGB( 0, r, g, b);
	}
	Palette.Convert24BitTo256Color( pTemp24Bit, 
			pbyBits[0], iWidth[0], iHeight[0], 0, GetDitherType(), FALSE);
	
	if (pTemp24Bit)
	{
		delete pTemp24Bit;
		pTemp24Bit = NULL;
	}

	// All the sub-mips need to be built.
	BYTE	*pbyMipData		= NULL;
	pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);
	if (!pbyMipData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}	
	
	memset (pbyMipData, 0, MAX_DATA_SIZE);
	
	LPQ2_MIP_S pMipHeader = (LPQ2_MIP_S)pbyMipData;	
	iHeaderSize = Q2_HEADER_SIZE;
	
	// RebuildWAL expects a Q2-style header
	pMipHeader->width = pQ1Header->width;
	pMipHeader->height = pQ1Header->height;
	
	iOffsets[0] = iHeaderSize;
	iOffsets[1] = iHeaderSize + iSizes[0];
	iOffsets[2] = iHeaderSize + iSizes[0] + iSizes[1];
	iOffsets[3] = iHeaderSize + iSizes[0] + iSizes[1] + iSizes[2];

	for (j = 0; j < 4; j++)
	{
		pMipHeader->offsets[j] = iOffsets[j];
	}
	
	memcpy( pbyMipData + iOffsets[0], pbyBits[0], iSizes[0]);

	///////////////////////////////
	// Build all the sub-mips //
	/////////////////////////////

	Palette.SetNumFullBrights( NUM_FULL_BRIGHTS);
	Palette.RebuildWAL( pbyMipData);
		
	/////////////////////////
	// copy the data back //
	///////////////////////		
	for (j = 1; j < 4; j++)
	{
		memcpy( pbyBits[j], pbyMipData + iOffsets[j], iSizes[j]);		
	}	

	pMipHeader = NULL;
	free (pbyMipData);
	pbyMipData = NULL;

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iTotalSize);
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s(&fp, m_strFileName, "wb");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);
			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}
			return FALSE;
		}
		else
		{
			fwrite (pbyPackedData, iTotalSize, 1, fp);	
			fclose(fp);			
		}
	}

	if (pbyPackedData)
	{
		delete [] pbyPackedData;
		pbyPackedData = NULL;
	}
	
	return TRUE;
}


BOOL CImageHelper::EncodeRawData ()
{
	if (m_bUseArchive)
	{
		m_parFile->Write (m_byPalette, 768);
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s (&fp, m_strFileName, "wb");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);		
			return FALSE;
		}
		else
		{
			fwrite (m_byPalette, 768, 1, fp);	
			fclose(fp);	
		}
	}
	return TRUE;
}

BOOL CImageHelper::DecodeWAL( int iFlags /* = 0 */)
{
	SetColorDepth (IH_8BIT);
	
	LPQ2_MIP_S Q2MipHeader = (LPQ2_MIP_S) m_pbyEncodedData;

	int iHeaderSize  = Q2_HEADER_SIZE;
	int iFileLength = GetFileLength();
		
	if (iFileLength < iHeaderSize)
	{
		SetErrorCode (IH_WAL_MALFORMED);		
		return FALSE;
	}

	int iWidth = Q2MipHeader->width;
	int iHeight = Q2MipHeader->height;
	int iSize = iWidth * iHeight;
	int iTotalSize;
	
	iTotalSize = iHeaderSize + iSize;
	
	if (iTotalSize > iFileLength)
	{
		SetErrorCode (IH_WAL_MALFORMED);		
		return FALSE;
	}	

	SetImageHeight (iHeight);
	SetImageWidth (iWidth);	

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	m_pbyDecodedData = new BYTE[iSize];
	memset (m_pbyDecodedData, 0, iSize);

	int iOffset = Q2MipHeader->offsets[0];

	if ( (iOffset + iSize) > iFileLength)
	{
		// Try to recover as much as possible		
		if ( iOffset < iFileLength)
		{
			memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iFileLength - iOffset);
		}
	}
	else
	{
		memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iSize);
	}
	
	memcpy (m_byPalette, quake2_pal, 768);

	return TRUE;
}

BOOL CImageHelper::EncodeWAL ()
{
	int iHeaderSize = Q2_HEADER_SIZE;

	ASSERT (m_pQ2ExportHeader);		// You must initialize the header first!	
	ASSERT (m_pbyDecodedData);	

	if (!m_pQ2ExportHeader)
	{
		m_pQ2ExportHeader = new Q2_MIP_S;
		memset (m_pQ2ExportHeader, 0, iHeaderSize);
	}

	BYTE* pbyPackedData = NULL;
	BYTE* pbyBits[4];
	LPQ2_MIP_S pQ2Header = NULL;
	CWallyPalette Palette;

	int iWidth[4];
	int iHeight[4];	
	int iSizes[4];
	int iOffsets[4];

	int j = 0;
	int iTotalSize = iHeaderSize;
	int r, g, b;
	
	if ((!IsValidImageSize ( GetImageWidth(), GetImageHeight(), FORCE_X_16)) && (!g_bAllowNon16))
	{
		SetErrorCode (IH_NOT_DIV16);
		return FALSE;
	}

	for (j = 0; j < 4; j++)
	{
		iWidth[j]	= max ( (int)(GetImageWidth()	/ pow (2, j)), 1);
		iHeight[j]	= max ( (int)(GetImageHeight()	/ pow (2, j)), 1);
		
		iSizes[j] = iWidth[j] * iHeight[j];
				
		iOffsets[j] = m_pQ2ExportHeader->offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
		iTotalSize += iSizes[j];
	}

	m_pQ2ExportHeader->width = iWidth[0];
	m_pQ2ExportHeader->height = iHeight[0];

	switch (g_iTextureNameSetting)
	{
	case BLANK_DIR:			
		break;

	case PRESET_DIR :
		{
			CString strInternalName = g_strDefaultTextureName.Left(31);
			char *szName = strInternalName.GetBuffer(strInternalName.GetLength());
			strcpy_s (m_pQ2ExportHeader->name, sizeof(m_pQ2ExportHeader->name), szName);
		}
		break;

	case PARENT_DIR :			
		break;

	default:
		ASSERT (FALSE);		// Unhandled Wally option
		break;
	}
	
	pbyPackedData = new BYTE[iTotalSize];
	
	if (!pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyPackedData, 0, iTotalSize);

	pQ2Header = (LPQ2_MIP_S)pbyPackedData;
	memcpy (pQ2Header, m_pQ2ExportHeader, iHeaderSize);

	for (j = 0; j < 4; j++)
	{
		pbyBits[j] = pbyPackedData + iOffsets[j];
	}
		
	memcpy (pbyBits[0], m_pbyDecodedData, iSizes[0]);

	COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
	if (!pTemp24Bit)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}
	Palette.SetPalette (quake2_pal, 256);

	for (j = 0; j < iSizes[0]; j++) 
	{
		r = m_byPalette[pbyBits[0][j] * 3 + 0];
		g = m_byPalette[pbyBits[0][j] * 3 + 1];
		b = m_byPalette[pbyBits[0][j] * 3 + 2];
		
		if (((r != 159) || (g != 91) || (b != 83)))
			pTemp24Bit[j] = IRGB( 0, r, g, b);
		else
			pTemp24Bit[j] = IRGB( 255, r, g, b);
	}
	Palette.Convert24BitTo256Color( pTemp24Bit, 
			pbyBits[0], iWidth[0], iHeight[0], 0, GetDitherType(), FALSE);
	
	if (pTemp24Bit)
	{
		free( pTemp24Bit);
		pTemp24Bit = NULL;
	}


	// All the sub-mips need to be built.	
	BYTE	*pbyMipData		= NULL;
	pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);	
	if (!pbyMipData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}	
	
	memset (pbyMipData, 0, MAX_DATA_SIZE);
	
	q2_miptex_s *pMipHeader = (q2_miptex_s *)pbyMipData;	
	memcpy (pMipHeader, pQ2Header, iHeaderSize);
	memcpy( pbyMipData + iOffsets[0], pbyBits[0], iSizes[0]);

	
	///////////////////////////////
	// Build all the sub-mips //
	/////////////////////////////

	Palette.SetNumFullBrights( NUM_FULL_BRIGHTS);
	Palette.RebuildWAL( pbyMipData);
		
	/////////////////////////
	// copy the data back //
	///////////////////////
	
	for (j = 1; j < 4; j++)
	{
		memcpy( pbyBits[j], pbyMipData + iOffsets[j], iSizes[j]);		
	}	

	pMipHeader = NULL;
	free (pbyMipData);
	pbyMipData = NULL;

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iTotalSize);
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s (&fp, m_strFileName, "wb");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);
			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}
			return FALSE;
		}
		else
		{
			fwrite (pbyPackedData, iTotalSize, 1, fp);	
			fclose(fp);			
		}
	}

	if (pbyPackedData)
	{
		delete [] pbyPackedData;
		pbyPackedData = NULL;
	}
	
	return TRUE;
}

BOOL CImageHelper::DecodeSWL( int iFlags /* = 0 */)
{
	SetColorDepth (IH_8BIT);
	
	LPSIN_MIP_S SWLHeader = (LPSIN_MIP_S) m_pbyEncodedData;

	int iHeaderSize  = sizeof(SIN_MIP_S);
	int iFileLength = GetFileLength();
		
	if (iFileLength < iHeaderSize)
	{
		SetErrorCode (IH_SWL_MALFORMED);		
		return FALSE;
	}

	int iWidth = SWLHeader->width;
	int iHeight = SWLHeader->height;
	int iSize = iWidth * iHeight;
	int j;
		
	int iTotalSize = iHeaderSize + iSize;

	if (iTotalSize > iFileLength)
	{
		SetErrorCode (IH_SWL_MALFORMED);
		return FALSE;
	}	

	SetImageHeight (iHeight);
	SetImageWidth (iWidth);

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	m_pbyDecodedData = new BYTE[iSize];
	memset (m_pbyDecodedData, 0, iSize);
	
	int iOffset = SWLHeader->offsets[0];

	if ( (iOffset + iSize) > iFileLength)
	{
		// Try to recover as much as possible		
		if ( iOffset < iFileLength)
		{
			memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iFileLength - iOffset);
		}
	}
	else
	{
		memcpy (m_pbyDecodedData, m_pbyEncodedData + iOffset, iSize);
	}
	
	for (j = 0; j < 256; j++)
	{
		//memcpy ((m_byPalette + (j * 3)), (SWLHeader->palette + (j * 4)), 3);
		m_byPalette[ j * 3 + 0 ] = SWLHeader->palette[ (j * 4) + 0 ];
		m_byPalette[ j * 3 + 1 ] = SWLHeader->palette[ (j * 4) + 1 ];
		m_byPalette[ j * 3 + 2 ] = SWLHeader->palette[ (j * 4) + 2 ];
	}
	
	return TRUE;
}

BOOL CImageHelper::EncodeSWL ()
{	

	ASSERT (m_lpSinExportHeader);	// You must initialize the header first!	
	ASSERT (m_pbyDecodedData);	

	if (!m_lpSinExportHeader)
	{
		m_lpSinExportHeader = new SIN_MIP_S;
		memset (m_lpSinExportHeader, 0, SIN_HEADER_SIZE);
	}

	BYTE* pbyPackedData = NULL;
	BYTE* pbyBits[4];
	LPSIN_MIP_S lpSinHeader = NULL;
	CWallyPalette Palette;
	int iHeaderSize = SIN_HEADER_SIZE;

	int iWidth[4];
	int iHeight[4];	
	int iSizes[4];
	int iOffsets[4];

	int j = 0;
	int iTotalSize = iHeaderSize;
	
	if ((!IsValidImageSize ( GetImageWidth(), GetImageHeight(), FORCE_X_16)) && (!g_bAllowNon16))
	{
		SetErrorCode (IH_NOT_DIV16);
		return FALSE;
	}

	for (j = 0; j < 4; j++)
	{
		iWidth[j]	= max ( (int)(GetImageWidth()	/ pow (2, j)), 1);
		iHeight[j]	= max ( (int)(GetImageHeight()	/ pow (2, j)), 1);
		
		iSizes[j] = iWidth[j] * iHeight[j];
				
		iOffsets[j] = m_lpSinExportHeader->offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
		iTotalSize += iSizes[j];
	}
	
	m_lpSinExportHeader->width = iWidth[0];
	m_lpSinExportHeader->height = iHeight[0];
	

	pbyPackedData = new BYTE[iTotalSize];
	
	if (!pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyPackedData, 0, iTotalSize);

	lpSinHeader = (LPSIN_MIP_S)pbyPackedData;
	memcpy (lpSinHeader, m_lpSinExportHeader, SIN_HEADER_SIZE);

	for (j = 0; j < 4; j++)
	{
		pbyBits[j] = pbyPackedData + iOffsets[j];
	}
		
	memcpy (pbyBits[0], m_pbyDecodedData, iSizes[0]);

	// Copy over the palette

	for (j = 0; j < 256; j++)
	{
		//memcpy (lpSinHeader->palette + (j * 4), m_byPalette + (j * 3), 3);
		lpSinHeader->palette[ (j * 4) + 0 ] = m_byPalette[ (j * 3) + 0 ];
		lpSinHeader->palette[ (j * 4) + 1 ] = m_byPalette[ (j * 3) + 1 ];
		lpSinHeader->palette[ (j * 4) + 2 ] = m_byPalette[ (j * 3) + 2 ];
	}

	Palette.SetPalette (m_byPalette, 256);

	// Calculate the color to be used
	CalcImageColor256 (iWidth[0], iHeight[0], pbyBits[0], m_byPalette, &(lpSinHeader->color[0]), &(lpSinHeader->color[1]), &(lpSinHeader->color[2]), TRUE);

	// All the sub-mips need to be built.	
	BYTE	*pbyMipData		= NULL;
	pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);	
	if (!pbyMipData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}	
	
	memset (pbyMipData, 0, MAX_DATA_SIZE);
	
	LPQ2_MIP_S pMipHeader = (LPQ2_MIP_S)pbyMipData;
	int iQ2HeaderSize = Q2_HEADER_SIZE;

	pMipHeader->width = iWidth[0];
	pMipHeader->height = iHeight[0];
	
	pMipHeader->offsets[0] = iQ2HeaderSize;
	pMipHeader->offsets[1] = iQ2HeaderSize + iSizes[0];
	pMipHeader->offsets[2] = iQ2HeaderSize + iSizes[0] + iSizes[1];
	pMipHeader->offsets[3] = iQ2HeaderSize + iSizes[0] + iSizes[1] + iSizes[2];	

	memcpy( pbyMipData + pMipHeader->offsets[0], pbyBits[0], iSizes[0]);
	
	///////////////////////////////
	// Build all the sub-mips //
	/////////////////////////////

	Palette.SetNumFullBrights( 0);
	Palette.RebuildWAL( pbyMipData);
		
	/////////////////////////
	// copy the data back //
	///////////////////////
	
	for (j = 1; j < 4; j++)
	{
		memcpy( pbyBits[j], pbyMipData + pMipHeader->offsets[j], iSizes[j]);		
	}	

	pMipHeader = NULL;
	free (pbyMipData);
	pbyMipData = NULL;

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iTotalSize);
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s (&fp, m_strFileName, "wb");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);
			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}
			return FALSE;
		}
		else
		{
			fwrite (pbyPackedData, iTotalSize, 1, fp);	
			fclose(fp);
		}
	}
	
	if (pbyPackedData)
	{
		delete [] pbyPackedData;
		pbyPackedData = NULL;
	}	
	
	return TRUE;
}

BOOL CImageHelper::DecodeM8( int iFlags /* = 0 */ )
{
	SetColorDepth (IH_8BIT);
	
	LPM8_MIPTEX_S M8MipHeader = (LPM8_MIPTEX_S) m_pbyEncodedData;

	int iHeaderSize  = M8_HEADER_SIZE;
		
	if (GetFileLength() < iHeaderSize)
	{
		SetErrorCode (IH_M8_MALFORMED);		
		return FALSE;
	}

	int iWidth = M8MipHeader->width[0];
	int iHeight = M8MipHeader->height[0];
	int iSizes[M8_MIPLEVELS];
	int iTotalSize;
	int j;

	iTotalSize = iHeaderSize;
	
	for (j = 0; j < M8_MIPLEVELS; j++)
	{
		iSizes[j] = M8MipHeader->width[j] * M8MipHeader->height[j];
		iTotalSize += iSizes[j];
	}	

/*	if (iTotalSize > GetFileLength())
	{
		SetErrorCode (IH_M8_MALFORMED);		
		return FALSE;
	}
	*/

	SetImageHeight (iHeight);
	SetImageWidth (iWidth);

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	m_pbyDecodedData = new BYTE[iSizes[0]];
	memset (m_pbyDecodedData, 0, iSizes[0]);

	memcpy (m_pbyDecodedData, m_pbyEncodedData + M8MipHeader->offsets[0], iSizes[0]);
	memcpy (m_byPalette, M8MipHeader->palette, 768);

	return TRUE;
}

BOOL CImageHelper::EncodeM8 ()
{
	int iHeaderSize = M8_HEADER_SIZE;

	ASSERT (m_lpM8ExportHeader);		// You must initialize the header first!	
	ASSERT (m_pbyDecodedData);	
	
	if (!m_lpM8ExportHeader)
	{
		m_lpM8ExportHeader = new M8_MIPTEX_S;
		memset (m_lpM8ExportHeader, 0, iHeaderSize);
	}

	BYTE* pbyPackedData = NULL;
	BYTE* pbyBits[M8_MIPLEVELS];
	LPM8_MIPTEX_S pM8Header = NULL;
	CWallyPalette Palette;

	int iWidth = GetImageWidth();
	int iHeight = GetImageHeight();	

	if ((!IsValidImageSize ( iWidth, iHeight, FORCE_X_16)) && (!g_bAllowNon16))
	{
		SetErrorCode (IH_NOT_DIV16);
		return FALSE;
	}

	int iWidths[M8_MIPLEVELS];
	int iHeights[M8_MIPLEVELS];
	int iSizes[M8_MIPLEVELS];
	int iOffsets[M8_MIPLEVELS];
		
	int iTotalSize = iHeaderSize;
	int j = 0;
	
	switch (g_iTextureNameSetting)
	{
	case BLANK_DIR:			
		break;

	case PRESET_DIR :
		{
			CString strInternalName = g_strDefaultTextureName.Left(31);
			char *szName = strInternalName.GetBuffer(strInternalName.GetLength());
			strcpy_s (m_lpM8ExportHeader->name, sizeof(m_lpM8ExportHeader->name), szName);
		}
		break;

	case PARENT_DIR :			
		break;

	default:
		ASSERT (FALSE);		// Unhandled Wally option
		break;
	}

	// Force the first 4 to be at least 1x1
	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
		iSizes[j] = iWidths[j] * iHeights[j];
		
		m_lpM8ExportHeader->width[j] = iWidths[j];
		m_lpM8ExportHeader->height[j] = iHeights[j];

		iOffsets[j] = m_lpM8ExportHeader->offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
		iTotalSize += iSizes[j];
	}

	// Anythting past it *can* be 0x0
	for (j = 4; j < M8_MIPLEVELS; j++)
	{
		iWidths[j] = (int)(iWidth / pow (2, j));
		iHeights[j] = (int)(iHeight / pow (2, j));
		iSizes[j] = iWidths[j] * iHeights[j];
		
		m_lpM8ExportHeader->width[j] = iWidths[j];
		m_lpM8ExportHeader->height[j] = iHeights[j];

		iOffsets[j] = m_lpM8ExportHeader->offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
		iTotalSize += iSizes[j];
	}
	
	pbyPackedData = new BYTE[iTotalSize];
	
	if (!pbyPackedData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	memset (pbyPackedData, 0, iTotalSize);

	pM8Header = (LPM8_MIPTEX_S)pbyPackedData;
	memcpy (pM8Header, m_lpM8ExportHeader, iHeaderSize);

	for (j = 0; j < M8_MIPLEVELS; j++)
	{
		pbyBits[j] = (iOffsets[j] != 0) ? pbyPackedData + iOffsets[j] : 0;
	}
	
	memcpy (pbyBits[0], m_pbyDecodedData, iSizes[0]);
	memcpy (pM8Header->palette, m_byPalette, 768);

	Palette.SetPalette (m_byPalette, 256);
	
	// All the sub-mips need to be built.
	BYTE	*pbyMipData		= NULL;
	pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);	
	if (!pbyMipData)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}
		return FALSE;
	}	
	
	memset (pbyMipData, 0, MAX_DATA_SIZE);
	
	LPREMIP_HEADER pMipHeader = (LPREMIP_HEADER)pbyMipData;
	int iPosition = sizeof (REMIP_HEADER);
	
	for (j = 0; j < M8_MIPLEVELS; j++)
	{
		pMipHeader->iWidths[j] = iWidths[j];
		pMipHeader->iHeights[j] = iHeights[j];
		pMipHeader->iOffsets[j] = iPosition;

		iPosition += iSizes[j];
	}
			
	memcpy( pbyMipData + pMipHeader->iOffsets[0], pbyBits[0], iSizes[0]);
	
	///////////////////////////////
	// Build all the sub-mips //
	/////////////////////////////

	Palette.SetNumFullBrights( 0);
	::RebuildMips (&Palette, pbyMipData, M8_MIPLEVELS);
		
	/////////////////////////
	// copy the data back //
	///////////////////////
	
	for (j = 1; j < M8_MIPLEVELS; j++)
	{
		if (iSizes[j] != 0)
		{
			memcpy( pbyBits[j], pbyMipData + pMipHeader->iOffsets[j], iSizes[j]);			
		}
	}	

	pMipHeader = NULL;
	free (pbyMipData);
	pbyMipData = NULL;

	if (m_bUseArchive)
	{
		m_parFile->Write (pbyPackedData, iTotalSize);
	}
	else
	{
		FILE *fp = NULL;
		errno_t err = fopen_s (&fp, m_strFileName, "wb");

		if (err != 0)
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);
			if (pbyPackedData)
			{
				delete [] pbyPackedData;
				pbyPackedData = NULL;
			}
			return FALSE;
		}
		else
		{
			fwrite (pbyPackedData, iTotalSize, 1, fp);	
			fclose(fp);
		}
	}
	
	if (pbyPackedData)
	{
		delete [] pbyPackedData;
		pbyPackedData = NULL;
	}
	
	return TRUE;
}

void CImageHelper::SaveART(CFile *pFile)
{
	m_BuildList.Serialize (NULL, pFile, NULL, TRUE);
}

void CImageHelper::SaveART(LPCTSTR szFileName)
{
	m_BuildList.Serialize (szFileName, NULL, NULL, TRUE);
}

void CImageHelper::SaveWAD(CFile *pFile)
{	
	m_WADList.Serialize (NULL, pFile, NULL, TRUE);
}

void CImageHelper::SaveWAD(LPCTSTR szFileName)
{	
	m_WADList.Serialize (szFileName, NULL, NULL, TRUE);
}

CWADItem *CImageHelper::GetFirstImage()
{
	return (m_WADList.GetFirstImage());
}

CWADItem *CImageHelper::GetNextImage()
{
	return (m_WADList.GetNextImage());
}

CWADItem *CImageHelper::GetFirstLump()
{
	return (m_WADList.GetFirstLump());
}

CWADItem *CImageHelper::GetNextLump()
{
	return (m_WADList.GetNextLump());
}

CWADItem *CImageHelper::GetLumpAtPosition(int iPosition)
{
	return (m_WADList.GetLumpAtPosition(iPosition));
}

int CImageHelper::GetNumImages ()
{
	return (m_WADList.GetNumImages());
}

int CImageHelper::GetNumLumps ()
{
	return (m_WADList.GetNumLumps());
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Name:	AddImageToWAD
//	Action:	Inserts a MIP into the already existing WAD3 package.
//
//	Parameters:
//			pbyBits			Array of pointers to the mip data being inserted.  
//			pPalette		Pointer to the CWallyPalette 
//			szName			Name of the image
//			iWidth/iHeight	duh
//			bRemip			Set to TRUE to rebuild the sub-mips.  False leaves them as-is
//////////////////////////////////////////////////////////////////////////////////////////
CWADItem *CImageHelper::AddImageToWAD (unsigned char *pbyBits[4], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight)
{

	ASSERT (pbyBits);	
	ASSERT (szName);
	ASSERT (!m_WADList.IsNameInList (szName));
	ASSERT (m_WADList.GetWADType() != UNKNOWN_TYPE);

	if (m_WADList.GetWADType() == WAD3_TYPE)
	{
		ASSERT (pPalette);
	}	
	return m_WADList.AddMipItem (pbyBits, pPalette, szName, iWidth, iHeight);
}

void CImageHelper::AddImageToWAD( CWADItem *pItem )
{
	if( !pItem )
	{
		ASSERT( FALSE );
		return;
	}
	else
	{	
		m_WADList.AddToList( pItem );
	}
}

CWADItem *CImageHelper::IsNameInList (LPCTSTR szName)
{
	return m_WADList.IsNameInList (szName);
}


void CImageHelper::RemoveImageFromWAD (CWADItem *pItem)
{
	ASSERT (pItem);
	m_WADList.RemoveImage (pItem);	
}

BOOL CImageHelper::DetermineWADType()
{
	char szWadType[5];
	memset (szWadType, 0, 5);	

	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, m_strFileName, "rb");
	if (err != 0)
	{
		SetErrorCode(IH_FILE_NOT_FOUND);
		return FALSE;
	}
		
	if (fread (szWadType, 1, 4, fp) != 4)
	{
		fclose (fp);
		SetErrorCode (IH_WAD_MALFORMED);
		return FALSE;
	}

	fclose (fp);	

	if (!strcmp(szWadType, "WAD2"))
	{
		SetImageType (IH_WAD2_TYPE);
		return TRUE;
	}

	if (!strcmp(szWadType, "WAD3"))
	{
		SetImageType (IH_WAD3_TYPE);
		return TRUE;
	}
	return FALSE;	
}

BOOL CImageHelper::IsValidImage (int iType /* = -1 */)
{
	if (iType == -1)
	{
		iType = GetImageType();
	}
	return ((iType > IMAGE_TYPE_START) && (iType <= IMAGE_TYPE_END));
}

BOOL CImageHelper::IsValidPackage (int iType /* = -1 */)
{
	if (iType == -1)
	{
		iType = GetImageType();
	}

	return (((iType > PACKAGE_TYPE_START) && (iType <= PACKAGE_TYPE_END)) || (iType == IH_ART_TYPE));
}

BOOL CImageHelper::IsValidPak (int iType /* = -1 */)
{
	if (iType == -1)
	{
		iType = GetImageType();
	}

	return ((iType > PAK_TYPE_START) && (iType <= PAK_TYPE_END));
}

BOOL CImageHelper::IsValidPalette (int iType /* = -1 */)
{
	if (iType == -1)
	{
		iType = GetImageType();
	}
	return ((iType > PALETTE_TYPE_START) && (iType <= PALETTE_TYPE_END));
}

BOOL CImageHelper::IsGameType (int iType /* = -1 */)
{
	int iCheckType = iType;

	if (iCheckType == -1)
	{
		iCheckType = GetImageType();
	}
	
	switch (iCheckType)
	{
	case IH_PCX_TYPE:
	case IH_BMP_TYPE:
	case IH_TGA_TYPE:
	case IH_PNG_TYPE:
		return FALSE;
		break;
	
	case IH_SWL_TYPE:
	case IH_MIP_TYPE:
	case IH_WAL_TYPE:
	case IH_WAD2_TYPE:
	case IH_WAD3_TYPE:
	case IH_ART_TYPE:
	case IH_M8_TYPE:
	case IH_PAK_TYPE:
		return TRUE;
		break;

	default:
		return FALSE;
		break;
	}

	return FALSE;
}

BOOL CImageHelper::IsNonGameType (LPCTSTR szFileName)
{
	if (SetFileName (szFileName))
	{
		switch (GetImageType())
		{		
		case IH_PCX_TYPE:
		case IH_BMP_TYPE:
		case IH_TGA_TYPE:
		case IH_SWL_TYPE:
		case IH_MIP_TYPE:
		case IH_WAL_TYPE:
		case IH_WAD2_TYPE:
		case IH_WAD3_TYPE:
		case IH_ART_TYPE:
		case IH_M8_TYPE:
		case IH_PAK_TYPE:
			return FALSE;
			break;

		default:
			return TRUE;
			break;
		}
	}
	
	return TRUE;
}

int CImageHelper::GetTypeFlags (LPCTSTR szExtension)
{
	CString strExtension (szExtension);
	strExtension.MakeLower();

	if (strExtension == ".pcx")
	{
		SetImageType(IH_PCX_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".bmp")
	{
		SetImageType (IH_BMP_TYPE);
		return GetTypeFlags();
	}	

	if (strExtension == ".mip")
	{
		SetImageType (IH_MIP_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".wal")
	{
		SetImageType (IH_WAL_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".tga")
	{
		SetImageType (IH_TGA_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".swl")
	{
		SetImageType (IH_SWL_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".m8")
	{
		SetImageType (IH_M8_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".png")
	{
		SetImageType (IH_PNG_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".jpg")
	{
		SetImageType (IH_JPG_TYPE);
		return GetTypeFlags();
	}

	if (strExtension == ".tex")
	{
		SetImageType (IH_TEX_TYPE);
		return GetTypeFlags();
	}

	ASSERT (FALSE);		// Can't be requesting flags for an unsupported type!
	return 0;
}

int CImageHelper::GetTypeFlags (int iType /* = -1 */)
{
	// The following flags are used to describe an image type:

	// #define IH_TYPE_FLAG_SUPPORTS8BIT	0x1
	// #define IH_TYPE_FLAG_SUPPORTS24BIT	0x2
	// #define IH_TYPE_FLAG_ISGAME			0x4

	int iFlags = 0;
	if (iType == -1)
	{
		iType = GetImageType();
	}

	switch (iType)
	{
	case IH_PCX_TYPE:
	case IH_BMP_TYPE:
	case IH_TGA_TYPE:
	case IH_PNG_TYPE:
		iFlags = IH_TYPE_FLAG_SUPPORTS8BIT | IH_TYPE_FLAG_SUPPORTS24BIT;
		break;

	case IH_JPG_TYPE:
	case IH_TEX_TYPE:
		iFlags = IH_TYPE_FLAG_SUPPORTS24BIT;
		break;
	
	case IH_M8_TYPE:
	case IH_SWL_TYPE:
	case IH_MIP_TYPE:
	case IH_WAL_TYPE:	
		iFlags = IH_TYPE_FLAG_SUPPORTS8BIT | IH_TYPE_FLAG_ISGAME;
		break;

	default:
		ASSERT (FALSE);			// Can't select an item that isn't available for saving
		break;
	}

	return iFlags;
}

void CImageHelper::MapGameFlags (int iSourceType, unsigned char *pbyData, LPCTSTR szFileName /* = NULL */)
{	
	int iQ2HeaderSize = Q2_HEADER_SIZE;
	int iQ1HeaderSize = Q1_HEADER_SIZE;
	
	if (m_pQ2ExportHeader)
	{
		delete m_pQ2ExportHeader;
		m_pQ2ExportHeader = NULL;
	}

	if (m_pQ1ExportHeader)
	{
		delete m_pQ1ExportHeader;
		m_pQ1ExportHeader = NULL;
	}

	if (m_lpSinExportHeader)
	{
		delete m_lpSinExportHeader;
		m_lpSinExportHeader = NULL;
	}

	if (m_lpM8ExportHeader)
	{
		delete m_lpSinExportHeader;
		m_lpSinExportHeader = NULL;
	}

	m_pQ2ExportHeader = new Q2_MIP_S;
	memset (m_pQ2ExportHeader, 0, iQ2HeaderSize);

	m_pQ1ExportHeader = new Q1_MIP_S;
	memset (m_pQ1ExportHeader, 0, iQ1HeaderSize);

	m_lpSinExportHeader = new SIN_MIP_S;
	memset (m_lpSinExportHeader, 0, SIN_HEADER_SIZE);

	m_lpM8ExportHeader = new M8_MIPTEX_S;
	memset (m_lpM8ExportHeader, 0, M8_HEADER_SIZE);

	m_lpSinExportHeader->nonlit = (float)0.5;
	m_lpSinExportHeader->friction = (float)1.0;
	m_lpSinExportHeader->animtime = (float)0.2;
	m_lpSinExportHeader->color[0] = (float)1.0;
	m_lpSinExportHeader->color[1] = (float)1.0;
	m_lpSinExportHeader->color[2] = (float)1.0;
		
	int iLength = 0;
	CString strInternalName ("");

	switch (g_iTextureNameSetting)
	{
	case PRESET_DIR :
		{
			strInternalName = TrimSlashes (g_strDefaultTextureName);
			strInternalName += "/";
		}
		break;

	case PARENT_DIR :
		{
			if (szFileName)
			{
				strInternalName = TrimSlashes (GetParentDirectory (szFileName));
				strInternalName += "/";
			}
		}
		break;
	
	default:		
		break;
	}

	if (szFileName)
	{
		strInternalName += GetRawFileName (szFileName);
	}
	
	switch (iSourceType)
	{
	case IH_PCX_TYPE:
	case IH_BMP_TYPE:
	case IH_TGA_TYPE:
	case IH_PNG_TYPE:
	case IH_JPG_TYPE:
	case IH_TEX_TYPE:
		{			
			iLength = strInternalName.GetLength();
			char *szName = strInternalName.GetBuffer (iLength);

			memcpy (m_pQ1ExportHeader->name, szName, min (iLength, 15));
			memcpy (m_pQ2ExportHeader->name, szName, min (iLength, 31));
			memcpy (m_lpSinExportHeader->name, szName, min (iLength, 63));
			memcpy (m_lpM8ExportHeader->name, szName, min (iLength, 31));			
		}
		break;

	case IH_M8_TYPE:
		{
		}
		break;

	case IH_MIP_TYPE:
		{
			LPQ1_MIP_S pQ1ExportHeader = (LPQ1_MIP_S)pbyData;
			memcpy (m_pQ1ExportHeader, pQ1ExportHeader, iQ1HeaderSize);
			
			iLength = strlen(pQ1ExportHeader->name);			
			memcpy (m_pQ2ExportHeader->name, pQ1ExportHeader->name, min (iLength, 15));
			memcpy (m_lpSinExportHeader->name, pQ1ExportHeader->name, min (iLength, 15));
			memcpy (m_lpM8ExportHeader->name, pQ1ExportHeader->name, min (iLength, 15));
		}
		break;

	case IH_WAL_TYPE:
		{			
			LPQ2_MIP_S pQ2ExportHeader = (LPQ2_MIP_S)pbyData;
			memcpy (m_pQ2ExportHeader, pQ2ExportHeader, iQ2HeaderSize);

			iLength = strlen(pQ2ExportHeader->name);			
			memcpy (m_pQ1ExportHeader->name, pQ2ExportHeader->name, min (iLength, 15));
			memcpy (m_lpSinExportHeader->name, pQ2ExportHeader->name, min (iLength, 31));
			memcpy (m_lpM8ExportHeader->name, pQ2ExportHeader->name, min (iLength, 31));

			// TODO:  Find out whether Heretic2 flags and contents are the same?
			m_lpM8ExportHeader->flags = pQ2ExportHeader->flags;
			m_lpM8ExportHeader->contents = pQ2ExportHeader->contents;

			m_lpSinExportHeader->flags = pQ2ExportHeader->flags;

			// Remove the flags that SiN doesn't have
			m_lpSinExportHeader->flags ^= TF_SLICK;
			m_lpSinExportHeader->flags ^= TF_TRANS33;
			m_lpSinExportHeader->flags ^= TF_TRANS66;
			
			if (pQ2ExportHeader->flags & TF_TRANS33)
			{
				m_lpSinExportHeader->translucence = (float)(1.0/3.0);
			}

			if (pQ2ExportHeader->flags & TF_TRANS66)
			{
				m_lpSinExportHeader->translucence = (float)(2.0/3.0);
			}

			if (pQ2ExportHeader->flags & TF_SLICK)
			{
				m_lpSinExportHeader->friction = 2.5;
			}			
			
			m_lpSinExportHeader->contents = pQ2ExportHeader->contents;

			// Remove the contents that SiN doesn't have
			m_lpSinExportHeader->contents ^= TC_AUX;
		}
		break;

	case IH_SWL_TYPE:
		{
			LPSIN_MIP_S lpSinHeader = (LPSIN_MIP_S)pbyData;
			memcpy (m_lpSinExportHeader, lpSinHeader, SIN_HEADER_SIZE);

			iLength = strlen(m_lpSinExportHeader->name);			
			memcpy (m_pQ1ExportHeader->name, m_lpSinExportHeader->name, min (iLength, 15));
			memcpy (m_pQ2ExportHeader->name, m_lpSinExportHeader->name, min (iLength, 31));

			m_pQ2ExportHeader->flags = lpSinHeader->flags;

			// Remove the flags that Q2 doesn't have
			m_pQ2ExportHeader->flags ^= 0x7fdffc00;
			m_pQ2ExportHeader->flags ^= SIN_TF_MASKED;
			m_pQ2ExportHeader->flags ^= SIN_TF_NONLIT;
			m_pQ2ExportHeader->flags ^= SIN_TF_NOFILTER;

			m_lpM8ExportHeader->flags = lpSinHeader->flags;

			// Remove the flags that Heretic2 doesn't have
			m_lpM8ExportHeader->flags ^= 0x7fdffc00;
			m_lpM8ExportHeader->flags ^= SIN_TF_MASKED;
			m_lpM8ExportHeader->flags ^= SIN_TF_NONLIT;
			m_lpM8ExportHeader->flags ^= SIN_TF_NOFILTER;			

			if (lpSinHeader->friction > 1.0)
			{
				m_pQ2ExportHeader->flags |= TF_SLICK;
				m_lpM8ExportHeader->flags |= TF_SLICK;
			}
			
			if (lpSinHeader->translucence != 0.0)
			{
				if (lpSinHeader->translucence < 0.50)
				{
					m_pQ2ExportHeader->flags |= TF_TRANS33;
					m_lpM8ExportHeader->flags |= TF_TRANS33;
				}
				else
				{
					m_pQ2ExportHeader->flags |= TF_TRANS66;
					m_lpM8ExportHeader->flags |= TF_TRANS66;
				}				
			}
			
			m_pQ2ExportHeader->contents = lpSinHeader->contents;
			m_lpM8ExportHeader->contents = lpSinHeader->contents;

			// Remove the contents that Q2 doesn't have
			m_pQ2ExportHeader->contents ^= SIN_TC_FENCE;
			m_lpM8ExportHeader->contents ^= SIN_TC_FENCE;
		}
		break;

	case IH_WAD3_IMAGE_TYPE:
		{
			char *szName = (char *)pbyData;
			char skyFlag[4];
			char cFlag = szName[0];
			iLength = strlen(szName);
			
			memcpy (m_pQ2ExportHeader->name, szName, min (iLength, 31));
			memcpy (m_pQ1ExportHeader->name, szName, min (iLength, 15));
			memcpy (m_lpSinExportHeader->name, szName, min (iLength, 63));
			memcpy (m_lpM8ExportHeader->name, szName, min (iLength, 31));

			memset (skyFlag, 0, 4);

			for (int j = 0; (j < 4) && (j < iLength); j++)
			{
				skyFlag[j] = tolower (szName[j]);
			}			

			if (!strcmp(skyFlag, "sky"))
			{
				m_pQ2ExportHeader->flags |= TF_SKY;
				m_lpSinExportHeader->flags |= SIN_TF_SKY;
				m_lpM8ExportHeader->flags |= TF_SKY;
			}

			switch (cFlag)
			{
				// Transparency (character {) doesn't have an equivalent in Quake2... 
				// transparency in HL really means masked.  There is only translucency
				// in Q2.
			case '!':
				m_pQ2ExportHeader->flags |= TF_WARP;
				m_pQ2ExportHeader->contents |= TC_WATER;

				m_lpM8ExportHeader->flags |= TF_WARP;
				m_lpM8ExportHeader->contents |= TC_WATER;
				
				m_lpSinExportHeader->flags |= SIN_TF_WARP;
				m_lpSinExportHeader->contents |= SIN_TC_WATER;
				break;

			case '+':
				{	
					int iAnimLength = iLength + 1;
					char *szAnimName;
					szAnimName = new char[iAnimLength];
					memset (szAnimName, 0, iAnimLength);
					memcpy (szAnimName, szName, iLength);
					szAnimName[1] = szAnimName[1] + 1;
					memcpy (m_pQ2ExportHeader->animname, szAnimName, min (iLength, 31));
					memcpy (m_lpSinExportHeader->animname, szAnimName, min (iLength, 63));
					memcpy (m_lpM8ExportHeader->animname, szAnimName, min (iLength, 31));
					
					if (szAnimName)
					{
						delete [] szAnimName;
						szAnimName = NULL;
					}
				}
				break;
				
			default:
				break;
			}
		}
		break;

	default:
		ASSERT (FALSE);		// Unhandled source game type!
		break;
	}
}

void CImageHelper::CreateQ2Header()
{
	int iSize = Q2_HEADER_SIZE;
	if (!m_pQ2ExportHeader)
	{		
		m_pQ2ExportHeader = new Q2_MIP_S;
	}	
	memset (m_pQ2ExportHeader, 0, iSize);
}

void CImageHelper::CreateQ1Header()
{
	int iSize = Q1_HEADER_SIZE;
	if (!m_pQ1ExportHeader)
	{		
		m_pQ1ExportHeader = new Q1_MIP_S;
	}	
	memset (m_pQ1ExportHeader, 0, iSize);
}


BOOL CImageHelper::DecodeGIF( int iFlags /* = 0 */)
{
	// This is just the starter code... the LZW decompression stuff needs
	// to be done
	ASSERT (FALSE);

	LPGIF_HEADER lpHeader = (LPGIF_HEADER)m_pbyEncodedData;

	if (
		(lpHeader->szID[0] != 'G') &&
		(lpHeader->szID[1] != 'I') &&
		(lpHeader->szID[2] != 'F')
		)
	{
		SetErrorCode (IH_GIF_MALFORMED);
		return FALSE;
	}

	int iGIFVersion = 0;

	switch (lpHeader->szVersion[1])
	{
	case '7':	
		iGIFVersion = GIF_87A;
		break;

	case '9':
		iGIFVersion = GIF_89A;
		break;

	default:
		SetErrorCode (IH_GIF_UNSUPPORTED_VERSION);
		return FALSE;
		break;
	}

	int iPosition = GIF_HEADER_SIZE;
	LPGIF_LSD lpLSD = (LPGIF_LSD)(m_pbyEncodedData + iPosition);
	int iWidth = 0;
	int iHeight = 0;
	BYTE byFlags = 0;

	byFlags = lpLSD->byFlags;
	
	BOOL bColorTable = (byFlags & GM_LSD_COLOR_TABLE);
	int iNumBits = ((byFlags & GM_LSD_COLOR_DEPTH) >> 1) + 1;
	BOOL bSorted = (byFlags & GM_LSD_SORT);
	int iGlobalColorTableSize =  3 * (int)pow (2, (((byFlags & GM_LSD_CT_SIZE) >> 5) + 1));

	BYTE *pbyGlobalPalette = NULL;
	iPosition += GIF_LSD_SIZE;

	if (bColorTable)
	{
		pbyGlobalPalette = (m_pbyEncodedData + iPosition);
		iPosition += iGlobalColorTableSize;
	}
	
	BYTE *pbyCheckType = m_pbyEncodedData + iPosition;
	BYTE *pbyCheckLabel = NULL;
	BYTE *pbyLocalPalette = NULL;
	BYTE *pbyCompressedData = NULL;
	
	LPGIF_ID lpID = NULL;

	BOOL bDone = FALSE;

	while (!bDone)
	{
		switch ((*pbyCheckType))
		{
		case 0x2c:		// Image
			{
				lpID = (LPGIF_ID)(m_pbyEncodedData + iPosition);

				iWidth = lpID->uiWidth;
				iHeight = lpID->uiHeight;			
				
				SetImageWidth (iWidth);
				SetImageHeight (iHeight);

				byFlags = lpID->byFlags;
				
				BOOL bLocalTable = (byFlags & GM_ID_LOCAL_TABLE);
				BOOL bInterlaced = (byFlags & GM_ID_INTERLACE) >> 1;
				BOOL bLocalSort = (byFlags & GM_ID_SORT) >> 2;
				int iLocalColorTableSize = 3 * (int)pow (2, (((byFlags & GM_ID_CT_SIZE) >> 5) + 1));

				iPosition += GIF_ID_SIZE;

				if (bLocalTable)
				{					
					pbyLocalPalette = (m_pbyEncodedData + iPosition);
					iPosition += iGlobalColorTableSize;
				}

				pbyCompressedData = (m_pbyEncodedData + iPosition);
				BYTE byMinCodeSize = (*pbyCompressedData);

				// TODO:  Decompress routines?

			}
			break;		// case 0x2c

		case 0x21:		// Extension
			{
				pbyCheckLabel = pbyCheckType + 1;

				switch ((*pbyCheckLabel))
				{
				case 0x01:		// Text Extension
					{
					}
					break;

				case 0xf9:		// Graphic Control Extension
					{
					}
					break;

				case 0xfe:		// Comment Extension
					{
					}
					break;

				case 0xff:		// Application Extension
					{
					}
					break;

				default:
					{
						SetErrorCode (IH_GIF_MALFORMED);
						return FALSE;
					}
					break;
				}			
			}
			break;		// case 0x21

		case 0x3b:		// Terminator
			{
				bDone = TRUE;
			}
			break;

		default:		
			{
				SetErrorCode (IH_GIF_MALFORMED);
				return FALSE;
			}
			break;
		}
	}
	
	return FALSE;
}

BOOL CImageHelper::EncodeGIF ()
{
	ASSERT (FALSE);
	return FALSE;
}

CBuildItem *CImageHelper::GetBuildTile (int iTileNumber)
{
	return m_BuildList.GetTile (iTileNumber);
}

int CImageHelper::GetFirstBuildTile ()
{
	return m_BuildList.GetFirstTileNumber();
}

int CImageHelper::GetLastBuildTile ()
{
	return m_BuildList.GetLastTileNumber();
}

int CImageHelper::GetNumColors()
{
	int iColors = 0;

	switch (GetColorDepth())
	{
	case IH_1BIT:
		iColors = 2;
		break;

	case IH_2BIT:
		iColors = 4;
		break;

	case IH_4BIT:
		iColors = 16;
		break;

	case IH_8BIT:
		iColors = 256;
		break;

	case IH_16BIT:
		iColors = 65536;
		break;

	case IH_24BIT:
	case IH_32BIT:			// just adds an alpha channel
		iColors = 16777216;
		break;

	default:
		ASSERT( FALSE);
	}

	return (iColors);
}

int CImageHelper::GetNumPlanes()
{
	int iNumPlanes = 1;

	switch (GetColorDepth())
	{
	case IH_1BIT:
	case IH_2BIT:
	case IH_8BIT:
		iNumPlanes = 1;
		break;

	case IH_16BIT:
		iNumPlanes = 2;
		break;

	case IH_24BIT:
		iNumPlanes = 3;
		break;

	case IH_4BIT:
	case IH_32BIT:
		iNumPlanes = 4;
		break;

	default:
		ASSERT( FALSE);
	}

	return (iNumPlanes);
}

int CImageHelper::GetNumBitsPerPixelPlane()
{
	int iNumBitsPerPixelPlane = 8;

	switch (GetColorDepth())
	{
	case IH_1BIT:
		iNumBitsPerPixelPlane = 1;
		break;

	case IH_2BIT:
		iNumBitsPerPixelPlane = 2;
		break;

	case IH_4BIT:
		iNumBitsPerPixelPlane = 1;
		break;

	case IH_8BIT:
	case IH_16BIT:
	case IH_24BIT:
	case IH_32BIT:
		iNumBitsPerPixelPlane = 8;
		break;

	default:
		ASSERT( FALSE);
	}

	return (iNumBitsPerPixelPlane);
}