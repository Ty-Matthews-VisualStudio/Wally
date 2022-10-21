/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// MainFrm.cpp : implementation of the CMainFrame class
//
// Created by Ty Matthews, 1-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Afxext.h"
#include "MainFrm.h"
#include "WallyDoc.h"
//#include "Layer.h"
#include "ReMipDlg.h"
//#include "Remip.h"
#include "Splash.h"
#include "Filter.h"
#include "GameSelectDlg.h"
#include "PackageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
extern int g_iLastLeftColor, g_iLastRightColor;
extern BOOL g_bCompressUndo;

CString strDataFileSignature( "WallyData");
WORD    wDataFileVersion = 1;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

//IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)
IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()	
	ON_COMMAND(ID_APPLY_LEAST, OnApplyLeast)
	ON_COMMAND(ID_APPLY_A_LITTLE_BIT, OnApplyALittleBit)
	ON_COMMAND(ID_APPLY_MEDIUM, OnApplyMedium)
	ON_COMMAND(ID_APPLY_MORE, OnApplyMore)
	ON_COMMAND(ID_APPLY_MOST, OnApplyMost)
	ON_UPDATE_COMMAND_UI(ID_APPLY_LEAST, OnUpdateApplyLeast)
	ON_UPDATE_COMMAND_UI(ID_APPLY_A_LITTLE_BIT, OnUpdateApplyALittleBit)
	ON_UPDATE_COMMAND_UI(ID_APPLY_MEDIUM, OnUpdateApplyMedium)
	ON_UPDATE_COMMAND_UI(ID_APPLY_MORE, OnUpdateApplyMore)
	ON_UPDATE_COMMAND_UI(ID_APPLY_MOST, OnUpdateApplyMost)
	ON_COMMAND(ID_SHAPE_SQUARE, OnShapeSquare)
	ON_COMMAND(ID_SHAPE_DIAMOND, OnShapeDiamond)
	ON_COMMAND(ID_SHAPE_CIRCULAR, OnShapeCircular)
	ON_UPDATE_COMMAND_UI(ID_SHAPE_SQUARE, OnUpdateShapeSquare)
	ON_UPDATE_COMMAND_UI(ID_SHAPE_DIAMOND, OnUpdateShapeDiamond)
	ON_UPDATE_COMMAND_UI(ID_SHAPE_CIRCULAR, OnUpdateShapeCircular)	
	ON_WM_SHOWWINDOW()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_VIEW_SHOWSUBMIPS, OnShowSubMips)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWSUBMIPS, OnUpdateShowSubMips)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND( ID_HELP_FINDER,        CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND( ID_HELP,               CMDIFrameWnd::OnHelp)
	ON_COMMAND( ID_CONTEXT_HELP,       CMDIFrameWnd::OnContextHelp)
	ON_COMMAND( ID_DEFAULT_HELP,       CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND( ID_IMAGE_EDIT_TOOLBAR, OnViewImageEditBar)
	ON_COMMAND( ID_MODES_TOOLBAR,	   OnViewModesBar)
	ON_COMMAND( ID_VIEW_TOOLBAR,       OnViewFileBar)
	ON_COMMAND( ID_PALETTE_TOOLBAR,    OnViewPaletteBar)
	ON_COMMAND( ID_TOOLSETTINGS_TOOLBAR,	   OnViewToolSettingsToolbar)
	ON_COMMAND( ID_SHOW_FINE_GRID,     OnShowFineGrid)
	ON_COMMAND( ID_SHOW_COURSE_GRID,   OnShowCourseGrid)

	ON_UPDATE_COMMAND_UI( ID_VIEW_TOOLBAR,       OnUpdateFileBar)
	ON_UPDATE_COMMAND_UI( ID_PALETTE_TOOLBAR,    OnUpdatePaletteBar)
	ON_UPDATE_COMMAND_UI( ID_TOOLSETTINGS_TOOLBAR,		 OnUpdateToolSettingsToolbar)
	ON_UPDATE_COMMAND_UI( ID_IMAGE_EDIT_TOOLBAR, OnUpdateImageEditBar)
	ON_UPDATE_COMMAND_UI( ID_MODES_TOOLBAR,		 OnUpdateModesBar)	
	ON_UPDATE_COMMAND_UI( ID_SHOW_FINE_GRID,     OnUpdateShowFineGrid)
	ON_UPDATE_COMMAND_UI( ID_SHOW_COURSE_GRID,   OnUpdateShowCourseGrid)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator		
	ID_XYPOS,
	ID_WIDTH_HEIGHT,	
//	ID_BLANK_PANE,
	ID_ZOOMVALUE_STATUSBAR,
	ID_CONSTRAIN_STATUSBAR,	
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bProgressCreated = false;	
}

CMainFrame::~CMainFrame()
{
	if (m_WndMdiClient.m_hWnd)
		m_WndMdiClient.UnsubclassWindow();	
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_WndMdiClient.SubclassWindow( m_hWndMDIClient);
	
	if (!m_wndToolBar.Create(this, WS_CHILD | CBRS_TOP | WS_VISIBLE | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY, ID_VIEW_TOOLBAR) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create MAINFRAME toolbar\n");
		return -1;      // fail to create
	}
	else
	{
		m_wndToolBar.SetWindowText("File");
	}
	

	if (!m_wndImageEditToolBar.Create(this, WS_CHILD | CBRS_TOP | WS_VISIBLE | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY,ID_IMAGE_EDIT_TOOLBAR) ||
		!m_wndImageEditToolBar.LoadToolBar(IDR_IMAGE_EDIT_TOOLBAR))
	{
		TRACE0("Failed to create IMAGE_EDIT_TOOLBAR toolbar\n");
		return -1;
	}
	else
	{
		m_wndImageEditToolBar.SetWindowText("Image Toolbar");
	}

	if (!m_wndDrawModesToolBar.Create(this, WS_CHILD | CBRS_TOP | WS_VISIBLE | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC | CBRS_FLYBY, ID_MODES_TOOLBAR) ||
		!m_wndDrawModesToolBar.LoadToolBar(IDR_MODES_TOOLBAR))
	{
		TRACE0("Failed to create MODES_TOOLBAR toolbar\n");
		return -1;
	}
	else
	{
		m_wndDrawModesToolBar.SetWindowText("Drawing Modes Toolbar");
	}	

	
	if (!m_wndPaletteToolBar.Create(this, IDD_PALETTE_TOOLBAR, CBRS_RIGHT, ID_PALETTE_TOOLBAR))
	{
		TRACE0("Failed to create PALETTE toolbar\n");
		return -1;
	}
	else
	{
		m_wndPaletteToolBar.SetWindowText( "Color Palette");
	}

	if (!m_wndToolSettingsToolbar.Create(this, IDD_TOOL_SETTINGS_TOOLBAR, CBRS_RIGHT, ID_TOOLSETTINGS_TOOLBAR))
	{
		TRACE0("Failed to create Tool Settings toolbar\n");
		return -1;
	}
	else
	{
		m_wndToolSettingsToolbar.SetWindowText( "Tool Settings");
	}
		
	if (!m_wndStatusBar.Create(this))
	{
		//TRACE0("Failed to create status bar\n");
		TRACE0("Failed to create status bar window\n");
		return -1;      // fail to create
	}
	if(!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to set status indicators\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo 
		(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), ID_SEPARATOR,	SBPS_NOBORDERS | SBPS_STRETCH, 200);

	m_wndStatusBar.SetPaneInfo 
//		(m_wndStatusBar.CommandToIndex(ID_XYPOS), ID_XYPOS,	SBPS_NOBORDERS, 120);
		(m_wndStatusBar.CommandToIndex(ID_XYPOS), ID_XYPOS,	
				m_wndStatusBar.GetPaneStyle(m_wndStatusBar.CommandToIndex(ID_XYPOS)), 120);
	
	m_wndStatusBar.SetPaneInfo 
//		(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), ID_WIDTH_HEIGHT, SBPS_NOBORDERS, 60);
		(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT), ID_WIDTH_HEIGHT, 
				m_wndStatusBar.GetPaneStyle(m_wndStatusBar.CommandToIndex(ID_WIDTH_HEIGHT)), 100);
	
/*	m_wndStatusBar.SetPaneInfo 
		(m_wndStatusBar.CommandToIndex(ID_STATUSBAR_PROGRESS), ID_STATUSBAR_PROGRESS, SBPS_NOBORDERS, 250); */
	
//	m_wndStatusBar.SetPaneInfo 
//		(m_wndStatusBar.CommandToIndex(ID_BLANK_PANE), ID_BLANK_PANE, SBPS_NOBORDERS, 10);
	
	m_wndStatusBar.SetPaneInfo 
		(m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR), ID_ZOOMVALUE_STATUSBAR, 
				m_wndStatusBar.GetPaneStyle(m_wndStatusBar.CommandToIndex(ID_ZOOMVALUE_STATUSBAR)), 75);

	m_wndStatusBar.SetPaneInfo 
		(m_wndStatusBar.CommandToIndex(ID_CONSTRAIN_STATUSBAR), ID_CONSTRAIN_STATUSBAR, 
				m_wndStatusBar.GetPaneStyle(m_wndStatusBar.CommandToIndex(ID_CONSTRAIN_STATUSBAR)), 95);


	EnableDocking( CBRS_ALIGN_ANY);

	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY);		
	m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	DockControlBar( &m_wndToolBar, AFX_IDW_DOCKBAR_TOP);	

	
	m_wndImageEditToolBar.EnableDocking( CBRS_ALIGN_ANY);
	
	m_wndImageEditToolBar.SetBarStyle( m_wndImageEditToolBar.GetBarStyle() | 
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
// Neal - TODO: make tools toolbar 2 btns wide - codeguru.com?
//virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
	DockControlBar( &m_wndImageEditToolBar, AFX_IDW_DOCKBAR_LEFT);

	m_wndDrawModesToolBar.EnableDocking( CBRS_ALIGN_ANY);
	m_wndDrawModesToolBar.SetBarStyle( m_wndDrawModesToolBar.GetBarStyle() | 
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	DockControlBar( &m_wndDrawModesToolBar, AFX_IDW_DOCKBAR_TOP);

	
	m_wndPaletteToolBar.EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);	
	m_wndPaletteToolBar.SetBarStyle( m_wndPaletteToolBar.GetBarStyle() | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);
	
	m_wndToolSettingsToolbar.EnableDocking( 0);
	m_wndToolSettingsToolbar.SetBarStyle( m_wndToolSettingsToolbar.GetBarStyle() | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);


	CRect rClient;
	GetClientRect( &rClient);
	ClientToScreen( &rClient);
	CPoint mypoint( rClient.right-80, rClient.top);
	FloatControlBar( &m_wndPaletteToolBar, mypoint);
	DockControlBar( &m_wndPaletteToolBar, AFX_IDW_DOCKBAR_RIGHT);

	CPoint DecalPoint( rClient.right - 150, rClient.top + 50);
	FloatControlBar( &m_wndToolSettingsToolbar, DecalPoint);
	
	
	LoadBarState ("ToolBars");		

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);
	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CMDIFrameWnd::PreCreateWindow(cs);
}

