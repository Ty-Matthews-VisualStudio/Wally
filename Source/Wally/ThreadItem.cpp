// ThreadItem.cpp: implementation of the CThreadItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "ThreadItem.h"
#include "ThreadList.h"
#include "ThreadJob.h"
#include "MiscFunctions.h"
#include "ImageHelper.h"
#include "ReMip.h"
#include "ColorOpt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadItem::CThreadItem(LPCTSTR szSourceFile, LPCTSTR szDestinationDirectory, LPCTSTR szOffsetDirectory, int iDestinationType, int iImageType, CPackageDoc *pDoc)
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
	
	m_strErrorMessage = "";
	m_pThreadJob = NULL;
	m_pbyData = NULL;
	m_WalHeader	= NULL;

	if( iDestinationType == DESTINATION_TEXTURES )
	{
		m_pPackage = NULL;
	}
	else
	{
		m_pPackage = pDoc;
	}

	SetImageType (iImageType);	
	
	m_iDestinationType = iDestinationType;	
}

CThreadItem::~CThreadItem()
{
	if (GetJobStatus() == THREADJOB_WAITING)
	{
		// The thread hasn't even been created, so no sense trying to wait
		// for it to finish, eh?
		SetJobStatus (THREADJOB_END_THREAD);
	}
	else
	{
		if (GetJobStatus() < THREADJOB_DONE)
		{			
			SetMessage (THREADJOB_STOP);
		}
		while (GetJobStatus() != THREADJOB_END_THREAD)
		{		
			// Wait until the thread is completely cleaned out.
			Sleep (100);
		}
	}

	m_WalHeader = NULL;

	if( m_pbyData )
	{
		delete [] m_pbyData;
		m_pbyData = NULL;
	}

}


void CThreadItem::StartJob()
{		
	ASSERT (GetJobStatus() == THREADJOB_WAITING);	// Can't start the job if it already is!
	SetMessage (THREADJOB_START);	
	m_pThreadJob = (CThreadJob *)AfxBeginThread ( (AFX_THREADPROC) CThreadJob::MainLoop, (LPVOID)this);
}

void CThreadItem::StopJob()
{
	SetJobStatus (THREADJOB_STOPPED);
}


void CThreadItem::SetJobStatus (int iStatus)
{	
	ASSERT (iStatus > THREADJOB_STATUS_CODES);
	m_iJobStatus = iStatus;
}

int CThreadItem::GetJobStatus ()
{
	return m_iJobStatus;
}

void CThreadItem::SetMessage (int iMessage)
{
	ASSERT (iMessage < THREADJOB_MESSAGES_END);

	while (GetMessage() != THREADJOB_NOMESSAGE)
	{
		// Another message is waiting to be processed... please hold!		
		Sleep (100);
	}
	m_iMessage = iMessage;
}

int CThreadItem::GetMessage ()
{
	return m_iMessage;
}

bool CThreadItem::CheckForStopMessage()
{
	if (GetMessage() == THREADJOB_STOP)
	{
		ClearMessage();
		return true;
	}

	return false;	
}

void CThreadItem::ClearMessage()
{
	m_iMessage = THREADJOB_NOMESSAGE;
}

void CThreadItem::SetNext(CThreadItem *pItem)
{
	m_pNext = pItem;
}

CThreadItem *CThreadItem::GetNext()
{
	return m_pNext;
}

