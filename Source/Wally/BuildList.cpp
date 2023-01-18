/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WADList.cpp : implementation of the CBuildItem and CBuildList classes
//
//  Created by Ty Matthews, 12-17-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "BuildList.h"
#include "Wally.h"
#include "WallyPal.h"
//#include "ProgressBar.h"
#include "DibSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CBuildItem implementation
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Name:		CBuildItem()
//	Action:		Default constructor for class.  Only to be used with a separate
//				call to CreateAsMip()
/////////////////////////////////////////////////////////////////////////////
CBuildItem::CBuildItem ()
{
	m_pbyData		= NULL;	
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);	
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CBuildItem (BYTE *pbyData, int iDataSize, char cType, LPCTSTR szName)
//	Action:		Constructor for class.  Initializes data pointers and builds
//				the lump item properly.  Caller must pass in valid data.
//
//	Parameters:
//
//	BYTE *pbyData		: pointer to raw lump data.  No formatting should be done prior to this
//	int iDatasize				: total lump size, as defined by the corresponding lump info
//	char cType					: type member from struct wad3_lumpinfo_s
//	LPCTSTR szName				: pointer to name of lump item
/////////////////////////////////////////////////////////////////////////////
CBuildItem::CBuildItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber)
{	
	m_pbyData		= NULL;	
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);

	ASSERT (pbyData);
	
	SetWidth (iWidth);
	SetHeight (iHeight);
	m_iTotalSize = iWidth * iHeight;

	m_iFlags = iFlags;

	if ((iWidth > 0) && (iHeight > 0))
	{
		m_pbyData = new BYTE[m_iTotalSize];
		memcpy (m_pbyData, pbyData, m_iTotalSize);

		BYTE byPalette[768];
		CDibSection ds;
		ds.Init (iHeight, iWidth, 8, byPalette, FALSE);
		ds.SetRawBits (m_pbyData);
		ds.RotateBits(ROTATETO90);			
		ds.MirrorBits();
		ds.GetRawBits (m_pbyData);
	}

	m_iTileNumber = iTileNumber;
	
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		~CBuildItem ()
//	Action:		Deconstructor... clean up!
/////////////////////////////////////////////////////////////////////////////
CBuildItem::~CBuildItem ()
{
	if (m_pbyData)
	{
		delete [] m_pbyData;	// neal - must use array delete to delete arrays!
		m_pbyData = NULL;
	}
	SetNext (NULL);
	SetPrevious (NULL);
}

BYTE *CBuildItem::GetBits ()
{
	return m_pbyData;		
}

void CBuildItem::UpdateData (BYTE *pbyBits, int iWidth, int iHeight)
{
	int iCurrentWidth	= GetWidth();
	int iCurrentHeight	= GetHeight();	

	if ((iCurrentWidth != iWidth) || (iCurrentHeight != iHeight))
	{
		// Rebuild the MIP from scratch
		ReplaceData (pbyBits, iWidth, iHeight);
		return;
	}

	
}

void CBuildItem::ReplaceData (BYTE *pbyBits, int iWidth, int iHeight)
{
	// Clear out all the data, but keep the linked list pointers intact.
	if (m_pbyData)
	{
		delete [] m_pbyData;
		m_pbyData = NULL;
	}	
		
	m_iTotalSize = iWidth * iHeight;

	if ((iWidth > 0) && (iHeight > 0))
	{
		m_pbyData = new BYTE[m_iTotalSize];
		memcpy (m_pbyData, pbyBits, m_iTotalSize);
	}
	
	SetWidth (iWidth);
	SetHeight (iHeight);
}

void CBuildItem::SetWidth (int iWidth)
{
	m_iWidth = iWidth;
}

int CBuildItem::GetWidth ()
{	
	return m_iWidth;
}

void CBuildItem::SetHeight (int iHeight)
{
	m_iHeight = iHeight;	
}

int CBuildItem::GetHeight ()
{
	return m_iHeight;
}

void CBuildItem::SetNext (CBuildItem *pNext)
{
	m_pNext = pNext;
}

