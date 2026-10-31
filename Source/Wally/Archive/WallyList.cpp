/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WallyList.cpp : implementation of the CWallyItem and CWallyList classes
//
//  Created by Ty Matthews, 3-28-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "Wally.h"
#include "WallyList.h"
#include "WallyType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//  CWallyItem implementation
/////////////////////////////////////////////////////////////////////////////

CWallyItem::CWallyItem(int iItemType, int iItemSize, int iItemFlags, BYTE *pbyData)
{	
	ASSERT (pbyData);
	ASSERT (iItemSize > 0);

	SetItemSize (iItemSize);
	
	SetItemCategory (HIWORD (iItemType));
	SetItemType (LOWORD (iItemType));	
	SetItemFlags (iItemFlags);

	SetNext (NULL);
	SetPrevious (NULL);

	m_pbyData = new BYTE[iItemSize];
	memset (m_pbyData, 0, iItemSize);

	memcpy (m_pbyData, pbyData, iItemSize);
}

CWallyItem::~CWallyItem()
{
	PurgeData();
}

void CWallyItem::PurgeData()
{
	if (m_pbyData)
	{
		delete [] m_pbyData;
		m_pbyData = NULL;
	}
}


BYTE *CWallyItem::GetItemData()
{
	return m_pbyData;
}


int CWallyItem::GetItemSize()
{
	return m_iItemSize;
}

void CWallyItem::SetItemSize(int iSize)
{
	m_iItemSize = iSize;
}


int CWallyItem::GetItemCategory()
{
	return m_iItemCategory;
}

void CWallyItem::SetItemCategory(int iCategory)
{
	m_iItemCategory = iCategory;
}


int CWallyItem::GetItemType()
{
	return m_iItemType;
}

void CWallyItem::SetItemType(int iType)
{
	m_iItemType = iType;
}


int CWallyItem::GetItemFlags()
{
	return m_iItemFlags;
}

void CWallyItem::SetItemFlags(int iFlags)
{
	m_iItemFlags = iFlags;
}


CWallyItem *CWallyItem::GetNext()
{
	return m_pNext;
}

void CWallyItem::SetNext(CWallyItem *pNext)
{	
	m_pNext = pNext;
}


CWallyItem *CWallyItem::GetPrevious()
{
	return m_pPrevious;
}