void CThreadItem::SetPrevious (CThreadItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CThreadItem *CThreadItem::GetPrevious()
{
	return m_pPrevious;
}

unsigned char *CThreadItem::GetBits (int iMipNumber)
{
	ASSERT ((iMipNumber < 4) && (iMipNumber >= 0));
	ASSERT (m_WalHeader);

	return (m_pbyData + m_WalHeader->offsets[iMipNumber]);

}

void CThreadItem::ConvertImage()
{
	SetJobStatus (THREADJOB_STARTED);	

	// This function is only to be called by the attached ThreadJob (m_pThreadJob) object.	
	CString strRawFileName("");
	CString strInternalName("");
	CString strErrorText("");	
	
	FILE *fp					= NULL;	
	unsigned char *pbyImageBits	= NULL;
	unsigned char *pbyPalette	= NULL;
	
//	div_t x_result, y_result;
	int iColorDepth = 0;
	int iWidth = 0;
	int iHeight = 0;	
	int iSizes[4];
	int iTotalSize = 0;
	int j = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	int x = 0;
		
	strRawFileName = GetRawFileName(m_strSourceFile);

	if (CheckForStopMessage())
	{
		SetJobStatus(THREADJOB_STOPPED);
		return;
	}

	// Go load up the image into CImageHelper
	m_ihHelper.LoadImage (m_strSourceFile);
		
	if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		SetJobStatus (THREADJOB_IH_ERROR);
		strErrorText = m_ihHelper.GetErrorText();		

		for (j = 0; j < strErrorText.GetLength(); j++)
		{
			if (strErrorText.GetAt(j) == '\n')
			{
				m_strErrorMessage += CRLF;
			}
			else
			{
				m_strErrorMessage += strErrorText.GetAt(j);
			}
		}
		return;		
	}

	if (!m_ihHelper.IsValidImage())
	{
		SetJobStatus (THREADJOB_IH_ERROR);
		m_strErrorMessage.Format ("%s is not a valid image file", m_strSourceFile);		
		return;
	}

	if (CheckForStopMessage())
	{
		SetJobStatus(THREADJOB_STOPPED);
		return;
	}
	
	iColorDepth = m_ihHelper.GetColorDepth();
	pbyImageBits = m_ihHelper.GetBits();
	pbyPalette = m_ihHelper.GetPalette();
	iWidth = m_ihHelper.GetImageWidth();
	iHeight = m_ihHelper.GetImageHeight();
	int iSourceImageType = m_ihHelper.GetImageType();
	
	if ((iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
	{
		SetJobStatus (THREADJOB_IH_ERROR);
		m_strErrorMessage.Format ("Texture is too large.  Textures must be less than %d x %d in size.", MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT);
		return;
	}

	int iWidths[4];
	int iHeights[4];
	
	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
		iSizes[j] = (iWidths[j] * iHeights[j]);
	}
	

// Neal - test of ANY SIZE image
//
//	// TODO:  support images that aren't evenly divisible by 16?
//	x_result = div(iWidth, 16);
//	y_result = div(iHeight, 16);
//
//	// Make sure it's of valid size
//	if ((x_result.rem != 0) || (y_result.rem != 0))
//	{
//		SetJobStatus (THREADJOB_NOT_DIV_16);
//		m_strErrorMessage = "The dimensions of this image are not evenly divisible by 16";
//		pbyImageBits = NULL;
//		return;
//	}	

	if (CheckForStopMessage())
	{
		SetJobStatus(THREADJOB_STOPPED);
		return;
	}
	
	switch (m_iDestinationType)
	{
	case DESTINATION_TEXTURES:
		{	
			int iExportColorDepth = iColorDepth;
			int iFlags = m_ihHelper.GetTypeFlags (m_iImageType);
			if ((iFlags & IH_TYPE_FLAG_SUPPORTS8BIT) && ((iFlags & IH_TYPE_FLAG_SUPPORTS24BIT) == 0))
			{
				// Supports 8BIT, but not 24BIT.
				iExportColorDepth = IH_8BIT;
			}
			
			if (g_bRetainDirectoryStructure)
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
			
			m_ihHelper.ConvertImage (iExportColorDepth, m_strDestinationDirectory, 
				m_iImageType, IH_USE_BATCH_SETTINGS | (g_bOverWriteFiles ? 0 : IH_SAVE_UNIQUE_FILENAME));
			
			if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
			{
				SetJobStatus (THREADJOB_IH_ERROR);
				m_strErrorMessage = m_ihHelper.GetErrorText();
				return;
			}
						
			m_strDestinationFile = strRawFileName + ".";
			m_strDestinationFile += m_ihHelper.GetWildCardExtension(m_iImageType);
			SetJobStatus (THREADJOB_SUCCESS);
		}
		break;

	case DESTINATION_PACKAGE:
		{
			int iWalHeaderSize = Q2_HEADER_SIZE;
			iTotalSize = iWalHeaderSize;
			for (j = 0; j < 4; j++)
			{
				iTotalSize += iSizes[j];
			}

			// Build a .wal file, so we can ReMip
			m_pbyData = new BYTE[iTotalSize];
			BYTE *pbyData = m_pbyData + iWalHeaderSize;

			memset (m_pbyData, 0, iTotalSize);
			m_WalHeader = (LPQ2_MIP_S)m_pbyData;

			m_WalHeader->height = iHeight;
			m_WalHeader->width = iWidth;

			m_WalHeader->offsets[0] = iWalHeaderSize;
			m_WalHeader->offsets[1] = iWalHeaderSize + iSizes[0];
			m_WalHeader->offsets[2] = iWalHeaderSize + iSizes[0] + iSizes[1];
			m_WalHeader->offsets[3] = iWalHeaderSize + iSizes[0] + iSizes[1] + iSizes[2];
			
			switch (iColorDepth)
			{
			case IH_8BIT:
				{
					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
									
					switch (m_pPackage->GetWADType())
					{
					case WAD2_TYPE:				
						m_Palette.SetPalette (quake1_pal, 256);
						memcpy (m_byPalette, pbyPalette, 768);
											
						for (j = 0; j < iSizes[0]; j++) 
						{				
							r = m_byPalette[pbyImageBits[j] * 3 + 0];
							g = m_byPalette[pbyImageBits[j] * 3 + 1];
							b = m_byPalette[pbyImageBits[j] * 3 + 2];					
							pTemp24Bit[j] = IRGB( 0, r, g, b);
						}
						m_Palette.Convert24BitTo256Color( pTemp24Bit, 
								pbyData, iWidth, iHeight, 0, GetDitherType(), FALSE);

						break;

					case WAD3_TYPE:
						m_Palette.SetPalette (pbyPalette, 256);
						memcpy (pbyData, pbyImageBits, iSizes[0]);
						break;

					default:
						ASSERT (false);
						break;
					}

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
						pTemp24Bit = NULL;
					}
				}

			break;	

			case IH_24BIT:
				{
					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
					CColorOptimizer ColorOpt;
					BYTE byPalette[256*3];
					strRawFileName = GetRawFileName(m_strSourceFile);
					char cFlag = strRawFileName.GetAt(0);			
					int iNumColors = (cFlag == '{' ? 255 : 256);

					for (j = 0; j < iSizes[0]; j++) 
					{				
						r = pbyImageBits[j * 3 + 0];
						g = pbyImageBits[j * 3 + 1];
						b = pbyImageBits[j * 3 + 2];
						pTemp24Bit[j] = IRGB( 0, r, g, b);
					}
									
					switch (m_pPackage->GetWADType())
					{
					case WAD2_TYPE:				
						m_Palette.SetPalette (quake1_pal, 256);
						break;

					case WAD3_TYPE:						
						ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, 256, FALSE);
						m_Palette.SetPalette( byPalette, 256);
						if (iNumColors == 255)
						{
							// Set index 255 to solid blue... it's our transparent index
							m_Palette.SetRGB( 255, 0, 0, 255);
						}
						break;

					default:
						ASSERT (false);
						break;
					}

					m_Palette.Convert24BitTo256Color( pTemp24Bit, 
							pbyData, iWidth, iHeight, 0, GetDitherType(), FALSE);

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
						pTemp24Bit = NULL;
					}
				}
				break;		// case IH_24BIT:

			default:

				ASSERT (false);
				break;

			}		// switch (iColorDepth)


			// Now that we've got the first mip done, all the sub-mips need to be built.

			// 512 is to reserve room for 512 byte header (max)
			#define MAX_DATA_SIZE (MAX_MIP_SIZE * MAX_MIP_SIZE + 512)
	
			BYTE	*pbyMipData		= NULL;
			pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);			
			
			if (!pbyMipData)
			{
				SetJobStatus (THREADJOB_OUT_OF_MEMORY);
#ifdef _DEBUG
				m_strErrorMessage = "Out of memory in CThreadItem::ConvertImage() -- pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE)";
#else
				m_strErrorMessage = "Could not allocate memory for conversion";
#endif
				pbyImageBits = NULL;
				m_WalHeader = NULL;		
				return;
			}	
			memset (pbyMipData, 0, MAX_DATA_SIZE);
			
			LPQ2_MIP_S pMipHeader = (LPQ2_MIP_S)pbyMipData;
			
			memcpy (pMipHeader, m_WalHeader, iWalHeaderSize);	
			memcpy( pbyMipData + pMipHeader->offsets[0], pbyData, iSizes[0]);

			if (CheckForStopMessage())
			{
				pMipHeader = NULL;
				if (pbyMipData)
				{
					delete pbyMipData;
					pbyMipData = NULL;
				}
				
				m_WalHeader = NULL;		
				SetJobStatus(THREADJOB_STOPPED);
				return;
			}
	
	
			///////////////////////////////
			// Build all the sub-mips //
			/////////////////////////////
			m_Palette.SetNumFullBrights( 0);
			m_Palette.RebuildWAL( pbyMipData);

			/////////////////////////
			// copy the data back //
			///////////////////////
			
			for (j = 1; j < 4; j++)
			{
				memcpy( m_pbyData + (m_WalHeader->offsets[j]), 
					pbyMipData + (pMipHeader->offsets[j]), iSizes[j]);		
			}	

			pMipHeader = NULL;
			free (pbyMipData);
			pbyMipData = NULL;
			
			m_strPackageName = strRawFileName;
			SetJobStatus (THREADJOB_SUCCESS);

		}		
		break;		// case DESTINATION_PACKAGE:

	default:
		ASSERT (false);
		break;
	}	
}

