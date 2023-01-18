#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Definition for MIPLEVELS
#if !defined MIPLEVELS
#define MIPLEVELS 4
#endif

#if !defined __PCX_DEFINITION_
#define __PCX_DEFINITION_

#define PCXFILLER        (128-74)		// 54

// PCX header struct
/*
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
*/

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

#endif	// __PCX_DEFINITION_


// MIP Header (Quake1) struct
struct q1_miptex_s
{
	char			name[16];
	unsigned		width, height;
	unsigned		offsets[MIPLEVELS];		// four mip maps stored
};  


// WAL Header (Quake2) struct
struct  miptex_s
{
    char            name[32];
    unsigned        width, height;
    unsigned        offsets[MIPLEVELS];      // four mip maps stored
    char            animname[32];            // next frame in animation chain
    int             flags;
    int             contents;
    int             value;
};


// Bitmap stuff
#define BMH	('B' + ('M'<<8))