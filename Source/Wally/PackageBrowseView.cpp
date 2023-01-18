
#include "stdafx.h"
#include "wally.h"
#include "PackageBrowseView.h"
#include "PackageView.h"
#include "PackageDoc.h"
#include "PackageListBox.h"
#include "DibSection.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "WADList.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
int WM_PACKAGE_BROWSE_CUSTOM = RegisterWindowMessage ("WM_PACKAGE_BROWSE_CUSTOM");

/////////////////////////////////////////////////////////////////////////////
// CPackageBrowseView

IMPLEMENT_DYNCREATE(CPackageBrowseView, CView)

CPackageBrowseView::CPackageBrowseView()
{
	m_iHorzButtons = 0;
	m_iVertButtons = 0;
	m_iNumImageRows = 0;
	m_iFocusItem = 0;	
	SetViewMode (PACKAGE_BROWSE_VIEWMODE_BROWSE);

	for (int j = 0; j < PACKAGE_BROWSE_NUM_ANIMATION_DIBS; j++)
	{
		m_pdsTiledDIBs[j].pdsImage = NULL;
		m_pdsTiledDIBs[j].pWADItem = NULL;
	}

	m_iNumDIBTiles = 0;
	m_iPreviousTiledItem = -1;
	m_iAnimateTimer = 0;
	m_iCurrentAnimation = 0;
	m_iZoomLevel = 1;
	
	m_bZoomMode = FALSE;
	m_bRandomMode = FALSE;
	m_bAnimateMode = FALSE;

	m_bInitialized = FALSE;
	m_bSized = FALSE;
	m_bScrolling = FALSE;
	m_bResetScrollbars = FALSE;
	m_bDragging	= FALSE;
	m_bCalcSizes = FALSE;
	
	m_iCurrentScrollPos = 0;
	m_iInvalidateScrollRange = 0;

	SetAllowRandomMode (FALSE);	
	SetAllowAnimateMode (FALSE);

	m_ftText.CreateFont (6, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"MS Sans Serif");

	m_ftTextSmall.CreateFont( 8, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Small Fonts");

	m_iPreviousClientWidth = 0;
	m_iPreviousClientHeight = 0;

	m_brBackroundErase.CreateSolidBrush ( RGB (200, 181, 160));

	m_bBackgroundDCInitialized = FALSE;
	m_bLButtonDown = FALSE;
	m_bRButtonDown = FALSE;
}

CPackageBrowseView::~CPackageBrowseView()
{
	for (int j = 0; j < PACKAGE_BROWSE_NUM_ANIMATION_DIBS; j++)
	{
		if (m_pdsTiledDIBs[j].pdsImage)
		{
			delete m_pdsTiledDIBs[j].pdsImage;
			m_pdsTiledDIBs[j].pdsImage = NULL;
		}
		m_pdsTiledDIBs[j].pWADItem = NULL;
	}

	m_brBackroundErase.DeleteObject();
	m_ftText.DeleteObject();
	m_ftTextSmall.DeleteObject();
	m_bmBackgroundDraw.DeleteObject();
	m_dcBackroundDraw.DeleteDC();
}


BEGIN_MESSAGE_MAP(CPackageBrowseView, CView)
	//{{AFX_MSG_MAP(CPackageBrowseView)	
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PACKAGE_EXPORT, OnPackageExport)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_EXPORT, OnUpdatePackageExport)
	ON_COMMAND(ID_PACKAGE_NEW_IMAGE, OnPackageNewImage)
	ON_COMMAND(ID_PACKAGE_OPEN, OnPackageOpen)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_OPEN, OnUpdatePackageOpen)
	ON_COMMAND(ID_USE_AS_LEFT_BULLET_SOURCE, OnUseAsLeftBulletSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_BULLET_SOURCE, OnUpdateUseAsLeftBulletSource)
	ON_COMMAND(ID_USE_AS_LEFT_DECAL_SOURCE, OnUseAsLeftDecalSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_DECAL_SOURCE, OnUpdateUseAsLeftDecalSource)
	ON_COMMAND(ID_USE_AS_LEFT_PATTERN_SOURCE, OnUseAsLeftPatternSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_PATTERN_SOURCE, OnUpdateUseAsLeftPatternSource)
	ON_COMMAND(ID_USE_AS_RIGHT_BULLET_SOURCE, OnUseAsRightBulletSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_BULLET_SOURCE, OnUpdateUseAsRightBulletSource)
	ON_COMMAND(ID_USE_AS_RIGHT_DECAL_SOURCE, OnUseAsRightDecalSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_DECAL_SOURCE, OnUpdateUseAsRightDecalSource)
	ON_COMMAND(ID_USE_AS_RIGHT_PATTERN_SOURCE, OnUseAsRightPatternSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_PATTERN_SOURCE, OnUpdateUseAsRightPatternSource)
	ON_COMMAND(ID_USE_AS_RIVET_SOURCE, OnUseAsRivetSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIVET_SOURCE, OnUpdateUseAsRivetSource)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_PASTE_OVER, OnEditPasteOver)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_OVER, OnUpdateEditPasteOver)
	ON_COMMAND(ID_EDIT_PASTE_PACKAGE, OnEditPastePackage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_PACKAGE, OnUpdateEditPastePackage)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_TP_ZOOM, OnTpZoom)
	ON_UPDATE_COMMAND_UI(ID_TP_ZOOM, OnUpdateTpZoom)
	ON_COMMAND(ID_TP_SELECTION, OnTpSelection)
	ON_UPDATE_COMMAND_UI(ID_TP_SELECTION, OnUpdateTpSelection)
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_PACKAGE_ADD, OnPackageAdd)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_ADD, OnUpdatePackageAdd)
	ON_COMMAND(ID_PACKAGE_REMIP_ALL, OnPackageRemipAll)
	ON_COMMAND(ID_PACKAGE_REMIP_SELECTED, OnPackageRemipSelected)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_REMIP_SELECTED, OnUpdatePackageRemipSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageBrowseView drawing

void CPackageBrowseView::OnInitialUpdate()
{
	DragAcceptFiles (true);
	m_oleDropTarget.Register (this);

	SetViewMode (g_iPackageViewMode);
	
	CView::OnInitialUpdate();
	CalcMaxButtons();

}

BOOL CPackageBrowseView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}


void CPackageBrowseView::OnDraw(CDC* pDC)
{
	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
			DrawBrowseMode(pDC);
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
			DrawTileMode(pDC);
		}
		break;
	
	default:
		ASSERT (FALSE);
		break;
	}
}

void CPackageBrowseView::OnTimer(UINT nIDEvent) 
{	
	CView::OnTimer(nIDEvent);

	if (nIDEvent == TIMER_PB_ID_ANIMATE)
	{
		// Kill the timer if we've switched views
		if (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_BROWSE)
		{
			KillTimer (TIMER_PB_ID_ANIMATE);
			m_iAnimateTimer = 0;
			m_iCurrentAnimation = 0;
		}
		else
		{
			m_iCurrentAnimation++;

			if (m_iCurrentAnimation >= m_iNumDIBTiles)
			{
				m_iCurrentAnimation = 0;
			}
			InvalidateRect (NULL, FALSE);
		}
	}

}

