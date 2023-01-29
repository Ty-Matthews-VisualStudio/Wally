// PackageView.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyPal.h"
#include "MainFrm.h"
#include "PackageView.h"
#include "RenameImageDlg.h"
#include "ClipboardDIB.h"
#include "ReMip.h"
//#include "ProgressBar.h"
#include "PackageExportDlg.h"
#include "BatchSummaryDlg.h"
#include "WallyDoc.h"
#include "Layer.h"
#include "ColorOpt.h"
#include "DuplicateNameDlg.h"
#include "PackageReMipThread.h"
#include "NewPackageImageDlg.h"
#include "HalfLifePaletteDlg.h"
#include "MiscFunctions.h"
#include "BrowserCacheList.h"
#include "BrowseView.h"
#include "PackageBrowseView.h"
#include "Filter.h"

#include <list>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

#define IsKeyPressed( vk) (GetKeyState( vk) < 0)

/////////////////////////////////////////////////////////////////////////////
// CPackageView

IMPLEMENT_DYNCREATE(CPackageView, CFormView)

CPackageView::CPackageView()
	: CFormView(CPackageView::IDD)
{
	//{{AFX_DATA_INIT(CPackageView)
	m_strFilterLimit = _T("");
	m_iThumbnailSize = 0;
	//}}AFX_DATA_INIT
	m_bFirstTime = true;
	m_bItemIsMip = false;
	m_iMipNumber = 0;
	m_pbyOutputData = NULL;
	m_iOutputDataSize = 0;
	m_bInitialUpdate = false;

	m_iLastWADType = WAD2_TYPE - 1000;

	m_imgList.m_hImageList = NULL;
	VERIFY(m_imgList.Create(IDB_PACKAGEDOC_ICONS, 16, 1, RGB (255, 0, 255)));

	m_iFilterTimerID = 0;
	m_iFilterTimerTicks = 0;
	m_iListBoxTimerID = 0;
	m_iListBoxTimerTicks = 0;	
	m_iSelectionTimerID = 0;
	m_iSelectionTimerTicks = 0;


	m_bFilter = false;
	m_iDragType = PV_DRAG_UNKNOWN;

	m_pDragSourcePackage = NULL;
	m_pbyClipboardBuffer = NULL;

	m_iReMipFlag = -1;
	m_pReMipThread = NULL;
	m_pReMipProgressBar = NULL;	

	m_brDlgColor.CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
	
	m_iOldRCWidth = 0;
	m_iOldRCHeight = 0;

	m_pBrowseView = NULL;	
}

CPackageView::~CPackageView()
{
	if (m_pbyOutputData)
	{
		delete [] m_pbyOutputData;	// neal - must use array delete to delete arrays!
		m_pbyOutputData = NULL;
	}
	if (m_pbyClipboardBuffer)
	{
		delete [] m_pbyClipboardBuffer;
		m_pbyClipboardBuffer = NULL;
	}

	m_bmRandom.DeleteObject();
	m_bmAnimate.DeleteObject();
	m_bmTileMode.DeleteObject();
	m_bmBrowseMode.DeleteObject();
}

void CPackageView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackageView)
	DDX_Control(pDX, IDC_SLIDER_THUMBNAIL_SIZE, m_slThumbnailSize);
	DDX_Control(pDX, IDC_STATIC_THUMB_SIZE, m_stThumbnailSize);
	//DDX_Control(pDX, IDC_TREE_IMAGES, m_tcImages);
	DDX_Control(pDX, IDC_PUSH_ANIMATE, m_btnAnimate);
	DDX_Control(pDX, IDC_PUSH_RANDOM, m_btnRandom);
	DDX_Control(pDX, IDC_PUSH_BROWSE_MODE, m_btnBrowseMode);
	DDX_Control(pDX, IDC_PUSH_TILE_MODE, m_btnTileMode);
	DDX_Control(pDX, IDC_BUTTON_SELECT, m_btnSelectAll);
	DDX_Control(pDX, IDC_CHECK_FILTER, m_ckFilter);
	DDX_Control(pDX, IDC_EDIT_FILTER, m_edFilter);
	DDX_Control(pDX, IDC_LIST_IMAGES, m_lbImages);
	DDX_Text(pDX, IDC_EDIT_FILTER, m_strFilterLimit);
	DDV_MaxChars(pDX, m_strFilterLimit, 15);
	DDX_Slider(pDX, IDC_SLIDER_THUMBNAIL_SIZE, m_iThumbnailSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPackageView, CFormView)
	//{{AFX_MSG_MAP(CPackageView)
	ON_REGISTERED_MESSAGE(WM_PACKAGELISTBOX_CUSTOM, OnPackageListBoxCustomMessage)
	ON_REGISTERED_MESSAGE(WM_PACKAGETREECONTROL_CUSTOM, OnPackageTreeControlCustomMessage)
	ON_LBN_SELCHANGE(IDC_LIST_IMAGES, OnSelchangeListImages)
	ON_WM_PAINT()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)	
	ON_LBN_DBLCLK(IDC_LIST_IMAGES, OnDblclkListImages)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_PACKAGE_OPEN, OnPackageOpen)
	ON_COMMAND(ID_PACKAGE_ADD, OnPackageAdd)
	ON_COMMAND(ID_EDIT_PASTE_PACKAGE, OnEditPastePackage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_PACKAGE, OnUpdateEditPastePackage)	
	ON_COMMAND(ID_EDIT_PASTE_OVER, OnEditPasteOver)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_OVER, OnUpdateEditPasteOver)
	ON_COMMAND(ID_EDIT_ZOOMIN, OnEditZoomin)
	ON_COMMAND(ID_EDIT_ZOOMOUT, OnEditZoomout)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_FILE_BATCH_CONVERSION, OnFileBatchConversion)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_OPEN, OnUpdatePackageOpen)
	ON_COMMAND(ID_PACKAGE_EXPORT, OnPackageExport)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_EXPORT, OnUpdatePackageExport)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_CHECK_FILTER, OnCheckFilter)
	ON_BN_DOUBLECLICKED(IDC_CHECK_FILTER, OnDoubleclickedCheckFilter)
	ON_EN_UPDATE(IDC_EDIT_FILTER, OnUpdateEditFilter)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_COMMAND(ID_PACKAGE_DRAG_CANCEL, OnPackageDragCancel)
	ON_COMMAND(ID_PACKAGE_DRAG_COPY, OnPackageDragCopy)
	ON_COMMAND(ID_PACKAGE_DRAG_MOVE, OnPackageDragMove)
	ON_COMMAND(ID_USE_AS_RIVET_SOURCE, OnUseAsRivetSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIVET_SOURCE, OnUpdateUseAsRivetSource)
	ON_COMMAND(ID_USE_AS_LEFT_BULLET_SOURCE, OnUseAsLeftBulletSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_BULLET_SOURCE, OnUpdateUseAsLeftBulletSource)
	ON_COMMAND(ID_USE_AS_RIGHT_BULLET_SOURCE, OnUseAsRightBulletSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_BULLET_SOURCE, OnUpdateUseAsRightBulletSource)
	ON_COMMAND(ID_USE_AS_LEFT_PATTERN_SOURCE, OnUseAsLeftPatternSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_PATTERN_SOURCE, OnUpdateUseAsLeftPatternSource)
	ON_COMMAND(ID_USE_AS_RIGHT_PATTERN_SOURCE, OnUseAsRightPatternSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_PATTERN_SOURCE, OnUpdateUseAsRightPatternSource)
	ON_COMMAND(ID_USE_AS_LEFT_DECAL_SOURCE, OnUseAsLeftDecalSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_LEFT_DECAL_SOURCE, OnUpdateUseAsLeftDecalSource)
	ON_COMMAND(ID_USE_AS_RIGHT_DECAL_SOURCE, OnUseAsRightDecalSource)
	ON_UPDATE_COMMAND_UI(ID_USE_AS_RIGHT_DECAL_SOURCE, OnUpdateUseAsRightDecalSource)
	ON_COMMAND(ID_PACKAGE_REMIP_ALL, OnPackageRemipAll)
	ON_COMMAND(ID_PACKAGE_REMIP_SELECTED, OnPackageRemipSelected)
	ON_COMMAND(ID_PACKAGE_NEW_IMAGE, OnPackageNewImage)
	ON_UPDATE_COMMAND_UI(ID_TP_SELECTION, OnUpdateTpSelection)
	ON_UPDATE_COMMAND_UI(ID_TP_ZOOM, OnUpdateTpZoom)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_PUSH_ANIMATE, OnPushAnimate)
	ON_BN_CLICKED(IDC_PUSH_BROWSE_MODE, OnPushBrowseMode)
	ON_BN_CLICKED(IDC_PUSH_RANDOM, OnPushRandom)
	ON_BN_CLICKED(IDC_PUSH_TILE_MODE, OnPushTileMode)
	ON_COMMAND(ID_TP_ZOOM, OnTpZoom)
	ON_COMMAND(ID_TP_SELECTION, OnTpSelection)
	ON_COMMAND(ID_PACKAGE_RESIZE, OnPackageResize)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_RESIZE, OnUpdatePackageResize)
	ON_UPDATE_COMMAND_UI(ID_PACKAGE_REMIP_SELECTED, OnUpdatePackageRemipSelected)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_IMAGES, OnSelchangedTreeImages)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_THUMBNAIL_SIZE, OnReleasedcaptureSliderThumbnailSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackageView diagnostics

#ifdef _DEBUG
void CPackageView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPackageView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackageView message handlers

void CPackageView::OnInitialUpdate() 
{
	DragAcceptFiles (TRUE);
	EnableToolTips(TRUE);

	m_oleDropTarget.Register (this);
	CFormView::OnInitialUpdate();
	
	bool bAtLeastOne = false;
	int iPosition = 0;
	int iItemAdded = 0;	

	CString strAddString("");
	CString strText ("");
	CPackageDoc *pDoc = GetDocument();

	m_bmRandom.LoadBitmap (IDB_PUSH_RANDOM);
	m_btnRandom.SetBitmap ((HBITMAP)m_bmRandom);

	m_bmAnimate.LoadBitmap (IDB_PUSH_ANIMATE);
	m_btnAnimate.SetBitmap ((HBITMAP)m_bmAnimate);

	m_bmTileMode.LoadBitmap (IDB_PUSH_TILE_MODE);
	m_btnTileMode.SetBitmap ((HBITMAP)m_bmTileMode);

	m_bmBrowseMode.LoadBitmap (IDB_PUSH_BROWSE_MODE);
	m_btnBrowseMode.SetBitmap ((HBITMAP)m_bmBrowseMode);

	// Set up the Tree Control
	//m_tcImages.OnInitialUpdate();

	if( pDoc )
	{
		m_pBrowseView = pDoc->GetBrowseView();
	
		CWADItem *pItem = NULL;		
		
		pItem = pDoc->GetFirstLump();
		while (pItem)
		{
			strAddString = pItem->GetName();
			iItemAdded = m_lbImages.AddString (strAddString);			
			m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
			pItem->SetListBoxIndex (iItemAdded);

			//m_tcImages.AddToAllImages( pItem );
			
			pItem = pDoc->GetNextLump();
			bAtLeastOne = true;
		}
	}

	strText = "Filter:";
	SetDlgItemText (IDC_CHECK_FILTER, strText);

	if (bAtLeastOne)
	{
	/*	int w = 0;
		int h = 0;
		unsigned char *pBits = NULL;
		unsigned char *pPalette = NULL;
		int iType = 0;*/
		

		m_lbImages.SetSel (0, true);
/*		pItem = (CWADItem *)(m_lbImages.GetItemData (0));
		
		if (pItem->IsValidMip())
		{
			pBits = pItem->GetBits (0);
			pPalette = pItem->GetPalette ();
			w = pItem->GetWidth();
			h = pItem->GetHeight();
			strText.Format ("Width: %d\tHeight: %d", w, h);
		}
		
		m_wndPreview.InitImage (w, h, pBits, pPalette);
		*/
	}
	
	CheckDlgButton (IDC_PUSH_BROWSE_MODE, g_iPackageViewMode == PACKAGE_BROWSE_VIEWMODE_BROWSE);
	CheckDlgButton (IDC_PUSH_TILE_MODE, g_iPackageViewMode == PACKAGE_BROWSE_VIEWMODE_TILE);
	
	m_edFilter.EnableWindow (false);		
	OnSelchangeListImages();

	// Set up the slider
	m_slThumbnailSize.SetRange( 1, 8 );
	m_slThumbnailSize.SetPos( g_iPackageThumbnailSize );
	
	m_pBrowseView = pDoc->GetBrowseView();
	if( m_pBrowseView )
	{
		m_pBrowseView->SetBrowseImageSize( g_iPackageThumbnailSize * 32 );
	}
	m_bInitialUpdate = true;
}

LRESULT CPackageView::OnPackageListBoxCustomMessage(WPARAM nType, LPARAM nFlags)
{
	switch (nType)
	{
	case PACKAGELISTBOX_SELCHANGE:
		// Start up a timer
		if (!m_iListBoxTimerID)
		{
			m_iListBoxTimerID = SetTimer( TIMER_PV_ID_LISTBOX, 100, NULL);
		}
		else
		{
			m_iListBoxTimerTicks = 0;
		}		
		OnSelchangeListImages();
		break;

	case PACKAGELISTBOX_OPENTEXTURE:
		OnPackageOpen();
		break;

	default:
		ASSERT (false);		// Unhandled type?
		break;
	}
	return 0;
}

LRESULT CPackageView::OnPackageTreeControlCustomMessage( WPARAM nType, LPARAM nFlags )
{
	switch (nType)
	{
	case PACKAGETREECONTROL_SELCHANGE:
		// Start up a timer
		if( !m_iTreeCtrlTimerID )
		{
			m_iTreeCtrlTimerID = SetTimer( TIMER_PV_ID_TREECTRL, 100, NULL );
		}
		else
		{
			m_iTreeCtrlTimerTicks = 0;
		}
		OnSelchangeListImages();
		break;

	case PACKAGETREECONTROL_OPENTEXTURE:
		OnPackageOpen();
		break;

	default:
		ASSERT (false);		// Unhandled type?
		break;
	}
	return 0;
}

