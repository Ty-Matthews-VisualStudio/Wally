	
#ifndef _BUILDLIST_H_
#define _BUILDLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define UNKNOWN_TYPE			-1
#define DUKE3D_TYPE				0
#define SHADOW_WARRIOR_TYPE		1
#define REDNECK_RAMPAGE_TYPE	2
#define BLOOD_TYPE				3


// Build header

#ifndef __ART_DEFINITION_
#define __ART_DEFINITION_

struct Build_Header
{
	int Version;
	int NumTiles;		// Not used anymore
	int TileStart;
	int TileEnd;
};

#endif	// __ART_DEFINITION_


class CWallyPalette;
class CWallyDoc;

class CBuildItem
{
// Members
private:
	BYTE *m_pbyData;	
	CBuildItem *m_pNext;
	CBuildItem *m_pPrevious;	
	int m_iTileNumber;
	int m_iWidth;
	int m_iHeight;
	int m_iARTType;
	int m_iTotalSize;
	int m_iFlags;
	CWallyDoc *m_pWallyDoc;		// This is used to keep track of the image
								// once it's been opened.

public:
	CBuildItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber);
	CBuildItem ();
	~CBuildItem ();

	//void CreateAsMip (BYTE *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight, int iWADType);

	BYTE *GetBits ();	
	void UpdateData (BYTE *pbyBits, int iWidth, int iHeight);
	void ReplaceData (BYTE *pbyBits, int iWidth, int iHeight);
	void SetWidth (int iWidth);
	int GetWidth ();
	void SetHeight (int iHeight);
	int GetHeight ();	
	int GetTileNumber();
	void SetNext (CBuildItem *pNext);
	CBuildItem *GetNext();
	void SetPrevious (CBuildItem *pPrevious);
	CBuildItem *GetPrevious();
	BYTE *GetPalette();
	int GetSize();
	int GetFlags();
	void SetFlags(int iFlags)
	{
		m_iFlags = iFlags;
	}
		
	void SetARTType (int iType);
	int GetARTType ();

	CWallyDoc *GetWallyDoc()
	{
		return m_pWallyDoc;
	}

	void SetWallyDoc (CWallyDoc *pDoc)
	{
		if (pDoc)
		{
			ASSERT (m_pWallyDoc == NULL);		// Better not be trying to open another!
		}
		m_pWallyDoc = pDoc;
	}

	bool HasWallyDoc ()
	{
		return (m_pWallyDoc != NULL);
	}
	
	bool HasValidData()
	{
		return (m_pbyData != NULL);
	}
};

class CBuildList
{
// Members
private:
	CBuildItem *m_pFirst;
	CBuildItem *m_pRead;
	CBuildItem *m_pWrite;
	int m_iARTType;
	int m_iFirstTile;
	int m_iLastTile;

// Methods
public:
	CBuildList ();
	~CBuildList ();

	void SetFirst (CBuildItem *pFirst);
	CBuildItem *GetFirst ();
	void SetRead (CBuildItem *pRead);
	CBuildItem *GetRead ();
	void SetWrite (CBuildItem *pWrite);
	CBuildItem *GetWrite ();
	void SetAll (CBuildItem *pItem);
	void PurgeList();

	int GetFirstTileNumber()
	{
		return m_iFirstTile;
	}
	int GetLastTileNumber()
	{
		return m_iLastTile;
	}

	/*int Serialize (CFile *pFile);	
	int Serialize (LPCTSTR szFileName, BYTE *pbyData, bool bSave = false);*/
	int Serialize (LPCTSTR szFileName, CFile *pFile, BYTE *pbyData, bool bSave = false);
	
	int GetNumTiles ();	
	CBuildItem *GetFirstTile();
	CBuildItem *GetNextTile();
	CBuildItem *GetTile(int iTileNumber);
	
	CBuildItem *AddItem (BYTE *pbyData, int iWidth, int iHeight, int iFlags, int iTileNumber);
	//CBuildItem *AddMipItem (BYTE *pbyBits, CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);	
	void RemoveImage (CBuildItem *pItem);
	
	int GetTotalItemsSize();
	bool IsEmpty();
	//CBuildItem *IsNameInList (LPCTSTR szName);

	void SetARTType (int iType);
	int GetARTType ();	
};

#endif		// _BUILDLIST_H_