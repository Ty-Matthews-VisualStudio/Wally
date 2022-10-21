/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WAD3List.cpp : implementation of the CWAD3Item and CWAD3List classes
//
//  Created by Ty Matthews, 12-17-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "WAD3List.h"
#include "Wally.h"
#include "WallyPal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CWAD3Item implementation
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Name:		CWAD3Item()
//	Action:		Default constructor for class.  Only to be used with a separate
//				call to CreateAsMip()
/////////////////////////////////////////////////////////////////////////////
CWAD3Item::CWAD3Item ()
{
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3Header	= NULL;
	m_pPaletteSize	= NULL;
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);
	SetType (0);
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CWAD3Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
//	Action:		Constructor for class.  Initializes data pointers and builds
//				the lump item properly.  Caller must pass in valid data.
//
//	Parameters:
//
//	unsigned char *pbyData		: pointer to raw lump data.  No formatting should be done prior to this
//	int iDatasize				: total lump size, as defined by the corresponding lump info
//	char cType					: type member from struct wad3_lumpinfo_s
//	LPCTSTR szName				: pointer to name of lump item
/////////////////////////////////////////////////////////////////////////////
CWAD3Item::CWAD3Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
{	
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3Header	= NULL;
	m_pPaletteSize	= NULL;
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);

	ASSERT (pbyData);
	ASSERT (iDataSize > 0);
	ASSERT (szName);
	
	SetType (cType);
	int iWidth	= 0;
	int iHeight	= 0;
	int iPaletteOffset = 0;
	int iBugSize = 0;
	int j = 0;

	m_iTotalSize = iDataSize;
	m_pbyData = new unsigned char[m_iTotalSize];
	memcpy (m_pbyData, pbyData, m_iTotalSize);	

	switch (GetType())
	{
	case WAD3_TYPE_MIP:
		m_wad3Header = (struct wad3_miptex_s *)m_pbyData;
		
		m_strName = m_wad3Header->name;
		
		iWidth	= m_wad3Header->width;
		iHeight = m_wad3Header->height;

		m_iSizes[0] = iWidth * iHeight;
		m_iSizes[1] = iWidth * iHeight / 4;
		m_iSizes[2] = iWidth * iHeight / 16;
		m_iSizes[3] = iWidth * iHeight / 64;

		iBugSize = sizeof (struct wad3_miptex_s);		
		iPaletteOffset = sizeof (struct wad3_miptex_s);
		for (j = 0; j < 4; j++)
		{
			iPaletteOffset += m_iSizes[j];
			iBugSize += m_iSizes[j];
		}		
		m_pPaletteSize = (unsigned short *)(m_pbyData + iPaletteOffset);
		iPaletteOffset += sizeof (unsigned short);
		iBugSize += sizeof (unsigned short);
		iBugSize += 768;

		// iBugSize is to fix a bug I had with determining the location
		// of the palette... our first couple releases of Wally would
		// write the palette out at the wrong location (although it still worked.)
		// I originally didn't add the padding at the end of the mip when
		// saving, even though I was accounting for it when reading.
		// This code tests for that bug, and accounts for it, so the user
		// is not affected by the new location.  Saving it again will
		// correct the problem.
		
		// This bug only affects people that created brand new images and saved
		// them into a WAD.  Existing WADs were handled as-is, so the correct
		// size was used

		if (iBugSize == iDataSize)
		{
			// Point at the palette, which is at the wrong spot
			m_pbyPalette = m_pbyData + iDataSize - 770;
			unsigned char byHoldPalette[768];

			// Save the palette into another buffer
			memcpy (byHoldPalette, m_pbyPalette, 768);			
			
			// Adjust the total size
			m_iTotalSize += sizeof (unsigned short);
			
			// Make sure we hang on to the real mip data
			unsigned char *pbyHoldData = m_pbyData;
			
			// Go build the proper size
			m_pbyData = NULL;
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			// Copy everything over
			memcpy (m_pbyData, pbyHoldData, iDataSize);

			// Reset the pointers
			m_wad3Header = (struct wad3_miptex_s *)m_pbyData;
			m_pbyPalette = m_pbyData + iPaletteOffset;
			m_pPaletteSize = (unsigned short *)(m_pbyData + iPaletteOffset - sizeof (unsigned short));
			(*m_pPaletteSize) = 256;
			
			// Copy over the palette
			memcpy (m_pbyPalette, byHoldPalette, 768);			

			// Delete the original buffer
			if (pbyHoldData)
			{
				delete pbyHoldData;
				pbyHoldData = NULL;
			}			
		}
		else
		{
			m_pbyPalette = m_pbyData + iPaletteOffset;
		}
		break;

	default:
		// Do nothing with the raw data, we'll just write it back out AS-IS later
		m_strName = szName;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CreateAsMip (unsigned char *pbyBits[], unsigned char *pbyPalette, LPCTSTR szName, int iWidth, int iHeight)
//	Action:		Only to be used with non-descript constructor.  Used to build
//				a mip item (and only a mip item), which is type 0x43.
//
//	Parameters:
//
//	unsigned char *pbyBits[]	: pointer to array of pointers.  4 pointers for the 4 mips
//	CWallyPalette *pPalette		: pointer to palette
//	LPCTSTR szName				: pointer to lump name
//	int iWidth, int iHeight		: hmmm
/////////////////////////////////////////////////////////////////////////////
void CWAD3Item::CreateAsMip (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight)
{
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3Header	= NULL;
	m_pPaletteSize	= NULL;
	SetNext (NULL);
	SetPrevious (NULL);
	SetType (WAD3_TYPE_MIP);
	int j = 0;
	int iPaletteOffset = 0;

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}
	ASSERT (pPalette);
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);
	
	m_iTotalSize = sizeof (struct wad3_miptex_s);
	
	m_iSizes[0] = iWidth * iHeight;
	m_iSizes[1] = iWidth * iHeight / 4;
	m_iSizes[2] = iWidth * iHeight / 16;
	m_iSizes[3] = iWidth * iHeight / 64;

	for (j = 0; j < 4; j++)
	{
		m_iTotalSize += m_iSizes[j];		
	}

	m_iTotalSize += sizeof (short) + 768 + sizeof (short);
	
	m_pbyData = new unsigned char[m_iTotalSize];
	memset (m_pbyData, 0, m_iTotalSize);

	m_wad3Header = (struct wad3_miptex_s *)m_pbyData;
	
	iPaletteOffset = sizeof (struct wad3_miptex_s);
	for (j = 0; j < 4; j++)
	{
		iPaletteOffset += m_iSizes[j];
	}
	m_pPaletteSize = (unsigned short *)(m_pbyData + iPaletteOffset);
	(*m_pPaletteSize) = 256;

	iPaletteOffset += sizeof (unsigned short);	
	m_pbyPalette = m_pbyData + iPaletteOffset;

	for (j = 0; j < 256; j++)
	{
		m_pbyPalette[j * 3 + 0] = pPalette->GetR(j);
		m_pbyPalette[j * 3 + 1] = pPalette->GetG(j);
		m_pbyPalette[j * 3 + 2] = pPalette->GetB(j);
	}		

	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);

	int iHeaderSize = sizeof (struct wad3_miptex_s);
	m_wad3Header->offsets[0] = iHeaderSize;	
	m_wad3Header->offsets[1] = iHeaderSize + m_iSizes[0];
	m_wad3Header->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
	m_wad3Header->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

	for (j = 0; j < 4; j++)
	{
		memcpy ((m_pbyData + m_wad3Header->offsets[j]), pbyBits[j], m_iSizes[j]);
	}

}

