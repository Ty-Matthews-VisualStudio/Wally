/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//  ImageHelper_TEX.cpp : implementation of the CImageHelper TEX functions
//
//  Created by Ty Matthews, 6-28-2001
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
//   TEX Support
////////////////////////////////////////////////////////////////////////////////////

#define TEX_ALPHACHANNEL (1UL<<0)   // texture has alpha channel (for old version support)
#define TEX_32BIT        (1UL<<1)   // texture needs to be in 32-bit quality uploaded if can

BOOL CImageHelper::EncodeTEX ()
{
	int iWidth	= GetImageWidth();
	int	iHeight = GetImageHeight();
	DWORD dwDataSize = 0;
	
	LPDWORD pdwChunk = NULL;
	LPTEX_TDAT_CHUNK lpHeader = NULL;
	LPTEX_ADAT_CHUNK lpAnimationHeader = NULL;
	LPBYTE pbyPackedData = NULL;
	LPBYTE pbyImageData = NULL;
	CMemBuffer mbData;	

	// Figure out how much space we need
	dwDataSize = 
		8 +									// TVER
		4 +									// TDAT marker
		sizeof( TEX_TDAT_CHUNK ) +			// TDAT
		4 +									// FRMS
		(iWidth * iHeight * 3) +			// Image data
		8 +									// ANIMADAT marker
		sizeof( TEX_ADAT_CHUNK );			// ADAT

	pbyPackedData = mbData.GetBuffer( dwDataSize );

	if( !pbyPackedData )
	{
		SetErrorCode( IH_OUT_OF_MEMORY );
		return FALSE;
	}	

	// Set up the TVER chunk
	pdwChunk = (LPDWORD)pbyPackedData;
	(*pdwChunk) = ( 'T' | 'V' << 8 | 'E' << 16 | 'R' << 24 );
	pdwChunk++;
	(*pdwChunk) = 4;
	pdwChunk++;

	// Set up the TDAT chunk	
	(*pdwChunk) = ( 'T' | 'D' << 8 | 'A' << 16 | 'T' << 24 );
	pdwChunk++;
	lpHeader = (LPTEX_TDAT_CHUNK)pdwChunk;	
	lpHeader->Flags = 0;
	
	// TODO:  What are the MipLevels used for?  These are hard-coded values
	lpHeader->FirstMipLevel = 6;
	lpHeader->FineMipLevels = 0;
	lpHeader->MexWidth = iWidth << lpHeader->FirstMipLevel;
	lpHeader->MexHeight = iHeight << lpHeader->FirstMipLevel;
	lpHeader->NumFrames = 1;

	// Set up the FRMS chunk
	pdwChunk = (LPDWORD)( (LPBYTE)pdwChunk + sizeof( TEX_TDAT_CHUNK ) );
	(*pdwChunk) = ( 'F' | 'R' << 8 | 'M' << 16 | 'S' << 24 );
	pdwChunk++;

	pbyImageData = (LPBYTE)pdwChunk;
	memcpy( pbyImageData, m_pbyDecodedData, (iWidth * iHeight * 3) );

	pdwChunk = (LPDWORD)( pbyImageData + (iWidth * iHeight * 3) );
	
	// Set up the ADAT chunk	
	(*pdwChunk) = ( 'A' | 'N' << 8 | 'I' << 16 | 'M' << 24 );
	pdwChunk++;
	(*pdwChunk) = ( 'A' | 'D' << 8 | 'A' << 16 | 'T' << 24 );
	pdwChunk++;

	// TODO:  This needs to be set via a new editor... these values are hard-coded
	lpAnimationHeader = (LPTEX_ADAT_CHUNK)pdwChunk;
	lpAnimationHeader->NumAnimations = 1;
	strcpy_s( lpAnimationHeader->Name, sizeof(lpAnimationHeader->Name), "OnlyAnim" );
	lpAnimationHeader->SecondsPerFrame = (float)0.02;
	lpAnimationHeader->NumberOfFrames = 1;
	lpAnimationHeader->Indexes = 0;

	if (m_bUseArchive)
	{
		m_parFile->Write( pbyPackedData, dwDataSize );
	}
	else
	{	
		try
		{
			mbData.WriteToFile( m_strFileName );
		}
		catch( CWallyException we )
		{
			SetErrorCode( IH_ERROR_WRITING_FILE );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CImageHelper::DecodeTEX( int iFlags /* = 0 */ )
{
	LPBYTE pbyData = m_pbyEncodedData;	
	LPTEX_TDAT_CHUNK lpHeader = NULL;
	LPTEX_ADAT_CHUNK lpAnimationHeader = NULL;
	
	DWORD dwDataLength = GetFileLength();
	DWORD dwVersion = 0;
	int iWidth = 0;
	int iHeight = 0;
	int iSize = 0;
	int j = 0;
	
	LPBYTE pbyEndData = pbyData + dwDataLength;
	LPBYTE pbyImageData = NULL;
	LPBYTE pbyPosition = NULL;
	LPDWORD pdwCompare = NULL;
	
	// Look for the version number
	pbyPosition = pbyData;
	while( pbyPosition < (pbyEndData - 8) )
	{
		if( (*pbyPosition) == 'T' )
		{
			// Possible match... let's look at the whole thing
			pdwCompare = (LPDWORD)pbyPosition;
			if( (*pdwCompare) == ('T' | 'V' << 8 | 'E' << 16 | 'R' << 24) )
			{
				pdwCompare++;
				dwVersion = (*pdwCompare);
				break;
			}
			else
			{
				pbyPosition++;
			}
		}
		else
		{
			pbyPosition++;
		}
	}

	if( pbyPosition >= pbyEndData )
	{
		SetErrorCode( IH_TEX_MALFORMED );
		return FALSE;
	}

	if( dwVersion != 4 )
	{
		SetErrorCode( IH_TEX_UNSUPPORTED_VERSION );
		return FALSE;
	}

	// Now look for the TDAT chunk
	pbyPosition = pbyData;
	while( pbyPosition < (pbyEndData - 4) )
	{
		if( (*pbyPosition) == 'T' )
		{
			// Possible match... let's look at the whole thing
			pdwCompare = (LPDWORD)pbyPosition;
			if( (*pdwCompare) == ('T' | 'D' << 8 | 'A' << 16 | 'T' << 24) )
			{
				lpHeader = (LPTEX_TDAT_CHUNK)(pbyPosition + 4);
				break;
			}
			else
			{
				pbyPosition++;
			}
		}
		else
		{
			pbyPosition++;
		}
	}

	if( pbyPosition >= pbyEndData )
	{
		SetErrorCode( IH_TEX_MALFORMED );
		return FALSE;
	}

	if( (pbyPosition + sizeof(TEX_TDAT_CHUNK)) >= pbyEndData )
	{
		SetErrorCode( IH_TEX_MALFORMED );
		return FALSE;
	}

	if( lpHeader->NumFrames > 1 )
	{
		SetErrorCode( IH_TEX_TOO_MANY_FRAMES );
		return FALSE;
	}

	// Look for the ADAT chunk... this should be at the very end, so let's work backwards to save some time
	pbyPosition = pbyEndData - sizeof(TEX_ADAT_CHUNK) - 4;
	while( pbyPosition >= pbyData )
	{
		if( (*pbyPosition) == 'A' )
		{
			// Possible match... let's look at the whole thing
			pdwCompare = (LPDWORD)pbyPosition;
			if( (*pdwCompare) == ('A' | 'D' << 8 | 'A' << 16 | 'T' << 24) )
			{
				lpAnimationHeader = (LPTEX_ADAT_CHUNK)(pbyPosition + 4);
				break;
			}
			else
			{
				pbyPosition--;
			}
		}
		else
		{
			pbyPosition--;
		}
	}

	if( pbyPosition < pbyData )
	{
		SetErrorCode( IH_TEX_MALFORMED );
		return FALSE;
	}

	if( 
		(strcmp( lpAnimationHeader->Name, "OnlyAnim" )) ||
		( lpAnimationHeader->NumberOfFrames > 1 ) ||
		( lpAnimationHeader->SecondsPerFrame != (float)0.02 )
	)
	{
		SetErrorCode( IH_TEX_ANIMATION_DATA );
		return FALSE;
	}	

	iWidth = (lpHeader->MexWidth >> lpHeader->FirstMipLevel);
	iHeight = (lpHeader->MexHeight >> lpHeader->FirstMipLevel);

	SetImageWidth( iWidth );
	SetImageHeight( iHeight );	

	if( lpHeader->Flags & TEX_ALPHACHANNEL )
	{
		SetColorDepth( IH_32BIT );
		iSize = iWidth * iHeight * 4;
	}
	else
	{
		// All TEX files are at least 24-bit
		SetColorDepth( IH_24BIT );
		iSize = iWidth * iHeight * 3;
	}

	if( iFlags & IH_LOAD_DIMENSIONS )
	{
		return TRUE;
	}

	// Build the buffer	
	m_pbyDecodedData = new BYTE[iSize];

	if( !m_pbyDecodedData )
	{
		SetErrorCode (IH_OUT_OF_MEMORY);			
		return FALSE;
	}
	
	// Now look for the FRMS chunk
	pbyPosition = pbyData;
	while( pbyPosition < (pbyEndData - 4) )
	{
		if( (*pbyPosition) == 'F' )
		{
			// Possible match... let's look at the whole thing
			pdwCompare = (LPDWORD)pbyPosition;
			if( (*pdwCompare) == ('F' | 'R' << 8 | 'M' << 16 | 'S' << 24) )
			{
				pbyImageData = pbyPosition + 4;
				break;
			}
			else
			{
				pbyPosition++;
			}
		}
		else
		{
			pbyPosition++;
		}
	}

	if( pbyPosition >= pbyEndData )
	{
		SetErrorCode( IH_TEX_MALFORMED );
		return FALSE;
	}

	if( lpHeader->Flags & TEX_ALPHACHANNEL )
	{
		if( (pbyPosition + (iWidth * iHeight * 4 * lpHeader->NumFrames) ) >= pbyEndData )
		{
			SetErrorCode( IH_TEX_MALFORMED );
			return FALSE;
		}

		memcpy( m_pbyDecodedData, pbyImageData, iSize );			

		// Build the alpha channel
		m_pbyAlphaChannel = new BYTE[ iWidth * iHeight ];

		if( !m_pbyAlphaChannel )
		{
			SetErrorCode (IH_OUT_OF_MEMORY);			
			return FALSE;
		}
		
		for( j = 0; j < (iWidth * iHeight); j++ )
		{			
			m_pbyAlphaChannel[j] = pbyImageData[(j * 4) + 3];
		}		
	}
	else
	{
		if( (pbyPosition + (iWidth * iHeight * 3 * lpHeader->NumFrames) ) >= pbyEndData )
		{
			SetErrorCode( IH_TEX_MALFORMED );
			return FALSE;
		}

		memcpy( m_pbyDecodedData, pbyImageData, iSize );
	}

	return TRUE;
}