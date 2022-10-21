#ifndef _IMAGE_HELPER_H_
#define _IMAGE_HELPER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// External dependencies
#include "SafeFile.h"
#include "WADList.h"
#include "BuildList.h"

class CImageHelper;

//////////////////////////////////////////////////////////////////////////////////////////
//	Here's where I got the various libraries used:
//
//		PNG/ZLIB	- http://www.libpng.org/pub/png/libpng.html
//					- http://libpng.sourceforge.net/
//		JPEG		- http://www.ijg.org/
//
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// PNG Library
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	// Makes pnglib verbose so we can find problems (needs to be before png.h)
	#define PNG_DEBUG 0
	#pragma comment (lib, "./lpng1012/libpngd.lib")
#else
	#pragma comment (lib, "./lpng1012/libpng.lib")
#endif

#pragma comment (lib, "./ZLib/zlib.lib")

#include "./lpng1012/png.h"

//////////////////////////////////////////////////////////////////////////////////////////
// JPEG Library 
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#pragma comment (lib, "./TempFiles/libjpegd.lib")
#else
	#pragma comment (lib, "./TempFiles/libjpeg.lib")
#endif

#include "./jpeg-6b/jpeglib.h"
#include "./jpeg-6b/jerror.h"


//////////////////////////////////////////////////////////////////////////////////////////
// Image file format structures
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PCX_DEFINITION_
#define __PCX_DEFINITION_

#define PCXFILLER        (128-74)		// 54

// PCXHEADER - PCX File Header

typedef struct
{
	BYTE  byManufacturer;		// keyword (10 decimal)
	BYTE  byVersion;			// hardware version (5, old files with no palette are 3)
	BYTE  byEncoding;			// run length encoded (1)
	BYTE  byBitsPerPixelPlane;	// bits per pixel per plane
	WORD  wXmin;				// picture dimensions in pixels
	WORD  wYmin;
	WORD  wXmax;
	WORD  wYmax;
	WORD  wHdpi;			// horizontal image dpi
	WORD  wVdpi;			// vertical image dpi
	BYTE  byPal[48];		// palette
	BYTE  byVMode;			// monitor (ignored)
	BYTE  byNumPlanes;		// number of planes in image 
	WORD  wBytesPerLine;	// bytes per line per plane in picture 
	WORD  wPalInfo;			// grayscale/color flag (obsolete)
	WORD  wHScreenSize;		// horizontal screen size in pixels
	WORD  wVScreenSize;		// vertical screen size in pixels

								// the rest of the 128 byte header is not used
	BYTE byFiller[PCXFILLER];	// (reserved, should be zero)
    BYTE byData;				// start of image data (unbounded)
} PCXHEADER;

// PCX data type defines
#define PCX_TYPE_24BIT	0
#define PCX_TYPE_8BIT	1
#define RED				0
#define GREEN			1
#define BLUE			2

#endif		// __PCX_DEFINITION_

// Bitmap stuff
#ifndef __BMP_STRUCT_
#define __BMP_STRUCT_

#define BMH	('B' + ('M'<<8))

#endif		// __BMP_STRUCT_

// MIP Header (Quake1) struct
typedef struct
{
	char			name[16];
	unsigned		width, height;
	unsigned		offsets[4];		// four mip maps stored
} q1_miptex_s, Q1_MIP_S, *LPQ1_MIP_S;

#define Q1_HEADER_SIZE		sizeof (Q1_MIP_S)


// WAL Header (Quake2) struct
typedef struct 
{
    char            name[32];
    unsigned        width, height;
    unsigned        offsets[4];      // four mip maps stored
    char            animname[32];    // next frame in animation chain
    int             flags;
    int             contents;
    int             value;
} q2_miptex_s, Q2_MIP_S, *LPQ2_MIP_S;

typedef struct 
{
    char            *szName;    
    char            *szAnimname;
    int             *pFlags;
    int             *pContents;
    int             *pValue;
} Q2_RULE_S, *LPQ2_RULE_S;

#define	Q2_HEADER_SIZE		sizeof (Q2_MIP_S)
#define Q2RULE_HEADER_SIZE	sizeof (Q2_RULE_S)

#ifndef _M8_STRUCT_
#define _M8_STRUCT_

#define M8_MIPLEVELS		16
typedef struct
{
	int			version;
	char		name[32];
	unsigned	width[M8_MIPLEVELS];
	unsigned	height[M8_MIPLEVELS];
	unsigned	offsets[M8_MIPLEVELS];
	char		animname[32];
//	int			what_the_hell_is_this;		// Neal - palatte bugfix
//	byte		what_the_hell_is_this[3];
	BYTE		palette[768];
	int			flags;
	int			contents;
	int			value;
//	byte		value;
} M8_MIPTEX_S, M8_MIP_S, *LPM8_MIPTEX_S;

