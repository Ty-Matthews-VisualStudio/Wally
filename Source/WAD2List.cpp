/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WAD2List.cpp : implementation of the CWAD2Item and CWAD2List classes
//
//  Created by Ty Matthews, 12-17-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "WAD2List.h"
#include "Wally.h"
#include "WallyPal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CWAD2Item implementation
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Name:		CWAD2Item()
//	Action:		Default constructor for class.  Only to be used with a separate
//				call to CreateAsMip()
/////////////////////////////////////////////////////////////////////////////
CWAD2Item::CWAD2Item ()
{
	m_pbyData		= NULL;	
	m_wad2Header	= NULL;	
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);
	SetType (0);
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CWAD2Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
//	Action:		Constructor for class.  Initializes data pointers and builds
//				the lump item properly.  Caller must pass in valid data.
//
//	Parameters:
//
//	unsigned char *pbyData		: pointer to raw lump data.  No formatting should be done prior to this
//	int iDatasize				: total lump size, as defined by the corresponding lump info
//	char cType					: type member from struct wad2_lumpinfo_s
//	LPCTSTR szName				: pointer to name of lump item
/////////////////////////////////////////////////////////////////////////////
CWAD2Item::CWAD2Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
{	
	m_pbyData		= NULL;	
	m_wad2Header	= NULL;	
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);

	ASSERT (pbyData);
	ASSERT (iDataSize > 0);
	ASSERT (szName);
	
	SetType (cType);
	int iWidth	= 0;
	int iHeight	= 0;	
	int j = 0;

	m_iTotalSize = iDataSize;
	m_pbyData = new unsigned char[m_iTotalSize];
	memcpy (m_pbyData, pbyData, m_iTotalSize);	

	switch (GetType())
	{
	case WAD2_TYPE_MIP:
		m_wad2Header = (struct wad2_miptex_s *)m_pbyData;
		
		m_strName = m_wad2Header->name;
		
		iWidth	= m_wad2Header->width;
		iHeight = m_wad2Header->height;

		m_iSizes[0] = iWidth * iHeight;
		m_iSizes[1] = iWidth * iHeight / 4;
		m_iSizes[2] = iWidth * iHeight / 16;
		m_iSizes[3] = iWidth * iHeight / 64;		
		break;

	default:
		// Do nothing with the raw data, we'll just write it back out AS-IS later
		m_strName = szName;
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CreateAsMip (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight)
//	Action:		Only to be used with non-descript constructor.  Used to build
//				a mip item (and only a mip item), which is type 0x43.
//
//	Parameters:
//
//	unsigned char *pbyBits[]	: pointer to array of pointers.  4 pointers for the 4 mips
//	LPCTSTR szName				: pointer to lump name
//	int iWidth, int iHeight		: hmmm
/////////////////////////////////////////////////////////////////////////////
void CWAD2Item::CreateAsMip (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight)
{
	m_pbyData		= NULL;	
	m_wad2Header	= NULL;	
	SetNext (NULL);
	SetPrevious (NULL);
	SetType (WAD2_TYPE_MIP);
	int j = 0;	

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}	
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);
	
	m_iTotalSize = sizeof (struct wad2_miptex_s);
	
	m_iSizes[0] = iWidth * iHeight;
	m_iSizes[1] = iWidth * iHeight / 4;
	m_iSizes[2] = iWidth * iHeight / 16;
	m_iSizes[3] = iWidth * iHeight / 64;

	for (j = 0; j < 4; j++)
	{
		m_iTotalSize += m_iSizes[j];		
	}

	m_iTotalSize += sizeof (short);
	
	m_pbyData = new unsigned char[m_iTotalSize];
	memset (m_pbyData, 0, m_iTotalSize);

	m_wad2Header = (struct wad2_miptex_s *)m_pbyData;	

	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);

	int iHeaderSize = sizeof (struct wad2_miptex_s);
	m_wad2Header->offsets[0] = iHeaderSize;	
	m_wad2Header->offsets[1] = iHeaderSize + m_iSizes[0];
	m_wad2Header->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
	m_wad2Header->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

	for (j = 0; j < 4; j++)
	{
		memcpy ((m_pbyData + m_wad2Header->offsets[j]), pbyBits[j], m_iSizes[j]);
	}

}

