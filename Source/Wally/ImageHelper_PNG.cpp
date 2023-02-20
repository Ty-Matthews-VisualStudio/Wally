/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  ImageHelper.cpp : implementation of the CImageHelper class
//
//  Created by Ty Matthews, 5-8-2001
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "Wally.h"
#include "TextureFlags.h"
#include "WallyPal.h"
#include "ColorOpt.h"
#include "WADList.h"
#include "RegistryHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////
//   PNG Support
////////////////////////////////////////////////////////////////////////////////////

// This function is called when there is a warning, but the library thinks
// it can continue anyway.  Replacement functions don't have to do anything
// here if you don't want to.  In the default configuration, png_ptr is
// not used, but it is passed in case it may be useful.
void CImageHelper::png_default_warning(png_structp png_ptr, png_const_charp message)
{	
	PNG_CONST char *name = "UNKNOWN (ERROR!)";	
	if( png_ptr )
	{
		png_voidp pError = png_get_error_ptr(png_ptr);
		if(pError != NULL )
		{
			name = (char *)pError;
		}
		png_voidp pIO = png_get_io_ptr(png_ptr);
		CImageHelper *pThis = (CImageHelper *)pIO;
		if( pThis )
		{
			pThis->SetErrorCode (IH_PNG_CUSTOM_ERROR);

			CString strError("");
			strError.Format ("%s: libpng warning: %s\n", name, message);

			pThis->SetErrorText (strError);
		}
	}
}

// This is the default error handling function for the PNG library.  Note 
// that replacements for this function MUST NOT RETURN, or the program will 
// likely crash. (?!)  This function is used by default, or if the program 
// supplies NULL for the error function pointer in png_set_error_fn()
 
void CImageHelper::png_default_error(png_structp png_ptr, png_const_charp message)
{
   png_default_warning(png_ptr, message);
   /* We can return because png_error calls the default handler, which is
    * actually OK in this case. */
}

// This is an override of the default handler for reading data.  The PNG library expects
// to use stdio to do the reading/writing, which means fread and fwrite.  That's not really
// what we want in every case, considering CImageHelper has done all of the buffering for us.
// So here, just take the length and copy it on over.  NOTE: because we can't be sure the 
// library 
void CImageHelper::png_default_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_voidp pIO = png_get_io_ptr(png_ptr);
	CImageHelper *pThis = (CImageHelper *)pIO;

	if( pThis )
	{
		UINT iReadPosition = pThis->GetPNGReadPosition();
		UINT iDataSize = pThis->GetDataSize();
		LPBYTE pbyEncodedData = pThis->GetEncodedData();

		if( iReadPosition >= iDataSize )
		{
			png_error(png_ptr, "Read Error");
			return;
		}

		CopyMemory( data, pbyEncodedData + iReadPosition, length);	
		iReadPosition += length;
			
		if( iReadPosition > iDataSize )
		{
			png_error(png_ptr, "Read Error");
			return;
		}

		pThis->SetPNGReadPosition( iReadPosition);
	}	
}

void CImageHelper::png_default_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_voidp pIO = png_get_io_ptr(png_ptr);
	CImageHelper* pThis = (CImageHelper*)pIO;

	if( pThis )
	{
		UINT iWritePosition = pThis->GetPNGWritePosition();

		if( pThis->UseArchive() )
		{
			CArchive *pArchive = pThis->GetArchive();
			pArchive->Write (data, length);
		}
		else
		{
			FILE *fp = pThis->GetPNGWriteFile();

			if( fp )
			{
				fseek( fp, iWritePosition, SEEK_SET);
				if( fwrite (data, 1, length, fp) != length )
				{
					png_error(png_ptr, "Write Error");
					return;
				}
			}
		}
		
		iWritePosition += length;
		pThis->SetPNGWritePosition( iWritePosition);
	}	
}

