/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  WADList.cpp : implementation of the CWADItem and CWADList classes
//
//  Created by Ty Matthews, 12-17-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "WADList.h"
#include "Wally.h"
#include "WallyPal.h"
#include "WadMergeJob.h"
//#include "ProgressBar.h"
#include "MiscFunctions.h"
#include "ColorOpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//	CWADItem implementation
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//	Name:		CWADItem()
//	Action:		Default constructor for class.  Only to be used with a separate
//				call to CreateAsMip()
/////////////////////////////////////////////////////////////////////////////
CWADItem::CWADItem ()
{
	Init();
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		CWADItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
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
CWADItem::CWADItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName, int iWADType)
{	
	Init();

	ASSERT (pbyData);
	ASSERT (iDataSize > 0);
	ASSERT (szName);
	
	SetType (cType);
	SetWADType (iWADType);

	int iWidth[4];
	int iHeight[4];
	int iPaletteOffset = 0;
	int iBugSize = 0;
	int j = 0;

	m_iTotalSize = iDataSize;
	m_pbyData = new unsigned char[m_iTotalSize];
	memcpy (m_pbyData, pbyData, m_iTotalSize);
	m_strName = szName;

	if ((cType == WAD3_TYPE_MIP) && (iWADType == WAD3_TYPE))	
	{
		m_wad3MipHeader = (LPWAD3_MIP)m_pbyData;
		
		m_strName = m_wad3MipHeader->name;
				
		for (j = 0; j < 4; j++)
		{
			iWidth[j] = max ( (int)(m_wad3MipHeader->width / pow (2, j)), 1);
			iHeight[j] = max ( (int)(m_wad3MipHeader->height / pow (2, j)), 1);

			m_iSizes[j] = iWidth[j] * iHeight[j];
		}
				
		iBugSize = sizeof(WAD3_MIP);
		iPaletteOffset = sizeof(WAD3_MIP);
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
			BYTE byHoldPalette[768];

			// Save the palette into another buffer
			memcpy (byHoldPalette, m_pbyPalette, 768);			
			
			// Adjust the total size
			m_iTotalSize += sizeof (WORD);
			
			// Make sure we hang on to the real mip data
			BYTE *pbyHoldData = m_pbyData;
			
			// Go build the proper size
			m_pbyData = NULL;
			m_pbyData = new BYTE[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			// Copy everything over
			memcpy (m_pbyData, pbyHoldData, iDataSize);

			// Reset the pointers
			m_wad3MipHeader = (LPWAD3_MIP)m_pbyData;
			m_pbyPalette = m_pbyData + iPaletteOffset;
			m_pPaletteSize = (WORD *)(m_pbyData + iPaletteOffset - sizeof (unsigned short));
			(*m_pPaletteSize) = 256;
			
			// Copy over the palette
			memcpy (m_pbyPalette, byHoldPalette, 768);			

			// Delete the original buffer
			if (pbyHoldData)
			{
				delete [] pbyHoldData;	// neal - must use array delete to delete arrays!
				pbyHoldData = NULL;
			}			
		}
		else
		{
			m_pbyPalette = m_pbyData + iPaletteOffset;
		}
	}

	if ((cType == WAD2_TYPE_MIP) && (iWADType == WAD2_TYPE))	
	{
		m_wad2MipHeader = (LPWAD2_MIP)m_pbyData;
	
		CString strCompare ("");
		strCompare = szName;
		strCompare.MakeLower();

		if (strCompare == "conchars")
		{
			m_strName = "CONCHARS";
			m_wad2MipHeader = NULL;		// Neal - Quake1 fix?
		}
		else
		{
			m_strName = m_wad2MipHeader->name;

			for (j = 0; j < 4; j++)
			{
				iWidth[j] = max ( (int)(m_wad2MipHeader->width / pow (2, j)), 1);
				iHeight[j] = max ( (int)(m_wad2MipHeader->height / pow (2, j)), 1);

				m_iSizes[j] = iWidth[j] * iHeight[j];
			}			
		}
	}	
}

void CWADItem::Init()
{
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3MipHeader	= NULL;
	m_wad2MipHeader	= NULL;
	m_pPaletteSize	= NULL;
	m_pWallyDoc		= NULL;
	SetNext (NULL);
	SetPrevious (NULL);
	SetType (0);
	m_iWadType = UNKNOWN_TYPE;
	SetSelected(FALSE);
	SetListBoxIndex(LB_ERR);
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
void CWADItem::CreateAsMip (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight, int iWADType)
{
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3MipHeader	= NULL;
	m_wad2MipHeader	= NULL;
	m_pPaletteSize	= NULL;
	SetNext (NULL);
	SetPrevious (NULL);	
	int j = 0;
	int iPaletteOffset = 0;

	SetWADType(iWADType);

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}

	if (iWADType == WAD3_TYPE)
	{
		ASSERT (pPalette);
	}
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);

	m_iTotalSize = 0;
	int iWidths[4];
	int iHeights[4];

	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);

		m_iSizes[j] = iWidths[j] * iHeights[j];
		m_iTotalSize += m_iSizes[j];
	}

	switch (iWADType)
	{
	case WAD2_TYPE:
		{
			SetType (WAD2_TYPE_MIP);
			m_iTotalSize += sizeof (WAD2_MIP);
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad2MipHeader = (LPWAD2_MIP)m_pbyData;

			int iHeaderSize = sizeof (WAD2_MIP);
			m_wad2MipHeader->offsets[0] = iHeaderSize;	
			m_wad2MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad2MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad2MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2];
			
			m_wad2MipHeader->width = iWidth;
			m_wad2MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad2MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}
		break;

	case WAD3_TYPE:
		{
			SetType (WAD3_TYPE_MIP);
			m_iTotalSize += sizeof (WAD3_MIP);
			m_iTotalSize += sizeof (short) + 768 + sizeof (short);
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad3MipHeader = (LPWAD3_MIP)m_pbyData;
			iPaletteOffset = sizeof (WAD3_MIP);
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

			int iHeaderSize = sizeof (WAD3_MIP);
			m_wad3MipHeader->offsets[0] = iHeaderSize;	
			m_wad3MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad3MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad3MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

			m_wad3MipHeader->width = iWidth;
			m_wad3MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad3MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}
		break;

	default:
		ASSERT (false);		// Unhandled WAD type
		break;
	}	

	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);
}