void CPackageBrowseView::DrawTileMode(CDC* pDC)
{
	CRect rClient;
	GetClientRect( &rClient);

	int  rWidth = rClient.Width();
	int  rHeight = rClient.Height();
	int  j = 0;
	bool bLiquid = false;

	SCROLLINFO initScroll;
	UINT uiStructSize = sizeof (SCROLLINFO);
	
	memset (&initScroll, 0, uiStructSize);

	initScroll.cbSize = uiStructSize;
	initScroll.fMask = SIF_PAGE | SIF_RANGE;

	initScroll.nMin = 0;
	initScroll.nMax = 0;
	initScroll.nPage = 0;	
			
	SetScrollInfo (SB_VERT, &initScroll, FALSE);
	
	CPackageTreeControl *pTreeControl = GetPackageTreeCtrl();

	BOOL bEraseBGround = TRUE;

	CDC MemDC;
	CBitmap Bmp;
	
	MemDC.CreateCompatibleDC( pDC);
	Bmp.CreateCompatibleBitmap( pDC, rWidth, rHeight);
	CBitmap *pOldBitmap = MemDC.SelectObject( &Bmp);

	CPackageDoc *pDoc = GetDocument();
	
	if( pTreeControl )
	{		
		CWADItem *pWADItem = NULL;
		pWADItem = pTreeControl->GetSelectedImage();

			if( pWADItem )
			{
				// Draw the image
				int iWidth = 0;
				int iHeight = 0;
				
				if( (pWADItem != m_pPreviousTiledItem) || (m_bForceDIBRebuild))
				{
					m_bRandomMode = FALSE;
					m_bAnimateMode = FALSE;

					// Rebuild the DIB
					m_pPreviousTiledItem = pWADItem;
					
					// Delete the old ones
					for (j = 0; j < PACKAGE_BROWSE_NUM_ANIMATION_DIBS; j++)
					{
						if (m_pdsTiledDIBs[j].pdsImage)
						{
							delete m_pdsTiledDIBs[j].pdsImage;
							m_pdsTiledDIBs[j].pdsImage = NULL;
						}
						m_pdsTiledDIBs[j].pWADItem = NULL;
					}

					m_pdsTiledDIBs[0].pdsImage = new CDibSection;
					m_pdsTiledDIBs[0].pWADItem = pWADItem;

					// Let's see if it's a random tile texture
					m_iNumDIBTiles = 1;
					CString strName("");
					CString strMatch("");
					strName = pWADItem->GetName();

					char szDigits[] = "0123456789";

					// Neal

					/////////////////////
					// Random Pattern //
					///////////////////

					if ((strName.GetAt(0) == '-') && (m_bAllowRandomMode))
					{						
						ASSERT (PACKAGE_BROWSE_NUM_RANDOM_DIBS <= 10);		// This has to be <= 10; more than 10 would screw up the routine for finding other random tiles
						CWADItem *pWADTemp = NULL;
						strMatch = strName;
						
						for (j = 0; j < PACKAGE_BROWSE_NUM_RANDOM_DIBS; j++)
						{
							// Only look at those images other than ourself!
							if (strName.GetAt(1) != szDigits[j])
							{
								// Find any matches for the 10 digits
								strMatch.SetAt(1, szDigits[j]);

								// See if there's a match
								pWADTemp = pDoc->FindNameInList (strMatch, FALSE);
								
								if (pWADTemp)
								{
									// Make sure the width/height are identical
									if ( (pWADTemp->GetWidth() == pWADItem->GetWidth()) && (pWADTemp->GetHeight() == pWADItem->GetHeight()))
									{
										m_bRandomMode = TRUE;
	
										m_pdsTiledDIBs[m_iNumDIBTiles].pWADItem = pWADTemp;
										m_pdsTiledDIBs[m_iNumDIBTiles].pdsImage = new CDibSection;
										m_iNumDIBTiles++;
									}
								}
							}
						}
					}

					if (m_bAllowAnimateMode)
					{
						///////////////////////
						// Animated Texture //
						/////////////////////

						if (strName.GetAt(0) == '+')
						{
							ASSERT (PACKAGE_BROWSE_NUM_RANDOM_DIBS <= 10);		// This has to be <= 10; more than 10 would screw up the routine for finding other random tiles
							CWADItem *pWADTemp = NULL;
							strMatch = strName;
							
							for (j = 0; j < PACKAGE_BROWSE_NUM_RANDOM_DIBS; j++)
							{
								// Only look at those images other than ourself!
								if (strName.GetAt(1) != szDigits[j])
								{
									// Find any matches for the 10 digits
									strMatch.SetAt(1, szDigits[j]);

									// See if there's a match
									pWADTemp = pDoc->FindNameInList (strMatch, FALSE);
									
									if (pWADTemp)
									{
										// Make sure the width/height are identical
										if ( (pWADTemp->GetWidth() == pWADItem->GetWidth()) && (pWADTemp->GetHeight() == pWADItem->GetHeight()))
										{
											m_bAnimateMode = TRUE;
											m_iAnimateTimerValue = PB_TIMER_ANIMATE_NORMAL;
											m_pdsTiledDIBs[m_iNumDIBTiles].pWADItem = pWADTemp;
											m_pdsTiledDIBs[m_iNumDIBTiles].pdsImage = new CDibSection;
											m_iNumDIBTiles++;
										}
									}
								}
							}
						}

						/////////////////////
						// Liquid Texture //
						///////////////////

						else if (strName.GetAt(0) == '!')
						{
							bLiquid = true;

							CWADItem *pWADTemp = NULL;

							// just find a copy of ourself
							pWADTemp = pDoc->FindNameInList (strName, FALSE);
							
							//////////////////////////////////////////////////////////
							// Neal - liquids just reuse first frame over-and-over //
							////////////////////////////////////////////////////////

							m_bAnimateMode = TRUE;
							m_iAnimateTimerValue = PB_TIMER_ANIMATE_LIQUID;
							m_pdsTiledDIBs[m_iNumDIBTiles].pWADItem = pWADTemp;
							m_pdsTiledDIBs[m_iNumDIBTiles].pdsImage = new CDibSection;

							//m_iNumDIBTiles++;
							m_iNumDIBTiles = 32;

							// Neal - for a liquid, we will always play frame[0]
						}
					}
					
					iWidth = pWADItem->GetWidth();
					iHeight = pWADItem->GetHeight();
					
					int iBitDepth = 0;

					int xOffset = 0;
					int yOffset = 0;
					
					BYTE *pbyData = NULL;
					BYTE *pbyPalette = NULL;
					
					iBitDepth = 8;
					if ((iWidth > 0) && (iHeight > 0))		// Neal - fix Quake1?
						pbyData = pWADItem->GetBits(0);

					pbyPalette = pWADItem->GetPalette();

					#define		PACKAGE_BROWSE_TILE_MULTIPLIER		4
					
					if (pbyData && pbyPalette)
					{
						if ( ((iWidth < 64) || (iHeight < 64)) && (m_iNumDIBTiles == 1))
						{					
							// Let's make a bigger source image so it doesn't take so long to draw
							int iNewWidth = iWidth * PACKAGE_BROWSE_TILE_MULTIPLIER;
							int iNewHeight = iHeight * PACKAGE_BROWSE_TILE_MULTIPLIER;
							int iNewSize = iNewWidth * iNewHeight;

							m_pdsTiledDIBs[0].pdsImage->Init( iNewWidth, iNewHeight, 8, pbyPalette, TRUE);

							int w = 0;
							int h = 0;
							for (h = 0; h < PACKAGE_BROWSE_TILE_MULTIPLIER; h++)
							{
								for (w = 0; w < PACKAGE_BROWSE_TILE_MULTIPLIER; w++)
								{
									m_pdsTiledDIBs[0].pdsImage->AddDIB (w * iWidth, h * iHeight, iWidth, iHeight, pbyData);
								}						
							}
							
							iWidth = iNewWidth;
							iHeight = iNewHeight;
						}
						else
						{	
							for (j = 0; j < m_iNumDIBTiles; j++)
							{
								pbyData = m_pdsTiledDIBs[j].pWADItem->GetBits(0);
								pbyPalette = m_pdsTiledDIBs[j].pWADItem->GetPalette();

								m_pdsTiledDIBs[j].pdsImage->Init( iWidth, iHeight, 8, pbyPalette, TRUE);

								// Neal - animated liquid texture?
								//if (strName.GetAt(0) == '!')
								if (bLiquid)
								{
									m_pdsTiledDIBs[j].pdsImage->SetRawLiquidBits( pbyData, m_iCurrentAnimation);
									break;
								}
								else
								{
									m_pdsTiledDIBs[j].pdsImage->SetRawBits( pbyData);
								}
							}
						}
					}			// if (pbyData && pbyPalette)
					else
					{
						if ((iWidth < 64) || (iHeight < 64))				
						{					
							iWidth *= PACKAGE_BROWSE_TILE_MULTIPLIER;
							iHeight *= PACKAGE_BROWSE_TILE_MULTIPLIER;
						}

						BYTE byPalette[768];
						memset (byPalette, 0, 768);

						// Neal - fix Quake1?
						if ((iWidth > 0) && (iHeight > 0))
						{
							m_pdsTiledDIBs[0].pdsImage->Init( iWidth, iHeight, 8, byPalette, TRUE);
							m_pdsTiledDIBs[0].pdsImage->ClearBits (0);
						}
					}
				}				// if (iCurSel != m_iPreviousTiledItem)

				if (m_bRandomMode)
				{
					InitRandom( (unsigned)time( NULL ));
				}

				if ((m_bAnimateMode) && (m_iAnimateTimer == 0))
				{
					m_iAnimateTimer = SetTimer( TIMER_PB_ID_ANIMATE, m_iAnimateTimerValue, NULL);
				}

				if ((!m_bAnimateMode) && (m_iAnimateTimer))
				{
					KillTimer (TIMER_PB_ID_ANIMATE);
					m_iAnimateTimer = 0;
					m_iCurrentAnimation = 0;
				}
				
				int iDibNum = 0;

				iWidth = m_pdsTiledDIBs[0].pdsImage->GetImageWidth();
				iHeight = m_pdsTiledDIBs[0].pdsImage->GetImageHeight();

				if ((iWidth > 0) && (iHeight > 0))		// Neal - fix Quake1?
				{
					// Figure out how many across we can display
					int iNumAcross = (rWidth / (iWidth * m_iZoomLevel)) + 1;
					int iNumDown = (rHeight / (iHeight * m_iZoomLevel)) + 1;

					for (int h = 0; h < iNumDown; h++)
					{
						for (int w = 0; w < iNumAcross; w++)
						{
							if (m_bRandomMode)
							{							
								iDibNum = (int)(Random() * m_iNumDIBTiles);
							}

							if (m_bAnimateMode)
							{
								if (bLiquid)
								{
									iDibNum = 0;	// Neal - to save space, just rebuild the first image
								}
								else
								{
									// Sanity check
									m_iCurrentAnimation = min (m_iCurrentAnimation, (m_iNumDIBTiles - 1));
									m_iCurrentAnimation = max (m_iCurrentAnimation, 0);

									iDibNum = m_iCurrentAnimation;
								}
							}						
							m_pdsTiledDIBs[iDibNum].pdsImage->Show (&MemDC, w * (iWidth * m_iZoomLevel), h * (iHeight * m_iZoomLevel), m_iZoomLevel);
						}
					}
					bEraseBGround = FALSE;
				}
			}			// if (pWADItem)
		}				// if (pListBox->GetCount() > 0)

	if (bEraseBGround)
	{
		// Erase the background (only needed when we're not showing anything		
		HBRUSH hBrush = CreateSolidBrush ( RGB (200, 181, 160));
		HBRUSH hOldBrush = (HBRUSH )SelectObject( MemDC, hBrush);

		::PatBlt (MemDC, 0, 0, rWidth, rHeight, PATCOPY);

		SelectObject( MemDC, hOldBrush);
		DeleteObject( hBrush);
	}

	pDC->BitBlt( 0, 0, rWidth, rHeight, &MemDC, 0, 0, SRCCOPY);

	MemDC.SelectObject( pOldBitmap);	
	Bmp.DeleteObject();

	MemDC.DeleteDC();
}