void CMainFrame::OnUpdateImageEditBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_wndImageEditToolBar.GetStyle () & WS_VISIBLE) ? 1 : 0);
}

void CMainFrame::OnViewImageEditBar()
{
	ShowControlBar (&m_wndImageEditToolBar, (m_wndImageEditToolBar.GetStyle() & WS_VISIBLE) == 0, FALSE);
}

void CMainFrame::OnUpdateModesBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_wndDrawModesToolBar.GetStyle () & WS_VISIBLE) ? 1 : 0);
}

void CMainFrame::OnViewModesBar()
{
	ShowControlBar (&m_wndDrawModesToolBar, (m_wndDrawModesToolBar.GetStyle() & WS_VISIBLE) == 0, FALSE);
}

void CMainFrame::OnViewFileBar()
{
	ShowControlBar (&m_wndToolBar, (m_wndToolBar.GetStyle() & WS_VISIBLE) == 0, FALSE);	
}

void CMainFrame::OnUpdateFileBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_wndToolBar.GetStyle () & WS_VISIBLE) ? 1 : 0);
}

void CMainFrame::OnViewPaletteBar()
{
	ShowControlBar (&m_wndPaletteToolBar, (m_wndPaletteToolBar.GetStyle() & WS_VISIBLE) == 0, FALSE);
}