void CPackageView::OnSelchangeListImages()
{	
	CPackageDoc *pDoc = GetDocument();
	CWADItem *pItem = NULL;
	
	int iSelCount = m_lbImages.GetSelCount();
	int j = 0;

	CString strText ("");
	if (iSelCount == 0)
	{
		m_bItemIsMip = false;
		return;
	}

	if (m_pBrowseView)
	{
		if (m_pBrowseView->GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE)
		{
			m_pBrowseView->InvalidateRect (NULL, FALSE);
		}
		else
		{
			if (!m_iSelectionTimerID)
			{
				m_iSelectionTimerID = SetTimer( TIMER_PV_ID_LISTBOX_SELECTION, 100, NULL);
			}
			else
			{
				m_iSelectionTimerTicks = 0;
			}
		}
	}
	
	int iIndex = m_lbImages.GetCaretIndex();	

	int w = 0;
	int h = 0;
	int w1 = 0;
	int h1 = 0;	
	int iMip = 0;
	unsigned char *pBits = NULL;
	unsigned char *pPalette = NULL;
	int iType = 0;		
	bool bFoundMatch = false;

	pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));

	if (!pItem)
	{
		return;
	}

	if (pItem->IsValidMip())
	{
		m_bItemIsMip = true;
		w = pItem->GetWidth();
		h = pItem->GetHeight();

		strText.Format ("Width: %d\tHeight: %d", w, h);
		
		pPalette = pItem->GetPalette ();

#ifdef _DEBUG		
		// Let the coders see each mip
		pBits = pItem->GetBits (m_iMipNumber);		
		int iDiv = (1 << m_iMipNumber);
		w = w / iDiv;
		h = h / iDiv;
#else
		// Find the best fit to 256x256		
		
		for (iMip = 0; (iMip < 4) && !bFoundMatch; iMip++)
		{
			int iDiv = (1 << iMip);
			w1 = w / iDiv;
			h1 = h / iDiv;

			if ((w1 <= 256) && (h1 <= 256))
			{
				w = w1;
				h = h1;
				bFoundMatch = true;
			}
		}

		if (!bFoundMatch)
		{
			iMip = 3;			
		}
		else
		{
			iMip--;
		}

		
		pBits = pItem->GetBits (iMip);
#endif

	}
	else
	{
		strText = "";
		m_bItemIsMip = false;
		pBits = NULL;
		pPalette = NULL;
		w = 0;
		h = 0;
	}
	
	UpdateStatusBar();
}

BOOL CPackageView::OnEraseBkgnd( CDC* pDC)
{
	return TRUE;
}

void CPackageView::UpdateStatusBar() 
{
	if (m_lbImages.GetCount() > 0)
	{
		int iFocusItem = m_lbImages.GetCaretIndex();

		iFocusItem = max (iFocusItem, 0);	
		iFocusItem = min (iFocusItem, m_lbImages.GetCount() - 1);

		CWADItem *pWADItem = NULL;
		pWADItem = (CWADItem *)(m_lbImages.GetItemData(iFocusItem));

		if (pWADItem)
		{
			int iWidth = pWADItem->GetWidth();
			int iHeight = pWADItem->GetHeight();
			CString strText("");

			// Num Items
			int iCount = m_lbImages.GetCount();
			strText.Format ("%d item%s %s", iCount, iCount == 1 ? "" : "s", m_bFilter ? "(filtered)" : "");
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), strText, true);
					
			// Name
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), pWADItem->GetName(), true);

			// Item width and height
			strText.Format ("Size: %d x %d", iWidth, iHeight);
			m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), strText, true);

			// Zoom Value
			if (m_pBrowseView)
			{			
				strText.Format ("Zoom: %d%%", m_pBrowseView->GetZoomLevel() * 100);
			}
			else
			{
				strText = "";
			}

			m_wndStatusBar.SetPaneText (m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), strText, true);		
		}	
		else	          
		{
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_XYPOS), "",true);
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), "", true);
			m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), "", true);		
		}		
	}
}

void CPackageView::OnPaint() 
{
	if (!m_bInitialUpdate)
	{
		return;
	}
	if( m_bFirstTime )
	{		
		m_bFirstTime = FALSE;		
	}	

	CPaintDC PaintDC(this); // device context for painting

	RECT rcClient;
	GetClientRect(&rcClient);
	CRect rcLB(rcClient);
	CRect rcED(rcClient);
	CRect rcST(rcClient);
		
	int iRCWidth = rcLB.Width();
	int iRCHeight = rcLB.Height();

	BOOL bReposition = FALSE;

	if ((m_iOldRCWidth != iRCWidth) || (m_iOldRCHeight != iRCHeight))
	{
		m_iOldRCWidth = iRCWidth;
		m_iOldRCHeight = iRCHeight;
		bReposition = TRUE;
	}

	CDC DC;
	DC.CreateCompatibleDC(&PaintDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap(&PaintDC, iRCWidth, iRCHeight);
	DC.SelectObject(Bmp);

	WINDOWPLACEMENT wpLB;
	wpLB.length = sizeof(WINDOWPLACEMENT);

	WINDOWPLACEMENT wpED;
	wpED.length = sizeof(WINDOWPLACEMENT);

	WINDOWPLACEMENT wpCK;
	wpCK.length = sizeof(WINDOWPLACEMENT);

	WINDOWPLACEMENT wpTMP;
	wpTMP.length = sizeof(WINDOWPLACEMENT);

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);

	int iHeight = 0;
	int iWidth = 0;

	if (bReposition)
	{
		m_lbImages.GetWindowPlacement(&wpLB);
		wpLB.rcNormalPosition.left = rcLB.left + PFORMVIEW_LB_OFFSET_LEFT;
		wpLB.rcNormalPosition.right = rcLB.right - PFORMVIEW_LB_OFFSET_RIGHT;
		wpLB.rcNormalPosition.top = rcLB.top + PFORMVIEW_LB_OFFSET_TOP;
		wpLB.rcNormalPosition.bottom = max((rcLB.bottom - PFORMVIEW_LB_OFFSET_BOTTOM), (wpLB.rcNormalPosition.top + 2 + 16));
		m_lbImages.SetWindowPlacement(&wpLB);
	}

	m_lbImages.GetWindowPlacement(&wpLB);

	HBRUSH hOldBrush = (HBRUSH)SelectObject(DC, (HBRUSH)m_brDlgColor);

	::PatBlt(DC, 0, 0, iRCWidth, iRCHeight, PATCOPY);

	int iB = wpLB.rcNormalPosition.bottom;
	int iT = wpLB.rcNormalPosition.top;
	int iL = wpLB.rcNormalPosition.left;
	int iR = wpLB.rcNormalPosition.right;

	// clear background, around our listbox (eliminates flashing)
	//PaintDC.BitBlt( 0, 0, iRCWidth, iRCHeight, &DC, 0, 0, SRCCOPY);	
	PaintDC.BitBlt(0, 0, iRCWidth, iT, &DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt(0, 0, iL, iRCHeight, &DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt(iR, 0, iRCWidth - iR, iRCHeight, &DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt(iL, iB, iR - iL, iRCHeight - iB, &DC, 0, 0, SRCCOPY);

	DC.SelectStockObject(NULL_BRUSH);
	SelectObject(DC, hOldBrush);

	DC.DeleteDC();

	if (bReposition)
	{
		m_btnSelectAll.GetWindowPlacement(&wp);
		iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		wp.rcNormalPosition.right = wpLB.rcNormalPosition.right;
		wp.rcNormalPosition.left = wp.rcNormalPosition.right - iWidth;
		wp.rcNormalPosition.bottom = wpLB.rcNormalPosition.top - 6;
		wp.rcNormalPosition.top = wp.rcNormalPosition.bottom - iHeight;
		m_btnSelectAll.SetWindowPlacement(&wp);

		m_edFilter.GetWindowPlacement(&wpED);
		iHeight = wpED.rcNormalPosition.bottom - wpED.rcNormalPosition.top;
		wpED.rcNormalPosition.left = rcED.left + PFORMVIEW_ED_OFFSET_LEFT;
		wpED.rcNormalPosition.right = wpLB.rcNormalPosition.right;
		wpED.rcNormalPosition.top = wpLB.rcNormalPosition.bottom + 6;
		wpED.rcNormalPosition.bottom = wpED.rcNormalPosition.top + iHeight;
		m_edFilter.SetWindowPlacement(&wpED);

		m_ckFilter.GetWindowPlacement(&wpCK);
		iHeight = wpCK.rcNormalPosition.bottom - wpCK.rcNormalPosition.top;
		wpCK.rcNormalPosition.left = rcLB.left + PFORMVIEW_LB_OFFSET_LEFT;
		wpCK.rcNormalPosition.right = wpED.rcNormalPosition.left - 4;
		wpCK.rcNormalPosition.top = wpED.rcNormalPosition.top + 2;
		wpCK.rcNormalPosition.bottom = wpCK.rcNormalPosition.top + iHeight;
		m_ckFilter.SetWindowPlacement(&wpCK);

		m_btnRandom.GetWindowPlacement(&wp);
		wp.rcNormalPosition.left = rcLB.left + PFORMVIEW_LB_OFFSET_LEFT;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + 25;
		wp.rcNormalPosition.top = wpCK.rcNormalPosition.bottom + 10;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + 25;
		m_btnRandom.SetWindowPlacement(&wp);

		wp.rcNormalPosition.left = wp.rcNormalPosition.right + 1;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + 25;
		m_btnAnimate.SetWindowPlacement(&wp);

		wp.rcNormalPosition.left = wp.rcNormalPosition.right + 1;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + 25;
		m_btnTileMode.SetWindowPlacement(&wp);

		wp.rcNormalPosition.left = wp.rcNormalPosition.right + 1;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + 25;
		m_btnBrowseMode.SetWindowPlacement(&wp);

		m_btnRandom.GetWindowPlacement(&wp);
		m_stThumbnailSize.GetWindowPlacement(&wpTMP);  // Get current dimensions for width
		wp.rcNormalPosition.left = rcLB.left + PFORMVIEW_LB_OFFSET_LEFT;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + (wpTMP.rcNormalPosition.right - wpTMP.rcNormalPosition.left);
		wp.rcNormalPosition.top = wp.rcNormalPosition.bottom + 5; // +5 for the padding
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (wpTMP.rcNormalPosition.bottom - wpTMP.rcNormalPosition.top);
		m_stThumbnailSize.SetWindowPlacement(&wp);		

		m_stThumbnailSize.GetWindowPlacement(&wp);
		m_slThumbnailSize.GetWindowPlacement(&wpTMP);  // Get current dimensions for width
		iHeight = wpTMP.rcNormalPosition.bottom - wpTMP.rcNormalPosition.top;
		wpTMP.rcNormalPosition.top = wp.rcNormalPosition.bottom + 5;
		wpTMP.rcNormalPosition.bottom = wpTMP.rcNormalPosition.top + iHeight;
		wpTMP.rcNormalPosition.left = rcLB.left + PFORMVIEW_LB_OFFSET_LEFT;
		wpTMP.rcNormalPosition.right = wpTMP.rcNormalPosition.left + (iR - iL);
		m_slThumbnailSize.SetWindowPlacement(&wpTMP);
		
		//m_slThumbnailSize;
		//m_stThumbnailSize;
	}

	HICON hIcon = NULL;
	HICON hPrevIcon = NULL;

	CPackageDoc* pDoc = GetDocument();

	if (pDoc)
	{
		int iType = pDoc->GetWADType();

		if (iType != m_iLastWADType)
		{
			m_iLastWADType = iType;

			switch (iType)
			{
			case WAD3_TYPE:
				hIcon = m_imgList.ExtractIcon(0);
				break;

			case WAD2_TYPE:
				hIcon = m_imgList.ExtractIcon(1);
				break;

			default:
				hIcon = m_imgList.ExtractIcon(0);
				break;
			}

			if (hIcon)
			{
				CWnd* pParent = GetParent();
				if (pParent)
				{
					hPrevIcon = (HICON)pParent->SendMessage(WM_SETICON,
						(WPARAM)ICON_SMALL, (LPARAM)hIcon);

					// Free the previous icon resource
					if (hPrevIcon)
					{
						DestroyIcon(hPrevIcon);
					}
				}
			}
			else
			{
				TRACE("Unable to create the icon!\n");
			}
		}
	}
}

BOOL CPackageView::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    
	UINT nID =pNMHDR->idFrom;
    
	if (pTTT->uFlags & TTF_IDISHWND)
	{
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
		if(nID)
		{
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
			return(TRUE);
        }
	}
	return(FALSE);
}

void CPackageView::Update()
{	
	// Invalidate the IDC_BUTTON_IMAGE_GUI button so that it hides itself
	
	/*
	CWnd* pWnd = GetDlgItem( IDC_BUTTON_IMAGE_GUI);
	if (pWnd)
	{		
		if (pWnd->m_hWnd)
			pWnd->InvalidateRect( NULL, FALSE);
	}
	else
	{
		ASSERT( FALSE);
	}
	*/

	// Invalidate our custom CHalfLifePaletteWnd class
	/*
	if (m_bFirstTime)
	{		
		CWnd* pWnd = GetDlgItem(IDC_BUTTON_IMAGE_GUI);

		if (pWnd)
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			pWnd->GetWindowRect( &Rect);
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
			pWnd->ShowWindow( SW_HIDE);

			m_wndPreview.Create( NULL, NULL, 
				WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);			
			CWnd* pParent = GetParent();	
		}
		m_bFirstTime = false;
		if (m_wndPreview.m_hWnd)
			m_wndPreview.InvalidateRect( NULL, FALSE);		
	}
	else
	{
		if (m_wndPreview.m_hWnd)
			m_wndPreview.InvalidateRect( NULL, FALSE);
	}
	*/
	
}

void CPackageView::OnEditCopy() 
{
	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init (w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits (m_wndPreview.GetBits());
	dsImage.WriteToClipboard (this);
	*/
}

void CPackageView::CheckForMipsUpdate (CCmdUI* pCmdUI)
{
	int iSelCount = m_lbImages.GetSelCount();
	if ((iSelCount == 0) || (iSelCount == LB_ERR))
	{
		pCmdUI->Enable (false);
		return;
	}
	
	int *piIndexes = new int[iSelCount];
	CWADItem *pItem = NULL;
		
	m_lbImages.GetSelItems (iSelCount, piIndexes);
	for (int j = 0; j < iSelCount; j++)
	{
		// At least one of the selected items has to be a mip
		pItem = (CWADItem *)(m_lbImages.GetItemData(piIndexes[j]));
	
		if (pItem->IsValidMip())
		{			
			pCmdUI->Enable (true);

			if (piIndexes)
			{
				delete [] piIndexes;	// neal - must use array delete to delete arrays!
				piIndexes = NULL;
			}
			return;
		}
	}	

	pCmdUI->Enable (false);
	if (piIndexes)
	{
		delete [] piIndexes;	// neal - must use array delete to delete arrays!
		piIndexes = NULL;
	}
}

void CPackageView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);
}

void CPackageView::RenameImage (CWallyDoc *pWallyDoc)
{
	CPackageDoc *pDoc = GetDocument();
	CRenameImageDlg dlgRename;
	CString strName ("");
	
	dlgRename.SetMaxLength(15);
	dlgRename.SetName (pWallyDoc->GetPackageMipName());
	
	int iCount = m_lbImages.GetCount();
	int iPosition	= 0;
	int iItemAdded  = 0;
	CWADItem *pItem = NULL;
	int j = 0;
	bool bFinished = false;

	for (j = 0; j < iCount; j++)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData (j));

		if (pItem->GetWallyDoc() == pWallyDoc)
		{
			while (!bFinished)
			{
				if (dlgRename.DoModal() == IDOK)
				{	
					strName = dlgRename.GetName();

					if (pDoc->IsNameInList (strName))
					{
						AfxMessageBox ("That name is already in the list.  Please enter a unique name.", MB_ICONSTOP);
					}
					else
					{
						pItem->SetName (strName);
					
						m_lbImages.DeleteString (j);
						iItemAdded = m_lbImages.AddString (strName);
						
						m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
						pItem->SetListBoxIndex (iItemAdded);

						m_lbImages.SetSel (iItemAdded, true);
						m_lbImages.SetTopIndex (iItemAdded);
						SetDlgItemText (IDC_STATIC_NAME, strName);
		
						if (pWallyDoc)
						{						
							pWallyDoc->SetName (strName);
						}					
						bFinished = true;
					}
				}
				else
				{
					bFinished = true;
				}
			}
			OnSelchangeListImages();
			return;
		}
	}
}