BOOL CImageHelper::DecodePNG( int iFlags /* = 0 */ )
{
	// Set up the #define to state we want our own read/write routines to be called.
	// Otherwise, the library uses FILE pointers when we probably have already loaded
	// the data and don't want that.
	#define PNG_NO_STDIO 0

	png_structp		pRead			= NULL;
	png_infop		pReadInfo		= NULL;
	png_infop		pEndInfo		= NULL;
	
	png_bytep		pbyRowBuf		= NULL;
	png_uint_32		iWidth				= 0;
	png_uint_32		iHeight				= 0;
	int				iBitDepth			= 0;
	int				iColorType			= 0;
	int				iInterlaceType		= 0;
	int				iCompressionType	= 0;
	int				iFilterType			= 0;
	int				iChannels			= 0;
	int				iRowBytes			= 0;
	UINT			j					= 0;

	// Make sure it's an actual PNG file. png_sig_cmp() returns zero if the 
	// image is a PNG and nonzero if it isn't a PNG.
	
	if (png_sig_cmp(m_pbyEncodedData, 0, 8))    
    {
		SetErrorCode (IH_PNG_MALFORMED);
        return FALSE;
    }

	pRead = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
		(png_error_ptr)NULL, (png_error_ptr)NULL);

	if (!pRead)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}
	
#if defined(PNG_NO_STDIO)
	png_set_error_fn( pRead, (png_voidp)(&m_strFileName), CImageHelper::png_default_error,
		CImageHelper::png_default_warning);	