void CMainFrame::OnUpdatePaletteBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_wndPaletteToolBar.GetStyle () & WS_VISIBLE) ? 1 : 0);
}

void CMainFrame::OnViewToolSettingsToolbar()
{
	ShowControlBar (&m_wndToolSettingsToolbar, (m_wndToolSettingsToolbar.GetStyle() & WS_VISIBLE) == 0, FALSE);
}

void CMainFrame::OnUpdateToolSettingsToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck ((m_wndToolSettingsToolbar.GetStyle () & WS_VISIBLE) ? 1 : 0);
}

	

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG

void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}  

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers





void CMainFrame::OnClose() 
{
	SaveBarState("ToolBars");	
	SaveWindowState();
	CMDIFrameWnd::OnClose();
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
//	if (nHitTest == HTCLIENT)
	//{
		//::SetCursor (m_hCursor);
		//return TRUE;
	//}

	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_wndStatusBar.SetPaneText (0,"",true);
	m_wndStatusBar.SetPaneText (1,"",true);
	m_wndStatusBar.SetPaneText (2,"",true);
	m_wndStatusBar.SetPaneText (3,"",true);
	m_wndStatusBar.SetPaneText (4,"",true);

	CMDIFrameWnd::OnMouseMove(nFlags, point);
}

