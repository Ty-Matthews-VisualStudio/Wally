// BatchJob.cpp: implementation of the CBatchJob class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "BatchJob.h"

#include "MiscFunctions.h"
#include "ImageHelper.h"
#include "ReMip.h"
#include "ColorOpt.h"
#include "WallyPal.h"
#include "PackageDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBatchJob::CBatchJob()
{

}

CBatchJob::CBatchJob( LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory, int iDestinationType, int iImageType, CPackageDoc *pDoc )
{
	ASSERT (szSourceFile);
	if (iDestinationType == DESTINATION_TEXTURES)
	{
		ASSERT (szDestinationDirectory);
	}
	else
	{
		ASSERT (pDoc);
	}

	m_strSourceFile = szSourceFile;	
	m_strDestinationDirectory = szDestinationDirectory;
	m_strDestinationFile = "";
	m_strOffsetDirectory = TrimSlashes (szOffsetDirectory);
	m_strOffsetDirectory += "\\";
		
	if( iDestinationType == DESTINATION_TEXTURES )
	{
		m_pPackageDoc = NULL;
	}
	else
	{
		m_pPackageDoc = pDoc;
	}

	m_iImageType = iImageType;	
	m_iDestinationType = iDestinationType;
}

CBatchJob::~CBatchJob()
{

}

void CBatchJob::Initialize()
{
	CThreadJob::Initialize();
}

LPCTSTR CBatchJob::GetSourceFile()
{
	return (LPCTSTR)m_strSourceFile;
}

LPCTSTR CBatchJob::GetDestinationFile()
{
	return (LPCTSTR)m_strDestinationFile;
}

LPCTSTR CBatchJob::GetMessage()
{
	m_strMessage = GetSourceFile();
	m_strMessage += " --> ";
	m_strMessage += GetDestinationFile();
	m_strMessage += CRLF;
	if( m_strErrorMessage.GetLength() > 0 )
	{
		m_strMessage += m_strErrorMessage;		
	}
	else
	{
		m_strMessage += "OK";
	}	

	return (LPCTSTR)m_strMessage;
}