#endif

	pReadInfo = png_create_info_struct( pRead );
	if (!pReadInfo)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		png_destroy_read_struct(&pRead, NULL, NULL);
		return FALSE;
	}
	
	pEndInfo = png_create_info_struct(pRead);
	if (!pEndInfo)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		png_destroy_read_struct(&pRead, &pReadInfo, NULL);
		return FALSE;
	}
		
	if (setjmp(png_jmpbuf(pRead)))
	{
		SetErrorCode (IH_PNG_READ_ERROR);
		png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
		return FALSE;
	}
	
	// Point to our in-house read routine
	png_set_read_fn(pRead, (png_voidp)this, CImageHelper::png_default_read_data);

	// We don't want intermittent status reports
	png_set_read_status_fn(pRead, NULL);

	png_read_info(pRead, pReadInfo);

	png_get_IHDR( pRead, pReadInfo, &iWidth, &iHeight, &iBitDepth,
		&iColorType, &iInterlaceType, &iCompressionType, &iFilterType);

	SetImageWidth (iWidth);
	SetImageHeight (iHeight);
   	
	if (
			(	(iColorType == PNG_COLOR_TYPE_GRAY) || (iColorType == PNG_COLOR_TYPE_PALETTE) ) &&
			(	iBitDepth < 8	)
		)
	{
		// TODO:  What we're doing here is converting 2- and 4- bit images (black/white and 16-colors)
		// to 8-bit RGB images (24-bit overall)  This works for loading, but the user might want
		// to actually keep it as 2 or 4 bit.  Do we really care?  How often will this happen?  The
		// thing is, we don't have a mechanism for either displaying or writing 2- or 4- bit images 
		// yet, so it's not even possible.  Hence the TODO :)

		// Set a transformation to crank it up to 8-bits per pixel (not 8-bit indexed!)
		png_set_expand(pRead);

		// Go re-read the info so we get the correct row_bytes, et al
	    png_read_update_info(pRead, pReadInfo);

		png_get_IHDR( pRead, pReadInfo, &iWidth, &iHeight, &iBitDepth,
			&iColorType, &iInterlaceType, &iCompressionType, &iFilterType);
	}

	iChannels = png_get_channels( pRead, pReadInfo);
	iRowBytes = png_get_rowbytes( pRead, pReadInfo);

	switch (iColorType)
	{
	case PNG_COLOR_TYPE_PALETTE:
		{
			SetColorDepth( IH_8BIT );
		}
		break;

	case PNG_COLOR_TYPE_GRAY:
		{
			SetColorDepth (IH_8BIT);
		}
		break;

	case PNG_COLOR_TYPE_RGB:
		{
			SetColorDepth (IH_24BIT);
		}
		break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			SetColorDepth(IH_32BIT);
		}
		break;

	default:
		{
			SetErrorCode( IH_PNG_MALFORMED );
			return FALSE;
		}
		break;
	}

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	switch (iColorType)
	{
		// These "row pointers" are used by the PNG library to fill in the data.
		// Without them set properly, you only get the very top row (which isn't very
		// good!)  I think it's because of the way the library handles interlacing.

	case PNG_COLOR_TYPE_PALETTE:
		{
			SetColorDepth (IH_8BIT);
			m_pbyDecodedData = new BYTE[iRowBytes * iHeight];

			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}
			
			if (png_get_valid(pRead, pReadInfo,	PNG_INFO_PLTE))		    
			{
				int iNumColors = 0;

				// Create a pointer for the palette
				png_colorp pPNGPalette = NULL;
								
				// Now read them in
				if (png_get_PLTE(pRead, pReadInfo, &pPNGPalette, &iNumColors))
				{
					// Take and move them to the member palette
					for (j = 0; (j < (UINT)iNumColors) && (j < 256); j++)
					{
						m_byPalette[j * 3 + 0] = pPNGPalette[j].red;					
						m_byPalette[j * 3 + 1] = pPNGPalette[j].green;
						m_byPalette[j * 3 + 2] = pPNGPalette[j].blue;					
					}
				}
			}

			png_bytepp row_pointers = new png_bytep[iHeight];

			// Point at every row
			for (j = 0; j < iHeight; j++)
			{
				row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iRowBytes);
			}			

			png_read_image(pRead, row_pointers);

			if (row_pointers)
			{
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	case PNG_COLOR_TYPE_GRAY:
		{
			SetColorDepth (IH_8BIT);
			
			m_pbyDecodedData = new BYTE[iRowBytes * iHeight];

			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}
			
			// Build the greyscale palette
			for (j = 0; j < 256; j++)
			{
				memset (m_byPalette + j * 3, j, 3);				
			}				

			png_bytepp row_pointers = new png_bytep[iHeight];

			// Point at every row
			for (j = 0; j < iHeight; j++)
			{
				row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iRowBytes);
			}			

			png_read_image(pRead, row_pointers);

			if (row_pointers)
			{
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	case PNG_COLOR_TYPE_RGB:
		{
			SetColorDepth (IH_24BIT);
			m_pbyDecodedData = new BYTE[iRowBytes * iHeight];
			
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}

			png_bytepp row_pointers = new png_bytep[iHeight];

			// Point at every row
			for (UINT j = 0; j < iHeight; j++)
			{
				row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iRowBytes);
			}			

			png_read_image(pRead, row_pointers);

			if (row_pointers)
			{
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			SetColorDepth(IH_32BIT);
			m_pbyDecodedData = new BYTE[iRowBytes * iHeight];
			
			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}

			png_bytepp row_pointers = new png_bytep[iHeight];

			// Point at every row
			for (UINT j = 0; j < iHeight; j++)
			{
				row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iRowBytes);
			}			

			png_read_image(pRead, row_pointers);

			if (row_pointers)
			{
				delete []row_pointers;
				row_pointers = NULL;
			}