/////////////////////////////////////////////////////////////////////////////
//	Name:		~CWAD3Item ()
//	Action:		Deconstructor... clean up!
/////////////////////////////////////////////////////////////////////////////
CWAD3Item::~CWAD3Item ()
{
	if (m_pbyData)
	{
		delete m_pbyData;
		m_pbyData = NULL;
	}
	SetNext (NULL);
	SetPrevious (NULL);
}

unsigned char *CWAD3Item::GetData ()
{
	return m_pbyData;
}

unsigned char *CWAD3Item::GetBits (int iMipNumber)
{
	ASSERT (GetType() == WAD3_TYPE_MIP);
	ASSERT ((iMipNumber >= 0) && (iMipNumber <= 3));
	return (m_pbyData + m_wad3Header->offsets[iMipNumber]);
}

void CWAD3Item::UpdateData (unsigned char *pbyBits[], CWallyPalette *pPalette)
{
	ASSERT (GetType() == WAD3_TYPE_MIP);

	for (int j = 0; j < 4; j++)
	{
		memcpy (m_pbyData + m_wad3Header->offsets[j], pbyBits[j], m_iSizes[j]);
	}
	
	//memcpy (m_pbyPalette, pPalette, 768);
	for (j = 0; j < 256; j++)
	{
		m_pbyPalette[j * 3 + 0] = pPalette->GetR(j);
		m_pbyPalette[j * 3 + 1] = pPalette->GetG(j);
		m_pbyPalette[j * 3 + 2] = pPalette->GetB(j);
	}
}

