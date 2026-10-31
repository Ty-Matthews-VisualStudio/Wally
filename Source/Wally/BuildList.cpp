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
//	Name:		CBuildItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber)
//	Action:		Constructor for class.  Copies the tile's pixels and rotates them
//				into Wally's orientation.  Caller must pass in valid data.
/////////////////////////////////////////////////////////////////////////////
CBuildItem::CBuildItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber)
{	
	m_pWallyDoc		= NULL;
	m_iARTType		= UNKNOWN_TYPE;
	m_iFlags		= iFlags;
	m_iTileNumber	= iTileNumber;

	ASSERT (pbyData);
	
	SetWidth (iWidth);
	SetHeight (iHeight);
	m_iTotalSize = iWidth * iHeight;

	if ((iWidth > 0) && (iHeight > 0))
	{
		m_Data.assign (pbyData, pbyData + m_iTotalSize);

		BYTE byPalette[768];
		CDibSection ds;
		ds.Init (iHeight, iWidth, 8, byPalette, FALSE);
		ds.SetRawBits (m_Data.data());
		ds.RotateBits(ROTATETO90);			
		ds.MirrorBits();
		ds.GetRawBits (m_Data.data());
	}
}

BYTE *CBuildItem::GetBits ()
{
	return m_Data.empty() ? NULL : m_Data.data();
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
	m_iTotalSize = iWidth * iHeight;

	if ((iWidth > 0) && (iHeight > 0))
	{
		m_Data.assign (pbyBits, pbyBits + m_iTotalSize);
	}
	else
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
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
	m_iFirstTile = 0;
	m_iLastTile = -1;
}

void CBuildList::PurgeList()
{
	m_Items.clear();
	m_iFirstTile = 0;
	m_iLastTile = -1;
}

int CBuildList::GetNumTiles ()
{
	return (m_iLastTile - m_iFirstTile + 1);
}

void CBuildList::AddItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber)
{
	ASSERT (iTileNumber == m_iFirstTile + (int)m_Items.size());		// GetTile() indexes by position
	m_Items.emplace_back (pbyData, iWidth, iHeight, iFlags, iTileNumber);
}

bool CBuildList::IsEmpty()
{
	return m_Items.empty();
}

int CBuildList::GetTotalItemsSize()
{
	int iSize = 0;

	for (CBuildItem &item : m_Items)
	{
		iSize += item.GetSize();
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
		std::vector<BYTE> vbyFileData;

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
			vbyFileData.resize (iLength);
			fp.Read (vbyFileData.data(), iLength);
			fp.Close();
			pbyFileData = vbyFileData.data();
		}

		if (pFile)
		{
			ASSERT (!pbyData);
			ASSERT (!szFileName);

			int iLength = pFile->GetLength();
			vbyFileData.resize (iLength);
			pFile->Read (vbyFileData.data(), iLength);			
			pbyFileData = vbyFileData.data();
		}
		
		if (pbyData)
		{
			ASSERT (!szFileName);
			ASSERT (!pFile);
			pbyFileData = pbyData;
		}
			
		
		BuildHeader = (struct Build_Header *)(pbyFileData);

		PurgeList();
		m_iFirstTile = BuildHeader->TileStart;
		m_iLastTile = BuildHeader->TileEnd;

		iNumTiles = (m_iLastTile - m_iFirstTile) + 1;
		int iTileNumber = 0;
		int j = 0;

		if (iNumTiles > 0)
		{
			m_Items.reserve (iNumTiles);
		}

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
		CFile fSaveFile;
		CFile *pSaveFile = NULL;

		if (pFile)
		{
			ASSERT (!szFileName);
			pSaveFile = pFile;
		}

		if (szFileName)
		{
			ASSERT (!pFile);
			CFileException e;

			if (!fSaveFile.Open(szFileName, CFile::modeCreate | CFile::modeWrite, &e ))
			{
				return IH_ERROR_WRITING_FILE;
			}			
			pSaveFile = &fSaveFile;
		}		

		ASSERT (pSaveFile);

		BYTE byPalette[768];
		
		iNumTiles = GetNumTiles();
		ASSERT ((int)m_Items.size() == iNumTiles);

		int iTotalSize = iHeaderSize;		
		iTotalSize += iNumTiles * ((sizeof(unsigned short) * 2) + sizeof (int));
		iTotalSize += GetTotalItemsSize();

		std::vector<BYTE> vbyPackedData (iTotalSize, 0);
		BYTE *pbyPackedData = vbyPackedData.data();

		BuildHeader = (struct Build_Header *)(pbyPackedData);
		piX = (unsigned short *)(pbyPackedData + iHeaderSize);
		piY = (unsigned short *)(pbyPackedData + iHeaderSize + (iNumTiles * sizeof(unsigned short)));
		piFlags = (int *)(pbyPackedData + iHeaderSize + (iNumTiles * sizeof(unsigned short) * 2)); 
		
		pbyItemData = pbyPackedData + iHeaderSize + ((iNumTiles * sizeof(unsigned short)) * 2) + (iNumTiles * sizeof (int));

		BuildHeader->Version = 1;
		BuildHeader->NumTiles = iNumTiles;
		BuildHeader->TileStart = m_iFirstTile;
		BuildHeader->TileEnd = m_iLastTile;
		
		for (CBuildItem &item : m_Items)
		{
			iWidth = (*piX) = item.GetWidth();
			iHeight = (*piY) = item.GetHeight();
			(*piFlags) = item.GetFlags();

			iSize = iWidth * iHeight;

			if (iSize)
			{
				CDibSection ds;
				ds.Init (iWidth, iHeight, 8, byPalette, FALSE);				
				ds.SetRawBits (item.GetBits());
				ds.MirrorBits();
				ds.RotateBits(ROTATETO270);				
				ds.GetRawBits (pbyItemData);
								
				pbyItemData += iSize;
			}			
			
			piX++;
			piY++;
			piFlags++;
		}

		pSaveFile->Write (pbyPackedData, iTotalSize);		
	}
	return IH_SUCCESS;
}

CBuildItem *CBuildList::GetTile(int iTileNumber)
{
	int iIndex = iTileNumber - m_iFirstTile;

	if ((iIndex < 0) || (iIndex >= (int)m_Items.size()))
	{
		return NULL;
	}

	ASSERT (m_Items[iIndex].GetTileNumber() == iTileNumber);
	return &m_Items[iIndex];
}