#define M8_HEADER_SIZE		sizeof (M8_MIPTEX_S)

#endif		//	#ifndef _M8_STRUCT_

// SWL Header (SiN) struct

#ifndef _SWL_STRUCT_
#define _SWL_STRUCT_

#define		SIN_PALETTE_SIZE	1024	
typedef struct
{
	char			name[64];
    unsigned int	width;
	unsigned int	height;
	BYTE			palette[SIN_PALETTE_SIZE];
	unsigned short	palcrc;
	unsigned		offsets[4];			// four mip maps stored
    char			animname[64];		// next frame in animation chain
    unsigned int	flags;
    unsigned int	contents;
    unsigned short	value;
	unsigned short	direct;
	float			animtime;
	float			nonlit;      
	unsigned short	directangle;
	unsigned short	trans_angle;
	float			directstyle;
	float			translucence;
	float			friction;
	float			restitution;
	unsigned int	trans_mag;
	float			color[3];      
} sin_miptex_s, SIN_MIP_S, *LPSIN_MIP_S;

typedef struct
{
	char			*szName;    
    char			*szAnimname;
    unsigned int	*pFlags;
    unsigned int	*pContents;
    unsigned short	*pValue;
	unsigned short	*pDirect;
	float			*pAnimtime;
	float			*pNonlit;      
	unsigned short	*pDirectangle;
	unsigned short	*pTrans_angle;
	float			*pDirectstyle;
	float			*pTranslucence; 
	float			*pFriction;   
	float			*pRestitution;   
	unsigned int	*pTrans_mag;     
	float			*pColor;
} SIN_RULE_S, *LPSIN_RULE_S;

#define SIN_HEADER_SIZE			sizeof (SIN_MIP_S)
#define SINRULE_HEADER_SIZE		sizeof (SIN_RULE_S)

#endif


// TGA Header
#ifndef __TGA_DEFINITION_
#define __TGA_DEFINITION_

struct TGA_Header
{
	char ID_Length;
	char ColorMapType;
	char ImageType;
	char ColorMapSpec[5];	
	WORD XOrigin;
	WORD YOrigin;
	WORD Width;
	WORD Height;
	char PixelDepth;
	char ImageDescriptor;
};

#endif	//__TGA_DEFINITION_

#ifndef __GIF_DEFINITION_
#define __GIF_DEFINITION_

#pragma pack (1)
typedef struct
{
	char szID[3];
	char szVersion[3];

} GIF_HEADER, *LPGIF_HEADER;


// Logical Screen Descriptor.  One per GIF file
typedef struct
{
	unsigned short uiWidth;
	unsigned short uiHeight;
	BYTE byFlags;
	BYTE byBGCIndex;
	BYTE byAspect;
} GIF_LSD, *LPGIF_LSD;		

// Image Descriptor.  One per image in a GIF file
typedef struct
{
	BYTE bySeparator;			// Should be 0x2c
	unsigned short uiLeft;
	unsigned short uiTop;
	unsigned short uiWidth;
	unsigned short uiHeight;
	BYTE byFlags;
} GIF_ID, *LPGIF_ID;

// Graphic Control Extension
typedef struct
{
	BYTE byIntroducer;
	BYTE byControlLabel;

} GIF_GCE, *LPGIF_GCE;

#pragma pack()      // Revert to previous packing


// GM == Gif Mask
#define GM_LSD_COLOR_TABLE		0x1
#define GM_LSD_COLOR_DEPTH		0xe
#define GM_LSD_SORT				0x10
#define GM_LSD_CT_SIZE			0xe0

#define GM_ID_LOCAL_TABLE		0x1
#define GM_ID_INTERLACE			0x2
#define GM_ID_SORT				0x4
#define GM_ID_RESERVED			0x18
#define GM_ID_CT_SIZE			0xe0

#define GIF_87A					0 
#define GIF_89A					1

#define GIF_HEADER_SIZE			sizeof (GIF_HEADER)
#define GIF_LSD_SIZE			sizeof (GIF_LSD)
#define GIF_ID_SIZE				sizeof (GIF_ID)

#endif __GIF_DEFINITION_


#ifndef __JPG_DEFINITION_
#define __JPG_DEFINITION_