void CWAD3Item::ReplaceData (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight)
{
	// Clear out all the data, but keep the linked list pointers intact.
	if (m_pbyData)
	{
		delete m_pbyData;		
	}
	
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3Header	= NULL;
	m_pPaletteSize	= NULL;	
	int iPaletteOffset = 0;
	int j = 0;

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}
	ASSERT (pPalette);
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);
	
	m_iTotalSize = sizeof (struct wad3_miptex_s);
	
	m_iSizes[0] = iWidth * iHeight;
	m_iSizes[1] = iWidth * iHeight / 4;
	m_iSizes[2] = iWidth * iHeight / 16;
	m_iSizes[3] = iWidth * iHeight / 64;

	for (j = 0; j < 4; j++)
	{
		m_iTotalSize += m_iSizes[j];		
	}

	m_iTotalSize += sizeof (short) + 768 + sizeof (short);
	
	m_pbyData = new unsigned char[m_iTotalSize];
	memset (m_pbyData, 0, m_iTotalSize);

	m_wad3Header = (struct wad3_miptex_s *)m_pbyData;
	
	iPaletteOffset = sizeof (struct wad3_miptex_s);
	for (j = 0; j < 4; j++)
	{
		iPaletteOffset += m_iSizes[j];
	}
	m_pPaletteSize = (unsigned short *)(m_pbyData + iPaletteOffset);
	(*m_pPaletteSize) = 256;

	iPaletteOffset += sizeof (unsigned short);	
	m_pbyPalette = m_pbyData + iPaletteOffset;

	for (j = 0; j < 256; j++)
	{
		m_pbyPalette[j * 3 + 0] = pPalette->GetR(j);
		m_pbyPalette[j * 3 + 1] = pPalette->GetG(j);
		m_pbyPalette[j * 3 + 2] = pPalette->GetB(j);
	}		

	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);

	int iHeaderSize = sizeof (struct wad3_miptex_s);
	m_wad3Header->offsets[0] = iHeaderSize;	
	m_wad3Header->offsets[1] = iHeaderSize + m_iSizes[0];
	m_wad3Header->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
	m_wad3Header->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

	for (j = 0; j < 4; j++)
	{
		memcpy ((m_pbyData + m_wad3Header->offsets[j]), pbyBits[j], m_iSizes[j]);
	}
}

void CWAD3Item::SetWidth (int iWidth)
{
	ASSERT (GetType() == WAD3_TYPE_MIP);
	m_wad3Header->width = iWidth;
}

int CWAD3Item::GetWidth ()
{
	ASSERT (GetType() == WAD3_TYPE_MIP);
	return m_wad3Header->width;
}

void CWAD3Item::SetHeight (int iHeight)
{
	ASSERT (GetType() == WAD3_TYPE_MIP);
	m_wad3Header->height = iHeight;
}

int CWAD3Item::GetHeight ()
{
	ASSERT (GetType() == WAD3_TYPE_MIP);
	return m_wad3Header->height;
}

void CWAD3Item::SetName (LPCTSTR szName)
{
	ASSERT (strlen(szName) < 16);		// Only 16 chars available, and one has to be the NULL terminator!
	m_strName = szName;
	if (GetType() == WAD3_TYPE_MIP)
	{
		ASSERT (m_wad3Header);
		memset (m_wad3Header->name, 0, 16);
		memcpy (m_wad3Header->name, szName, 15);
	}
}

CString CWAD3Item::GetName ()
{
	//CString strReturn (m_wad3Header->name);
	return m_strName;
}

void CWAD3Item::SetNext (CWAD3Item *pNext)
{
	m_pNext = pNext;
}

CWAD3Item *CWAD3Item::GetNext()
{	
	return m_pNext;
}
void CWAD3Item::SetPrevious (CWAD3Item *pPrevious)
{
	m_pPrevious = pPrevious;
}

CWAD3Item *CWAD3Item::GetPrevious()
{
	return m_pPrevious;
}