void CBatchJob::Process()
{
	CString strRawFileName("");
	CString strInternalName("");
	CString strErrorText("");	

	CImageHelper ihHelper;
	CWallyPalette Palette;
	CColorOptimizer ColorOpt;
	BYTE byPalette[ 768 ];

	LPBYTE pbyBuffer		= NULL;
	LPBYTE pbyData			= NULL;
	LPBYTE pbyImageBits		= NULL;
	LPBYTE pbyPalette		= NULL;
	LPBYTE pbyMipData		= NULL;
	
	LPQ2_MIP_S pWalHeader	= NULL;
	COLOR_IRGB	*pTemp24Bit = NULL;	

	list<DWORD> lstBlueIndexes;
	list<DWORD>::iterator itIndex;
	BOOL bSetBlueIndex = FALSE;
	BOOL bNonBlueColor = FALSE;
	BOOL bBlueIndex = FALSE;

	BYTE byRedHold = 0;
	BYTE byGreenHold = 0;
	BYTE byBlueHold = 0;
	
	int iColorDepth = 0;
	int iWidth = 0;
	int iHeight = 0;	
	int iSizes[4];
	int iWidths[4];
	int iHeights[4];
	int iTotalSize = 0;
	int iSourceImageType = 0;
	int iExportColorDepth = 0;
	int iFlags = 0;
	int iBlueIndex = 0;
	int iNumColors = 0;
	BOOL bNameExists = FALSE;	
	
	int j = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	int x = 0;

	try
	{
		strRawFileName = GetRawFileName( m_strSourceFile );

		if( m_iDestinationType == DESTINATION_PACKAGE )
		{
			// Check to see if the image is already in the WAD
			if( m_pPackageDoc->IsNameInList( strRawFileName.Left( 15 ) ) )
			{
				if( !g_bOverWriteFiles )
				{
					strErrorText.Format( "Image name %s already exists in the package", strRawFileName.Left( 15 ) );
					throw( (LPCTSTR)strErrorText );
				}
				
				bNameExists = TRUE;
			}
		}

		// Go load up the image
		ihHelper.LoadImage (m_strSourceFile);
	
		if( ihHelper.GetErrorCode() != IH_SUCCESS )
		{
			throw ( (LPCTSTR)ihHelper.GetErrorText() );
		}
		
		if( !ihHelper.IsValidImage() )
		{
			strErrorText.Format( "%s is not a valid image file", m_strSourceFile );
			throw ( (LPCTSTR)strErrorText );
		}
		
		if( Stopped() )
		{
			throw ( "Thread was stopped prematurely" );
		}

		iColorDepth = ihHelper.GetColorDepth();
		pbyImageBits = ihHelper.GetBits();
		pbyPalette = ihHelper.GetPalette();
		iWidth = ihHelper.GetImageWidth();
		iHeight = ihHelper.GetImageHeight();
		iSourceImageType = ihHelper.GetImageType();
		
		if ( (iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT) )
		{			
			strErrorText.Format( "Texture is too large.  Textures must be less than %d x %d in size.", MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT );
			throw ( (LPCTSTR)strErrorText );
		}
		
		for( j = 0; j < 4; j++ )
		{
			iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
			iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
			iSizes[j] = (iWidths[j] * iHeights[j]);
		}
	
		if( Stopped() )
		{
			throw ( "Thread was stopped prematurely" );
		}
		
		switch( m_iDestinationType )
		{
		case DESTINATION_TEXTURES:
			{	
				iExportColorDepth = g_iExportColorDepth;
				iFlags = ihHelper.GetTypeFlags( m_iImageType );

				if ((iFlags & IH_TYPE_FLAG_SUPPORTS8BIT) && ((iFlags & IH_TYPE_FLAG_SUPPORTS24BIT) == 0))
				{					
					// Supports 8BIT, but not 24BIT.
					iExportColorDepth = IH_8BIT;
				}
				
				if( g_bRetainDirectoryStructure )
				{
					// Figure out the directories we need to create
					CString strRemainingPath("");
					int iPosition = 0;

					strRemainingPath = TrimSlashes (GetPathToFile (m_strSourceFile));

					strRemainingPath = strRemainingPath.Right (strRemainingPath.GetLength() - m_strOffsetDirectory.GetLength());
					
					m_strDestinationDirectory = TrimSlashes (m_strDestinationDirectory);
					m_strDestinationDirectory += "\\";

					while (strRemainingPath != "")
					{
						iPosition = strRemainingPath.Find ("\\");

						if (iPosition == -1)
						{
							m_strDestinationDirectory += strRemainingPath;
							strRemainingPath = "";						
						}
						else
						{
							m_strDestinationDirectory += strRemainingPath.Left(iPosition);
							m_strDestinationDirectory += "\\";							
							strRemainingPath = 	strRemainingPath.Right (strRemainingPath.GetLength() - (iPosition + 1));
						}		
						
						_mkdir (m_strDestinationDirectory);
					}
				}
				
				ihHelper.ConvertImage( iExportColorDepth, m_strDestinationDirectory, 
					m_iImageType, IH_USE_BATCH_SETTINGS | (g_bOverWriteFiles ? 0 : IH_SAVE_UNIQUE_FILENAME));
				
				if( ihHelper.GetErrorCode() != IH_SUCCESS )
				{					
					throw( (LPCTSTR)ihHelper.GetErrorText() );					
				}
							
				m_strDestinationFile = strRawFileName + ".";
				m_strDestinationFile += ihHelper.GetWildCardExtension(m_iImageType);				
			}
			break;

		case DESTINATION_PACKAGE:
			{
				iTotalSize = Q2_HEADER_SIZE + 1024;
				for( j = 0; j < 4; j++ )
				{
					iTotalSize += iSizes[j];
				}

				// Build a .wal file, so we can ReMip
				pbyBuffer = new BYTE[ iTotalSize ];

				if( !pbyBuffer )
				{
					throw( "Failed to allocate memory for image buffer" );
				}

				pbyData = pbyBuffer + Q2_HEADER_SIZE;
				memset( pbyBuffer, 0, Q2_HEADER_SIZE );
				
				pWalHeader = (LPQ2_MIP_S)pbyBuffer;

				pWalHeader->height = iHeight;
				pWalHeader->width = iWidth;

				pWalHeader->offsets[0] = Q2_HEADER_SIZE;
				pWalHeader->offsets[1] = Q2_HEADER_SIZE + iSizes[0];
				pWalHeader->offsets[2] = Q2_HEADER_SIZE + iSizes[0] + iSizes[1];
				pWalHeader->offsets[3] = Q2_HEADER_SIZE + iSizes[0] + iSizes[1] + iSizes[2];

				pTemp24Bit = new COLOR_IRGB[ iSizes[0] * sizeof( COLOR_IRGB) ];

				if( !pTemp24Bit )
				{
					throw( "Failed to allocate memory for 24-bit image buffer" );
				}

				if( Stopped() )
				{
					throw ( "Thread was stopped prematurely" );
				}
				
				switch( iColorDepth )
				{
				case IH_8BIT:
					{
						switch( m_pPackageDoc->GetWADType() )
						{
						case WAD2_TYPE:
							{
								Palette.SetPalette( quake1_pal, 256 );
													
								for (j = 0; j < iSizes[0]; j++) 
								{				
									r = pbyPalette[ pbyImageBits[j] * 3 + 0 ];
									g = pbyPalette[ pbyImageBits[j] * 3 + 1 ];
									b = pbyPalette[ pbyImageBits[j] * 3 + 2 ];
									pTemp24Bit[j] = IRGB( 0, r, g, b );
								}

								if( Stopped() )
								{
									throw ( "Thread was stopped prematurely" );
								}

								Palette.Convert24BitTo256Color( pTemp24Bit, 
										pbyData, iWidth, iHeight, 0, GetDitherType(), FALSE);
							}

							break;

						case WAD3_TYPE:
							{
								// Let's check to see if any of our pixels are pure blue
								for( j = 0; j < 255; j++ )
								{
									if(
										( pbyPalette[ j * 3 + 0 ] == 0 ) &&
										( pbyPalette[ j * 3 + 1 ] == 0 ) &&
										( pbyPalette[ j * 3 + 2 ] == 255 )
									)
									{
										lstBlueIndexes.push_back( j );
										bSetBlueIndex = TRUE;
									}									
								}

								if( bSetBlueIndex )
								{									
									// We need to map over some pixels first

									// Let's see if index 255 is already blue
									if(
										 ( pbyPalette[ 255 * 3 + 0 ] != 0 ) &&
										 ( pbyPalette[ 255 * 3 + 1 ] != 0 ) &&
										 ( pbyPalette[ 255 * 3 + 2 ] != 255 )
									)									
									{
										// We need to flip around ones that are already at index 255
										
										// Switch palette entries with the first one we found to be blue
										itIndex = lstBlueIndexes.begin();
										pbyPalette[ (*itIndex) * 3 + 0 ] = pbyPalette[ 255 * 3 + 0 ];
										pbyPalette[ (*itIndex) * 3 + 1 ] = pbyPalette[ 255 * 3 + 1 ];
										pbyPalette[ (*itIndex) * 3 + 2 ] = pbyPalette[ 255 * 3 + 2 ];

										pbyPalette[ 255 * 3 + 0 ] = 0;
										pbyPalette[ 255 * 3 + 1 ] = 0;
										pbyPalette[ 255 * 3 + 2 ] = 255;
										
										// Now flip indexes
										for( j = 0; j < iSizes[0]; j++ )
										{
											if( pbyImageBits[j] == (*itIndex) )
											{
												pbyImageBits[j] = 255;
											}
											else
											{
												if( pbyImageBits[j] == 255 )
												{
													pbyImageBits[j] = (BYTE)(*itIndex);
												}
											}
										}
									}
									else
									{
										// Index 255 is already blue, so no need to switch palette entries.  Find
										// all blue indexes and just move them
										for( itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++ )
										{
											for( j = 0; j < iSizes[0]; j++ )
											{
												if( pbyImageBits[j] == (*itIndex) )
												{
													pbyImageBits[j] = 255;
												}
											}
										}
									}
								}
								
								Palette.SetPalette( pbyPalette, 256 );
								memcpy( pbyData, pbyImageBits, iSizes[0] );
							}
							break;

						default:
							ASSERT( FALSE );
							break;
						}						
					}
					break;	

				case IH_24BIT:
					{										
						switch( m_pPackageDoc->GetWADType() )
						{
						case WAD2_TYPE:
							{
								Palette.SetPalette (quake1_pal, 256);
								
								for( j = 0; j < iSizes[0]; j++ ) 
								{				
									r = pbyImageBits[ j * 3 + 0 ];
									g = pbyImageBits[ j * 3 + 1 ];
									b = pbyImageBits[ j * 3 + 2 ];
									pTemp24Bit[j] = IRGB( 0, r, g, b );
								}
							}
							break;

						case WAD3_TYPE:
							{
								if( strRawFileName.GetAt( 0 ) == '{' )
								{
									bSetBlueIndex = TRUE;
								}

								for( j = 0; j < iSizes[0]; j++ ) 
								{				
									r = pbyImageBits[ j * 3 + 0 ];
									g = pbyImageBits[ j * 3 + 1 ];
									b = pbyImageBits[ j * 3 + 2 ];

									if( (r == 0) && (g == 0) && (b == 255) )
									{
										// Store these away, we'll set them to the hold color in a bit
										lstBlueIndexes.push_back( j );
										bSetBlueIndex = TRUE;
									}
									else
									{
										// Only set these values once... save some time
										if( !bNonBlueColor )
										{
											bNonBlueColor = TRUE;
											byRedHold = r;
											byGreenHold = g;
											byBlueHold = b;
										}
										pTemp24Bit[j] = IRGB( 0, r, g, b );
									}									
								}

								// Set these guys to some other color for now, so we don't add blue to the palette
								if( bSetBlueIndex )
								{
									for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
									{
										pTemp24Bit[ (*itIndex) ] = IRGB( 0, byRedHold, byGreenHold, byBlueHold );
									}
								}

								if( Stopped() )
								{
									throw ( "Thread was stopped prematurely" );
								}

								if( bSetBlueIndex )
								{
									// Only retrieve 255 indexes
									ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 255, FALSE);

									byPalette[255 * 3 + 0] = 0;
									byPalette[255 * 3 + 1] = 0;
									byPalette[255 * 3 + 2] = 255;									
								}
								else
								{									
									ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 256, FALSE);									
								}
								
								Palette.SetPalette( byPalette, 256);

								// Put our indexes back to blue now
								for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
								{
									pTemp24Bit[ (*itIndex) ] = IRGB( 255, 0, 0, 255);
								}
							}
							break;

						default:
							ASSERT (false);
							break;
						}

						if( Stopped() )
						{
							throw ( "Thread was stopped prematurely" );
						}

						Palette.Convert24BitTo256Color( pTemp24Bit, 
								pbyData, iWidth, iHeight, 0, GetDitherType(), FALSE);						
					}
					break;		// case IH_24BIT:

				default:

					ASSERT( FALSE );
					break;

				}		// switch (iColorDepth)

				if( Stopped() )
				{
					throw ( "Thread was stopped prematurely" );
				}


				// Now that we've got the first mip done, all the sub-mips need to be built.
						
				///////////////////////////////
				// Build all the sub-mips //
				/////////////////////////////
				Palette.SetNumFullBrights( 0 );
				Palette.RebuildWAL( pbyBuffer );

				LPBYTE pbyBits[4];

				for (j = 0; j < 4; j++)
				{
					pbyBits[j] = pbyBuffer + pWalHeader->offsets[j];
				}

				// Try one more time to see if that name is there... with multiple threads
				// running during a batch conversion, it is possible that while this guy was 
				// building the image, an identically named image was inserted into the WAD
				// by another thread.  If we attempt to insert this sucker, an ASSERT will
				// occur.
				if( m_pPackageDoc->IsNameInList( strRawFileName.Left( 15 ) ) )
				{
					if( !g_bOverWriteFiles )
					{						
						strErrorText.Format( "Image name %s already exists in the package", strRawFileName.Left( 15 ) );
						throw( (LPCTSTR)strErrorText );
					}
					
					bNameExists = TRUE;
				}

				if( bNameExists )
				{
					m_pPackageDoc->ReplaceImage( pbyBits, &Palette, strRawFileName.Left( 15 ), iWidth, iHeight );
				}
				else
				{
					m_pPackageDoc->AddImage( pbyBits, &Palette, strRawFileName.Left( 15 ), iWidth, iHeight, TRUE);
				}

				m_strDestinationFile = strRawFileName.Left( 15 );				
			}		
			break;		// case DESTINATION_PACKAGE:

		default:
			ASSERT( FALSE );
			break;
		}
	}
	catch( LPCTSTR szErrorMessage )
	{
		for (j = 0; j < strlen( szErrorMessage ); j++)
		{
			if( szErrorMessage[j] == '\n' )
			{
				AddToErrorMessage( CRLF );
			}
			else
			{
				AddToErrorMessage( szErrorMessage[j] );
			}
		}
	}

	if( pbyBuffer )
	{
		delete []pbyBuffer;
		pbyBuffer = NULL;
	}

	if( pTemp24Bit )
	{
		delete []pTemp24Bit;
		pTemp24Bit = NULL;
	}

	if( pbyMipData )
	{
		delete []pbyMipData;
		pbyMipData = NULL;
	}
}