// JPG Error handling
struct JPGErrorMgr
{
	struct jpeg_error_mgr pub;		/* "public" fields */
	jmp_buf setjmp_buffer;			/* for return to caller */
	CImageHelper *p_ImageHelper;
};

typedef struct JPGErrorMgr JPGErrorMgr_s;
typedef struct JPGErrorMgr * lpJPGErrorMgr;

// This came from jdatasrc in the JPEG lib
typedef struct 
{
	struct jpeg_source_mgr pub;		// Public fields
	BYTE *pbySourceData;			// Buffer
	UINT iDataPosition;				// Location in the buffer
	UINT iDataSize;					// Size of the buffer
} JPGSourceManager, *lpJPGSourceManager;

// This came from jdatadst.c in the JPEG lib
typedef struct
{
	struct jpeg_destination_mgr pub;	// Public fields
  	BYTE *pbyDestinationData;			// Buffer
	UINT iDataPosition;					// Location in the buffer
	UINT iDataSize;						// Size of the buffer
	LPVOID lpMemFile;					// Grows as file is built
} JPGDestinationManager, *lpJPGDestinationManager;

#define JPG_OUTPUT_BUFFER_SIZE	4096

#endif		// #ifndef __JPG_DEFINITION_


#ifndef __TEX_DEFINITION_
#define __TEX_DEFINITION_
#pragma pack (1)

typedef signed long  int	MEX;

// some mexels constants
#define MAX_MEX_LOG2 10
#define MIN_MEX_LOG2  0
#define MAX_MEX     (1L<<MAX_MEX_LOG2)
#define MIN_MEX     (1L<<MIN_MEX_LOG2)

// macro for converting mexels to meters
#define METERS_MEX(mex)    ((FLOAT)(((FLOAT)mex)/MAX_MEX))
#define MEX_METERS(meters) ((MEX)(meters*MAX_MEX))

typedef struct
{
	unsigned int	Flags;
	unsigned int	MexWidth;
	unsigned int	MexHeight;
	unsigned int	FineMipLevels;
	unsigned int	FirstMipLevel;
	unsigned int	NumFrames;	
} TEX_TDAT_CHUNK, *LPTEX_TDAT_CHUNK;

typedef struct
{
	unsigned int	NumAnimations;
	char			Name[32];
	float			SecondsPerFrame;
	unsigned int	NumberOfFrames;
	unsigned int	Indexes;			// Boundless; NumberOfFrames * sizeof(unsigned int)
} TEX_ADAT_CHUNK, *LPTEX_ADAT_CHUNK;

#pragma pack ()

#endif		// #ifndef __TEX_DEFINITION_

// Image Helper defines
#define	IMAGE_TYPE_START	-1
#define IH_M8_TYPE			(IMAGE_TYPE_START + 1)
#define IH_JPG_TYPE			(IMAGE_TYPE_START + 2)
#define IH_PNG_TYPE			(IMAGE_TYPE_START + 3)
#define IH_WAL_TYPE			(IMAGE_TYPE_START + 4)
#define IH_MIP_TYPE			(IMAGE_TYPE_START + 5)
#define	IH_TEX_TYPE			(IMAGE_TYPE_START + 6)
#define IH_SWL_TYPE			(IMAGE_TYPE_START + 7)
#define IH_TGA_TYPE			(IMAGE_TYPE_START + 8)
#define IH_BMP_TYPE			(IMAGE_TYPE_START + 9)
#define	IH_PCX_TYPE			(IMAGE_TYPE_START + 10)

#ifdef _TEST
#define IH_WLY_TYPE			(IMAGE_TYPE_START + 11)
#define IH_GIF_TYPE			(IMAGE_TYPE_START + 12)
#define	IMAGE_TYPE_END		(IMAGE_TYPE_START + 12)
#else
#define	IMAGE_TYPE_END		(IMAGE_TYPE_START + 10)
#endif		// #ifdef _TEST

#define IH_ART_TYPE			10000		// Testing for now

#define	PACKAGE_TYPE_START	(IMAGE_TYPE_END)
#define IH_WAD2_TYPE		(PACKAGE_TYPE_START + 1)
#define IH_WAD3_TYPE		(PACKAGE_TYPE_START + 2)
#define	PACKAGE_TYPE_END	(PACKAGE_TYPE_START + 2)

#define	PALETTE_TYPE_START	(PACKAGE_TYPE_END)
#define IH_PAL_TYPE			(PALETTE_TYPE_START + 1)
#define IH_LMP_TYPE			(PALETTE_TYPE_START + 2)
#define IH_ACT_TYPE			(PALETTE_TYPE_START + 3)
#define PALETTE_TYPE_END	(PALETTE_TYPE_START + 3)