bool CMainFrame::RestoreWindowState()
{
	ReadGlobalVariables();		// Neal - BUGFIX

	//RegisterGlobalVariables (GLOBAL_FLAG_RESTORE);
	TRY
	{
		CString strFileName ("");
		strFileName.Format ("%s%s", g_szAppDirectory, "Wally.dat");
		CFile File( strFileName, CFile::modeRead);
		CArchive ar( &File, CArchive::load);

		CString strSignature;
		WORD    wVersion;

		ar >> strSignature;
		ar >> wVersion;

		if (strSignature != strDataFileSignature)
		{
			AfxMessageBox( "Warning: File \"Wally.dat\" is corrupt and will be ignored.");
		}

		g_LeftPatternToolLayerInfo.Serialize( ar, TRUE);
		g_LeftDecalToolLayerInfo.Serialize( ar, TRUE);
		g_RightDecalToolLayerInfo.Serialize( ar, TRUE);
		g_LeftBulletLayerInfo.Serialize( ar, TRUE);
		g_RightBulletLayerInfo.Serialize( ar, TRUE);
		g_RivetToolLayerInfo.Serialize( ar, TRUE);
		g_RightPatternToolLayerInfo.Serialize( ar, TRUE);

		if (wVersion >= 2)
			g_CloneSourceLayerInfo.Serialize( ar, TRUE);
	}
	CATCH_ALL( e)
	{
	}
	END_CATCH_ALL


	CString WallyKey ("Settings");	
	CString BatchRegistryKey ("BatchSettings");

	///////////////////////////////////////////
	// Load Wally Options from the Registry //
	/////////////////////////////////////////

	g_WildCardList.ReadRegistry();

	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);
	GetWindowPlacement (&wp);
	CString strMainFrameKey ("MainFrame");

	if (((wp.flags =
		    theApp.GetProfileInt (strMainFrameKey, "Flags", -1)) != -1)  &&
		((wp.showCmd =
		    theApp.GetProfileInt (strMainFrameKey, "ShowCmd", -1)) != -1) &&
		((wp.rcNormalPosition.left = 
			theApp.GetProfileInt (strMainFrameKey, "Left", -1)) != -1) &&
		((wp.rcNormalPosition.top =
			theApp.GetProfileInt (strMainFrameKey, "Top", -1)) != -1) &&
		((wp.rcNormalPosition.right =
			theApp.GetProfileInt (strMainFrameKey, "Right", -1)) != -1) &&
		((wp.rcNormalPosition.bottom =
			theApp.GetProfileInt (strMainFrameKey, "Bottom", -1)) != -1))
	{
		wp.rcNormalPosition.left = min (wp.rcNormalPosition.left, 
			::GetSystemMetrics (SM_CXSCREEN) - 
			::GetSystemMetrics (SM_CXICON));
	
		wp.rcNormalPosition.top = min (wp.rcNormalPosition.top, 
			::GetSystemMetrics (SM_CYSCREEN) - 
			::GetSystemMetrics (SM_CYICON));
		SetWindowPlacement (&wp);
		return true;
	}	
	
	return false;		
}