void CPackageView::OnEditRename() 
{
	RenameImage();
}

void CPackageView::RenameImage()
{
	CPackageDoc *pDoc = GetDocument();
	//m_tcImages.RenameSelectedItem( pDoc );
	
#if 1
	CRenameImageDlg dlgRename;
	CString strName ("");

	int iSelCount = m_lbImages.GetSelCount();
	//int iSelCount = m_tcImages.GetSelectedCount();
	ASSERT (iSelCount == 1);		// OnEditRename() should not be possible with multiple items selected!

	int iIndex		= 0;
	int iPosition	= 0;
	int iItemAdded  = 0;
	CWADItem *pItem = NULL;
	CWallyDoc *pWallyDoc = NULL;
	
	m_lbImages.GetSelItems (1, &iIndex);
	pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));
	//pItem = m_tcImages.GetSelectedImage();
	strName = pItem->GetName();

	dlgRename.SetMaxLength( 15 );
	dlgRename.SetName( strName );

	bool bFinished = false;

	while (!bFinished)
	{
		if (dlgRename.DoModal() == IDOK)
		{
			strName = dlgRename.GetName();

			if (pDoc->IsNameInList (strName))
			{
				AfxMessageBox ("That name is already in the list.  Please enter a unique name.", MB_ICONSTOP);
			}
			else
			{		
				pItem->SetName (strName);
				
				m_lbImages.DeleteString (iIndex);
				iItemAdded = m_lbImages.AddString (strName);
				pItem->SetListBoxIndex (iItemAdded);
				
				m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
				m_lbImages.SetSel (iItemAdded, true);
				m_lbImages.SetTopIndex (iItemAdded);
				SetDlgItemText (IDC_STATIC_NAME, strName);

				pWallyDoc = pItem->GetWallyDoc ();

				if (pWallyDoc)
				{
					pWallyDoc->SetTitle (strName);
				}
				pWallyDoc = NULL;

				pDoc->SetModifiedFlag (true);
				bFinished = true;
			}
		}	
		else
		{
			bFinished = true;
		}
	}
#endif
}

void CPackageView::OnUpdateEditRename(CCmdUI* pCmdUI) 
{
	int iSelCount = m_lbImages.GetSelCount();
	//int iSelCount = m_tcImages.GetSelectedCount();
	pCmdUI->Enable (iSelCount == 1);
}

void CPackageView::OnDblclkListImages() 
{
	OnPackageOpen();	
}

void CPackageView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CFormView::OnLButtonDblClk(nFlags, point);
	OnPackageOpen();
}

void CPackageView::OnRButtonUp( UINT uFlags, CPoint ptPos)
{
	CFormView::OnRButtonUp( uFlags, ptPos);	
	ShowRightButtonPopup(ptPos);
}

void CPackageView::ShowRightButtonPopup(CPoint ptPos)
{
	// Need this point to position the menu, relative to the screen.  
	// The passed-in point is relative to the view window

	CPoint ptScreenPos( ptPos);
	ClientToScreen( &ptScreenPos);
		
	CMenu Menu;
	VERIFY( Menu.LoadMenu( IDR_PACKAGE_POPUP));

	CMenu* pPopup = Menu.GetSubMenu( 0);
	ASSERT( pPopup != NULL);

	CWnd* pWndPopupOwner = this;

	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		ptScreenPos.x, ptScreenPos.y, pWndPopupOwner);
}

void CPackageView::OnUpdatePackageOpen(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate (pCmdUI);
}

void CPackageView::OnPackageOpen() 
{
	OpenSelectedImages();
}

#if 1
void CPackageView::OpenSelectedImages(BOOL bCaretOnly /* = FALSE */)
{
	int iSelCount = m_lbImages.GetSelCount();
	if ((iSelCount == 0) || (iSelCount == LB_ERR))
	{
		return;
	}

	int *piIndexes = NULL;
	
	if (bCaretOnly)
	{
		iSelCount = 1;
		piIndexes = new int[iSelCount];
		piIndexes[0] = m_lbImages.GetCaretIndex();
	}
	else
	{
		piIndexes = new int[iSelCount];
		m_lbImages.GetSelItems (iSelCount, piIndexes);		
	}
	
	int iSizes[4];

	CWADItem *pWADItem	= NULL;
	CWallyDoc *pWallyDoc	= NULL;
	CWallyView *pFirstView  = NULL;
	CPackageDoc *pPackage	= (CPackageDoc *)GetDocument();
	BYTE* pbyMipData = NULL;
	BYTE* pbyWADData = NULL;
	POSITION pos;

	int j = 0;
	int k = 0;
	
	for (j = 0; j < iSelCount; j++)
	{
		pWADItem = (CWADItem *)(m_lbImages.GetItemData(piIndexes[j]));

		if ((pWADItem->IsValidMip()) && (!pWADItem->HasWallyDoc()))
		{	
			// Set the globals so that the Doc gets created properly.
			g_iDocWidth  = pWADItem->GetWidth();
			g_iDocHeight = pWADItem->GetHeight();
			g_iDocColorDepth = 8;

			int iWadType = (pWADItem->GetWADType() == WAD3_TYPE ? FILE_TYPE_HALF_LIFE : FILE_TYPE_QUAKE1);
			int iWidths[4];
			int iHeights[4];

			for (k = 0; k < 4; k++)
			{
				iWidths[k] = max ( (int)(g_iDocWidth / pow (2, k)), 1);
				iHeights[k] = max ( (int)(g_iDocHeight / pow (2, k)), 1);
				iSizes[k] = iWidths[k] * iHeights[k];
			}
			
			int iHoldType = g_iFileTypeDefault;
			g_iFileTypeDefault = iWadType;
			pWallyDoc = (CWallyDoc *)theApp.m_pWallyDocTemplate->OpenDocumentFile(NULL, TRUE);
			g_iFileTypeDefault = iHoldType;

			pWallyDoc->SetGameType (iWadType);
			pWallyDoc->SetName (pWADItem->GetName());
			pWallyDoc->SetModifiedFlag (false);

			pWallyDoc->SetPackageDoc (pPackage);
			pWADItem->SetWallyDoc (pWallyDoc);

			for (k = 0; k < 4; k++)
			{			
				pbyWADData = pWADItem->GetBits(k);
				pbyMipData = pWallyDoc->GetBits(k);
				memcpy (pbyMipData, pbyWADData, iSizes[k]);
			}

			// Ty-  bug fix:  not sure why, but when the bUpdate parameter below is set to TRUE,
			// the first and largest mip ends up being a solid color.  Immediately after this
			// SetPalette call, you'd see the first mip in CWallyDoc has been wiped clean.
			pWallyDoc->SetPalette (pWADItem->GetPalette(), 256, FALSE);
			pWallyDoc->CopyMipToLayer();

			// Find that first view so we can tell it the data has changed
			pos = pWallyDoc->GetFirstViewPosition();
			pFirstView = (CWallyView *)pWallyDoc->GetNextView( pos );
			pFirstView->UpdateDIBs();
			pFirstView->Invalidate();	

			pFirstView = NULL;
			pWallyDoc = NULL;
			pWADItem = NULL;
		}		
	}	
	
	if (piIndexes)
	{
		delete [] piIndexes;	// neal - must use array delete to delete arrays!
		piIndexes = NULL;
	}
}
#else
void CPackageView::OpenSelectedImages(BOOL bCaretOnly /* = FALSE */)
{
	int iSelCount = m_tcImages.GetSelectedCount();
	if( (iSelCount == 0) || (iSelCount == LB_ERR) )
	{
		return;
	}

	int *piIndexes = NULL;	
	int iSizes[4];
	HTREEITEM hItem = NULL;

	CWADItem *pWADItem	= NULL;
	CWallyDoc *pWallyDoc	= NULL;
	CWallyView *pFirstView  = NULL;
	CPackageDoc *pPackage	= (CPackageDoc *)GetDocument();
	BYTE* pbyMipData = NULL;
	BYTE* pbyWADData = NULL;
	POSITION pos;

	int j = 0;
	int k = 0;
	
	hItem = m_tcImages.GetFirstSelectedItem();
	while( hItem )
	{
		pWADItem = m_tcImages.GetImageFromHTREEITEM( hItem );
		if( !pWADItem )
		{
			hItem = m_tcImages.GetNextSelectedItem( hItem );
			continue;
		}

		if ((pWADItem->IsValidMip()) && (!pWADItem->HasWallyDoc()))
		{	
			// Set the globals so that the Doc gets created properly.
			g_iDocWidth  = pWADItem->GetWidth();
			g_iDocHeight = pWADItem->GetHeight();
			g_iDocColorDepth = 8;

			int iWadType = (pWADItem->GetWADType() == WAD3_TYPE ? FILE_TYPE_HALF_LIFE : FILE_TYPE_QUAKE1);
			int iWidths[4];
			int iHeights[4];

			for (k = 0; k < 4; k++)
			{
				iWidths[k] = max ( (int)(g_iDocWidth / pow (2, k)), 1);
				iHeights[k] = max ( (int)(g_iDocHeight / pow (2, k)), 1);
				iSizes[k] = iWidths[k] * iHeights[k];
			}
			
			int iHoldType = g_iFileTypeDefault;
			g_iFileTypeDefault = iWadType;
			pWallyDoc = (CWallyDoc *)theApp.WallyDocTemplate->OpenDocumentFile(NULL, TRUE);
			g_iFileTypeDefault = iHoldType;

			pWallyDoc->SetGameType (iWadType);
			pWallyDoc->SetName (pWADItem->GetName());
			pWallyDoc->SetModifiedFlag (false);

			pWallyDoc->SetPackageDoc (pPackage);
			pWADItem->SetWallyDoc (pWallyDoc);

			for (k = 0; k < 4; k++)
			{			
				pbyWADData = pWADItem->GetBits(k);
				pbyMipData = pWallyDoc->GetBits(k);
				memcpy (pbyMipData, pbyWADData, iSizes[k]);
			}

			// Ty-  bug fix:  not sure why, but when the bUpdate parameter below is set to TRUE,
			// the first and largest mip ends up being a solid color.  Immediately after this
			// SetPalette call, you'd see the first mip in CWallyDoc has been wiped clean.
			pWallyDoc->SetPalette (pWADItem->GetPalette(), 256, FALSE);
			pWallyDoc->CopyMipToLayer();

			// Find that first view so we can tell it the data has changed
			pos = pWallyDoc->GetFirstViewPosition();
			pFirstView = (CWallyView *)pWallyDoc->GetNextView( pos );
			pFirstView->UpdateDIBs();
			pFirstView->Invalidate();	

			pFirstView = NULL;
			pWallyDoc = NULL;
			pWADItem = NULL;
		}

		hItem = m_tcImages.GetNextSelectedItem( hItem );
	}	
}
#endif


void CPackageView::OnPackageAdd() 
{
	ImportImages();
}

void CPackageView::ImportImages()
{
	theApp.OnFileBatchConversion();
	InvalidateRect (NULL, false);
}

void CPackageView::OnEditPastePackage() 
{
	DoPaste (false);
}

void CPackageView::OnEditPaste() 
{
	DoPaste (false);
}

void CPackageView::OnUpdateEditPastePackage(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ::IsClipboardFormatAvailable(CF_DIB) || ::IsClipboardFormatAvailable(CF_HDROP) );
}


void CPackageView::OnEditPasteOver() 
{
	DoPaste (true);
}

void CPackageView::OnUpdateEditPasteOver(CCmdUI* pCmdUI) 
{
	if ( !::IsClipboardFormatAvailable(CF_DIB) && !::IsClipboardFormatAvailable(g_iBrowseCopyPasteFormat) )
	{
		// Get out of here without even bothering about checking the list
		pCmdUI->Enable(FALSE);
		return;
	}

	int iIndex = 0;
	int iSelCount = m_lbImages.GetSelCount();
	bool bEnable = false;

	if (iSelCount == 1)
	{		
		m_lbImages.GetSelItems (1, &iIndex);
		CWADItem *pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));		

		bEnable = (pItem->IsValidMip());
	}
	else
	{
		bEnable = false;
	}	
	pCmdUI->Enable(bEnable);
}

void CPackageView::DoBrowsingPaste ()
{
	
	CWnd *pWnd = (CWnd *)this;
	// This is our custom clipboard format, created during browsing.  The 
	// data contains a list of file names.
	if (pWnd->OpenClipboard())
	{			
		HGLOBAL hgData = ::GetClipboardData(g_iBrowseCopyPasteFormat);
		if (!hgData)
		{
			AfxMessageBox ("Error reading from clipboard", MB_ICONSTOP);
			return;
		}

		// Determine the data size
		int iDataSize = GlobalSize(hgData);
					
		// Alloc memory
		BYTE *pbyData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

		if (!pbyData)
		{
			AfxMessageBox ("Ran out of memory trying to access clipboard", MB_ICONSTOP);
			return;
		}

		// Grab the data					
		CopyMemory(pbyData, GlobalLock(hgData), iDataSize); 

		// Get out as fast as we can from the clipboard
		GlobalUnlock(hgData);
		CloseClipboard();

		LPBC_HEADERS_S lpHeader = (LPBC_HEADERS_S)(pbyData);
		LPBROWSE_CLIPBOARD_S lpClip = NULL;
		int iPosition = lpHeader->iFirstEntryOffset;

		for (int j = 0; j < lpHeader->iNumSelections; j++)
		{
			lpClip = (LPBROWSE_CLIPBOARD_S)(pbyData + iPosition);
			
			int iLength = lpClip->iFileNameLength;
			iPosition += iLength + BROWSE_CLIPBOARD_SIZE;
			
			char *szFileName = new char[iLength + 1];

			strcpy_s (szFileName, sizeof(szFileName), (char *)(&lpClip->szFileName));

			ImportImage (szFileName);

			if (szFileName)
			{
				delete []szFileName;
				szFileName = NULL;
			}
		}

		lpHeader = NULL;
		lpClip = NULL;

		if (pbyData)
		{
			GlobalFree ((HGLOBAL)pbyData);
			pbyData = NULL;
		}
	}
}

