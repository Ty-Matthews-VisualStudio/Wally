// Preliminary WLY type structure


// WallyHeader->Flags
#define		WLY_FLAG_READ_ONLY		0x1

//			0123456789012345

#define		WC_UNKNOWN					-1
#define		WI_UNKNOWN					-1

// Recognized Type Categories  (WC_)
#define		ITEM_CATEGORY_START			0
#define		WC_RESERVED					(ITEM_CATEGORY_START + 1)
#define		WC_IMAGES					(ITEM_CATEGORY_START + 2)
#define		WC_GAME_TEXTURES			(ITEM_CATEGORY_START + 3)
#define		WC_PALETTES					(ITEM_CATEGORY_START + 4)
#define		WC_DEVELOPER				(ITEM_CATEGORY_START + 5)

// Recognized reserved types  (WR_)
#define		WR_TYPE_START				0
#define		WR_IRGB_DATA				(WR_TYPE_START + 1)
#define		WR_BYTE_PALETTE				(WR_TYPE_START + 2)
#define		WR_MIP_EFFECTS				(WR_TYPE_START + 3)
#define		WR_UNDO						(WR_TYPE_START + 4)

// Recognized image types (WI_)
#define		WI_TYPE_START				0
#define		WI_AMIGA_IFF				(WI_TYPE_START + 1)
#define		WI_AUTODESK_DXF				(WI_TYPE_START + 2)
#define		WI_COMPUSERVE_GIF			(WI_TYPE_START + 3)
#define		WI_COMPUTER_METAFILE_CGM	(WI_TYPE_START + 4)
#define		WI_COREL_CLIPART_CMX		(WI_TYPE_START + 5)
#define		WI_CORELDRAW_CDR			(WI_TYPE_START + 6)
#define		WI_DELUXE_PAINT_LBM			(WI_TYPE_START + 7)
#define		WI_DRHALO_CUT				(WI_TYPE_START + 8)
#define		WI_GEM_PAINT_IMG			(WI_TYPE_START + 9)
#define		WI_HP_GL_HGL				(WI_TYPE_START + 10)
#define		WI_JPEG_JPG					(WI_TYPE_START + 11)
#define		WI_JFIF_JIF					(WI_TYPE_START + 12)
#define		WI_KODAK_DIGITAL_KDC		(WI_TYPE_START + 13)
#define		WI_KODAK_FLASHPIX_FPX		(WI_TYPE_START + 14)
#define		WI_KODAK_PHOTOCD_PCD		(WI_TYPE_START + 15)
#define		WI_LOTUS_PIC				(WI_TYPE_START + 16)
#define		WI_MACINTOSH_PCT			(WI_TYPE_START + 17)
#define		WI_MACPAINT_MAC				(WI_TYPE_START + 18)
#define		WI_MICROGRAFX_DRW			(WI_TYPE_START + 19)
#define		WI_MS_PAINT_MSP				(WI_TYPE_START + 20)
#define		WI_PAINTSHOP_PSP			(WI_TYPE_START + 21)
#define		WI_PCPAINT_PIC				(WI_TYPE_START + 22)
#define		WI_PHOTOSHOP_PSD			(WI_TYPE_START + 23)
#define		WI_PORTABLE_BITMAP_PBM		(WI_TYPE_START + 24)
#define		WI_PORTABLE_GREYMAP_PGM		(WI_TYPE_START + 25)
#define		WI_PORTABLE_NETWORK_PNG		(WI_TYPE_START + 26)
#define		WI_PORTABLE_PIXELMAP_PPM	(WI_TYPE_START + 27)
#define		WI_RAW_RAW					(WI_TYPE_START + 28)
#define		WI_SCITEX_SCT				(WI_TYPE_START + 29)
#define		WI_SUN_RASTER_RAS			(WI_TYPE_START + 30)
#define		WI_TAGGED_IMAGE_TIF			(WI_TYPE_START + 31)
#define		WI_TRUEVISION_TARGA_TGA		(WI_TYPE_START + 32)
#define		WI_GEM_DRAWING_GEM			(WI_TYPE_START + 33)
#define		WI_WINDOWS_CLIPBOARD_CLP	(WI_TYPE_START + 34)
#define		WI_WINDOWS_ENH_METAFILE_EMF	(WI_TYPE_START + 35)
#define		WI_WINDOWS_METAFILE_WMF		(WI_TYPE_START + 36)
#define		WI_COMPUSERVE_RLE			(WI_TYPE_START + 37)
#define		WI_WINDOWS_BMP				(WI_TYPE_START + 38)
#define		WI_WINDOWS_DIB				(WI_TYPE_START + 39)
#define		WI_WORDPERFECT_WPG			(WI_TYPE_START + 40)
#define		WI_ZSOFT_MULTIPAGE_ZCX		(WI_TYPE_START + 41)
#define		WI_ZSOFT_PAINTBRUSH_PCX		(WI_TYPE_START + 42)

// Recognized game texture types (WG_)
#define		WG_TYPE_START				0
#define		WG_QUAKE2_WAL				(WG_TYPE_START + 1)
#define		WG_QUAKE1_MIP				(WG_TYPE_START + 2)
#define		WG_QUAKE1_WAD				(WG_TYPE_START + 3)
#define		WG_HALFLIFE_WAD				(WG_TYPE_START + 4)
#define		WG_SIN_SWL					(WG_TYPE_START + 5)


// Recognized palette types  (WP_)
#define		WP_TYPE_START				0
#define		WP_JASC_PAL					(WP_TYPE_START + 1)
#define		WP_PHOTOSHOP_ACT			(WP_TYPE_START + 2)
#define		WP_BYTE_LMP					(WP_TYPE_START + 3)

struct WallyHeader
{
	char Identification[6];			// Should be WALLY, NULL terminated
	int Version;					// Version 1 right now :)
	int NumTypeEntries;				// How many types are included
	int TypeOffset;					// Where to locate the types
	int NumDirectoryEntries;		// Number of items in this file
	int DirectoryOffset;			// Offset from the beginning of the file
	int Flags;						// Read-only flag, etc
};

struct WallyTypeEntry
{
	char Identification[64];		// Name to associate with this type
	int ItemType;					// Corresponds to ItemType in the Directory Entry
};

struct WallyDirectoryEntry
{
	int ItemType;					// What is it?
	int ItemSize;					// How much file space does it take up?
	int ItemFlags;					// Any special flags?
	int ItemOffset;					// Where can we find it?	
};



/*

The basic concept behind the Wally (WLY) file type is that it can contain
just about anything.  Normal PCX or BMP files, palettes, WAD files, custom
WAD files, you name it.  Each entry that is put into a WLY document is considered
one item.

The first thing to be read is the WallyHeader.  Determine whether the file is a true
WLY document by comparing the first 6 bytes to the NULL terminated string "WALLY":

	57	41	4C	4C	59	00
	W	A	L	L	Y	\0

Read in the entire WallyHeader and determine whether or not the version is one you 
currently support.  The DirectoryOffset value is the number of bytes from the beginning
of the file that the Directory can be found.  The directory is a list of every item in 
the file.  The NumEntries member tells you how many items are actually stored in this
WLY document.

Every item in a WLY file must have a directory entry.  The directory entry structure
defines exactly what the item is (its type,) how much file space it is using, and 
the item's offset.

Each directory entry must have a valid type.  The ItemType member of the directory
entry struct matches that in the TypeEntry struct.  The high-order WORD of ItemType
defines what category the item belongs in.  The low-order WORD is the unique ID for 
that item in the category.




*/