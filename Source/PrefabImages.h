////////////////////////////////////////////////////////////////////////////////////
//  PrefabImages.h
//  (c) 1998 Ty Matthews, all rights reserved
////////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined __PREFAB_IMAGES_H_
#define __PREFAB_IMAGES_H_

#define PI_MOUSE							0x1
#define PI_MOUSE_LEFT						0x2
#define PI_MOUSE_RIGHT						0x4
#define PI_MOUSE_DISABLED					0x8
#define PI_MOUSE_RBUTTON_DISABLED			0x10
#define PI_MOUSE_LEFT_RBUTTON_DISABLED		0x20
#define PI_BUILDVIEW_BUTTON					0x40
#define PI_WAD_THUMBNAIL					0x80
#define PI_WALLY_LOGO						0x100


// PCX header struct

typedef struct
{
    char			manufacturer;
    char			version;
    char			encoding;
    char			bits_per_pixel;
    unsigned short  xmin,ymin,xmax,ymax;
    unsigned short  hres,vres;
    unsigned char   palette[48];
    char			reserved;
    char			color_planes;
    unsigned short  bytes_per_line;
    unsigned short  palette_type;
    char			filler[58];
    unsigned char   data;                   // unbounded
} pcx_t;

#if !defined __PCX_DEFINITION_
#define __PCX_DEFINITION_

#define PCXFILLER        (128-74)		// 54


// PCXHEADER - PCX File Header

typedef struct
{
	BYTE  byManufacturer;		// keyword (10 decimal)
	BYTE  byHardware;			// hardware series (5, old files with no palette are 3)
	BYTE  byEncoding;			// run length encoded (1)
	BYTE  byBitsPerPixelPlane;	// bits per pixel per plane
	WORD  wX1;					// picture dimensions in pixels
	WORD  wY1;
	WORD  wX2;
	WORD  wY2;
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
} PCXHEADER;

// PCX data type defines
#define PCX_TYPE_24BIT	0
#define PCX_TYPE_8BIT	1
#define RED				0
#define GREEN			1
#define BLUE			2

#endif		// __PCX_DEFINITION_

////////////////////////////////////////////////////////////////////////////////////
//	Class Declarations

class CPrefabImage
{
// Members
private:
	LPBYTE p_DecodedData;
	LPBYTE p_EncodedData;
	BYTE m_byPalette[768];
	int m_iWidth;
	int m_iHeight;
	int m_iRawDataLength;

// Methods
public:
	CPrefabImage();
	~CPrefabImage();
	
	void CreatePrefabImage( int iType, unsigned char **p_ucData, unsigned char **p_ucPalette, DWORD dwSysColor = COLOR_BTNFACE);
	void CreatePrefabImageFromID( int iResourceID, BYTE **pbyData, BYTE **pbyPalette, DWORD dwSysColor = COLOR_BTNFACE);
	bool DecodePCX( DWORD dwSysColor );
	unsigned char *GetBits ();
	unsigned char *GetPalette ();	
	void SetImageWidth (int iWidth);
	int GetImageWidth ();
	void SetImageHeight (int iHeight);
	int GetImageHeight ();
	int GetRawDataLength ();
	void SetRawDataLength (int iSize);
};

#endif   //  __PREFAB_IMAGES_H_


