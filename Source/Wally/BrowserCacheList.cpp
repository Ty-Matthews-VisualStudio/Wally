// BrowserCacheList.cpp: implementation of the CBrowserCacheList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "BrowserCacheList.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "WallyPal.h"
#include "SortStringArray.h"
#include "BrowseDoc.h"
#include "BrowserCacheThread.h"
#include "ColorOpt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CBrowserCacheItem Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrowserCacheItem::CBrowserCacheItem()
{
	m_pbyData = NULL;
	m_pbyImageData = NULL;
	m_pbyPalette = NULL;
	m_lpItemHeader = NULL;

	m_iWidth = 0;
	m_iHeight = 0;
	m_iThumbnailWidth = 0;
	m_iThumbnailHeight = 0;
	m_iImageType = 0;
	
	SetErrorCode (CI_SUCCESS);
	SetSelected (FALSE);
	SetNext (NULL);
	SetPrevious (NULL);
}

CBrowserCacheItem::~CBrowserCacheItem()
{
	if (m_pbyData)
	{
		delete [] m_pbyData;
		m_pbyData = NULL;
	}

	SetNext (NULL);
	SetPrevious (NULL);
}

void CBrowserCacheItem::SetNext (CBrowserCacheItem *pNext)
{
	m_pNext = pNext;
}

CBrowserCacheItem *CBrowserCacheItem::GetNext()
{
	return m_pNext;
}