#define PAK_TYPE_START		(PALETTE_TYPE_END)
#define IH_PAK_TYPE			(PAK_TYPE_START + 1)
#define PAK_TYPE_END		(PAK_TYPE_START + 1)

#ifdef _TEST
	#define WILDCARD_LIST_COUNT			18
#else
	#define WILDCARD_LIST_COUNT			16
#endif

#define WILDCARD_LIST_ITEM_COUNT	7
#define WILDCARD_LIST_LENGTH	(WILDCARD_LIST_COUNT * WILDCARD_LIST_ITEM_COUNT)

extern	CString g_strTypeDescriptions[WILDCARD_LIST_LENGTH];
extern	CWildCardList	g_WildCardList;

// LoadImage flags
#define	IH_LOAD_ONLYIMAGE		0x1
#define	IH_LOAD_ONLYPACKAGE		0x2
#define IH_LOAD_ONLYPALETTE		0x4
#define IH_LOAD_FILEOPEN		0x8
#define IH_LOAD_DIMENSIONS		0x10

// SaveImage flags
#define IH_SAVE_UNIQUE_FILENAME	0x1
#define IH_USE_BATCH_SETTINGS	0x2

// Image descriptor flags
#define IH_TYPE_FLAG_SUPPORTS8BIT	0x1
#define IH_TYPE_FLAG_SUPPORTS24BIT	0x2
#define IH_TYPE_FLAG_ISGAME			0x4			

// These defines are not actual image files; they are not able to exist
// on their own
#define GAME_ONLY_TYPE_START	(PALETTE_TYPE_END)
#define IH_WAD3_IMAGE_TYPE		(GAME_ONLY_TYPE_START + 1)
#define GAME_ONLY_TYPE_END		(GAME_ONLY_TYPE_START + 2)


// Special flag defines
#define IH_1BIT				1
#define IH_2BIT				2
#define IH_4BIT				4
#define IH_8BIT				8
#define IH_16BIT			16
#define IH_24BIT			24
#define IH_32BIT			32

// Image Helper error codes
#define IH_NONE						-1
#define IH_SUCCESS					0
#define IH_UNSUPPORTED_IMAGE		(IH_SUCCESS + 1)		
#define IH_UNSUPPORTED_PACKAGE		(IH_SUCCESS + 2)		
#define IH_UNSUPPORTED_PALETTE		(IH_SUCCESS + 3)		
#define IH_UNSUPPORTED_FILE			(IH_SUCCESS + 4)		
#define IH_ERROR_READING_FILE		(IH_SUCCESS + 5)		
#define	IH_ERROR_WRITING_FILE		(IH_SUCCESS + 6)		
#define IH_NOT_DIV16				(IH_SUCCESS + 7)		
#define IH_FILE_NOT_FOUND			(IH_SUCCESS + 8)		
#define IH_CANT_CREATE_UNIQUE		(IH_SUCCESS + 9)
#define IH_OUT_OF_MEMORY			(IH_SUCCESS + 10)
#define IH_GENERIC_MSG_END			(IH_SUCCESS + 10)

// PCX Error codes
#define IH_PCX_ERROR_START				(IH_GENERIC_MSG_END)
#define	IH_PCX_MALFORMED				(IH_PCX_ERROR_START + 1)	
#define	IH_PCX_INCOMPATIBLE				(IH_PCX_ERROR_START + 2)	
#define	IH_PCX_BAD_COLOR_PLANES			(IH_PCX_ERROR_START + 3)	
#define IH_PCX_ERROR_END				(IH_PCX_ERROR_START + 4)	

// BMP Error codes
#define IH_BMP_ERROR_START				(IH_PCX_ERROR_END)
#define IH_BMP_MALFORMED				(IH_BMP_ERROR_START + 1)	
#define IH_BMP_UNSUPPORTED_VERSION 		(IH_BMP_ERROR_START + 2)	
#define IH_BMP_UNSUPPORTED_COMPRESSION	(IH_BMP_ERROR_START + 3)
#define IH_BMP_ERROR_END				(IH_BMP_ERROR_START + 4)

// TGA Error codes
#define IH_TGA_ERROR_START				(IH_BMP_ERROR_END)
#define IH_TGA_MALFORMED				(IH_TGA_ERROR_START + 1)
#define IH_TGA_UNSUPPORTED_BIT_DEPTH	(IH_TGA_ERROR_START + 2)
#define IH_TGA_UNSUPPORTED_PALETTE		(IH_TGA_ERROR_START + 3)
#define IH_TGA_ERROR_END				(IH_TGA_ERROR_START + 4)