/////////////////////////////////////////////////////////////////////////////
//	Name:		~CWAD2Item ()
//	Action:		Deconstructor... clean up!
/////////////////////////////////////////////////////////////////////////////
CWAD2Item::~CWAD2Item ()
{
	if (m_pbyData)
	{
		delete m_pbyData;
		m_pbyData = NULL;
	}
	SetNext (NULL);
	SetPrevious (NULL);
}

unsigned char *CWAD2Item::GetData ()
{
	return m_pbyData;
}

unsigned char *CWAD2Item::GetBits (int iMipNumber)
{
	ASSERT (GetType() == WAD2_TYPE_MIP);
	ASSERT ((iMipNumber >= 0) && (iMipNumber <= 3));
	return (m_pbyData + m_wad2Header->offsets[iMipNumber]);
}

void CWAD2Item::UpdateData (unsigned char *pbyBits[])
{
	ASSERT (GetType() == WAD2_TYPE_MIP);

	for (int j = 0; j < 4; j++)
	{
		memcpy (m_pbyData + m_wad2Header->offsets[j], pbyBits[j], m_iSizes[j]);
	}	
}

void CWAD2Item::ReplaceData (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight)
{
	// Clear out all the data, but keep the linked list pointers intact.
	if (m_pbyData)
	{
		delete m_pbyData;		
	}
	
	m_pbyData		= NULL;	
	m_wad2Header	= NULL;	
	int j = 0;

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}	
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);
	
	m_iTotalSize = sizeof (struct wad2_miptex_s);
	
	m_iSizes[0] = iWidth * iHeight;
	m_iSizes[1] = iWidth * iHeight / 4;
	m_iSizes[2] = iWidth * iHeight / 16;
	m_iSizes[3] = iWidth * iHeight / 64;

	for (j = 0; j < 4; j++)
	{
		m_iTotalSize += m_iSizes[j];		
	}

	m_iTotalSize += sizeof (short);
	
	m_pbyData = new unsigned char[m_iTotalSize];
	memset (m_pbyData, 0, m_iTotalSize);

	m_wad2Header = (struct wad2_miptex_s *)m_pbyData;	

	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);

	int iHeaderSize = sizeof (struct wad2_miptex_s);
	m_wad2Header->offsets[0] = iHeaderSize;	
	m_wad2Header->offsets[1] = iHeaderSize + m_iSizes[0];
	m_wad2Header->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
	m_wad2Header->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

	for (j = 0; j < 4; j++)
	{
		memcpy ((m_pbyData + m_wad2Header->offsets[j]), pbyBits[j], m_iSizes[j]);
	}
}

void CWAD2Item::SetWidth (int iWidth)
{
	ASSERT (GetType() == WAD2_TYPE_MIP);
	m_wad2Header->width = iWidth;
}

int CWAD2Item::GetWidth ()
{
	ASSERT (GetType() == WAD2_TYPE_MIP);
	return m_wad2Header->width;
}

void CWAD2Item::SetHeight (int iHeight)
{
	ASSERT (GetType() == WAD2_TYPE_MIP);
	m_wad2Header->height = iHeight;
}

int CWAD2Item::GetHeight ()
{
	ASSERT (GetType() == WAD2_TYPE_MIP);
	return m_wad2Header->height;
}

void CWAD2Item::SetName (LPCTSTR szName)
{
	ASSERT (strlen(szName) < 16);		// Only 16 chars available, and one has to be the NULL terminator!

	m_strName = szName;
	if (GetType() == WAD2_TYPE_MIP)
	{
		ASSERT (m_wad2Header);
		memset (m_wad2Header->name, 0, 16);
		memcpy (m_wad2Header->name, szName, 15);
	}
}