void CPackageView::DoPaste (bool bReplaceCurrent)
{
	/*
	if ( ::IsClipboardFormatAvailable(g_iBrowseCopyPasteFormat) )
	{
		DoBrowsingPaste();
		return;
	}
	*/

	if (::IsClipboardFormatAvailable(CF_HDROP))
	{
		CWnd *pWnd = (CWnd *)this;
		
		if (pWnd->OpenClipboard())
		{			
			HGLOBAL hgData = ::GetClipboardData(CF_HDROP);
			if (!hgData)
			{
				AfxMessageBox ("Error reading from clipboard", MB_ICONSTOP);
				return;
			}

			// Determine the data size
			int iDataSize = GlobalSize(hgData);
						
			// Alloc memory
			BYTE *pbyData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

			if (!pbyData)
			{
				AfxMessageBox ("Ran out of memory trying to access clipboard", MB_ICONSTOP);
				return;
			}

			// Grab the data					
			CopyMemory(pbyData, GlobalLock(hgData), iDataSize);

			// Get out as fast as we can from the clipboard
			GlobalUnlock(hgData);
			CloseClipboard();
		
			LPDROPFILES pDropFiles;
		
			pDropFiles = (LPDROPFILES)pbyData;

			// find the offset where the starting point of the files start
			int iCurOffset = pDropFiles->pFiles;

			// copy the filename
			CString strFileName("");
			BOOL bDone = FALSE;

			while (!bDone)
			{
				if (*(pbyData + iCurOffset) == 0)
				{
					bDone = TRUE;
				}
				else
				{
					if (pDropFiles->fWide)
					{
						wchar_t *szWideString = (wchar_t *)(pbyData + iCurOffset);
						strFileName = szWideString;

						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += (strFileName.GetLength() + 1) * 2;						
					}
					else
					{
						strFileName = (LPSTR) ((LPSTR)(pDropFiles) + iCurOffset);
						
						// move the current position beyond the file name copied
						// don't forget the Null terminator (+1)
						iCurOffset += strFileName.GetLength() + 1;
					}

					ImportImage (strFileName);					
				}
			}
		}
	}

	if (::IsClipboardFormatAvailable(CF_DIB))
	{
		// Take the current image from the clipboard and paste it into
		// our open WAD package. 	
		BeginWaitCursor();	
		CDibSection ClipboardDIB;

		// Show the user what's going on
		CProgressBar ctlProgress ("Reading...", 50, 7, false, 0);
		
		if (!ClipboardDIB.InitFromClipboard(this))
		{
			AfxMessageBox ("Failed to open clipboard!", MB_ICONSTOP);
			ctlProgress.Clear();
			return;
		}
		
		ctlProgress.StepIt();
		CPackageDoc *pDoc = GetDocument();
		CMemBuffer mbBits[4];
		CMemBuffer mbClipboardData;
		BYTE *pbyBits[4];

		BYTE *pbyClipboardData = NULL;
		
		int iWidth	= 0;
		int iHeight = 0;
		int iSize	= 0;
		int j		= 0;
		int r, g, b;
		int iSizes[4];
		int iItemAdded = 0;
		
		WORD BitsPerPixel = 0;
		BYTE byClipboardPalette[1024];
		CWallyPalette Palette;
		CString strName ("");
		CString strText ("");
		
		iWidth = ClipboardDIB.GetWidth();
		iHeight = ClipboardDIB.GetHeight();
		BitsPerPixel = ClipboardDIB.GetBitCount();
		
		div_t d_Width, d_Height;
		d_Width = div (iWidth, 16);
		d_Height = div (iHeight, 16);
		if ((d_Width.rem != 0) || (d_Height.rem != 0))
		{
			AfxMessageBox ("Image on clipboard is not evenly divisible by 16.",MB_ICONSTOP);
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

		for (j = 0; j < 4; j++)
		{			
			pbyBits[j] = mbBits[j].GetBuffer (iSizes[j]);
			if (!pbyBits[j])
			{
				AfxMessageBox ("Out of memory during paste operation", MB_ICONSTOP);
				return;
			}			
		}

		if (!bReplaceCurrent)
		{			
			CRenameImageDlg renameDlg;

			bool bFinished = false;
			while (!bFinished)
			{
				if (renameDlg.DoModal() == IDOK)
				{
					strName = renameDlg.GetName();

					if (pDoc->IsNameInList (strName))
					{
						AfxMessageBox ("That name is already in the list.  Please enter a unique name.", MB_ICONSTOP);
					}
					else
					{		
						strName = renameDlg.GetName();
						bFinished = true;
					}
				}	
				else
				{					
					EndWaitCursor();
					ctlProgress.Clear();
					return;
				}
			}			
		}
		else
		{
			int iIndex = 0;
			m_lbImages.GetSelItems (1, &iIndex);
			m_lbImages.GetText (iIndex, strName);
		}
				
		switch (BitsPerPixel)
		{
		case 8:
			{
				// Build the buffer so we can work with it
				iSize = iWidth * iHeight;
				pbyClipboardData = mbClipboardData.GetBuffer (iSize);

				if (!pbyClipboardData)
				{
					AfxMessageBox( "CPackageView::DoPaste() out of memory error.  pbyClipboardData == NULL", MB_ICONSTOP);
					return;
				}
				
				ClipboardDIB.GetRawBits (pbyClipboardData);

				switch (pDoc->GetWADType())
				{
				case WAD2_TYPE:
					{
						COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));					

						memcpy (byClipboardPalette, ClipboardDIB.GetPalette(), 768);					

						Palette.SetPalette (quake1_pal, 256);
						for (j = 0; j < iSizes[0]; j++) 
						{
							r = byClipboardPalette[pbyClipboardData[j] * 3 + 0];
							g = byClipboardPalette[pbyClipboardData[j] * 3 + 1];
							b = byClipboardPalette[pbyClipboardData[j] * 3 + 2];
					
							//pTemp24Bit[j].byRed   = r;
							//pTemp24Bit[j].byGreen = g;
							//pTemp24Bit[j].byBlue  = b;

							// TODO:  32 fullbrights in Quake1!						
							pTemp24Bit[j] = IRGB (0, r, g, b);
						}
						Palette.Convert24BitTo256Color( pTemp24Bit, pbyBits[0], iWidth, iHeight, 
							0, GetDitherType(), FALSE);
						if (pTemp24Bit)
						{
							free (pTemp24Bit);
						}
						ctlProgress.StepIt();
					}
					break;

				case WAD3_TYPE:			
					// Grab the clipboard bits
					memcpy (pbyBits[0], pbyClipboardData, iSizes[0]);
					memcpy (byClipboardPalette, ClipboardDIB.GetPalette(), 768);

					Palette.SetPalette (byClipboardPalette, 256);
					ctlProgress.StepIt();
					break;

				default:
					ASSERT (false);
					break;
				}

			}
			break;
		
		case 24:
			{
				// Build the buffer so we can work with it
				iSize = iWidth * iHeight * 3;
				pbyClipboardData = mbClipboardData.GetBuffer (iSize);

				if (!pbyClipboardData)
				{
					AfxMessageBox( "CPackageView::DoPaste() out of memory error.  pbyClipboardData == NULL", MB_ICONSTOP);
					return;
				}
				
				ClipboardDIB.GetRawBits (pbyClipboardData);
				
				switch (pDoc->GetWADType())
				{
				case WAD2_TYPE:
					{
						Palette.SetPalette (quake1_pal, 256);
						COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
						
						for (j = 0; j < iSizes[0]; j++)
						{
							r = pbyClipboardData[j * 3 + 0];
							g = pbyClipboardData[j * 3 + 1];
							b = pbyClipboardData[j * 3 + 2];
														
							// TODO:  Quake1 fullbrights?							
							pTemp24Bit[j] = IRGB (0, r, g, b);
						}
						Palette.Convert24BitTo256Color( pTemp24Bit, pbyBits[0], 
								iWidth, iHeight, 0, GetDitherType(), FALSE);
												
						if (pTemp24Bit)
								free( pTemp24Bit);
						ctlProgress.StepIt();
					}
					break;

				case WAD3_TYPE:
					{
						COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
						BYTE byPalette[256*3];
						char cFlag = strName.GetAt(0);
						int iNumColors = (cFlag == '{' ? 255 : 256);
						CColorOptimizer ColorOpt;
						
						for (j = 0; j < iSizes[0]; j++)
						{
							r = pbyClipboardData[j * 3 + 0];
							g = pbyClipboardData[j * 3 + 1];
							b = pbyClipboardData[j * 3 + 2];
														
							pTemp24Bit[j] = IRGB (0, r, g, b);
						}
						ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byPalette, iNumColors, FALSE);
							
						// SetPalette ASSERTs with anything other than 256 colors
						Palette.SetPalette( byPalette, 256);
							
						if (iNumColors == 255)
						{
							// Set index 255 to solid blue... it's our transparent index
							Palette.SetRGB( 255, 0, 0, 255);
						}
						Palette.Convert24BitTo256Color( pTemp24Bit, pbyBits[0], 
								iWidth, iHeight, 0, GetDitherType(), FALSE);
						
						if (pTemp24Bit)
								free( pTemp24Bit);
						ctlProgress.StepIt();

					}
					break;

				default:
					ASSERT (false);
					break;
				}			
			}
			break;

		default:
			ASSERT (false);				// Unhandled bit depth!

			return;
			break;

		}

		int iFlags = PV_FLAG_REMIP;
		iFlags |= (bReplaceCurrent ? PV_FLAG_REPLACE : 0);

		AddImage (pbyBits, &Palette, strName, iWidth, iHeight, iFlags, NULL);		

		ctlProgress.StepIt();				
		ctlProgress.Clear();
	}
	else if (::IsClipboardFormatAvailable(CF_TEXT) && (::GetFocus() == m_edFilter.m_hWnd))
	{
		if (this->OpenClipboard())
		{
			// GetClipboardFile		
			HGLOBAL hg = ::GetClipboardData(CF_TEXT);
			if (hg == NULL)
			{
				AfxMessageBox ("Error reading from clipboard");
			}

			// Determine the data size
			int iDataSize = GlobalSize(hg); 
			
			// Alloc memory
			BYTE *byTextData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

			// Grab the data					
			CopyMemory(byTextData, GlobalLock(hg), iDataSize); 
			
			// Get out as fast as we can from the clipboard
			GlobalUnlock(hg);
			CloseClipboard();

			char *szText = (char *)byTextData;

			m_edFilter.ReplaceSel (szText, TRUE);			
		}
	}
}

void CPackageView::ImportImage (LPCTSTR szPath, BOOL bForDecal /* = FALSE */)
{
	CImageHelper ihHelper;
	ihHelper.LoadImage (szPath, IH_LOAD_ONLYIMAGE);

	CPackageDoc *pDoc = GetDocument();

	if (ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		AfxMessageBox (ihHelper.GetErrorText());
		return;
	}

	CString strName (GetRawFileName (szPath));
	CString strError("");
	strName = strName.Left (15);

	BYTE* pbyImageBits[4];
	BYTE* pbyIHBits = ihHelper.GetBits();
	
	int iWidth = ihHelper.GetImageWidth();
	int iHeight = ihHelper.GetImageHeight();

	div_t d_Width, d_Height;
	d_Width = div (iWidth, 16);
	d_Height = div (iHeight, 16);
	if ((d_Width.rem != 0) || (d_Height.rem != 0))
	{
		strError.Format ("Dimensions of %s are not evenly divisible by 16", szPath);
		AfxMessageBox (strError, MB_ICONSTOP);
		return;
	}

	if (bForDecal)
	{
		if ((iWidth * iHeight) > 10752)
		{				
			strError.Format ("This texture is too large.  Width * Height must be less than or equal to 10752");
			AfxMessageBox (strError, MB_ICONSTOP);
			return;
		}

		strName = "{LOGO";
	}

	int iSizes[4];
	int j = 0;
	int iWidths[4];
	int iHeights[4];
	
	for (j = 0; j < 4; j++)
	{
		iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
		iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
		iSizes[j] = (iWidths[j] * iHeights[j]);
	}

	for (j = 0; j < 4; j++)
	{
		pbyImageBits[j] = new BYTE[iSizes[j]];
		memset (pbyImageBits[j], 0, iSizes[j]);
	}

	CColorOptimizer ColorOpt;
	CWallyPalette Palette;
	BYTE byOptimizedPalette[768];
	int iBPP = ihHelper.GetColorDepth();
	
	BYTE byRedHold = 0;
	BYTE byGreenHold = 0;
	BYTE byBlueHold = 0;
	list<DWORD> lstBlueIndexes;
	list<DWORD>::iterator itIndex;
	BOOL bNonBlueColor = FALSE;

	char cFlag = strName.GetAt(0);

	switch (iBPP)
	{
	case IH_8BIT:
		{			
			int r, g, b;
			unsigned char *pbyPalette = ihHelper.GetPalette();
			switch (pDoc->GetWADType())
			{
			case WAD2_TYPE:
				{
					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
					for (j = 0; j < iSizes[0]; j++)
					{
						r = pbyPalette[pbyIHBits[j] * 3 + 0];
						g = pbyPalette[pbyIHBits[j] * 3 + 1];
						b = pbyPalette[pbyIHBits[j] * 3 + 2];
						// TODO:  Lotsa fullbrights, man!
						pTemp24Bit[j]= IRGB (0, r, g, b);
					}
					
					Palette.SetPalette (quake1_pal, 256);
					Palette.Convert24BitTo256Color( pTemp24Bit, pbyImageBits[0], 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
					}
				}
				break;

			case WAD3_TYPE:
				{
					if ( (bForDecal) || (cFlag == '{') )
					{
						COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
						
						for (j = 0; j < iSizes[0]; j++)
						{
							r = pbyPalette[pbyIHBits[j] * 3 + 0];
							g = pbyPalette[pbyIHBits[j] * 3 + 1];
							b = pbyPalette[pbyIHBits[j] * 3 + 2];

							if ((r == 0) && (g == 0) && (b == 255))
							{
								// Store these away, we'll set them to the hold color in a bit
								lstBlueIndexes.push_back(j);
							}
							else
							{
								// Only set these values once... save some time
								if (!bNonBlueColor)
								{
									bNonBlueColor = TRUE;
									byRedHold = r;
									byGreenHold = g;
									byBlueHold = b;
								}
							}
							
							pTemp24Bit[j]= IRGB (0, r, g, b);
						}

						// Okay, set these blue indexes to some other color
						for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
						{				
							pTemp24Bit[*itIndex] = IRGB( 0, byRedHold, byGreenHold, byBlueHold);
						}

						// Only 255 colors, so we can reserve #256 for pure blue
						ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byOptimizedPalette, 255, TRUE);

						byOptimizedPalette[765] = 0;
						byOptimizedPalette[766] = 0;
						byOptimizedPalette[767] = 255;

						Palette.SetPalette( byOptimizedPalette, 256);

						// Put our indexes back to blue now
						for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
						{
							pTemp24Bit[*itIndex] = IRGB( 0, 0, 0, 255);
						}

						Palette.Convert24BitTo256Color( pTemp24Bit, pbyImageBits[0], 
							iWidth, iHeight, 0, GetDitherType(), FALSE);

						if (pTemp24Bit)
						{
							free (pTemp24Bit);
						}
					}
					else
					{
						// Grab the bits					
						memcpy (pbyImageBits[0], pbyIHBits, iSizes[0]);
						Palette.SetPalette (pbyPalette, 256);
						pbyPalette = NULL;
					}
				}
				break;

			default:
				ASSERT (false);
				break;
			}			
			break;
		}
	
	case IH_24BIT:
		{
			COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSizes[0] * sizeof( COLOR_IRGB));
			int iNumColors = (cFlag == '{' ? 255 : 256);			
			BYTE r, g, b;
			
			switch (pDoc->GetWADType())
			{
			case WAD2_TYPE:
				{
					for (j = 0; j < iSizes[0]; j++)
					{
						r = pbyIHBits[j * 3];
						g = pbyIHBits[j * 3 + 1];
						b = pbyIHBits[j * 3 + 2];
						
						pTemp24Bit[j]= IRGB (0, r, g, b);
					}
					Palette.SetPalette( quake1_pal, 256);
				}
				break;

			case WAD3_TYPE:
				{
					for (j = 0; j < iSizes[0]; j++)
					{
						r = pbyIHBits[j * 3];
						g = pbyIHBits[j * 3 + 1];
						b = pbyIHBits[j * 3 + 2];

						if ((bForDecal) || (cFlag == '{'))
						{
							if ((r == 0) && (g == 0) && (b == 255))
							{
								// Store these away, we'll set them to the hold color in a bit
								lstBlueIndexes.push_back(j);
							}
							else
							{
								// Only set these values once... save some time
								if (!bNonBlueColor)
								{
									bNonBlueColor = TRUE;
									byRedHold = r;
									byGreenHold = g;
									byBlueHold = b;
								}
							}
						}
						
						pTemp24Bit[j]= IRGB (0, r, g, b);
					}

					// Okay, set these blue indexes to some other color
					for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
					{				
						pTemp24Bit[*itIndex] = IRGB( 0, byRedHold, byGreenHold, byBlueHold);
					}

					ColorOpt.Optimize( pTemp24Bit, iWidth, iHeight, byOptimizedPalette, iNumColors, TRUE);

					// Put our indexes back to blue now
					for (itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++)
					{
						pTemp24Bit[*itIndex] = IRGB( 0, 0, 0, 255);
					}

					if (iNumColors == 255)
					{
						byOptimizedPalette[765] = 0;
						byOptimizedPalette[766] = 0;
						byOptimizedPalette[767] = 255;
					}
				
					// SetPalette ASSERTs with anything other than 256 colors
					Palette.SetPalette( byOptimizedPalette, 256);						
				}
				break;

			default:
				ASSERT (false);
				break;
			}
			
			Palette.Convert24BitTo256Color( pTemp24Bit, pbyImageBits[0], 
					iWidth, iHeight, 0, GetDitherType(), FALSE);
			
			if (pTemp24Bit)
			{
				free( pTemp24Bit);
			}

			break;
		}

	default:
		ASSERT (false);				// Unhandled bit depth!

		for (j = 0; j < 4; j++)
		{
			if (pbyImageBits[j])
			{
				delete [] pbyImageBits[j];	// neal - must use array delete to delete arrays!
				pbyImageBits[j] = NULL;
			}		
		}
		return;
		break;
	}

	AddImage (pbyImageBits, &Palette, strName, iWidth, iHeight, PV_FLAG_REMIP, NULL);

	for (j = 0; j < 4; j++)
	{
		if (pbyImageBits[j])
		{
			delete [] pbyImageBits[j];	// neal - must use array delete to delete arrays!
			pbyImageBits[j] = NULL;
		}		
	}
}