/////////////////////////////////////////////////////////////////////////////
//	Name:		~CWADItem ()
//	Action:		Deconstructor... clean up!
/////////////////////////////////////////////////////////////////////////////
CWADItem::~CWADItem ()
{
	if (m_pbyData)
	{
		delete [] m_pbyData;	// neal - must use array delete to delete arrays!
		m_pbyData = NULL;
	}
	SetNext (NULL);
	SetPrevious (NULL);
}

unsigned char *CWADItem::GetData ()
{
	return m_pbyData;
}

unsigned char *CWADItem::GetBits (int iMipNumber)
{

	ASSERT ((iMipNumber >= 0) && (iMipNumber <= 3));
	
	switch (GetWADType())
	{
	case WAD2_TYPE:
		if (m_wad2MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (GetType() == WAD2_TYPE_MIP);
			return (m_pbyData + m_wad2MipHeader->offsets[iMipNumber]);
		}
		break;

	case WAD3_TYPE:
		if (m_wad3MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (GetType() == WAD3_TYPE_MIP);
			return (m_pbyData + m_wad3MipHeader->offsets[iMipNumber]);
		}
		break;

	default:
		ASSERT (false);
		break;
	}
	return NULL;
}

void CWADItem::UpdateData (unsigned char *pbyBits[], CWallyPalette *pPalette, int iWidth, int iHeight)
{
	int iCurrentWidth	= GetWidth();
	int iCurrentHeight	= GetHeight();	
	CString strName (GetName());

	if ((iCurrentWidth != iWidth) || (iCurrentHeight != iHeight))
	{
		// Rebuild the MIP from scratch
		ReplaceData (pbyBits, pPalette, strName, iWidth, iHeight);
		return;
	}

	switch (GetWADType())
	{
	case WAD2_TYPE:
		{
			ASSERT (GetType() == WAD2_TYPE_MIP);
			for (int j = 0; j < 4; j++)
			{
				if (pbyBits[j])
				{
					memcpy (m_pbyData + m_wad2MipHeader->offsets[j], pbyBits[j], m_iSizes[j]);
				}
			}
		}
		break;

	case WAD3_TYPE:
		{
			ASSERT (GetType() == WAD3_TYPE_MIP);			
			int j = 0;
			for (j = 0; j < 4; j++)
			{
				if (pbyBits[j])
				{
					memcpy (m_pbyData + m_wad3MipHeader->offsets[j], pbyBits[j], m_iSizes[j]);
				}
			}	
			
			if (pPalette)
			{
				for (j = 0; j < 256; j++)
				{
					m_pbyPalette[j * 3 + 0] = pPalette->GetR(j);
					m_pbyPalette[j * 3 + 1] = pPalette->GetG(j);
					m_pbyPalette[j * 3 + 2] = pPalette->GetB(j);
				}
			}
		}
		break;

	default:
		ASSERT (false);
		break;
	}
}

void CWADItem::ReplaceData (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight)
{
	// Clear out all the data, but keep the linked list pointers intact.
	if (m_pbyData)
	{
		delete [] m_pbyData;	// neal - must use array delete to delete arrays!
	}
	
	m_pbyData		= NULL;
	m_pbyPalette	= NULL;
	m_wad3MipHeader	= NULL;
	m_wad2MipHeader	= NULL;
	m_pPaletteSize	= NULL;	
	int iPaletteOffset = 0;
	m_iTotalSize = 0;
	int j = 0;

	int iWADType = GetWADType();

	for (j = 0; j < 4; j++)
	{
		ASSERT (pbyBits[j]);
	}

	if (iWADType == WAD3_TYPE)
	{
		ASSERT (pPalette);
	}
	ASSERT (szName);
	ASSERT (iWidth > 0);
	ASSERT (iHeight > 0);

	int iWidths[4];
	int iHeights[4];

	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);

		m_iSizes[j] = iWidths[j] * iHeights[j];
		m_iTotalSize += m_iSizes[j];
	}

	switch (GetWADType())
	{
	case WAD2_TYPE:
		{
			m_iTotalSize += sizeof (WAD2_MIP);

			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad2MipHeader = (LPWAD2_MIP)m_pbyData;

			int iHeaderSize = sizeof (WAD2_MIP);
			m_wad2MipHeader->offsets[0] = iHeaderSize;	
			m_wad2MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad2MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad2MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

			m_wad2MipHeader->width = iWidth;
			m_wad2MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad2MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}
		break;

	case WAD3_TYPE:
		{
			m_iTotalSize += sizeof (WAD3_MIP);

			m_iTotalSize += sizeof (short) + 768 + sizeof (short);
			
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad3MipHeader = (LPWAD3_MIP)m_pbyData;
			
			iPaletteOffset = sizeof (WAD3_MIP);
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

			int iHeaderSize = sizeof (WAD3_MIP);
			m_wad3MipHeader->offsets[0] = iHeaderSize;	
			m_wad3MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad3MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad3MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

			m_wad3MipHeader->width = iWidth;
			m_wad3MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad3MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}

		break;

	default:
		ASSERT (false);
		break;
	}	


	SetName (szName);
	SetWidth (iWidth);
	SetHeight (iHeight);
}