CString CWAD2Item::GetName ()
{
	//CString strReturn (m_wad2Header->name);
	return m_strName;
}

void CWAD2Item::SetNext (CWAD2Item *pNext)
{
	m_pNext = pNext;
}

CWAD2Item *CWAD2Item::GetNext()
{	
	return m_pNext;
}

void CWAD2Item::SetPrevious (CWAD2Item *pPrevious)
{
	m_pPrevious = pPrevious;
}

CWAD2Item *CWAD2Item::GetPrevious()
{
	return m_pPrevious;
}

int CWAD2Item::GetSize()
{
	return m_iTotalSize;
}

void CWAD2Item::SetType (char cType)
{
	m_cType = cType;
}

char CWAD2Item::GetType ()
{
	return m_cType;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// 	CWAD2List 
////////////////////////////////////////////////////////////////////////////////////////////////////
CWAD2List::CWAD2List ()
{
	SetAll (NULL);
}

CWAD2List::~CWAD2List ()
{
	PurgeList();
}


void CWAD2List::SetFirst (CWAD2Item *pFirst)
{
	m_pFirst = pFirst;
}

CWAD2Item *CWAD2List::GetFirst ()
{
	return m_pFirst;
}

void CWAD2List::SetRead (CWAD2Item *pRead)
{
	m_pRead = pRead;
}

CWAD2Item *CWAD2List::GetRead ()
{
	return m_pRead;
}

void CWAD2List::SetWrite (CWAD2Item *pWrite)
{
	m_pWrite = pWrite;
}

CWAD2Item *CWAD2List::GetWrite ()
{
	return m_pWrite;
}

void CWAD2List::SetAll (CWAD2Item *pItem)
{
	SetFirst (pItem);
	SetRead (pItem);
	SetWrite (pItem);
}

void CWAD2List::PurgeList()
{
	CWAD2Item *pItem = GetFirst();
	CWAD2Item *pItem2 = NULL;

	while (pItem)
	{
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;
	}
	pItem = pItem2 = NULL;
	SetAll (NULL);
}

void CWAD2List::RemoveImage (CWAD2Item *pItem)
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

	
int CWAD2List::GetNumImages ()
{
	// Only return the count of images
	int iCount = 0;

	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->GetType() == WAD2_TYPE_MIP)
		{
			iCount++;
		}
		m_pRead = m_pRead->GetNext();
	}

	return iCount;
}

int CWAD2List::GetNumLumps ()
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

CWAD2Item *CWAD2List::GetFirstImage()
{	
	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->GetType() == WAD2_TYPE_MIP)
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

