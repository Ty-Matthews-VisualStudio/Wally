	
#ifndef _WAD3LIST_H_
#define _WAD3LIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

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

#define WAD3_TYPE_MIP	0x43

struct wad3_miptex_s
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[4];		// four mip maps stored
};

class CWallyPalette;
class CWallyDoc;

class CWAD3Item
{
// Members
private:
	unsigned char *m_pbyData;
	unsigned char *m_pbyPalette;
	struct wad3_miptex_s *m_wad3Header;
	CWAD3Item *m_pNext;
	CWAD3Item *m_pPrevious;
	unsigned short *m_pPaletteSize;
	int m_iTotalSize;
	int m_iSizes[4];
	char m_cType;
	CString m_strName;
	CWallyDoc *m_pWallyDoc;		// This is used to keep track of the image
								// once it's been opened.

public:
	CWAD3Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName);
	CWAD3Item ();
	~CWAD3Item ();

	void CreateAsMip (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);

	unsigned char *GetBits (int iMipNumber = 0);	
	unsigned char *GetData ();
	void UpdateData (unsigned char *pbyBits[], CWallyPalette *pPalette);
	void ReplaceData (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);
	void SetWidth (int iWidth);
	int GetWidth ();
	void SetHeight (int iHeight);
	int GetHeight ();
	void SetName (LPCTSTR szName);
	CString GetName ();
	void SetNext (CWAD3Item *pNext);
	CWAD3Item *GetNext();
	void SetPrevious (CWAD3Item *pPrevious);
	CWAD3Item *GetPrevious();
	unsigned char *GetPalette();	
	
	void Convert ();

	int GetSize();
	struct wad3_miptex_s *GetHeader()
	{
		return m_wad3Header;
	}
	void SetType (char cType = 0x43);
	char GetType ();

	CWallyDoc *GetWallyDoc()
	{
		return m_pWallyDoc;
	}

	void SetWallyDoc (CWallyDoc *pDoc)
	{
		m_pWallyDoc = pDoc;
	}


};

class CWAD3List
{
// Members
private:
	CWAD3Item *m_pFirst;
	CWAD3Item *m_pRead;
	CWAD3Item *m_pWrite;

// Methods
public:
	CWAD3List ();
	~CWAD3List ();

	void SetFirst (CWAD3Item *pFirst);
	CWAD3Item *GetFirst ();
	void SetRead (CWAD3Item *pRead);
	CWAD3Item *GetRead ();
	void SetWrite (CWAD3Item *pWrite);
	CWAD3Item *GetWrite ();
	void SetAll (CWAD3Item *pItem);
	void PurgeList();

	int Serialize (CFile *pFile);
	
	int GetNumImages ();
	int GetNumLumps ();
	CWAD3Item *GetFirstImage();
	CWAD3Item *GetNextImage();
	CWAD3Item *GetFirstLump();
	CWAD3Item *GetNextLump();
	CWAD3Item *AddItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName);
	CWAD3Item *AddMipItem (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight);
	unsigned char *GetPaletteFromWAD3 (int iPosition = -1);
	void SetImageName (LPCTSTR szName, int iPosition);
	void RemoveImage (CWAD3Item *pItem);
	
	int GetTotalItemsSize();
	bool IsEmpty();
	CWAD3Item *IsNameInList (LPCTSTR szName);
};

#endif		// _WAD3LIST_H_