void CMainFrame::SaveWindowState()
{	
	WriteGlobalVariables();		// Neal - BUGFIX - save now, in case Wally dies

	TRY
	{
		CString strFileName ("");
		strFileName.Format ("%s%s", g_szAppDirectory, "Wally.dat");
		
		CFile File( strFileName, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
		CArchive ar( &File, CArchive::store);

		wDataFileVersion = 2;

		ar << strDataFileSignature;
		ar << wDataFileVersion;

		g_LeftPatternToolLayerInfo.Serialize( ar, TRUE);
		g_LeftDecalToolLayerInfo.Serialize( ar, TRUE);
		g_RightDecalToolLayerInfo.Serialize( ar, TRUE);
		g_LeftBulletLayerInfo.Serialize( ar, TRUE);
		g_RightBulletLayerInfo.Serialize( ar, TRUE);
		g_RivetToolLayerInfo.Serialize( ar, TRUE);
		g_RightPatternToolLayerInfo.Serialize( ar, TRUE);
		g_CloneSourceLayerInfo.Serialize( ar, TRUE);
	}
	CATCH_ALL( e)
	{
	}
	END_CATCH_ALL

	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);

	GetWindowPlacement (&wp);
	CString strMainFrameKey ("MainFrame");

	// MainFrame window positions
	theApp.WriteProfileInt (strMainFrameKey, "Flags",   wp.flags);
	theApp.WriteProfileInt (strMainFrameKey, "ShowCmd", wp.showCmd);
	theApp.WriteProfileInt (strMainFrameKey, "Left",    wp.rcNormalPosition.left);
	theApp.WriteProfileInt (strMainFrameKey, "Top",     wp.rcNormalPosition.top);
	theApp.WriteProfileInt (strMainFrameKey, "Right",   wp.rcNormalPosition.right);
	theApp.WriteProfileInt (strMainFrameKey, "Bottom",  wp.rcNormalPosition.bottom);

	
	CString WallyKey ("Settings");		
	CString BatchRegistryKey ("BatchSettings");
		
	
	// Batch Conversion settings
	theApp.WriteProfileString	(BatchRegistryKey,    "SourceDirectory",		g_szSourceConvertDirectory);
	theApp.WriteProfileString	(BatchRegistryKey,    "DestinationDirectory",	g_szDestinationConvertDirectory);
	theApp.WriteProfileString	(BatchRegistryKey,    "WildCard",				g_szConvertWildCard);
	theApp.WriteProfileInt		(BatchRegistryKey,    "MaxThreads",				g_iMaxConversionThreads);
	theApp.WriteProfileInt		(BatchRegistryKey,    "OverWriteFiles",			g_bOverWriteFiles);	
	theApp.WriteProfileInt		(BatchRegistryKey,    "Recurse Subdirectories",	g_bRecurseSubdirectories);
	theApp.WriteProfileInt		(BatchRegistryKey,    "Retain Structure",		g_bRetainDirectoryStructure);

	CWallyUndo Undo;	// undo automatically saves its own settings in constructor
}