bool CPackageView::AddImage (unsigned char *pbyBits[], CWallyPalette *pPalette, LPCTSTR szName, int iWidth, int iHeight, UINT iFlags, CProgressBar *pProgress)
{	
	bool bReMip = (iFlags & PV_FLAG_REMIP ? true : false);
	bool bUpdate = (iFlags & PV_FLAG_UPDATE ? true : false);
	bool bReplace = (iFlags & PV_FLAG_REPLACE ? true : false);
	int iSizes[4];
	int j = 0;
	int iItemAdded = 0;
	CPackageDoc *pDoc = GetDocument();
	CString strName (szName == NULL ? "" : szName);

	if (bReMip)
	{
		// We've got the main image, time to rebuild the sub-mips					
		BYTE	*pbyMipData		= NULL;
		pbyMipData = (BYTE *) malloc(MAX_DATA_SIZE);

		if (!pbyMipData)
		{
			AfxMessageBox ("Out of memory", MB_ICONSTOP);
			return false;
		}

		if (pProgress)
		{
			// 1
			pProgress->StepIt();
		}

		// TODO:  RebuildWAL is geared around WAL textures
		LPQ2_MIP_S pMipHeader = (LPQ2_MIP_S)pbyMipData;

		int iHeaderSize = Q2_HEADER_SIZE;

		int iWidths[4];
		int iHeights[4];
		
		for (j = 0; j < 4; j++)
		{
			iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
			iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
			iSizes[j] = (iWidths[j] * iHeights[j]);
		}		

		pMipHeader->width = iWidth;
		pMipHeader->height = iHeight;
		pMipHeader->offsets[0] = iHeaderSize;
		pMipHeader->offsets[1] = iHeaderSize + iSizes[0];
		pMipHeader->offsets[2] = iHeaderSize + iSizes[0] + iSizes[1];
		pMipHeader->offsets[3] = iHeaderSize + iSizes[0] + iSizes[1] + iSizes[2];
			
		memcpy( pbyMipData + pMipHeader->offsets[0], pbyBits[0], iSizes[0]);

		if (pProgress)
		{
			// 2
			pProgress->StepIt();
		}

		///////////////////////////////
		// Build all the sub-mips //
		/////////////////////////////

		pPalette->SetNumFullBrights( 0);
		pPalette->RebuildWAL( pbyMipData);

		if (pProgress)
		{
			// 3
			pProgress->StepIt();
		}
		/////////////////////////
		// copy the data back //
		///////////////////////
			
		for (j = 0; j < 4; j++)
		{
			memcpy( pbyBits[j], pbyMipData + (pMipHeader->offsets[j]), iSizes[j]);
		}

		if (pProgress)
		{
			pProgress->StepIt();
		}

		pMipHeader = NULL;
		if (pbyMipData)
		{
			free (pbyMipData);
			pbyMipData = NULL;
		}
	}

	bool bDupNameReplace = false;

	if (!bReplace)
	{
		bool bFinished = false;
		CDuplicateNameDlg dlgDuplicate;
		dlgDuplicate.SetMaxLength (15);

		while (!bFinished)
		{
			if (pDoc->IsNameInList (strName))
			{				
				dlgDuplicate.SetName (strName);				
				if (dlgDuplicate.DoModal() == IDOK)
				{
					bDupNameReplace = dlgDuplicate.ReplaceImage();
					if (!bDupNameReplace)
					{			
						strName = dlgDuplicate.GetName();
					}
					else
					{
						bFinished = true;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				bFinished = true;
			}
		}
	}

	if ((!bReplace) && (!bDupNameReplace))
	{			
		CWADItem *pItem = pDoc->AddImage (pbyBits, pPalette, strName, iWidth, iHeight, bUpdate);
		m_lbImages.SelItemRange( false, 0, (m_lbImages.GetCount() - 1));
		iItemAdded = m_lbImages.AddString (strName);
		pItem->SetListBoxIndex (iItemAdded);

		m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
		m_lbImages.SetSel (iItemAdded, true);
		m_lbImages.SetTopIndex (iItemAdded);
	}


	if (bReplace || bDupNameReplace)
	{
		CWADItem *pItem = NULL;
		int iIndex		= 0;

		// Only one or the other, not both!
		ASSERT ((bReplace && !bDupNameReplace) || (!bReplace && bDupNameReplace));		

		if (bDupNameReplace)
		{
			pItem = pDoc->FindNameInList(strName);
		}
														
		if (bReplace)
		{
			int iSelCount = m_lbImages.GetSelCount();
			ASSERT (iSelCount == 1);		// Replacing a current image should not be possible with multiple items selected!
			
			m_lbImages.GetSelItems (1, &iIndex);
			pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));
		}
		
		/*int iCurrentWidth = pItem->GetWidth();
		int iCurrentHeight = pItem->GetHeight();
										
		if ((iCurrentWidth != iWidth) || (iCurrentHeight != iHeight))
		{
			// They're not the same size, so go yank out the old one
			strName = pItem->GetName();
			pDoc->RemoveImage (pItem);
			pItem = pDoc->AddImage (pbyBits, pPalette, strName, iWidth, iHeight, bUpdate);
			m_lbImages.SetItemData (iIndex, (DWORD)pItem);				
		}
		else
		{
			// They're the same size*/
			pItem->UpdateData (pbyBits, pPalette, iWidth, iHeight);
			pDoc->SetModifiedFlag (true);
		//}		
	}

	// Call Selchange to update the view and static text members
	OnSelchangeListImages();
	if (pProgress)
	{
		// 4
		pProgress->StepIt();
	}
	return true;
}


void CPackageView::OnEditZoomin() 
{
#ifdef _DEBUG
	// Currently only the developers have this capability, to avoid confusion
	m_iMipNumber--;
	m_iMipNumber = min (m_iMipNumber, 3);
	m_iMipNumber = max (m_iMipNumber, 0);
	
	OnSelchangeListImages();
#endif
}

void CPackageView::OnEditZoomout() 
{
#ifdef _DEBUG
	// Currently only the developers have this capability, to avoid confusion
	m_iMipNumber++;
	m_iMipNumber = min (m_iMipNumber, 3);
	m_iMipNumber = max (m_iMipNumber, 0);

	OnSelchangeListImages();
#endif
}

void CPackageView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	int iSelCount = m_lbImages.GetSelCount();
	pCmdUI->Enable ((iSelCount > 0) && (iSelCount != LB_ERR));
}

// neal - BUGFIX - you cannot have any parameters on a command handler (even with defaults)
void CPackageView::OnEditDelete()
{
	EditDelete();
}

void CPackageView::EditDelete( int iSelection /* = -1 */, bool bPrompt /* = true */)
{
	int iSelCount = m_lbImages.GetSelCount();	

	if ((iSelCount == 0) || (iSelCount == LB_ERR))
	{
		ASSERT (false);// Shouldn't be able to get here if no items are selected!
		return;
	}

	// If iSelection is anything other than -1, then we don't want to delete all of the 
	// currently selected items, just the one specified.  This is really for the drag-n-drop
	// support so that when it comes time to delete items from the source package, we can
	// delete one at a time as opposed to all selected.  This protects us in case the image
	// wasn't successfully moved to the destination package.

	if (iSelection != -1)
	{
		ASSERT ((iSelection >= 0) && (iSelection < m_lbImages.GetCount()));
	}

	if (bPrompt)
	{
		CString strText ("");
		// Just a grammar thing here...
		if (iSelCount > 1)
		{
			strText.Format ("Are you sure you want to delete these %d items?", iSelCount);
		}
		else
		{
			strText.Format ("Are you sure you want to delete this item?");
		}

		int iReturn = AfxMessageBox (strText, MB_YESNO);

		if (iReturn == IDNO)
		{
			return;
		}
	}
	
	CPackageDoc *pDoc = GetDocument();
	CWADItem *pItem = NULL;
	int iIndex = 0;
	int iTopIndex = m_lbImages.GetCount();
	int k = 0;

	if (iSelection != -1)
	{
		if ((iSelection - 1) < iTopIndex)
		{
			iTopIndex = iSelection - 1;
		}
		pItem = (CWADItem *)(m_lbImages.GetItemData (iSelection));
		m_lbImages.SetItemData (iSelection, NULL);
		pDoc->RemoveImage (pItem);
		m_lbImages.DeleteString (iSelection);
	}
	else
	{
		for (int j = 0; j < iSelCount; j++)
		{
			iIndex = 0;
			while ((k = m_lbImages.GetSel(iIndex)) == 0)
			{
				iIndex++;
			}
			if (k == LB_ERR)
			{
				return;
			}
	
			if ((iIndex - 1) < iTopIndex)
			{
				iTopIndex = iIndex - 1;
			}
			pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));
			m_lbImages.SetItemData (iIndex, NULL);
			pDoc->RemoveImage (pItem);
			m_lbImages.DeleteString (iIndex);
		}
	}

	int iCount = m_lbImages.GetCount();
	
	iTopIndex = min (iTopIndex, (iCount - 1));
	iTopIndex = max (iTopIndex, 0);
	
	if (iCount > 0)
	{
		m_lbImages.SelItemRange (false, 0, (iCount - 1));
		int w = 0;
		int h = 0;
		unsigned char *pBits = NULL;
		unsigned char *pPalette = NULL;
		CString strText ("");

		m_lbImages.SetSel (iTopIndex, true);
		m_lbImages.SetTopIndex (iTopIndex);
	}

	OnSelchangeListImages();
}

void CPackageView::RemoveImage (int iIndex)
{
	CPackageDoc *pDoc = GetDocument();
	
	CWADItem *pItem = (CWADItem *)(m_lbImages.GetItemData (iIndex));
	m_lbImages.SetItemData (iIndex, NULL);
	
	pDoc->RemoveImage (pItem);
	m_lbImages.DeleteString (iIndex);
}

void CPackageView::OnEditClear() 
{
	EditDelete();	
}

void CPackageView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	int iSelCount = m_lbImages.GetSelCount();
	pCmdUI->Enable ((iSelCount > 0) && (iSelCount != LB_ERR));
}

void CPackageView::OnFileBatchConversion() 
{
	theApp.OnFileBatchConversion();
	OnSelchangeListImages();
}


void CPackageView::AddString (CWADItem *pItem, LPCTSTR szName, bool bSetSelection)
{	
	int iItemAdded = m_lbImages.AddString (szName);	
	pItem->SetListBoxIndex (iItemAdded);
	m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);	
	
	if (bSetSelection)
	{
		m_lbImages.SelItemRange( false, 0, (m_lbImages.GetCount() - 1));	
		m_lbImages.SetSel (iItemAdded, true);	
		m_lbImages.SetTopIndex (iItemAdded);	
		OnSelchangeListImages();
	}
}


