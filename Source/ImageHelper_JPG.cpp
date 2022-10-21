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
//   JPG Support
////////////////////////////////////////////////////////////////////////////////////

// Here's the routine that will replace the standard error_exit method:

void CImageHelper::JPGErrorHandler (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	lpJPGErrorMgr lpErrorMgr = (lpJPGErrorMgr) cinfo->err;
  
	/* Create the message */
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message) (cinfo, buffer);

	lpErrorMgr->p_ImageHelper->SetErrorText (buffer);
	lpErrorMgr->p_ImageHelper->SetErrorCode (IH_JPG_CUSTOM_ERROR);

	/* Return control to the setjmp point */
	longjmp(lpErrorMgr->setjmp_buffer, 1);
}

void CImageHelper::JPGInitSource (j_decompress_ptr jDecompressInfo)
{
	lpJPGSourceManager pSource = (lpJPGSourceManager) jDecompressInfo->src;

	// We reset the empty-input-file flag for each image,
	// but we don't clear the input buffer.
	// This is correct behavior for reading a series of images from one source.
	
	pSource->iDataPosition = 0;
}

boolean CImageHelper::JPGFillInputBuffer (j_decompress_ptr jDecompressInfo)
{
	lpJPGSourceManager pSource = (lpJPGSourceManager) jDecompressInfo->src;

	pSource->pub.next_input_byte = pSource->pbySourceData;
	pSource->pub.bytes_in_buffer = pSource->iDataSize;
	pSource->iDataPosition = pSource->iDataSize;

	return TRUE;
}


void CImageHelper::JPGSkipInputData (j_decompress_ptr jDecompressInfo, long lNumBytes)
{
	lpJPGSourceManager pSource = (lpJPGSourceManager) jDecompressInfo->src;
	CImageHelper *pThis = (CImageHelper *)jDecompressInfo->client_data;

	// Just a dumb implementation for now.  Could use fseek() except
	// it doesn't work on pipes.  Not clear that being smart is worth
	// any trouble anyway --- large skips are infrequent.
   	if (lNumBytes > 0)
	{
	    while (lNumBytes > (long) pSource->pub.bytes_in_buffer)
		{
			lNumBytes -= (long) pSource->pub.bytes_in_buffer;

			if( pThis )
			{
				pThis->JPGFillInputBuffer(jDecompressInfo);
			}
			// note we assume that fill_input_buffer will never return FALSE,
			// so suspension need not be handled.			
		}
		
		pSource->pub.next_input_byte += (size_t) lNumBytes;
		pSource->pub.bytes_in_buffer -= (size_t) lNumBytes;
	}
}

void CImageHelper::JPGTermSource (j_decompress_ptr jDecompressInfo)
{
	// no work necessary here
}


void CImageHelper::JPGSetSource (j_decompress_ptr jDecompressInfo, BYTE *pbySourceData, UINT iDataSize)
{
	lpJPGSourceManager pSource = NULL;
	
	// The source object and input buffer are made permanent so that a series
	// of JPEG images can be read from the same file by calling jpeg_stdio_src
	// only before the first one.  (If we discarded the buffer at the end of
	// one image, we'd likely lose the start of the next one.)
	// This makes it unsafe to use this manager and a different source
	// manager serially with the same JPEG object.  Caveat programmer.
	
	if (jDecompressInfo->src == NULL)
	{
		// First time for this JPEG object?
		jDecompressInfo->src = (struct jpeg_source_mgr *) 
			(*jDecompressInfo->mem->alloc_small) ((j_common_ptr) jDecompressInfo, JPOOL_PERMANENT,
				  sizeof(JPGSourceManager));
    
		// Cast to our overridden struct
		pSource = (lpJPGSourceManager) jDecompressInfo->src;
		pSource->pbySourceData = pbySourceData;
		pSource->iDataSize = iDataSize;
	}

	pSource = (lpJPGSourceManager) jDecompressInfo->src;
	pSource->pub.init_source = CImageHelper::JPGInitSource;
	pSource->pub.fill_input_buffer = CImageHelper::JPGFillInputBuffer;
	pSource->pub.skip_input_data = CImageHelper::JPGSkipInputData;
	pSource->pub.resync_to_restart = jpeg_resync_to_restart;	// use default method
	pSource->pub.term_source = CImageHelper::JPGTermSource;	
	pSource->pub.bytes_in_buffer = 0;		// Forces fill_input_buffer on first read
	pSource->pub.next_input_byte = NULL;	// Uuntil buffer loaded
}