void CPackageBrowseView::DrawBrowseMode(CDC* pDC)
{
	CPackageDoc* pDoc = GetDocument();	
	
	CPackageView *pFormView = pDoc->GetFormView();
	CPackageTreeControl *pTreeCtrl = pFormView->GetTreeCtrl();

	int iNumItems = pTreeCtrl->GetVisibleCount();

	CWADItem *pWADItem = NULL;
	int iOldWidth = 0;
	int iOldHeight = 0;
	int iOldBitDepth = 0;

	int iWidth = 0;
	int iHeight = 0;
	int iBitDepth = 0;

	BYTE *pbyData = NULL;
	BYTE *pbyPalette = NULL;
	
	int j = 0;
	int k = 0;	
	int x = 0;
	int y = 0;
	int x1 = 0;
	int y1 = 0;	
	int xOffset = 0;
	int yOffset = 0;

	int iOffsetPosition = 0;
	int iCount = 0;
	int iTextX = 0;
	int iTextY = 0;
	int iTextX1 = 0;
	int iTextY1 = 0;

	CRect rButton;
	CRect rText;
	CRect rClient;
	GetClientRect( &rClient);

	int rWidth = rClient.Width();
	int rHeight = rClient.Height() + m_iBrowseImageSize;
	
	CFont *pOldFont;

	CString strText("");
	BOOL bScrolling = FALSE;

	if (!m_bScrolling)
	{
		// Get the max number of buttons
		CalcMaxButtons();	
	}
	else
	{
		bScrolling = TRUE;
	}

	// Erase the background
	
	if ((m_iPreviousClientWidth < rWidth) || (m_iPreviousClientHeight < rHeight))
	{
		m_bmBackgroundDraw.DeleteObject();
		m_bmBackgroundDraw.CreateCompatibleBitmap( pDC, rWidth, rHeight);

		m_iPreviousClientWidth = rWidth;
		m_iPreviousClientHeight = rHeight;
	}

	if (!m_bBackgroundDCInitialized)
	{	
		m_dcBackroundDraw.CreateCompatibleDC( pDC);
		m_bBackgroundDCInitialized = TRUE;
	}	

	//  Find out where the scrollbar is at, and calculate how many "rows" down we are 
	int iRow = m_iCurrentScrollPos;

	int iStartRow = 0;
	int iEndRow = 0;
/*
	iStartRow = bScrolling ? (m_iInvalidateScrollRange < 0 ? m_iVertButtons + m_iInvalidateScrollRange : 0) : 0;
	iEndRow = bScrolling ? (m_iInvalidateScrollRange < 0 ? m_iVertButtons + 1 : (m_iInvalidateScrollRange - 1)) : m_iVertButtons + 1;
*/

	iEndRow = m_iVertButtons + 1;

	iStartRow = max (iStartRow, 0);
	iEndRow = min (iEndRow, m_iVertButtons + 1);
	
	CBitmap *pOldBitmap = m_dcBackroundDraw.SelectObject( &m_bmBackgroundDraw);
		
	HBRUSH hOldBrush = (HBRUSH )SelectObject( m_dcBackroundDraw, (HBRUSH)m_brBackroundErase);

	// If we're scrolling, only erase the part that needs it
	/*
	if (bScrolling)
	{		
		::PatBlt (m_dcBackroundDraw, 0, iStartRow * PACKAGE_BROWSE_BUTTON_HEIGHT, rWidth, (iEndRow - iStartRow) * PACKAGE_BROWSE_BUTTON_HEIGHT, PATCOPY);
		::PatBlt (m_dcBackroundDraw, 0, (iEndRow + 1) * PACKAGE_BROWSE_BUTTON_HEIGHT, rWidth, rHeight - ((iEndRow + 1) * PACKAGE_BROWSE_BUTTON_HEIGHT), PATCOPY);
	}
	else
	*/
	{
		::PatBlt (m_dcBackroundDraw, 0, 0, rWidth, rHeight, PATCOPY);
	}

	SelectObject( m_dcBackroundDraw, hOldBrush);
	
	m_dcBackroundDraw.SetBkColor (GetSysColor (COLOR_BTNFACE));
		
	if( m_iBrowseImageSize <= 64 )
	{
		pOldFont = m_dcBackroundDraw.SelectObject( &m_ftTextSmall );
	}
	else
	{
		pOldFont = m_dcBackroundDraw.SelectObject( &m_ftText );
	}
	
		
	// Only draw the set of textures that can possibly be seen, and only draw those rows that 
	// were invalidated (if we're scrolling.)
	
	for (j = iStartRow; j <= iEndRow; j++)
	{
		for (k = 0; k < m_iHorzButtons; k++)
		{
			// OffsetPosition is the position of the texture we're currently at
			iOffsetPosition = (iRow * m_iHorzButtons) + (j * m_iHorzButtons) + k;

			if( iOffsetPosition < pTreeCtrl->GetVisibleCount() )
			{
				pWADItem = (CWADItem *)( pTreeCtrl->GetImageAtPosition( iOffsetPosition ));

				if (pWADItem)
				{
					if (pWADItem->IsValidMip())
					{
						pWADItem->SetListBoxIndex (iOffsetPosition);
						//DrawItem( pWADItem, (pListBox->GetSel(iOffsetPosition) > 0) ? TRUE : FALSE, iOffsetPosition == m_iFocusItem ? TRUE : FALSE, &m_dcBackroundDraw, j, k);
						DrawItem( pWADItem, pWADItem->IsSelected(), iOffsetPosition == m_iFocusItem ? TRUE : FALSE, &m_dcBackroundDraw, j, k);
					}
				}
			}			
		}			
	}
	
	//int rcY = iRow * PACKAGE_BROWSE_BUTTON_HEIGHT;
	int rcY = 0;
	pDC->BitBlt( 0, rcY, rWidth, rHeight, &m_dcBackroundDraw, 0, 0, SRCCOPY);

	m_dcBackroundDraw.SelectObject (pOldFont);	
	m_dcBackroundDraw.SelectObject( pOldBitmap);
	
}