void CMainFrame::ApplyAmount( int iAmount)
{
	// neal - force tools to rebuild look-up tables
	BOOL bRebuild = FALSE;

	switch (g_iCurrentTool)
	{
	case EDIT_MODE_SELECTION:
		if (g_iPasteAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iPasteAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_SELECTION, 0);
		}
		break;
	case EDIT_MODE_DARKEN:
		if (g_iDarkenAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iDarkenAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_DARKEN, 0);
		}
		break;
	case EDIT_MODE_LIGHTEN:
		if (g_iLightenAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iLightenAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_LIGHTEN, 0);
		}
		break;
	case EDIT_MODE_SCRATCH:
		if (g_iScratchAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iScratchAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_SCRATCH, 0);
		}
		break;
	case EDIT_MODE_TINT:
		if (g_iTintAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iTintAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_TINT, 0);
		}
		break;
	case EDIT_MODE_BLEND:
		if (g_iBlendAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iBlendAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_BLEND, 0);
		}
		break;
	case EDIT_MODE_SHARPEN:
		if (g_iSharpenAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iSharpenAmount = iAmount;
			ASSERT( FALSE);		// implement
//			PostMessage( WM_COMMAND, ID_TP_SHARPEN, 0);
		}
		break;
	case EDIT_MODE_RECOLOR:
		if (g_iRecolorAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iRecolorAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_RECOLOR, 0);
		}
		break;
	case EDIT_MODE_SPRAY:
		if (g_iSprayAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iSprayAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_SPRAY, 0);
		}
		break;
	case EDIT_MODE_SPRAY_RECOLOR:
		if (g_iSprayRecolorAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iSprayRecolorAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_SPRAY_RECOLOR, 0);
		}
		break;
	case EDIT_MODE_BULLET_HOLES:
		if (g_iBulletHoleAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iBulletHoleAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_BULLET_HOLES, 0);
		}
		break;
	case EDIT_MODE_RIVETS:
		if (g_iRivetAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iRivetAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_RIVETS, 0);
		}
		break;
	case EDIT_MODE_PATTERNED_PAINT:
		if (g_iPatternPaintAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iPatternPaintAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_PATTERNED_PAINT, 0);
		}
		break;
	case EDIT_MODE_DECAL:
		if (g_iDecalAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iDecalAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_DECAL, 0);
		}
		break;
	case EDIT_MODE_CLONE:
		if (g_iCloneAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iCloneAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_CLONE, 0);
		}
		break;
	case EDIT_MODE_RUBBER_STAMP:
		if (g_iCloneAmount != iAmount)
		{
			bRebuild = TRUE;
			g_iCloneAmount = iAmount;
			PostMessage( WM_COMMAND, ID_TP_RUBBER_STAMP, 0);
		}
		break;
	default:
		ASSERT( FALSE);		// implement?
	}

	if (bRebuild)
		g_iLastLeftColor = g_iLastRightColor = -1;
}

void CMainFrame::OnApplyLeast()
{
	ApplyAmount( 0);
}

void CMainFrame::OnApplyALittleBit()
{
	ApplyAmount( 1);
}

void CMainFrame::OnApplyMedium()
{
	ApplyAmount( 2);
}

void CMainFrame::OnApplyMore()
{
	ApplyAmount( 3);
}

void CMainFrame::OnApplyMost()
{
	ApplyAmount( 4);
}