void CWADItem::SetWidth (int iWidth)
{
	ASSERT (IsValidMip());

	switch (GetWADType())
	{
	case WAD2_TYPE:
		m_wad2MipHeader->width = iWidth;
		break;

	case WAD3_TYPE:
		m_wad3MipHeader->width = iWidth;
		break;

	default:
		ASSERT (false);
		break;
	}	
}

int CWADItem::GetWidth ()
{
	switch (GetWADType())
	{
	case WAD2_TYPE:
		if (m_wad2MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (IsValidMip());
			return m_wad2MipHeader->width;
		}
		break;

	case WAD3_TYPE:
		if (m_wad3MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (IsValidMip());
			return m_wad3MipHeader->width;
		}
		break;

	default:
		ASSERT (false);
		break;
	}
	return 0;
}

void CWADItem::SetHeight (int iHeight)
{
	ASSERT (IsValidMip());

	switch (GetWADType())
	{
	case WAD2_TYPE:
		m_wad2MipHeader->height = iHeight;
		break;

	case WAD3_TYPE:
		m_wad3MipHeader->height = iHeight;
		break;

	default:
		ASSERT (false);
		break;
	}	
}

int CWADItem::GetHeight ()
{
	switch (GetWADType())
	{
	case WAD2_TYPE:
		if (m_wad2MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (IsValidMip());
			return m_wad2MipHeader->height;
		}
		break;

	case WAD3_TYPE:
		if (m_wad3MipHeader)		// Neal - fixe Quake1?
		{
			ASSERT (IsValidMip());
			return m_wad3MipHeader->height;
		}
		break;

	default:
		ASSERT (false);
		break;
	}
	return 0;
}

void CWADItem::SetName (LPCTSTR szName)
{
	ASSERT (strlen(szName) < 16);		// Only 16 chars available, and one has to be the NULL terminator!

	m_strName = szName;

	switch (GetWADType())
	{
	case WAD2_TYPE:
		if (GetType() == WAD2_TYPE_MIP)
		{
			ASSERT (m_wad2MipHeader);

			memset (m_wad2MipHeader->name, 0, 16);
			memcpy (m_wad2MipHeader->name, szName, 15);
		}
		break;

	case WAD3_TYPE:
		if (GetType() == WAD3_TYPE_MIP)
		{
			ASSERT (m_wad3MipHeader);

			memset (m_wad3MipHeader->name, 0, 16);
			memcpy (m_wad3MipHeader->name, szName, 15);
		}
		break;

	default:
		ASSERT (false);
		break;
	}
}

CString CWADItem::GetName ()
{	
	return m_strName;
}

void CWADItem::SetNext (CWADItem *pNext)
{
	m_pNext = pNext;
}

CWADItem *CWADItem::GetNext()
{	
	return m_pNext;
}
void CWADItem::SetPrevious (CWADItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CWADItem *CWADItem::GetPrevious()
{
	return m_pPrevious;
}

unsigned char *CWADItem::GetPalette()
{
	switch (GetWADType())
	{
	case WAD2_TYPE:
		return quake1_pal;
		break;

	case WAD3_TYPE:
		return m_pbyPalette;
		break;

	default:
		ASSERT (false);
		break;
	}

	return NULL;	
}

int CWADItem::GetSize()
{
	return m_iTotalSize;
}

void CWADItem::SetType (char cType)
{
	m_cType = cType;
}

char CWADItem::GetType ()
{
	return m_cType;
}


void CWADItem::SetWADType (int iType)
{
	ASSERT ((iType == WAD2_TYPE) || (iType == WAD3_TYPE));
	m_iWadType = iType;
}

int CWADItem::GetWADType ()
{
	return m_iWadType;
}

void CWADItem::RebuildSubMips()
{
	if (HasWallyDoc())
	{
		// Don't do anything; assume the user knows about the sub-mips already
		return;
	}
	
	int iWidth  = GetWidth();
	int iHeight = GetHeight();
	int iHeaderSize = sizeof( q2_miptex_s);
	CWallyPalette Palette;

	Palette.SetPalette(GetPalette(), 256);

	// 512 is to reserve room for 512 byte header (max)
//	#define MAX_DATA_SIZE (MAX_MIP_SIZE * MAX_MIP_SIZE + 512)

	int iSizes[4];
	int iMipSize = iHeaderSize;
	int j = 0;

	int iWidths[4];
	int iHeights[4];

	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);

		iSizes[j] = iWidths[j] * iHeights[j];
		iMipSize += iSizes[j];
	}

	BYTE* pbyMipData = new BYTE [iMipSize];
	CString strError("");

	if (!pbyMipData)
	{
		strError.Format ("Out of memory during ReMip of %s", GetName());
		AfxMessageBox (strError, MB_ICONSTOP);
		return;
	}
	BYTE* pbyItemData[4];

	for (j = 0; j < 4; j++)
	{
		pbyItemData[j] = GetBits(j);
	}
	
	q2_miptex_s *pMipHeader = (q2_miptex_s *)pbyMipData;
	
	pMipHeader->offsets[0] = iHeaderSize;
	pMipHeader->offsets[1] = iHeaderSize + iSizes[0];
	pMipHeader->offsets[2] = iHeaderSize + iSizes[0] + iSizes[1];
	pMipHeader->offsets[3] = iHeaderSize + iSizes[0] + iSizes[1] + iSizes[2];
		
	memcpy( pbyMipData + iHeaderSize, pbyItemData[0], iSizes[0]);

	pMipHeader->width  = iWidth;
	pMipHeader->height = iHeight;

	///////////////////////////////
	// Rebuild all the sub-mips //
	/////////////////////////////

	Palette.RebuildWAL( pbyMipData);

	/////////////////////////
	// copy the data back //
	///////////////////////

	for (j = 1; j < 4; j++)
	{
		memcpy (pbyItemData[j], pbyMipData + (pMipHeader->offsets[j]), iSizes[j]);
	}
	

	pMipHeader = NULL;

	if (pbyMipData)
	{
		delete [] pbyMipData;
		pbyMipData = NULL;
	}	
}