// PAL Error codes
#define IH_PAL_ERROR_START				(IH_TGA_ERROR_END)
#define IH_PAL_INVALID_TYPE				(IH_PAL_ERROR_START + 1)
#define IH_PAL_WRONG_NUM_COLORS			(IH_PAL_ERROR_START + 2)
#define IH_PAL_MALFORMED				(IH_PAL_ERROR_START + 3)
#define IH_PAL_ERROR_END				(IH_PAL_ERROR_START + 4)

// WAD Error codes
#define IH_WAD_ERROR_START				(IH_PAL_ERROR_END)
#define IH_WAD_MALFORMED				(IH_WAD_ERROR_START + 1)
#define IH_WAD_NO_IMAGES				(IH_WAD_ERROR_START + 2)
#define IH_WAD_UNSUPPORTED_VERSION		(IH_WAD_ERROR_START + 3)
#define IH_WAD_ERROR_END				(IH_WAD_ERROR_START + 4)

// WAL Error codes
#define IH_WAL_ERROR_START				(IH_WAD_ERROR_END)
#define IH_WAL_MALFORMED				(IH_WAL_ERROR_START + 1)
#define IH_WAL_ERROR_END				(IH_WAL_ERROR_START + 2)

// SWL Error codes
#define IH_SWL_ERROR_START				(IH_WAL_ERROR_END)
#define IH_SWL_MALFORMED				(IH_SWL_ERROR_START + 1)
#define IH_SWL_ERROR_END				(IH_SWL_ERROR_START + 2)

// M8 Error codes
#define IH_M8_ERROR_START				(IH_SWL_ERROR_END)
#define IH_M8_MALFORMED					(IH_M8_ERROR_START + 1)
#define IH_M8_ERROR_END					(IH_M8_ERROR_START + 2)

// MIP Error codes
#define IH_MIP_ERROR_START				(IH_M8_ERROR_END)
#define IH_MIP_MALFORMED				(IH_MIP_ERROR_START + 1)
#define IH_MIP_ERROR_END				(IH_MIP_ERROR_START + 2)

// WLY Error codes
#define IH_WLY_ERROR_START				(IH_MIP_ERROR_END)
#define IH_WLY_MALFORMED				(IH_WLY_ERROR_START + 1)
#define IH_WLY_UNSUPPORTED_VERSION		(IH_WLY_ERROR_START + 2)
#define IH_WLY_ERROR_END				(IH_WLY_ERROR_START + 3)

// GIF Error codes
#define IH_GIF_ERROR_START				(IH_WLY_ERROR_END)
#define IH_GIF_MALFORMED				(IH_GIF_ERROR_START + 1)
#define IH_GIF_UNSUPPORTED_VERSION		(IH_GIF_ERROR_START + 2)
#define IH_GIF_ERROR_END				(IH_GIF_ERROR_START + 3)

// PNG Error codes
#define IH_PNG_ERROR_START				(IH_GIF_ERROR_END)
#define IH_PNG_MALFORMED				(IH_PNG_ERROR_START + 1)
#define IH_PNG_UNSUPPORTED_VERSION		(IH_PNG_ERROR_START + 2)
#define IH_PNG_CUSTOM_ERROR				(IH_PNG_ERROR_START + 3)
#define IH_PNG_READ_ERROR				(IH_PNG_ERROR_START + 4)
#define IH_PNG_WRITE_ERROR				(IH_PNG_ERROR_START + 5)
#define IH_PNG_ERROR_END				(IH_PNG_ERROR_START + 6)

// JPG Error codes
#define IH_JPG_ERROR_START				(IH_PNG_ERROR_END)
#define IH_JPG_MALFORMED				(IH_JPG_ERROR_START + 1)
#define IH_JPG_CUSTOM_ERROR				(IH_JPG_ERROR_START + 2)
#define IH_JPG_ERROR_END				(IH_JPG_ERROR_START + 3)

// TEX Error codes
#define IH_TEX_ERROR_START				(IH_JPG_ERROR_END)
#define IH_TEX_MALFORMED				(IH_TEX_ERROR_START + 1)
#define IH_TEX_UNSUPPORTED_VERSION		(IH_TEX_ERROR_START + 2)
#define IH_TEX_TOO_MANY_FRAMES			(IH_TEX_ERROR_START + 3)
#define IH_TEX_ANIMATION_DATA			(IH_TEX_ERROR_START + 4)
#define IH_TEX_ERROR_END				(IH_TEX_ERROR_START + 5)