void CPackageBrowseView::DrawItem (CWADItem *pItem, BOOL bIsSelected, BOOL bIsFocus, CDC *pMemDC, int iRow, int iColumn)
{
	// Figure out the location to plot this guy... (x,y) = top left, (x1/y1) = bottom right
	int iButtonWidth = m_iBrowseImageSize + 10;
	int iButtonHeight = m_iBrowseImageSize + 14 + 16;
	int x = iColumn * iButtonWidth;
	int y = iRow * iButtonHeight;
	int x1 = x + iButtonWidth - 1;
	int y1 = y + iButtonHeight - 1;

	// Plop in the rectangle coords and draw the button behind the texture
	CRect rButton (x, y, x1, y1);
	CRect rSelectButton (x + 2, y + 2, x1 - 3, y1 - 3);
	pMemDC->DrawFrameControl (&rButton, DFC_BUTTON, DFCS_BUTTONPUSH);

	COLORREF crColorBK;
	COLORREF crColorText;

	if (bIsSelected)
	{
		crColorBK = pMemDC->SetBkColor ( GetSysColor (COLOR_HIGHLIGHT));
		crColorText = pMemDC->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT));

//		HBRUSH hBrush = CreateSolidBrush ( GetSysColor (COLOR_HIGHLIGHT));
		HBRUSH hBrush    = GetSysColorBrush( COLOR_HIGHLIGHT);
		HBRUSH hOldBrush = (HBRUSH )pMemDC->SelectObject( hBrush);

		pMemDC->PatBlt (rSelectButton.left, rSelectButton.top, rSelectButton.right - rSelectButton.left, rSelectButton.bottom - rSelectButton.top, PATCOPY);

		pMemDC->SelectObject( hOldBrush);
//		DeleteObject( hBrush);
	}

	if (bIsFocus)
	{
		/*
		CRect rcFocus (x + 4, y + 4, x1 - 4, y1 - 4);
		CBrush brHighlight(GetSysColor (COLOR_3DHIGHLIGHT));
		
		pMemDC->FrameRect (&rcFocus, &brHighlight);
		
		brHighlight.DeleteObject();
		*/
	}

	// Draw the thumbnail
	int iWidth = pItem->GetWidth();
	int iHeight = pItem->GetHeight();
	int iWidths[4];
	int iHeights[4];
	int j = 0;
	int iMatch = 3;
	BOOL bMatch = FALSE;

	for (j = 0; j < 4; j++)
	{
		iWidths[j] = (int)(iWidth / pow (2, j));
		iHeights[j] = (int)(iHeight / pow (2, j));
	}

	// Find the closest match to 96 x 96
	for (j = 3; (j >= 0) && !bMatch; j--)
	{
		if ((iWidths[j] > m_iBrowseImageSize) || (iHeights[j] > m_iBrowseImageSize))
		{
			bMatch = TRUE;
		}
		else
		{
			iMatch = j;
		}
	}

	iWidth = iWidths[iMatch];
	iHeight = iHeights[iMatch];

	int iBitDepth = 0;
	int xOffset = 0;
	int yOffset = 0;
	
	BYTE *pbyData = NULL;
	BYTE *pbyPalette = NULL;

	CDibSection DibItem;
	
	iBitDepth = 8;
	pbyData = pItem->GetBits(iMatch);
	pbyPalette = pItem->GetPalette();

	if (pbyData && pbyPalette)
	{
		DibItem.Init( iWidth, iHeight, 8, pbyPalette, TRUE);
		DibItem.SetRawBits( pbyData);
	}
	else
	{
		BYTE *pbyBits = new BYTE[iWidth * iHeight];
		memset (pbyBits, 0, iWidth * iHeight);

		BYTE byPalette[768];
		memset (byPalette, 0, 768);

		DibItem.Init( iWidth, iHeight, 8, byPalette, TRUE);		
		DibItem.SetRawBits (pbyBits);

		if (pbyBits)
		{
			delete []pbyBits;
			pbyBits = NULL;
		}
	}

	BOOL bRestrict = FALSE;
	int iZoomValue = 1;
	
	if ((iWidth > m_iBrowseImageSize) || (iHeight > m_iBrowseImageSize))
	{
		bRestrict = TRUE;
	}
	else
	{
		int iW, iH, iM;
		
		// How many multiples can we fit in?
		iW = m_iBrowseImageSize / iWidth;
		iH = m_iBrowseImageSize / iHeight;

		iM = min( iW, 4 );
		iM = min( iM, min( iH, 4 ) );

		iWidth *= iM;
		iHeight *= iM;
		iZoomValue = iM;
#if 0
		int i, j;
		bool bDone = false;
		i = 16;	
		j = ((m_iBrowseImageSize / 2) / 16);
		// Start at 16 and keep adding until we've reached 1/2 the button width/height
		for( i = 16; (i < (m_iBrowseImageSize / 2)) && !bDone; )
		{
			if( (i >= iWidth) && (i >= iHeight) )
			{
				bDone = true;				
			}
			else
			{
				i += 16;
				j--;
			}
		}

		iWidth *= j;
		iHeight *= j;
		iZoomValue = j;


		if ((iWidth <= 16) && (iHeight <= 16))
		{
			iWidth *= 4;
			iHeight *= 4;
			iZoomValue = 4;
		}
		else
		{
			if ((iWidth <= 32) && (iHeight <= 32))
			{
				iWidth *= 3;
				iHeight *= 3;
				iZoomValue = 3;
			}
			else
			{
				if ((iWidth <= (m_iBrowseImageSize / 2)) && (iHeight <= m_iBrowseImageSize / 2))
				{
					iWidth *= 2;
					iHeight *= 2;
					iZoomValue = 2;
				}
			}
		}
#endif
	}
	
	if (iWidth < m_iBrowseImageSize)
	{
		xOffset = (iButtonWidth / 2) - (iWidth / 2);
	}
	else
	{		
		xOffset = 3;
	}

	if (iHeight < m_iBrowseImageSize)
	{
		yOffset = (iButtonHeight / 2) - (iHeight / 2);
	}
	else
	{
		yOffset = 3;
	}
	
	CRect rcRestricted (x + xOffset, y + yOffset, x + iButtonWidth - xOffset - 2, y + iButtonHeight - yOffset - 2);

	if (bRestrict)
	{
		DibItem.ShowRestricted (pMemDC, rcRestricted);
	}
	else
	{
		DibItem.Show (pMemDC, x + xOffset, y + yOffset, iZoomValue);
	}
	
	
	// Set up where the text should be plotted
	int iTextX = 0;
	int iTextY = 0;
	int iTextX1 = 0;
	int iTextY1 = 0;
	if( m_iBrowseImageSize <= 64 )
	{
		iTextX = x + 2;
		iTextY = y + iButtonHeight - 18;
		iTextX1 = x + iButtonWidth - 4;
		iTextY1 = y + iButtonHeight;
	}
	else
	{
		iTextX = x + 6;
		iTextY = y + iButtonHeight - 22;
		iTextX1 = x + iButtonWidth - 6;
		iTextY1 = y + iButtonHeight;
	}	

	// This guy is used to clip the text, in case it's really long
	CRect rText( iTextX, iTextY, iTextX1, iTextY1 );

	CString strText("");
	strText = pItem->GetName();
	
	// Write to the screen!
	pMemDC->ExtTextOut(iTextX, iTextY, ETO_CLIPPED, &rText, strText, strText.GetLength(), NULL);

	if( bIsSelected )
	{
		pMemDC->SetBkColor ( crColorBK);
		pMemDC->SetTextColor( crColorText);
	}	
}

