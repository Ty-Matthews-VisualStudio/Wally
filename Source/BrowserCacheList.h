// BrowserCacheList.h: interface for the CBrowserCacheList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BROWSERCACHELIST_H__
#define _BROWSERCACHELIST_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSortStringArray;
class CBrowserCacheThread;
class CBrowserCacheList;

void LzrwCompress( const BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len);
void LzrwDecompress( const BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len);

#define FLAG_BYTES    4     /* Number of bytes used by copy flag. */
#define FLAG_COMPRESS 0     /* Signals that compression occurred. */
#define FLAG_COPY     1     /* Signals that a copyover occurred.  */

typedef struct
{
	// These first three items must remain for any future version.  The CACHE_HEADER
	// will be read to at least 16 bytes, which will retrieve the version number, and
	// header size so that the data can be read properly
	char szId[8];					// 'CACHE' with 3 NULLs after it
	DWORD iVersion;					// Version number
	DWORD iHeaderSize;				// Total bytes (including iDirNameLength) of header	
	
	DWORD iNumThumbnails;			// Number of thumbnails in the file
	DWORD iFirstItemOffset;			// Location of the first cache item
	long lAccessTime;				// Time last accessed	
	DWORD iDirNameLength;			// Length in bytes of the directory string plus NULL terminators... always DWORD align the bytes
	char szDirName;					// iDirNameLength bytes in size.  At least one NULL terminator

} CACHE_HEADER, *LPCACHE_HEADER, **LPPCACHE_HEADER;

#define		CACHE_HEADER_SIZE					(sizeof (CACHE_HEADER) - 1)
#define		CURRENT_CACHE_VERSION				1
#define		CACHE_SZID							"CACHE"

typedef struct
{
	// This first item must remain for any future version.  If this structure
	// is changed at all, then the CACHE_HEADER.iVersion number must be changed
	// accordingly, and a process must be introduced to convert any older cache
	// files to the new format before they can be used
	DWORD iHeaderSize;				// Total bytes (including szFileName length) of header	
	DWORD iVersion;					// Matches version from the Cache Header
	
	DWORD iFlags;					// Delete flags, etc	
	DWORD iCompressionType;			// Type of compression used for the image data (0 for no compression)
	DWORD iDataOffset;				// Location of the actual image data.  Relative to the beginning of the header 
	DWORD iDataSize;				// Total bytes of image data (stored in file, compressed)  Does not include header or palette!
	DWORD iPaletteOffset;			// Location of RGB palette (if any.)  Relative to the beginning of the header
	DWORD iNumPaletteColors;		// How many colors are stored in the palette
	DWORD iBitDepth;				// Bit depth of data stored (8, 16, 24, 32, etc)
	DWORD iWidth;					// Original image width
	DWORD iHeight;					// Original image height
	DWORD iThumbnailWidth;
	DWORD iThumbnailHeight;
	DWORD iImageType;				// Type of image... PCX, TGA, WAL, etc.  Matches ImageHelper defines
	long lModifyTime;				// Time last modified	
	int iFileNameLength;			// Length in bytes of the filename string plus NULL terminators... always DWORD align the bytes
	char szFileName;				// iFileNameLength bytes in size.  At least one NULL terminator
	
} CACHE_ITEM, *LPCACHE_ITEM, **LPPCACHE_ITEM;


#define		CACHE_ITEM_HEADER_SIZE				(sizeof (CACHE_ITEM) - 1)
#define		CACHE_ITEM_CURRENT_COMPRESSION		1

// Cache item flags
#define		CIFLAG_DELETE						0x1

#define		MAX_THUMBNAIL_SIZE					96

// Error codes
#define		CI_SUCCESS							0
#define		CI_ERROR_START						0
#define		CI_ERROR_UNSUPPORTED_VERSION		(CI_ERROR_START + 1)
#define		CI_ERROR_UNSUPPORTED_COMPRESSION	(CI_ERROR_START + 2)
#define		CI_ERROR_OUT_OF_MEMORY				(CI_ERROR_START + 3)
#define		CI_ERROR_UNSUPPORTED_BITDEPTH		(CI_ERROR_START + 4)
#define		CI_ERROR_FAIL_TO_OPEN				(CI_ERROR_START + 5)
#define		CI_ERROR_IMAGE_HELPER				(CI_ERROR_START + 6)

#define		CL_SUCCESS							0
#define		CL_ERROR_START						0
#define		CL_ERROR_OUT_OF_MEMORY				(CL_ERROR_START + 1)
#define		CL_ERROR_FAIL_TO_WRITE				(CL_ERROR_START + 2)


// Cache Thread defines
#define		CACHE_THREAD_MESSAGE_BEGIN			0
#define		CACHE_THREAD_MESSAGE_NONE			(CACHE_THREAD_MESSAGE_BEGIN + 1)
#define		CACHE_THREAD_MESSAGE_CLEANUP		(CACHE_THREAD_MESSAGE_BEGIN + 2)
#define		CACHE_THREAD_MESSAGE_GROOM			(CACHE_THREAD_MESSAGE_BEGIN + 3)
#define		CACHE_THREAD_MESSAGE_PAUSE			(CACHE_THREAD_MESSAGE_BEGIN + 4)
#define		CACHE_THREAD_MESSAGE_STOP			(CACHE_THREAD_MESSAGE_BEGIN + 5)
#define		CACHE_THREAD_MESSAGE_END			(CACHE_THREAD_MESSAGE_BEGIN + 6)

