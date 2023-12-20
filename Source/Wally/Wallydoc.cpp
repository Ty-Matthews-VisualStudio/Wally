/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyDoc.cpp : implementation of the CWallyDoc class
//
// Created by Ty Matthews, 1-16-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "TextureInfo.h"
#include "ToolDlg.h"
#include "TextureFlags.h"
#include "WallyDoc.h"
#include "MiscFunctions.h"
#include "GraphicsFunctions.h"
#include "ImageHelper.h"
#include "NewImageDlg.h"
#include "MainFrm.h"
#include "RenameImageDlg.h"
#include "DuplicateNameDlg.h"
#include "ColorOpt.h"
#include "PackageDoc.h"
#include "SinFlagsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWallyDoc

IMPLEMENT_DYNCREATE(CWallyDoc, CDocument)

BEGIN_MESSAGE_MAP(CWallyDoc, CDocument)
	//{{AFX_MSG_MAP(CWallyDoc)
	ON_COMMAND(ID_TP_FLAGS, OnTpFlags)
	ON_COMMAND(ID_VIEW_TOOL_OPTIONS, OnViewToolOptions)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_UPDATE_COMMAND_UI(ID_TP_FLAGS, OnUpdateTpFlags)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyDoc construction/destruction

CWallyDoc::CWallyDoc()
{
	int j = 0;
	for ( j = 0; j < 4; j++)	
		MipArray[j].mip_pointer = NULL;

//	m_EffectsMipData.width       = 0;
//	m_EffectsMipData.height      = 0;
//	m_EffectsMipData.mip_pointer = NULL;
	m_pEffectLayer    = NULL;
	
	// Set stuff to initial values	
	Q2Header.flags    = 0;
	Q2Header.contents = 0;
	Q2Header.value    = 0;

	memset (Q2Header.name, 0, 32);
	memset (Q2Header.animname, 0, 32);

	memset (Q1Header.name, 0, 16);
	Q1Header.width = 0;
	Q1Header.height = 0;

	for (j = 0; j < 4; j++)
	{
		Q2Header.offsets[j] = 0;
		Q1Header.offsets[j] = 0;
	}

	m_bCloneReset        = true;
	m_bCloneInitialized  = false;

	m_pSelection = NULL;
	m_iGameType  = FILE_TYPE_START;
	m_LastGameType = FILE_TYPE_START - 1000;

	ClearPalette();
	m_pPackageDoc = NULL;
	m_strPackageFile = "";
	m_strPackageMipName = "";

	m_Base.GetLayer()->Init();

	m_bFreeEffectsData = true;
}

CWallyDoc::~CWallyDoc()
{
	DeleteContents();	
}

void CWallyDoc::FreeEffectsData()
{
	// Clear out the effects block

//	if (m_EffectsMipData.mip_pointer)
//	{
//		free( m_EffectsMipData.mip_pointer);
//		m_EffectsMipData.mip_pointer = NULL;
//	}
//
//	m_EffectsMipData.width  = 0;
//	m_EffectsMipData.height = 0;

	if (m_pEffectLayer)
	{
		delete m_pEffectLayer;
		m_pEffectLayer = NULL;
	}
}

void CWallyDoc::FreeCutAndPasteData()
{
	// Clear out the effects block

	if (HasSelection())
	{
		m_pSelection->SetBoundsRect( 0, 0, 0, 0);

		if ((m_pSelection != &m_Base) && (m_pSelection != &m_Cutout))
			delete m_pSelection;

		m_pSelection = NULL;
		UpdateAllViews( NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Name:		MergeSelectionIntoBase
// Action:		Pastes the current select into the base layer
//
// Parameter:	szUndoMsg: msg to use for undo, if NULL, no undo is saved
/////////////////////////////////////////////////////////////////////////////
void CWallyDoc::MergeSelectionIntoBase( LPCTSTR szUndoMsg)
{
	// Clear out the effects block

	if (HasSelection())
	{
		// Neal - paste the data from the selection into the base layer

		CLayer* pBaseLayer = m_Base.GetLayer();
		CLayer* pLayer     = m_pSelection->GetLayer();

		if (pLayer != pBaseLayer)
		{
			ASSERT( m_pSelection != &m_Base);

			if (szUndoMsg)
				m_Undo.SaveUndoState( this, szUndoMsg);

			CRect rRect = m_pSelection->GetBoundsRect();

			int iWidth  = GetWidth();
			int iHeight = GetHeight();

			for (int j = rRect.top; j < rRect.bottom; j++)
			{
				// Neal - cutouts are allowed to go out-of-bounds
				//
				if ((j >= 0) && (j < iHeight))
				{
					for (int i = rRect.left; i < rRect.right; i++)
					{
						// Neal - TODO: what about offsets ???
						// Neal - TODO: what about tiled view ???

						// Neal - cutouts are allowed to go out-of-bounds
						//
						if ((i >= 0) && (i < iWidth))
							pBaseLayer->SetPixel( NULL, i, j, pLayer->GetPixel( i, j));
					}
				}
			}
		}
		FreeCutAndPasteData();
		SetModifiedFlag();
	}
}

CLayer* CWallyDoc::GetCurrentLayer()
{
	CSelection* pSel   = GetSelection();
	CLayer*     pLayer = NULL;

	if (pSel)
		pLayer = &(pSel->m_Layer);

	ASSERT( pSel);
	ASSERT( pLayer);

	return (pLayer);
}

void CWallyDoc::DeleteMips()
{	
	// neal - bugfix - don't memleak

	for (int j = 0; j < 4; j++)
	{
		if (MipArray[j].mip_pointer != NULL)
		{
			delete [] MipArray[j].mip_pointer;	// neal - must use array delete to delete arrays!
			MipArray[j].mip_pointer = NULL;
		}
	}
}

void CWallyDoc::DeleteContents()
{	
	// neal - bugfix - don't memleak

	DeleteMips();

	FreeEffectsData();
	FreeCutAndPasteData();

	m_Base.m_Layer.FreeMem();

	CDocument::DeleteContents();
}


BOOL CWallyDoc::OnNewDocument()
{	
	int w = 0;
	int h = 0;
	int j = 0;
	int MipSize = 0;
	//int iSizes[4];
	int Q2HeaderSize = Q2_HEADER_SIZE;	
	int Q1HeaderSize = Q1_HEADER_SIZE;	
	int SinHeaderSize = SIN_HEADER_SIZE;
	int M8HeaderSize = M8_HEADER_SIZE;

	memset (&Q2Header, 0, Q2HeaderSize);
	memset (&Q1Header, 0, Q1HeaderSize);
	memset (&SinHeader, 0, SinHeaderSize);
	memset (&M8Header, 0, M8HeaderSize);
	M8Header.version = M8_VERSION;

	SinHeader.nonlit = (float)0.5;
	SinHeader.friction = (float)1.0;
	SinHeader.animtime = (float)0.2;
	SinHeader.color[0] = (float)1.0;
	SinHeader.color[1] = (float)1.0;
	SinHeader.color[2] = (float)1.0;

	CString strTitle("");
	strTitle.Format ("Untitled%d", g_iUntitledCount++);
	
	if ((g_iDocHeight == 0) && (g_iDocWidth == 0))
	{
		if (! CDocument::OnNewDocument())
			return FALSE;	

		w = g_iPreviousTextureWidth;
		h = g_iPreviousTextureHeight;
		SetDimensions(w, h);
		SetOriginalBitDepth (g_iDocColorDepth);
		
		SetTitle( strTitle);		
	}	
	else
	{
		// This part is called when doing "Paste as New" or "File...Convert".  It 
		// requires that g_iDocWidth and g_iDocHeight are set to the appropriate
		// width/height of the respective image being created.  Checking for 
		// accurate division by 16 is handled by the respective calling function

		if (!CDocument::OnNewDocument())
		   return FALSE;	

		//SetModifiedFlag(TRUE);					
		h = g_iDocHeight;
		w = g_iDocWidth;
		SetDimensions (w, h);

		SetOriginalBitDepth (g_iDocColorDepth);
				
		// Set these back to 0 
		g_iDocHeight = 0;
		g_iDocWidth  = 0;
		
		CString TempName ("");
		
		switch (g_iTextureNameSetting)
		{
		case BLANK_DIR:			
			break;

		case PRESET_DIR :		
			TempName = g_strDefaultTextureName;
			break;

		case PARENT_DIR :			
			break;

		default:
			ASSERT (false);		// Unhandled Wally option
			break;
		}

		char *szName = NULL;

		TempName = TempName.Left (63);
		szName = TempName.GetBuffer (TempName.GetLength());
		strcpy_s (SinHeader.name, sizeof(SinHeader.name), szName);
		TempName.ReleaseBuffer();
	
		TempName = TempName.Left (31);
		szName = TempName.GetBuffer (TempName.GetLength());
		strcpy_s (Q2Header.name, sizeof(Q2Header.name), szName);
		TempName.ReleaseBuffer();

		TempName = TempName.Left (15);
		szName = TempName.GetBuffer (TempName.GetLength());
		strcpy_s (Q1Header.name, sizeof(Q1Header.name), szName);
		TempName.ReleaseBuffer();		

		TempName = TempName.Left (31);
		szName = TempName.GetBuffer (TempName.GetLength());
		strcpy_s (M8Header.name, sizeof(M8Header.name), szName);
		TempName.ReleaseBuffer();	
	}

	SetTitle( strTitle);

	// Neal - BUGFIX - paste as new image (24 bit) was asserting
	m_Base.m_Layer.SetNumBits( m_iOriginalBitDepth);

	RebuildLayerAndMips( true);
	//theApp.UpdateAllDocs( HINT_UPDATE_IMAGE_SIZE);	// Neal - fixes missing submips on new wal view (by commenting it out)

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWallyDoc serialization

void CWallyDoc::Serialize( CArchive& ar)
{
	// neal - TODO: when we use our own .wly file format,
	// we should start saving all this then, too!
//	int  iSize = 0;
	BYTE byHasEffects = 0;

	if (ar.IsStoring())
		CopyLayerToMip();

	if (m_Undo.IsAnyUndoInProgress())
	{
		//////////////////////////
		// Undo specific stuff //
		////////////////////////

		// neal - TODO: CSelection should serialize the IRGB
		// data instead of calling SerializeQuake2()
		
		switch (GetGameType())
		{
		case FILE_TYPE_QUAKE2:
			SerializeQuake2( ar);
			break;

		case FILE_TYPE_QUAKE1:
			SerializeQuake1( ar);
			break;

		case FILE_TYPE_HERETIC2:
			SerializeHeretic2( ar);
			break;

		case FILE_TYPE_SIN:
			SerializeSin( ar);
			break;

		case FILE_TYPE_TGA:
		case FILE_TYPE_PCX:
		case FILE_TYPE_BMP:
		case FILE_TYPE_PNG:
		case FILE_TYPE_JPG:
		case FILE_TYPE_TEX:
			SerializeImage( ar);
			break;			

		default:
			SerializeQuake2( ar);
			break;
		}
		

		// neal - TODO: come up with a robust way of preserving 
		// m_pSelection within the context of a saved file

		// neal - TODO: if selection memory becomes dynamically
		// allocated this needs to be more intelligent!

		if (ar.IsStoring())
		{
			ar << (DWORD )m_pSelection;
			
			if (m_pEffectLayer)		// save the effects layer
			{
				byHasEffects = 1;
				ar << byHasEffects;
				m_pEffectLayer->Serialize( ar, FALSE);
			}
			else					// no effect layer
			{
				byHasEffects = 0;
				ar << byHasEffects;
			}
		}
		else	// loading
		{
			DWORD dwTemp;
			ar >> dwTemp;
			m_pSelection = (CSelection* )dwTemp;

			// restore the effects layer

			ar >> byHasEffects;

			if (byHasEffects)
			{
				// Neal - first, free left-over mip buffer
				FreeEffectsData();
				// Neal - next, alloc buffer
				m_pEffectLayer = new CLayer;

				if (m_pEffectLayer)
				{
					m_pEffectLayer->Serialize( ar, FALSE);
				}
				else
				{
					ASSERT( FALSE);	// out-of-mem?
				}
			}
			else
			{
				// Neal - fixes line and shape drawing mode "over-apply" bug

				if (m_bFreeEffectsData)
				{
					// Neal - effects buffer bug fix

					FreeEffectsData();
				}
			}
		}
		m_Base.Serialize( ar);
		m_Cutout.Serialize( ar);
	}
	else	// no undo in progress
	{
		if (ar.IsStoring())
			m_Undo.MarkAsModified();

		/////////////////////////////////
		// Game specific file formats //
		///////////////////////////////

		int iType = GetGameType();
		
		switch (iType)
		{
		case FILE_TYPE_QUAKE2:
			SerializeQuake2 (ar);
			break;

		case FILE_TYPE_QUAKE1:
			if (m_pPackageDoc)
			{
				SerializePackage (m_strPackageFile == "" ? true : false, NULL);
			}
			else
			{
				SerializeQuake1 (ar);
			}
			break;

		case FILE_TYPE_HERETIC2:
			SerializeHeretic2 (ar);
			break;

		case FILE_TYPE_SIN:
			SerializeSin (ar);
			break;

		case FILE_TYPE_HALF_LIFE:
			ASSERT (ar.IsStoring());	// HL textures cannot be loaded from disk!
			// If there is no attached doc, go find a WAD to add to
			SerializePackage (m_strPackageFile == "" ? true : false, NULL);
			break;

		case FILE_TYPE_TGA:
		case FILE_TYPE_PCX:
		case FILE_TYPE_BMP:
		case FILE_TYPE_PNG:
		case FILE_TYPE_JPG:
		case FILE_TYPE_TEX:
			SerializeImage( ar);
			break;

		default:
			ASSERT (false);		// Unhandled game type!
			break;
		}
		//if (! ar.IsStoring())	// Neal - wait until later (no palette yet)
		//	CopyMipToLayer();
	}
}

void CWallyDoc::SerializeImage (CArchive& ar)
{
	CFile *pFile = ar.GetFile();
	CImageHelper ihSerialize;
	CLayer *pLayer = GetCurrentLayer();

	CString strFilePath(pFile->GetFilePath());

	if (m_Undo.IsAnyUndoInProgress())
	{
		pLayer->Serialize( ar, TRUE);

		if (!ar.IsStoring())
		{
			SetDimensions( pLayer->GetWidth(), pLayer->GetHeight());
		}
	}
	else
	{
		if (ar.IsStoring())
		{
			// TODO:  This uses CLayer::GetPixel() to pick up the current image data.  
			// Should we add a function to CLayer for BYTE access instead of doing this?

			COLOR_IRGB IRGBData;

			UINT iWidth = pLayer->GetWidth();
			UINT iHeight = pLayer->GetHeight();
			UINT iColorDepth = GetOriginalBitDepth(); 
			UINT iByteCount = iColorDepth / 8;
						
			UINT iSize = iWidth * iHeight * iByteCount;
			CMemBuffer mbImageData(iSize);
			LPBYTE pbyImageData = mbImageData.GetBuffer();
			BYTE byPalette[768];
			
			if (!pbyImageData)
			{
				ASSERT (FALSE);
				return;
			}

			switch (iColorDepth)
			{
			case 8:
				{
					GetPalette(byPalette, 256);

					UINT x, y;
					UINT iOffset = 0;

					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iOffset = (y * iWidth) + x;

							IRGBData = pLayer->GetPixel (x, y);
							pbyImageData[iOffset] = GetIValue (IRGBData);
						}
					}
				}
				break;

			case 24:
				{
					UINT x, y;
					UINT iOffset = 0;

					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iOffset = (y * iWidth) + x;

							IRGBData = pLayer->GetPixel (x, y);

							pbyImageData[iOffset * 3 + 0] = GetRValue (IRGBData);
							pbyImageData[iOffset * 3 + 1] = GetGValue (IRGBData);
							pbyImageData[iOffset * 3 + 2] = GetBValue (IRGBData);							
						}
					}
				}
				break;

			case 32:
			{
				UINT x, y;
				UINT iOffset = 0;

				for (y = 0; y < iHeight; y++)
				{
					for (x = 0; x < iWidth; x++)
					{
						iOffset = (y * iWidth) + x;

						IRGBData = pLayer->GetPixel(x, y);						
						pbyImageData[iOffset * 4 + 0] = GetRValue(IRGBData);
						pbyImageData[iOffset * 4 + 1] = GetGValue(IRGBData);
						pbyImageData[iOffset * 4 + 2] = GetBValue(IRGBData);
					}
				}
			}
			break;

			default:
				ASSERT (FALSE);
				break;
			}			
			
			ihSerialize.SaveImage (iColorDepth, &ar, pbyImageData, byPalette, iWidth, iHeight);
			
			if (ihSerialize.GetErrorCode() != IH_SUCCESS)
			{
				CString strError("");
				strError.Format ("Failed to write %s.  Error code = %s\n\n", strFilePath, ihSerialize.GetErrorText());
				AfxMessageBox (strError);
				return;
			}
		}

		else
		// We're loading up
		
		{
			ihSerialize.LoadImage (pFile);

			if (ihSerialize.GetErrorCode() != IH_SUCCESS)
			{
				CString strError("");
				strError.Format ("Failed to read %s.  Error code = %s\n\n", strFilePath, ihSerialize.GetErrorText());
				AfxMessageBox (strError);
				return;
			}

			UINT iWidth      = ihSerialize.GetImageWidth();
			UINT iHeight     = ihSerialize.GetImageHeight();
			UINT iColorDepth = ihSerialize.GetColorDepth(); 
			UINT iByteCount  = iColorDepth / 8;

			SetOriginalBitDepth( iColorDepth);
			m_Base.m_Layer.SetNumBits( iColorDepth);	// Neal - fixes NumBits == 8, instead of 24 bug

			UINT iSize = iWidth * iHeight * iByteCount;			
			BYTE byPalette[768];

			LPBYTE pbyImageData = ihSerialize.GetBits();
			CWallyView *pWallyView = GetView();

			SetDimensions (iWidth, iHeight);
			RebuildLayerAndMips( FALSE);

			switch (iColorDepth)
			{
			case 8:
				{
					// Build the IRGB data
					memcpy (byPalette, ihSerialize.GetPalette(), 768);
					SetPalette (byPalette, 256, TRUE);
					
					UINT x, y, r, g, b;
					UINT iIndex = 0;

					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iIndex = pbyImageData[y * iWidth + x];

							r = byPalette[iIndex * 3 + 0];
							g = byPalette[iIndex * 3 + 1];
							b = byPalette[iIndex * 3 + 2];

							pLayer->SetPixel (pWallyView, x, y, IRGB( iIndex, r, g, b));
						}
					}					
				}
				break;

			case 24:
				{
					CColorOptimizer ColorOpt;
					LPCOLOR_IRGB pIRGBData = NULL;/*pLayer->GetData(); */

					pIRGBData = new COLOR_IRGB[iWidth * iHeight];

					UINT x, y, r, g, b;
					UINT iIndex = 0;
					
					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iIndex = y * iWidth + x;

							r = pbyImageData[iIndex * 3 + 0];
							g = pbyImageData[iIndex * 3 + 1];
							b = pbyImageData[iIndex * 3 + 2];

							// Set it initially to no index (we need to calculate first)
							//pLayer->SetPixel (pWallyView, x, y, IRGB( 0, r, g, b));
							pIRGBData[iIndex] = IRGB( 0, r, g, b);
						}
					}

					BOOL bPaletteLoaded = FALSE;
	
					if (! g_bBuildOptimizedPalette)
					{
						bPaletteLoaded = LoadDefaultEditingPalette( NULL, GetPalette(), 256);
					}

					if (! bPaletteLoaded)
					{
						// Come up with a palette						
						ColorOpt.Optimize( pIRGBData, iWidth, iHeight, byPalette, 256, TRUE);
						SetPalette (byPalette, 256, TRUE);					
					}

					// Use this to hold the indexes for 8-bit data
					CMemBuffer mb8BitData(iWidth * iHeight);
					LPBYTE pby8BitData = mb8BitData.GetBuffer();

					if (!pby8BitData)
					{
						CString strError("");
						strError.Format ("Out of memory error reading %s", strFilePath);
						AfxMessageBox (strError);
						return;
					}

					// Go find those indexes
					Convert24BitTo256Color( pIRGBData, pby8BitData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					// Now go back and set the indexes properly
					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iIndex = y * iWidth + x;

							r = pbyImageData[iIndex * 3 + 0];
							g = pbyImageData[iIndex * 3 + 1];
							b = pbyImageData[iIndex * 3 + 2];

							pLayer->SetPixel (pWallyView, x, y, IRGB( pby8BitData[iIndex], r, g, b));
						}
					}
					
					if (pIRGBData)
					{
						delete []pIRGBData;
						pIRGBData = NULL;
					}
				}
				break;

			case 32:
				{
AfxMessageBox( "This is a 32-bit image. Wally does not yet support editing\n"
			   "images with an alpha channel.\n\nYou should close this image NOW!");

					CColorOptimizer ColorOpt;
					LPCOLOR_IRGB pIRGBData = NULL;/*pLayer->GetData(); */

					pIRGBData = new COLOR_IRGB[iWidth * iHeight];

					UINT x, y, r, g, b, a;
					UINT iIndex = 0;
					
					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iIndex = y * iWidth + x;

							r = pbyImageData[iIndex * 4 + 0];
							g = pbyImageData[iIndex * 4 + 1];
							b = pbyImageData[iIndex * 4 + 2];

							// Set it initially to no index (we need to calculate first)
							//pLayer->SetPixel (pWallyView, x, y, IRGB( 0, r, g, b));
							pIRGBData[iIndex] = IRGB( 0, r, g, b);
						}
					}

					BOOL bPaletteLoaded = FALSE;
	
					if (! g_bBuildOptimizedPalette)
					{
						bPaletteLoaded = LoadDefaultEditingPalette( NULL, GetPalette(), 256);
					}

					if (! bPaletteLoaded)
					{
						// Come up with a palette						
						ColorOpt.Optimize( pIRGBData, iWidth, iHeight, byPalette, 256, TRUE);
						SetPalette (byPalette, 256, TRUE);					
					}

					// Use this to hold the indexes for 8-bit data
					CMemBuffer mb8BitData(iWidth * iHeight);
					LPBYTE pby8BitData = mb8BitData.GetBuffer();

					if (!pby8BitData)
					{
						CString strError("");
						strError.Format ("Out of memory error reading %s", strFilePath);
						AfxMessageBox (strError);
						return;
					}

					// Go find those indexes
					Convert24BitTo256Color( pIRGBData, pby8BitData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					// Now go back and set the indexes properly
					for (y = 0; y < iHeight; y++)
					{
						for (x = 0; x < iWidth; x++)
						{
							iIndex = y * iWidth + x;

							r = pbyImageData[iIndex * 4 + 0];
							g = pbyImageData[iIndex * 4 + 1];
							b = pbyImageData[iIndex * 4 + 2];
							a = pbyImageData[iIndex * 4 + 3];

							// Neal - TODO: store the alpha channel data

							pLayer->SetPixel (pWallyView, x, y, IRGB( pby8BitData[iIndex], r, g, b));
						}
					}
					
					if (pIRGBData)
					{
						delete []pIRGBData;
						pIRGBData = NULL;
					}
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}			
		}		
	}	
}