#ifdef _DEBUG
			// Go spit out the alpha channel
			for (UINT y = 0; y < iHeight; y++)
			{
				for (UINT x = 0; x < iWidth; x += 4)
				{
					if (((y & 7) == 0) && (((x/4) & 7) == 0))	// only show every eighth pixel
					{
						int iOffset = y * iRowBytes + x + 3;
						int a = m_pbyDecodedData[iOffset];		// alpha channel
						
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
				}
				
				if ((y & 7) == 0)	// only show every eighth line
				{
					OutputDebugString( "\n");
				}
			}
#endif
		}
		break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			CMemBuffer mbDecodedData;
			LPBYTE pbyDecodedData = mbDecodedData.GetBuffer (iRowBytes * iHeight, 0);

			if (!pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}
						
			png_bytepp row_pointers = new png_bytep[iHeight];

			// Point at every row
			for (j = 0; j < iHeight; j++)
			{
				row_pointers[j] = (png_bytep)(pbyDecodedData + j * iRowBytes);
			}			

			png_read_image(pRead, row_pointers);

			if (row_pointers)
			{
				delete []row_pointers;
				row_pointers = NULL;
			}

			// Now take and move to 32-bit; the only current method for playing with the Alpha channel
			SetColorDepth (IH_32BIT);
			m_pbyDecodedData = new BYTE[iWidth * iHeight * 4];

			if (!m_pbyDecodedData)
			{
				SetErrorCode (IH_OUT_OF_MEMORY);
				png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
				return FALSE;
			}

			UINT iOffset = 0;
			UINT iDestOffset = 0;
			UINT a = 0;

			for (UINT y = 0; y < iHeight; y++)
			{
				for (UINT x = 0; x < iWidth; x++)
				{
					iOffset = (y * iWidth * 2) + (x * 2);
					iDestOffset = (y * iWidth * 4) + (x * 4);

					memset (m_pbyDecodedData + iDestOffset, pbyDecodedData[iOffset], 3);										
					a = m_pbyDecodedData[iDestOffset + 3] = pbyDecodedData[iOffset + 1];
 
#ifdef _DEBUG
				// Go spit out the alpha channel

					if (((y & 7) == 0) && (((x/4) & 7) == 0))	// only show every eighth pixel
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
#endif

				}

#ifdef _DEBUG
				if ((y & 7) == 0)	// only show every eighth line
				{
					OutputDebugString( "\n");
				}
#endif

			}			
			
		}
		break;
		
	default:
		{
			png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);
			ASSERT (FALSE);
			return FALSE;
		}
		break;
	}

	// Read the end data (anything useful here?)
	png_read_end(pRead, pEndInfo);
	
	// Clean up
	png_destroy_read_struct(&pRead, &pReadInfo, &pEndInfo);

	return TRUE;
}

BOOL CImageHelper::EncodePNG ()
{	
	// Set up the #define to state we want our own read/write routines to be called.
	// Otherwise, the library uses FILE pointers when we probably have already loaded
	// the data and don't want that.
	#define PNG_NO_STDIO 0

	png_structp		pWrite			= NULL;
	png_infop		pWriteInfo		= NULL;
	
	png_bytep		pbyRowBuf		= NULL;
	png_uint_32		y					= 0;
	png_uint_32		iWidth				= GetImageWidth();
	png_uint_32		iHeight				= GetImageHeight();
	int				iBitDepth			= GetColorDepth();
	int				iColorType			= 0;
	int				iInterlaceType		= 0;
	int				iCompressionType	= 0;
	int				iFilterType			= 0;
	int				iChannels			= 0;
	int				iRowBytes			= 0;
	UINT			j					= 0;
	
	pWrite = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
		(png_error_ptr)NULL, (png_error_ptr)NULL);

	if (!pWrite)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		return FALSE;
	}

#if defined(PNG_NO_STDIO)	
	png_set_error_fn(pWrite, (png_voidp)(&m_strFileName), CImageHelper::png_default_error,
		CImageHelper::png_default_warning);	