class CWallyPalette;

class CWildCardItem
{
// Members
private:
	CString m_strDescription;
	CString m_strWildCard;	
	CString m_strWildCardExtension;
	int m_iImageType;
	CWildCardItem *m_pNext;
	CWildCardItem *m_pPrevious;
	CString m_strSourcePaletteFile;
	CString m_strDestPaletteFile;
	CString m_strSourceRegKey;
	CString m_strDestRegKey;
	CString m_strUseCurrentSourceRegKey;
	CString m_strUseCurrentDestRegKey;

	CString m_strTempSourcePaletteFile;
	CString m_strTempDestPaletteFile;
	
	CString m_strAssociatedProgram;
	CString m_strAssociatedAppRegKey;

	BOOL m_bInChangeMode;

public:
	BOOL m_bUseCurrentSourcePalette;
	BOOL m_bUseCurrentDestPalette;
	BOOL m_bTempUseCurrentSourcePalette;
	BOOL m_bTempUseCurrentDestPalette;

// Methods
public:
	CWildCardItem (LPCTSTR szDescription, LPCTSTR szWildCard, LPCTSTR szWildCardExtension, LPCTSTR szSourceRegKey, LPCTSTR szDestRegKey, LPCTSTR szUseCurrentSourceRegKey, LPCTSTR szUseCurrentDestRegKey, int iImageType);
	~CWildCardItem ();

	void SolidifyChanges();	
	void PrepareForChanges();
	void CancelChanges();
	void FindAssociatedProgram();
	CString GetAssociatedProgram();
	void AssociateWithWally();
	void RemoveIfAssociatedWithWally();
	
	CWildCardItem *GetNext();
	void SetNext (CWildCardItem *pNext);
	CWildCardItem *GetPrevious();
	void SetPrevious (CWildCardItem *pPrevious);

	CString GetDescription();
	CString GetWildCard();
	CString GetWildCardExtension();
	
	CString GetSourcePaletteFile();
	void SetSourcePaletteFile(LPCTSTR szDirectory);
	CString GetDestPaletteFile();	
	void SetDestPaletteFile(LPCTSTR szDirectory);

	CString GetSourceRegKey();
	CString GetDestRegKey();
	CString GetUseCurrentSourceRegKey();
	CString GetUseCurrentDestRegKey();

	int GetImageType();
	BOOL SupportsSourcePalette();
	BOOL UseCurrentSourcePalette();
	BOOL UseCurrentDestPalette();

	BOOL GetSourcePalette(BYTE *pbyPalette);
	BOOL GetDestPalette(BYTE *pbyPalette);
};

class CWildCardList
{
// Members
private:
	CWildCardItem *m_pFirst;
	CWildCardItem *m_pWrite;
	CWildCardItem *m_pRead;

// Methods
public:
	CWildCardList();
	~CWildCardList();
	void SetAll(CWildCardItem *pItem);
	void PurgeList();
	void AddItem (LPCTSTR szDescription, LPCTSTR szWildCard, LPCTSTR szWildCardExtension, LPCTSTR szSourceRegKey, LPCTSTR szDestRegKey, LPCTSTR szUseCurrentSourceRegKey, LPCTSTR szUseCurrentDestRegKey, int iImageType);
	int GetCount();
	CWildCardItem *GetFirstSupportedImage();
	CWildCardItem *GetNextSupportedImage();
	
	CWildCardItem *GetAt(int iPosition);
	
	CWildCardItem *GetFirst();
	void SetFirst(CWildCardItem *pFirst);
	CWildCardItem *GetWrite();
	void SetWrite(CWildCardItem *pWrite);
	CWildCardItem *GetRead();
	void SetRead(CWildCardItem *pRead);

	void PrepareForChanges();
	void SolidifyAllChanges();
	void CancelAllChanges();
	
	void ReadRegistry();
	void WriteRegistry();

};

class CImageHelper
{
// Members
private:
	BYTE *m_pbyDecodedData;
	BYTE *m_pbyEncodedData;
	//BYTE *m_pBMPPalette;
	BYTE m_byPalette[768];
	LPBYTE m_pbyAlphaChannel;
	
	int m_iImageType;
	UINT m_iDataSize;	
	int m_iWidth;
	int m_iHeight;
	int m_iErrorCode;
	int m_iFileLength;
	int m_iSpecialFlag;
	int m_iColorDepth;

	CString m_strFileName;
	CString m_strErrorText;