void CPackageView::OnUpdatePackageExport(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageView::OnPackageExport() 
{
	DoPackageExport();
}

void CPackageView::DoPackageExport (LPCTSTR szDirectory /* = NULL */)
{
	int iSelCount = m_lbImages.GetSelCount();

	if ((iSelCount == LB_ERR) || (iSelCount == 0))
	{
		ASSERT (FALSE);		// This option should not be available if no items are selected!
		return;
	}

	int *piIndexes = NULL;
	piIndexes = new int[iSelCount];
	CWADItem *pItem = NULL;
	int j = 0;
	int k = 0;
	FILE *fp = NULL;
	CString strText ("");
	CString strFileName ("");
	CString strFileExtension ("");
	CString strErrorMessage ("");
	CString strDirectory ("");

	m_lbImages.GetSelItems (iSelCount, piIndexes);	

	if ((iSelCount > 1) || (szDirectory))
	{		
		CPackageExportDlg dlgExport;

		dlgExport.SetDirectory (szDirectory != NULL ? szDirectory : g_strFileSaveDirectory.GetBuffer());
				
		if (dlgExport.DoModal() == IDOK)
		{
			strDirectory = dlgExport.GetDirectory();
			strDirectory = TrimSlashes (strDirectory);
			
			// Only update the global if nothing was passed in
			if (!szDirectory)
			{
				g_strFileSaveDirectory = strDirectory;
			}

			// Let the dialog box clear out
			Sleep (500);
			CProgressBar ctlProgress ("", 50, iSelCount, false, 0);
			strFileExtension = "." + dlgExport.GetExtension();

			for (j = 0; j < iSelCount; j++)
			{					
				pItem = (CWADItem *)(m_lbImages.GetItemData (piIndexes[j]));

				if (pItem->IsValidMip())
				{				
					strFileName.Format ("%s%s%s%s", 
								strDirectory,
								"\\",
								pItem->GetName(),
								strFileExtension);
					
					if (!g_bOverWriteFiles)
					{
						errno_t err = fopen_s (&fp, strFileName, "r");
						k = 1;
						while (err == 0)
						{
							// Come up with a filename that isn't already there
							fclose (fp);
							strFileName.Format ("%s%s%s%d%s", 
								strDirectory,
								"\\",
								pItem->GetName(),							
								k++,
								strFileExtension);
							err = fopen_s(&fp, strFileName, "r");							
						}
					}
				
					if (!ExportImage (pItem, strFileName, &strErrorMessage))
					{
						strText.Format ("Export %s --> %s%s === FAIL ===%s%s%s%s", pItem->GetName(), GetRawFileNameWExt(strFileName), CRLF, CRLF, strErrorMessage, CRLF, CRLF);
					}
					else
					{
						strText.Format ("Export %s --> %s%s === OK ===%s%s", pItem->GetName(), GetRawFileNameWExt(strFileName), CRLF, CRLF, CRLF);
					}
				}
				else
				{
					strText.Format ("Export %s --> %s%s === FAIL ===%s%s%s%s", pItem->GetName(), pItem->GetName(), CRLF, CRLF, "This item is not a MIP", CRLF, CRLF);
				}	

				m_strStatusText += strText;
				ctlProgress.StepIt();				
			}			
			ctlProgress.Clear();

			if (g_bDisplayExportSummary)
			{
				CBatchSummaryDlg dlgSummary;	

				dlgSummary.SetStatusText (m_strStatusText);
				dlgSummary.SetTitle ("Export Summary");
				dlgSummary.DoModal();
			}
			
			m_strStatusText = "";
		}	
	}
	else
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData(piIndexes[0]));		
		CImageHelper ihExport;
		
		// Build some strings based on the String Table entries	
		CString strWildCard(ihExport.GetSupportedImageList());
		CString strTitle((LPCTSTR)IDS_FILEEXPORT_TITLE);			
		CString strExportFileName(pItem->GetName());			
		CString strAddExtension;
		int iExtensionMarker	= 0;
		int iFileNameLength		= 0;

		
		// Create a CFileDialog, init with our strings
		CFileDialog	dlgExport (FALSE, NULL, strExportFileName, 
			OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
			strWildCard, NULL); 		
		
		// Set some of the CFileDialog vars
		if (g_strFileSaveDirectory != "")
		{
			dlgExport.m_ofn.lpstrInitialDir = g_strFileSaveDirectory;
		}
		dlgExport.m_ofn.lpstrTitle = strTitle;	
		dlgExport.m_ofn.nFilterIndex = g_iFileExportExtension;
		
		if (dlgExport.DoModal() == IDOK)
		{
			g_strFileSaveDirectory = dlgExport.GetPathName().Left(dlgExport.m_ofn.nFileOffset);
			g_iFileExportExtension = dlgExport.m_ofn.nFilterIndex; 
			strAddExtension = ihExport.GetWildCardExtension(g_iFileExportExtension - 1);
			strFileName = (dlgExport.GetPathName());

			iExtensionMarker = dlgExport.m_ofn.nFileExtension;
			iFileNameLength = strFileName.GetLength();

			// Ty- fix for goofy NT/98 OPENFILENAME issues... with Win98/NT, if the user
			// doesn't enter an extension at the end (and no period '.' at the end) of the filename,
			// the m_ofn.nFileExtension member is equal to 0.  With Win95 under the same circumstance,
			// m_ofn.nFileExtension is equal to the offset to the terminating NULL character (aka FileName.GetLength()).
			// We have to check for each instance and add the extension, as required.

			if ((strFileName.GetAt(iFileNameLength - 1) != _T('.')) && (iExtensionMarker == 0 || iExtensionMarker == iFileNameLength))
			{
				strFileName += ".";
			}

			if ((iExtensionMarker == iFileNameLength) || (iExtensionMarker == 0))
			{				
				// User didn't type in the extension			
				strFileName += strAddExtension;
			}

			if (!ExportImage (pItem, strFileName, &strErrorMessage))
			{
				strText.Format ("Export failed!\n\n%s", strErrorMessage);
				AfxMessageBox (strText, MB_ICONSTOP);
			}
		}
	}

	if (piIndexes)
	{
		delete [] piIndexes;	// neal - must use array delete to delete arrays!
		piIndexes = NULL;
	}
}

bool CPackageView::ExportImage (CWADItem *pItem, LPCTSTR szFileName, CString *pstrErrorMessage)
{	
	CImageHelper ihExport;	

	unsigned char *pBits = pItem->GetBits(0);
	unsigned char *pPalette = NULL;
	
	switch (pItem->GetWADType())
	{
	case WAD2_TYPE:
		pPalette = quake1_pal;
		break;

	case WAD3_TYPE:
		pPalette = pItem->GetPalette();
		break;

	default:
		ASSERT(false);		// Unhandled game type?
		break;

	}
	
	(*pstrErrorMessage) = "";

	int iWidth = pItem->GetWidth();
	int iHeight = pItem->GetHeight();	
	int iSize		= 0;
	int i24BitSize	= 0;
	int j			= 0;

	CString strExtension = GetExtension(szFileName);
	int iExportColorDepth = g_iExportColorDepth;		
	int iFlags = ihExport.GetTypeFlags (strExtension);
	
	if ((iFlags & IH_TYPE_FLAG_SUPPORTS8BIT) && ((iFlags & IH_TYPE_FLAG_SUPPORTS24BIT) == 0))
	{
		// Supports 8BIT, but not 24BIT.
		iExportColorDepth = IH_8BIT;
	}

	if (iFlags & IH_TYPE_FLAG_ISGAME)
	{
		CString strName (pItem->GetName());
		char *szName = strName.GetBuffer(strName.GetLength());
		ihExport.MapGameFlags (IH_WAD3_IMAGE_TYPE, (unsigned char *)szName);
		strName.ReleaseBuffer();
	}
	
	switch (iExportColorDepth)
	{
	case IH_8BIT:		
		ihExport.SaveImage (iExportColorDepth, szFileName, pBits, pPalette, iWidth, iHeight);
		break;

	case IH_24BIT:
		{		
			iSize = iWidth * iHeight;
			i24BitSize = iSize * 3;

			// Keep a local RAM buffer so we don't have to keep re-allocating space
			if (i24BitSize > m_iOutputDataSize)
			{
				m_iOutputDataSize = i24BitSize;
				if (m_pbyOutputData)
				{			
					delete [] m_pbyOutputData;	// neal - must use array delete to delete arrays!
					m_pbyOutputData = NULL;
				}
				m_pbyOutputData = new BYTE[m_iOutputDataSize];				
			}
			memset (m_pbyOutputData, 0, m_iOutputDataSize);

			for (j = 0; j < iSize; j++)
			{			
				m_pbyOutputData[j * 3] = pPalette[pBits[j] * 3];
				m_pbyOutputData[j * 3 + 1] = pPalette[pBits[j] * 3 + 1];
				m_pbyOutputData[j * 3 + 2] = pPalette[pBits[j] * 3 + 2];
			}
			ihExport.SaveImage (iExportColorDepth, szFileName, m_pbyOutputData, NULL, iWidth, iHeight);
		}
		break;

	default:
		ASSERT(false);		// Missed implementation?
		break;
	}				

	if (ihExport.GetErrorCode() != IH_SUCCESS)
	{
		(*pstrErrorMessage) = ihExport.GetErrorText();
		return false;
	}
		
	return true;
}

void CPackageView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CFormView::OnActivateView( bActivate, pActivateView, pDeactiveView);

	CMainFrame* pWndMain = NULL;

	CWnd* pParent = GetParent();
	if (pParent)
		pWndMain = (CMainFrame* )GetTopLevelFrame();
	
	if (m_bItemIsMip)
	{
		if (bActivate)
		{
			CColorPalette* pWndPalette = &pWndMain->m_wndPaletteToolBar;

			//g_CurrentPalette = *(GetDocument()->GetPalette());
			//g_CurrentPalette.SetPalette( m_wndPreview.GetPalette(), 256);

			pWndPalette->Update( NULL, TRUE);
	//
	//		if (pWndMain)
	//			pWndMain->SetActiveView( this, FALSE);
		}
	}
//	else
//	{
//		if (pWndMain)
//			pWndMain->SetActiveView( NULL, FALSE);
//	}
}

void CPackageView::UpdateAllOpenImages(LPCTSTR szPath)
{
	int iCount = m_lbImages.GetCount();
	CWADItem *pItem = NULL;
	CWallyDoc *pWallyDoc = NULL;

	for (int j = 0; j < iCount; j++)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData (j));
		pWallyDoc = pItem->GetWallyDoc();
		
		if (pWallyDoc)
		{
			pWallyDoc->SetPackagePath(szPath);
		}		
	}

	pItem = NULL;
	pWallyDoc = NULL;
}

void CPackageView::UpdateImageData (CWallyDoc *pWallyDoc)
{
	int iCount = m_lbImages.GetCount();
	CWADItem *pItem = NULL;
	CWallyDoc *pDoc = NULL;
	BYTE* pbyBits[4];	
	int j = 0;
	int k = 0;
	int iWidth	= 0;
	int iHeight = 0;

	for (j = 0; j < iCount; j++)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData (j));
		pDoc = pItem->GetWallyDoc();
		
		if (pWallyDoc == pDoc)
		{
			for (k = 0; k < 4; k++)
			{
				pbyBits[k] = pWallyDoc->GetBits(k);
			}			
			
			iWidth	= pWallyDoc->GetWidth();
			iHeight = pWallyDoc->GetHeight();

			pItem->UpdateData (pbyBits, pWallyDoc->GetPalette(), iWidth, iHeight);

			m_lbImages.SelItemRange( false, 0, (m_lbImages.GetCount() - 1));			
			m_lbImages.SetSel (j, true);
			m_lbImages.SetTopIndex (j);
			OnSelchangeListImages();

			for (k = 0; k < 4; k++)
			{
				pbyBits[k] = NULL;
			}
			pItem = NULL;
			pWallyDoc = NULL;
			return;
		}		
	}

	pItem = NULL;
	pWallyDoc = NULL;
}

void CPackageView::BreakDocConnection (CWallyDoc *pWallyDoc)
{
	int iCount = m_lbImages.GetCount();
	CWADItem *pItem = NULL;
	CWallyDoc *pDoc = NULL;	
	int j = 0;	

	for (j = 0; j < iCount; j++)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData (j));
		pDoc = pItem->GetWallyDoc();
		
		if (pWallyDoc == pDoc)
		{
			pItem->SetWallyDoc(NULL);			
			pItem = NULL;
			pWallyDoc = NULL;
			return;
		}		
	}

	pItem = NULL;
	pWallyDoc = NULL;
}

BOOL CPackageView::PreTranslateMessage(MSG* pMsg) 
{	
	if (pMsg->message == WM_DROPFILES)
	{
		DoDragDrop (pMsg);
		return true;
	}	
	return CFormView::PreTranslateMessage(pMsg);
}

BOOL CPackageView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	return DoDragDrop (pDataObject, dropEffect, point);
}

void CPackageView::DoDragDrop(MSG *pMsg)
{
	HDROP hDrop = (HDROP)pMsg->wParam;

	char szPath[_MAX_PATH];
	
	// Figure out how many files we're dealing with
	int iCount = DragQueryFile (hDrop, 0xffffffff, szPath, _MAX_PATH);

	for (int j = 0; j < iCount; j++)
	{
		DragQueryFile (hDrop, j, szPath, _MAX_PATH);
		ImportImage (szPath);	
	}
	DragFinish(hDrop);
}

BOOL CPackageView::DoDragDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{	
	//return CFormView::OnDrop(pDataObject, dropEffect, point); 

	bool bShiftKey = IsKeyPressed( VK_SHIFT);
	bool bControlKey = IsKeyPressed( VK_CONTROL);

	if (pDataObject->IsDataAvailable(g_iBrowseCopyPasteFormat)) 
	{
		HGLOBAL hmem = pDataObject->GetGlobalData(g_iBrowseCopyPasteFormat);
		CMemFile sf((BYTE*) ::GlobalLock(hmem), ::GlobalSize(hmem));
		
		DWORD dwSize = ::GlobalSize(hmem);

		BYTE *pbyDragData = new BYTE[dwSize];
		
		if (!pbyDragData)
		{
			return FALSE;
		}
		
		sf.Read(pbyDragData, dwSize);
		::GlobalUnlock(hmem);

		LPBC_HEADERS_S lpHeader = (LPBC_HEADERS_S)(pbyDragData);
		LPBROWSE_CLIPBOARD_S lpClip = NULL;
		int iPosition = lpHeader->iFirstEntryOffset;
		int iLength = 0;
		
		for (int j = 0; j < lpHeader->iNumSelections; j++)
		{
			lpClip = (LPBROWSE_CLIPBOARD_S)(pbyDragData + iPosition);
			iLength = lpClip->iFileNameLength;
			iPosition += iLength + BROWSE_CLIPBOARD_SIZE;
			
			char *szFileName = new char[iLength + 1];

			strcpy_s (szFileName, sizeof(szFileName), (char *)(&lpClip->szFileName));

			ImportImage (szFileName);

			if (szFileName)
			{
				delete []szFileName;
				szFileName = NULL;
			}
		}	

		if (pbyDragData)
		{
			delete []pbyDragData;
			pbyDragData = NULL;
		}
	}

	if (pDataObject->IsDataAvailable(g_iPackageFormat)) 
	{
		HGLOBAL hmem = pDataObject->GetGlobalData(g_iPackageFormat);
		CMemFile sf((BYTE*) ::GlobalLock(hmem), ::GlobalSize(hmem));

		DWORD dwSize = ::GlobalSize(hmem);
		
		if (m_pbyClipboardBuffer)
		{
			delete [] m_pbyClipboardBuffer;
			m_pbyClipboardBuffer = NULL;
		}
		
		m_pbyClipboardBuffer = new BYTE[dwSize];

		if (!m_pbyClipboardBuffer)
		{
			return FALSE;
		}

		sf.Read(m_pbyClipboardBuffer, dwSize);

		::GlobalUnlock(hmem);

		memcpy (&m_pDragSourcePackage, m_pbyClipboardBuffer, sizeof (CPackageView *));	
		
		if (m_pDragSourcePackage == this)
		{
			return FALSE;
		}

		// Control key handled first... if user has both SHIFT and CONTROL, copy
		// instead of move
		if (bControlKey)
		{
			m_iDragType = PV_DRAG_COPY;
			FinishDragDrop();
			return TRUE;
		}

		if (bShiftKey)
		{
			m_iDragType = PV_DRAG_MOVE;
			FinishDragDrop();
			return TRUE;
		}		

		if (!bShiftKey && !bControlKey)
		{
			m_iDragType = PV_DRAG_UNKNOWN;

			CPoint ptScreenPos( point);
			ClientToScreen( &ptScreenPos);
				
			CMenu Menu;
			VERIFY( Menu.LoadMenu( IDR_PACKAGE_DRAG_POPUP));

			CMenu* pPopup = Menu.GetSubMenu( 0);
			ASSERT( pPopup != NULL);
	
			pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
				ptScreenPos.x, ptScreenPos.y, this);
		}

		return TRUE;	
	}	
	
	return FALSE;
}