#define		CACHE_THREAD_STATUS_BEGIN			0
#define		CACHE_THREAD_STATUS_IDLE			(CACHE_THREAD_STATUS_BEGIN + 1)
#define		CACHE_THREAD_STATUS_CLEANING		(CACHE_THREAD_STATUS_BEGIN + 2)
#define		CACHE_THREAD_STATUS_GROOMING		(CACHE_THREAD_STATUS_BEGIN + 3)
#define		CACHE_THREAD_STATUS_STOPPED			(CACHE_THREAD_STATUS_BEGIN + 4)
#define		CACHE_THREAD_STATUS_END				(CACHE_THREAD_STATUS_BEGIN + 5)

class CBrowserCacheItem  
{
// Private Members
private:
	CString m_strFileName;
	
	int m_iWidth;
	int m_iHeight;
	int m_iThumbnailWidth;
	int m_iThumbnailHeight;
	int m_iImageType;
	int m_iErrorCode;

	BOOL m_bItemSelected;

	BYTE *m_pbyData;
	BYTE *m_pbyImageData;
	BYTE *m_pbyPalette;

	CBrowserCacheItem *m_pNext;
	CBrowserCacheItem *m_pPrevious;

	LPCACHE_ITEM m_lpItemHeader;

	CBrowserCacheList *m_pParentList;

public:
	CBrowserCacheItem();
	~CBrowserCacheItem();

	BOOL Create (LPCTSTR szFileName);	// Item isn't in the cache
	BOOL Create (BYTE *pbyBuffer, LPCTSTR szParentDirectory);		// Item is already there
	BYTE *GetData();	
	BYTE *DecompressData(BYTE **pbyCompressedSource, DWORD *pdwDataSize, BOOL bFreeSrc = FALSE);
	BYTE *CompressData(BYTE **pbySource, DWORD dwOriginalSize, DWORD *pdwCompressedSize, BOOL bFreeSrc = FALSE);	
	BOOL Serialize (FILE *wp, int *piPosition);
	
	int GetTotalItemSize();
	int GetThumbnailWidth()
	{
		return m_lpItemHeader->iThumbnailWidth;
	}
	int GetThumbnailHeight()
	{
		return m_lpItemHeader->iThumbnailHeight;
	}
	int GetBitDepth()
	{
		return m_lpItemHeader->iBitDepth;
	}
	int GetImageType()
	{
		return m_lpItemHeader->iImageType;
	}
	BYTE *GetItemData()
	{
		return m_pbyData;
	}
	BYTE *GetImageData()
	{
		return m_pbyImageData;
	}
	BYTE *GetPalette()
	{
		return m_pbyPalette;
	}
	CString GetFileName()
	{
		return m_strFileName;
	}
	LPCACHE_ITEM GetHeader()
	{
		return m_lpItemHeader;
	}
	int GetWidth()
	{
		return m_iWidth;
	}
	int GetHeight()
	{
		return m_iHeight;
	}
	void SetParent (CBrowserCacheList *pParent)
	{
		m_pParentList = pParent;
	}
	BOOL IsSelected()
	{
		return m_bItemSelected;
	}
	void SetSelected(BOOL bIsSelected = TRUE)
	{
		m_bItemSelected = bIsSelected;
	}

	void SetErrorCode (int iCode);
	int GetErrorCode();

	BOOL RecreateThumbnail (int iWidth = 0, int iHeight = 0);
	void SetNext (CBrowserCacheItem *pNext);
	CBrowserCacheItem *GetNext();
	void SetPrevious (CBrowserCacheItem *pPrevious);
	CBrowserCacheItem *GetPrevious();
};

class CBrowserCacheList  
{
// Private Members
private:
	CBrowserCacheItem *m_pFirst;
	CBrowserCacheItem *m_pWrite;
	CBrowserCacheItem *m_pSelected;

	CString m_strDirectory;
	CString m_strCacheFileName;
	LPCACHE_HEADER m_lpCacheHeader;
	
	int m_iErrorCode;

	CBrowserCacheThread *m_pCacheThread;
	int m_iCacheThreadMessage;
	int m_iCacheThreadStatus;
		
public:
	CBrowserCacheList();
	~CBrowserCacheList();

	void SetErrorCode (int iCode);
	int GetErrorCode();

	CBrowserCacheItem *GetAtPosition (int iPosition);
	int GetCount();

	void AddItem(CBrowserCacheItem *pItem);
	void RemoveItem(CBrowserCacheItem *pItem);
	CString GenerateRandomFileName(int iLength);

	CString GetDirectory()
	{
		return m_strDirectory;
	}
	CString GetCacheFileName()
	{
		return m_strCacheFileName;
	}
	void Initialize(LPCTSTR szDirectory, LPVOID lpDoc, BOOL bRefresh = FALSE);
	BOOL GetHeaderFromFile (LPCTSTR szFileName);
	CString FindDirNameFromFile(LPCTSTR szFileName);
	void FindItemsFromFile(CSortStringArray *pStrArray, LPCTSTR szFileName);
	BOOL CreateCacheFile (LPCTSTR szDirectory);
	BOOL CreateTempCacheFile ();

	void PurgeList();
	void SetFirst (CBrowserCacheItem *pFirst);
	CBrowserCacheItem *GetFirst();
	void SetWrite (CBrowserCacheItem *pWrite);
	CBrowserCacheItem *GetWrite();
	void SetAll (CBrowserCacheItem *pItem);

	void SetThreadMessage (int iMessage);
	int GetThreadMessage ();
	void SetThreadStatus (int iStatus);
	int GetThreadStatus ();

	CBrowserCacheItem *GetFirstSelectedItem();
	CBrowserCacheItem *GetNextSelectedItem();
	
	void ResetAllSelections();

};


// Global functions
void ResizeImage256 (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination, BYTE *pbyPalette);
void ResizeImage24Bit (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination);
void ResizeImage32Bit (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination);

#endif // #ifndef _BROWSERCACHELIST_H__