void CWallyDoc::SerializePackage (bool bSaveAs, LPCTSTR szPath)
{
	// Get out if there's an undo in progress
	if (m_Undo.IsAnyUndoInProgress())
	{
		ASSERT( FALSE);		// Shouldn't be here if there's an undo/redo!
		return;
	}

	int j = 0;
	int k = 0;

	if (g_bRebuildSubMipsOnSave)
	{
		BeginWaitCursor();
		//RebuildSubMips();
		UpdateAllDIBs( TRUE);
		EndWaitCursor();
	}

	// Check to see if we're attached to a currently open PackageDoc
	if ((m_pPackageDoc) && (!bSaveAs))
	{
		m_pPackageDoc->UpdateImageData(this);
		SetModifiedFlag (false);
		return;
	}

	if ((m_pPackageDoc) && (m_strPackageFile == ""))
	{
		m_pPackageDoc->UpdateImageData(this);
		SetModifiedFlag (false);
		return;
	}
	
	
	CImageHelper ihHelper;
	unsigned char *pbyBits[4];

	for (j = 0; j < 4; j++)
	{
		pbyBits[j] = GetBits(j);
	}	
	
	// We're not attached, so let's go see if there's at least a path
	// to a WAD we were opened from.
	if (!bSaveAs)
	{			
		ASSERT (m_strPackageFile != "");		// Hard to save if there is no path!

		ihHelper.LoadImage (m_strPackageFile);
		if (ihHelper.GetErrorCode() == IH_SUCCESS)
		{
			// If there's a duplicate name, remove the old one first.  The user
			// has already saved to this WAD before, so we assume that they're
			// looking to update the data.
			CWADItem *pItem = ihHelper.IsNameInList(m_strPackageMipName);
			if (pItem)
			{
				ihHelper.RemoveImageFromWAD (pItem);
			}

			ihHelper.AddImageToWAD (pbyBits, GetPalette(), m_strPackageMipName, Width(), Height());
			ihHelper.SaveWAD (m_strPackageFile);
			if (ihHelper.GetErrorCode() != IH_SUCCESS)
			{
				AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			}
			else
			{
				SetModifiedFlag (false);
			}
			return;
		}

		// If we can't find the file, that's okay, we'll prompt the user.  If anything else, get out
		if (ihHelper.GetErrorCode() != IH_FILE_NOT_FOUND)
		{	
			AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			return;
		}
		else
		{
			AfxMessageBox ("Whoops, I can't seem to find the original WAD.\nPlease select a WAD package to add this item.", MB_ICONEXCLAMATION);
		}
	}
	
	if ((ihHelper.GetErrorCode() == IH_FILE_NOT_FOUND) || (bSaveAs))
	{
		if (!szPath)
		{
			CString strWildCard("Half-Life/Quake1 package (*.wad)|*.wad||");		
			CString strTitle;
			strTitle.LoadString( IDS_FILESAVE_TITLE);			
			CString strAppendExtension ("wad");			

			// Create a CFileDialog, init with our strings
			CFileDialog	dlgSave (FALSE, strAppendExtension, NULL, 
				OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, strWildCard, NULL);

			// Set some of the CFileDialog vars
			if (g_strFileSaveDirectory !=  "")
			{
				dlgSave.m_ofn.lpstrInitialDir = g_strFileSaveDirectory;
			}
			dlgSave.m_ofn.lpstrTitle = strTitle;			

			if (dlgSave.DoModal() == IDOK)
			{
				g_strFileSaveDirectory = dlgSave.GetPathName().Left(dlgSave.m_ofn.nFileOffset);					
				m_strPackageFile = (dlgSave.GetPathName());

				int iExtensionMarker = dlgSave.m_ofn.nFileExtension;
				int iFileNameLength = m_strPackageFile.GetLength();

				// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
				// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
				// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
				// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
				// We have to check for each instance and add the extension, as required.

				if ((m_strPackageFile.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
				{
					m_strPackageFile += ".";
				}

				if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
				{				
					// User didn't type in the extension			
					m_strPackageFile += "wad";
				}
			}
			else
			{
				return;
			}
		}
		else
		{		
			m_strPackageFile = szPath;
		}

		// Let's go see if it's there.
		FILE *fp = NULL;
		errno_t err = fopen_s (&fp, m_strPackageFile, "r");

		if (err == 0)
		{
			fclose (fp);
			// It's there, let's first see if maybe it's open already in Wally			

			POSITION pos = theApp.PackageDocTemplate->GetFirstDocPosition();
			CPackageDoc *pDoc = NULL;
			bool bReplace = false;
			
			while (pos != NULL)
			{
				pDoc = (CPackageDoc *)theApp.PackageDocTemplate->GetNextDoc(pos);
				if (pDoc->GetPathName() == m_strPackageFile)
				{
					// The package is already open!
					CString strName (m_strPackageMipName);
					bool bFinished = false;

					while (!bFinished)
					{						
						if (pDoc->IsNameInList(strName))
						{
							CDuplicateNameDlg dlgDuplicate;
							dlgDuplicate.SetName (m_strPackageMipName);
							dlgDuplicate.SetMaxLength(15);
							if (dlgDuplicate.DoModal() == IDOK)
							{
								bReplace = dlgDuplicate.ReplaceImage();
								if (bReplace)
								{									
									bFinished = true;
								}
								else
								{
									strName = dlgDuplicate.GetName();
								}
							}
							else
							{
								return;
							}
						}
						else
						{
							bFinished = true;
						}
					}
					m_strPackageMipName = strName;				
					SetTitle (m_strPackageMipName);
					CWADItem *pWADItem = NULL;

					if (bReplace)
					{
						pWADItem = pDoc->ReplaceImage (this, m_strPackageMipName);
					}
					else
					{
						pWADItem = pDoc->AddImage (this, m_strPackageMipName, true);
					}
					
					SetModifiedFlag (false);
					pDoc->SetModifiedFlag (true);
					m_strPathName = m_strPackageMipName;
					
					if (m_pPackageDoc)
					{
						m_pPackageDoc->BreakDocConnection(this);						
					}
							
					pWADItem->SetWallyDoc (this);				
					SetPackageDoc (pDoc);

					// Reset the icon
					m_LastGameType = FILE_TYPE_START - 1000;
					return;
				}				
			}			

			// The doc isn't open in Wally, so go open it and then write.

			ihHelper.LoadImage (m_strPackageFile, IH_LOAD_ONLYPACKAGE);
			if (ihHelper.GetErrorCode() == IH_SUCCESS)
			{
				switch (GetGameType())
				{
				case FILE_TYPE_QUAKE1:
					if (ihHelper.GetWADType() != WAD2_TYPE)
					{
						AfxMessageBox ("This is not a Quake1 WAD file", MB_ICONSTOP);
						return;
					}
					break;

				case FILE_TYPE_HALF_LIFE:
					if (ihHelper.GetWADType() != WAD3_TYPE)
					{
						AfxMessageBox ("This is not a Half-Life WAD file", MB_ICONSTOP);
						return;
					}					
					break;

				default:
					ASSERT (false);
					break;
				}
				
				CString strName (m_strPackageMipName);
				bool bFinished = false;

				while (!bFinished)
				{
					CWADItem *pItem = ihHelper.IsNameInList(strName);
					if (pItem)
					{
						CDuplicateNameDlg dlgDuplicate;
						dlgDuplicate.SetName (m_strPackageMipName);
						dlgDuplicate.SetMaxLength(15);
						if (dlgDuplicate.DoModal() == IDOK)
						{
							bool bReplace = dlgDuplicate.ReplaceImage();
							if (bReplace)
							{
								ihHelper.RemoveImageFromWAD (pItem);
								bFinished = true;
							}
							else
							{
								strName = dlgDuplicate.GetName();
							}
						}
						else
						{
							return;
						}
					}
					else
					{
						bFinished = true;
					}
				}
				m_strPackageMipName = strName;
				SetTitle (m_strPackageMipName);

				ihHelper.AddImageToWAD (pbyBits, GetPalette(), m_strPackageMipName, Width(), Height());
				ihHelper.SaveWAD (m_strPackageFile);
				if (ihHelper.GetErrorCode() != IH_SUCCESS)
				{
					AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
				}
				else
				{
					SetModifiedFlag (false);
					m_strPathName = m_strPackageMipName;
					// We've saved to another WAD... break the connection!
					if (m_pPackageDoc)
					{
						m_pPackageDoc->BreakDocConnection(this);
						m_pPackageDoc = NULL;		
					}
				}
			}
			else
			{
				AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			}
			return;
		}
		else
		{
			// It's not there, start from scratch!

			switch (GetGameType())
			{
			case FILE_TYPE_QUAKE1:
				ihHelper.SetWADType(WAD2_TYPE);
				break;

			case FILE_TYPE_HALF_LIFE:
				ihHelper.SetWADType(WAD3_TYPE);
				break;

			default:
				ASSERT (false);
				break;
			}

			ihHelper.AddImageToWAD (pbyBits, GetPalette(), m_strPackageMipName, Width(), Height());
			ihHelper.SaveWAD (m_strPackageFile);
			if (ihHelper.GetErrorCode() != IH_SUCCESS)
			{
				AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			}
			else
			{
				SetModifiedFlag (false);
				m_strPathName = m_strPackageMipName;

				// We've saved to another WAD... break the connection!
				if (m_pPackageDoc)
				{
					m_pPackageDoc->BreakDocConnection(this);
					m_pPackageDoc = NULL;		
				}
			}
			return;
		}		
	}		
}

void CWallyDoc::SerializeQuake1 (CArchive& ar)
{
	CFile* p_File = ar.GetFile();
	int iHeaderSize = Q1_HEADER_SIZE;
	Q1_MIP_S q1TempHeader;

	int iSizes[4];
	int j = 0;
	BYTE *pbyData[4];
	int iWidth = Width();
	int iHeight = Height();

	for (j = 0; j < 4; j++)
	{
		pbyData[j] = GetBits(j);
	}
	
	if (ar.IsStoring())
	{	
		// Take care of the various headers
		SetDimensions (iWidth, iHeight);

		if (! m_Undo.IsAnyUndoInProgress()) 
		{
			if ((!IsValidImageSize ( iWidth, iHeight, FORCE_X_16)) && (!g_bAllowNon16))
			{
				AfxMessageBox ("Quake2 texture dimensions must be in multiples of 16", MB_ICONSTOP);
				return;
			}
			
			if (g_bRebuildSubMipsOnSave)
			{		
				BeginWaitCursor();
				//RebuildSubMips();
				UpdateAllDIBs( TRUE);
				EndWaitCursor();
			}
		}

		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
		}				

		ar.Write( &Q1Header, iHeaderSize);

		for (j = 0; j < 4; j++)
		{
			ar.Write( pbyData[j], iSizes[j]);
		}		
	}
	else	// loading
	{		
		int iFileSize = p_File->GetLength();		
		BYTE *pbySourceData = NULL;
		
		if (iFileSize < iHeaderSize)
		{
			AfxMessageBox (".mip header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();
			return;
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			ar.Read (&q1TempHeader, iHeaderSize);
		}
		else
		{
			pbySourceData = new BYTE[iFileSize];

			if (!pbySourceData)
			{
				AfxMessageBox ("Out of memory trying to load image.  Creating blank texture.", MB_ICONSTOP);
				g_iDocWidth  = g_iDefaultTextureWidth;
				g_iDocHeight = g_iDefaultTextureHeight;
				OnNewDocument();
				return;
			}

			ar.Read (pbySourceData, iFileSize);
			
			memcpy (&q1TempHeader, pbySourceData, iHeaderSize);			
		}
		
		m_strPackageMipName = q1TempHeader.name;

		iWidth  = q1TempHeader.width;
		iHeight = q1TempHeader.height;

		if ((iWidth <= 0) || (iHeight <= 0) || (iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
		{
			AfxMessageBox (".mip header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();

			if (pbySourceData)
			{
				delete [] pbySourceData;
				pbySourceData = NULL;
			}
			return;
		}

		SetOriginalBitDepth (8);

		// Bring in the flags, contents, etc.
		memcpy (&Q1Header, &q1TempHeader, iHeaderSize);

		// Set the headers the way we want them
		SetDimensions(iWidth, iHeight);

		int iTotalSize = iHeaderSize;
		
		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
			iTotalSize += iSizes[j];
		}

		if (iTotalSize > iFileSize)
		{			
			// neal try to recover as much data as possible
			AfxMessageBox ("Texture file is malformed.  Salvaging texture data.",MB_ICONSTOP);			
		}
		
		RebuildLayerAndMips( false);

		for (j = 0; j < 4; j++)
		{
			pbyData[j] = GetBits(j);
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			int iOffset = 0;
		
			for (j = 0; j < 4; j++)
			{		
				iOffset = q1TempHeader.offsets[j];
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					ar.Read (pbyData[j], iSizes[j]);
				}
				else
				{
					memset (pbyData[j], 255, iSizes[j]);
				}
			}		
		}
		else
		{
			// Ty- offsets could be wrong; check to see if there is enough data there
			// to load in

			int iOffset = 0;
			
			for (j = 0; j < 4; j++)
			{		
				iOffset = q1TempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					memcpy (pbyData[j], pbySourceData + iOffset, iSizes[j]);
				}
				else
				{
//					memset (pbyData[j], g_iColorIndexRight, iSizes[j]);
					memset (pbyData[j], GetIValue( g_irgbColorRight), iSizes[j]);

					// Copy as much as we can from the file, if any
					if (iFileSize > iOffset)
					{
						memcpy (pbyData[j], pbySourceData + iOffset, iFileSize - iOffset);
					}				
				}
			}			
		}

		if (pbySourceData)
		{
			delete [] pbySourceData;
			pbySourceData = NULL;
		}
	}
}

void CWallyDoc::SerializeQuake2 (CArchive& ar)
{	
	CFile* p_File = ar.GetFile();
	int iHeaderSize = Q2_HEADER_SIZE;
	Q2_MIP_S q2TempHeader;

	int iSizes[4];
	int j = 0;
	BYTE *pbyData[4];
	int iWidth	= Width();
	int iHeight = Height();

	for (j = 0; j < 4; j++)
	{
		pbyData[j] = GetBits(j);
	}
	
	if (ar.IsStoring())
	{
		// Take care of the various headers
		SetDimensions (iWidth, iHeight);

		if (! m_Undo.IsAnyUndoInProgress()) 
		{
			if ((!IsValidImageSize ( iWidth, iHeight, FORCE_X_16)) && (!g_bAllowNon16))
			{
				AfxMessageBox ("Quake2 texture dimensions must be in multiples of 16", MB_ICONSTOP);
				return;
			}
			
			if (g_bRebuildSubMipsOnSave)
			{		
				BeginWaitCursor();
				//RebuildSubMips();
				UpdateAllDIBs( TRUE);
				EndWaitCursor();
			}
		}

		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
		}
	
		ar.Write( &Q2Header, Q2_HEADER_SIZE);

		for (j = 0; j < 4; j++)
		{
			ar.Write( pbyData[j], iSizes[j]);
		}		
	}
	else
	{
		int iFileSize = p_File->GetLength();
		BYTE *pbySourceData = NULL;
		
		if (iFileSize < iHeaderSize)
		{		
			AfxMessageBox (".wal header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();
			return;
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			ar.Read (&q2TempHeader, iHeaderSize);
		}
		else
		{
			pbySourceData = new BYTE[iFileSize];

			if (!pbySourceData)
			{
				AfxMessageBox ("Out of memory trying to load image.  Creating blank texture.", MB_ICONSTOP);
				g_iDocWidth  = g_iDefaultTextureWidth;
				g_iDocHeight = g_iDefaultTextureHeight;
				OnNewDocument();
				return;
			}

			ar.Read (pbySourceData, iFileSize);
			memcpy (&q2TempHeader, pbySourceData, iHeaderSize);
		}

		iWidth  = q2TempHeader.width;
		iHeight = q2TempHeader.height;

		SetOriginalBitDepth (8);

		if ((iWidth <= 0) || (iHeight <= 0) || (iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
		{
			AfxMessageBox (".wal header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();

			if (pbySourceData)
			{
				delete [] pbySourceData;
				pbySourceData = NULL;
			}
			return;
		}

		// Bring in the flags, contents, etc.
		memcpy (&Q2Header, &q2TempHeader, iHeaderSize);

		// Set the headers the way we want them
		SetDimensions(iWidth, iHeight);
		
		int iTotalSize = iHeaderSize;
		
		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
			iTotalSize += iSizes[j];
		}

		if (iTotalSize > iFileSize)
		{			
			// neal try to recover as much data as possible
			AfxMessageBox ("Texture file is malformed.  Salvaging texture data.",MB_ICONSTOP);			
		}

		RebuildLayerAndMips( false);

		for (j = 0; j < 4; j++)
		{
			pbyData[j] = GetBits(j);
		}

//... Neal - TODO

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			int iOffset = 0;
		
			for (j = 0; j < 4; j++)
			{		
				iOffset = q2TempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					ar.Read (pbyData[j], iSizes[j]);
				}
				else
				{
					memset (pbyData[j], 255, iSizes[j]);
				}
			}		
		}
		else
		{
			// Ty- offsets could be wrong; check to see if there is enough data there
			// to load in

			int iOffset = 0;
			
			for (j = 0; j < 4; j++)
			{		
				iOffset = q2TempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					memcpy (pbyData[j], pbySourceData + iOffset, iSizes[j]);
				}
				else
				{
//					memset (pbyData[j], g_iColorIndexRight, iSizes[j]);
					memset (pbyData[j], GetIValue( g_irgbColorRight), iSizes[j]);

					// Copy as much as we can from the file, if any
					if (iFileSize > iOffset)
					{
						memcpy (pbyData[j], pbySourceData + iOffset, iFileSize - iOffset);
					}				
				}
			}			
		}
		
		if (pbySourceData)
		{
			delete [] pbySourceData;
			pbySourceData = NULL;
		}
	}
}

void CWallyDoc::SerializeSin(CArchive& ar)
{
	CFile* p_File = ar.GetFile();
	int iHeaderSize = SIN_HEADER_SIZE;
	SIN_MIP_S sinTempHeader;
	
	int iSizes[4];
	int j = 0;
	BYTE *pbyData[4];
	int iWidth = Width();
	int iHeight = Height();

	for (j = 0; j < 4; j++)
	{
		pbyData[j] = GetBits(j);
	}

	BYTE byPalette[768];
	
	if (ar.IsStoring())
	{	
		// Take care of the various headers
		SetDimensions (iWidth, iHeight);
	
		if (! m_Undo.IsAnyUndoInProgress()) 
		{
			if ((!IsValidImageSize ( iWidth, iHeight, FORCE_X_16)) && (!g_bAllowNon16))
			{
				AfxMessageBox ("SiN texture dimensions must be in multiples of 16", MB_ICONSTOP);
				return;
			}
			
			if (g_bRebuildSubMipsOnSave)
			{		
				BeginWaitCursor();
				//RebuildSubMips();
				UpdateAllDIBs( TRUE);
				EndWaitCursor();
			}
		}

		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
		}
			
		GetPalette (byPalette, 256);		
		for (j = 0; j < 256; j++)
		{
			//memcpy ((SinHeader.palette + (j * 4)), (byPalette + (j * 3)), 3);
			SinHeader.palette[ (j * 4) + 0 ] = byPalette[ (j * 3) + 0 ];
			SinHeader.palette[ (j * 4) + 1 ] = byPalette[ (j * 3) + 1 ];
			SinHeader.palette[ (j * 4) + 2 ] = byPalette[ (j * 3) + 2 ];
		}	

		ar.Write( &SinHeader, iHeaderSize);

		for (j = 0; j < 4; j++)
		{
			ar.Write( pbyData[j], iSizes[j]);
		}		
	}
	else
	{		
		int iFileSize = p_File->GetLength();		
		BYTE *pbySourceData = NULL;
		
		if (iFileSize < iHeaderSize)
		{		
			AfxMessageBox (".swl header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();
			return;
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			ar.Read (&sinTempHeader, iHeaderSize);
		}
		else
		{
			pbySourceData = new BYTE[iFileSize];

			if (!pbySourceData)
			{
				AfxMessageBox ("Out of memory trying to load image.  Creating blank texture.", MB_ICONSTOP);
				g_iDocWidth  = g_iDefaultTextureWidth;
				g_iDocHeight = g_iDefaultTextureHeight;
				OnNewDocument();
				return;
			}

			ar.Read (pbySourceData, iFileSize);
			memcpy (&sinTempHeader, pbySourceData, iHeaderSize);
		}

		iWidth  = sinTempHeader.width;
		iHeight = sinTempHeader.height;

		SetOriginalBitDepth (8);

		if ((iWidth <= 0) || (iHeight <= 0) || (iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
		{
			AfxMessageBox (".swl header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();

			if (pbySourceData)
			{
				delete [] pbySourceData;
				pbySourceData = NULL;
			}
			return;
		}

		// Bring in the flags, contents, etc.
		memcpy (&SinHeader, &sinTempHeader, iHeaderSize);

		// Set the headers the way we want them
		SetDimensions(iWidth, iHeight);
		
		int iTotalSize = iHeaderSize;
		
		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
			iTotalSize += iSizes[j];
		}

		if (iTotalSize > iFileSize)
		{			
			// neal try to recover as much data as possible
			AfxMessageBox ("Texture file is malformed.  Salvaging texture data.",MB_ICONSTOP);			
		}

		RebuildLayerAndMips( false);

		for (j = 0; j < 4; j++)
		{
			pbyData[j] = GetBits(j);
		}
		
		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			int iOffset = 0;
		
			for (j = 0; j < 4; j++)
			{		
				iOffset = sinTempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					ar.Read (pbyData[j], iSizes[j]);
				}
				else
				{
					memset (pbyData[j], 255, iSizes[j]);
				}
			}		
		}
		else
		{
			// Ty- offsets could be wrong; check to see if there is enough data there
			// to load in

			int iOffset = 0;
			
			for (j = 0; j < 4; j++)
			{		
				iOffset = sinTempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					memcpy (pbyData[j], pbySourceData + iOffset, iSizes[j]);
				}
				else
				{
//					memset (pbyData[j], g_iColorIndexRight, iSizes[j]);
					memset (pbyData[j], GetIValue( g_irgbColorRight), iSizes[j]);
					
					// Copy as much as we can from the file, if any
					if (iFileSize > iOffset)
					{
						memcpy (pbyData[j], pbySourceData + iOffset, iFileSize - iOffset);
					}				
				}
			}			
		}
		
		for (j = 0; j < 256; j++)
		{
			//memcpy (byPalette + (j * 3), sinTempHeader.palette + (j * 4), 3);
			byPalette[ (j * 3) + 0 ] = sinTempHeader.palette[ (j * 4) + 0 ];
			byPalette[ (j * 3) + 1 ] = sinTempHeader.palette[ (j * 4) + 1 ];
			byPalette[ (j * 3) + 2 ] = sinTempHeader.palette[ (j * 4) + 2 ];
		}
		SetPalette(byPalette, 256, FALSE);
		
		if (pbySourceData)
		{
			delete [] pbySourceData;
			pbySourceData = NULL;
		}
	}
}