DROPEFFECT CPackageView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
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
	
	//return CFormView::OnDragOver(pDataObject, dwKeyState, point);
}


CString CPackageView::GetName()
{
	CPackageDoc *pDoc = GetDocument();
	return (pDoc->GetTitle());
}

void CPackageView::OnCheckFilter() 
{
	m_bFilter = IsDlgButtonChecked (IDC_CHECK_FILTER);
	m_edFilter.EnableWindow (m_bFilter);	
	m_edFilter.SetFocus();
	m_edFilter.SetSel (0, -1, false);
	
	FilterList();
}

void CPackageView::OnDoubleclickedCheckFilter() 
{
	OnCheckFilter();	
}

void CPackageView::OnUpdateEditFilter() 
{
	// This function is called when the user is typing in text

	if (!m_iFilterTimerID)
	{
		m_iFilterTimerID = SetTimer( TIMER_PV_ID_FILTER, 100, NULL);
	}
	else
	{
		m_iFilterTimerTicks = 0;
	}
	
}

void CPackageView::OnTimer(UINT nIDEvent) 
{		
	CFormView::OnTimer(nIDEvent);


	if (nIDEvent == TIMER_PV_ID_FILTER)
	{
		m_iFilterTimerTicks++;
		
		if (m_iFilterTimerTicks > 10)
		{
			BeginWaitCursor();
			FilterList();
			EndWaitCursor();

			KillTimer (TIMER_PV_ID_FILTER);
			m_iFilterTimerTicks = 0;			
			m_iFilterTimerID = 0;
		}
	}

	if (nIDEvent == TIMER_PV_ID_LISTBOX)
	{
		m_iListBoxTimerTicks++;

		if (m_iListBoxTimerTicks > 10)
		{
			if (m_bItemIsMip)
			{
				CMainFrame*    pWndMain    = (CMainFrame* )AfxGetMainWnd();
				if (pWndMain)
				{
					CColorPalette* pWndPalette = &pWndMain->m_wndPaletteToolBar;
					
					//g_CurrentPalette.SetPalette( m_wndPreview.GetPalette(), 256);

					if (pWndPalette)
					{
						pWndPalette->Update( NULL, TRUE);
					}
				}
			}
			KillTimer (TIMER_PV_ID_LISTBOX);
			m_iListBoxTimerID = 0;
			m_iListBoxTimerTicks = 0;
		}
	}		

	if (nIDEvent == TIMER_PV_ID_LISTBOX_SELECTION)
	{
		m_iSelectionTimerTicks++;

		if (m_iSelectionTimerTicks > 5)
		{
			CPackageDoc *pDoc = GetDocument();

			if (pDoc)
			{
				CPackageBrowseView *pBrowseView = pDoc->GetBrowseView();
				pBrowseView->InvalidateRect (NULL, FALSE);
								
				KillTimer (TIMER_PV_ID_LISTBOX_SELECTION);
				m_iSelectionTimerID = 0;
				m_iSelectionTimerTicks = 0;
			}
		}
	}	

	if( nIDEvent == TIMER_PV_ID_TREECTRL )
	{
		m_iTreeCtrlTimerTicks++;

		if( m_iTreeCtrlTimerTicks > 10 )
		{
			if( m_bItemIsMip )
			{
				CMainFrame* pWndMain = (CMainFrame* )AfxGetMainWnd();
				if( pWndMain )
				{
					CColorPalette* pWndPalette = &pWndMain->m_wndPaletteToolBar;
					if( pWndPalette )
					{
						pWndPalette->Update( NULL, TRUE );
					}
				}
			}
			KillTimer( TIMER_PV_ID_TREECTRL );
			m_iTreeCtrlTimerID = 0;
			m_iTreeCtrlTimerTicks = 0;
		}
	}		

	if( nIDEvent == TIMER_PV_ID_TREECTRL_SELECTION )
	{
		m_iSelectionTimerTicks++;

		if( m_iSelectionTimerTicks > 5 )
		{
			CPackageDoc *pDoc = GetDocument();

			if( pDoc )
			{
				CPackageBrowseView *pBrowseView = pDoc->GetBrowseView();
				pBrowseView->InvalidateRect( NULL, FALSE );
								
				KillTimer( TIMER_PV_ID_LISTBOX_SELECTION );
				m_iSelectionTimerID = 0;
				m_iSelectionTimerTicks = 0;
			}
		}
	}
}

void CPackageView::ResetListBox()
{
	m_lbImages.ResetContent();
}

void CPackageView::FilterList()
{
	GetDlgItemText (IDC_EDIT_FILTER, m_strFilter);
	m_strFilter.MakeLower();
	ResetListBox();

	CWADItem *pItem = NULL;
	CString strAddString("");
	CString strCompare("");
	int iItemAdded = 0;
	int iLength = m_strFilter.GetLength();
	bool bAtLeastOne = false;
	CString strText("");

	m_bFilter = IsDlgButtonChecked (IDC_CHECK_FILTER);
	if (iLength == 0)
	{
		m_bFilter = false;		
	}

	CStringArray saWildCard;
	saWildCard.Add (m_strFilter);

	CPackageDoc *pDoc = GetDocument();
	pItem = pDoc->GetFirstLump();
	while (pItem)
	{
		strAddString = pItem->GetName();

		if (m_bFilter)
		{
			strCompare = strAddString.Left(iLength);
			strCompare.MakeLower();

			if (MatchesWildCard (strCompare.GetBuffer(strCompare.GetLength()), &saWildCard))
			{			
				iItemAdded = m_lbImages.AddString (strAddString);
				pItem->SetListBoxIndex (iItemAdded);
				m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
				bAtLeastOne = true;
			}
		}
		else
		{
			iItemAdded = m_lbImages.AddString (strAddString);
			pItem->SetListBoxIndex (iItemAdded);
			m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
			bAtLeastOne = true;
		}
		pItem = pDoc->GetNextLump();		
	}

	if (bAtLeastOne)
	{
		m_lbImages.SetSel (0, true);
	}

	if (m_pBrowseView)
	{
		m_pBrowseView->ResetScrollbars();
		m_pBrowseView->ForceDIBRebuild();
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}
	OnSelchangeListImages();	
}

void CPackageView::OnButtonSelect() 
{
	int iCount = m_lbImages.GetCount();
	m_lbImages.SelItemRange (true, 0, iCount - 1);		
	m_lbImages.SetFocus ();
	if (m_pBrowseView)
	{		
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}
}


void CPackageView::OnPackageDragCancel() 
{
	m_iDragType = PV_DRAG_CANCEL;
	
}

void CPackageView::OnPackageDragCopy() 
{
	m_iDragType = PV_DRAG_COPY;
	FinishDragDrop();
}

void CPackageView::OnPackageDragMove() 
{
	m_iDragType = PV_DRAG_MOVE;
	FinishDragDrop();	
}

void CPackageView::FinishDragDrop()
{
	switch (m_iDragType)
	{
	case PV_DRAG_MOVE:
		break;

	case PV_DRAG_COPY:
		break;

	case PV_DRAG_CANCEL:
	case PV_DRAG_UNKNOWN:
		return;
		break;

	default:
		ASSERT (false);
		return;
		break;
	}

	CPackageListBox *pListBox = m_pDragSourcePackage->GetListBox();
	ASSERT (pListBox);

	int j, k;
	int iSelCount = pListBox->GetSelCount();
	int *piIndexes = NULL;

	CWADItem *pItem = NULL;
	BYTE* pbyBits[4];
	CWallyPalette Palette;
	CString szName("");
	int iWidth = 0;
	int iHeight = 0;

	int iIndex = 0;
	piIndexes = new int[iSelCount];
	pListBox->GetSelItems (iSelCount, piIndexes);

	CPackageDoc *pDoc = GetDocument();
		
	for (j = 0; j < iSelCount; j++)
	{
		if (m_iDragType == PV_DRAG_MOVE)		
		{
			iIndex = 0;

			// Find the next selected item
			while ((k = pListBox->GetSel(iIndex)) == 0)
			{
				iIndex++;
			}
			if (k == LB_ERR)
			{
				return;
			}
		}
		else
		{
			iIndex = piIndexes[j];
		}
		
		// Figure out if the item is a valid MIP or not
		pItem = (CWADItem *)pListBox->GetItemData(iIndex);
		if (pItem->IsValidMip())
		{
			for (k = 0; k < 4; k++)
			{
				pbyBits[k] = pItem->GetBits(k);
			}
			Palette.SetPalette (pItem->GetPalette(), 256);
			iWidth = pItem->GetWidth();
			iHeight = pItem->GetHeight();
			szName = pItem->GetName();

			switch (pDoc->GetWADType())
			{
			case WAD2_TYPE:
				{
					unsigned char *pbyPalette = pItem->GetPalette();						
					int iSize = iWidth * iHeight;

					COLOR_IRGB* pTemp24Bit = (COLOR_IRGB* )malloc( iSize * sizeof( COLOR_IRGB));
					int r, g, b;
					
					for (k = 0; k < iSize; k++)
					{
						r = pbyPalette[pbyBits[0][k] * 3 + 0];
						g = pbyPalette[pbyBits[0][k] * 3 + 1];
						b = pbyPalette[pbyBits[0][k] * 3 + 2];
						
						// TODO:  Lotsa fullbrights, man!
						pTemp24Bit[k]= IRGB (0, r, g, b);
					}
					
					Palette.SetPalette (quake1_pal, 256);
					Palette.Convert24BitTo256Color( pTemp24Bit, pbyBits[0], 
						iWidth, iHeight, 0, GetDitherType(), FALSE);

					if (!AddImage (pbyBits, &Palette, szName, iWidth, iHeight, PV_FLAG_REMIP, NULL))
					{
						if (m_iDragType == PV_DRAG_MOVE)
						{
							// Deselect it, it wasn't moved properly
							pListBox->SetSel (iIndex, FALSE);
						}
					}
					else
					{
						if (m_iDragType == PV_DRAG_MOVE)
						{
							m_pDragSourcePackage->EditDelete( iIndex, FALSE);
						}
					}

					if (pTemp24Bit)
					{
						free (pTemp24Bit);
					}
				}
				break;

			case WAD3_TYPE:
				// Just plop it in, no palette conversion necessary
				if (!AddImage (pbyBits, &Palette, szName, iWidth, iHeight, 0, NULL))
				{
					if (m_iDragType == PV_DRAG_MOVE)
					{
						// Deselect it, it wasn't moved properly
						pListBox->SetSel (iIndex, FALSE);
					}
				}
				else
				{
					if (m_iDragType == PV_DRAG_MOVE)
					{
						m_pDragSourcePackage->EditDelete( iIndex, FALSE);
					}
				}								
				break;

			default:
				ASSERT (false);
				break;
			}				
		}					
	}

	if (piIndexes)
	{
		delete [] piIndexes;
		piIndexes = NULL;
	}
}
void CPackageView::OnUseAsRivetSource() 
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_RivetToolLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsLeftBulletSource() 
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_LeftBulletLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsRightBulletSource() 
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_RightBulletLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsLeftPatternSource() 
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_LeftPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsRightPatternSource() 
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_RightPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsLeftDecalSource()
{
	// TODO: don't use clipboard (do it direct)
	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_LeftDecalToolLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUseAsRightDecalSource()
{
	// TODO: don't use clipboard (do it direct)

	/*
	int w = m_wndPreview.GetWidth();
	int h = m_wndPreview.GetHeight();
	
	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_wndPreview.GetPalette());
	dsImage.SetRawBits( m_wndPreview.GetBits());
	dsImage.WriteToClipboard( this);

	g_RightDecalToolLayerInfo.LoadFromClipboard( FALSE, this);
	*/
}

void CPackageView::OnUpdateUseAsRivetSource( CCmdUI* pCmdUI)
{
	CheckForMipsUpdate( pCmdUI);
}

void CPackageView::OnUpdateUseAsLeftBulletSource( CCmdUI* pCmdUI)
{
	CheckForMipsUpdate( pCmdUI);
}

void CPackageView::OnUpdateUseAsRightBulletSource( CCmdUI* pCmdUI)
{
	CheckForMipsUpdate( pCmdUI);
}

void CPackageView::OnUpdateUseAsLeftPatternSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate( pCmdUI);
}

void CPackageView::OnUpdateUseAsRightPatternSource(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate( pCmdUI);
}

void CPackageView::OnUpdateUseAsLeftDecalSource( CCmdUI* pCmdUI)
{
	CheckForMipsUpdate( pCmdUI);	
}

void CPackageView::OnUpdateUseAsRightDecalSource( CCmdUI* pCmdUI)
{
	CheckForMipsUpdate( pCmdUI);
}


void CPackageView::OnPackageRemipAll() 
{
	ReMipAll();
}

void CPackageView::ReMipAll()
{
	/*
	m_iReMipFlag = PACKAGE_REMIP_ALL;

	if (!m_pReMipThread)
	{
		m_pReMipThread = (CPackageReMipThread *)AfxBeginThread ( (AFX_THREADPROC) CPackageReMipThread::MainLoop, (LPVOID)this);
	}
	else
	{
		AfxMessageBox ("There is already a ReMip in progress");
	}
	*/

	CProgressBar ProgressBar;

	CWADItem *pItem = NULL;

	int iCount = m_lbImages.GetCount();
	int j = 0;

	ProgressBar.Create ("ReMip Deluxe  ", 60, iCount, TRUE, 0);

	BeginWaitCursor();
	for (j = 0; j < iCount; j++)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData(j));
		pItem->RebuildSubMips();		
		ProgressBar.StepIt();
	}

	CPackageDoc *pDoc = GetDocument();
	if( pDoc )
	{
		pDoc->SetModifiedFlag (true);
	}
	ProgressBar.Clear();
	EndWaitCursor();
}

void CPackageView::OnPackageRemipSelected() 
{
	RemipSelected();
}