/////////////////////////////////////////////////////////////////////////////
// CPackageBrowseView diagnostics

#ifdef _DEBUG
void CPackageBrowseView::AssertValid() const
{
	CView::AssertValid();
}

void CPackageBrowseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPackageDoc* CPackageBrowseView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPackageDoc)));
	return (CPackageDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackageBrowseView message handlers

DROPEFFECT CPackageBrowseView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if (pDataObject->IsDataAvailable(CF_HDROP)) 
	{
		return DROPEFFECT_COPY;
	}

	if (pDataObject->IsDataAvailable(g_iPackageFormat)) 
	{		
		return DROPEFFECT_COPY;
	}

	if (pDataObject->IsDataAvailable(g_iBrowseCopyPasteFormat)) 
	{		
		return DROPEFFECT_COPY;
	}
	
	return DROPEFFECT_NONE;
	
	//return CScrollView::OnDragOver(pDataObject, dwKeyState, point);
}

BOOL CPackageBrowseView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{	
	//return CView::OnDrop(pDataObject, dropEffect, point);

	CPackageView *pView = GetPackageView();
	
	if (pView)
	{
		// Adjust the point for the package view width.  6 for the separator bar
		CRect rcClient;
		pView->GetClientRect (&rcClient);

		point.x += rcClient.Width() + 6;

		return GetPackageView()->DoDragDrop (pDataObject, dropEffect, point);
	}
	return FALSE;
}

BOOL CPackageBrowseView::PreTranslateMessage(MSG* pMsg) 
{
	if (GetPackageView())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			GetPackageView()->DoDragDrop (pMsg);
			return true;
		}
	}
	return CView::PreTranslateMessage(pMsg);
}


void CPackageBrowseView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if (!m_bCalcSizes)
	{
		CalcMaxButtons();
	}
}

void CPackageBrowseView::CalcMaxButtons()
{	
	// Figure out how many buttons can fit 

	CPackageDoc* pDoc = GetDocument();

	if (!pDoc)
	{
		return;
	}
		
	m_bCalcSizes = TRUE;

	CPackageView *pFormView = pDoc->GetFormView();
	CPackageTreeControl *pTreeCtrl = pFormView->GetTreeCtrl();
	CPackageListBox *pListBox = pFormView->GetListBox();

	int iNumItems = 0;

	if( ::IsWindow( pTreeCtrl->m_hWnd ) )
	{
		iNumItems = pTreeCtrl->GetVisibleCount();
	}
	else
	{
		iNumItems = pDoc->GetNumImages();
	}

	CRect crView;
	GetClientRect (&crView);
	CSize csPageAmount;
	CSize csLineAmount;

	int iVertButtons = 0;
	int iHorzButtons = 0;
	int iButtonWidth = m_iBrowseImageSize + 10;
	int iButtonHeight = m_iBrowseImageSize + 14 + 16;

	div_t dtSize;
	dtSize = div (crView.Height(), iButtonHeight);
	
	iVertButtons = dtSize.quot;
	iVertButtons = max (iVertButtons, 1);

	dtSize = div (crView.Width(), iButtonWidth);

	iHorzButtons = dtSize.quot;
	iHorzButtons = max (iHorzButtons, 1);

	int iNumImageRows = (iNumItems + iHorzButtons) / iHorzButtons;
	
	// Only adjust the scrollbars if the dimensions of the window have changed	
	if (
		(iHorzButtons != m_iHorzButtons) || 
		(iVertButtons != m_iVertButtons) || 
		((iNumImageRows != m_iNumImageRows) && (iNumImageRows > iVertButtons)) ||
		m_bResetScrollbars
		)
	{	
		m_iVertButtons = iVertButtons;
		m_iHorzButtons = iHorzButtons;
		m_iNumImageRows = iNumImageRows;
		
		SCROLLINFO initScroll;
		UINT uiStructSize = sizeof (SCROLLINFO);
		
		memset (&initScroll, 0, uiStructSize);

		initScroll.cbSize = uiStructSize;
		initScroll.fMask = SIF_PAGE | SIF_RANGE;

		initScroll.nMin = 0;
		initScroll.nMax = iNumImageRows;
		initScroll.nPage = m_iVertButtons;

		SetScrollInfo (SB_VERT, &initScroll, FALSE);

		/*
		m_iCurrentScrollPos = min (m_iCurrentScrollPos, iNumImageRows);
		m_iCurrentScrollPos = max (m_iCurrentScrollPos, 0);
		*/
		m_iCurrentScrollPos = 0;
		SetScrollPos (SB_VERT, m_iCurrentScrollPos, TRUE);

		m_bResetScrollbars = FALSE;
	}
	
	m_bCalcSizes = FALSE;	
}

BOOL CPackageBrowseView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{	
	return CView::OnScrollBy(sizeScroll, bDoScroll);
}

void CPackageBrowseView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CView::OnVScroll(nSBCode, nPos, pScrollBar);

	int iScrollBarPos = m_iCurrentScrollPos;
	int iButtonWidth = m_iBrowseImageSize + 10;
	int iButtonHeight = m_iBrowseImageSize + 14 + 16;
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		{
			iScrollBarPos -= 1;
		}
		break;

	case SB_LINEDOWN:
		{
			iScrollBarPos += 1;
		}
		break;

	case SB_PAGEUP:
		{
			iScrollBarPos -= m_iVertButtons;
		}
		break;

	case SB_PAGEDOWN:
		{
			iScrollBarPos += m_iVertButtons;
		}
		break;

	case SB_BOTTOM:
		{
			iScrollBarPos = m_iNumImageRows;
		}
		break;

	case SB_TOP:
		{
			iScrollBarPos = 0;
		}
		break;

	case SB_THUMBPOSITION:
		{
			iScrollBarPos = nPos;
		}
		break;

	case SB_THUMBTRACK:
		{
			iScrollBarPos = nPos;
		}
		break;

	case SB_ENDSCROLL:
		{
			m_bScrolling = FALSE; 
		}
		break;
	}

	iScrollBarPos = min (iScrollBarPos, m_iNumImageRows);
	iScrollBarPos = max (iScrollBarPos, 0);

	if (iScrollBarPos != m_iCurrentScrollPos)
	{
		m_bScrolling = TRUE;
		m_iInvalidateScrollRange = m_iCurrentScrollPos - iScrollBarPos;

		m_iCurrentScrollPos = iScrollBarPos;		

		if (abs (m_iInvalidateScrollRange) < m_iVertButtons)
		{
			ScrollWindow (0, (m_iInvalidateScrollRange * iButtonHeight), NULL, NULL);
			UpdateWindow();
		}
		else
		{
			InvalidateRect (NULL, FALSE);
		}
		
		//TRACE ("m_iCurrentScrollPos = %d\n", m_iCurrentScrollPos);
		SetScrollPos (SB_VERT, m_iCurrentScrollPos, TRUE);		
	}	
}