#endif

	pWriteInfo = png_create_info_struct(pWrite);
	if (!pWriteInfo)
	{
		SetErrorCode (IH_OUT_OF_MEMORY);
		png_destroy_write_struct(&pWrite, NULL);
		return FALSE;
	}

	if (setjmp(png_jmpbuf(pWrite)))
	{
		SetErrorCode (IH_PNG_WRITE_ERROR);
		png_destroy_write_struct(&pWrite, &pWriteInfo);

		if (!UseArchive())
		{
			if (m_fpPNGOutput)
			{
				fclose (m_fpPNGOutput);
			}
		}

		return FALSE;
	}

	// Point to our in-house read routine
	png_set_write_fn(pWrite, (png_voidp)this, CImageHelper::png_default_write_data, NULL);

	// We don't want intermittent status reports
	png_set_write_status_fn(pWrite, NULL);

	
	if (!UseArchive())	
	{
		// Go open the file
		if (!m_fpPNGOutput)
		{
			errno_t err = fopen_s (&m_fpPNGOutput, m_strFileName, "wb+");

			if (err != 0)
			{
				SetErrorCode (IH_ERROR_WRITING_FILE);
				png_destroy_write_struct(&pWrite, &pWriteInfo);
				return FALSE;
			}		
		}
		else
		{
			SetErrorCode (IH_ERROR_WRITING_FILE);
			return FALSE;
		}
	}

	// Build the header
	switch (iBitDepth)
	{
		// These "row pointers" are used by the PNG library to fill in the data.
		// Without them set properly, you only get the very top row (which isn't very
		// good!)  I think it's because of the way the library handles interlacing.

	case IH_8BIT:
		{
			png_set_IHDR( pWrite, pWriteInfo, iWidth, iHeight, 8, PNG_COLOR_TYPE_PALETTE, 
				PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
			
			// Create a pointer for the palette
			png_color pPNGPalette[256];
							
			// Now set the entries
			for (j = 0; j < 256; j++)
			{
				pPNGPalette[j].red		= m_byPalette[j * 3 + 0];
				pPNGPalette[j].green	= m_byPalette[j * 3 + 1];
				pPNGPalette[j].blue		= m_byPalette[j * 3 + 2];
			}

			png_set_PLTE( pWrite, pWriteInfo, pPNGPalette, 256);			
		}
		break;

	case IH_24BIT:
		{
			png_set_IHDR( pWrite, pWriteInfo, iWidth, iHeight, 8, PNG_COLOR_TYPE_RGB, 
				PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		}
		break;

	case IH_32BIT:
		{
			png_set_IHDR( pWrite, pWriteInfo, iWidth, iHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, 
				PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);			
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	// Set the gamma
	png_set_gAMA( pWrite, pWriteInfo, 1 / g_dfGamma);

	// Set the software creator
	char szKey[] = "Software";
	char szText[] = "Wally";
	
	png_text szSoftware;
	szSoftware.compression = PNG_TEXT_COMPRESSION_NONE;
	szSoftware.key = (png_charp)szKey;
	szSoftware.text = (png_charp)szText;
	szSoftware.text_length = sizeof (szText);

	png_set_text( pWrite, pWriteInfo, &szSoftware, 1);

	// Write the info
	png_write_info(pWrite, pWriteInfo);

	// Write the data
	switch (iBitDepth)
	{
	case IH_8BIT:
		{			
			png_bytepp row_pointers = new png_bytep[iHeight];

			if (row_pointers)
			{
				// Point at every row
				for (j = 0; j < iHeight; j++)
				{
					row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iWidth);
				}			

				png_write_image(pWrite, row_pointers);
			
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	case IH_24BIT:
		{
			png_bytepp row_pointers = new png_bytep[iHeight];

			if (row_pointers)
			{
				// Point at every row
				for (j = 0; j < iHeight; j++)
				{
					row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iWidth * 3);
				}			

				png_write_image(pWrite, row_pointers);
			
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	case IH_32BIT:
		{
			//ASSERT (FALSE);

			// TODO:  Where will the alpha channel be coming from?  Is it the first index, I? 
			png_bytepp row_pointers = new png_bytep[iHeight];

			if (row_pointers)
			{

				// Point at every row
				for (j = 0; j < iHeight; j++)
				{
					row_pointers[j] = (png_bytep)(m_pbyDecodedData + j * iWidth * 4);
				}			

				png_write_image(pWrite, row_pointers);
			
				delete []row_pointers;
				row_pointers = NULL;
			}
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	// We don't have an end, so tell the lib we're not going to write one
	png_write_end( pWrite, NULL);

	// Clean up
	png_destroy_write_struct(&pWrite, &pWriteInfo);

	if (!UseArchive())	
	{
		if (m_fpPNGOutput)
		{
			fclose (m_fpPNGOutput);
			m_fpPNGOutput = NULL;
		}
	}

	return TRUE;
}