BOOL CWADItem::ConvertToType( int iWADType )
{
	int iWidth = 0;
	int iHeight = 0;
	int iTotalSize = 0;
	int iHeaderSize = 0;
	int j = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	LPBYTE pbyNewBuffer = NULL;
	LPBYTE pbyImageData = GetBits( 0 );
	LPBYTE pbyPalette = GetPalette();
	LPBYTE pbyNewMipData = NULL;

	iWidth = GetWidth();
	iHeight = GetHeight();
	
	// This is our current type
	switch( GetWADType() )
	{
	case WAD3_TYPE:
		{
			switch( iWADType )
			{
			case WAD3_TYPE:
				{
					return TRUE;
				}
				break;

			case WAD2_TYPE:
				{
					SetType( WAD2_TYPE_MIP );
					SetWADType( WAD2_TYPE );

					iHeaderSize = sizeof( WAD2_MIP );
					iTotalSize = iHeaderSize;
					for( j = 0; j < 4; j++ )
					{
						iTotalSize += m_iSizes[j];
					}					

					pbyNewBuffer = new BYTE[ iTotalSize ];

					if( !pbyNewBuffer )
					{
						return FALSE;
					}

					m_wad2MipHeader = (LPWAD2_MIP)pbyNewBuffer;
					
					m_wad2MipHeader->offsets[0] = iHeaderSize;
					m_wad2MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
					m_wad2MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
					m_wad2MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2];

					pbyNewMipData = pbyNewBuffer + m_wad2MipHeader->offsets[0];

					SetName( m_wad3MipHeader->name );
					SetWidth( iWidth );
					SetHeight( iHeight );

					// Now we have to convert down to the Quake1 palette
					CColorOptimizer ColorOpt;
					CWallyPalette Palette;					

					COLOR_IRGB* pTemp24Bit = new COLOR_IRGB [ m_iSizes[0] ];
					if( !pTemp24Bit )
					{
						if( pbyNewBuffer )
						{
							delete []pbyNewBuffer;
						}
						return FALSE;
					}

					for( j = 0; j < m_iSizes[0]; j++ )
					{
						r = pbyPalette[ pbyImageData[j] * 3 + 0 ];
						g = pbyPalette[ pbyImageData[j] * 3 + 1 ];
						b = pbyPalette[ pbyImageData[j] * 3 + 2 ];
						
						pTemp24Bit[j] = IRGB( 0, r, g, b );						
					}

					Palette.SetPalette( quake1_pal, 256 );
					Palette.Convert24BitTo256Color( pTemp24Bit, pbyNewMipData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					if( pTemp24Bit )
					{
						delete []pTemp24Bit;
						pTemp24Bit = NULL;
					}

					// Wipe out the old stuff
					m_pbyPalette = NULL;
					if( m_pbyData )
					{
						delete []m_pbyData;
					}
					
					m_pbyData = pbyNewBuffer;
					m_iTotalSize = iTotalSize;
					m_wad3MipHeader = NULL;
					m_pPaletteSize = NULL;

					RebuildSubMips();
				}
				break;

			default:
				ASSERT( FALSE );
				return FALSE;
				break;
			}
		}
		break;

	case WAD2_TYPE:
		{
			switch( iWADType )
			{
			case WAD3_TYPE:
				{
					SetType( WAD3_TYPE_MIP );
					SetWADType( WAD3_TYPE );

					iHeaderSize = sizeof( WAD3_MIP );
					iTotalSize = iHeaderSize;
					for( j = 0; j < 4; j++ )
					{
						iTotalSize += m_iSizes[j];
					}
										
					iTotalSize += sizeof (short) + 768 + sizeof (short);
					pbyNewBuffer = new BYTE[ iTotalSize ];
					
					if( !pbyNewBuffer )
					{
						return FALSE;
					}

					m_wad3MipHeader = (LPWAD3_MIP)pbyNewBuffer;
					int iPaletteOffset = sizeof( WAD3_MIP );
					for( j = 0; j < 4; j++ )
					{
						iPaletteOffset += m_iSizes[j];
					}
					m_pPaletteSize = (unsigned short *)( pbyNewBuffer + iPaletteOffset );
					(*m_pPaletteSize) = 256;

					iPaletteOffset += sizeof (short);	
					m_pbyPalette = pbyNewBuffer + iPaletteOffset;

					memcpy( m_pbyPalette, quake1_pal, 768 );

					m_wad3MipHeader->offsets[0] = iHeaderSize;
					m_wad3MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
					m_wad3MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
					m_wad3MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2];

					pbyNewMipData = pbyNewBuffer + m_wad3MipHeader->offsets[0];

					memcpy( pbyNewMipData, pbyImageData, m_iSizes[0] );

					SetName( m_wad2MipHeader->name );
					SetWidth( iWidth );
					SetHeight( iHeight );

					if( m_pbyData )
					{
						delete []m_pbyData;
					}
					
					m_pbyData = pbyNewBuffer;
					m_iTotalSize = iTotalSize;
					m_wad2MipHeader = NULL;
					
					RebuildSubMips();					
				}
				break;

			case WAD2_TYPE:
				{
					return TRUE;
				}
				break;

			default:
				ASSERT( FALSE );
				return FALSE;
				break;
			}
		}
		break;

	default:
		ASSERT( FALSE );
		return FALSE;
		break;
	}

	return TRUE;
}

