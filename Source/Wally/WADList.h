	
#ifndef _WADLIST_H_
#define _WADLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Forward declarations
class CWallyPalette;
class CWallyDoc;
class CWadMergeJob;

#define UNKNOWN_TYPE	-1
#define WAD2_TYPE		0
#define WAD3_TYPE		1

#define WAD2_TYPE_MIP	0x44
#define WAD3_TYPE_MIP	0x43

// WAD3 (Half-Life) Header and mip structs

struct wad3_headerinfo_s
{
	char		identification[4];		// should be WAD3 or 3DAW
	int			numlumps;
	int			infotableofs;
};

struct wad3_lumpinfo_s
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[16];				// must be null terminated
};



typedef struct
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[4];		// four mip maps stored
} wad3_miptex_s, WAD3_MIP, *LPWAD3_MIP;

// WAD2 (Quake1) Header and mip structs

struct wad2_headerinfo_s
{
	char		identification[4];		// should be WAD2 or 2DAW
	int			numlumps;
	int			infotableofs;
};

struct wad2_lumpinfo_s
{
	int			filepos;
	int			disksize;
	int			size;					// uncompressed
	char		type;
	char		compression;
	short		pad;
	char		name[16];				// must be null terminated
};


typedef struct
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[4];		// four mip maps stored
} wad2_miptex_s, WAD2_MIP, *LPWAD2_MIP;

#include "PackageTreeControl.h"


class CWADItem
{
// Members
private:
	unsigned char *m_pbyData;
	unsigned char *m_pbyPalette;
	LPWAD3_MIP m_wad3MipHeader;
	LPWAD2_MIP m_wad2MipHeader;
	CWADItem *m_pNext;
	CWADItem *m_pPrevious;
	unsigned short *m_pPaletteSize;
	int m_iTotalSize;
	int m_iSizes[4];
	char m_cType;
	CString m_strName;
	int m_iWadType;	

	BOOL m_bIsSelected;
	int m_iListBoxIndex;

	_PackageTreeEntryVector m_PackageTreeEntryVector;
	CWallyDoc *m_pWallyDoc;		// This is used to keep track of the image
								// once it's been opened.

public:
	CWADItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName, int iWADType);
	CWADItem ();
	~CWADItem ();
	void Init();

	void CreateAsMip (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight, int iWADType);

	unsigned char *GetBits (int iMipNumber = 0);	
	unsigned char *GetData ();
	void UpdateData (unsigned char *pbyBits[], CWallyPalette *pPalette, int iWidth, int iHeight);
	void ReplaceData (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);
	void SetWidth (int iWidth);
	int GetWidth ();
	void SetHeight (int iHeight);
	int GetHeight ();
	void SetName (LPCTSTR szName);
	CString GetName ();
	void SetNext (CWADItem *pNext);
	CWADItem *GetNext();
	void SetPrevious (CWADItem *pPrevious);
	CWADItem *GetPrevious();
	unsigned char *GetPalette();	
	
	BOOL ConvertToType( int iWADType );
	void RebuildSubMips();

	int GetSize();
	BOOL IsSelected()
	{
		return m_bIsSelected;
	}
	void SetSelected(BOOL bIsSelected = TRUE)
	{
		m_bIsSelected = bIsSelected;
	}
	void SetListBoxIndex (int iIndex)
	{
		m_iListBoxIndex = iIndex;
	}
	int GetListBoxIndex()
	{
		return m_iListBoxIndex;
	}
	/*struct wad3_miptex_s *GetWAD3Header()
	{
		return m_wad3MipHeader;
	}
	struct wad2_miptex_s *GetWAD2Header()
	{
		return m_wad2MipHeader;
	}*/
	void SetType (char cType);
	char GetType ();
	void SetWADType (int iType);
	int GetWADType ();

	void AddPackageEntry( CPackageTreeEntryBase *pBase );
	void GetPackageEntries( _PackageTreeEntryVector &PackageTreeEntryVector );

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

	bool IsValidMip ()
	{
		switch (GetWADType())
		{
		case WAD2_TYPE:
			return ((m_cType == WAD2_TYPE_MIP) && (m_strName != "CONCHARS"));
			break;

		case WAD3_TYPE:
			return (m_cType == WAD3_TYPE_MIP);
			break;

		default:
			ASSERT (false);
			break;
		}

		return false;
	}



};

class CWADList
{
// Members
private:
	CWADItem *m_pFirst;
	CWADItem *m_pRead;
	CWADItem *m_pWrite;
	int m_iWadType;

	wad3_headerinfo_s	*m_pwad3Header;
	wad3_lumpinfo_s		*m_pwad3LumpInfo;
	wad3_miptex_s		*m_pwad3Mip;

	wad2_headerinfo_s	*m_pwad2Header;
	wad2_lumpinfo_s		*m_pwad2LumpInfo;
	wad2_miptex_s		*m_pwad2Mip;

	int					m_iLumpPosition;
	unsigned char		*m_pEncodedData;

// Methods
public:
	CWADList ();
	~CWADList ();

	void SetFirst (CWADItem *pFirst);
	CWADItem *GetFirst ();
	void SetRead (CWADItem *pRead);
	CWADItem *GetRead ();
	void SetWrite (CWADItem *pWrite);
	CWADItem *GetWrite ();
	void SetAll (CWADItem *pItem);
	void PurgeList();

	/*int Serialize (CFile *pFile);	
	int Serialize (LPCTSTR szFileName, unsigned char *pbyData, bool bSave = false);*/
	int Serialize( LPCTSTR szFileName, CFile *pFile, unsigned char *pbyData, bool bSave = false );
	int Serialize( CWadMergeJob *pMergeJob );
	
	int GetNumImages ();
	DWORD GetNumImages( CWadMergeJob *pMergeJob );
	int GetNumLumps ();
	CWADItem *GetFirstImage();
	CWADItem *GetNextImage();
	CWADItem *GetFirstLump();
	CWADItem *GetNextLump();
	CWADItem *GetLumpAtPosition(int iPosition);
	CWADItem *AddItem( unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName );
	CWADItem *AddMipItem( unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);
	void AddToList( CWADItem *pItem);	
	void SetImageName (LPCTSTR szName, int iPosition);
	void RemoveImage (CWADItem *pItem, BOOL bDelete = TRUE );
	
	int GetTotalItemsSize();
	bool IsEmpty();
	CWADItem *IsNameInList (LPCTSTR szName);

	unsigned char *GetPaletteFromWAD3 (int iPosition = -1);	
	void DiscoverWidthHeight( LPBYTE pbyData, int iDataSize, char cType, LPDWORD pdwWidth, LPDWORD pdwHeight );
	void SetWADType (int iType);
	int GetWADType ();	
};

#endif		// _WADLIST_H_