CBuildItem *CBuildItem::GetNext()
{	
	return m_pNext;
}
void CBuildItem::SetPrevious (CBuildItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CBuildItem *CBuildItem::GetPrevious()
{
	return m_pPrevious;
}

BYTE *CBuildItem::GetPalette()
{
	return NULL;	
}

int CBuildItem::GetTileNumber()
{
	return m_iTileNumber;
}

int CBuildItem::GetSize()
{
	return m_iTotalSize;
}

void CBuildItem::SetARTType (int iType)
{
	m_iARTType = iType;
}

int CBuildItem::GetARTType ()
{
	return m_iARTType;
}

int CBuildItem::GetFlags()
{
	return m_iFlags;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 	CBuildList 
////////////////////////////////////////////////////////////////////////////////////////////////////
CBuildList::CBuildList ()
{
	m_iARTType = UNKNOWN_TYPE;
	SetAll (NULL);
}

CBuildList::~CBuildList ()
{
	PurgeList();
}


void CBuildList::SetFirst (CBuildItem *pFirst)
{
	m_pFirst = pFirst;
}

CBuildItem *CBuildList::GetFirst ()
{
	return m_pFirst;
}

void CBuildList::SetRead (CBuildItem *pRead)
{
	m_pRead = pRead;
}

CBuildItem *CBuildList::GetRead ()
{
	return m_pRead;
}

void CBuildList::SetWrite (CBuildItem *pWrite)
{
	m_pWrite = pWrite;
}

CBuildItem *CBuildList::GetWrite ()
{
	return m_pWrite;
}

void CBuildList::SetAll (CBuildItem *pItem)
{
	SetFirst (pItem);
	SetRead (pItem);
	SetWrite (pItem);
}

void CBuildList::PurgeList()
{
	CBuildItem *pItem = GetFirst();
	CBuildItem *pItem2 = NULL;

	while (pItem)
	{
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;
	}
	pItem = pItem2 = NULL;
	SetAll (NULL);
}

void CBuildList::RemoveImage (CBuildItem *pItem)
{
	SetRead (GetFirst());

	while (m_pRead)
	{		
		if (m_pRead == pItem)
		{
			if (m_pRead->GetPrevious())
			{
				m_pRead->GetPrevious()->SetNext (m_pRead->GetNext());
			}

			if (m_pRead->GetNext())
			{
				m_pRead->GetNext()->SetPrevious (m_pRead->GetPrevious());
			}

			if (m_pRead == m_pFirst)
			{
				SetFirst (m_pRead->GetNext());
			}

			if (m_pRead == m_pWrite)
			{
				SetWrite (m_pRead->GetPrevious());
			}

			m_pRead->SetPrevious (NULL);
			m_pRead->SetNext (NULL);
			delete m_pRead;
			m_pRead = NULL;			
		}
		else
		{
			m_pRead = m_pRead->GetNext();
		}
	}
}

int CBuildList::GetNumTiles ()
{
	return (m_iLastTile - m_iFirstTile + 1);
}

CBuildItem *CBuildList::GetFirstTile()
{	
	SetRead (GetFirst());
	return m_pRead;	
}

CBuildItem *CBuildList::GetNextTile()
{
	if (GetRead())
	{
		m_pRead = m_pRead->GetNext();		
		return m_pRead;
	}
	else
	{
		ASSERT (FALSE);
	}

	return NULL;	
}


CBuildItem *CBuildList::AddItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber)
{
	//ASSERT (GetWADType() != UNKNOWN_TYPE);		// Have to set the WAD type before adding anything!
	
	CBuildItem *pNewItem = new CBuildItem (pbyData, iWidth, iHeight, iFlags, iTileNumber);
	
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
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

	return pNewItem;
}

bool CBuildList::IsEmpty()
{
	return (m_pFirst == NULL);
}

int CBuildList::GetTotalItemsSize()
{
	int iSize = 0;

	SetRead (GetFirst());
	while (m_pRead)
	{
		iSize += m_pRead->GetSize();
		m_pRead = m_pRead->GetNext();
	}
	return iSize;
}

void CBuildList::SetARTType (int iType)
{	
	m_iARTType = iType;
}

int CBuildList::GetARTType ()
{
	return m_iARTType;
}

int CBuildList::Serialize (LPCTSTR szFileName, CFile *pFile, BYTE *pbyData, bool bSave /* = false */)
{	
	BYTE *pbyFileData = NULL;
	BYTE *pbyItemData = NULL;
	struct Build_Header *BuildHeader;
	unsigned short *piX = NULL;
	unsigned short *piY = NULL;
	int *piFlags		= NULL;
	int iHeaderSize = sizeof(struct Build_Header);
	int iNumTiles = 0;
	int iSize = 0;

	int iWidth = 0;
	int iHeight = 0;
	
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
			
			int iLength = fp.GetLength();
			pbyFileData = new BYTE[iLength];
			fp.Read (pbyFileData, iLength);
			fp.Close();
		}

		if (pFile)
		{
			ASSERT (!pbyData);
			ASSERT (!szFileName);

			int iLength = pFile->GetLength();
			pbyFileData = new BYTE[iLength];
			pFile->Read (pbyFileData, iLength);			
		}
		
		if (pbyData)
		{
			ASSERT (!szFileName);
			ASSERT (!pFile);
			pbyFileData = pbyData;
		}
			
		
		BuildHeader = (struct Build_Header *)(pbyFileData);

		m_iFirstTile = BuildHeader->TileStart;
		m_iLastTile = BuildHeader->TileEnd;

		iNumTiles = (m_iLastTile - m_iFirstTile) + 1;
		int iTileNumber = 0;
		int j = 0;

		piX = (unsigned short *)(pbyFileData + iHeaderSize);
		piY = (unsigned short *)(pbyFileData + iHeaderSize + (iNumTiles * sizeof(unsigned short)));
		piFlags = (int *)(pbyFileData + iHeaderSize + (iNumTiles * sizeof(unsigned short) * 2)); 

		pbyItemData = pbyFileData + iHeaderSize + ((iNumTiles * sizeof(unsigned short)) * 2) + (iNumTiles * sizeof (int));

		for (j = 0, iTileNumber = BuildHeader->TileStart; j < iNumTiles; j++, iTileNumber++)
		{
			iSize = (*piX) * (*piY);
			AddItem (pbyItemData, *piX, *piY, *piFlags, iTileNumber);		

			pbyItemData += iSize;
			piX++;
			piY++;
			piFlags++;
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

		BYTE *pbyPackedData	= NULL;		
		BYTE byPalette[768];
		CDibSection *ds		= NULL;
		
		
		iNumTiles = GetNumTiles();

		int iTotalSize = iHeaderSize;		
		iTotalSize += iNumTiles * ((sizeof(unsigned short) * 2) + sizeof (int));
		iTotalSize += GetTotalItemsSize();

		pbyPackedData = new BYTE[iTotalSize];

		if (!pbyPackedData)
		{
			return IH_OUT_OF_MEMORY;
		}
		memset (pbyPackedData, 0, iTotalSize);

		BuildHeader = (struct Build_Header *)(pbyPackedData);
		piX = (unsigned short *)(pbyPackedData + iHeaderSize);
		piY = (unsigned short *)(pbyPackedData + iHeaderSize + (iNumTiles * sizeof(unsigned short)));
		piFlags = (int *)(pbyPackedData + iHeaderSize + (iNumTiles * sizeof(unsigned short) * 2)); 
		
		pbyItemData = pbyPackedData + iHeaderSize + ((iNumTiles * sizeof(unsigned short)) * 2) + (iNumTiles * sizeof (int));

		BuildHeader->Version = 1;
		BuildHeader->NumTiles = iNumTiles;
		BuildHeader->TileStart = m_iFirstTile;
		BuildHeader->TileEnd = m_iLastTile;
		
		CBuildItem *pItem = GetFirst();

		while (pItem)
		{
			iWidth = (*piX) = pItem->GetWidth();
			iHeight = (*piY) = pItem->GetHeight();
			(*piFlags) = pItem->GetFlags();

			iSize = iWidth * iHeight;

			if (iSize)
			{
				ds = new CDibSection();
				if (!ds)
				{
					if (pbyPackedData)
					{
						delete [] pbyPackedData;
						pbyPackedData = NULL;
					}
					return IH_OUT_OF_MEMORY;
				}
				ds->Init (iWidth, iHeight, 8, byPalette, FALSE);				
				ds->SetRawBits (pItem->GetBits());
				ds->MirrorBits();
				ds->RotateBits(ROTATETO270);				
				ds->GetRawBits (pbyItemData);
								
				pbyItemData += iSize;

				if (ds)
				{
					delete ds;
					ds = NULL;
				}
			}			
			
			piX++;
			piY++;
			piFlags++;

			pItem = pItem->GetNext();
		}

		pSaveFile->Write (pbyPackedData, iTotalSize);		

		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}

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

CBuildItem *CBuildList::GetTile(int iTileNumber)
{
	CBuildItem *pTemp = GetFirstTile();

	while (pTemp)
	{
		if (pTemp->GetTileNumber() == iTileNumber)
		{
			return pTemp;
		}
		pTemp = GetNextTile();
	}

	return NULL;

}