void CBrowserCacheItem::SetPrevious (CBrowserCacheItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CBrowserCacheItem *CBrowserCacheItem::GetPrevious()
{
	return m_pPrevious;
}

BOOL CBrowserCacheItem::Create (LPCTSTR szFileName)
{	
	ASSERT (m_pbyData == NULL);
	ASSERT (szFileName);

	int iHeaderSize = 0;
	int iDataSize = 0;	
	int iFileNameLength = 0;
	int iWidth = 0; 
	int iHeight = 0; 
	int iColorDepth = 0; 
	
	int iThumbnailWidth = 0;
	int iThumbnailHeight = 0;
	BYTE *pbyThumbnailData = NULL;
	int iImageSize = 0;
	
	CImageHelper ihHelper;

	// See what type it is without actually opening anything
	if (!ihHelper.SetFileName (szFileName))
	{
		SetErrorCode (CI_ERROR_IMAGE_HELPER);
		return FALSE;
	}

	// Get the time stamp
	CFile ImageFile;
	CFileException e;
	CFileStatus FileStatus;
	
	if( !ImageFile.Open( szFileName, CFile::modeRead, &e ) )
	{
		//e.m_cause
		SetErrorCode (CI_ERROR_FAIL_TO_OPEN);
		return FALSE;	
	}

	ImageFile.GetStatus(FileStatus);
	ImageFile.Close();

	m_strFileName = GetRawFileNameWExt (szFileName);
	
	int iLength = m_strFileName.GetLength();
	LPCTSTR lpFileName = m_strFileName.GetBuffer(iLength);

	iFileNameLength = PadDWORD (iLength + 1);
	iHeaderSize = CACHE_ITEM_HEADER_SIZE + iFileNameLength;

	// What type of image is it... don't load any package files (would take too long!)
	int iImageType = ihHelper.GetImageType();
		
	if (ihHelper.IsValidPackage(iImageType))
	{		
		iDataSize = iHeaderSize;
		m_pbyData = new BYTE[iDataSize];

		if (!m_pbyData)
		{
			return FALSE;
		}
		
		memset (m_pbyData, 0, iDataSize);
		m_lpItemHeader = (LPCACHE_ITEM)m_pbyData;
			
		m_lpItemHeader->iVersion = CURRENT_CACHE_VERSION;
		m_lpItemHeader->iHeaderSize = iHeaderSize;
		m_lpItemHeader->iImageType = ihHelper.GetImageType();
		m_lpItemHeader->iFileNameLength = iFileNameLength;

		memcpy (&m_lpItemHeader->szFileName, lpFileName, iLength);
		//strcpy ((char *)m_lpItemHeader->szFileName, szFileName);

		m_lpItemHeader->lModifyTime = (long)(FileStatus.m_mtime.GetTime());

		return TRUE;
	}

	// It's not a package, go load it up
	ihHelper.LoadImage (szFileName);

	if (ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		SetErrorCode (CI_ERROR_IMAGE_HELPER);
		return FALSE;
	}

	if (ihHelper.IsValidPalette(iImageType))
	{		
		iThumbnailWidth  = 64;
		iThumbnailHeight = 64;

		iImageSize = iThumbnailWidth * iThumbnailHeight;
		iDataSize  = iHeaderSize + iImageSize + 768;
		
		m_pbyData = new BYTE[iDataSize];

		if (!m_pbyData)
		{
			return FALSE;
		}

		memset (m_pbyData, 0, iDataSize);

		m_iThumbnailWidth  = iThumbnailWidth;
		m_iThumbnailHeight = iThumbnailHeight;
		m_iImageType       = iImageType;
	
		m_lpItemHeader = (LPCACHE_ITEM)m_pbyData;

		m_lpItemHeader->iVersion    = CURRENT_CACHE_VERSION;
		m_lpItemHeader->iHeaderSize = iHeaderSize;
		m_lpItemHeader->iDataSize   = iImageSize;

		m_lpItemHeader->iBitDepth        = 8;
		m_lpItemHeader->iWidth           = iThumbnailWidth;
		m_lpItemHeader->iHeight          = iThumbnailHeight;
		m_lpItemHeader->iThumbnailWidth  = iThumbnailWidth;
		m_lpItemHeader->iThumbnailHeight = iThumbnailHeight;

		m_lpItemHeader->iImageType      = iImageType;
		m_lpItemHeader->iFileNameLength = iFileNameLength;
		m_lpItemHeader->lModifyTime     = (long)(FileStatus.m_mtime.GetTime());

		memcpy (&m_lpItemHeader->szFileName, lpFileName, iLength);
	
		m_lpItemHeader->iDataOffset = iHeaderSize;
		m_pbyImageData = m_pbyData + m_lpItemHeader->iDataOffset;

		m_lpItemHeader->iPaletteOffset    = iHeaderSize + iImageSize;
		m_lpItemHeader->iNumPaletteColors = 256;
		m_pbyPalette = m_pbyData + m_lpItemHeader->iPaletteOffset;
	
		memcpy (m_pbyPalette, ihHelper.GetBits(), 768);

		int x = 0;
		int y = 0;
		
		for (y = 0; y < 16; y++)
		{
			for (x = 0; x < 16; x++)
			{
				memset (m_pbyImageData + (x * 4) + (y * 64 * 4), (y * 16) + x, 4);
				memset (m_pbyImageData + (x * 4) + (y * 64 * 4) + 64, (y * 16) + x, 4);
				memset (m_pbyImageData + (x * 4) + (y * 64 * 4) + 128, (y * 16) + x, 4);
				memset (m_pbyImageData + (x * 4) + (y * 64 * 4) + 192, (y * 16) + x, 4);
			}
		}

		return TRUE;
	}
	

	iDataSize =  iHeaderSize;
	iWidth  = ihHelper.GetImageWidth();		
	iHeight = ihHelper.GetImageHeight();
	iColorDepth = ihHelper.GetColorDepth();
	BYTE byPalette[256 * 3];
	
	BOOL bResized = TRUE;

	int iPercent = 0;

	if (max (iWidth, iHeight) <= 32)
	{
		// Just double the size, don't expand to 96x96
		iPercent = 200;
	}
	else
	{
		iPercent = (MAX_THUMBNAIL_SIZE * 100) / max (iWidth, iHeight);
	}
	
	switch (iColorDepth)
	{
	case IH_8BIT:
		{
			if (iPercent != 100)
			{
				ResizeImage256 (iWidth, iHeight, &iThumbnailWidth, &iThumbnailHeight, iPercent, ihHelper.GetBits(), &pbyThumbnailData, ihHelper.GetPalette());
				
				if (!pbyThumbnailData)
				{
					return FALSE;
				}
			}
			else
			{
				bResized         = FALSE;
				iThumbnailWidth  = iWidth;
				iThumbnailHeight = iHeight;
			}

			iImageSize = iThumbnailWidth * iThumbnailHeight;

			iDataSize += iImageSize;			// For data
			iDataSize += 768;					// For palette

			memcpy (byPalette, ihHelper.GetPalette(), 768);
		}
		break;

	case IH_24BIT:
		{
			if (iPercent != 100)
			{
				ResizeImage24Bit (iWidth, iHeight, &iThumbnailWidth, &iThumbnailHeight, iPercent, ihHelper.GetBits(), &pbyThumbnailData);
				
				if (!pbyThumbnailData)
				{
					return FALSE;
				}
			}
			else
			{
				bResized         = FALSE;
				iThumbnailWidth  = iWidth;
				iThumbnailHeight = iHeight;
			}

			// Neal - now show 24 bit
/*
			iImageSize = iThumbnailHeight * iThumbnailWidth;

			// Let's bring it down to 256 colors, just to save some space.  It's a thumbnail,
			// afterall, so we're not exactly concerned about image purity.
			CWallyPalette Palette;			
			COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iImageSize * sizeof( COLOR_IRGB));			
			CColorOptimizer ColorOpt;
			int r, g, b, j;
			
			for (j = 0; j < iImageSize; j++)
			{
				r = pbyThumbnailData[j * 3 + 0];
				g = pbyThumbnailData[j * 3 + 1];
				b = pbyThumbnailData[j * 3 + 2];
				
				pTemp24Bit[j] = IRGB( 0, r, g, b);				
			}
			
			ColorOpt.Optimize( pTemp24Bit, iThumbnailWidth, iThumbnailHeight, byPalette, 256, TRUE);
			
					
			// SetPalette ASSERTs with anything other than 256 colors
			Palette.SetPalette( byPalette, 256);

			Palette.Convert24BitTo256Color( pTemp24Bit, pbyThumbnailData, 
						iThumbnailWidth, iThumbnailHeight, 0, GetDitherType(), FALSE);

			if (pTemp24Bit)
			{
				free( pTemp24Bit);
				pTemp24Bit = NULL;
			}
*/			
			iImageSize = iThumbnailHeight * PadDWORD( iThumbnailWidth * 3);		// For 24 bit
			iDataSize += iImageSize;		// For data
			//iDataSize += 768;				// For palette
		}
		break;

	case IH_32BIT:
		{
//break;
			if (iPercent != 100)
			{
				ResizeImage32Bit (iWidth, iHeight, &iThumbnailWidth, &iThumbnailHeight, iPercent, ihHelper.GetBits(), &pbyThumbnailData);
				
				if (!pbyThumbnailData)
				{
					return FALSE;
				}
			}
			else
			{
				bResized         = FALSE;
				iThumbnailWidth  = iWidth;
				iThumbnailHeight = iHeight;
			}

			// Neal - now show 24 bit
/*
			iImageSize = iThumbnailHeight * iThumbnailWidth;

			// Let's bring it down to 256 colors, just to save some space.  It's a thumbnail,
			// afterall, so we're not exactly concerned about image purity.
			CWallyPalette Palette;			
			COLOR_IRGB* pTemp32Bit = (COLOR_IRGB* )malloc( iImageSize * sizeof( COLOR_IRGB));			
			CColorOptimizer ColorOpt;
			int r, g, b, a, j;

#ifdef _DEBUG
			int ix = 0;
			int iy = 0;
#endif
			
			for (j = 0; j < iImageSize; j++)
			{
				r = pbyThumbnailData[j * 4 + 0];
				g = pbyThumbnailData[j * 4 + 1];
				b = pbyThumbnailData[j * 4 + 2];
				a = pbyThumbnailData[j * 4 + 3];	// alpha channel

				// Neal - this will show alpha channel as black

				pTemp32Bit[j] = IRGB( 0, r*a/255, g*a/255, b*a/255);

#ifdef _DEBUG
				if (((iy & 7) == 0) && (((ix) & 7) == 0))	// only show every eighth pixel
				{
					if (a == 0)
						OutputDebugString( ".");
					else if (a < 64)
						OutputDebugString( "!");
					else if (a < 128)
						OutputDebugString( "+");
					else if (a < 192)
						OutputDebugString( "*");
					else
						OutputDebugString( "@");
				}

				ix++;
				if (ix == iThumbnailWidth)
				{
					if ((iy & 7) == 0)	// only show every eighth line
					{
						OutputDebugString( "\n");
					}
					ix = 0;
					iy++;
				}
#endif
			}
			
			ColorOpt.Optimize( pTemp32Bit, iThumbnailWidth, iThumbnailHeight, byPalette, 256, TRUE);
			
					
			// SetPalette ASSERTs with anything other than 256 colors
			Palette.SetPalette( byPalette, 256);

			Palette.Convert24BitTo256Color( pTemp32Bit, pbyThumbnailData, 
						iThumbnailWidth, iThumbnailHeight, 0, GetDitherType(), FALSE);

			if (pTemp32Bit)
			{
				free( pTemp32Bit);
				pTemp32Bit = NULL;
			}
*/			
			iImageSize = iThumbnailHeight * iThumbnailWidth * 4;	// For 32 bit
			iDataSize += iImageSize;		// For data
			//iDataSize += 768;				// For palette
		}
		break;

	default:
		ASSERT( FALSE);		// unhandled color depth
		return FALSE;
	}

	m_pbyData = new BYTE[iDataSize];

	if (!m_pbyData)
	{
		SetErrorCode (CI_ERROR_OUT_OF_MEMORY);
		return FALSE;
	}

	// Set the class member variables
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iThumbnailWidth = iThumbnailWidth;
	m_iThumbnailHeight = iThumbnailHeight;
	m_iImageType = ihHelper.GetImageType();
	
	memset (m_pbyData, 0, iDataSize);

	m_lpItemHeader = (LPCACHE_ITEM)m_pbyData;

	m_lpItemHeader->iVersion         = CURRENT_CACHE_VERSION;
	m_lpItemHeader->iHeaderSize      = iHeaderSize;
	m_lpItemHeader->iDataSize        = iImageSize;

	m_lpItemHeader->iBitDepth        = iColorDepth;
	m_lpItemHeader->iWidth           = iWidth;
	m_lpItemHeader->iHeight          = iHeight;
	m_lpItemHeader->iThumbnailWidth  = iThumbnailWidth;
	m_lpItemHeader->iThumbnailHeight = iThumbnailHeight;

	m_lpItemHeader->iImageType       = ihHelper.GetImageType();
	m_lpItemHeader->iFileNameLength  = iFileNameLength;
	m_lpItemHeader->lModifyTime      = (long)(FileStatus.m_mtime.GetTime());

	memcpy (&m_lpItemHeader->szFileName, lpFileName, iLength);
	
	m_lpItemHeader->iDataOffset = iHeaderSize;
	m_pbyImageData = m_pbyData + m_lpItemHeader->iDataOffset;

	if (bResized)
	{
		memcpy (m_pbyImageData, pbyThumbnailData, iImageSize);

		if (pbyThumbnailData)
		{
			delete []pbyThumbnailData;
			pbyThumbnailData = NULL;
		}
	}
	else
	{
		memcpy (m_pbyImageData, ihHelper.GetBits(), iImageSize);
	}

	m_lpItemHeader->iPaletteOffset = iHeaderSize + iImageSize;

	// Neal - now supports palettes other than 256
	if (iColorDepth <= IH_8BIT)
	{
		m_lpItemHeader->iNumPaletteColors = ihHelper.GetNumColors();
		m_pbyPalette = m_pbyData + m_lpItemHeader->iPaletteOffset;

		memcpy (m_pbyPalette, byPalette, 3*m_lpItemHeader->iNumPaletteColors);
	}
	else
	{
		m_lpItemHeader->iPaletteOffset = 0;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//	CBrowserCacheItem::Create()
//
//		pbyBuffer is the buffer for just the item... NOT the entire cache file
//
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CBrowserCacheItem::Create (BYTE *pbyBuffer, LPCTSTR szParentDirectory)
{
	CImageHelper ihHelper;
	ASSERT (m_pbyData == NULL);
	ASSERT (pbyBuffer);

	LPCACHE_ITEM lpItemHeader = (LPCACHE_ITEM)pbyBuffer;

	switch (lpItemHeader->iVersion)
	{
	case 1:
		{
			int iImageSize   = 0;
			int iHeaderSize  = lpItemHeader->iHeaderSize;
			int iDataSize    = iHeaderSize;
			int iImageType   = lpItemHeader->iImageType;

			char *szFileName = &lpItemHeader->szFileName;
			
			m_strFileName = szFileName;

			if (ihHelper.IsValidPackage (iImageType))
			{				
				m_pbyData = new BYTE[iDataSize];

				if (!m_pbyData)
				{
					SetErrorCode (CI_ERROR_OUT_OF_MEMORY);
					return FALSE;
				}
				
				memset (m_pbyData, 0, iDataSize);
				m_lpItemHeader = (LPCACHE_ITEM)m_pbyData;
				
				memcpy ((BYTE *)m_lpItemHeader, (BYTE *)lpItemHeader, iHeaderSize);

				return TRUE;
			}

			// Get the time stamp
			CFile ImageFile;
			CFileException e;
			CFileStatus FileStatus;
			CString strFullPath ("");

			strFullPath = TrimSlashes (szParentDirectory) + "\\" + m_strFileName;
			
			if( !ImageFile.Open( strFullPath, CFile::modeRead, &e ) )
			{
				//e.m_cause
				SetErrorCode (CI_ERROR_FAIL_TO_OPEN);
				return FALSE;	
			}

			ImageFile.GetStatus(FileStatus);
			ImageFile.Close();

			if ((long)(FileStatus.m_mtime.GetTime()) != lpItemHeader->lModifyTime)
			{
				TRACE ("File %s was modified since it's creation date\n", strFullPath);
				return FALSE;
			}
			
			int iThumbnailWidth  = lpItemHeader->iThumbnailWidth;
			int iThumbnailHeight = lpItemHeader->iThumbnailHeight;

			// Set the class member variables
			m_iWidth = lpItemHeader->iWidth;
			m_iHeight = lpItemHeader->iHeight;			
			m_iImageType = lpItemHeader->iImageType;			
			
			m_iThumbnailWidth = iThumbnailWidth;
			m_iThumbnailHeight = iThumbnailHeight;
			
			switch (lpItemHeader->iBitDepth)
			{
			case 8:
				{
					iImageSize = iThumbnailHeight * PadDWORD( iThumbnailWidth);
					iDataSize += iImageSize;

					if ((lpItemHeader->iPaletteOffset != 0) && (lpItemHeader->iNumPaletteColors != 0))
					{
						iDataSize += (lpItemHeader->iNumPaletteColors * 3);
					}
				}
				break;

			case 24:
				{
					if (iThumbnailWidth == 94)		// TEST TEST TEST
					{
						int iBreakpoint = 0;
					}
					iImageSize = iThumbnailHeight * (PadDWORD( iThumbnailWidth * 3));
					iDataSize += iImageSize;
				}
				break;

			case 32:
				{
					iImageSize = iThumbnailHeight * (PadDWORD( iThumbnailWidth * 4));
					iDataSize += iImageSize;
				}
				break;

			default:
				SetErrorCode (CI_ERROR_UNSUPPORTED_BITDEPTH);
				return FALSE;
				break;
			}

			m_pbyData = new BYTE[iDataSize];
			memset (m_pbyData, 0, iDataSize);

			m_lpItemHeader = (LPCACHE_ITEM)m_pbyData;				
			memcpy ((BYTE *)m_lpItemHeader, (BYTE *)lpItemHeader, iHeaderSize);

			switch (m_lpItemHeader->iCompressionType)
			{
			case 0:		// No compression
				{
					m_pbyImageData = m_pbyData + m_lpItemHeader->iDataOffset;
					memcpy (m_pbyImageData, (pbyBuffer + m_lpItemHeader->iDataOffset), iImageSize);
					
					if ((m_lpItemHeader->iPaletteOffset != 0) && (m_lpItemHeader->iNumPaletteColors != 0))
					{
						m_pbyPalette = m_pbyData + m_lpItemHeader->iPaletteOffset;
						memcpy (m_pbyPalette, (pbyBuffer + m_lpItemHeader->iPaletteOffset), m_lpItemHeader->iNumPaletteColors * 3);
					}					
				}

			case 1:
				{
					m_pbyImageData = m_pbyData + iHeaderSize;
					
					DWORD dwUncompressedSize = 0;
					BYTE *pbyCompressedData = pbyBuffer + m_lpItemHeader->iDataOffset;
					BYTE *pbyDecompressedData = DecompressData (&pbyCompressedData, &dwUncompressedSize, FALSE);
					
					memcpy (m_pbyImageData, pbyDecompressedData, dwUncompressedSize);

					if (pbyDecompressedData)
					{
						delete []pbyDecompressedData;
						pbyDecompressedData = NULL;
					}

					// Neal - fix 24/32 bit
					if (m_lpItemHeader->iBitDepth > 8)
					{
						m_lpItemHeader->iPaletteOffset = 0;
						m_lpItemHeader->iNumPaletteColors = 0;
					}

					if ((m_lpItemHeader->iPaletteOffset != 0) && (m_lpItemHeader->iNumPaletteColors != 0))
					{
						m_pbyPalette = m_pbyData + iHeaderSize + dwUncompressedSize;
						memcpy (m_pbyPalette, (pbyBuffer + m_lpItemHeader->iPaletteOffset), m_lpItemHeader->iNumPaletteColors * 3);
					}
				}
				break;

			default:
				SetErrorCode (CI_ERROR_UNSUPPORTED_COMPRESSION);
				return FALSE;
				break;
			}
		}
		break;

	default:
		// Unhandled version... must be a newer cache file.
		return FALSE;
		break;		
	}	

	return TRUE;
}

BOOL CBrowserCacheItem::RecreateThumbnail (int iWidth /* = 0 */, int iHeight /* = 0 */)
{
	return TRUE;
}

BYTE *CBrowserCacheItem::GetData()
{
	return m_pbyData;
}

int CBrowserCacheItem::GetTotalItemSize()
{
	return  m_lpItemHeader->iHeaderSize + m_lpItemHeader->iDataSize + (m_lpItemHeader->iNumPaletteColors * 3);	
}

BOOL CBrowserCacheItem::Serialize (FILE *wp, int *piPosition)
{
	if ((m_lpItemHeader->iThumbnailWidth == 0) && (m_lpItemHeader->iThumbnailHeight == 0))
	{
		// This is a WAD file... can't compress data because there is none
		fseek (wp, (*piPosition), SEEK_SET);

		int iHeaderSize = m_lpItemHeader->iHeaderSize;
		fwrite ((BYTE *)m_lpItemHeader, 1, iHeaderSize, wp);
		
		(*piPosition) += iHeaderSize;
		return TRUE;
	}

	int iBytes = m_lpItemHeader->iBitDepth / 8;		// Neal - for 24/32 bit

	DWORD dwImageSize = m_lpItemHeader->iThumbnailHeight * m_lpItemHeader->iThumbnailWidth * iBytes;
	DWORD dwCompressedSize = 0;

	BYTE *pbyCompressed = CompressData( &m_pbyImageData, dwImageSize, &dwCompressedSize, FALSE);
	
	if (!pbyCompressed)
	{
		return FALSE;
	}
	
	int iHeaderSize = m_lpItemHeader->iHeaderSize;

	// Build a temporary header so we can change stuff
	BYTE *pbyHeader = new BYTE[iHeaderSize];

	if (!pbyHeader)
	{
		if (pbyCompressed)
		{
			delete []pbyCompressed;
			pbyCompressed = NULL;
		}
		return FALSE;
	}
	memcpy (pbyHeader, (BYTE *)m_lpItemHeader, iHeaderSize);

	LPCACHE_ITEM lpItemHeader = (LPCACHE_ITEM)pbyHeader;
	
	lpItemHeader->iDataSize = dwCompressedSize;
	lpItemHeader->iPaletteOffset = iHeaderSize + dwCompressedSize;
	lpItemHeader->iCompressionType = CACHE_ITEM_CURRENT_COMPRESSION;

	fseek (wp, (*piPosition), SEEK_SET);
	fwrite (pbyHeader, 1, iHeaderSize, wp);
	fwrite (pbyCompressed, 1, dwCompressedSize, wp);

	if (m_pbyPalette)
	{
		fwrite (m_pbyPalette, 1, (lpItemHeader->iNumPaletteColors * 3), wp);
	}

	(*piPosition) += iHeaderSize + dwCompressedSize + (lpItemHeader->iNumPaletteColors * 3);

	if (pbyCompressed)
	{
		delete []pbyCompressed;
		pbyCompressed = NULL;
	}

	if (pbyHeader)
	{
		delete []pbyHeader;
		pbyHeader = NULL;
	}

	return TRUE;
}

BYTE *CBrowserCacheItem::DecompressData(BYTE **pbyCompressedSource, DWORD *pdwDataSize, BOOL bFreeSrc /* = FALSE */)
{
	BYTE* lpSrc               = (*pbyCompressedSource);
	DWORD dwSize              = *(DWORD* )(lpSrc);
	DWORD dwCompressedSize    = *(DWORD* )(lpSrc + sizeof( DWORD));
	DWORD dwUncompressedSize  = dwSize;

	if (pdwDataSize)
	{
		(*pdwDataSize) = dwUncompressedSize;
	}

	//HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE, dwUncompressedSize);
	
	BYTE* lpDest = new BYTE[dwUncompressedSize];		

	if (lpDest)
	{		
		LzrwDecompress( lpSrc + 2 * sizeof( DWORD), dwCompressedSize, lpDest, &dwSize);

		if (bFreeSrc)
		{
			delete [](*pbyCompressedSource);
			(*pbyCompressedSource) = NULL;
		}		
	}

	return lpDest;
}


BYTE *CBrowserCacheItem::CompressData(BYTE **pbySource, DWORD dwOriginalSize, DWORD *pdwCompressedSize, BOOL bFreeSrc /* = FALSE */)
{
	BYTE* lpSrc               = (*pbySource);
	DWORD dwSize              = dwOriginalSize;		// GlobalSize() may be slightly too large
	DWORD dwUncompressedSize  = dwSize;

#define FUDGE 100		// fudge factor - compress can overrun buffer a little bit
						// (this should be plenty)

	BYTE *pbyDest = new BYTE[dwSize + FLAG_BYTES + 2 * sizeof( DWORD) + FUDGE];
	
	if (pbyDest)
	{		
		*(DWORD* )(pbyDest) = dwOriginalSize;	// save original size in block

		LzrwCompress( lpSrc, dwSize, pbyDest + 2 * sizeof( DWORD), &dwSize);

		*(DWORD* )(pbyDest + sizeof( DWORD)) = dwSize;		// save compressed size in block

		dwSize += 2 * sizeof( DWORD);  // return size of memory block (not just size of compressed data)

		if (pdwCompressedSize)
		{
			*pdwCompressedSize = dwSize;
		}
#ifdef _DEBUG

		// double-check - did data compress PERFECTLY?

		BYTE *pbyCompare = pbyDest;		// neal - just for clarity
		BYTE *pbyDecomp = DecompressData( &pbyCompare, &dwSize, FALSE);

		if (pbyDecomp)
		{			
			for (UINT j = 0; j < dwUncompressedSize; j++)
			{
				if (pbyDecomp[j] != (*pbySource)[j])
				{
					ASSERT( FALSE);
					break;
				}
			}

			delete []pbyDecomp;
			pbyDecomp = NULL;
		}	

#endif

		if (bFreeSrc)
		{
			delete [](*pbySource);
			(*pbySource) = NULL;
		}
	}

	return pbyDest;
}

void CBrowserCacheItem::SetErrorCode (int iCode)
{
	m_iErrorCode = iCode;
}

int CBrowserCacheItem::GetErrorCode()
{
	return m_iErrorCode;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBrowserCacheList::CBrowserCacheList()
{
	SetAll (NULL);
	SetErrorCode (CL_SUCCESS);
	m_lpCacheHeader = NULL;

	SetThreadMessage (CACHE_THREAD_MESSAGE_NONE);
	SetThreadStatus (CACHE_THREAD_STATUS_IDLE);

	m_pCacheThread = (CBrowserCacheThread *) AfxBeginThread ( (AFX_THREADPROC) CBrowserCacheThread::MainLoop, (LPVOID)this);
}

CBrowserCacheList::~CBrowserCacheList()
{
	SetThreadMessage (CACHE_THREAD_MESSAGE_STOP);

	while (GetThreadStatus() != CACHE_THREAD_STATUS_STOPPED)
	{
		Sleep (200);
	}

	Sleep (1000);

	PurgeList();
	if (m_lpCacheHeader)
	{
		free (m_lpCacheHeader);
		m_lpCacheHeader = NULL;
	}
}

void CBrowserCacheList::PurgeList()
{
	CBrowserCacheItem *pTemp = GetFirst();

	while (pTemp)
	{
		SetFirst(pTemp->GetNext());
		delete pTemp;
		pTemp = GetFirst();
	}

	pTemp = NULL;
	SetAll (NULL);
}

void CBrowserCacheList::SetFirst (CBrowserCacheItem *pFirst)
{
	m_pFirst = pFirst;
}

CBrowserCacheItem *CBrowserCacheList::GetFirst()
{
	return m_pFirst;
}

void CBrowserCacheList::SetWrite (CBrowserCacheItem *pWrite)
{
	m_pWrite = pWrite;
}

CBrowserCacheItem *CBrowserCacheList::GetWrite()
{
	return m_pWrite;
}

void CBrowserCacheList::SetAll (CBrowserCacheItem *pItem)
{
	SetFirst (pItem);
	SetWrite (pItem);
}

void CBrowserCacheList::SetErrorCode (int iCode)
{
	m_iErrorCode = iCode;
}

int CBrowserCacheList::GetErrorCode()
{
	return m_iErrorCode;
}

void CBrowserCacheList::SetThreadMessage (int iMessage)
{
	ASSERT (iMessage > CACHE_THREAD_MESSAGE_BEGIN);
	ASSERT (iMessage < CACHE_THREAD_MESSAGE_END);

	m_iCacheThreadMessage = iMessage;
}

int CBrowserCacheList::GetThreadMessage()
{
	return m_iCacheThreadMessage;
}

void CBrowserCacheList::SetThreadStatus (int iStatus)
{
	ASSERT (iStatus > CACHE_THREAD_STATUS_BEGIN);
	ASSERT (iStatus < CACHE_THREAD_STATUS_END);

	m_iCacheThreadStatus = iStatus;
}

int CBrowserCacheList::GetThreadStatus()
{
	return m_iCacheThreadStatus;
}

void CBrowserCacheList::ResetAllSelections()
{
	CBrowserCacheItem *pItem = GetFirstSelectedItem();

	while (pItem)
	{
		pItem->SetSelected(FALSE);

		pItem = GetNextSelectedItem();
	}
}

CBrowserCacheItem *CBrowserCacheList::GetFirstSelectedItem()
{
	m_pSelected = GetFirst();

	while (m_pSelected)
	{
		if (m_pSelected->IsSelected())
		{
			return m_pSelected;
		}
		m_pSelected = m_pSelected->GetNext();
	}

	return m_pSelected;
}

CBrowserCacheItem *CBrowserCacheList::GetNextSelectedItem()
{
	while (m_pSelected)
	{
		m_pSelected = m_pSelected->GetNext();
		
		if ((m_pSelected) && ( (m_pSelected != NULL) ? m_pSelected->IsSelected() : FALSE) )
		{
			return m_pSelected;
		}		
	}
	
	return m_pSelected;
}



CBrowserCacheItem *CBrowserCacheList::GetAtPosition (int iPosition)
{
	CBrowserCacheItem *pTemp = GetFirst();
	int iCounter = 0;

	while (pTemp)
	{
		if (iCounter == iPosition)
		{
			return pTemp;
		}
		
		iCounter++;
		pTemp = pTemp->GetNext();
	}

	return NULL;
}

int CBrowserCacheList::GetCount()
{
	CBrowserCacheItem *pTemp = GetFirst();
	int iCounter = 0;

	while (pTemp)
	{		
		iCounter++;
		pTemp = pTemp->GetNext();
	}

	return iCounter;
}

void CBrowserCacheList::RemoveItem(CBrowserCacheItem *pItem)
{
	ASSERT (pItem);

	CBrowserCacheItem *pTemp = GetFirst();

	while (pTemp)
	{
		if (pTemp == pItem)
		{
			if (pTemp->GetPrevious())
			{
				pTemp->GetPrevious()->SetNext(pTemp->GetNext());
			}

			if (pTemp->GetNext())
			{
				pTemp->GetNext()->SetPrevious(pTemp->GetPrevious());
			}

			if (pTemp == GetFirst())
			{
				SetFirst (pTemp->GetNext());
			}

			if (pTemp == GetWrite())
			{
				SetWrite (pTemp->GetPrevious());
			}

			if (m_pSelected == pTemp)
			{
				// Let's move the selected item to the next item so we don't 
				// accidentally access a deleted item
				m_pSelected = pTemp->GetNext();
			}

			delete pTemp;
			pTemp = NULL;
		}
		else
		{
			pTemp = pTemp->GetNext();
		}
	}
}

void CBrowserCacheList::AddItem(CBrowserCacheItem *pItem)
{
	ASSERT (pItem);
	CString strCompare1("");
	CString strCompare2(pItem->GetFileName());
	strCompare2.MakeLower();

	if (pItem)
	{
		pItem->SetParent (this);

		if (!GetFirst())
		{
			SetAll (pItem);
		}
		else
		{
			CBrowserCacheItem *pRead = GetFirst();
			CBrowserCacheItem *pTempLeft = NULL;
			CBrowserCacheItem *pTempRight = NULL;
		
			// Insert the item into the sorted list 
			while (pRead)
		    {
				strCompare1 = pRead->GetFileName();
				strCompare1.MakeLower();
	
		        if (strCompare1 > strCompare2)
				{                           
					pTempLeft = pRead->GetPrevious();
					pTempRight = pRead;
					pRead = NULL;
				}
				else
				{
					pRead = pRead->GetNext();
				}
			}
	        
			//  Item belongs at the end 
			if ((pTempRight == NULL) && (pTempLeft == NULL))
			{                   
	            GetWrite()->SetNext (pItem);
				pItem->SetPrevious (GetWrite());
	            SetWrite (pItem);
			}
	
			//  Item is the first in the list 
			if ((pTempLeft == NULL) && (pTempRight != NULL))
			{		    
				pItem->SetNext (GetFirst());
				GetFirst()->SetPrevious (pItem);
				SetFirst (pItem);
			}

			//  Item is somewhere in the middle of the list 
			if ((pTempLeft != NULL) && (pTempRight != NULL))
			{			
				pTempLeft->SetNext (pItem);
				pItem->SetPrevious (pTempLeft);
				pItem->SetNext (pTempRight);
				pTempRight->SetPrevious (pItem);
			}			
		}
	}
}

void CBrowserCacheList::Initialize(LPCTSTR szDirectory, LPVOID lpDoc, BOOL bRefresh /* = FALSE */)
{
	// Only continue if we're looking at a new directory.
	if (!m_strDirectory.CompareNoCase(szDirectory) && !bRefresh)
	{
		return;
	}

	CBrowseDoc *pDoc = (CBrowseDoc *)lpDoc;
		
	PurgeList();
	pDoc->ResetScrollbars(TRUE);

	// If we're currently cleaning up a file, stop the thread
	if (GetThreadStatus() == CACHE_THREAD_STATUS_CLEANING)
	{
		SetThreadMessage (CACHE_THREAD_MESSAGE_PAUSE);
		while (GetThreadStatus() != CACHE_THREAD_STATUS_IDLE)
		{
			Sleep (100);
		}
	}	

	m_strDirectory = szDirectory;
	m_strDirectory.MakeLower();

	CString strCacheFileName ("");
	CString strDiskFileName ("");
	CString strSearch ("");
	CString strDirectory ("");

	CSortStringArray saDiskFiles;
	CFileFind fFinder;

	// Build a list of all the files in this directory
	strSearch = TrimSlashes (m_strDirectory) + "\\*.*";
	
	BOOL bScanning = fFinder.FindFile(strSearch);
	BOOL bCleanup = FALSE;

	while (bScanning)
	{
		bScanning = fFinder.FindNextFile();
		if ( !fFinder.IsDirectory() && !fFinder.IsDots() )
		{
			saDiskFiles.Add (fFinder.GetFileName());
		}			
	}
	
	//saDiskFiles.Sort();

	// Go look to see if the cache file is already there
	strSearch = TrimSlashes (g_strBrowseCacheDirectory) + "\\*.*";
	
	BOOL bWorking = fFinder.FindFile(strSearch);
	BOOL bCacheFileFound = FALSE;

	while (bWorking)
	{
		bWorking = fFinder.FindNextFile();
		if ( !fFinder.IsDirectory() && !fFinder.IsDots() )
		{
			strCacheFileName = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + fFinder.GetFileName();
			strDirectory = TrimSlashes (FindDirNameFromFile(strCacheFileName));
			strDirectory.MakeLower();

			if (GetErrorCode() != CL_SUCCESS)
			{
				// Something serious happened... better stop
				return;
			}

			if (strDirectory == m_strDirectory)
			{
				// This is the cache file we're looking for
				TRACE ("Cache file is found\n");
				bWorking = FALSE;
				bCacheFileFound = TRUE;
				m_strCacheFileName = strCacheFileName;
			}
		}
	}

	if (!bCacheFileFound)
	{
		TRACE ("Cache file not found, going to build a new file, with header\n");
		// Time to create a new header
		if (!CreateCacheFile (m_strDirectory))
		{
			return;
		}
	}

	ASSERT (m_lpCacheHeader);

	// Now we've got a confirmed header in m_lpCacheHeader, and we can begin comparing
	// those files found in the directory with what's in the cache file

	FILE* rp = NULL;
	errno_t err = fopen_s( &rp, m_strCacheFileName, "r+b");
	if (err != 0)
	{
		SetErrorCode(CL_ERROR_FAIL_TO_OPEN);
		return;
	}
	int iPosition = m_lpCacheHeader->iFirstItemOffset;
	UINT j = 0;
	
	BYTE *pbyTempHeader = NULL;
	BYTE *pbyTempBuffer = NULL;
	BYTE byItemHeader[CACHE_ITEM_HEADER_SIZE];
	BYTE *pbyItemData	= NULL;

	LPCACHE_ITEM lpCacheItem = NULL;
	CBrowserCacheItem *pNewItem = NULL;
	
	int iNumThumbnails = 0; 
	int iItemHeaderSize = 0;
	int iTotalItemSize = 0;
	int iFileNameLength = 0;
	char *szFileName = NULL;
	int iStrPosition = 0;
	int iThreadMessage = 0;
	int iRefreshCounter = 0;

	DWORD dwCompressedSize = 0;

	BOOL bValidFile = TRUE;
	BOOL bDeleteItem = FALSE;

	for (j = 0; (j < m_lpCacheHeader->iNumThumbnails) && bValidFile; j++)
	{
		bDeleteItem = FALSE;

		// Clear out the partial header
		memset (byItemHeader, 0, CACHE_ITEM_HEADER_SIZE);

		// Go point at the item
		fseek (rp, iPosition, SEEK_SET);

		// Read the first four bytes so we can determine the header size
		fread (byItemHeader, 1, 4, rp);
		lpCacheItem = (LPCACHE_ITEM)byItemHeader;

		iItemHeaderSize = lpCacheItem->iHeaderSize;

		if ((iItemHeaderSize > 2048) || (iItemHeaderSize <= 0))
		{
			// Cache file is corrupt... yank it out
			fclose (rp);
			remove (m_strCacheFileName);
			bValidFile = FALSE;
			continue;
		}

		// Build the temp header
		pbyTempHeader = new BYTE[iItemHeaderSize];
		
		if (!pbyTempHeader)
		{
			SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
			fclose (rp);			
			return;
		}

		memset (pbyTempHeader, 0, iItemHeaderSize);

		fseek (rp, iPosition, SEEK_SET);
		fread (pbyTempHeader, 1, iItemHeaderSize, rp);

		lpCacheItem     = (LPCACHE_ITEM )pbyTempHeader;
		iFileNameLength = lpCacheItem->iFileNameLength;

		if ((iFileNameLength > 2048) || (iFileNameLength <= 0))
		{
			// Cache file is corrupt... yank it out
			fclose (rp);

			remove (m_strCacheFileName);
			bValidFile = FALSE;

			if (pbyTempHeader)
			{
				delete []pbyTempHeader;
				pbyTempHeader = NULL;
			}			
			continue;
		}

		szFileName = new char[iFileNameLength + 1];

		if (! szFileName)
		{
			SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
			fclose (rp);

			if (pbyTempHeader)
			{
				delete []pbyTempHeader;
				pbyTempHeader = NULL;
			}
			
			return;
		}
		
		memset (szFileName, 0, iFileNameLength + 1);
		memcpy ((BYTE *)szFileName, &lpCacheItem->szFileName, iFileNameLength);

		iTotalItemSize = iItemHeaderSize + lpCacheItem->iDataSize + (lpCacheItem->iNumPaletteColors * 3);

		if (lpCacheItem->iFlags & CIFLAG_DELETE)
		{
			// Item is flagged as being deleted... skip it.
			iPosition += iTotalItemSize;

			if (pbyTempHeader)
			{
				delete []pbyTempHeader;
				pbyTempHeader = NULL;
			}

			if (szFileName)
			{
				delete []szFileName;
				szFileName = NULL;
			}			
			continue;
		}

		iStrPosition = saDiskFiles.HasString (szFileName);

		if (iStrPosition != -1)
		{			
			// Go create a new item
			pNewItem = new CBrowserCacheItem;
			
			if (!pNewItem)
			{
				SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
				fclose (rp);
			
				if (pbyTempHeader)
				{
					delete []pbyTempHeader;
					pbyTempHeader = NULL;
				}

				if (szFileName)
				{
					delete []szFileName;
					szFileName = NULL;
				}				
				return;
			}

			pbyTempBuffer = new BYTE[iTotalItemSize];

			if (!pbyTempBuffer)
			{
				SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
				fclose (rp);
			
				if (pbyTempHeader)
				{
					delete []pbyTempHeader;
					pbyTempHeader = NULL;
				}
				
				if (szFileName)
				{
					delete []szFileName;
					szFileName = NULL;
				}				
				return;
			}
			
			memset (pbyTempBuffer, 0, iTotalItemSize);
			fseek (rp, iPosition, SEEK_SET);
			
			fread (pbyTempBuffer, 1, iTotalItemSize, rp);
			
			if (pNewItem->Create (pbyTempBuffer, szDirectory))
			{
				AddItem (pNewItem);
				
				// File is already in the cache and created; don't need to add again
				saDiskFiles.RemoveAt (iStrPosition);
				
				// Show that we're doing stuff
				iRefreshCounter++;

				if (iRefreshCounter > 2)
				{
					pDoc->RefreshView();
					Sleep (20);

					iRefreshCounter = 0;
				}				
			}
			else
			{
				delete pNewItem;
				pNewItem = NULL;
				bDeleteItem = TRUE;
			}

			if (pbyTempBuffer)
			{
				delete []pbyTempBuffer;
				pbyTempBuffer = NULL;
			}			
		}
		else
		{
			bDeleteItem = TRUE;
		}

		if (bDeleteItem)
		{
			// File is in the cache, but it's not in the directory anymore.  Flag it for deletion
			lpCacheItem->iFlags |= CIFLAG_DELETE;
			
			// Go point at the item
			fseek (rp, iPosition, SEEK_SET);

			fwrite (pbyTempHeader, 1, iItemHeaderSize, rp);

			bCleanup = TRUE;
		}

		iPosition += iTotalItemSize;

		if (pbyTempHeader)
		{
			delete []pbyTempHeader;
			pbyTempHeader = NULL;
		}

		if (szFileName)
		{
			delete []szFileName;
			szFileName = NULL;
		}

		iThreadMessage = pDoc->GetThreadMessage();

		if (iThreadMessage != BROWSE_DOC_MESSAGE_NONE)
		{
			// User is breaking the process or has selected a new directory... get out now			
			return;
		}		
	}
	
	if (!bValidFile)
	{
		TRACE ("Cache file was corrupt, going to build a new file, with header\n");
		// Time to create a new header
		if (!CreateCacheFile (m_strDirectory))
		{
			return;
		}
	
		err = fopen_s( &rp, m_strCacheFileName, "r+b");		
		if (err != 0)
		{
			SetErrorCode(CL_ERROR_FAIL_TO_WRITE);
			return;
		}

		iPosition = m_lpCacheHeader->iFirstItemOffset;

		bCleanup = FALSE;
	}

	// Set the access time
	time_t osBinaryTime;
	time( &osBinaryTime );

	m_lpCacheHeader->lAccessTime = (long)osBinaryTime;
	
	for (iStrPosition = 0; iStrPosition < saDiskFiles.GetSize(); iStrPosition++)
	{
		strDiskFileName = TrimSlashes (m_strDirectory) + "\\" + saDiskFiles.GetAt(iStrPosition);

		// Go create a new item
		pNewItem = new CBrowserCacheItem;

		if (!pNewItem)
		{
			fclose (rp);
			SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
			
			return;
		}

		if (pNewItem->Create (strDiskFileName))
		{
			AddItem (pNewItem);

			// Show that we're doing stuff
			pDoc->RefreshView();
			Sleep (20);

			// Write out the item to the cache file.  iPosition will reflect the new cache file length (or ending position)
			if (!pNewItem->Serialize(rp, &iPosition))
			{
				fclose (rp);
				SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
				
				return;
			}

			//iPosition += pNewItem->GetTotalItemSize();
			m_lpCacheHeader->iNumThumbnails++;
		}
		else
		{
			delete pNewItem;
			pNewItem = NULL;
		}

		iThreadMessage = pDoc->GetThreadMessage();

		if (iThreadMessage != BROWSE_DOC_MESSAGE_NONE)
		{
			// User is breaking the process or has selected a new directory... get out now

			// First, though, we need to write out all of the files that have been processed
			// to save some time the next time we hit this directory
			int iHeaderSize = m_lpCacheHeader->iHeaderSize;

			// Build the temp header
			pbyTempHeader = new BYTE[iHeaderSize];
				
			if (!pbyTempHeader)
			{
				SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
				fclose (rp);				
				return;
			}

			memset (pbyTempHeader, 0, iHeaderSize);
			memcpy (pbyTempHeader, (BYTE *)m_lpCacheHeader, iHeaderSize);

			fseek (rp, 0, SEEK_SET);
			fwrite (pbyTempHeader, 1, iHeaderSize, rp);

			if (pbyTempHeader)
			{
				delete []pbyTempHeader;
				pbyTempHeader = NULL;
			}
			
			fclose (rp);
			return;
		}		
	}

	int iHeaderSize = m_lpCacheHeader->iHeaderSize;

	// Build the temp header
	pbyTempHeader = new BYTE[iHeaderSize];
		
	if (!pbyTempHeader)
	{
		SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
		fclose (rp);
		return;
	}

	memset (pbyTempHeader, 0, iHeaderSize);
	memcpy (pbyTempHeader, (BYTE *)m_lpCacheHeader, iHeaderSize);

	fseek (rp, 0, SEEK_SET);
	fwrite (pbyTempHeader, 1, iHeaderSize, rp);

	if (pbyTempHeader)
	{
		delete []pbyTempHeader;
		pbyTempHeader = NULL;
	}
	
	fclose (rp);

	saDiskFiles.RemoveAll();
	pDoc->ResetScrollbars(TRUE);

	if (bCleanup)
	{
		SetThreadMessage (CACHE_THREAD_MESSAGE_CLEANUP);
	}
	else
	{
		SetThreadMessage (CACHE_THREAD_MESSAGE_GROOM);
	}
}

BOOL CBrowserCacheList::CreateCacheFile (LPCTSTR szDirectory)
{
	// Time to create a new header
	if (m_lpCacheHeader)
	{
		free (m_lpCacheHeader);
		m_lpCacheHeader = NULL;
	}

	CString strCacheFileName("");
	int iDirNameLength = PadDWORD (strlen (szDirectory) + 1);
	int iHeaderSize = CACHE_HEADER_SIZE + iDirNameLength;
	errno_t err = 0;
	
	m_lpCacheHeader = (LPCACHE_HEADER) malloc (iHeaderSize);
	
	if (!m_lpCacheHeader)
	{
		SetErrorCode (CL_ERROR_OUT_OF_MEMORY);
		return FALSE;
	}

	memset ((BYTE *)m_lpCacheHeader, 0, iHeaderSize);
	
	strcpy_s(m_lpCacheHeader->szId, sizeof(m_lpCacheHeader->szId), CACHE_SZID);
	m_lpCacheHeader->iVersion       = CURRENT_CACHE_VERSION;
	m_lpCacheHeader->iHeaderSize    = iHeaderSize;
	m_lpCacheHeader->iDirNameLength = iDirNameLength;
	memcpy (&m_lpCacheHeader->szDirName, (BYTE *)szDirectory, strlen (szDirectory));

	m_lpCacheHeader->iFirstItemOffset = iHeaderSize;
	
	time_t osBinaryTime;
	time( &osBinaryTime );

	m_lpCacheHeader->lAccessTime = (long)osBinaryTime;
	
	// Come up with a unique filename for this cache file
	FILE *wp = NULL;	
	strCacheFileName = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + GenerateRandomFileName (8);
	
	err = fopen_s( &wp, strCacheFileName, "r");
	while (err == 0)
	{
		fclose (wp);
		strCacheFileName = TrimSlashes (g_strBrowseCacheDirectory) + "\\" + GenerateRandomFileName (8);
		err = fopen_s(&wp, strCacheFileName, "r");
	}

	err = fopen_s(&wp, strCacheFileName, "wb");	
	if (err != 0)
	{
		SetErrorCode (CL_ERROR_FAIL_TO_WRITE);
		return FALSE;
	}

	fwrite ((BYTE *)m_lpCacheHeader, 1, iHeaderSize, wp);
	fclose (wp);

	m_strCacheFileName = strCacheFileName;

	return TRUE;
}

CString CBrowserCacheList::GenerateRandomFileName(int iLength)
{
	srand( (unsigned)time( NULL ) );
	
	div_t dt;
	int iNum = 0;
	int j = 0;

	char *szName = new char[iLength + 1];
	memset (szName, 0, iLength + 1);

	for (j = 0; j < iLength; j++)
	{
		dt = div (rand(), 16);
		iNum = dt.rem;
		
		sprintf_s(szName + j, iLength + 1 - j, "%x", iNum); 
	}

	CString strName(szName);

	if (szName)
	{
		delete []szName;
		szName = NULL;
	}

	return strName;
}

BOOL CBrowserCacheList::GetHeaderFromFile (LPCTSTR szFileName)
{
	FILE *rp = NULL;
	int iFileLength = 0;
	int iHeaderSize = 0;
	errno_t err = 0;
		
	BYTE *pbyHeader = NULL;
	BYTE byTempHeader[CACHE_HEADER_SIZE];
	memset (byTempHeader, 0, CACHE_HEADER_SIZE);

	LPCACHE_HEADER lpHeader = (LPCACHE_HEADER)byTempHeader;

	if (m_lpCacheHeader)
	{
		free (m_lpCacheHeader);
		m_lpCacheHeader = NULL;
	}

	ASSERT (szFileName);
		
	err = fopen_s( &rp, szFileName, "rb");

	if (err != 0)
	{
		return FALSE;
	}
	else
	{
		iFileLength = GetFileLength (rp);

		if (iFileLength >= 16)
		{
			fread (byTempHeader, 1, 16, rp);
			
			if (!strcmp (lpHeader->szId, CACHE_SZID))
			{
				switch (lpHeader->iVersion)
				{
				case 1:
					{
						iHeaderSize = lpHeader->iHeaderSize;

						// A little bounds checking.  If the header size is way over the top or less
						// than 0, then this cache file is corrupt and needs to be deleted.
						if ((iHeaderSize > 32768) || (iHeaderSize <= 0) || (iHeaderSize > iFileLength))
						{
							fclose (rp);
							remove (szFileName);
							return FALSE;
						}

						pbyHeader = new BYTE[iHeaderSize];

						if (!pbyHeader)
						{
							// Whoa!
							ASSERT (FALSE);
							fclose (rp);		
							SetErrorCode (CL_ERROR_OUT_OF_MEMORY);			
							return FALSE;
						}						

						memset (pbyHeader, 0, iHeaderSize);

						fseek (rp, 0, SEEK_SET);
						fread (pbyHeader, 1, iHeaderSize, rp);

						m_lpCacheHeader = (LPCACHE_HEADER) malloc (iHeaderSize);
						
						memcpy ((BYTE *)m_lpCacheHeader, (BYTE *)pbyHeader, iHeaderSize);
						
						if (pbyHeader)
						{
							delete []pbyHeader;
							pbyHeader = NULL;
						}						
					}
					break;

				default:
					{
						// Cache file is for a newer version. Perhaps they're
						// using an older copy of Wally here?
						return FALSE;
					}
					break;
				}
			}		
		}
		fclose (rp);
	}	

	return TRUE;
}

CString CBrowserCacheList::FindDirNameFromFile(LPCTSTR szFileName)
{
	CString strCompare("");
	char *szDirName = NULL;

	if (!GetHeaderFromFile (szFileName))
	{
		return strCompare;
	}

	int iDirNameLength = m_lpCacheHeader->iDirNameLength;

	if ((iDirNameLength > 2048) || (iDirNameLength <= 0))
	{		
		remove (szFileName);		
		return strCompare;
	}

	szDirName = new char[iDirNameLength + 1];
	memset (szDirName, 0, iDirNameLength + 1);

	memcpy ((BYTE *)szDirName, &m_lpCacheHeader->szDirName, iDirNameLength);
	
	strCompare = TrimSlashes (szDirName);

	if (szDirName)
	{
		delete []szDirName;
		szDirName = NULL;
	}	
	return strCompare;
}

void CBrowserCacheList::FindItemsFromFile(CSortStringArray* pStrArray, LPCTSTR szFileName)
{
	//LPCACHE_HEADER lpHeader = NULL;

	if (!GetHeaderFromFile(szFileName))
	{
		return;
	}

	int j = 0;
	FILE* rp = NULL;
	errno_t err = 0;

	err = fopen_s(&rp, szFileName, "rb");
	if (err == 0)
	{
		fseek(rp, m_lpCacheHeader->iFirstItemOffset, SEEK_SET);
		int iNumThumbnails = m_lpCacheHeader->iNumThumbnails;

		for (j = 0; j < iNumThumbnails; j++)
		{
			// This code seems to be started but long forgotten, no reference to this function anywhere
			ASSERT(FALSE);
			//pStrArray->
		}
	}
	
	//free (lpHeader);
	//lpHeader = NULL;
}

void ResizeImage24Bit (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination)
{
	BYTE *pbyData2 = (*pbyDestination);
	BYTE *pbyData = NULL;
	
	double dfPercent = 1.0 * (100.0 / (double)iPercent);

	int w = 0;
	double dfw1 = 0.0;
	int w1 = 0;
	int h = 0;
	double dfh1 = 0.0;
	int h1 = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	int FrontLean = 0;
	int HitLean = 0;
	int BackLean = 0;
	int iRem = 0;
	int TotalLean = 0;

	div_t dtLean;
	srand( (unsigned)time( NULL ) );
	
	int iSourceOffset = 0;
	int iDestOffset   = 0;
	int iSize;
	int iIndex  = 0;
	int iRIndex = 0;
	int iGIndex = 0;
	int iBIndex = 0;

	double dfStarter = dfPercent / 2.0;
	BOOL bGrow = FALSE;
	
	int iNewWidth  = (int)((iWidth * 1.0) / dfPercent);

	// Neal - TODO: remove this KLUDGE fix (there's a DWORD padding bug somewhere later)

	iNewWidth      = iNewWidth & ~3;		// KLUDGE KLUDGE KLUDGE

	int iNewHeight = (int)((iHeight * 1.0) / dfPercent);

	if (iPercent >= 100)
	{
		iNewWidth  = max (iNewWidth, iWidth + 1);
		iNewHeight = max (iNewHeight, iHeight + 1);
		
		HitLean = 5;
		bGrow   = TRUE;
	}
	else
	{
		iNewWidth  = min (iNewWidth, iWidth - 1);
		iNewHeight = min (iNewHeight, iHeight - 1);
		
		HitLean = iPercent / 5;
		HitLean = max (HitLean, 10);
		bGrow   = FALSE;
	}

	(*pNewWidth)  = iNewWidth;
	(*pNewHeight) = iNewHeight;

	if (iNewWidth == 94)		// TEST TEST TEST
	{
		int iBreakpoint = 0;
	}

	if (bGrow)
	{
		iSize = iNewHeight * iNewWidth * 3;
		//iSize = iNewHeight * PadDWORD( iNewWidth * 3);
		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		HitLean = 1;
		TotalLean = HitLean + 4;

		for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				w1 = (int) (dfw1);
				h1 = (int) (dfh1);
				h1 = min(h1, (iHeight - 1));
			
				if (w1 >= (iWidth - 1))
				{
					// Neal - fix for width = 94?
					//iDestOffset = h * iNewWidth + w;
					//iSourceOffset = h1 * iWidth + (iWidth - 1);
					//iDestOffset   = h * PadDWORD( iNewWidth) + w;
					iDestOffset   = h * iNewWidth*3;
					//iDestOffset   = h * PadDWORD( iNewWidth*3);
					iSourceOffset = h1 * iWidth + (iWidth - 1);
					
					r = (
						  pbySource[(iSourceOffset - 1) * 3 + 0]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 3 + 0]		// Top
						+ pbySource[iSourceOffset * 3 + 0] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 3 + 0]  // Bottom
						+ pbySource[(h1 * iWidth * 3) + 0] // Right
						) / TotalLean;						  
					g = (
						  pbySource[(iSourceOffset - 1) * 3 + 1]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 3 + 1]		// Top
						+ pbySource[iSourceOffset * 3 + 1] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 3 + 1]  // Bottom
						+ pbySource[(h1 * iWidth * 3) + 1] // Right
						) / TotalLean;						  
					b = (
						  pbySource[(iSourceOffset - 1) * 3 + 2]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 3 + 2]		// Top
						+ pbySource[iSourceOffset * 3 + 2] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 3 + 2]  // Bottom
						+ pbySource[(h1 * iWidth * 3) + 2] // Right
						) / TotalLean;						  

					//pbyData2[iDestOffset * 3 + 0] = r;
					//pbyData2[iDestOffset * 3 + 1] = g;
					//pbyData2[iDestOffset * 3 + 2] = b;
					pbyData2[iDestOffset + w * 3 + 0] = r;
					pbyData2[iDestOffset + w * 3 + 1] = g;
					pbyData2[iDestOffset + w * 3 + 2] = b;
				}
				else
				{
					// Neal - fix for width = 94?
					//iDestOffset = h * iNewWidth + w;
					//iSourceOffset = h1 * iWidth + w1;
					//iDestOffset   = h * PadDWORD( iNewWidth) + w;
					iDestOffset   = h * iNewWidth * 3;
					//iDestOffset   = h * PadDWORD( iNewWidth * 3);
					iSourceOffset = h1 * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  pbySource[(iSourceOffset - 1) * 3 + 0]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 0]  // Top
							+ pbySource[(iSourceOffset * 3) + 0] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 0]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 0]   // Right
							) / TotalLean;
						g = (
							  pbySource[(iSourceOffset - 1) * 3 + 1]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 1]  // Top
							+ pbySource[(iSourceOffset * 3) + 1] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 1]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 1]   // Right
							) / TotalLean;
						b = (
							  pbySource[(iSourceOffset - 1) * 3 + 2]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 2]  // Top
							+ pbySource[(iSourceOffset * 3) + 2] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 2]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 2]   // Right
							) / TotalLean;
					}					
					else
					{						
						r = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 3 + 0]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 0]  // Top
							+ pbySource[iSourceOffset * 3 + 0] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 0]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 0]  // Right
							) / TotalLean;
						g = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 3 + 1]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 1]  // Top
							+ pbySource[iSourceOffset * 3 + 1] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 1]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 1]  // Right
							) / TotalLean;
						b = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 3 + 2]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 3 + 2]  // Top
							+ pbySource[iSourceOffset * 3 + 2] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 3 + 2]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 3 + 2]  // Right
							) / TotalLean;
					}
					
					pbyData2[iDestOffset + w * 3 + 0] = r;
					pbyData2[iDestOffset + w * 3 + 1] = g;
					pbyData2[iDestOffset + w * 3 + 2] = b;
				}
			}
		}
	}
	else
	{
		if (iNewWidth == 94)		// TEST TEST TEST
		{
			int iBreakpoint = 0;
		}

		iSize = iHeight * iNewWidth * 3;
		//iSize = iHeight * PadDWORD( iNewWidth * 3);

		pbyData = new BYTE[iSize];
		memset (pbyData, 0, iSize);
		
		for (h = 0; h < iHeight; h++)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;		
		
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 3;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 3;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				w1 = (int) (dfw1 + 0.50);

				//iDestOffset = h * iNewWidth + w;
				iDestOffset = h * iNewWidth * 3;
				//iDestOffset = h * PadDWORD( iNewWidth*3);

				if (w1 >= (iWidth - 1))
				{
					iSourceOffset =  h * iWidth + (iWidth - 1);
					
					r = (
						  pbySource[(iSourceOffset - 1) * 3 + 0] * FrontLean
						+ pbySource[iSourceOffset * 3 + 0] * HitLean
						+ pbySource[h * iWidth * 3 + 0] * BackLean
						) / TotalLean;
					g = (
						  pbySource[(iSourceOffset - 1) * 3 + 1] * FrontLean
						+ pbySource[iSourceOffset * 3 + 1] * HitLean
						+ pbySource[h * iWidth * 3 + 1] * BackLean
						) / TotalLean;
					b = (
						  pbySource[(iSourceOffset - 1) * 3 + 2] * FrontLean
						+ pbySource[iSourceOffset * 3 + 2] * HitLean
						+ pbySource[h * iWidth * 3 + 2] * BackLean
						) / TotalLean;

					//pbyData[iDestOffset * 3 + 0] = r;
					//pbyData[iDestOffset * 3 + 1] = g;
					//pbyData[iDestOffset * 3 + 2] = b;
					pbyData[iDestOffset + w * 3 + 0] = r;
					pbyData[iDestOffset + w * 3 + 1] = g;
					pbyData[iDestOffset + w * 3 + 2] = b;
				}
				else
				{
					iSourceOffset = h * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  pbySource[(iSourceOffset - 1) * 3 + 0] * FrontLean
							+ pbySource[iSourceOffset * 3 + 0] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbySource[(iSourceOffset - 1) * 3 + 1] * FrontLean
							+ pbySource[iSourceOffset * 3 + 1] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbySource[(iSourceOffset - 1) * 3 + 2] * FrontLean
							+ pbySource[iSourceOffset * 3 + 2] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 2] * BackLean
							) / TotalLean;
					}					
					else
					{						
						r = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 3 + 0] * FrontLean
							+ pbySource[iSourceOffset * 3 + 0] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 3 + 1] * FrontLean
							+ pbySource[iSourceOffset * 3 + 1] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 3 + 2] * FrontLean
							+ pbySource[iSourceOffset * 3 + 2] * HitLean
							+ pbySource[(iSourceOffset + 1) * 3 + 2] * BackLean
							) / TotalLean;
					}
				
					pbyData[iDestOffset + w * 3 + 0] = r;
					pbyData[iDestOffset + w * 3 + 1] = g;
					pbyData[iDestOffset + w * 3 + 2] = b;					
				}
			}
		}

		iSize = iNewHeight * PadDWORD( iNewWidth * 3);

		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		for (w = 0; w < iNewWidth; w++)
		{			
			for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;
				
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 3;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 3;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				h1 = (int)(dfh1 + 0.50);
				
				if (h1 >= (iHeight - 1))
				{
					//iDestOffset   = h * iNewWidth + w;
					iDestOffset   = h * iNewWidth * 3;
					//iDestOffset   = h * PadDWORD( iNewWidth * 3);
					iSourceOffset = iNewWidth * (iHeight - 1) + w;
				
					r = (
						  pbyData[(iSourceOffset - iNewWidth) * 3 + 0] * FrontLean
						+ pbyData[iSourceOffset * 3 + 0] * HitLean
						+ pbyData[w * 3 + 0] * BackLean
						) / TotalLean;
					g = (
						  pbyData[(iSourceOffset - iNewWidth) * 3 + 1] * FrontLean
						+ pbyData[iSourceOffset * 3 + 1] * HitLean
						+ pbyData[w * 3 + 1] * BackLean
						) / TotalLean;
					b = (
						  pbyData[(iSourceOffset - iNewWidth) * 3 + 2] * FrontLean
						+ pbyData[iSourceOffset * 3 + 2] * HitLean
						+ pbyData[w * 3 + 2] * BackLean
						) / TotalLean;
					
					//pbyData2[iDestOffset * 3 + 0] = r;
					//pbyData2[iDestOffset * 3 + 1] = g;
					//pbyData2[iDestOffset * 3 + 2] = b;
					pbyData2[iDestOffset + w * 3 + 0] = r;
					pbyData2[iDestOffset + w * 3 + 1] = g;
					pbyData2[iDestOffset + w * 3 + 2] = b;
				}
				else
				{
					//iDestOffset   = h * iNewWidth + w;
					iDestOffset   = h * iNewWidth * 3;
					//iDestOffset   = h * PadDWORD( iNewWidth * 3);
					iSourceOffset = h1 * iNewWidth + w;

					if (h1 != 0)
					{						
						r = (
							  pbyData[(iSourceOffset - iNewWidth) * 3 + 0] * FrontLean
							+ pbyData[iSourceOffset * 3 + 0] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbyData[(iSourceOffset - iNewWidth) * 3 + 1] * FrontLean
							+ pbyData[iSourceOffset * 3 + 1] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbyData[(iSourceOffset - iNewWidth) * 3 + 2] * FrontLean
							+ pbyData[iSourceOffset * 3 + 2] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 2] * BackLean
							) / TotalLean;

					}					
					else
					{
						r = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 3 + 0] * FrontLean
							+ pbyData[iSourceOffset * 3 + 0] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 3 + 1] * FrontLean
							+ pbyData[iSourceOffset * 3 + 1] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 3 + 2] * FrontLean
							+ pbyData[iSourceOffset * 3 + 2] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 3 + 2] * BackLean
							) / TotalLean;

					}
					
					//pbyData2[iDestOffset * 3 + 0] = r;
					//pbyData2[iDestOffset * 3 + 1] = g;
					//pbyData2[iDestOffset * 3 + 2] = b;
					pbyData2[iDestOffset + w * 3 + 0] = r;
					pbyData2[iDestOffset + w * 3 + 1] = g;
					pbyData2[iDestOffset + w * 3 + 2] = b;
				}
			}
		}
	}

	if (pbyData)
	{
		delete []pbyData;
		pbyData = NULL;
	}

	(*pbyDestination) = pbyData2;
}