BOOL CPackageBrowseView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	return TRUE;
	
	//return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CPackageBrowseView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	CView::OnMouseMove(nFlags, point);

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{			
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:		
		{
			CPackageView *pView = GetPackageView();
			if (pView)
			{
				if (m_bLButtonDown)
				{		
					int x = m_ptLButtonDown.x;
					int y = m_ptLButtonDown.y;

					int x1 = point.x;
					int y1 = point.y;

					int xd = abs (x - x1);
					int yd = abs (y - y1);

					if ((xd > 5) || (yd > 5))
					{
						if (!m_bDragging)
						{
							// Make sure the current item is selected too
							CPackageListBox *pListBox = GetPackageListBox();

							if (pListBox)
							{
								CWADItem *pWADItem = GetWADItem(m_ptLButtonDown, MB_LEFT_BUTTON);								
								int iItemNumber = GetItemNumber (point);
								m_iFocusItem = iItemNumber;
										
								if (pWADItem)
								{									
									pListBox->SetSel (iItemNumber, TRUE);
									InvalidateRect (NULL, FALSE);
								}
							}

							COleDataSource*	pSource = new COleDataSource();
							CSharedFile	sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);
							
							// Just write out the PackageView's address, the other packageview will take it from there
							
							sf.Write(&pView, sizeof(CPackageView *));
							HGLOBAL hMem = sf.Detach();	
							
							if (!hMem) return;
							pSource->CacheGlobalData(g_iPackageFormat, hMem);	
							pSource->DoDragDrop();

							if (pSource)
							{
								delete pSource;
								pSource = NULL;
							}
							m_bDragging = TRUE;
						}
					}
				}			// if (m_bLButtonDown)
			}				// if (GetPackageView())
		}					// case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		break;

	default:
		ASSERT (FALSE);
		break;
	}	
}

void CPackageBrowseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
			if (m_bZoomMode)
			{
				int iZoomLevel = m_iZoomLevel;
				iZoomLevel++;
				iZoomLevel = min (iZoomLevel, 16);
				
				if (m_iZoomLevel != iZoomLevel)
				{
					m_iZoomLevel = iZoomLevel;
					InvalidateRect (NULL, FALSE);
				}
				GetPackageView()->UpdateStatusBar();
			}
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
			m_ptLButtonDown = point;
			m_iLButtonItem = GetItemNumber (point);
		}
		break;

	default:
		ASSERT (FALSE);
	}

	
	m_bLButtonDown = TRUE;
	m_bDragging = FALSE;
	
	SetCapture();
}

void CPackageBrowseView::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	CView::OnLButtonUp(nFlags, point);

	CPackageDoc *pDoc = GetDocument();	
	
	m_bLButtonDown = FALSE;
	m_bDragging = FALSE;

	ReleaseCapture();

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
			if (pDoc)
			{
				CWADItem *pWADItem = GetWADItem(point, MB_LEFT_BUTTON);
				CPackageView *pFormView = pDoc->GetFormView();
				CPackageListBox *pListBox = pFormView->GetListBox();
				int iItemNumber = GetItemNumber (point);
				m_iFocusItem = iItemNumber;
						
				if (pWADItem)
				{
					BOOL bControlKey = IsKeyPressed( VK_CONTROL);

					if (!bControlKey)
					{
						pListBox->SetSel (-1, FALSE);
					}
					pListBox->SetSel (iItemNumber, TRUE);
					InvalidateRect (NULL, FALSE);
				}
			}
		}
		break;

	default:
		ASSERT (FALSE);		// Unhandled mode?
	}	
}

void CPackageBrowseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CView::OnLButtonDblClk(nFlags, point);

	CPackageDoc *pDoc = GetDocument();	
	
	m_bLButtonDown = FALSE;
	m_bDragging = FALSE;

	ReleaseCapture();

	if ((m_bZoomMode) && (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE))
	{
		int iZoomLevel = m_iZoomLevel;
		iZoomLevel += 2;
		iZoomLevel = min (iZoomLevel, 16);
		
		if (m_iZoomLevel != iZoomLevel)
		{
			m_iZoomLevel = iZoomLevel;
			InvalidateRect (NULL, FALSE);
		}
		GetPackageView()->UpdateStatusBar();
	}
	else
	{
		if (pDoc)
		{
			CPackageView *pFormView = pDoc->GetFormView();
			if (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE)
			{				
				pFormView->OpenSelectedImages(TRUE);
			}				
			else
			{			
				CWADItem *pWADItem = GetWADItem(point, MB_LEFT_BUTTON);				
				CPackageListBox *pListBox = pFormView->GetListBox();
				int iItemNumber = GetItemNumber (point);
						
				if (pWADItem)
				{
					BOOL bControlKey = IsKeyPressed( VK_CONTROL);				

					if (!bControlKey)
					{
						pListBox->SetSel (-1, FALSE);					
					}
					pListBox->SetSel (iItemNumber, TRUE);
					InvalidateRect (NULL, FALSE);			
				}
				
				pFormView->OpenSelectedImages();
			}
		}
	}
}

void CPackageBrowseView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDown(nFlags, point);

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
			if (m_bZoomMode)
			{
				int iZoomLevel = m_iZoomLevel;
				iZoomLevel--;
				iZoomLevel = max (iZoomLevel, 1);
				
				if (m_iZoomLevel != iZoomLevel)
				{
					m_iZoomLevel = iZoomLevel;
					InvalidateRect (NULL, FALSE);
				}
				GetPackageView()->UpdateStatusBar();
			}
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
			m_ptRButtonDown = point;
			m_iRButtonItem = GetItemNumber (point);
		}
		break;

	default:
		ASSERT (FALSE);		// Unhandled mode?
		break;
	}

	m_bRButtonDown = TRUE;
	m_bDragging = FALSE;
	
	SetCapture();
}

void CPackageBrowseView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{	
	CView::OnRButtonDblClk(nFlags, point);

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
			if (m_bZoomMode)
			{
				int iZoomLevel = m_iZoomLevel;
				iZoomLevel -= 2;
				iZoomLevel = max (iZoomLevel, 1);
				
				if (m_iZoomLevel != iZoomLevel)
				{
					m_iZoomLevel = iZoomLevel;
					InvalidateRect (NULL, FALSE);
				}
				GetPackageView()->UpdateStatusBar();
			}
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
		}
		break;

	default:
		ASSERT (FALSE);		// Unhandled mode?
		break;
	}

}

void CPackageBrowseView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnRButtonUp(nFlags, point);

	CPackageDoc *pDoc = GetDocument();	
	
	m_bRButtonDown = FALSE;
	m_bDragging = FALSE;

	ReleaseCapture();

	switch (GetViewMode())
	{
	case PACKAGE_BROWSE_VIEWMODE_TILE:
		{
			if (m_bZoomMode)
			{
				return;
			}
		}
		break;

	case PACKAGE_BROWSE_VIEWMODE_BROWSE:
		{
			if (pDoc)
			{
				CWADItem *pWADItem = GetWADItem(point, MB_RIGHT_BUTTON);
				CPackageView *pFormView = pDoc->GetFormView();
				CPackageListBox *pListBox = pFormView->GetListBox();
				int iItemNumber = GetItemNumber (point);

				BOOL bControlKey = IsKeyPressed( VK_CONTROL);

				if (!bControlKey)
				{
					pListBox->SetSel (-1, FALSE);
				}
						
				if (pWADItem)
				{			
					pListBox->SetSel (iItemNumber, TRUE);			
				}

				InvalidateRect (NULL, FALSE);
			}
		}
		break;

	default:
		ASSERT (FALSE);		// Unhandled mode?
		break;
	}
	
	// Need this point to position the menu, relative to the screen.  
	// The passed-in point is relative to the view window

	CPoint ptScreenPos( point);
	ClientToScreen( &ptScreenPos);
		
	CMenu Menu;
	VERIFY( Menu.LoadMenu( IDR_PACKAGE_BROWSE_POPUP));

	CMenu* pPopup = Menu.GetSubMenu( 0);
	ASSERT( pPopup != NULL);

	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		ptScreenPos.x, ptScreenPos.y, pWndPopupOwner);
	
}