/*

  	switch (iWADType)
	{
	case WAD2_TYPE:
		{
			SetType (WAD2_TYPE_MIP);
			m_iTotalSize += sizeof (WAD2_MIP);
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad2MipHeader = (LPWAD2_MIP)m_pbyData;

			int iHeaderSize = sizeof (WAD2_MIP);
			m_wad2MipHeader->offsets[0] = iHeaderSize;	
			m_wad2MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad2MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad2MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2];
			
			m_wad2MipHeader->width = iWidth;
			m_wad2MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad2MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}
		break;

	case WAD3_TYPE:
		{
			SetType (WAD3_TYPE_MIP);
			m_iTotalSize += sizeof (WAD3_MIP);
			m_iTotalSize += sizeof (short) + 768 + sizeof (short);
			m_pbyData = new unsigned char[m_iTotalSize];
			memset (m_pbyData, 0, m_iTotalSize);

			m_wad3MipHeader = (LPWAD3_MIP)m_pbyData;
			iPaletteOffset = sizeof (WAD3_MIP);
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

			int iHeaderSize = sizeof (WAD3_MIP);
			m_wad3MipHeader->offsets[0] = iHeaderSize;	
			m_wad3MipHeader->offsets[1] = iHeaderSize + m_iSizes[0];
			m_wad3MipHeader->offsets[2] = iHeaderSize + m_iSizes[0] + m_iSizes[1];
			m_wad3MipHeader->offsets[3] = iHeaderSize + m_iSizes[0] + m_iSizes[1] + m_iSizes[2]; 

			m_wad3MipHeader->width = iWidth;
			m_wad3MipHeader->height = iHeight;

			for (j = 0; j < 4; j++)
			{
				memcpy ((m_pbyData + m_wad3MipHeader->offsets[j]), pbyBits[j], m_iSizes[j]);
			}
		}


*/

void CWADItem::AddPackageEntry( CPackageTreeEntryBase *pBase )
{
	m_PackageTreeEntryVector.push_back( pBase );
}