void ResizeImage32Bit (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination)
{
	BYTE *pbyData2 = (*pbyDestination);
	BYTE *pbyData = NULL;
	
	double dfPercent = 1.0 * (100.0 / (double)iPercent);

	int w = 0;
	double dfw1 = 0.0;
	int w1 = 0;
	int h = 0;
	double dfh1 = 0.0;
	int h1 = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;		// alpha

	int FrontLean = 0;
	int HitLean = 0;
	int BackLean = 0;
	int iRem = 0;
	int TotalLean = 0;

	div_t dtLean;
	srand( (unsigned)time( NULL ) );
	
	int iSourceOffset = 0;
	int iDestOffset = 0;
	int iIndex = 0;
	int iRIndex = 0;
	int iGIndex = 0;
	int iBIndex = 0;

	double dfStarter = dfPercent / 2.0;
	BOOL bGrow = FALSE;
	
	int iNewWidth = (int)((iWidth * 1.0) / dfPercent);
	int iNewHeight = (int)((iHeight * 1.0) / dfPercent);

	if (iPercent >= 100)
	{
		iNewWidth = max (iNewWidth, iWidth + 1);
		iNewHeight = max (iNewHeight, iHeight + 1);
		
		HitLean = 5;
		bGrow = TRUE;
	}
	else
	{
		iNewWidth = min (iNewWidth, iWidth - 1);
		iNewHeight = min (iNewHeight, iHeight - 1);
		
		HitLean = iPercent / 5;
		HitLean = max (HitLean, 10);
		bGrow = FALSE;
	}

	(*pNewWidth) = iNewWidth;
	(*pNewHeight) = iNewHeight;

	if (bGrow)
	{
		int iSize = iNewWidth * iNewHeight * 4;
		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		HitLean = 1;
		TotalLean = HitLean + 4;

		for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				w1 = (int) (dfw1);
				h1 = (int) (dfh1);
				h1 = min(h1, (iHeight - 1));
			
				if (w1 >= (iWidth - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iWidth + (iWidth - 1);
					
					r = (
						  pbySource[(iSourceOffset - 1) * 4 + 0]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 4 + 0]		// Top
						+ pbySource[iSourceOffset * 4 + 0] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 4 + 0]  // Bottom
						+ pbySource[(h1 * iWidth * 4) + 0] // Right
						) / TotalLean;						  
					g = (
						  pbySource[(iSourceOffset - 1) * 4 + 1]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 4 + 1]		// Top
						+ pbySource[iSourceOffset * 4 + 1] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 4 + 1]  // Bottom
						+ pbySource[(h1 * iWidth * 4) + 1] // Right
						) / TotalLean;						  
					b = (
						  pbySource[(iSourceOffset - 1) * 4 + 2]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 4 + 2]		// Top
						+ pbySource[iSourceOffset * 4 + 2] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 4 + 2]  // Bottom
						+ pbySource[(h1 * iWidth * 4) + 2] // Right
						) / TotalLean;						  
					a = (
						  pbySource[(iSourceOffset - 1) * 4 + 3]		// Left						
						+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)) * 4 + 3]		// Top
						+ pbySource[iSourceOffset * 4 + 3] * HitLean	// Hit
						+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1) * 4 + 3]  // Bottom
						+ pbySource[(h1 * iWidth * 4) + 3] // Right
						) / TotalLean;						  

					pbyData2[iDestOffset * 4 + 0] = r;
					pbyData2[iDestOffset * 4 + 1] = g;
					pbyData2[iDestOffset * 4 + 2] = b;
					pbyData2[iDestOffset * 4 + 3] = a;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  pbySource[(iSourceOffset - 1) * 4 + 0]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 0]  // Top
							+ pbySource[(iSourceOffset * 4) + 0] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 0]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 0]   // Right
							) / TotalLean;
						g = (
							  pbySource[(iSourceOffset - 1) * 4 + 1]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 1]  // Top
							+ pbySource[(iSourceOffset * 4) + 1] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 1]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 1]   // Right
							) / TotalLean;
						b = (
							  pbySource[(iSourceOffset - 1) * 4 + 2]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 2]  // Top
							+ pbySource[(iSourceOffset * 4) + 2] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 2]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 2]   // Right
							) / TotalLean;
						a = (
							  pbySource[(iSourceOffset - 1) * 4 + 3]  // Left
							+ pbySource[( h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 3]  // Top
							+ pbySource[(iSourceOffset * 4) + 3] * HitLean  // Hit
							+ pbySource[( h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 3]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 3]   // Right
							) / TotalLean;
					}					
					else
					{						
						r = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 4 + 0]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 0]  // Top
							+ pbySource[iSourceOffset * 4 + 0] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 0]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 0]  // Right
							) / TotalLean;
						g = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 4 + 1]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 1]  // Top
							+ pbySource[iSourceOffset * 4 + 1] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 1]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 1]  // Right
							) / TotalLean;
						b = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 4 + 2]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 2]  // Top
							+ pbySource[iSourceOffset * 4 + 2] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 2]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 2]  // Right
							) / TotalLean;
						a = (
							  pbySource[(h1 * iWidth + (iWidth - 1)) * 4 + 3]  // Left
							+ pbySource[(h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)) * 4 + 3]  // Top
							+ pbySource[iSourceOffset * 4 + 3] * HitLean  // Hit
							+ pbySource[(h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1) * 4 + 3]  // Bottom
							+ pbySource[(iSourceOffset + 1) * 4 + 3]  // Right
							) / TotalLean;
					}
					
					pbyData2[iDestOffset * 4 + 0] = r;
					pbyData2[iDestOffset * 4 + 1] = g;
					pbyData2[iDestOffset * 4 + 2] = b;
					pbyData2[iDestOffset * 4 + 3] = a;
				}
			}
		}
	}
	else
	{
		int iSize = iNewWidth * iHeight * 4;
			
		pbyData = new BYTE[iSize];
		memset (pbyData, 0, iSize);
		
		for (h = 0; h < iHeight; h++)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;		
		
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 4;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 4;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				w1 = (int) (dfw1 + 0.50);

				if (w1 >= (iWidth - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h * iWidth + (iWidth - 1);
					
					r = (
						  pbySource[(iSourceOffset - 1) * 4 + 0] * FrontLean
						+ pbySource[iSourceOffset * 4 + 0] * HitLean
						+ pbySource[h * iWidth * 4 + 0] * BackLean
						) / TotalLean;
					g = (
						  pbySource[(iSourceOffset - 1) * 4 + 1] * FrontLean
						+ pbySource[iSourceOffset * 4 + 1] * HitLean
						+ pbySource[h * iWidth * 4 + 1] * BackLean
						) / TotalLean;
					b = (
						  pbySource[(iSourceOffset - 1) * 4 + 2] * FrontLean
						+ pbySource[iSourceOffset * 4 + 2] * HitLean
						+ pbySource[h * iWidth * 4 + 2] * BackLean
						) / TotalLean;
					a = (
						  pbySource[(iSourceOffset - 1) * 4 + 3] * FrontLean
						+ pbySource[iSourceOffset * 4 + 3] * HitLean
						+ pbySource[h * iWidth * 4 + 3] * BackLean
						) / TotalLean;

					pbyData[iDestOffset * 4 + 0] = r;
					pbyData[iDestOffset * 4 + 1] = g;
					pbyData[iDestOffset * 4 + 2] = b;
					pbyData[iDestOffset * 4 + 3] = a;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  pbySource[(iSourceOffset - 1) * 4 + 0] * FrontLean
							+ pbySource[iSourceOffset * 4 + 0] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbySource[(iSourceOffset - 1) * 4 + 1] * FrontLean
							+ pbySource[iSourceOffset * 4 + 1] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbySource[(iSourceOffset - 1) * 4 + 2] * FrontLean
							+ pbySource[iSourceOffset * 4 + 2] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 2] * BackLean
							) / TotalLean;
						a = (
							  pbySource[(iSourceOffset - 1) * 4 + 3] * FrontLean
							+ pbySource[iSourceOffset * 4 + 3] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 3] * BackLean
							) / TotalLean;
					}					
					else
					{						
						r = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 4 + 0] * FrontLean
							+ pbySource[iSourceOffset * 4 + 0] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 4 + 1] * FrontLean
							+ pbySource[iSourceOffset * 4 + 1] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 4 + 2] * FrontLean
							+ pbySource[iSourceOffset * 4 + 2] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 2] * BackLean
							) / TotalLean;
						a = (
							  pbySource[(h * iWidth + (iWidth - 1)) * 4 + 3] * FrontLean
							+ pbySource[iSourceOffset * 4 + 3] * HitLean
							+ pbySource[(iSourceOffset + 1) * 4 + 3] * BackLean
							) / TotalLean;
					}
				
					pbyData[iDestOffset * 4 + 0] = r;
					pbyData[iDestOffset * 4 + 1] = g;
					pbyData[iDestOffset * 4 + 2] = b;					
					pbyData[iDestOffset * 4 + 3] = a;					
				}
			}
		}

		iSize = iNewWidth * iNewHeight * 4;
		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		for (w = 0; w < iNewWidth; w++)
		{			
			for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;
				
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 4;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 4;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				h1 = (int)(dfh1 + 0.50);
				
				if (h1 >= (iHeight - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = iNewWidth * (iHeight - 1) + w;
				
					r = (
						  pbyData[(iSourceOffset - iNewWidth) * 4 + 0] * FrontLean
						+ pbyData[iSourceOffset * 4 + 0] * HitLean
						+ pbyData[w * 4 + 0] * BackLean
						) / TotalLean;
					g = (
						  pbyData[(iSourceOffset - iNewWidth) * 4 + 1] * FrontLean
						+ pbyData[iSourceOffset * 4 + 1] * HitLean
						+ pbyData[w * 4 + 1] * BackLean
						) / TotalLean;
					b = (
						  pbyData[(iSourceOffset - iNewWidth) * 4 + 2] * FrontLean
						+ pbyData[iSourceOffset * 4 + 2] * HitLean
						+ pbyData[w * 4 + 2] * BackLean
						) / TotalLean;
					a = (
						  pbyData[(iSourceOffset - iNewWidth) * 4 + 3] * FrontLean
						+ pbyData[iSourceOffset * 4 + 3] * HitLean
						+ pbyData[w * 4 + 3] * BackLean
						) / TotalLean;
					
					pbyData2[iDestOffset * 4 + 0] = r;
					pbyData2[iDestOffset * 4 + 1] = g;
					pbyData2[iDestOffset * 4 + 2] = b;
					pbyData2[iDestOffset * 4 + 3] = a;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iNewWidth + w;

					if (h1 != 0)
					{						
						r = (
							  pbyData[(iSourceOffset - iNewWidth) * 4 + 0] * FrontLean
							+ pbyData[iSourceOffset * 4 + 0] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbyData[(iSourceOffset - iNewWidth) * 4 + 1] * FrontLean
							+ pbyData[iSourceOffset * 4 + 1] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbyData[(iSourceOffset - iNewWidth) * 4 + 2] * FrontLean
							+ pbyData[iSourceOffset * 4 + 2] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 2] * BackLean
							) / TotalLean;
						a = (
							  pbyData[(iSourceOffset - iNewWidth) * 4 + 3] * FrontLean
							+ pbyData[iSourceOffset * 4 + 3] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 3] * BackLean
							) / TotalLean;

					}					
					else
					{
						r = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 4 + 0] * FrontLean
							+ pbyData[iSourceOffset * 4 + 0] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 0] * BackLean
							) / TotalLean;
						g = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 4 + 1] * FrontLean
							+ pbyData[iSourceOffset * 4 + 1] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 1] * BackLean
							) / TotalLean;
						b = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 4 + 2] * FrontLean
							+ pbyData[iSourceOffset * 4 + 2] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 2] * BackLean
							) / TotalLean;
						a = (
							  pbyData[(iNewWidth * (iHeight - 1) + w) * 4 + 3] * FrontLean
							+ pbyData[iSourceOffset * 4 + 3] * HitLean
							+ pbyData[(iSourceOffset + iNewWidth) * 4 + 3] * BackLean
							) / TotalLean;
					}
					
					pbyData2[iDestOffset * 4 + 0] = r;
					pbyData2[iDestOffset * 4 + 1] = g;
					pbyData2[iDestOffset * 4 + 2] = b;
					pbyData2[iDestOffset * 4 + 3] = a;
				}
			}
		}
	}

	if (pbyData)
	{
		delete []pbyData;
		pbyData = NULL;
	}

	(*pbyDestination) = pbyData2;
}