void CWallyDoc::SerializeHeretic2(CArchive& ar)
{
	CFile* pFile = ar.GetFile();
	int iHeaderSize = M8_HEADER_SIZE;
	M8_MIP_S m8TempHeader;

	int iSizes[M8_MIPLEVELS];
	int j = 0;

	BYTE *pbyTempData = NULL;
	BYTE *pbyData[M8_MIPLEVELS];
	int iWidth	= Width();
	int iHeight = Height();
	
	int iFileSize = pFile->GetLength();

//	for (j = 0; j < 4; j++)
//	{
//		pbyData[j] = GetBits(j);
//	}

	BYTE byPalette[768];
	GetPalette (byPalette, 256);
	
	if (ar.IsStoring())
	{
		// Take care of the various headers
		SetDimensions (iWidth, iHeight);

		if (! m_Undo.IsAnyUndoInProgress()) 
		{
			if ((!IsValidImageSize ( iWidth, iHeight, FORCE_X_16)) && (!g_bAllowNon16))
			{
				AfxMessageBox ("Heretic2 texture dimensions must be in multiples of 16", MB_ICONSTOP);
				return;
			}
			
			if (g_bRebuildSubMipsOnSave)
			{		
				BeginWaitCursor();
				//RebuildSubMips();
				UpdateAllDIBs( TRUE);
				EndWaitCursor();
			}
		}

		for (j = 0; j < 4; j++)
		{
			iSizes[j] = GetSize(j);
		}

		BYTE* pbyPackedData = NULL;
		BYTE* pbyBits[M8_MIPLEVELS];
		LPM8_MIPTEX_S pM8Header = NULL;
		CWallyPalette Palette;

		int iWidth = GetWidth();
		int iHeight = GetHeight();	

		int iWidths[M8_MIPLEVELS];
		int iHeights[M8_MIPLEVELS];
		int iSizes[M8_MIPLEVELS];
		int iOffsets[M8_MIPLEVELS];
			
		int iTotalSize = iHeaderSize;
		int j = 0;
		
		for (j = 0; j < M8_MIPLEVELS; j++)
		{
			iWidths[j] = (int)(iWidth / pow (2, j));
			iHeights[j] = (int)(iHeight / pow (2, j));
			iSizes[j] = iWidths[j] * iHeights[j];
			
			M8Header.width[j] = iWidths[j];
			M8Header.height[j] = iHeights[j];

			iOffsets[j] = M8Header.offsets[j] = (iSizes[j] != 0) ? iTotalSize : 0;
			iTotalSize += iSizes[j];
		}
		
		pbyPackedData = new BYTE[iTotalSize];
		
		if (!pbyPackedData)
		{
			AfxMessageBox ("Out of memory during SerializeHeretic2()\nFile was not saved.", MB_ICONSTOP);
			return;
		}
		memset (pbyPackedData, 0, iTotalSize);
		
		pM8Header = (LPM8_MIPTEX_S)pbyPackedData;
		memcpy (pM8Header, &M8Header, iHeaderSize);

		for (j = 0; j < M8_MIPLEVELS; j++)
		{
			pbyBits[j] = (iOffsets[j] != 0) ? pbyPackedData + iOffsets[j] : NULL;

			// Only 4 mips contained within WallyDoc right now...
			if ((pbyBits[j]) && (j < 4))
			{
				memcpy (pbyBits[j], GetBits(j), iSizes[j]);
			}
		}
				
		memcpy (pM8Header->palette, byPalette, 768);

		if (! m_Undo.IsAnyUndoInProgress())
		{
			// All the sub-mips need to be built.
			// 512 is to reserve room for 512 byte header (max)
			//#define MAX_DATA_SIZE (MAX_MIP_SIZE * MAX_MIP_SIZE + 512)
			
			int iMipDataSize = iTotalSize * 3 + 512;
			BYTE	*pbyMipData		= NULL;
			pbyMipData = new BYTE[iMipDataSize];
			
			if (!pbyMipData)
			{
				AfxMessageBox ("Out of memory during SerializeHeretic2()\nFile was not saved.", MB_ICONSTOP);			
				if (pbyPackedData)
				{
					delete [] pbyPackedData;
					pbyPackedData = NULL;
				}
				return;
			}	
			
			memset (pbyMipData, 0, iMipDataSize);
			
			LPREMIP_HEADER pMipHeader = (LPREMIP_HEADER)pbyMipData;
			int iPosition = sizeof (REMIP_HEADER);
			
			for (j = 0; j < M8_MIPLEVELS; j++)
			{
				pMipHeader->iWidths[j] = iWidths[j];
				pMipHeader->iHeights[j] = iHeights[j];
				pMipHeader->iOffsets[j] = iPosition;

				iPosition += iSizes[j];
			}
					
			memcpy( pbyMipData + pMipHeader->iOffsets[0], pbyBits[0], iSizes[0]);
			
			///////////////////////////////
			// Build all the sub-mips //
			/////////////////////////////

			Palette.SetNumFullBrights( 0);

			// Start at 4; the others have already been accounted for... it's just those beyond 
			// the bottom mip that we care about
			::RebuildMips (&Palette, pbyMipData, M8_MIPLEVELS, 4);
				
			/////////////////////////
			// copy the data back //
			///////////////////////
			
			for (j = 4; j < M8_MIPLEVELS; j++)
			{
				if (iSizes[j] != 0)
				{
					memcpy( pbyBits[j], pbyMipData + pMipHeader->iOffsets[j], iSizes[j]);				
				}
			}	

			pMipHeader = NULL;
			if (pbyMipData)
			{
				delete [] pbyMipData;
				pbyMipData = NULL;
			}
		}

		ar.Write (pbyPackedData, iTotalSize);
		
		if (pbyPackedData)
		{
			delete [] pbyPackedData;
			pbyPackedData = NULL;
		}		
	}
	else
	{
		BYTE *pbyTempData = NULL;
		
		if (iFileSize <= M8_HEADER_SIZE)
		{
			AfxMessageBox (".m8 file is malformed.  Creating blank texture.", MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();			
			return;
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{
			ar.Read (&m8TempHeader, iHeaderSize);
		}
		else
		{
			pbyTempData = new BYTE[iFileSize];
			
			if (!pbyTempData)
			{
				AfxMessageBox ("Out of memory trying to load image.  Creating blank texture.", MB_ICONSTOP);
				g_iDocWidth  = g_iDefaultTextureWidth;
				g_iDocHeight = g_iDefaultTextureHeight;
				OnNewDocument();
				return;
			}
			
			ar.Read (pbyTempData, iFileSize);			
			memcpy (&m8TempHeader, pbyTempData, M8_HEADER_SIZE);
		}

		iWidth = m8TempHeader.width[0];
		iHeight = m8TempHeader.height[0];

		SetOriginalBitDepth (8);

		if ((iWidth <= 0) || (iHeight <= 0) || (iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
		{
			AfxMessageBox (".m8 header is malformed.  Creating blank texture.",MB_ICONSTOP);
			g_iDocWidth  = g_iDefaultTextureWidth;
			g_iDocHeight = g_iDefaultTextureHeight;
			OnNewDocument();

			if (pbyTempData)
			{
				delete [] pbyTempData;
				pbyTempData = NULL;
			}
			return;
		}
		
		// Bring in the flags, contents, etc.
		memcpy (&M8Header, &m8TempHeader, iHeaderSize);

		// Set the headers the way we want them
		SetDimensions(iWidth, iHeight);
	
		int iTotalSize = M8_HEADER_SIZE;
	
		for (j = 0; j < M8_MIPLEVELS; j++)
		{
			iSizes[j] = m8TempHeader.width[j] * m8TempHeader.height[j];
			iTotalSize += iSizes[j];
		}	

		if (iTotalSize > iFileSize)
		{
			AfxMessageBox (".m8 file is malformed.  Salvaging texture data.", MB_ICONSTOP);			
		}
		
		SetPalette (m8TempHeader.palette, 256, FALSE);

		int iOffset = 0;

		// TODO:  Support more than 4 submips for these textures?

		RebuildLayerAndMips( false);

		// Neal - fix access violation
		//
		for (j = 0; j < 4; j++)
		{
			pbyData[j] = GetBits(j);
		}

		if ( m_Undo.IsAnyUndoInProgress()) 
		{		
			for (j = 0; j < 4; j++)
			{		
				iOffset = m8TempHeader.offsets[j];		
				
				if ((iOffset + iSizes[j]) <= iFileSize)
				{				
					ar.Read (pbyData[j], iSizes[j]);
				}
				else
				{
					memset (pbyData[j], 255, iSizes[j]);
				}
			}

			// Just read the extra submips from the archive so our undo data is preserved
			BYTE *pbyGarbageData = new BYTE[iSizes[0]];

			for (j = 4; j < M8_MIPLEVELS; j++)
			{
				if (iSizes[j] != 0)
				{
					ar.Read (pbyGarbageData, iSizes[j]);
				}
			}

			if (pbyGarbageData)
			{
				delete [] pbyGarbageData;
				pbyGarbageData = NULL;
			}
		}
		else
		{
			// Ty- offsets could be wrong; check to see if there is enough data there
			// to load in
			
			for (j = 0; j < 4; j++)
			{
				iOffset = m8TempHeader.offsets[j];		
				
				// Neal - fix access violation
				//
				if (pbyData[j] != NULL)
				{
					if ((iOffset + iSizes[j]) <= iFileSize)
					{
						memcpy (pbyData[j], pbyTempData + iOffset, iSizes[j]);
					}
					else
					{
//						memset (pbyData[j], g_iColorIndexRight, iSizes[j]);
						memset (pbyData[j], GetIValue( g_irgbColorRight), iSizes[j]);
						
						// Copy as much as we can from the file, if any
						if (iFileSize > iOffset)
						{
							memcpy (pbyData[j], pbyTempData + iOffset, iFileSize - iOffset);
						}
					}
				}
			}
		}

		if (pbyTempData)
		{
			delete [] pbyTempData;
			pbyTempData = NULL;
		}
	}
}


void CWallyDoc::ClearMipArray( COLOR_IRGB irgbColor, int MipNumber)
{
	ASSERT( MipNumber == 0);
	if (MipNumber != 0)
		return;

	int i, j;
	//int iIndex;
	int iDocWidth  = Width();
	int iDocHeight = Height();

	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->m_pMainWnd;

	// Get the active MDI child window.

	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();

	// Get the active view attached to the active MDI child window.

	CView*      pView      = pChild->GetActiveView();
	CWallyView* pWallyView = DYNAMIC_DOWNCAST( CWallyView, pView);
	BOOL        bTile      = FALSE;
	
	if (pWallyView)
		bTile = pWallyView->m_bTile;

	if (HasSelection())
	{
		CSelection* pSel = GetSelection();
		ASSERT( pSel);

		CRect Rect = pSel->GetBoundsRect();

		for (j = Rect.top; j < Rect.bottom; j++)
		{
			for (i = Rect.left; i < Rect.right; i++)
			{
				if (bTile)
				{
					SetWrappedPixel( pWallyView, i, j, irgbColor);
				}
				else
				{
					if ((i >= 0) && (j >= 0) && (i < iDocWidth) && (j < iDocHeight))
					{
						SetPixel( pWallyView, i, j, irgbColor);
					}
				}
			}
		}
		SetModifiedFlag( TRUE);
	}
	else
	{
		ClearLayer( irgbColor);
	}
}

void CWallyDoc::RebuildMipArray()
{
	// Neal - TODO: we should not be doing anything to the mips
	// (leave ptrs NULL), when GetNumMips() == 1

	for (int j = 0; j < 4; j++)
	{
		if (MipArray[j].mip_pointer != NULL)
			delete [] MipArray[j].mip_pointer;

		int iSize = GetWidth(j) * GetHeight(j);
		MipArray[j].mip_pointer = new BYTE[iSize];
	}
	UpdateAllViews( NULL, HINT_UPDATE_DIBS, NULL);	// Neal - new size - update pView->m_DIB
}

void CWallyDoc::ClearMipArray( COLOR_IRGB irgbColor)
{		
	int iColor = GetIValue( irgbColor);

	for (int j = 0; j < 4; j++)
	{
		memset( MipArray[j].mip_pointer, iColor, GetSize(j));
	}
}

void CWallyDoc::RebuildLayerAndMips( bool bFillWithBackground)
{		
	int iNumBits = m_Base.m_Layer.GetNumBits();
	m_Base.m_Layer.FreeMem();

	int iWidth  = GetWidth();
	int iHeight = GetHeight();

	if ((iWidth > 0) && (iHeight > 0))
	{
		m_Base.m_Layer.Create( this, iWidth, iHeight, iNumBits);
	}

	RebuildMipArray();

	if (bFillWithBackground)
	{
		// neal - pre-fill with right button (background) color

		ClearMipArray( g_irgbColorRight);

		int iWidth  = Width();
		int iHeight = Height();

		for (int j = 0; j < iHeight; j++)
		{
			for (int i = 0; i < iWidth; i++)
			{
				SetPixel( NULL, i, j, g_irgbColorRight);
			}
		}
	}
}

void CWallyDoc::SetPalette( BYTE* pPackedPal, int iNumColors, BOOL bUpdate)
{
	if (pPackedPal)
	{		
		//int iNum = m_Palette.GetNumColors();
		m_Palette.SetPalette( pPackedPal, iNumColors);

		//if ((iNum > 0) && (bUpdate))
		if (bUpdate)
		{
			UpdateAllViews( NULL, HINT_UPDATE_PAL);
		}
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CWallyDoc::SetPalette( CWallyPalette* pPal, BOOL bUpdate)
{
	if (pPal)
	{
		m_Palette = *( pPal);
		if (bUpdate)
			UpdateAllViews( NULL, HINT_UPDATE_PAL);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWallyDoc diagnostics

#ifdef _DEBUG
void CWallyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWallyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWallyDoc commands


void CWallyDoc::OnTpFlags() 
{		
	CTextureInfo TexInfoDlg;
	CRenameImageDlg dlgRename;

	switch (GetGameType())
	{
	case FILE_TYPE_HERETIC2:
		Q2_MIP_S TempHeader;
		memset (&TempHeader, 0, Q2_HEADER_SIZE);
		TempHeader.flags = M8Header.flags;
		TempHeader.contents = M8Header.contents;

		TempHeader.width = M8Header.width[0];
		TempHeader.height = M8Header.height[0];
		
		memcpy (TempHeader.name, M8Header.name, min (strlen (M8Header.name), 31));
		memcpy (TempHeader.animname, M8Header.animname, min (strlen (M8Header.animname), 31));
		TempHeader.value = M8Header.value;

		TexInfoDlg.Init (&TempHeader);
		if (TexInfoDlg.DoModal() == IDOK)
		{
			if (TexInfoDlg.ModifiedFlag)
			{
				SetModifiedFlag( TRUE);
			}
			M8Header.flags = TempHeader.flags;
			M8Header.contents = TempHeader.contents;
			M8Header.value = TempHeader.value;

			strcpy_s (M8Header.name, sizeof(M8Header.name),TempHeader.name);
			strcpy_s (M8Header.animname, sizeof(M8Header.animname), TempHeader.animname);
		}
		break;


	case FILE_TYPE_QUAKE2:
		TexInfoDlg.Init(&Q2Header);
		if (TexInfoDlg.DoModal() == IDOK)
		{
			if (TexInfoDlg.ModifiedFlag)
			{
				SetModifiedFlag( TRUE);
			}			
		}
		break;

	case FILE_TYPE_QUAKE1:
	case FILE_TYPE_HALF_LIFE:
		if (m_pPackageDoc)
		{
			m_pPackageDoc->RenameImage(this);
			m_strPathName = m_strPackageMipName;
		}		
		break;

	case FILE_TYPE_SIN:
		{
			CSinFlagsDlg dlgSin;
			dlgSin.SetSinHeader (&SinHeader);

// Neal - TODO - this may be a problem for 24-bit
			dlgSin.SetDocData( MipArray[0].mip_pointer, GetPalette());

			if (dlgSin.DoModal() == IDOK)
			{
				LPSIN_MIP_S lpSinHeader = dlgSin.GetSinHeader();

				BYTE *pbyCompare = (BYTE *)lpSinHeader;
				BYTE *pbyCurrent = (BYTE *)(&SinHeader);

				for (int j = 0; j < SIN_HEADER_SIZE; j++)
				{
					if (pbyCompare[j] != pbyCurrent[j])
					{
						SetModifiedFlag( TRUE);
						j = SIN_HEADER_SIZE;
					}
				}
				memcpy (pbyCurrent, pbyCompare, SIN_HEADER_SIZE);
			}
		}
		break;

	default:
		ASSERT (false);		// Unhandled game type?
		break;
	}	

}

void CWallyDoc::OnUpdateTpFlags(CCmdUI* pCmdUI) 
{
	switch (GetGameType())
	{
	case FILE_TYPE_QUAKE2:
		pCmdUI->Enable (TRUE);
		break;
		
	case FILE_TYPE_QUAKE1:
		pCmdUI->Enable (FALSE);
		break;

	case FILE_TYPE_HERETIC2:
		pCmdUI->Enable (TRUE);
		break;

	case FILE_TYPE_SIN:
		pCmdUI->Enable (TRUE);
		break;

	case FILE_TYPE_HALF_LIFE:
		pCmdUI->Enable (m_pPackageDoc != NULL);
		break;

	case FILE_TYPE_TGA:
	case FILE_TYPE_PCX:
	case FILE_TYPE_BMP:
	case FILE_TYPE_PNG:
	case FILE_TYPE_JPG:
	case FILE_TYPE_TEX:
		pCmdUI->Enable (FALSE);
		break;

	default:
		ASSERT (false);
		break;
	}	
}

void CWallyDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (IsModified());
}

bool CWallyDoc::LoadFromImageHelper (CImageHelper *pImageHelper)
{
	// TODO:  this still touches the MipArray!  Move it to CLayer
	ASSERT (pImageHelper);

	BYTE* pbyImportData	= pImageHelper->GetBits();
	BYTE* pbyPalette    = pImageHelper->GetPalette();
	BYTE* pbyDocBits    = GetBits();
	
	int iWidth       = pImageHelper->GetImageWidth();
	int iHeight      = pImageHelper->GetImageHeight();		
	int iSize        = iWidth * iHeight;
	int iColorDepth  = pImageHelper->GetColorDepth();	
	int iGameType	 = GetGameType();
	int r, g, b, i, j;
	
	CWallyPalette *pPal = GetPalette();	
	CString strFileName = pImageHelper->GetFileName();
	CString strName = GetRawFileName(strFileName);
			
	switch (iColorDepth)
	{
	case IH_8BIT:
		m_Base.GetLayer()->SetNumBits( 8);

		switch (iGameType)
		{		
		case FILE_TYPE_QUAKE1:
		case FILE_TYPE_QUAKE2:
			{		
				pPal->SetNumColors(256);
				//pPal->SetPalette (iGameType == FILE_TYPE_QUAKE2 ? quake2_pal : quake1_pal, 256);

				if ((iGameType == FILE_TYPE_QUAKE2))
					pPal->SetPalette( quake2_pal, 256);
				else
					pPal->SetPalette( quake1_pal, 256);

				switch (g_iPaletteConversion)
				{
				case PALETTE_CONVERT_MAINTAIN:
		
					// Just jam it in, no conversion necessary
					memcpy (pbyDocBits, pbyImportData, iSize);
					break;
					
				case PALETTE_CONVERT_NEAREST:
					{
						COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
					
						for (j = 0; j < iSize; j++) 
						{
							r = pbyPalette[pbyImportData[j] * 3];
							g = pbyPalette[pbyImportData[j] * 3 + 1];
							b = pbyPalette[pbyImportData[j] * 3 + 2];
					
							//pTemp24Bit[j].byRed   = r;
							//pTemp24Bit[j].byGreen = g;
							//pTemp24Bit[j].byBlue  = b;
							
							//pTemp24Bit[j].byFullBright = ((r == 159) && (g == 91) && (b == 83));

							if (((r == 159) || (g == 91) || (b == 83)) && (iGameType == FILE_TYPE_QUAKE2))
								pTemp24Bit[j] = IRGB( 255, r, g, b);
							else
								pTemp24Bit[j] = IRGB( 0, r, g, b);
								
						}		
						Convert24BitTo256Color( pTemp24Bit, pbyDocBits, 
									iWidth, iHeight, 0, GetDitherType(), FALSE);
						if (pTemp24Bit)
						{
							free (pTemp24Bit);
						}
					}
					break;

				default:
					ASSERT (false);		// Unsupported palette conversion
					break;
				}		// switch (g_iPaletteConversion)
			}			// case FILE_TYPE_QUAKE2:
			break;

		case FILE_TYPE_SIN:			
			{
				// Calculate the "overall" color of the image
				CalcImageColor256 (iWidth, iHeight, pbyImportData, pbyPalette, &(SinHeader.color[0]), &(SinHeader.color[1]), &(SinHeader.color[2]), TRUE);
			}
		
		case FILE_TYPE_HERETIC2:
		case FILE_TYPE_HALF_LIFE:
			{
				// Don't care about the palette conversion option for HL texs.				
				pPal->SetPalette (pbyPalette, 256);
				memcpy (pbyDocBits, pbyImportData, iSize);
			}
			break;

		case FILE_TYPE_TGA:
		case FILE_TYPE_PCX:
		case FILE_TYPE_BMP:
		case FILE_TYPE_PNG:
		case FILE_TYPE_JPG:
		case FILE_TYPE_TEX:
			{
				pPal->SetPalette (pbyPalette, 256);
				memcpy (pbyDocBits, pbyImportData, iSize);
			}
			break;

		default:
			ASSERT (false);		// Unhandled game type?
			return false;
			break;
		}					// switch (iGameType)

		break;				// case IH_8BIT

	
	case IH_24BIT:
		{
			m_Base.GetLayer()->SetNumBits( 24);

			COLOR_IRGB* pTemp24Bit = NULL;
				
			pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
			BYTE byPalette[256 * 3];
			
			char cFlag = strName.GetAt(0);
			int iNumColors = (cFlag == '{' ? 255 : 256);
			CColorOptimizer ColorOpt;

			for (j = 0; j < (iWidth * iHeight); j++)
			{				
				r = pbyImportData[j * 3];
				g = pbyImportData[j * 3 + 1];
				b = pbyImportData[j * 3 + 2];

//				ASSERT( (r==0) && (g==0) && (b==0));	// TEST TEST TEST

				//pTemp24Bit[j].byRed   = r;
				//pTemp24Bit[j].byGreen = g;
				//pTemp24Bit[j].byBlue  = b;

				if (iGameType == FILE_TYPE_QUAKE2)
				{
					//pTemp24Bit[j].byFullBright = ((r == 159) && (g == 91) && (b == 83));

					if ((r != 159) || (g != 91) || (b != 83))
						pTemp24Bit[j] = IRGB( 0, r, g, b);
					else
						pTemp24Bit[j] = IRGB( 255, r, g, b);
				}
				else
				{
					//pTemp24Bit[j].byFullBright = 0;

					pTemp24Bit[j] = IRGB( 0, r, g, b);
				}
			}		

			switch (iGameType)
			{
			case FILE_TYPE_QUAKE1:
				pPal->SetPalette (quake1_pal, 256);
				break;

			case FILE_TYPE_QUAKE2:
				pPal->SetPalette (quake2_pal, 256);
				break;

			case FILE_TYPE_TGA:
			case FILE_TYPE_PCX:
			case FILE_TYPE_BMP:			
			case FILE_TYPE_HERETIC2:
			case FILE_TYPE_SIN:
			case FILE_TYPE_PNG:
			case FILE_TYPE_JPG:
			case FILE_TYPE_TEX:
				{
					ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, iNumColors, TRUE);
				
					// SetPalette ASSERTs with anything other than 256 colors
					pPal->SetPalette( byPalette, 256);
				}
				break;

			case FILE_TYPE_HALF_LIFE:
				{
					ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, iNumColors, TRUE);
				
					// SetPalette ASSERTs with anything other than 256 colors
					pPal->SetPalette( byPalette, 256);
					
					if (iNumColors == 255)
					{
						// Set index 255 to solid blue... it's our transparent index
						pPal->SetRGB( 255, 0, 0, 255);
					}
				}
				break;

			default:
				ASSERT (false);		// Unhandled game type?
				return false;
				break;
			}
			
			//Convert24BitTo256Color( pTemp24Bit, pbyDocBits, 
			//		iWidth, iHeight, 0, GetDitherType(), FALSE);

			//////////////////////////////////////
			// Neal - copy data over to CLayer //
			////////////////////////////////////

			int iIndex = 0;

			for (j = 0; j < iHeight; j++)
			{
				for (i = 0; i < iWidth; i++)
				{
					COLOR_IRGB rgbColor = pTemp24Bit[iIndex++];

					SetNearestColorPixel( NULL, i, j, rgbColor, FALSE);
				}
			}
						
			if (pTemp24Bit)
			{
				free (pTemp24Bit);
			}
		
		}
		break;				// case IH_24BIT

	case IH_32BIT:
		{
			AfxMessageBox( "This image has an ALPHA channel (it's 32 bits.)\nWally does not yet support alpha channel editing.", MB_ICONINFORMATION);
			ASSERT (false);		// Unsupported version.  Missed implementation?
			return false;
		}
		break;				// case IH_32BIT

	default :
		ASSERT (false);		// Unsupported version.  Missed implementation?
		return false;
		break;
	}

	CalcImageColorSinHeader();
	pbyImportData = NULL;
	pbyPalette = NULL;

	CString TempName ("");
	
	switch (g_iTextureNameSetting)		
	{
	case BLANK_DIR:			
		break;

	case PRESET_DIR:
		
		TempName = TrimSlashes (g_strDefaultTextureName);
		TempName += "/";
		break;
		
	case PARENT_DIR:

		TempName =  GetParentDirectory(strFileName);
		TempName += "/";
		break;

	default:
		ASSERT (false);		// Unhandled Wally Option
		break;

	}		

	TempName += GetRawFileName(strFileName);
	char *szName = NULL;

	switch (iGameType)
	{
	case FILE_TYPE_QUAKE1:
		TempName = TempName.Left (15);
		szName = TempName.GetBuffer(TempName.GetLength());
		memset (Q1Header.name, 0, 16);
		strcpy_s (Q1Header.name, sizeof(Q1Header.name), szName);
		TempName.ReleaseBuffer();
		m_strPackageMipName = TempName;
		break;

	case FILE_TYPE_HERETIC2:
		TempName = TempName.Left (31);
		szName = TempName.GetBuffer(TempName.GetLength());
		memset (M8Header.name, 0, 32);
		strcpy_s (M8Header.name, sizeof(M8Header.name), szName);
		TempName.ReleaseBuffer();
		break;

	case FILE_TYPE_QUAKE2:
		TempName = TempName.Left (31);
		szName = TempName.GetBuffer(TempName.GetLength());
		memset (Q2Header.name, 0, 32);
		strcpy_s (Q2Header.name, sizeof(Q2Header.name), szName);
		TempName.ReleaseBuffer();
		break;

	case FILE_TYPE_SIN:
		TempName = TempName.Left (63);
		szName = TempName.GetBuffer(TempName.GetLength());
		memset (SinHeader.name, 0, 64);
		strcpy_s (SinHeader.name, sizeof(SinHeader.name), szName);
		TempName.ReleaseBuffer();
		break;		

	case FILE_TYPE_HALF_LIFE:
		m_strPackageMipName = strName.Left (15);
		SetTitle (m_strPackageMipName);
		break;

	case FILE_TYPE_TGA:
	case FILE_TYPE_PCX:
	case FILE_TYPE_BMP:
	case FILE_TYPE_PNG:
	case FILE_TYPE_JPG:
	case FILE_TYPE_TEX:
		break;			

	default:
		ASSERT (false);		 // Unhandled game type!
		return false;
		break;
	}

	EndWaitCursor();		
	return true;
}

bool CWallyDoc::LoadImage(CImageHelper *pImageHelper)
{
	return LoadFromImageHelper (pImageHelper);
}

bool CWallyDoc::LoadImage(CString FileName)
{	
	CImageHelper ihLoadImage;	
		
	CString strName ("");
	strName = GetRawFileName (FileName);

	BeginWaitCursor();
	
	ihLoadImage.LoadImage (FileName, IH_LOAD_ONLYIMAGE);

	if (ihLoadImage.GetErrorCode() != IH_SUCCESS)
	{		
		AfxMessageBox (ihLoadImage.GetErrorText(), MB_ICONSTOP);
		return false;
	}

	return LoadFromImageHelper (&ihLoadImage);
}

void CWallyDoc::CalcImageColorSinHeader()
{
	// Calculate the "overall" color of the image
	BYTE byPalette[768];
	GetPalette (byPalette, 256);
	CalcImageColor256 (GetWidth(), GetHeight(), GetBits(), byPalette, &(SinHeader.color[0]), &(SinHeader.color[1]), &(SinHeader.color[2]), TRUE);	
}


void CWallyDoc::OnFileExport()
{
	CImageHelper ihExport;
	
	// Build some strings based on the String Table entries	
	CString strWildCard(ihExport.GetSupportedImageList());
	CString strTitle((LPCTSTR)IDS_FILEEXPORT_TITLE);			
	CString m_ExportFileName(GetRawFileName(GetTitle()));
	CString FileName;
	CString FileExtension;
	CString AddExtension;
	int     iExtensionMarker = 0;
	int     iFileNameLength  = 0;
	
	// Create a CFileDialog, init with our strings
	CFileDialog	dlgExport (FALSE, NULL, m_ExportFileName, 
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		strWildCard, NULL); 		
	
	// Set some of the CFileDialog vars
	if (g_strFileSaveDirectory != "")
	{
		dlgExport.m_ofn.lpstrInitialDir = g_strFileSaveDirectory;
	}
	dlgExport.m_ofn.lpstrTitle = strTitle;	
	dlgExport.m_ofn.nFilterIndex = g_iFileExportExtension + 1;
	
	if (dlgExport.DoModal() == IDOK)
	{
		g_strFileSaveDirectory = dlgExport.GetPathName().Left(dlgExport.m_ofn.nFileOffset);		
		g_iFileExportExtension = dlgExport.m_ofn.nFilterIndex - 1;
		AddExtension = GetWildCardExtension( strWildCard, g_iFileExportExtension);
		FileName     = (dlgExport.GetPathName());

		iExtensionMarker = dlgExport.m_ofn.nFileExtension;
		iFileNameLength  = FileName.GetLength();

		// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
		// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
		// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
		// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
		// We have to check for each instance and add the extension, as required.

		if ((FileName.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
		{
			FileName += ".";
		}

		if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
		{				
			// User didn't type in the extension			
			FileName += AddExtension;
		}		
				
		FileExtension = GetExtension (FileName);		
	
		BeginWaitCursor();

		int     j                 = 0;
		int     iSize             = 0;
		int     i24BitSize        = 0;
		char*   szName            = NULL;
		BYTE*   pbyDocBits        = GetBits();
		BYTE*   p_24BitData       = NULL;
		int     iExportColorDepth = g_iExportColorDepth;		
		int     iFlags            = ihExport.GetTypeFlags( g_iFileExportExtension);
		CString szErrorMessage;		
		
		if ((iFlags & IH_TYPE_FLAG_SUPPORTS8BIT) && ((iFlags & IH_TYPE_FLAG_SUPPORTS24BIT) == 0))
		{
			// Supports 8BIT, but not 24BIT.
			iExportColorDepth = IH_8BIT;
		}

		if (iFlags & IH_TYPE_FLAG_ISGAME)
		{			
			switch (GetGameType())
			{
			case FILE_TYPE_QUAKE1:
				{
					szName = Q1Header.name;
					ihExport.MapGameFlags (IH_MIP_TYPE, (unsigned char *)szName);
					m_strPackageMipName.ReleaseBuffer();
				}
				break;

			case FILE_TYPE_HERETIC2:
				{
					int   iHeaderSize = M8_HEADER_SIZE;
					BYTE* pbyHeader   = new BYTE[iHeaderSize];
					memset( pbyHeader, 0, iHeaderSize);
					LPM8_MIPTEX_S pM8Header = (LPM8_MIPTEX_S)pbyHeader;					
					
					memcpy( pM8Header, &M8Header, iHeaderSize);
					ihExport.MapGameFlags( IH_M8_TYPE, pbyHeader);

					pM8Header = NULL;
					if (pbyHeader)
					{
						delete [] pbyHeader;
						pbyHeader = NULL;
					}
				}
				break;

			case FILE_TYPE_QUAKE2:
				{
					int   iHeaderSize = Q2_HEADER_SIZE;
					BYTE* pbyHeader   = new BYTE[iHeaderSize];
					memset( pbyHeader, 0, iHeaderSize);
					LPQ2_MIP_S pQ2Header = (LPQ2_MIP_S)pbyHeader;
					
					memcpy( pQ2Header, &Q2Header, iHeaderSize);
					ihExport.MapGameFlags( IH_WAL_TYPE, pbyHeader);

					pQ2Header = NULL;
					if (pbyHeader)
					{
						delete [] pbyHeader;
						pbyHeader = NULL;
					}
				}
				break;

			case FILE_TYPE_SIN:
				{
					int   iHeaderSize = sizeof (SIN_MIP_S);
					BYTE* pbyHeader   = new BYTE[iHeaderSize];
					memset( pbyHeader, 0, iHeaderSize);
					LPSIN_MIP_S pSWLHeader = (LPSIN_MIP_S)pbyHeader;
					ihExport.MapGameFlags (IH_SWL_TYPE, pbyHeader);

					pSWLHeader = NULL;
					if (pbyHeader)
					{
						delete [] pbyHeader;
						pbyHeader = NULL;
					}
				}
				break;

			case FILE_TYPE_HALF_LIFE:
				{
					szName = m_strPackageMipName.GetBuffer(m_strPackageMipName.GetLength());
					ihExport.MapGameFlags (IH_WAD3_IMAGE_TYPE, (BYTE *)szName);
					m_strPackageMipName.ReleaseBuffer();
				}
				break;

			case FILE_TYPE_TGA:
			case FILE_TYPE_PCX:
			case FILE_TYPE_BMP:
			case FILE_TYPE_PNG:
			case FILE_TYPE_JPG:
			case FILE_TYPE_TEX:
				break;			

			default:
				ASSERT( FALSE);			// Unhandled game type!
				break;
			}
			
		}

		switch (iExportColorDepth)
		{
		case IH_8BIT:
			{
				BYTE byPackedPal[3 * 256];
				GetPalette( byPackedPal, 256);
				ihExport.SaveImage (iExportColorDepth, FileName, pbyDocBits, byPackedPal, Width(), Height());
			}
			break;

		case IH_24BIT:
			iSize       = Width() * Height();
			i24BitSize  = iSize * 3;
			p_24BitData = new BYTE[i24BitSize];
			memset( p_24BitData, 0, i24BitSize);
	
			for (j = 0; j < iSize; j++)
			{			
				p_24BitData[j * 3]     = m_Palette.GetR( pbyDocBits[j]);
				p_24BitData[j * 3 + 1] = m_Palette.GetG( pbyDocBits[j]);
				p_24BitData[j * 3 + 2] = m_Palette.GetB( pbyDocBits[j]);
			}
			ihExport.SaveImage( iExportColorDepth, FileName, p_24BitData, NULL, Width(), Height());

			if (p_24BitData)
			{
				delete [] p_24BitData;
				//p_24BitData = NULL;
			}
			break;

		default:
			ASSERT( FALSE);		// Missed implementation?
			break;
		}				

		if (ihExport.GetErrorCode() != IH_SUCCESS)
		{
			szErrorMessage = ihExport.GetErrorText();
			AfxMessageBox( szErrorMessage, MB_ICONSTOP);
		}		
		EndWaitCursor();
	}	
}

///////////////////////////////////////////////////////////////////////
// Name:	ClearLayer
// Action:	Clears the layer to irgbColor
///////////////////////////////////////////////////////////////////////
void CWallyDoc::ClearLayer( COLOR_IRGB irgbColor)
{
	CLayer* pLayer = GetCurrentLayer();
	ASSERT( pLayer);

	pLayer->Clear( NULL, irgbColor);
}

///////////////////////////////////////////////////////////////////////
// Name:	CopyLayerToMip
// Action:	Copies IRGB Clayer data to a 256 color mip (same dimensions)
///////////////////////////////////////////////////////////////////////
void CWallyDoc::CopyLayerToMip( BYTE* pbyRawMipData)
{
	CLayer* pLayer = GetCurrentLayer();
	ASSERT( pLayer);

	if (pbyRawMipData == NULL)
		pbyRawMipData = GetBits();

	ASSERT( pbyRawMipData);

	int iWidth  = Width();
	int iHeight = Height();

	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidth; i++)
		{
			COLOR_IRGB irgbTexel = pLayer->GetPixel( i, j);
			*pbyRawMipData       = (BYTE )GetIValue( irgbTexel);

			pbyRawMipData++;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Name:	CopyMipToLayer
// Action:	Copies a 256 color mip data to IRGB Clayer (same dimensions)
///////////////////////////////////////////////////////////////////////
void CWallyDoc::CopyMipToLayer( BYTE* pbyRawMipData)
{
	CLayer* pLayer = GetCurrentLayer();
	ASSERT( pLayer);

	if (pbyRawMipData == NULL)
		pbyRawMipData = GetBits();

	ASSERT( pbyRawMipData);

	int iWidth  = Width();
	int iHeight = Height();

	ASSERT( pLayer->GetWidth() == iWidth);
	ASSERT( pLayer->GetHeight() == iHeight);
	
	// Neal - I think a CLayer should always be 24 bit ??? (default is 24)
	//pLayer->SetNumBits( 8);
	pLayer->m_pDoc = this;

	CMDIFrameWnd* pFrame = (CMDIFrameWnd *)AfxGetApp()->m_pMainWnd;

	// Get the active MDI child window.

	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();

	// Get the active view attached to the active MDI child window.

	CView*      pView      = pChild->GetActiveView();
	CWallyView* pWallyView = DYNAMIC_DOWNCAST( CWallyView, pView);
//	pLayer->m_pView        = pWallyView;

	int iIndex = 0;
	int iColor = 0;
	for (int j = 0; j < iHeight; j++)
	{
		for (int i = 0; i < iWidth; i++)
		{
			iIndex = (j * iWidth) + i;

			iColor = pbyRawMipData[iIndex];
			SetIndexColorPixel( NULL, i, j, iColor);

			//pbyRawMipData++;
		}
	}
}

///////////////////////////////////////////////////////////////////////
// Name:	RebuildSubMips
// Action:	Uses ReMipDLX technology to rebuild the three sub-mips
//			of the current WAL document
///////////////////////////////////////////////////////////////////////
void CWallyDoc::RebuildSubMips()
{
	BeginWaitCursor();

	int j;
	int iWidth  = Width();
	int iHeight = Height();
	int iHeaderSize = Q2_HEADER_SIZE;
	int iSizes[4];
	int iMipSize = 0;

	for (j = 0; j < 4; j++)
	{
		iSizes[j] = GetSize(j);
		iMipSize += iSizes[j];
	}

	iMipSize += iHeaderSize;

	BYTE* pbyMipData = (BYTE *)malloc( iMipSize);	
	
	LPQ2_MIP_S pMipHeader = (LPQ2_MIP_S)pbyMipData;

	pMipHeader->offsets[0] = iHeaderSize;
	pMipHeader->offsets[1] = iHeaderSize + iSizes[0];
	pMipHeader->offsets[2] = iHeaderSize + iSizes[0] + iSizes[1];
	pMipHeader->offsets[3] = iHeaderSize + iSizes[0] + iSizes[1] + iSizes[2];
		
	BYTE* pbyTexel = pbyMipData + iHeaderSize;
	CopyLayerToMip( pbyTexel);

	pMipHeader->width  = iWidth;
	pMipHeader->height = iHeight;

	///////////////////////////////
	// Rebuild all the sub-mips //
	/////////////////////////////

#ifdef _DEBUG
	BOOL bValid = TRUE;

	for (j = 0; j < 4; j++)
	{		
		if (IsBadReadPtr( pbyMipData + (pMipHeader->offsets[j]), iSizes[j]))
		{
			ASSERT( FALSE);
			bValid = FALSE;
			break;
		}
		if (IsBadWritePtr( MipArray[j].mip_pointer, iSizes[j]))
		{
			ASSERT( FALSE);
			bValid = FALSE;
			break;
		}
	}
	if (bValid)
#endif
		m_Palette.RebuildWAL( pbyMipData);

	for (j = 0; j < 4; j++)
	{
		ASSERT( iSizes[j] == GetSize(j));
	}

	/////////////////////////
	// copy the data back //
	///////////////////////

	for (j = 1; j < 4; j++)
	{		
#ifdef _DEBUG
		if (IsBadReadPtr( pbyMipData + (pMipHeader->offsets[j]), iSizes[j]))
		{
			ASSERT( FALSE);
			break;
		}
		if (IsBadWritePtr( MipArray[j].mip_pointer, iSizes[j]))
		{
			ASSERT( FALSE);
			break;
		}
#endif
		memcpy( MipArray[j].mip_pointer, pbyMipData + (pMipHeader->offsets[j]), iSizes[j]);
	}

	pMipHeader = NULL;
	free (pbyMipData);
	pbyMipData = NULL;

	EndWaitCursor();
}

BOOL CWallyDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	// All of this code was retrieved from MFC's Doccore.cpp
	if (IsModified())
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");

	CFileException fe;
	CFile* pFile = GetFile(lpszPathName,
		CFile::modeRead|CFile::shareDenyWrite, &fe);

	if (pFile == NULL)
	{
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

   	CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
	loadArchive.m_pDocument = this;
	loadArchive.m_bForceFlat = FALSE;
	TRY
	{
		CWaitCursor wait;
		if (pFile->GetLength() != 0)
			Serialize(loadArchive);     // load me
		loadArchive.Close();
		ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e)
	{
		ReleaseFile(pFile, TRUE);
		DeleteContents();   // remove failed contents

		TRY
		{
			ReportSaveLoadException(lpszPathName, e,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		}
		END_TRY
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

	
	switch (GetGameType())
	{
	case FILE_TYPE_QUAKE2:		
		SetPalette( quake2_pal, 256, FALSE);
		CopyMipToLayer();
		break;

	case FILE_TYPE_QUAKE1:
		SetPalette( quake1_pal, 256, FALSE);
		// drop thru
	case FILE_TYPE_HERETIC2:
	case FILE_TYPE_SIN:
		CopyMipToLayer();
		break;

	case FILE_TYPE_TGA:
	case FILE_TYPE_PCX:
	case FILE_TYPE_BMP:
	case FILE_TYPE_PNG:
	case FILE_TYPE_JPG:
	case FILE_TYPE_TEX:
		CopyLayerToMip();
		break;

	default:
		ASSERT (false);		// Unhandled game type!
		break;
	}
	//CopyMipToLayer();

	SetModifiedFlag( FALSE);     // start off with unmodified

	if (m_pDocTemplate != NULL)
		m_pDocTemplate->SetDefaultTitle(this);

	return TRUE;

}

void CWallyDoc::SetTitle( LPCTSTR lpstrTitle)
{
	if (lpstrTitle != NULL)
		m_strTitle = lpstrTitle;

	int iPos = m_strTitle.Find( " *");

	if (IsModified())
	{
		if (iPos == -1)
			m_strTitle += " *";
	}
	else
	{
		if (iPos != -1)
			m_strTitle = m_strTitle.Left( m_strTitle.GetLength() - 2);
	}
	UpdateFrameCounts();        // will cause name change in views	
}

void CWallyDoc::OnViewToolOptions() 
{
	CToolDlg dlg;

	if (IDOK == dlg.DoModal())
	{
		// force a rebuild

		FreeEffectsData();
	}	
}

void CWallyDoc::OnFileSaveAs() 
{
	if (GetGameType() == FILE_TYPE_HALF_LIFE)
	{
		m_strPathName = "";
	}
	if (!DoSave(NULL))
		TRACE0("Warning: File save-as failed.\n");
}


////////////////////////////////////////////////////////////////////////////////////////////
//  Name:		DoSave
//	Action:		Handles the saving of files, overridden here to trap the directory
//				that the file was saved to.  This code came straight out of doccore.cpp

BOOL CWallyDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	/*

	// TODO: change SaveAs to include all supported image types

	BOOL bNon16 = FALSE;

	if (!IsValidImageSize ( Width(), Height(), FORCE_X_16))
	{
		// Let them save to PCX, TGA, or BMP
		bNon16 = TRUE;
		lpszPathName = NULL;	
	}
	*/

	if ((!IsValidImageSize ( Width(), Height(), FORCE_X_16)) && (!g_bAllowNon16))
	{
		BOOL bError = FALSE;
		CString strError("");

		switch (GetGameType())
		{
		case FILE_TYPE_HALF_LIFE:
			bError = TRUE;
			strError = "Half-Life";
			break;
		
		case FILE_TYPE_SIN:
			bError = TRUE;
			strError = "SiN";
			break;
		
		case FILE_TYPE_QUAKE2:
			bError = TRUE;
			strError = "Quake2";
			break;

		case FILE_TYPE_QUAKE1:
			bError = TRUE;
			strError = "Quake1";
			break;

		case FILE_TYPE_HERETIC2:
			bError = TRUE;
			strError = "Heretic2";
			break;

		default:			
			break;
		}


		if (bError)
		{
			strError += " texture dimensions must be in multiples of 16.\nGo to Image|Adjust Canvas to correct this.";
			AfxMessageBox (strError, MB_ICONSTOP);
			
			return FALSE;
		}
	}

	if (GetGameType() == FILE_TYPE_HALF_LIFE)
	{		
		if (m_strPathName == m_strPackageMipName)
		{
			// if m_strPathName was set to the same as the m_strPackageMipName, then 
			// this item belongs to a currently-existing WAD.  Do not SaveAs, just Save
			SerializePackage(false, NULL);
		}
		else
		{
			// if m_strPathName is set to whatever, we haven't been saved into a WAD.  Go
			// SaveAs			
			SerializePackage(true, NULL);
		}
		return true;
	}	

	if (GetGameType() == FILE_TYPE_QUAKE1)
	{		
		if ((m_strPathName == m_strPackageMipName) && (m_strPathName != ""))
		{
			SerializePackage(false, NULL);
			return true;
		}

		if ((m_pPackageDoc) && (m_strPackageFile == ""))
		{
			m_pPackageDoc->UpdateImageData(this);
			SetModifiedFlag (false);
			return true;
		}
	}


	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
#ifndef _MAC
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
#else
			int iBad = newName.FindOneOf(_T(":"));
#endif
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);

#ifndef _MAC
			/*
			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
			*/
#endif
		}


		// This is the custom stuff:
				
		CString strWildCard("");
		CString strTitle("Save As");
		CString strAppendExtension("");
		
		CString strExtension ("");
		strExtension = GetExtension (newName);
		strExtension.MakeLower();

		BOOL bExtension = (strExtension != "");

		int iFlag = 0;

		switch (GetGameType())
		{
		case FILE_TYPE_QUAKE1:
			{
				if (!bExtension)
				{
					newName += ".mip";
				}
				strAppendExtension = "mip";
				strWildCard = "Quake1 Texture (*.mip)|*.mip|Quake1 Package (*.wad)|*.wad||";				
			}
			break;

		case FILE_TYPE_QUAKE2:
			{
				if (!bExtension)
				{
					newName += ".wal";
				}
				strAppendExtension = "wal";
				strWildCard = "Quake2 Texture (*.wal)|*.wal||";	
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_SIN:
			{
				if (!bExtension)
				{
					newName += ".swl";
				}
				strAppendExtension = "swl";
				strWildCard = "SiN Texture (*.swl)|*.swl||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;
		
		case FILE_TYPE_HERETIC2:
			{
				if (!bExtension)
				{
					newName += ".m8";
				}
				strAppendExtension = "m8";
				strWildCard = "Heretic2 Texture (*.m8)|*.m8||";	
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_TGA:
			{
				if (!bExtension)
				{
					newName += ".tga";
				}
				strAppendExtension = "tga";
				strWildCard = "Truevision Targa (*.tga)|*.tga||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_PCX:
			{
				if (!bExtension)
				{
					newName += ".pcx";
				}
				strAppendExtension = "pcx";
				strWildCard = "Zsoft Paintbrush (*.pcx)|*.pcx||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_BMP:
			{
				if (!bExtension)
				{
					newName += ".bmp";
				}
				strAppendExtension = "bmp";
				strWildCard = "Windows Bitmap (*.bmp)|*.bmp||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_PNG:
			{
				if (!bExtension)
				{
					newName += ".png";
				}
				strAppendExtension = "png";
				strWildCard = "Portable Network Graphics (*.png)|*.png||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_JPG:
			{
				if (!bExtension)
				{
					newName += ".jpg";
				}
				strAppendExtension = "jpg";
				strWildCard = "JPEG - JFIF (*.jpg)|*.jpg||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		case FILE_TYPE_TEX:
			{
				if (!bExtension)
				{
					newName += ".tex";
				}
				strAppendExtension = "tex";
				strWildCard = "Serious Sam Texture (*.tex)|*.tex||";
				iFlag |= OFN_OVERWRITEPROMPT;
			}
			break;

		default:
			ASSERT (false);
			break;
		}
				
		// Create a CFileDialog, init with our strings		
		CFileDialog	dlgSave (FALSE, strAppendExtension, newName, 
			iFlag | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, strWildCard, NULL);
	
		// Set some of the CFileDialog vars
		if (g_strFileSaveDirectory !=  "")
		{
			dlgSave.m_ofn.lpstrInitialDir = g_strFileSaveDirectory;
		}
		dlgSave.m_ofn.lpstrTitle = strTitle;	
				
		//dlgSave.m_ofn.nFilterIndex = g_iFileSaveExtension;
		dlgSave.m_ofn.nFilterIndex = 0;
	
		if (dlgSave.DoModal() == IDOK)
		{
			g_strFileSaveDirectory = dlgSave.GetPathName().Left(dlgSave.m_ofn.nFileOffset);
			g_iFileSaveExtension = dlgSave.m_ofn.nFilterIndex;
			newName = dlgSave.GetPathName();
			
			int iExtensionMarker = dlgSave.m_ofn.nFileExtension;
			int iFileNameLength = newName.GetLength();

			// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
			// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
			// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
			// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
			// We have to check for each instance and add the extension, as required.

			if ((newName.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
			{
				newName += ".";
			}

			if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
			{
				// User didn't type in the extension

				switch (GetGameType())
				{
				case FILE_TYPE_QUAKE1:
					{
						newName += (g_iFileSaveExtension == 1) ? "mip" : "wad";						
					}
					break;

				case FILE_TYPE_QUAKE2:
					{
						newName += "wal";
					}
					break;

				case FILE_TYPE_SIN:
					{
						newName += "swl";						
					}
					break;

				case FILE_TYPE_HERETIC2:
					{
						newName += "m8";
					}
					break;

				case FILE_TYPE_TGA:
					{				
						newName += "tga";
					}
					break;

				case FILE_TYPE_PCX:
					{				
						newName += "pcx";
					}
					break;

				case FILE_TYPE_BMP:
					{				
						newName += "bmp";
					}
					break;

				case FILE_TYPE_PNG:
					{
						newName += "png";
					}
					break;

				case FILE_TYPE_JPG:
					{
						newName += "jpg";
					}
					break;

				case FILE_TYPE_TEX:
					{
						newName += "tex";
					}
					break;

				default:
					ASSERT (FALSE);
					break;
				}				
			}			
		}
		else
		{			
			return FALSE;  // don't even attempt to save
		}

		if (GetGameType() == FILE_TYPE_QUAKE1)
		{
			if (g_iFileSaveExtension == 2)
			{
				// Selected a WAD, save there!
				SerializePackage (true, newName);
				return false;
			}
		}	
	
		//  Original code follows:

//		if (!AfxGetApp()->DoPromptFileName(newName,
//		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
//		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
//			return FALSE;        
	}

	// Nothing past here has been changed

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
				e->Delete();
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);

	return TRUE;        // success
}


void CWallyDoc::UpdateSubMipDIBs()
{
	CWallyView* pView = NULL;
	POSITION    Pos   = GetFirstViewPosition();

	while (Pos != NULL)
	{
		pView = (CWallyView *)GetNextView( Pos);

		for (int j = 1; j < 4; j++)
		{					
			//pView->m_DIB[j].SetRawBits( GetBits(j));		
			pView->SetRawDibBits( j, NULL, false);		
		}		
		pView->RedrawWindow( NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
	}
}


//////////////////////////////////////////////////////////////////////////////
//	Name:	UpdateAllDIBs
//	Action:	To search out and find all DIBs attached to this document, and 
//			update their information, as the data has been changed.  Will call
//			SetScrollBars() for those views if needed (width/height difference
//			on Undo/Redo)
//////////////////////////////////////////////////////////////////////////////
void CWallyDoc::UpdateAllDIBs( BOOL bRebuildSubMips)
{
	if (bRebuildSubMips)
		RebuildSubMips();

	POSITION Pos          = GetFirstViewPosition();
	BOOL     bSizeChanged = FALSE;
	
	while (Pos != NULL)
	{
		int         j;
		BOOL        bPaletteChanged = FALSE;

		int         iWidth          = MipArray[0].width;
		int         iHeight         = MipArray[0].height;		
		CWallyView* pView           = (CWallyView *)GetNextView( Pos);		
		BYTE        byPackedPal[3*256];

		GetPalette( byPackedPal, 256);
		
		bSizeChanged = FALSE;	// neal - needs to be reset for every view

		/////////////////////////////////////
		// determine amount of work to do //
		///////////////////////////////////

		// neal - speed-up (get it out of SUB-MIP loop)

		if ((pView->m_DIB[0].GetImageWidth() != iWidth) ||
				(pView->m_DIB[0].GetImageHeight() != iHeight)
//				|| (iWidth != iHeight)
				)
		{
			bSizeChanged = TRUE;
		}
		//else
		{
			// Neal - BUGFIX
			// undo/redo after doc & pal change => incorrect dib palette bug

			BYTE* byDibPackedPal = pView->m_DIB[0].GetPalette();

			for (j = 0; j < 3*256; j++)
			{
				if (byDibPackedPal[j] != byPackedPal[j])
				{
					bPaletteChanged = TRUE;
					break;
				}
			}
		}

		if (bSizeChanged)
		{
			// Neal - BUGFIX - moving it up here fixed HL WAD filters go to black bug
			// Neal - BUGFIX - fixes crash after resize/change_canvas_size
			//				   followed by undo
			//
			RebuildMipArray();
			RebuildSubMips();
		}

		///////////////////////////
		// rebuild the sub-mips //
		/////////////////////////

		for (j = 0; j < 4; j++)
		{			
			iWidth  = MipArray[j].width;
			iHeight = MipArray[j].height;

			// Reset DIBSection, if something has changed

			if (bSizeChanged)
			{
//				// Neal - BUGFIX - fixes crash after resize/change_canvas_size
//				//				   followed by undo
//				//
//				RebuildMipArray();
//				RebuildSubMips();

				pView->m_DIB[j].SetWidth( iWidth);
				pView->m_DIB[j].SetHeight( iHeight);
				pView->m_DIB[j].RebuildDibSection( byPackedPal);
			}
			else if (bPaletteChanged)
			{
				// Neal - fixes UNDO palette color change bug - (how?)

				//pView->m_DIB[j].RebuildPalette( byPackedPal, TRUE);
				pView->m_DIB[j].RebuildDibSection( byPackedPal);
			}
			//pView->m_DIB[j].SetRawBits( GetBits(j));
			pView->SetRawDibBits( j, NULL, false);
		}

		// Neal - fix cutout-not-shown bug
		if (HasSelection())
		{
			CLayer *pLayer = &GetBase()->m_Layer;
			pView->m_DIB[0].SetBitsFromLayer( pLayer, NULL, FALSE, FALSE);

			CSelection *pSel = GetSelection();
			pLayer = pSel->GetLayer();
			CRect BoundsRect = pSel->GetBoundsRect();
			pView->m_DIB[0].SetBitsFromLayer( pLayer, &BoundsRect, FALSE, pView->m_bTile);
		}

		if (bSizeChanged)
		{
			pView->SetScrollBars( FALSE);
			RebuildMipArray();
		}
		else
		{
			// neal - much less flashing this way
			pView->InvalidateRect( NULL, FALSE);
		}

		// Neal - "if (bPaletteChanged)" fixes UNDO palette color change SUBMIP bug - (how?)
		if (bPaletteChanged)
			RebuildSubMips();
	}
//	return bSizeChanged;
	return;
}

void CWallyDoc::SetDimensions( int iWidth, int iHeight)
{
//	m_Base.GetLayer()->SetWidth( Width);
//	m_Base.GetLayer()->SetHeight( Height);

#ifdef _DEBUG
	if (! m_Undo.IsAnyUndoInProgress())		// undo serialize can change size (false alarm)
	{
		if (m_Base.GetLayer()->HasData())
		{
			ASSERT( m_Base.GetLayer()->GetWidth()  == iWidth);
			ASSERT( m_Base.GetLayer()->GetHeight() == iHeight);
		}
	}
#endif

	Q2Header.width     = iWidth;
	Q2Header.height    = iHeight;
	Q1Header.width     = iWidth;
	Q1Header.height    = iHeight;
	SinHeader.width    = iWidth;
	SinHeader.height   = iHeight;
	
	MipArray[0].width  = iWidth;
	MipArray[0].height = iHeight;

	// Make sure the submips are at least 1 pixel
	MipArray[1].width  = max (iWidth  / 2, 1);
	MipArray[1].height = max (iHeight / 2, 1);
	MipArray[2].width  = max (iWidth  / 4, 1);
	MipArray[2].height = max (iHeight / 4, 1);
	MipArray[3].width  = max (iWidth  / 8, 1);
	MipArray[3].height = max (iHeight / 8, 1);

	int iMipSize   = iWidth * iHeight;
	int iMipSize4  = MipArray[1].width * MipArray[1].height;
	int iMipSize16 = MipArray[2].width * MipArray[2].height;
	int iMipSize64 = MipArray[3].width * MipArray[3].height;;

	// neal - need to rebuild header - size may have changed

	Q2Header.offsets[0] = Q2_HEADER_SIZE;
	Q2Header.offsets[1] = Q2_HEADER_SIZE + iMipSize;
	Q2Header.offsets[2] = Q2_HEADER_SIZE + iMipSize + iMipSize4;
	Q2Header.offsets[3] = Q2_HEADER_SIZE + iMipSize + iMipSize4 + iMipSize16;
	
	Q1Header.offsets[0] = Q1_HEADER_SIZE;
	Q1Header.offsets[1] = Q1_HEADER_SIZE + iMipSize;
	Q1Header.offsets[2] = Q1_HEADER_SIZE + iMipSize + iMipSize4;
	Q1Header.offsets[3] = Q1_HEADER_SIZE + iMipSize + iMipSize4 + iMipSize16;

	SinHeader.offsets[0] = SIN_HEADER_SIZE;
	SinHeader.offsets[1] = SIN_HEADER_SIZE + iMipSize;
	SinHeader.offsets[2] = SIN_HEADER_SIZE + iMipSize + iMipSize4;
	SinHeader.offsets[3] = SIN_HEADER_SIZE + iMipSize + iMipSize4 + iMipSize16;

	int iWidths[M8_MIPLEVELS];
	int iHeights[M8_MIPLEVELS];
	int iSizes[M8_MIPLEVELS];
	int iPosition = M8_HEADER_SIZE;

	for (int j = 0; j < M8_MIPLEVELS; j++)
	{
		iWidths[j]  = (int)(iWidth / pow (2, j));
		iHeights[j] = (int)(iHeight / pow (2, j));
		iSizes[j]   = iWidths[j] * iHeights[j];
		
		M8Header.width[j]  = iWidths[j];
		M8Header.height[j] = iHeights[j];

		M8Header.offsets[j] = (iSizes[j] != 0) ? iPosition : 0;
		iPosition          += iSizes[j];
	}	
}

void CWallyDoc::OnCloseDocument() 
{
	if (m_pPackageDoc)
	{
		DisconnectPackage (true);		
	}	
	CDocument::OnCloseDocument();
}

void CWallyDoc::SetName (LPCTSTR szName)
{
	int iType = GetGameType();
	CString strTitle = GetTitle();
	CString strCompare (szName);

	switch (iType)
	{
	case FILE_TYPE_QUAKE2:
		memset (Q2Header.name, 0, 32);
		strcpy_s (Q2Header.name, sizeof(Q2Header.name), szName);
		break;

	case FILE_TYPE_HERETIC2:
		memset (M8Header.name, 0, 32);
		strcpy_s (M8Header.name, sizeof(M8Header.name), szName);
		break;

	case FILE_TYPE_SIN:
		memset (SinHeader.name, 0, 64);
		strcpy_s (SinHeader.name, sizeof(SinHeader.name), szName);
		break;

	case FILE_TYPE_QUAKE1:
		{
			memset (Q1Header.name, 0, 16);
			strcpy_s (Q1Header.name, sizeof(Q1Header.name), szName);
			
			if (strCompare == "")
			{
				m_strPackageMipName = strTitle;				
				m_strPackageMipName = m_strPackageMipName.Left(15);
			}
			else
			{
				m_strPackageMipName = szName;
				m_strPackageMipName = m_strPackageMipName.Left(15);
				SetTitle (m_strPackageMipName);
			}
		}
		break;

	case FILE_TYPE_HALF_LIFE:
		{			
			if (strCompare == "")
			{
				m_strPackageMipName = strTitle;
				m_strPackageMipName = m_strPackageMipName.Left(15);				
			}
			else
			{
				m_strPackageMipName = szName;
				m_strPackageMipName = m_strPackageMipName.Left(15);
				SetTitle (m_strPackageMipName);
			}
		}
		break;

	case FILE_TYPE_TGA:
	case FILE_TYPE_PCX:
	case FILE_TYPE_BMP:
	case FILE_TYPE_PNG:
	case FILE_TYPE_JPG:
	case FILE_TYPE_TEX:
		break;

	default:
		ASSERT (false);		// Unhandled game type?
		break;
	}
	
}

CWallyView *CWallyDoc::GetView()
{
	// TODO:  This only returns the first view for the Doc, which theoretically
	// should be the main editing window.

	CWallyView *pView = NULL;
	POSITION    Pos   = GetFirstViewPosition();

	while (Pos != NULL)
	{
		pView = (CWallyView *)GetNextView( Pos);
		return pView;
	}

	return NULL;
}

UINT CWallyDoc::GetNumMips()
{
	int iGameType = GetGameType();

	// Neal - if this fails, the number of sub-mips will be always be 0
	ASSERT ((iGameType >= FILE_TYPE_START) && (iGameType < FILE_TYPE_END));

	// This returns the total number of mips in a game texture, including the first and
	// largest
	switch (iGameType)
	{
		case FILE_TYPE_QUAKE2:
		case FILE_TYPE_HALF_LIFE:				
		case FILE_TYPE_QUAKE1:					
		case FILE_TYPE_SIN:		
			return 4;

		case FILE_TYPE_HERETIC2:
			return 16;

		case FILE_TYPE_TGA:
		case FILE_TYPE_PCX:
		case FILE_TYPE_BMP:
		case FILE_TYPE_PNG:
		case FILE_TYPE_JPG:
		case FILE_TYPE_TEX:
			// Not really a game... just one image
			return 1;
//			return 4;		// Neal - TEST TEST TEST

		default:			
			break;
	}
	return 0;
}