void CMainFrame::OnUpdateApply( CCmdUI* pCmdUI, int iTestAmount)
{
	int iAmount = -1;

	switch (g_iCurrentTool)
	{
	case EDIT_MODE_SELECTION:
		iAmount = g_iPasteAmount;
		break;
	case EDIT_MODE_DARKEN:
		iAmount = g_iDarkenAmount;
		break;
	case EDIT_MODE_LIGHTEN:
		iAmount = g_iLightenAmount;
		break;
	case EDIT_MODE_SCRATCH:
		iAmount = g_iScratchAmount;
		break;
	case EDIT_MODE_TINT:
		iAmount = g_iTintAmount;
		break;
	case EDIT_MODE_BLEND:
		iAmount = g_iBlendAmount;
		break;
	case EDIT_MODE_SHARPEN:
		iAmount = g_iSharpenAmount;
		break;
	case EDIT_MODE_RECOLOR:
		iAmount = g_iRecolorAmount;
		break;
	case EDIT_MODE_SPRAY:
		iAmount = g_iSprayAmount;
		break;
	case EDIT_MODE_SPRAY_RECOLOR:
		iAmount = g_iSprayRecolorAmount;
		break;
	case EDIT_MODE_BULLET_HOLES:
		iAmount = g_iBulletHoleAmount;
		break;
	case EDIT_MODE_RIVETS:
		iAmount = g_iRivetAmount;
		break;
	case EDIT_MODE_PATTERNED_PAINT:
		iAmount = g_iPatternPaintAmount;
		break;
	case EDIT_MODE_DECAL:
		iAmount = g_iDecalAmount;
		break;
	case EDIT_MODE_RUBBER_STAMP:
	case EDIT_MODE_CLONE:
		iAmount = g_iCloneAmount;
		break;
	default:
		ASSERT( FALSE);		// implement?
	}
	pCmdUI->SetCheck( iAmount == iTestAmount);
}

void CMainFrame::OnUpdateApplyLeast( CCmdUI* pCmdUI)
{
	OnUpdateApply( pCmdUI, 0);
}

void CMainFrame::OnUpdateApplyALittleBit( CCmdUI* pCmdUI)
{
	OnUpdateApply( pCmdUI, 1);
}

void CMainFrame::OnUpdateApplyMedium( CCmdUI* pCmdUI)
{
	OnUpdateApply( pCmdUI, 2);
}

void CMainFrame::OnUpdateApplyMore( CCmdUI* pCmdUI)
{
	OnUpdateApply( pCmdUI, 3);
}

void CMainFrame::OnUpdateApplyMost( CCmdUI* pCmdUI)
{
	OnUpdateApply( pCmdUI, 4);
}

void CMainFrame::OnShapeSquare() 
{
	g_iBrushShape = SHAPE_SQUARE;
}

void CMainFrame::OnShapeCircular() 
{
	g_iBrushShape = SHAPE_CIRCULAR;
}

void CMainFrame::OnShapeDiamond() 
{
	g_iBrushShape = SHAPE_DIAMOND;
}

void CMainFrame::OnUpdateShapeSquare( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iBrushShape == SHAPE_SQUARE);
}

void CMainFrame::OnUpdateShapeCircular( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iBrushShape == SHAPE_CIRCULAR);
}

void CMainFrame::OnUpdateShapeDiamond( CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( g_iBrushShape == SHAPE_DIAMOND);
}