	CSafeFile *m_psfFile;
	CArchive *m_parFile;
	BOOL m_bUseArchive;

	PCXHEADER*        m_pPcxHeader;
	BITMAPFILEHEADER* m_pBmfHeader;
	BITMAPINFO*       m_pBmfInfo;

	CWADList m_WADList;
	//CWildCardList m_WildCardList;
	CBuildList m_BuildList;
	
	q2_miptex_s		*m_pQ2ExportHeader;
	q1_miptex_s		*m_pQ1ExportHeader;
	LPSIN_MIP_S		m_lpSinExportHeader;
	LPM8_MIPTEX_S	m_lpM8ExportHeader;

	// PNG read/write stuff:
	UINT			m_iPNGReadPosition;
	UINT			m_iPNGWritePosition;
	FILE			*m_fpPNGOutput;

// Private Methods
private:
	//int PadDWORD (int x);

// Public Methods
public:	
	CImageHelper ();	
	~CImageHelper ();	
	void LoadImage (LPCTSTR szFileName, int iFlags = 0);
	void LoadImage (CFile *cfOpen, int iFlags = 0);
	void SaveImage (int iColorDepth, LPCTSTR szFileName, unsigned char *pbyRawData, unsigned char *pbyPalette, int iWidth, int iHeight);
	void SaveImage (int iColorDepth, CArchive *parFile, LPBYTE pbyRawData, LPBYTE pbyPalette, int iWidth, int iHeight);
	void ConvertImage (int iColorDepth, LPCTSTR szDestinationDirectory, int iImageType, int iFlags = 0);
	void ResetContent();
	void Initialize();
		
	BOOL DecodePAL( int iFlags = 0 );
	BOOL EncodePAL ();	
	BOOL DecodeMIP( int iFlags = 0 );
	BOOL EncodeMIP ();
	BOOL EncodeRawData ();
	BOOL DecodeWAL( int iFlags = 0 );
	BOOL EncodeWAL ();
	BOOL DecodeSWL( int iFlags = 0 );
	BOOL EncodeSWL ();
	BOOL DecodeM8( int iFlags = 0 );
	BOOL EncodeM8 ();
	
	BOOL DecodePCX( int iFlags = 0 );
	BOOL EncodePCX ();
	BOOL DecodeBMP( int iFlags = 0 );
	BOOL EncodeBMP ();
	BOOL DecodeTGA( int iFlags = 0 );
	BOOL EncodeTGA ();
	BOOL DecodeGIF( int iFlags = 0 );
	BOOL EncodeGIF ();
	BOOL DecodePNG( int iFlags = 0 );
	BOOL EncodePNG ();
	BOOL DecodeJPG( int iFlags = 0 );
	BOOL EncodeJPG ();	
	BOOL DecodeTEX( int iFlags = 0 );
	BOOL EncodeTEX ();

	// WAD stuff
	BOOL DetermineWADType();
	int GetNumImages ();
	int GetNumLumps ();
	CWADItem *GetFirstImage();
	CWADItem *GetNextImage();
	CWADItem *GetFirstLump();
	CWADItem *GetNextLump();
	CWADItem *GetLumpAtPosition(int iPosition);
	CWADItem *AddImageToWAD (unsigned char *pbyBits[4], CWallyPalette* pPalette, LPCTSTR szName, int iWidth, int iHeight);
	void AddImageToWAD( CWADItem *pItem );
	void RemoveImageFromWAD (CWADItem *pItem);			
	void SaveWAD(CFile *pFile);
	void SaveWAD(LPCTSTR szFileName);
	void SaveART(CFile *pFile);
	void SaveART(LPCTSTR szFileName);
	CWADItem *IsNameInList (LPCTSTR szName);
	void SetWADType (int iType)
	{
		m_WADList.SetWADType (iType);
	}
	int GetWADType()
	{
		return m_WADList.GetWADType();
	}
	CWADList *GetWADList()
	{
		return &m_WADList;
	}

	// Build stuff
	CBuildItem *GetBuildTile (int iTileNumber);
	int GetFirstBuildTile ();
	int GetLastBuildTile ();

	// PNG stuff
	static void png_default_warning(png_structp png_ptr, png_const_charp message);
	static void png_default_error(png_structp png_ptr, png_const_charp message);
	static void png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
	static void png_default_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
	UINT GetPNGReadPosition() { return m_iPNGReadPosition; };
	void SetPNGReadPosition(UINT iPosition) { m_iPNGReadPosition = iPosition; };
	UINT GetPNGWritePosition() { return m_iPNGWritePosition; };
	void SetPNGWritePosition(UINT iPosition) { m_iPNGWritePosition = iPosition; };
	LPBYTE GetEncodedData() { return m_pbyEncodedData; };
	LPBYTE GetDecodedData() { return m_pbyDecodedData; };
	FILE *GetPNGWriteFile() { return m_fpPNGOutput;	};