void CWADItem::GetPackageEntries( _PackageTreeEntryVector &PackageTreeEntryVector )
{
	_itPackageTreeEntry itPackageTreeEntry;
	itPackageTreeEntry = m_PackageTreeEntryVector.begin();
	while( itPackageTreeEntry != m_PackageTreeEntryVector.end() )
	{
		PackageTreeEntryVector.push_back( (*itPackageTreeEntry) );
		itPackageTreeEntry++;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// 	CWADList 
////////////////////////////////////////////////////////////////////////////////////////////////////
CWADList::CWADList ()
{
	m_iWadType = UNKNOWN_TYPE;
	SetAll (NULL);
}

CWADList::~CWADList ()
{
	PurgeList();
}


void CWADList::SetFirst (CWADItem *pFirst)
{
	m_pFirst = pFirst;
}

CWADItem *CWADList::GetFirst ()
{
	return m_pFirst;
}

void CWADList::SetRead (CWADItem *pRead)
{
	m_pRead = pRead;
}

CWADItem *CWADList::GetRead ()
{
	return m_pRead;
}

void CWADList::SetWrite (CWADItem *pWrite)
{
	m_pWrite = pWrite;
}

CWADItem *CWADList::GetWrite ()
{
	return m_pWrite;
}

void CWADList::SetAll (CWADItem *pItem)
{
	SetFirst (pItem);
	SetRead (pItem);
	SetWrite (pItem);
}

void CWADList::PurgeList()
{
	CWADItem *pItem = GetFirst();
	CWADItem *pItem2 = NULL;

	while (pItem)
	{
		pItem2 = pItem->GetNext();
		delete pItem;
		pItem = pItem2;
	}
	pItem = pItem2 = NULL;
	SetAll (NULL);
}

void CWADList::RemoveImage (CWADItem *pItem, BOOL bDelete /* = TRUE */)
{
	CWADItem *pTemp = GetFirst();
	
	while( pTemp )
	{		
		if( pTemp == pItem )
		{
			if (pTemp->GetPrevious())
			{
				pTemp->GetPrevious()->SetNext (pTemp->GetNext());
			}

			if (pTemp->GetNext())
			{
				pTemp->GetNext()->SetPrevious (pTemp->GetPrevious());
			}

			if (pTemp == m_pFirst)
			{
				SetFirst (pTemp->GetNext());
			}

			if (pTemp == m_pWrite)
			{
				SetWrite (pTemp->GetPrevious());
			}

			if (pTemp == m_pRead)
			{
				SetRead( pTemp->GetNext());
			}

			pTemp->SetPrevious (NULL);
			pTemp->SetNext (NULL);

			if( bDelete )
			{
				delete pTemp;				
			}
			pTemp = NULL;
		}
		else
		{
			pTemp = pTemp->GetNext();
		}
	}
}

	
int CWADList::GetNumImages ()
{
	// Only return the count of images
	int iCount = 0;

	CWADItem *pTemp = GetFirst();

	while (pTemp)
	{
		if (pTemp->IsValidMip())
		{
			iCount++;
		}
		pTemp = pTemp->GetNext();
	}

	return iCount;
}

int CWADList::GetNumLumps ()
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

CWADItem *CWADList::GetFirstImage()
{	
	SetRead (GetFirst());

	while (m_pRead)
	{
		if (m_pRead->IsValidMip())
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

CWADItem *CWADList::GetNextImage()
{
	if (GetRead())
	{
		m_pRead = m_pRead->GetNext();		
		while (m_pRead)
		{
			if (m_pRead->IsValidMip())
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

CWADItem *CWADList::GetFirstLump()
{
	SetRead (GetFirst());	
	return (GetRead());
}

CWADItem *CWADList::GetNextLump()
{
	if (m_pRead)
	{
		m_pRead = m_pRead->GetNext();
		return m_pRead;
	}
	
	return NULL;
}

CWADItem *CWADList::GetLumpAtPosition(int iPosition)
{
	int iCount = 0;
	SetRead (GetFirst());

	while (m_pRead && (iCount != iPosition))
	{
		m_pRead = m_pRead->GetNext();
		iCount++;
	}
	
	return (GetRead());
}

void CWADList::DiscoverWidthHeight( LPBYTE pbyData, int iDataSize, char cType, LPDWORD pdwWidth, LPDWORD pdwHeight )
{
	ASSERT( pbyData );
	ASSERT( pdwWidth );
	ASSERT( pdwHeight );

	if ((cType == WAD3_TYPE_MIP) && (GetWADType() == WAD3_TYPE))	
	{
		if( iDataSize < sizeof( wad3_miptex_s ))
		{
			return;
		}

		LPWAD3_MIP pHeader = (LPWAD3_MIP)pbyData;
		
		(*pdwWidth) = pHeader->width;
		(*pdwHeight) = pHeader->height;
	}
		
	if ((cType == WAD2_TYPE_MIP) && (GetWADType() == WAD2_TYPE))	
	{
		if( iDataSize < sizeof( wad2_miptex_s ))
		{
			return;
		}

		LPWAD2_MIP pHeader = (LPWAD2_MIP)pbyData;
		
		(*pdwWidth) = pHeader->width;
		(*pdwHeight) = pHeader->height;
	}
}

CWADItem *CWADList::AddItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName)
{
	ASSERT (GetWADType() != UNKNOWN_TYPE);		// Have to set the WAD type before adding anything!
	
	CWADItem *pNewItem = new CWADItem (pbyData, iDataSize, cType, szName, GetWADType());
	
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}

	AddToList( pNewItem );
	return pNewItem;
}

void CWADList::AddToList( CWADItem *pItem )
{	
	if (!GetFirst())
	{
		SetAll( pItem );
	}
	else
	{
		m_pWrite->SetNext( pItem );
		pItem->SetPrevious( m_pWrite );
		m_pWrite = pItem;
	}	
}

CWADItem *CWADList::AddMipItem (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight)
{	
	CWADItem *pNewItem = new CWADItem ();	
	if (!pNewItem)
	{
		ASSERT (false);
		return NULL;
	}	

	pNewItem->CreateAsMip (pbyBits, pPalette, szName, iWidth, iHeight, GetWADType());
	AddToList( pNewItem );
	
	return pNewItem;
}

bool CWADList::IsEmpty()
{
	return (m_pFirst == NULL);
}

int CWADList::GetTotalItemsSize()
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

CWADItem *CWADList::IsNameInList (LPCTSTR szName)
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


void CWADList::SetWADType (int iType)
{
	ASSERT ((iType == WAD2_TYPE) || (iType == WAD3_TYPE));
	m_iWadType = iType;
}

int CWADList::GetWADType ()
{
	return m_iWadType;
}


/*int CWADList::Serialize (CFile *pFile)
{
	int iLength = pFile->GetLength();
	m_pEncodedData = new unsigned char[iLength];
	pFile->Read (m_pEncodedData, iLength);

	int iReturn = Serialize ("", m_pEncodedData, false);
	
	if (m_pEncodedData)
	{
		delete m_pEncodedData;
		m_pEncodedData = NULL;
	}
	return iReturn;
}*/

DWORD CWADList::GetNumImages( CWadMergeJob *pMergeJob )
{
	if( !pMergeJob )
	{
		ASSERT( FALSE );
		return 0;
	}

	char szWadType[5]	= { 0, 0, 0, 0 ,0 };
	BYTE byHeader[12];
	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, pMergeJob->GetWadFile(), "rb");
	
	if( err == 0 )
	{
		fread( byHeader, 1, sizeof( byHeader ), fp );
		fclose( fp );

		memcpy_s( szWadType, sizeof(szWadType), byHeader, 4);

		if( !strcmp(szWadType, "WAD2") )
		{
			struct wad2_headerinfo_s *lpHeader = (struct wad2_headerinfo_s *)byHeader;
			return lpHeader->numlumps;
		}

		if( !strcmp(szWadType, "WAD3") )
		{
			struct wad3_headerinfo_s *lpHeader = (struct wad3_headerinfo_s *)byHeader;
			return lpHeader->numlumps;
		}
	}

	return 0;
}

int CWADList::Serialize( CWadMergeJob *pMergeJob )
{
	if( !pMergeJob )
	{
		ASSERT( FALSE );
		return IH_ERROR_READING_FILE;
	}
	
	CSafeFile ReadFile;
	CWADItem *pNewItem = NULL;
	
	LPBYTE pbyFileData = NULL;
	LPBYTE pLumpData	= NULL;
	LPBYTE pbyPalette	= NULL;

	CString strName ("");	
	CStringArray saWildCards;
	
	int j = 0;
	int k = 0;
	int iOffset			= 0;
	int iLastMipSize	= 0;
	int iSize			= 0;

	DWORD dwWidth		= 0;
	DWORD dwHeight		= 0;
	
	char szWadType[5]	= { 0, 0, 0, 0 ,0 };
	char cType			= 0;
	char *szName		= NULL;
	char *szWildCard	= NULL;

	if( !ReadFile.Open( pMergeJob->GetWadFile() ))
	{
		return IH_ERROR_READING_FILE;
	}

	szWildCard = pMergeJob->GetWildcard();
	GetAllWildCards( szWildCard, &saWildCards );
	
	pbyFileData = ReadFile.GetBuffer();	
	memcpy( szWadType, pbyFileData, 4);

	if( !strcmp(szWadType, "WAD2") )
	{
		SetWADType (WAD2_TYPE);			
	}

	if( !strcmp(szWadType, "WAD3") )
	{
		SetWADType (WAD3_TYPE);
	}
		
	switch (GetWADType())
	{
	case WAD2_TYPE:
		{
			m_pwad2Header = (wad2_headerinfo_s *)(pbyFileData);			
			m_pwad2LumpInfo = (struct wad2_lumpinfo_s *)(pbyFileData + m_pwad2Header->infotableofs);

			for (j = 0; j < m_pwad2Header->numlumps; j++)
			{				
				szName = m_pwad2LumpInfo->name;
				dwWidth	= 0;
				dwHeight = 0;

				if( MatchesWildCard( szName, &saWildCards ))
				{
					iOffset = m_pwad2LumpInfo->filepos;
					pLumpData = pbyFileData + iOffset;
					iSize = m_pwad2LumpInfo->disksize;
					cType = m_pwad2LumpInfo->type;

					DiscoverWidthHeight( pLumpData, iSize, cType, &dwWidth, &dwHeight );

					if( (dwWidth > 0) && (dwHeight > 0) )
					{
						if( pMergeJob->MeetsMinMaxRestrictions( dwWidth, dwHeight) )
						{
							pNewItem = AddItem (pLumpData, iSize, cType, szName);
						}
					}
				}
				m_pwad2LumpInfo++;		
			}	

		}
		break;

	case WAD3_TYPE:
		{
			m_pwad3Header = (wad3_headerinfo_s *)(pbyFileData);		
			m_pwad3LumpInfo = (struct wad3_lumpinfo_s *)(pbyFileData + m_pwad3Header->infotableofs);

			for (j = 0; j < m_pwad3Header->numlumps; j++)
			{
				szName = m_pwad3LumpInfo->name;
				dwWidth	= 0;
				dwHeight = 0;

				if( MatchesWildCard( szName, &saWildCards ))
				{
					iOffset = m_pwad3LumpInfo->filepos;
					pLumpData = pbyFileData + iOffset;
					iSize = m_pwad3LumpInfo->disksize;
					cType = m_pwad3LumpInfo->type;

					DiscoverWidthHeight( pLumpData, iSize, cType, &dwWidth, &dwHeight );

					if( (dwWidth > 0) && (dwHeight > 0) )
					{
						if( pMergeJob->MeetsMinMaxRestrictions( dwWidth, dwHeight) )
						{
							pNewItem = AddItem (pLumpData, iSize, cType, szName);
						}
					}					
				}
				m_pwad3LumpInfo++;		
			}	

		}
		break;

	default:
		return IH_WAD_UNSUPPORTED_VERSION;
		break;
	}

	return IH_SUCCESS;
}

int CWADList::Serialize (LPCTSTR szFileName, CFile *pFile, unsigned char *pbyData, bool bSave /* = false */)
{	
	unsigned char *pbyFileData = NULL;
	
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
			pbyFileData = new unsigned char[iLength];
			fp.Read (pbyFileData, iLength);
			fp.Close();
		}

		if (pFile)
		{
			ASSERT (!pbyData);
			ASSERT (!szFileName);

			int iLength = pFile->GetLength();
			pbyFileData = new unsigned char[iLength];
			pFile->Read (pbyFileData, iLength);			
		}
		
		if (pbyData)
		{
			ASSERT (!szFileName);
			ASSERT (!pFile);
			pbyFileData = pbyData;
		}
		
		char szWadType[5];
		memset (szWadType, 0, 5);
		memcpy (szWadType, pbyFileData, 4);

		if (!strcmp(szWadType, "WAD2"))
		{
			SetWADType (WAD2_TYPE);			
		}

		if (!strcmp(szWadType, "WAD3"))
		{
			SetWADType (WAD3_TYPE);
		}
			
		switch (GetWADType())
		{
		case WAD2_TYPE:
			{
				m_pwad2Header = (wad2_headerinfo_s *)(pbyFileData);
				unsigned char *pLumpData = NULL;
				unsigned char *pbyPalette = NULL;
				CString strName ("");
				int j = 0;
				int k = 0;
				int iOffset			= 0;
				int iWidth			= 0;
				int iHeight			= 0;
				int iLastMipSize	= 0;
				int iSize			= 0;
				char cType			= 0;
				char *szName		= NULL;
				
				m_pwad2LumpInfo = (struct wad2_lumpinfo_s *)(pbyFileData + m_pwad2Header->infotableofs);

				CProgressBar ctlProgress ("Decoding...", 50, m_pwad2Header->numlumps, false, 0);

				for (j = 0; j < m_pwad2Header->numlumps; j++)
				{		
					iOffset = m_pwad2LumpInfo->filepos;
					pLumpData = pbyFileData + iOffset;
					iSize = m_pwad2LumpInfo->disksize;
					cType = m_pwad2LumpInfo->type;
					
					szName = m_pwad2LumpInfo->name;

					AddItem (pLumpData, iSize, cType, szName);
					ctlProgress.StepIt();	
					m_pwad2LumpInfo++;		
				}	

				ctlProgress.Clear();
			}
			break;

		case WAD3_TYPE:
			{
				m_pwad3Header = (wad3_headerinfo_s *)(pbyFileData);
				unsigned char *pLumpData = NULL;
				unsigned char *pbyPalette = NULL;
			
				CString strName ("");
				int j = 0;
				int k = 0;
				int iOffset			= 0;
				int iWidth			= 0;
				int iHeight			= 0;
				int iLastMipSize	= 0;
				int iSize			= 0;
				char cType			= 0;
				char *szName		= NULL;
				
				m_pwad3LumpInfo = (struct wad3_lumpinfo_s *)(pbyFileData + m_pwad3Header->infotableofs);

				CProgressBar ctlProgress ("Decoding...", 50, m_pwad3Header->numlumps, false, 0);

				for (j = 0; j < m_pwad3Header->numlumps; j++)
				{		
					iOffset = m_pwad3LumpInfo->filepos;
					pLumpData = pbyFileData + iOffset;
					iSize = m_pwad3LumpInfo->disksize;
					cType = m_pwad3LumpInfo->type;
					szName = m_pwad3LumpInfo->name;

					AddItem (pLumpData, iSize, cType, szName);
					ctlProgress.StepIt();	
					m_pwad3LumpInfo++;		
				}	

				ctlProgress.Clear();
			}
			break;

		default:
			return IH_WAD_UNSUPPORTED_VERSION;
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

		int iTotalSize = 0;
		int iNumLumps = GetNumLumps();
		int j = 0;
		int k = 0;

		// Show the user what's going on
		CProgressBar ctlProgress ("Saving...", 50, iNumLumps + 3, false, 0);
				

		switch (GetWADType())
		{
		case WAD2_TYPE:
			{				
				// The first four bytes of the WAD are its identification.  Here, that's "WAD2"
				char szIdentification[] = "WAD2";
					
				int iHeaderSize = sizeof (struct wad2_headerinfo_s);
				int iInfoSize = sizeof (struct wad2_lumpinfo_s);
				int iMipHeaderSize = sizeof (WAD2_MIP);
				int iFilePosition = 0;
				int iSize	= 0;				
				int iWidth	= 0;
				int iHeight = 0;

				CWADItem *pItem = NULL;
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
				pPackedData = new BYTE[iTotalSize];
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
					memcpy (m_pwad2LumpInfo->name, (LPCTSTR)strName, min (strName.GetLength(), 16));

					m_pwad2LumpInfo->filepos = iFilePosition;		// Points to where our mip starts
					m_pwad2LumpInfo->disksize = iSize;				// Total size of the mip (header + data).  This is the actual disk storage, which could be less than true size if compression is used
					m_pwad2LumpInfo->size = iSize;					// Total size of the mip (header + data).  This is the true expanded size of the mip
					m_pwad2LumpInfo->type = pItem->GetType();
					m_pwad2LumpInfo->compression = 0x0;				// TODO: compression supported at some point?
					m_pwad2LumpInfo->pad = 0x0;					
				
					pItem = pItem->GetNext();
					m_pwad2LumpInfo++;
					iFilePosition += iSize;
					ctlProgress.StepIt();
				}
				
				pSaveFile->Write (pPackedData, iTotalSize);
				ctlProgress.StepIt();

				m_pwad2LumpInfo = NULL;
				m_pwad2Mip = NULL;
				m_pwad2Header = NULL;
				
				if (pPackedData)
				{
					delete [] pPackedData;	// neal - must use array delete to delete arrays!
					pPackedData = NULL;
				}
				ctlProgress.StepIt();
				ctlProgress.Clear();
			}
			break;

		case WAD3_TYPE:
			{				
				short *psPaletteSize = NULL;

				// The first four bytes of the WAD are its identification.  Here, that's "WAD3"
				char szIdentification[] = "WAD3";				
					
				int iHeaderSize = sizeof (struct wad3_headerinfo_s);
				int iInfoSize = sizeof (struct wad3_lumpinfo_s);
				int iMipHeaderSize = sizeof (WAD3_MIP);
				int iFilePosition = 0;
				int iSize	= 0;				
				int iWidth	= 0;
				int iHeight = 0;

				CWADItem *pItem = NULL;
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
					memcpy (m_pwad3LumpInfo->name, (LPCTSTR)strName, min (strName.GetLength(), 16));

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
				
				pSaveFile->Write (pPackedData, iTotalSize);
				ctlProgress.StepIt();

				m_pwad3LumpInfo = NULL;
				m_pwad3Mip = NULL;
				m_pwad3Header = NULL;
				
				if (pPackedData)
				{
					delete [] pPackedData;
					pPackedData = NULL;
				}
				ctlProgress.StepIt();
				ctlProgress.Clear();
			}
			break;

		default:
			ASSERT (false);
			break;
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