void CWallyItem::SetPrevious(CWallyItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

/////////////////////////////////////////////////////////////////////////////
//  CWallyList implementation
/////////////////////////////////////////////////////////////////////////////


CWallyList::CWallyList()
{
	SetAll (NULL);
}

CWallyList::~CWallyList()
{
	PurgeList();
}

void CWallyList::SetAll (CWallyItem *pItem)
{
	SetFirst (pItem);
	SetRead (pItem);
	SetWrite (pItem);	
}

void CWallyList::SetFirst (CWallyItem *pFirst)
{
	m_pFirst = pFirst;
}

CWallyItem *CWallyList::GetFirst ()
{
	return m_pFirst;
}

void CWallyList::SetRead (CWallyItem *pRead)
{
	m_pRead = pRead;
}

CWallyItem *CWallyList::GetRead ()
{
	return m_pRead;
}

void CWallyList::SetWrite (CWallyItem *pWrite)
{
	m_pWrite = pWrite;
}

CWallyItem *CWallyList::GetWrite ()
{
	return m_pWrite;
}

void CWallyList::PurgeList()
{
	CWallyItem *pItem = GetFirst();

	while (pItem)
	{
		m_pRead = pItem->GetNext();
		delete pItem;
		pItem = m_pRead;
	}

	pItem = NULL;
	SetAll (NULL);
}

void CWallyList::AddItem (int iItemType, int iItemSize, int iItemFlags, BYTE *pbyData)
{
	CWallyItem *pNewItem = new CWallyItem (iItemType, iItemSize, iItemFlags, pbyData);

	if (!GetFirst())
	{
		SetAll (pNewItem);
	}
	else
	{
		GetWrite()->SetNext (pNewItem);
		pNewItem->SetPrevious (GetWrite());
		SetWrite (pNewItem);
	}

	pNewItem = NULL;
}

int CWallyList::Serialize (LPCTSTR szFileName, CFile *pFile, BYTE *pbyData, bool bSave /* = false */)
{
	BYTE *pbyTempBuffer		= NULL;
	BYTE *pbyFileData		= NULL;
	int iFileLength = 0;

	if (!bSave)		// We're loading
	{
		if (szFileName)
		{
			ASSERT (!pbyData);
			ASSERT (!pFile);

			CFile fp;
			CFileException e;

			if (!fp.Open(szFileName, CFile::modeRead, &e ))
			{
				return IH_ERROR_READING_FILE;
			}
			
			iFileLength = fp.GetLength();
			pbyTempBuffer = new unsigned char[iFileLength];
			fp.Read (pbyTempBuffer, iFileLength);
			fp.Close();

			pbyFileData = pbyTempBuffer;
		}

		if (pFile)
		{
			ASSERT (!pbyData);
			ASSERT (!szFileName);

			iFileLength = pFile->GetLength();
			pbyTempBuffer = new unsigned char[iFileLength];
			pFile->Read (pbyTempBuffer, iFileLength);

			pbyFileData = pbyTempBuffer;
		}
		
		if (pbyData)
		{
			ASSERT (!szFileName);
			ASSERT (!pFile);
			pbyFileData = pbyData;
		}
		
		struct WallyHeader *pWallyHeader = (struct WallyHeader *)pbyFileData;
				
		if (!strcmp(pWallyHeader->Identification, "WALLY"))
		{
			if (pbyTempBuffer)
			{
				delete [] pbyTempBuffer;
				pbyTempBuffer = NULL;
			}
			return IH_WLY_MALFORMED;
		}


		switch (pWallyHeader->Version)
		{
		case 1:
			{				
				int iNumTypeEntries			= pWallyHeader->NumTypeEntries;
				int iNumDirectoryEntries	= pWallyHeader->NumDirectoryEntries;
				int iDirectoryOffset		= pWallyHeader->DirectoryOffset;
			
				BYTE *pbyItemData = NULL;				
				int iItemType = 0;
				int iItemSize = 0;				
				int iItemFlags	= 0;
				int iItemOffset	= 0;
				int j = 0;

				struct WallyDirectoryEntry *pDirectoryEntry = NULL;
				int iDirectoryEntrySize = sizeof (struct WallyDirectoryEntry);

				for (j = 0; j < iNumDirectoryEntries; j++)
				{
					pDirectoryEntry = (struct WallyDirectoryEntry *)(pbyFileData + iDirectoryOffset + (j * iDirectoryEntrySize));
					
					iItemType = pDirectoryEntry->ItemType;
					iItemSize = pDirectoryEntry->ItemSize;
					iItemFlags = pDirectoryEntry->ItemFlags;
					iItemOffset = pDirectoryEntry->ItemOffset;

					if ((iItemOffset + iItemSize) > iFileLength)
					{
						if (pbyTempBuffer)
						{
							delete [] pbyTempBuffer;
							pbyTempBuffer = NULL;
						}
						return IH_WLY_MALFORMED;
					}

					pbyItemData = pbyFileData + iItemOffset;					

					AddItem (iItemType, iItemSize, iItemFlags, pbyItemData);					
				}

			}
			break;

		default:
			{
				if (!pbyData)
				{
					if (pbyFileData)
					{
						delete [] pbyFileData;
						pbyFileData = NULL;
					}
				}
				return IH_WLY_UNSUPPORTED_VERSION;
			}
			break;
		}
		

					
		if (!pbyData)
		{
			if (pbyFileData)
			{
				delete [] pbyFileData;
				pbyFileData = NULL;
			}
		}
	}

	else		// We're saving
	{
		CFile *pSaveFile = NULL;

		if (pFile)
		{
			ASSERT (!szFileName);
			pSaveFile = pFile;
		}

		if (szFileName)
		{
			ASSERT (!pFile);
			pSaveFile = new CFile();
			CFileException e;

			if (!pSaveFile->Open(szFileName, CFile::modeCreate | CFile::modeWrite, &e ))
			{
				return IH_ERROR_WRITING_FILE;
			}			
		}
		
/*		if (IsEmpty())
		{	
			ASSERT (false);		// Can't save an empty list	
			return IH_WAD_NO_IMAGES;			
		}*/

		if (szFileName)
		{
			if (pSaveFile)
			{
				delete pSaveFile;
				pSaveFile = NULL;
			}
		}
	}
	return IH_SUCCESS;
}