	// JPG stuff
	static void		JPGErrorHandler (j_common_ptr cinfo);
	static void		JPGInitSource (j_decompress_ptr jDecompressInfo);
	static boolean	JPGFillInputBuffer (j_decompress_ptr jDecompressInfo);
	static void		JPGSkipInputData (j_decompress_ptr jDecompressInfo, long lNumBytes);
	static void		JPGTermSource (j_decompress_ptr jDecompressInfo);
	static void		JPGSetSource (j_decompress_ptr jDecompressInfo, BYTE *pbySourceData, UINT iDataSize);
	static void		JPGInitDestination (j_compress_ptr pjCompressInfo);
	static boolean	JPGEmptyOutputBuffer (j_compress_ptr pjCompressInfo);
	static void		JPGTermDestination (j_compress_ptr pjCompressInfo);
	static void		JPGSetDestination (j_compress_ptr pjCompressInfo);
	
	BOOL SetFileName (LPCTSTR szFileName);
	BOOL UseArchive() { return m_bUseArchive; };
	CArchive *GetArchive() { return m_parFile; };
	void SetImageType (int iType);	
	int GetImageType ();
	int GetImageFlag ();
	LPBYTE GetBits ();	
	LPBYTE GetPalette ();
	LPBYTE GetAlphaChannel ();

	void SetErrorCode (int iCode);	
	int GetErrorCode ();	
	void SetErrorText (LPCTSTR szText);
	CString GetErrorText( int iCode = IH_NONE );
	
	void SetImageWidth (int iWidth);	
	int GetImageWidth ();
	void SetImageHeight (int iHeight);
	int GetImageHeight ();

	int GetNumColors( void);
	int GetNumPlanes( void);
	int GetNumBitsPerPixelPlane( void);
	
	int GetFileLength ();
	UINT GetDataSize() { return m_iDataSize; };
	void SetDataSize( UINT iDataSize) { m_iDataSize = iDataSize; };
	
	PCXHEADER*        GetPCXHeader();
	BITMAPFILEHEADER* GetBMPHeader();
	BITMAPINFO*       GetBMPInfo();
	void SetColorDepth(int iDepth);
	int GetColorDepth();
	
	CWildCardItem *GetNextSupportedImage();
	CWildCardItem *GetFirstSupportedImage();
	CWildCardItem *GetFirstSupportedPackage();
	CWildCardItem *GetNextSupportedPackage();
	CWildCardItem *GetFirstSupportedPalette();
	CWildCardItem *GetNextSupportedPalette();
	CWildCardItem *GetFirstSupportedPak();
	CWildCardItem *GetNextSupportedPak();
	CWildCardItem *GetWildCardAt (int iPosition);

	CString GetNon16ImageList();
	CString GetSupportedImageList(BOOL bIncludePackages = FALSE);	
	CString GetSupportedWADList();
	CString GetSupportedPaletteList();
	CString GetWildCardExtension (int iIndex);

	BOOL IsValidImage (int iType = -1);
	BOOL IsValidPackage (int iType = -1);
	BOOL IsValidPalette (int iType = -1);
	BOOL IsValidPak (int iType = -1);
	BOOL IsNonGameType (LPCTSTR szFileName);
	BOOL IsGameType (int iType = -1);
	int GetTypeFlags (LPCTSTR szExtension);
	int GetTypeFlags (int iType = -1);
	void MapGameFlags (int iSourceType, unsigned char *pbyData, LPCTSTR szFileName = NULL);

	CString GetFileName()
	{
		return m_strFileName;
	}

	q2_miptex_s	*GetQ2Header()
	{
		return m_pQ2ExportHeader;
	}
	void CreateQ2Header();	
	q1_miptex_s	*GetQ1Header()
	{
		return m_pQ1ExportHeader;
	}
	void CreateQ1Header();

	LPM8_MIPTEX_S GetM8Header()
	{
		return (LPM8_MIPTEX_S)(m_pbyEncodedData);
	}

	COLORREF GetPaletteRGB( int iPalIndex)
	{
		return RGB( m_byPalette[iPalIndex*3], 
				m_byPalette[iPalIndex*3+1], m_byPalette[iPalIndex*3+2]);
	}	
};



#endif   // _IMAGE_HELPER_H_