unsigned char *CWAD3Item::GetPalette()
{
	return m_pbyPalette;
}

int CWAD3Item::GetSize()
{
	return m_iTotalSize;
}

void CWAD3Item::SetType (char cType)
{
	m_cType = cType;
}

char CWAD3Item::GetType ()
{
	return m_cType;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// 	CWAD3List 
////////////////////////////////////////////////////////////////////////////////////////////////////
CWAD3List::CWAD3List ()
{
	SetAll (NULL);
}

CWAD3List::~CWAD3List ()
{
	PurgeList();
}


void CWAD3List::SetFirst (CWAD3Item *pFirst)
{
	m_pFirst = pFirst;
}

CWAD3Item *CWAD3List::GetFirst ()
{
	return m_pFirst;
}

void CWAD3List::SetRead (CWAD3Item *pRead)
{
	m_pRead = pRead;
}

CWAD3Item *CWAD3List::GetRead ()
{
	return m_pRead;
}

void CWAD3List::SetWrite (CWAD3Item *pWrite)
{
	m_pWrite = pWrite;
}

CWAD3Item *CWAD3List::GetWrite ()
{
	return m_pWrite;
}

void CWAD3List::SetAll (CWAD3Item *pItem)
{
	SetFirst (pItem);
	SetRead (pItem);
	SetWrite (pItem);
}

void CWAD3List::PurgeList()
{
	CWAD3Item *pItem = GetFirst();
	CWAD3Item *pItem2 = NULL;

	while (pItem)
	{
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;
	}
	pItem = pItem2 = NULL;
	SetAll (NULL);
}

void CWAD3List::RemoveImage (CWAD3Item *pItem)
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

	
int CWAD3List::GetNumImages ()
{
	// Only return the count of images
	int iCount = 0;

	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->GetType() == WAD3_TYPE_MIP)
		{
			iCount++;
		}
		m_pRead = m_pRead->GetNext();
	}

	return iCount;
}

int CWAD3List::GetNumLumps ()
{
	// Return the count of all lumps, regardless of type
	int iCount = 0;

	SetRead (GetFirst());

	while (m_pRead)
	{
		iCount++;		
		m_pRead = m_pRead->GetNext();
	}

	return iCount;
}

CWAD3Item *CWAD3List::GetFirstImage()
{	
	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->GetType() == WAD3_TYPE_MIP)
		{
			return m_pRead;
		}
		else
		{
			m_pRead = m_pRead->GetNext();
		}
	}
	
	return (NULL);
}

CWAD3Item *CWAD3List::GetNextImage()
{
	if (GetRead())
	{
		m_pRead = m_pRead->GetNext();		
		while (m_pRead)
		{
			if (m_pRead->GetType() == WAD3_TYPE_MIP)
			{
				return m_pRead;
			}
			else
			{
				m_pRead = m_pRead->GetNext();
			}
		}		
	}

	return NULL;	
}

CWAD3Item *CWAD3List::GetFirstLump()
{
	SetRead (GetFirst());	
	return (GetRead());
}

CWAD3Item *CWAD3List::GetNextLump()
{
	if (m_pRead)
	{
		m_pRead = m_pRead->GetNext();
		return m_pRead;
	}
	
	return NULL;
}

CWAD3Item *CWAD3List::AddItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
{
	CWAD3Item *pNewItem = new CWAD3Item (pbyData, iDataSize, cType, szName);
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}
	CString strCompare1 ("");
	CString strCompare2 (szName);
	CWAD3Item *pTempLeft	= NULL;
	CWAD3Item *pTempRight	= NULL;        

	strCompare2.MakeLower();

	if (!GetFirst())
	{
		SetAll (pNewItem);
	}
	else
	{
#define SORT 0

		switch (SORT)
		{
		case 0:	
			m_pWrite->SetNext (pNewItem);
			pNewItem->SetPrevious (m_pWrite);
			m_pWrite = pNewItem;
			break;
			
		case 1:
			m_pRead = GetFirst();
		
			// Insert the item into the sorted list 
			while (m_pRead)
		    {
				strCompare1 = m_pRead->GetName();
				strCompare1.MakeLower();
	
		        if (strCompare1 > strCompare2)
				{                           
					pTempLeft	= m_pRead->GetPrevious();
					pTempRight = m_pRead;
					m_pRead = NULL;
				}
				else
				{
					m_pRead = m_pRead->GetNext();
				}
			}
	        
			//  Item belongs at the end 
			if ((pTempRight == NULL) && (pTempLeft == NULL))
			{                   
	            m_pWrite->SetNext (pNewItem);
				pNewItem->SetPrevious (m_pWrite);
	            SetWrite (pNewItem);
			}
	
			//  Item is the first in the list 
			if ((pTempLeft == NULL) && (pTempRight != NULL))
			{		    
				pNewItem->SetNext (m_pFirst);
				m_pFirst->SetPrevious (pNewItem);
				SetFirst (pNewItem);
			}

			//  Item is somewhere in the middle of the list 
			if ((pTempLeft != NULL) && (pTempRight != NULL))
			{			
				pTempLeft->SetNext (pNewItem);
				pNewItem->SetPrevious (pTempLeft);
				pNewItem->SetNext (pTempRight);
				pTempRight->SetPrevious (pNewItem);
			}
			break;

		default:
			ASSERT (false);
			break;
		}
	}		

	return pNewItem;
}