CWADItem *CPackageBrowseView::GetWADItem (CPoint pt, int iButton)
{		
	int iItemNumber = GetItemNumber (pt);
	CPackageView *pFormView = GetPackageView();
	CWADItem *pWADItem = NULL;

	CPackageDoc *pDoc = GetDocument();
	
	if (pFormView)
	{
		CPackageListBox *pListBox = pFormView->GetListBox();
		
		if (pListBox)
		{
			switch (GetViewMode())
			{
			case PACKAGE_BROWSE_VIEWMODE_BROWSE:
				{
					if ((iItemNumber >= pListBox->GetCount()) || (iItemNumber < 0))
					{
						return NULL;
					}

					switch (iButton)
					{
					case MB_LEFT_BUTTON:
						{
							if (iItemNumber == m_iLButtonItem)
							{
								pWADItem = (CWADItem *)(pListBox->GetItemData(iItemNumber));
							}
						}
						break;

					case MB_RIGHT_BUTTON:
						{
							if (iItemNumber == m_iRButtonItem)
							{
								pWADItem = (CWADItem *)(pListBox->GetItemData(iItemNumber));
							}
						}
						break;

					default:
						ASSERT (FALSE);
					}
				}
				break;

			case PACKAGE_BROWSE_VIEWMODE_TILE:
				{
					if (pListBox->GetCount() > 0)
					{
						int iCurSel = pListBox->GetCaretIndex();
						pWADItem = (CWADItem *)(pListBox->GetItemData(iCurSel));
					}
				}
				break;

			default:
				ASSERT (FALSE);
				break;
			}
			
		}
	}

	return pWADItem;	
}

int CPackageBrowseView::GetItemNumber (CPoint pt)
{
	CPackageDoc *pDoc = GetDocument();	

	if (!pDoc)
	{
		return -1;
	}

	int x, y;
	int iRow = 0;
	int iColumn = 0;
	int iOffset = 0;
	int iButtonWidth = m_iBrowseImageSize + 10;
	int iButtonHeight = m_iBrowseImageSize + 14 + 16;

	if ((pt.x > m_iHorzButtons * iButtonWidth) || (pt.y > (m_iVertButtons + 1) * iButtonHeight))
	{
		return -1;
	}
	
	//  Find out where the scrollbar is at
	y = (m_iCurrentScrollPos * iButtonHeight) + pt.y;
	x = pt.x;

	iColumn = x / iButtonWidth;
	iRow = y / iButtonHeight;

	iOffset = iColumn + (iRow * m_iHorzButtons);

	if (iOffset < pDoc->GetNumLumps())
	{
		return iOffset;
	}


	return -1;	
}

CPackageListBox *CPackageBrowseView::GetPackageListBox()
{
	CPackageView *pView = GetPackageView();
	
	if (pView)
	{
		return (pView->GetListBox());
	}

	return NULL;
}

CPackageTreeControl *CPackageBrowseView::GetPackageTreeCtrl()
{
	CPackageView *pView = GetPackageView();
	
	if( pView )
	{
		return( pView->GetTreeCtrl() );
	}

	return NULL;
}

CPackageView *CPackageBrowseView::GetPackageView()
{
	CPackageDoc *pDoc = GetDocument();
	
	if (pDoc)
	{
		return (pDoc->GetFormView());
	}

	return NULL;
}

void CPackageBrowseView::CheckForMipsUpdate (CCmdUI* pCmdUI)
{	
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->CheckForMipsUpdate(pCmdUI);
	}
}

void CPackageBrowseView::OnPackageExport() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->DoPackageExport();	
	}
}

void CPackageBrowseView::OnUpdatePackageExport(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnPackageAdd() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->ImportImages();
	}	
}

void CPackageBrowseView::OnUpdatePackageAdd(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (TRUE);
}

void CPackageBrowseView::OnPackageNewImage() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{					
		pFormView->CreateNewImage();
	}
}

void CPackageBrowseView::OnPackageOpen() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{					
		pFormView->OpenSelectedImages();
	}
}

void CPackageBrowseView::OnUpdatePackageOpen(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
}

void CPackageBrowseView::OnUpdateUseAsLeftBulletSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
}

void CPackageBrowseView::OnUpdateUseAsLeftDecalSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
}

void CPackageBrowseView::OnUpdateUseAsLeftPatternSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
}

void CPackageBrowseView::OnUpdateUseAsRightBulletSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnUpdateUseAsRightDecalSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnUpdateUseAsRightPatternSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnUpdateUseAsRivetSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnUpdateEditRename(CCmdUI* pCmdUI) 
{
	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		CPackageListBox *pListBox = pFormView->GetListBox();
		
		if (pListBox)
		{
			int iSelCount = pListBox->GetSelCount();
			pCmdUI->Enable (iSelCount == 1);
		}
	}
}

void CPackageBrowseView::OnUpdateEditPasteOver(CCmdUI* pCmdUI) 
{
	if ( !::IsClipboardFormatAvailable(CF_DIB) && !::IsClipboardFormatAvailable(g_iBrowseCopyPasteFormat) )
	{
		// Get out of here without even bothering about checking the list
		pCmdUI->Enable(FALSE);
		return;
	}

	BOOL bEnable = FALSE;

	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		CPackageListBox *pListBox = pFormView->GetListBox();
		
		if (pListBox)
		{
			int iIndex = 0;
			int iSelCount = pListBox->GetSelCount();	

			if (iSelCount == 1)
			{		
				pListBox->GetSelItems (1, &iIndex);
				CWADItem *pItem = (CWADItem *)(pListBox->GetItemData (iIndex));		

				bEnable = (pItem->IsValidMip());
			}			
		}
	}
	pCmdUI->Enable(bEnable);
}

void CPackageBrowseView::OnUpdateEditPastePackage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ::IsClipboardFormatAvailable(CF_DIB) || ::IsClipboardFormatAvailable(CF_HDROP) );
}

void CPackageBrowseView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CPackageView *pFormView = GetPackageView();	
	if (pFormView)
	{
		CPackageListBox *pListBox = pFormView->GetListBox();
		
		if (pListBox)
		{
			int iSelCount = pListBox->GetSelCount();
			bEnable = ((iSelCount > 0) && (iSelCount != LB_ERR));			
		}
	}

	pCmdUI->Enable (bEnable);
}

void CPackageBrowseView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageBrowseView::OnUseAsLeftBulletSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_LeftBulletLayerInfo.LoadFromClipboard( FALSE, this);
	}
}

void CPackageBrowseView::OnUseAsLeftDecalSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_LeftDecalToolLayerInfo.LoadFromClipboard( FALSE, this);
	}
}


void CPackageBrowseView::OnUseAsLeftPatternSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_LeftPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
	}
}

void CPackageBrowseView::OnUseAsRightBulletSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_RightBulletLayerInfo.LoadFromClipboard( FALSE, this);
	}
}

void CPackageBrowseView::OnUseAsRightDecalSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_RightBulletLayerInfo.LoadFromClipboard( FALSE, this);
	}
}


void CPackageBrowseView::OnUseAsRightPatternSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_RightPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
	}
}