void CPackageView::RemipSelected()
{
	/*
	m_iReMipFlag = PACKAGE_REMIP_SELECTED;
	
	if (!m_pReMipThread)
	{
		m_pReMipThread = (CPackageReMipThread *)AfxBeginThread ( (AFX_THREADPROC) CPackageReMipThread::MainLoop, (LPVOID)this);
	}
	else
	{
		AfxMessageBox ("There is already a ReMip in progress");
	}
	*/
	
	CProgressBar ProgressBar;

	CWADItem *pItem = NULL;

	int iSelCount = m_lbImages.GetSelCount();
	ProgressBar.Create ("ReMip Deluxe  ", 60, iSelCount, TRUE, 0);

	BeginWaitCursor();
	if (iSelCount != LB_ERR)
	{
		int *piIndexes = NULL;
		piIndexes = new int[iSelCount];
		m_lbImages.GetSelItems (iSelCount, piIndexes);

		int j = 0;		
		for (j = 0; j < iSelCount; j++)
		{
			pItem = (CWADItem *)(m_lbImages.GetItemData(piIndexes[j]));
			pItem->RebuildSubMips();
			ProgressBar.StepIt();
		}

		if (piIndexes)
		{
			delete [] piIndexes;
			piIndexes = NULL;
		}
		ProgressBar.Clear();

		CPackageDoc *pDoc = GetDocument();
		if( pDoc )
		{
			pDoc->SetModifiedFlag (true);
		}
	}
	EndWaitCursor();
}

void CPackageView::StopThread()
{
	m_iReMipFlag = -1;
	m_pReMipThread = NULL;
	CPackageDoc *pDoc = GetDocument();

	pDoc->SetModifiedFlag (TRUE);

	InitReMipProgress("Testing", 60, 50, TRUE, 0);
	for (int j = 0; j < 50; j++)
	{
		StepReMipProgress ();
	}
	ClearReMipProgress ();

}

bool CPackageView::InitReMipProgress(LPCTSTR szText, int iPercent, int iMaxItems, bool bSmooth, int iPane)
{
	if (m_pReMipProgressBar)
	{
		ASSERT (FALSE);
		return FALSE;
	}
	
	m_pReMipProgressBar = new CProgressBar(szText, iPercent, iMaxItems, bSmooth, iPane);

	if (m_pReMipProgressBar)
	{
		TRACE ("Progress created\n");
		return TRUE;
	}

	TRACE ("Progress NOT created\n");
	return FALSE;
}

void CPackageView::StepReMipProgress ()
{
	if (!m_pReMipProgressBar)
	{
		ASSERT (FALSE);
		return;
	}

	TRACE ("Progress stepping\n");
	m_pReMipProgressBar->StepIt();
}

void CPackageView::ClearReMipProgress ()
{
	if (!m_pReMipProgressBar)
	{
		ASSERT (FALSE);
		return;
	}

	TRACE ("Progress clearing\n");
	
	m_pReMipProgressBar->Clear();
	delete m_pReMipProgressBar;
	m_pReMipProgressBar = NULL;

}

void CPackageView::OnPackageNewImage() 
{
	CreateNewImage();
}

void CPackageView::CreateNewImage()
{
	CNewPackageImageDlg NewImageDlg;

	if (NewImageDlg.DoModal() == IDOK)
	{
		int iWidth = NewImageDlg.GetWidth();
		int iHeight = NewImageDlg.GetHeight();
		CString strName ("");

		strName = NewImageDlg.GetName();

		BYTE *pbyBits[4];
		CWallyPalette wlyPalette;		
		int iSizes[4];

		int iWidths[4];
		int iHeights[4];
		int j = 0;
		
		for (j = 0; j < 4; j++)
		{
			iWidths[j] = max ( (int)(iWidth / pow (2, j)), 1);
			iHeights[j] = max ( (int)(iHeight / pow (2, j)), 1);
			iSizes[j] = (iWidths[j] * iHeights[j]);
		}
		
		for (j = 0; j < 4; j++)
		{
			pbyBits[j] = new BYTE[iSizes[j]];
			memset (pbyBits[j], 0, iSizes[j]);
		}

		CHalfLifePaletteDlg dlgPalettePicker;
		CPackageDoc *pDoc = GetDocument();

		int iType = pDoc->GetWADType();

		switch (iType)
		{		
		case WAD2_TYPE:
			wlyPalette.SetPalette (quake1_pal, 256);
			break;

		case WAD3_TYPE:
			dlgPalettePicker.DoModal();
			wlyPalette.SetPalette (dlgPalettePicker.GetPalette(), 256);
			break;

		default:
			ASSERT (FALSE);
			break;
		}
		
		AddImage (pbyBits, &wlyPalette, strName, iWidth, iHeight, 0, NULL);

		for (j = 0; j < 4; j++)
		{	
			if (pbyBits[j])
			{
				delete [] pbyBits[j];
				pbyBits[j] = NULL;
			}		
		}
		OnPackageOpen();
	}
		
}

void CPackageView::OnUpdateTpSelection(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	
	if (m_pBrowseView)
	{
		bEnable = (m_pBrowseView->GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE);
		pCmdUI->SetCheck( !m_pBrowseView->IsInZoomMode());
	}
	pCmdUI->Enable (bEnable);
}

void CPackageView::OnUpdateTpZoom(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	
	if (m_pBrowseView)
	{
		bEnable = (m_pBrowseView->GetViewMode() == PACKAGE_BROWSE_VIEWMODE_TILE);
		pCmdUI->SetCheck (m_pBrowseView->IsInZoomMode());
	}	
	pCmdUI->Enable (bEnable);
}

void CPackageView::OnTpZoom() 
{
	if (m_pBrowseView)
	{
		m_pBrowseView->SetZoomMode(TRUE);
	}
}

void CPackageView::OnTpSelection() 
{
	if (m_pBrowseView)
	{
		m_pBrowseView->SetZoomMode(FALSE);
	}
}

BOOL CPackageView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (nHitTest == HTCLIENT)
	{
		UpdateStatusBar();
	}
	return CFormView::OnSetCursor(pWnd, nHitTest, message);
}

void CPackageView::OnPushAnimate()
{	
	m_lbImages.SetFocus();
	if (m_pBrowseView)
	{
		m_pBrowseView->SetAllowAnimateMode(IsDlgButtonChecked (IDC_PUSH_ANIMATE) > 0);
		m_pBrowseView->ForceDIBRebuild();
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}
}

void CPackageView::OnPushBrowseMode() 
{
	m_lbImages.SetFocus();
	if (m_pBrowseView)
	{
		g_iPackageViewMode = PACKAGE_BROWSE_VIEWMODE_BROWSE;
		m_pBrowseView->SetViewMode(PACKAGE_BROWSE_VIEWMODE_BROWSE);
		m_pBrowseView->ResetScrollbars (TRUE);
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}
}

void CPackageView::OnPushRandom() 
{
	m_lbImages.SetFocus();
	
	if (m_pBrowseView)
	{
		m_pBrowseView->SetAllowRandomMode (IsDlgButtonChecked (IDC_PUSH_RANDOM) > 0);
		m_pBrowseView->ForceDIBRebuild();
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}	
}

void CPackageView::OnPushTileMode() 
{
	m_lbImages.SetFocus();

	if (m_pBrowseView)
	{
		g_iPackageViewMode = PACKAGE_BROWSE_VIEWMODE_TILE;
		m_pBrowseView->SetViewMode(PACKAGE_BROWSE_VIEWMODE_TILE);
		m_pBrowseView->ResetScrollbars (TRUE);
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}
}


void CPackageView::OnPackageResize() 
{	
	CLayer ImageLayer;
	CFilter ResizeFilter;

	int iSelCount = m_lbImages.GetSelCount();
	if ((iSelCount == 0) || (iSelCount == LB_ERR))
	{
		return;
	}

	int *piIndexes = NULL;
	
	piIndexes = new int[iSelCount];
	m_lbImages.GetSelItems (iSelCount, piIndexes);

	CProgressBar pbStatus ("Resizing...", iSelCount, 100, FALSE, 0);
	
	CWADItem *pWADItem	= NULL;
	CPackageDoc *pPackage	= (CPackageDoc *)GetDocument();
	
	BYTE *pbyPalette = NULL;
	BYTE *pbyMipData = NULL;
	BYTE *pbyWADData = NULL;

	int j = 0;
	int k = 0;
	int iWidth = 0;
	int iHeight = 0;
	int x = 0;
	int y = 0;
	int r, g, b;
	COLOR_IRGB irgbData;
	int iOffset = 0;
	
	for (j = 0; j < iSelCount; j++)
	{
		pWADItem = (CWADItem *)(m_lbImages.GetItemData(piIndexes[j]));

		if (pWADItem->IsValidMip())
		{	
			iWidth = pWADItem->GetWidth();
			iHeight = pWADItem->GetHeight();

			if ((iWidth == 256) && (iHeight == 256))
			{
				ImageLayer.Init();
				ImageLayer.SetWidth(iWidth);
				ImageLayer.SetHeight(iHeight);
				ImageLayer.SetNumBits(8);

				ImageLayer.Create();

				pbyMipData = pWADItem->GetBits();
				pbyPalette = pWADItem->GetPalette();

				for (y = 0; y < iHeight; y++)
				{
					for (x = 0; x < iWidth; x++)
					{
						iOffset = y * iWidth + x;

						r = pbyPalette[pbyMipData[iOffset] * 3 + 0];
						g = pbyPalette[pbyMipData[iOffset] * 3 + 1];
						b = pbyPalette[pbyMipData[iOffset] * 3 + 2];

						irgbData = IRGB (iOffset, r, g, b);

						ImageLayer.SetPixel (NULL, x, y, irgbData);
					}
				}

				ResizeFilter.PrepareSoloFilter( &ImageLayer, FILTER_RESIZE, "Resize Filter");
				ResizeFilter.Reduce( &ImageLayer, 2, 2, TRUE);

				//////////////////////////////////////////
				// convert back to 256 color (indexes) //
				////////////////////////////////////////

				COLOR_IRGB*		pIRGB_Src = NULL;
				CColorOptimizer ColorOpt;
				BYTE byPalette[768];

				CWallyPalette	Palette;
				
				int iWidth  = ResizeFilter.m_rDest.Width();
				int iHeight = ResizeFilter.m_rDest.Height();

				ASSERT (ResizeFilter.m_pIRGB_DestBuffer);
				pIRGB_Src = ResizeFilter.m_pIRGB_DestBuffer;
				
				// Grab a new optimized palette
				ColorOpt.Optimize( pIRGB_Src, iWidth, iHeight, byPalette, 256, TRUE);
							
				// SetPalette ASSERTs with anything other than 256 colors
				Palette.SetPalette( byPalette, 256);

				// Build the new buffer
				BYTE *pbyResizeBuffers[4];
				pbyResizeBuffers[0] = new BYTE[iWidth * iHeight];

				if (!pbyResizeBuffers[0])
				{
					AfxMessageBox ("Out of memory during batch resize!", MB_ICONSTOP);
					return;
				}

				// These are just dummies... they have more than enough data, and we 
				// need to ReMip later on.
				for (k = 1; k < 4; k++)
				{
					pbyResizeBuffers[k] = pbyResizeBuffers[0];
				}
				
				// Convert down
				Palette.Convert24BitTo256Color( pIRGB_Src, pbyResizeBuffers[0], 
								iWidth, iHeight, 0, GetDitherType(), FALSE);
				
				// Go replace the data
				pWADItem->ReplaceData (pbyResizeBuffers, &Palette, pWADItem->GetName(), iWidth, iHeight);								

				ResizeFilter.EndSoloFilter();

				
				for (k = 1; k < 4; k++)
				{
					pbyResizeBuffers[k] = NULL;
				}
				
				if (pbyResizeBuffers[0])
				{
					delete []pbyResizeBuffers[0];
					pbyResizeBuffers[0] = NULL;
				}
			}
		}
		pbStatus.StepIt();
	}

	pbStatus.Clear();

	// Now go and ReMip them all, considering we haven't yet
	OnPackageRemipSelected();	
}

void CPackageView::OnUpdatePackageResize(CCmdUI* pCmdUI) 
{	
#ifdef _DEBUG
	pCmdUI->Enable (TRUE);
#else
	CMenu *pParent = pCmdUI->m_pParentMenu;

	if (pParent)
	{
		pParent->RemoveMenu (pCmdUI->m_nID, MF_BYCOMMAND);
	}
	
#endif
	
}

void CPackageView::OnUpdatePackageRemipSelected(CCmdUI* pCmdUI) 
{
	CheckForMipsUpdate(pCmdUI);	
}

void CPackageView::OnSelchangedTreeImages(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;
	CPackageDoc *pDoc = GetDocument();
	CPackageTreeEntryBase *pBase = NULL;
	CWADItem *pItem = NULL;	
	CString strText( "" );
	TV_ITEM tvItem = pNMTreeView->itemNew;
	
	if( tvItem.lParam )
	{
		pBase = reinterpret_cast< CPackageTreeEntryBase * >( tvItem.lParam );
		pItem = pBase->GetItem();
	}
	else
	{
		m_bItemIsMip = false;
		return;
	}
	
	if( m_pBrowseView )
	{
		m_pBrowseView->ResetScrollbars();
		m_pBrowseView->ForceDIBRebuild();
		m_pBrowseView->InvalidateRect (NULL, FALSE);	
		if( m_pBrowseView->GetViewMode() == PACKAGE_BROWSE_VIEWMODE_BROWSE )
		{		
			if( !m_iSelectionTimerID )
			{
				m_iSelectionTimerID = SetTimer( TIMER_PV_ID_TREECTRL_SELECTION, 50, NULL );
			}
			else
			{
				m_iSelectionTimerTicks = 0;
			}
		}
	}	
	
	int w = 0;
	int h = 0;
	int w1 = 0;
	int h1 = 0;	
	int iMip = 0;
	unsigned char *pBits = NULL;
	unsigned char *pPalette = NULL;
	int iType = 0;		
	bool bFoundMatch = false;
	
	if( !pItem )
	{
		return;
	}

	if (pItem->IsValidMip())
	{
		m_bItemIsMip = true;
		w = pItem->GetWidth();
		h = pItem->GetHeight();

		strText.Format ("Width: %d\tHeight: %d", w, h);
		
		pPalette = pItem->GetPalette ();

#ifdef _DEBUG		
		// Let the coders see each mip
		pBits = pItem->GetBits (m_iMipNumber);		
		int iDiv = (1 << m_iMipNumber);
		w = w / iDiv;
		h = h / iDiv;
#else
		// Find the best fit to 256x256		
		
		for (iMip = 0; (iMip < 4) && !bFoundMatch; iMip++)
		{
			int iDiv = (1 << iMip);
			w1 = w / iDiv;
			h1 = h / iDiv;

			if ((w1 <= 256) && (h1 <= 256))
			{
				w = w1;
				h = h1;
				bFoundMatch = true;
			}
		}

		if (!bFoundMatch)
		{
			iMip = 3;			
		}
		else
		{
			iMip--;
		}

		
		pBits = pItem->GetBits (iMip);
#endif

	}
	else
	{
		strText = "";
		m_bItemIsMip = false;
		pBits = NULL;
		pPalette = NULL;
		w = 0;
		h = 0;
	}
	
	UpdateStatusBar();	
}

void CPackageView::OnReleasedcaptureSliderThumbnailSize(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
	UpdateData( TRUE );
	g_iPackageThumbnailSize = m_iThumbnailSize;
	if( m_pBrowseView )
	{
		m_pBrowseView->SetBrowseImageSize( m_iThumbnailSize * 32 );
		m_pBrowseView->ResetScrollbars();
		m_pBrowseView->ForceDIBRebuild();
		m_pBrowseView->InvalidateRect (NULL, FALSE);
	}	
}