CWAD3Item *CWAD3List::AddMipItem (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight)
{	
	CWAD3Item *pNewItem = new CWAD3Item ();	
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}	

	pNewItem->CreateAsMip (pbyBits, pPalette, szName, iWidth, iHeight);	

	CString strCompare1 ("");
	CString strCompare2 (szName);
	CWAD3Item *pTempLeft	= NULL;
	CWAD3Item *pTempRight	= NULL;        

	strCompare2.MakeLower();

	if (!GetFirst())
	{		
		SetAll (pNewItem);
	}
	else
	{		
#define SORT 0

		switch (SORT)
		{
		case 0:	
			m_pWrite->SetNext (pNewItem);
			pNewItem->SetPrevious (m_pWrite);
			m_pWrite = pNewItem;
			break;
			
		case 1:
			m_pRead = GetFirst();
		
			// Insert the item into the sorted list 
			while (m_pRead)
		    {
				strCompare1 = m_pRead->GetName();
				strCompare1.MakeLower();
	
		        if (strCompare1 > strCompare2)
				{                           
					pTempLeft	= m_pRead->GetPrevious();
					pTempRight = m_pRead;
					m_pRead = NULL;
				}
				else
				{
					m_pRead = m_pRead->GetNext();
				}
			}
	        
			//  Item belongs at the end 
			if ((pTempRight == NULL) && (pTempLeft == NULL))
			{                   
	            m_pWrite->SetNext (pNewItem);
				pNewItem->SetPrevious (m_pWrite);
	            SetWrite (pNewItem);
			}
	
			//  Item is the first in the list 
			if ((pTempLeft == NULL) && (pTempRight != NULL))
			{		    
				pNewItem->SetNext (m_pFirst);
				m_pFirst->SetPrevious (pNewItem);
				SetFirst (pNewItem);
			}

			//  Item is somewhere in the middle of the list 
			if ((pTempLeft != NULL) && (pTempRight != NULL))
			{			
				pTempLeft->SetNext (pNewItem);
				pNewItem->SetPrevious (pTempLeft);
				pNewItem->SetNext (pTempRight);
				pTempRight->SetPrevious (pNewItem);
			}
			break;

		default:
			ASSERT (false);
			break;
		}
	}		

	return pNewItem;

}

bool CWAD3List::IsEmpty()
{
	return (m_pFirst == NULL);
}

int CWAD3List::GetTotalItemsSize()
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

CWAD3Item *CWAD3List::IsNameInList (LPCTSTR szName)
{		
	CString strName (szName);
	CString strCompare ("");
	strName.MakeLower();

	SetRead (GetFirst());

	while (m_pRead)
	{		
		strCompare = m_pRead->GetName();
		strCompare.MakeLower();

		if (strCompare == strName)
		{
			return m_pRead;
		}
		m_pRead = m_pRead->GetNext();
	}

	return NULL;
}