BOOL CMainFrame::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
/*
	NMHDR* pNotifyMsgHdr = (NMHDR* )lParam;

	// neal - this is where we can handle custom
	// toolbar stuff like: 
	//
	// dbl-click eraser to erase image
	// dbl-click zoom to set zoom to 1:1 (or just fits?)
	// dbl-click color-replacer to replace all
	// dbl-click pattern-paint to paint all
	// dbl-click bullet hole to draw a random hole

	if (pNotifyMsgHdr->code == NM_DBLCLK)
	{
		if (pNotifyMsgHdr->idFrom == ID_IMAGE_EDIT_TOOLBAR)
		{
			//int iBreakPoint = 0;
		}
	}
	else if (pNotifyMsgHdr->code == NM_RCLICK)
	{
		if (pNotifyMsgHdr->idFrom == ID_IMAGE_EDIT_TOOLBAR)
		{
			//int iBreakPoint = 0;
		}
	}
*/
	return CMDIFrameWnd::OnNotify( wParam, lParam, pResult);
}
/*

	RECT MyRect;	
	int iIndex = m_wndStatusBar.CommandToIndex(ID_STATUSBAR_PROGRESS);
	m_wndStatusBar.GetItemRect(iIndex, &MyRect);

	if (!m_bProgressCreated)
	{
		//Create the progress control
		g_ctProgress.Create(WS_VISIBLE|WS_CHILD, MyRect, &m_wndStatusBar, 1);
		m_bProgressCreated = TRUE;
		theApp.SetProgressBarInfo (0, 100, 1);
	}

*/

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	SetActiveWindow();      // activate us first !
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	/*

	Ty-  None of this should be needed now; there aren't any types supported
	by ImageHelper that are not CWallyDoc items, as well.

	bool bNonGameType = false;
	CImageHelper ihHelper;

	for (UINT iFile = 0; iFile < nFiles; iFile++)
	{
		TCHAR szFileName[_MAX_PATH];		
		::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
		if (ihHelper.IsNonGameType (szFileName))
		{
			bNonGameType = true;
			iFile = nFiles;
		}		
	}
	
	int iFileType = 0;


	if ((g_iFileTypeDefault == FILE_TYPE_PROMPT) && (bNonGameType))
	{
		CGameSelectDlg dlgGameSelect;
		dlgGameSelect.SetMessageType (GAME_SELECT_MSG_DRAGDROP);
		if (dlgGameSelect.DoModal() != IDOK)
		{
			::DragFinish(hDropInfo);
			return;
		}
		iFileType = dlgGameSelect.GetFileType();
		g_iFileTypeDragDrop = g_iFileTypeNew;
	}	
	else
	{
		iFileType = g_iFileTypeDefault;	
	}
		
	if (g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD)
	{			
		CPackageDoc *pPackageDoc = theApp.CreatePackageDoc();		
		CPackageView *pPackageView = pPackageDoc->GetView();		
		
		for (iFile = 0; iFile < nFiles; iFile++)
		{
			TCHAR szFileName[_MAX_PATH];
			::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);			
			
			pPackageView->ImportImage (szFileName);			
		}		
	}
	else
	{	
	*/		
		for (UINT iFile = 0; iFile < nFiles; iFile++)
		{
			TCHAR szFileName[_MAX_PATH];
			::DragQueryFile(hDropInfo, iFile, szFileName, _MAX_PATH);
			theApp.OpenDocumentFile(szFileName);
		}
	//}
	::DragFinish(hDropInfo);
}

void CMainFrame::OnShowFineGrid()
{	
	g_bShowFineGrid = !g_bShowFineGrid;
	theApp.UpdateAllDocs();
}
void CMainFrame::OnShowCourseGrid()
{	
	g_bShowCourseGrid = !g_bShowCourseGrid;
	theApp.UpdateAllDocs();
}

void CMainFrame::OnUpdateShowFineGrid( CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( g_bShowFineGrid);
}
void CMainFrame::OnUpdateShowCourseGrid( CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( g_bShowCourseGrid);
}

void CMainFrame::OnShowSubMips()
{
	g_bShowSubMips = !g_bShowSubMips;
	theApp.UpdateAllDocs( HINT_UPDATE_IMAGE_SIZE);
}

void CMainFrame::OnUpdateShowSubMips( CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( g_bShowSubMips);
}

#if 0

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{

	//return CMDIFrameWnd::OnEraseBkgnd(pDC);

	CRect rcItem;
	GetClientRect( &rcItem);

	int rcWidth = rcItem.Width();
	int rcHeight = rcItem.Height();	
	
	CDC DC;
	DC.CreateCompatibleDC( pDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( pDC, rcWidth, rcHeight);
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = CreateSolidBrush( RGB( 0xff, 0x80, 0x80 ));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, rcWidth, rcHeight, PATCOPY);
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);	
	
	pDC->BitBlt( 0, 0, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);
	DC.DeleteDC();

	return TRUE;
}

#endif