void ResizeImage256 (int iWidth, int iHeight, int *pNewWidth, int *pNewHeight, int iPercent, BYTE *pbySource, BYTE **pbyDestination, BYTE *pbyPalette)
{	
	BYTE *pbyData2 = (*pbyDestination);
	BYTE *pbyData = NULL;
	
	double dfPercent = 1.0 * (100.0 / (double)iPercent);
	
	CWallyPalette Palette;

	int w = 0;
	double dfw1 = 0.0;
	int w1 = 0;
	int h = 0;
	double dfh1 = 0.0;
	int h1 = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	int FrontLean = 0;
	int HitLean = 0;
	int BackLean = 0;
	int iRem = 0;
	int TotalLean = 0;

	div_t dtLean;
	srand( (unsigned)time( NULL ) );
	
	int iSourceOffset = 0;
	int iDestOffset = 0;
	int iIndex = 0;
	int iRIndex = 0;
	int iGIndex = 0;
	int iBIndex = 0;

	double dfStarter = dfPercent / 2.0;
	BOOL bGrow = FALSE;
	
	int iNewWidth = (int)((iWidth * 1.0) / dfPercent);
	int iNewHeight = (int)((iHeight * 1.0) / dfPercent);

	if (iPercent >= 100)
	{
		iNewWidth = max (iNewWidth, iWidth + 1);
		iNewHeight = max (iNewHeight, iHeight + 1);
		
		HitLean = 5;
		bGrow = TRUE;
	}
	else
	{
		iNewWidth = min (iNewWidth, iWidth - 1);
		iNewHeight = min (iNewHeight, iHeight - 1);
		
		HitLean = iPercent / 5;
		HitLean = max (HitLean, 10);
		bGrow = FALSE;
	}

	(*pNewWidth) = iNewWidth;
	(*pNewHeight) = iNewHeight;

	Palette.SetPalette (pbyPalette, 256);	
	
	if (bGrow)
	{
		int iSize = iNewWidth * iNewHeight;
		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		HitLean = 1;
		TotalLean = HitLean + 4;

		for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				w1 = (int) (dfw1);
				h1 = (int) (dfh1);
				h1 = min(h1, (iHeight - 1));
			
				if (w1 >= (iWidth - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iWidth + (iWidth - 1);
					
					r = (
						  pbyPalette[pbySource[iSourceOffset - 1] * 3]		// Left
						+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)] * 3]  // Top
						+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)	// Hit
						+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1] * 3]  // Bottom
						+ pbyPalette[pbySource[h1 * iWidth] * 3]				// Right
						) / TotalLean;
					g = (
						  pbyPalette[pbySource[iSourceOffset - 1] * 3 + 1]		// Left
						+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)] * 3 + 1]  // Top
						+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)	// Hit
						+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1] * 3 + 1]  // Bottom
						+ pbyPalette[pbySource[h1 * iWidth] * 3 + 1]				// Right
						) / TotalLean;
					b = (
						  pbyPalette[pbySource[iSourceOffset - 1] * 3 + 2]		// Left
						+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : (iHeight * iWidth - 1)] * 3 + 2]  // Top
						+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)	// Hit
						+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : iWidth - 1] * 3 + 2]  // Bottom
						+ pbyPalette[pbySource[h1 * iWidth] * 3 + 2]				// Right
						) / TotalLean;

					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData2[iDestOffset] = iIndex;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  pbyPalette[pbySource[iSourceOffset - 1] * 3]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3]   // Right
							) / TotalLean;
						g = (
							  pbyPalette[pbySource[iSourceOffset - 1] * 3 + 1]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3 + 1]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3 + 1]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3 + 1]   // Right
							) / TotalLean;
						b = (
							  pbyPalette[pbySource[iSourceOffset - 1] * 3 + 2]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3 + 2]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3 + 2]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3 + 2]   // Right
							) / TotalLean;
					}					
					else
					{						
						r = (
							  pbyPalette[pbySource[h1 * iWidth + (iWidth - 1)] * 3]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3]  // Right
							) / TotalLean;
						g = (
							  pbyPalette[pbySource[h1 * iWidth + (iWidth - 1)] * 3 + 1]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3 + 1]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3 + 1]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3 + 1]  // Right
							) / TotalLean;
						b = (
							  pbyPalette[pbySource[h1 * iWidth + (iWidth - 1)] * 3 + 2]  // Left
							+ pbyPalette[pbySource[h1 != 0 ? (iSourceOffset - iWidth) : ((iHeight - 1) * iWidth + w1)] * 3 + 2]  // Top
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)  // Hit
							+ pbyPalette[pbySource[h1 != (iHeight - 1) ? (iSourceOffset + iWidth) : w1] * 3 + 2]  // Bottom
							+ pbyPalette[pbySource[iSourceOffset + 1] * 3 + 2]  // Right
							) / TotalLean;
					}
					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData2[iDestOffset] = iIndex;
				}
			}
		}
	}
	else
	{
		int iSize = iNewWidth * iHeight;
			
		pbyData = new BYTE[iSize];
		memset (pbyData, 0, iSize);
		
		for (h = 0; h < iHeight; h++)
		{			
			for (w = 0, dfw1 = dfStarter; w < iNewWidth; w++, dfw1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;		
		
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 3;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 3;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				w1 = (int) (dfw1 + 0.50);

				if (w1 >= (iWidth - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h * iWidth + (iWidth - 1);
					
					r = (
						  (pbyPalette[pbySource[iSourceOffset - 1] * 3] * FrontLean)
						+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)
						+ (pbyPalette[pbySource[h * iWidth] * 3] * BackLean)
						) / TotalLean;
					g = (
						  (pbyPalette[pbySource[iSourceOffset - 1] * 3 + 1] * FrontLean)
						+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)
						+ (pbyPalette[pbySource[h * iWidth] * 3 + 1] * BackLean)
						) / TotalLean;
					b = (
						  (pbyPalette[pbySource[iSourceOffset - 1] * 3 + 2] * FrontLean)
						+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)
						+ (pbyPalette[pbySource[h * iWidth] * 3 + 2] * BackLean)
						) / TotalLean;

					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData[iDestOffset] = iIndex;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h * iWidth + w1;

					if (w1 != 0)
					{							
						r = (
							  (pbyPalette[pbySource[iSourceOffset - 1] * 3] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3] * BackLean)
							) / TotalLean;
						g = (
							  (pbyPalette[pbySource[iSourceOffset - 1] * 3 + 1] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3 + 1] * BackLean)
							) / TotalLean;
						b = (
							  (pbyPalette[pbySource[iSourceOffset - 1] * 3 + 2] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3 + 2] * BackLean)
							) / TotalLean;
					}					
					else
					{						
						r = (
							  (pbyPalette[pbySource[h * iWidth + (iWidth - 1)] * 3] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3] * BackLean)
							) / TotalLean;
						g = (
							  (pbyPalette[pbySource[h * iWidth + (iWidth - 1)] * 3 + 1] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 1] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3 + 1] * BackLean)
							) / TotalLean;
						b = (
							  (pbyPalette[pbySource[h * iWidth + (iWidth - 1)] * 3 + 2] * FrontLean)
							+ (pbyPalette[pbySource[iSourceOffset] * 3 + 2] * HitLean)
							+ (pbyPalette[pbySource[iSourceOffset + 1] * 3 + 2] * BackLean)
							) / TotalLean;
					}
					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData[iDestOffset] = iIndex;
				}
			}
		}

		iSize = iNewWidth * iNewHeight;
		pbyData2 = new BYTE[iSize];
		memset (pbyData2, 0, iSize);

		for (w = 0; w < iNewWidth; w++)
		{			
			for (h = 0, dfh1 = dfStarter; h < iNewHeight; h++, dfh1 += dfPercent)
			{
				// Randomize which color gets the lean.  Heavier preference towards the actual hit
				dtLean = div (rand(), 100);
				iRem = dtLean.rem;
				
				FrontLean = 2;
				BackLean = 2;
				
				if ((iRem >= 60) && (iRem < 80))
				{
					FrontLean = 3;
				}
				if ((iRem >= 80) && (iRem < 100))
				{
					BackLean = 3;
				}

				TotalLean = HitLean + FrontLean + BackLean;

				h1 = (int)(dfh1 + 0.50);
				
				if (h1 >= (iHeight - 1))
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = iNewWidth * (iHeight - 1) + w;
				
					r = (
						  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3] * FrontLean)
						+ (pbyPalette[pbyData[iSourceOffset] * 3] * HitLean)
						+ (pbyPalette[pbyData[w] * 3] * BackLean)
						) / TotalLean;
					g = (
						  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3 + 1] * FrontLean)
						+ (pbyPalette[pbyData[iSourceOffset] * 3 + 1] * HitLean)
						+ (pbyPalette[pbyData[w] * 3 + 1] * BackLean)
						) / TotalLean;
					b = (
						  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3 + 2] * FrontLean)
						+ (pbyPalette[pbyData[iSourceOffset] * 3 + 2] * HitLean)
						+ (pbyPalette[pbyData[w] * 3 + 2] * BackLean)
						) / TotalLean;

					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData2[iDestOffset] = iIndex;
				}
				else
				{
					iDestOffset = h * iNewWidth + w;
					iSourceOffset = h1 * iNewWidth + w;

					if (h1 != 0)
					{						
						r = (
							  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3] * BackLean)
							) / TotalLean;
						g = (
							  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3 + 1] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3 + 1] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3 + 1] * BackLean)
							) / TotalLean;
						b = (
							  (pbyPalette[pbyData[iSourceOffset - iNewWidth] * 3 + 2] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3 + 2] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3 + 2] * BackLean)
							) / TotalLean;

					}					
					else
					{
						r = (
							  (pbyPalette[pbyData[iNewWidth * (iHeight - 1) + w] * 3] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3] * BackLean)
							) / TotalLean;
						g = (
							  (pbyPalette[pbyData[iNewWidth * (iHeight - 1) + w] * 3 + 1] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3 + 1] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3 + 1] * BackLean)
							) / TotalLean;
						b = (
							  (pbyPalette[pbyData[iNewWidth * (iHeight - 1) + w] * 3 + 2] * FrontLean)
							+ (pbyPalette[pbyData[iSourceOffset] * 3 + 2] * HitLean)
							+ (pbyPalette[pbyData[iSourceOffset + iNewWidth] * 3 + 2] * BackLean)
							) / TotalLean;

					}
					iIndex = Palette.FindNearestColor (r, g, b, FALSE);
					pbyData2[iDestOffset] = iIndex;
				}
			}
		}
	}

	if (pbyData)
	{
		delete []pbyData;
		pbyData = NULL;
	}

	(*pbyDestination) = pbyData2;

}