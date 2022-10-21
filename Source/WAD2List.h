	
#ifndef _WAD2LIST_H_
#define _WAD2LIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

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

#define WAD2_TYPE_MIP	0x44

struct wad2_miptex_s
{
	char		name[16];
	unsigned	width, height;
	unsigned	offsets[4];		// four mip maps stored
};

class CWallyDoc;

class CWAD2Item
{
// Members
private:
	unsigned char *m_pbyData;	
	struct wad2_miptex_s *m_wad2Header;
	CWAD2Item *m_pNext;
	CWAD2Item *m_pPrevious;	
	int m_iTotalSize;
	int m_iSizes[4];
	char m_cType;
	CString m_strName;
	CWallyDoc *m_pWallyDoc;		// This is used to keep track of the image
								// once it's been opened.

public:
	CWAD2Item (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName);
	CWAD2Item ();
	~CWAD2Item ();

	void CreateAsMip (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight);

	unsigned char *GetBits (int iMipNumber = 0);	
	unsigned char *GetData ();
	void UpdateData (unsigned char *pbyBits[]);
	void ReplaceData (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight);
	void SetWidth (int iWidth);
	int GetWidth ();
	void SetHeight (int iHeight);
	int GetHeight ();
	void SetName (LPCTSTR szName);
	CString GetName ();
	void SetNext (CWAD2Item *pNext);
	CWAD2Item *GetNext();
	void SetPrevious (CWAD2Item *pPrevious);
	CWAD2Item *GetPrevious();	
	
	void Convert ();

	int GetSize();
	struct wad2_miptex_s *GetHeader()
	{
		return m_wad2Header;
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

class CWAD2List
{
// Members
private:
	CWAD2Item *m_pFirst;
	CWAD2Item *m_pRead;
	CWAD2Item *m_pWrite;

// Methods
public:
	CWAD2List ();
	~CWAD2List ();

	void SetFirst (CWAD2Item *pFirst);
	CWAD2Item *GetFirst ();
	void SetRead (CWAD2Item *pRead);
	CWAD2Item *GetRead ();
	void SetWrite (CWAD2Item *pWrite);
	CWAD2Item *GetWrite ();
	void SetAll (CWAD2Item *pItem);
	void PurgeList();

	int Serialize (CFile *pFile);
	
	int GetNumImages ();
	int GetNumLumps ();
	CWAD2Item *GetFirstImage();
	CWAD2Item *GetNextImage();
	CWAD2Item *GetFirstLump();
	CWAD2Item *GetNextLump();
	CWAD2Item *AddItem (unsigned char *pbyData, int iDataSize, char cType, LPCTSTR szName);
	CWAD2Item *AddMipItem (unsigned char *pbyBits[], LPCTSTR szName, int iWidth, int iHeight);	
	void SetImageName (LPCTSTR szName, int iPosition);
	void RemoveImage (CWAD2Item *pItem);
	
	int GetTotalItemsSize();
	bool IsEmpty();
	CWAD2Item *IsNameInList (LPCTSTR szName);
};

#endif		// _WAD2LIST_H_