CWAD2Item *CWAD2List::GetNextImage()
{
	if (GetRead())
	{
		m_pRead = m_pRead->GetNext();		
		while (m_pRead)
		{
			if (m_pRead->GetType() == WAD2_TYPE_MIP)
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

CWAD2Item *CWAD2List::GetFirstLump()
{
	SetRead (GetFirst());	
	return (GetRead());
}

CWAD2Item *CWAD2List::GetNextLump()
{
	if (m_pRead)
	{
		m_pRead = m_pRead->GetNext();
		return m_pRead;
	}
	
	return NULL;
}

CWAD2Item *CWAD2List::AddItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
{
	CWAD2Item *pNewItem = new CWAD2Item (pbyData, iDataSize, cType, szName);
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}
	CString strCompare1 ("");
	CString strCompare2 (szName);
	CWAD2Item *pTempLeft	= NULL;
	CWAD2Item *pTempRight	= NULL;        

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

CWAD2Item *CWAD2List::AddMipItem (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight)
{	
	CWAD2Item *pNewItem = new CWAD2Item ();	
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}	

	pNewItem->CreateAsMip (pbyBits, szName, iWidth, iHeight);	

	CString strCompare1 ("");
	CString strCompare2 (szName);
	CWAD2Item *pTempLeft	= NULL;
	CWAD2Item *pTempRight	= NULL;        

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

bool CWAD2List::IsEmpty()
{
	return (m_pFirst == NULL);
}

int CWAD2List::GetTotalItemsSize()
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

CWAD2Item *CWAD2List::IsNameInList (LPCTSTR szName)
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

int CWAD2List::Serialize (CFile *pFile)
{
	if (IsEmpty())
	{
		ASSERT (false);		// Can't save an empty list
		return IH_WAD_NO_IMAGES;		
	}

	unsigned char *pPackedData	= NULL;
	unsigned char *pBits		= NULL;	
	unsigned char *pWADData		= NULL;
	unsigned char *pLumpData	= NULL;
	
	// The first four bytes of the WAD are its identification.  Here, that's "WAD2"
	char szIdentification[] = "WAD2";
	
	int iTotalSize = 0;
	int iNumLumps = GetNumLumps();
	int j = 0;
	int k = 0;

	// Show the user what's going on
	CProgressBar ctlProgress ("Saving...", 50, iNumLumps + 3, false, 0);
		
	int iHeaderSize = sizeof (struct wad2_headerinfo_s);
	int iInfoSize = sizeof (struct wad2_lumpinfo_s);
	int iMipHeaderSize = sizeof (struct wad2_miptex_s);
	int iFilePosition = 0;
	int iSize	= 0;
	//int iSizes[4];
	int iWidth	= 0;
	int iHeight = 0;

	CWAD2Item *pItem = NULL;
	CString strName("");
	
	// First, we need to determine the overall WAD2 size.
	// A WAD2 is split up like so:
	//
	//			- Beginning of file
	//			- wad2_headerinfo_s
	//			- All of the lumps (each lump (mip) has a header, plus actual pixel values. The overall size varies from mip to mip)
	//			- wad2_lumpinfo_s[] (one lump info for every lump)
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
	m_pwad2Header = (struct wad2_headerinfo_s *)(pPackedData);
	memcpy (m_pwad2Header->identification, szIdentification, 4);
	m_pwad2Header->numlumps = iNumLumps;
	m_pwad2Header->infotableofs = iHeaderSize + iItemsSize;

	// m_pwad2LumpInfo is pointed at the proper offset, and adjusted each time a mip is copied in
	m_pwad2LumpInfo = (struct wad2_lumpinfo_s *)(pPackedData + m_pwad2Header->infotableofs);
	
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
		memset (m_pwad2LumpInfo->name, 0, 16);
		memcpy (m_pwad2LumpInfo->name, (LPCTSTR)strName, strName.GetLength());

		m_pwad2LumpInfo->filepos = iFilePosition;		// Points to where our mip starts
		m_pwad2LumpInfo->disksize = iSize;				// Total size of the mip (header + data).  This is the actual disk storage, which could be less than true size if compression is used
		m_pwad2LumpInfo->size = iSize;					// Total size of the mip (header + data).  This is the true expanded size of the mip
		m_pwad2LumpInfo->type = pItem->GetType();
		m_pwad2LumpInfo->compression = 0x0;				// TODO: compression supported at some point?
		m_pwad2LumpInfo->pad1 = 0x0;
		m_pwad2LumpInfo->pad2 = 0x0;		
	
		pItem = pItem->GetNext();
		m_pwad2LumpInfo++;
		iFilePosition += iSize;
		ctlProgress.StepIt();
	}
	
	pFile->Write (pPackedData, iTotalSize);
	ctlProgress.StepIt();

	m_pwad2LumpInfo = NULL;
	m_pwad2Mip = NULL;
	m_pwad2Header = NULL;
	
	if (pPackedData)
	{
		delete pPackedData;
		pPackedData = NULL;
	}
	ctlProgress.StepIt();
	ctlProgress.Clear();
}