void CPackageBrowseView::OnUseAsRivetSource() 
{
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int w = pWADItem->GetWidth();
		int h = pWADItem->GetHeight();
		
		CDibSection dsImage;
		dsImage.Init( w, h, 8, pWADItem->GetPalette());
		dsImage.SetRawBits( pWADItem->GetBits());
		dsImage.WriteToClipboard( this);

		g_RivetToolLayerInfo.LoadFromClipboard( FALSE, this);
	}
}


void CPackageBrowseView::OnEditRename() 
{
	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		pFormView->RenameImage();
		InvalidateRect (NULL, FALSE);
	}	
}

void CPackageBrowseView::OnEditPaste() 
{
	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		pFormView->DoPaste (FALSE);
		InvalidateRect (NULL, FALSE);
	}	
}

void CPackageBrowseView::OnEditPasteOver() 
{
	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		pFormView->DoPaste (TRUE);
		InvalidateRect (NULL, FALSE);
	}
}

void CPackageBrowseView::OnEditPastePackage() 
{
	CPackageView *pFormView = GetPackageView();
	if (pFormView)
	{
		pFormView->DoPaste (FALSE);
		InvalidateRect (NULL, FALSE);
	}
}

void CPackageBrowseView::OnEditDelete() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->EditDelete();
	}
}

void CPackageBrowseView::OnEditClear() 
{
	OnEditDelete();	
}

void CPackageBrowseView::OnEditCopy() 
{
	
	CWADItem *pWADItem = GetWADItem(m_ptRButtonDown, MB_RIGHT_BUTTON);

	if (pWADItem)
	{
		int iWidth = pWADItem->GetWidth();
		int iHeight = pWADItem->GetHeight();
		
		BYTE *pbyData = NULL;
		BYTE *pbyPalette = NULL;
		
		pbyData = pWADItem->GetBits(0);
		pbyPalette = pWADItem->GetPalette();

		if (pbyData && pbyPalette)
		{
			CDibSection dsImage;
			dsImage.Init (iWidth, iHeight, 8, pbyPalette);
			dsImage.SetRawBits (pbyData);
			dsImage.WriteToClipboard (this);
		}
	}	
}

void CPackageBrowseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);

	BOOL bReposition = FALSE;
	BOOL bPage = FALSE;
	int iPageAdjust = 0;
	BOOL bTile = (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE) ? TRUE : FALSE;

	CPackageListBox *pListBox = GetPackageListBox();

	if (pListBox)
	{
		m_iFocusItem = pListBox->GetCaretIndex();
	}
		
	switch (nChar)
	{
	case VK_NEXT:
		{
			if (!bTile)
			{
				iPageAdjust = m_iVertButtons;
				m_iFocusItem += m_iVertButtons * m_iHorzButtons;
				
				bReposition = TRUE;
				bPage = TRUE;
			}
		}
		break;

	case VK_PRIOR:
		{
			if (!bTile)
			{
				iPageAdjust = -1 * m_iVertButtons;
				m_iFocusItem -= m_iVertButtons * m_iHorzButtons;
				bReposition = TRUE;
				bPage = TRUE;
			}
		}
		break;

	case VK_LEFT:
		{
			if (!bTile)
			{
				m_iFocusItem -= 1;
				bReposition = TRUE;
			}
		}
		break;

	case VK_RIGHT:
		{
			if (!bTile)
			{
				m_iFocusItem += 1;
				bReposition = TRUE;
			}
		}
		break;

	case VK_DOWN:
		{
			if (!bTile)
			{
				m_iFocusItem += m_iHorzButtons;				
			}
			else
			{
				m_iFocusItem += 1;
			}
			bReposition = TRUE;
		}
		break;

	case VK_UP:
		{
			if (!bTile)
			{
				m_iFocusItem -= m_iHorzButtons;			
			}
			else
			{
				m_iFocusItem -= 1;
			}
			bReposition = TRUE;
		}
		break;

	case VK_RETURN:
		{
			OnPackageOpen();
		}
		break;

	case VK_DELETE:
		{
			OnEditDelete();
		}
		break;
	
	default:
		// Do nothing
		break;
	}


	if (bReposition)
	{		
		if (pListBox)
		{
			m_iFocusItem = max (m_iFocusItem, 0);	
			m_iFocusItem = min (m_iFocusItem, pListBox->GetCount() - 1);

			int iRow = GetScrollPos (SB_VERT);
			int iStartPosition = (iRow * m_iHorzButtons);
			int iEndPosition = (iRow * m_iHorzButtons) + (m_iVertButtons * m_iHorzButtons) - 1;

			if (!bPage)
			{
				// Make sure the currently selected image (m_iFocusItem) is within the scrollbar 
				// range.  If not, move the scrollbar
				if (m_iFocusItem < iStartPosition)
				{
					iRow = (m_iFocusItem / m_iHorzButtons);
				}

				if (m_iFocusItem > iEndPosition)
				{
					iRow = (m_iFocusItem / m_iHorzButtons) + 1 - m_iVertButtons;
				}				
			}
			else
			{
				iRow += iPageAdjust;
			}

			int iNumItems = 0;
			CPackageDoc *pDoc = GetDocument();

			if (pListBox)
			{
				if (::IsWindow(pListBox->m_hWnd))
				{
					iNumItems = pListBox->GetCount();
				}
				else
				{
					if (pDoc)
					{
						iNumItems = pDoc->GetNumImages();
					}
				}
			}
			else
			{
				if (pDoc)
				{
					iNumItems = pDoc->GetNumImages();
				}
			}

			int iNumImageRows = (iNumItems + m_iHorzButtons) / m_iHorzButtons;

			iRow = max (iRow, 0);
			iRow = min (iRow, iNumImageRows);

			SetScrollPos (SB_VERT, iRow, TRUE);
			m_iCurrentScrollPos = iRow;
			
			CWADItem *pWADItem = NULL;
			pWADItem = (CWADItem *)(pListBox->GetItemData(m_iFocusItem));

			if (pWADItem)
			{
				BOOL bControlKey = IsKeyPressed( VK_CONTROL);

				if (!bControlKey)
				{
					pListBox->SetSel (-1, FALSE);
				}
				pListBox->SetSel (m_iFocusItem, TRUE);
				pListBox->SetCaretIndex (m_iFocusItem);
				
				InvalidateRect (NULL, FALSE);
			}
		}
		
		GetPackageView()->UpdateStatusBar();
	}
}

void CPackageBrowseView::SetZoomMode (BOOL bZoom /* = TRUE */)
{
	m_bZoomMode = bZoom;
}

BOOL CPackageBrowseView::IsInZoomMode()
{
	return m_bZoomMode;
}

void CPackageBrowseView::OnTpZoom()
{
	SetZoomMode (TRUE);
}

void CPackageBrowseView::OnUpdateTpZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE);
	pCmdUI->SetCheck( m_bZoomMode);
}

void CPackageBrowseView::OnTpSelection() 
{
	SetZoomMode (FALSE);
}

void CPackageBrowseView::OnUpdateTpSelection(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE);	
	pCmdUI->SetCheck( !m_bZoomMode);
}


BOOL CPackageBrowseView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		GetPackageView()->UpdateStatusBar();	
		if ((m_bZoomMode) && (GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE))
		{
			::SetCursor(AfxGetApp()->LoadCursor( IDC_ZOOM_CURSOR));
			return TRUE;
		}
		else
		{				
			::SetCursor( ::LoadCursor( NULL, IDC_ARROW));
			return TRUE;
		}		
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);	
	
}




void CPackageBrowseView::OnPackageRemipAll() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->ReMipAll();
	}
	
}

void CPackageBrowseView::OnPackageRemipSelected() 
{
	CPackageView *pFormView = GetPackageView();

	if (pFormView)
	{
		pFormView->RemipSelected();
	}	
}

void CPackageBrowseView::OnUpdatePackageRemipSelected(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
	
}