int CWAD3List::Serialize (CFile *pFile)
{
	if (IsEmpty())
	{
		ASSERT (false);		// Can't save an empty list
		return IH_WAD_NO_IMAGES;		
	}

	unsigned char *pPackedData	= NULL;
	unsigned char *pBits		= NULL;
	unsigned char *pPalette		= NULL;
	unsigned char *pWADData		= NULL;
	unsigned char *pLumpData	= NULL;
	
	short *psPaletteSize = NULL;

	// The first four bytes of the WAD are its identification.  Here, that's "WAD3"
	char szIdentification[] = "WAD3";
	
	int iTotalSize = 0;
	int iNumLumps = GetNumLumps();
	int j = 0;
	int k = 0;

	// Show the user what's going on
	CProgressBar ctlProgress ("Saving...", 50, iNumLumps + 3, false, 0);
		
	int iHeaderSize = sizeof (struct wad3_headerinfo_s);
	int iInfoSize = sizeof (struct wad3_lumpinfo_s);
	int iMipHeaderSize = sizeof (struct wad3_miptex_s);
	int iFilePosition = 0;
	int iSize	= 0;
	//int iSizes[4];
	int iWidth	= 0;
	int iHeight = 0;

	CWAD2Item *pItem = NULL;
	CString strName("");
	
	// First, we need to determine the overall WAD3 size.
	// A WAD3 is split up like so:
	//
	//			- Beginning of file
	//			- wad3_headerinfo_s
	//			- All of the lumps (each lump (mip) has a header, plus actual pixel values. The overall size varies from mip to mip)
	//			- wad3_lumpinfo_s[] (one lump info for every lump)
	//			- End of file

	iTotalSize += iHeaderSize;
	iTotalSize += (iNumLumps * iInfoSize);

	// Figure out the size of all the mips combined 
	int iItemsSize = GetTotalItemsSize();
	iTotalSize += iItemsSize;

	// Build the buffer
	pPackedData = new unsigned char[iTotalSize];
	if (!pPackedData)
	{
		return IH_OUT_OF_MEMORY;		
	}	

	// Clear it out
	memset (pPackedData, 0, iTotalSize);
	ctlProgress.StepIt();

	// Point the header, and set the data
	m_pwad3Header = (struct wad3_headerinfo_s *)(pPackedData);
	memcpy (m_pwad3Header->identification, szIdentification, 4);
	m_pwad3Header->numlumps = iNumLumps;
	m_pwad3Header->infotableofs = iHeaderSize + iItemsSize;

	// m_pwad3LumpInfo is pointed at the proper offset, and adjusted each time a mip is copied in
	m_pwad3LumpInfo = (struct wad3_lumpinfo_s *)(pPackedData + m_pwad3Header->infotableofs);
	
	// iFilePosition is used to track where we should write out the next mip.  It is adjusted after
	// each mip is copied in
	iFilePosition = iHeaderSize;	

	pItem = GetFirst();

	// Let's go!
	while (pItem)	
	{
		// This iSize *includes* the mip header, and all 4 mips.  It's NOT width * height
		// For non-image types, it includes all the raw data
		iSize = pItem->GetSize();
		
		// Set the lump data pointer, and raw-copy over the lump data from our item
		pWADData = pPackedData + iFilePosition;
		pLumpData = pItem->GetData();
		memcpy (pWADData, pLumpData, iSize);

		// Go set data in the lump info struct
		strName = pItem->GetName();
		memset (m_pwad3LumpInfo->name, 0, 16);
		memcpy (m_pwad3LumpInfo->name, (LPCTSTR)strName, strName.GetLength());

		m_pwad3LumpInfo->filepos = iFilePosition;		// Points to where our mip starts
		m_pwad3LumpInfo->disksize = iSize;				// Total size of the mip (header + data).  This is the actual disk storage, which could be less than true size if compression is used
		m_pwad3LumpInfo->size = iSize;					// Total size of the mip (header + data).  This is the true expanded size of the mip
		m_pwad3LumpInfo->type = pItem->GetType();
		m_pwad3LumpInfo->compression = 0x0;				// TODO: compression supported at some point?
		m_pwad3LumpInfo->pad1 = 0x0;
		m_pwad3LumpInfo->pad2 = 0x0;		
	
		pItem = pItem->GetNext();
		m_pwad3LumpInfo++;
		iFilePosition += iSize;
		ctlProgress.StepIt();
	}
	
	pFile->Write (pPackedData, iTotalSize);
	ctlProgress.StepIt();

	m_pwad3LumpInfo = NULL;
	m_pwad3Mip = NULL;
	m_pwad3Header = NULL;
	
	if (pPackedData)
	{
		delete pPackedData;
		pPackedData = NULL;
	}
	ctlProgress.StepIt();
	ctlProgress.Clear();
}