BOOL CImageHelper::DecodeJPG( int iFlags /* = 0 */)
{
	// This struct contains the JPEG decompression parameters and pointers to
	// working space (which is allocated as needed by the JPEG library).
	struct jpeg_decompress_struct jDecompressInfo;

	// Point back to ourself so the static functions can access this object
	jDecompressInfo.client_data = (LPVOID)this;

	// We use our private extension JPEG error handler.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.   
	JPGErrorMgr_s jError;

	LPBYTE pbyDecodedData;		// Output row buffer
	int row_stride;				// Physical row width in output buffer
		
	// We set up the normal JPEG error routines, then override error_exit.
	jDecompressInfo.err = jpeg_std_error(&jError.pub);
	jError.pub.error_exit = CImageHelper::JPGErrorHandler;
	jError.p_ImageHelper = this;
  
	// Establish the setjmp return context for my_error_exit to use.
	if (setjmp(jError.setjmp_buffer))
	{
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object, close the input file, and return.     
		jpeg_destroy_decompress(&jDecompressInfo);
		return FALSE;
	}
	
	// Now we can initialize the JPEG decompression object.
	jpeg_create_decompress(&jDecompressInfo);
		
	JPGSetSource(&jDecompressInfo, m_pbyEncodedData, GetDataSize());
	
	(void) jpeg_read_header(&jDecompressInfo, TRUE);
	(void) jpeg_start_decompress(&jDecompressInfo);

	row_stride = jDecompressInfo.output_width * jDecompressInfo.output_components;

	int iWidth = jDecompressInfo.image_width;
	int iHeight = jDecompressInfo.image_height;

	SetImageWidth (iWidth);
	SetImageHeight (iHeight);
	SetColorDepth (IH_24BIT);
	
	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}
	
	pbyDecodedData = new BYTE[iWidth * iHeight * 3];

	// This is how the library scans in data; using these JSAMPROW "rows."  Build
	// an array of iHeight row pointers, then point them at our buffer
	JSAMPARRAY jaRows = new JSAMPROW[iHeight];
	 
	for (int j = 0; j < iHeight; j++)
	{
		jaRows[j] = (JSAMPROW)(pbyDecodedData + (iWidth * 3 * j));
	}

	// I tried setting jpeg_read_scanlines to the number of rows (iHeight) but it
	// only read two.  Looping through one row at a time, however, posed no problems.  
	// Go figure!
	while (jDecompressInfo.output_scanline < jDecompressInfo.output_height)
	{
		(void) jpeg_read_scanlines(&jDecompressInfo, &jaRows[jDecompressInfo.output_scanline], 1);
	}

	// Finish off any miscellany decompression
	(void) jpeg_finish_decompress(&jDecompressInfo);

	// Clean this out
	jpeg_destroy_decompress(&jDecompressInfo);

	// Point our internal buffer
	m_pbyDecodedData = pbyDecodedData;

	// Clean up the row pointers
	if (jaRows)
	{
		delete []jaRows;
		jaRows = NULL;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
// This stuff is for encoding JPGs
//////////////////////////////////////////////////////////////////////////////////////////

void CImageHelper::JPGInitDestination (j_compress_ptr pjCompressInfo)
{
	CImageHelper *pThis = (CImageHelper *)pjCompressInfo->client_data;
	lpJPGDestinationManager pDestination = (lpJPGDestinationManager) pjCompressInfo->dest;

	// Init the temp buffer
	pDestination->pbyDestinationData = new BYTE[JPG_OUTPUT_BUFFER_SIZE];

	if (!pDestination->pbyDestinationData)
	{
		ERREXIT1(pjCompressInfo, JERR_OUT_OF_MEMORY, 0);
		return;
	}

	ZeroMemory( pDestination->pbyDestinationData, JPG_OUTPUT_BUFFER_SIZE );

	if( pThis )
	{
		if( pThis->UseArchive() )
		{
			// Just set to the Archive
			pDestination->lpMemFile = (LPVOID)pThis->GetArchive();
		}
		else
		{
			// Init the MemFile
			pDestination->lpMemFile = (LPVOID) new CMemFile(JPG_OUTPUT_BUFFER_SIZE);

			if( !pDestination->lpMemFile )
			{
				ERREXIT1(pjCompressInfo, JERR_OUT_OF_MEMORY, 0);
				return;
			}
		}
	}
	

	pDestination->iDataPosition = 0;
	pDestination->iDataSize = JPG_OUTPUT_BUFFER_SIZE;
	pDestination->pub.next_output_byte = pDestination->pbyDestinationData;	
	pDestination->pub.free_in_buffer = JPG_OUTPUT_BUFFER_SIZE;
}

boolean CImageHelper::JPGEmptyOutputBuffer (j_compress_ptr pjCompressInfo)
{
	CImageHelper *pThis = (CImageHelper *)pjCompressInfo->client_data;
	lpJPGDestinationManager pDestination = (lpJPGDestinationManager) pjCompressInfo->dest;

	if( pThis )
	{
		if( pThis->UseArchive())
		{
			CArchive *pArchive = (CArchive *)pDestination->lpMemFile;
			pArchive->Write (pDestination->pbyDestinationData, JPG_OUTPUT_BUFFER_SIZE);
		}
		else
		{
			CMemFile *pMemFile = (CMemFile *)pDestination->lpMemFile;
			pMemFile->Write (pDestination->pbyDestinationData, JPG_OUTPUT_BUFFER_SIZE);
		}
	}

	pDestination->pub.next_output_byte = pDestination->pbyDestinationData;
	pDestination->pub.free_in_buffer = pDestination->iDataSize;

	return TRUE;
}


void CImageHelper::JPGTermDestination (j_compress_ptr pjCompressInfo)
{
	CImageHelper *pThis = (CImageHelper *)pjCompressInfo->client_data;
	lpJPGDestinationManager pDestination = (lpJPGDestinationManager) pjCompressInfo->dest;
	size_t iDataSize = JPG_OUTPUT_BUFFER_SIZE - pDestination->pub.free_in_buffer;

	// Write any data remaining in the buffer
	if (iDataSize > 0)
	{
		if( pThis )
		{
			if( pThis->UseArchive() )
			{
				CArchive *pArchive = (CArchive *)pDestination->lpMemFile;
				pArchive->Write (pDestination->pbyDestinationData, iDataSize);
			}
			else
			{
				CMemFile *pMemFile = (CMemFile *)pDestination->lpMemFile;
				pMemFile->Write (pDestination->pbyDestinationData, iDataSize);
			}
		}
	}	
}


void CImageHelper::JPGSetDestination (j_compress_ptr pjCompressInfo)
{
	lpJPGDestinationManager pDestination = NULL;
	
	if (pjCompressInfo->dest == NULL)
	{	
		// First time for this JPEG object?
		pjCompressInfo->dest = (struct jpeg_destination_mgr *)
			(*pjCompressInfo->mem->alloc_small) ((j_common_ptr) pjCompressInfo, JPOOL_PERMANENT,
			sizeof(JPGDestinationManager));
	}

	pDestination = (lpJPGDestinationManager) pjCompressInfo->dest;
	pDestination->pub.init_destination = CImageHelper::JPGInitDestination;
	pDestination->pub.empty_output_buffer = CImageHelper::JPGEmptyOutputBuffer;
	pDestination->pub.term_destination = CImageHelper::JPGTermDestination;	
}


BOOL CImageHelper::EncodeJPG ()
{
	struct jpeg_compress_struct jCompressInfo;

	// Point back to ourself so the static functions can access this object
	jCompressInfo.client_data = (LPVOID)this;
  
	// We use our private extension JPEG error handler.
	// Note that this struct must live as long as the main JPEG parameter
	// struct, to avoid dangling-pointer problems.   
	JPGErrorMgr_s jError;
	jError.p_ImageHelper = this;
	
	// Allocate and initialize JPEG compression object
	jCompressInfo.err = jpeg_std_error(&jError.pub);
	jError.pub.error_exit = CImageHelper::JPGErrorHandler;
  
	// Establish the setjmp return context for my_error_exit to use.
	if (setjmp(jError.setjmp_buffer))
	{
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object and return.
		if (!UseArchive())
		{
			lpJPGDestinationManager pDestination = (lpJPGDestinationManager) jCompressInfo.dest;
			CMemFile *pMemFile = (CMemFile *)pDestination->lpMemFile;

			delete pMemFile;
			pDestination->lpMemFile = NULL;
		}
		jpeg_destroy_compress(&jCompressInfo);		
		return FALSE;
	}
	
	// Now we can initialize the JPEG compression object
	jpeg_create_compress(&jCompressInfo);

	// Set the destination object	
	JPGSetDestination(&jCompressInfo);

	int iWidth = GetImageWidth();
	int iHeight = GetImageHeight();

	// First we supply a description of the input image.
	// Four fields of the cinfo struct must be filled in:   
	jCompressInfo.image_width = iWidth;
	jCompressInfo.image_height = iHeight;
	jCompressInfo.input_components = 3;
	jCompressInfo.in_color_space = JCS_RGB;
	
	// Now use the library's routine to set default compression parameters.
	// (You must set at least jCompressInfo.in_color_space before calling this,
	// since the defaults depend on the source color space.)	
	jpeg_set_defaults(&jCompressInfo);
  
	// Now you can set any non-default parameters you wish to.
	// Here we just illustrate the use of quality (quantization table) scaling:	

	// TODO:  set a slider scale in Options to change the quality here?
	jpeg_set_quality(&jCompressInfo, 100, TRUE /* limit to baseline-JPEG values */);
	

	// TRUE ensures that we will write a complete interchange-JPEG file
	// Pass TRUE unless you are very sure of what you're doing.
	jpeg_start_compress(&jCompressInfo, TRUE);

	int iColorDepth = GetColorDepth();
	BYTE *pbySourceData = NULL;
	CMemBuffer mbSourceData;

	switch (iColorDepth)
	{
	case IH_8BIT:
		{
			// We need to build a 24-bit data source
			pbySourceData = mbSourceData.GetBuffer(iWidth * iHeight * 3, 0);
			BYTE *pby8BitData = GetDecodedData();
			UINT iOffset = 0;
			
			for (int y = 0; y < iHeight; y++)
			{
				for (int x = 0; x < iWidth; x++)
				{
					iOffset = y * iWidth + x;
					
					CopyMemory (pbySourceData + (iOffset * 3), m_byPalette + (pby8BitData[iOffset] * 3), 3);
				}
			}
		}
		break;

	case IH_24BIT:
		{
			// We're fine!
			pbySourceData = GetDecodedData();
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	// Build the SAMPROW pointer array
	JSAMPARRAY jaRows = new JSAMPROW[iHeight];

	// Set the array pointers
	for (int j = 0; j < iHeight; j++)
	{
		jaRows[j] = pbySourceData + (j * iWidth * 3);
	}

	while (jCompressInfo.next_scanline < jCompressInfo.image_height)
	{
		// jpeg_write_scanlines expects an array of pointers to scanlines
		// Here the array is only one element long, but you could pass
		// more than one scanline at a time if that's more convenient. (Ty- doesn't work!)
		jpeg_write_scanlines(&jCompressInfo, &jaRows[jCompressInfo.next_scanline], 1);
	}

	// Finish compression
	jpeg_finish_compress(&jCompressInfo);

	if ( !UseArchive() )
	{
		lpJPGDestinationManager pDestination = (lpJPGDestinationManager) jCompressInfo.dest;
		CMemFile *pMemFile = (CMemFile *)pDestination->lpMemFile;
	
		// We need now to actually write this to disk
		FILE *fp = fopen (m_strFileName, "wb");

		if (!fp)
		{
			SetErrorCode( IH_ERROR_WRITING_FILE);
			return FALSE;
		}
				
		int iDataSize = pMemFile->GetLength();
		BYTE *pbyDestinationData = new BYTE[iDataSize];

		if (!pbyDestinationData)
		{
			SetErrorCode( IH_OUT_OF_MEMORY);
			return FALSE;
		}

		pMemFile->SeekToBegin();
		pMemFile->Read( pbyDestinationData, iDataSize);

		fwrite (pbyDestinationData, iDataSize, 1, fp);
		fclose (fp);

		delete pMemFile;
		pDestination->lpMemFile = NULL;

		if (pbyDestinationData)
		{
			delete []pbyDestinationData;
			pbyDestinationData = NULL;
		}
		
	}

	// Release JPEG compression object
	jpeg_destroy_compress(&jCompressInfo);

	return TRUE;
}
