/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// Wally.cpp : Defines the class behaviors for the application.
//
// Created by Ty Matthews, 1-17-1998
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// ***** **  ***   **
//   *  *  * *  * *  *  *
//   *  *  * *  * *  * 
//   *  *  * *  * *  *  *
//   *   **  ***   **
//
/////////////////////////////////////////////////////////////////////////////
/*
<font color="#ffff88">

Bugs:

</font>
<ul> 
<li>Any known major bugs ALWAYS have priority...
<li>Polyline freaks out when you use the scroll bars while drawing. 
</ul>
<br>
<font color="#ffff88">

Textures:

</font>
<ul>
<li>Fractal Clouds
<li>Wood grain
<li>Fractal textures
<li>Wood planks (sort of like tiled marble)
<li>Metal (brushed, panels, etc.)
<li>Liquids
</ul>
<br>
<font color="#ffff88">

Tools:

</font>
<ul>
<li>Smooth antiliased brush, airbrush
<li>Circle/Ellipse
<li>Text
<li>Flood Fill Hue-Saturation only (Flood Fill Recolor tool)
<li>Use SpotMarker to implement "Fade Last Stroke" (need one per doc?)
<li>Sharpen 
<li>Smudge tool
<li>Toggle spray tools to either blend or exact color placement
<li>Spray Darken/Lighten
<li>Polygon, Regular Polygon
<li>Drop Shadow
<li>Drip tool
<li>Add color tolerance (+/- Range) to appropriate tools for improved 24-bit support
	<ul>
	<li>Magic Wand
	</ul>
</ul>
<br>
<font color="#ffff88">

Filters:

</font>
<ul>
<li>Combine
<li>Minimum/Maximum
<li>Hue and Saturation filter
<li>Historgram Equalization filter
<li>Resize / Resample
<li>Buttonize (Neal needs more info -- Please send email)
<li>Bump Map
<li>Add an "Apply" Button
<li>MMX support (???)
<li>Remove Seams Filter
</ul>
<br>
<font color="#ffff88">

Other Games And Image File Formats:

</font>
<ul>
<li>Quake 3: Arena (full 32 bit support with alpha channel)
<li>PNG file format
<li>Tribes ???
<li>Unreal
<li>Alien Vs Predator
</ul>
<br>
<font color="#ffff88">

Main Wally Features:

</font>
<ul>
<li>Alpha Channel editing
<li>Snap to grid
<li>File | Print
<li>...for .WADs (Halflife, Quake 1 & 2), thumbnails of the included 
   pictures are printed (also, option: print thumbnails of selected only)
<li>...for .PAK files: print directory and file tree in two or three columns, 
   listing all files contained within.
</ul>
<br>
<font color="#ffff88">

Miscellaneous Wally Options:

</font>
<ul>
<li>"Set as Wallpaper" option
</ul>
<br>
<font color="#ffff88">

Cut and paste tools:

</font>
<ul>
(Neal: these are very time consuming to implement and the
code may be broken for weeks at a time, so that's why they
are here at the end of the list.  I'd like to have them, but I
feel it is more important to provide tools and features you
can't find anywhere else.  You can always use PaintShop Pro
for cutouts in the meantime.)
<br>
<br>
<li>Rectangular cutout tool (partially completed)
<li>Freehand Resizing
<li>Scissors
<li>Lasso
<li>Magic Wand
<li>Select | Drawing Tool Fill Area (turns last stroke into a selection)
</ul>
<br>
<font color="#ffff88">

Recently Completed (in version 1.56B, not yet released):
</font>
<ul>
<li>Image Resize can now stretch to any size (whole image only)
<li>Print and Print Preview for single pictures (.BMP, .PNG, .JPG, .WAL, etc.), 
    the image prints alone or with submips and/or tiling
<li>New Fix Uneven Lighting Filter, for photographic textures, works best on
    images that are basically the same color shade.
<li>Much faster Color Optimization, for quicker loads of 24 bit images
<li>An automatic "what's new" message
<li>BUGFIX: BMPs File / New initial size is wrong (first time only) 
<li>BUGFIX: Scrolled view windows have clipping problem
<li>BUGFIX: Zoom way out, view is too small
<li>BUGFIX: Missing sub-mips (in view) on File New
<li>BUGFIX: Mis-aligned background when scrolling
</ul>
<br>
<font color="#ffff88">

Completed:
</font>
<ul>
<li>BUGFIX: Undo doesn't clear document modified flag (minor)
<li>Update drawing tools to use an RGB color, instead of a palette index
<li>24 bit TGA/JPG support for Quake 3: Arena
<li>Add color tolerance (+/- Range) to appropriate tools for improved 24-bit support
	<ul>
	<li>Color Replacer
	<li>Recolor
	<li>Spray Recolor
	<li>Flood Fill
	</ul>
<li>Browsing/saving/loading from .PAK files
<li>Tools are now fully 24 bit (internally)
<li>Clone and Rubber Stamp
<li>Speed ups
<li>Overhaul of browsing textures:
	<ul>
	<li>Move to a split-pane explorer view for easier directory navigation
	<li>Add rename/delete/move operations
	<li>Add scrolling with arrow keys and PgUp/PgDn
	<li>Clean-up thumbnail views for images larger than 128x128
	<li>Animated texture view; also animate liquid textures
	<li>Randomized texture view for Half-Life textures starting with '-'
	</ul>
<li>Improved Drawing Tools toolbar - resizable; double-wide in 800x600 res
<li>Decal Tool
<li>Soften (Water Drop blend tool)
<li>SiN and Heretic2 game format support
<li>Marble grout is now drawn properly
<li>Zoom-Out (for editing large textures that don't fit entirely on screen)
<li>Quake1 WAD files
<li>Grid
<li>Rectangular Selection (masking)
<li>Mirror Filter
<li>Limited Resizing Filter
<li>TGA support
</ul>
<br>
*/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <mmsystem.h>	// requires linking with "winmm.lib"
#include "Wally.h"
#include "Afxext.h"

#include "MainFrm.h"
#include "Tool.h"

#include "ChildFrm.h"
#include "WallyDoc.h"
#include "BrowseChildFrm.h"
#include "BrowseDoc.h"
#include "BrowseView.h"

#include "PackageChildFrm.h"
#include "PackageDoc.h"
#include "PackageView.h"
#include "PackageBrowseView.h"

#include "BuildDoc.h"
#include "BuildView.h"
#include "BuildChildFrm.h"

#include "WLYDoc.h"
#include "WLYView.h"
#include "WLYChildFrm.h"

#include "DebugDibView.h"
#include "DebugDoc.h"
#include "DebugChildFrm.h"

#include "PakDoc.h"
#include "PakTreeView.h"
#include "PakListView.h"
#include "PakChildFrm.h"

#include "MiscFunctions.h"
#include "GraphicsFunctions.h"
#include "WallyDocTemplate.h"
#include "ClipboardDIB.h"
#include "Splash.h"
#include "ReMip.h"
#include "WallyPropertySheet.h"
#include "Filter.h"
#include "ImageHelper.h"
#include "BatchDlg.h"
#include "NewImageDlg.h"
#include "GameSelectDlg.h"
#include "RenameImageDlg.h"
#include "ColorOpt.h"
#include "DirectoryList.h"

#include "WndList.h"
#include "RulesDlg.h"
#include "Genesis3D.h"
#include "RegistryHelper.h"

#include "CustomResource.h"
#include "ColorDecalDlg.h"
#include "WadMergeDlg.h"
#include "2PassScale.h"

#include <list>

using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_FLICKER 901

/////////////////////////////////////////////////////////////////////////////

void WhatsNew(void)
{
	AfxMessageBox(
//		"Neal - ALWAYS save existing text by commenting it out (for future reference)"
//		""
		"What's New in version 1.56B of Wally?\n"
		"\n"
		" * Image Resize can now stretch to any size (whole image only)\n"
		" * Print and Print Preview\n"
		" * New Fix Uneven Lighting Filter, for photographic textures,\n"
		"    works best on images that are basically the same color shade.\n"
		" * Much faster Color Optimization, for quicker loads of 24 bit images\n"
		" * New Crop, Resize, and Transparency Blend features in Decal Wizard\n"
		" * Slider control for size of thumbnails in WAD viewer browse mode\n"
		" * This automatic \"what's new\" message\n"		
		"\n"
		"Bugs fixed:\n"
		"\n"
		" * BMPs File / New initial size is wrong (first time only)\n"
		" * Scrolled view windows have clipping problem\n"
		" * Zoom way out, view is too small\n"
		" * Missing sub-mips (in view) on File New\n"
		" * Mis-aligned background when scrolling\n"
		" * Export to 8-bit PCX has entirely black palette\n"
/*

		"What's New in version 1.56B of Wally?\n"
		"\n"
		" * Image Resize can now stretch to any size (whole image only)\n"
		" * Print and Print Preview\n"
		" * New Fix Uneven Lighting Filter, for photographic textures,\n"
		"    works best on images that are basically the same color shade.\n"
		" * Much faster Color Optimization, for quicker loads of 24 bit images\n"
		" * New Crop, Resize, and Transparency Blend features in Decal Wizard\n"
		" * Slider control for size of thumbnails in WAD viewer browse mode\n"
		" * This automatic \"what's new\" message\n"
		"\n"
		"Bugs fixed:\n"
		"\n"
		" * BMPs File / New initial size is wrong (first time only)\n"
		" * Scrolled view windows have clipping problem\n"
		" * Zoom way out, view is too small\n"
		" * Missing sub-mips (in view) on File New\n"
		" * Mis-aligned background when scrolling\n"
		" * Export to 8-bit PCX has entirely black palette\n"
*/
		, MB_OK | MB_ICONINFORMATION);
}

BOOL LoadDefaultEditingPalette( LPBYTE pbyPalette, CWallyPalette *pPalette, int iNumColors)
{
	if (g_strDefaultEditingPalette == "")
	{
		g_strDefaultEditingPalette.Format ("%s\\blend.pal", TrimSlashes (g_strPaletteDirectory));
	}

	CImageHelper ihHelper;
	ihHelper.LoadImage (g_strDefaultEditingPalette);

	if (ihHelper.GetErrorCode() == IH_SUCCESS)
	{		
		if (ihHelper.IsValidPalette())
		{
			if (pbyPalette)
			{
				CopyMemory (pbyPalette, ihHelper.GetBits(), iNumColors * 3);
			}
			
			if (pPalette)
			{				
				pPalette->SetPalette (ihHelper.GetBits(), iNumColors);
			}

			return TRUE;
		}
	}
	else
	{
		// ToDo:  should we load up a palette from a built-in resource (aka hard-coded) or 
		// should we just let this pass through to FALSE so an optimized palette will be used?
		// Do we care now... because we'll have a better color picker in the future?
	}
	
	return FALSE;	
}

/////////////////////////////////////////////////////////////////////////////
// CWallyApp

BEGIN_MESSAGE_MAP(CWallyApp, CWinApp)
	//{{AFX_MSG_MAP(CWallyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)		
	ON_COMMAND(ID_EDIT_PASTE_AS_NEW_IMAGE, OnEditPasteAsNewImage)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_AS_NEW_IMAGE, OnUpdateEditPasteAsNewImage)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_BROWSE, OnFileBrowse)
	ON_COMMAND(ID_FILE_CONVERT, OnFileConvert)
	ON_COMMAND(ID_FILE_BATCH_CONVERSION, OnFileBatchConversion)
	ON_COMMAND(ID_FILE_LOAD_Q2PALETTE, OnFileLoadQ2palette)
	ON_COMMAND(ID_FILE_LOAD_Q1PALETTE, OnFileLoadQ1palette)
	ON_COMMAND(ID_EDIT_PASTE_SPECIALEFFECTS, OnEditPasteSpecialEffects)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SPECIALEFFECTS, OnUpdateEditPasteSpecialEffects)
	ON_COMMAND(ID_VIEW_RULES, OnViewRules)
	ON_COMMAND(ID_FILE_TEST_TXL, OnFileTestTxl)
	ON_UPDATE_COMMAND_UI(ID_FILE_TEST_TXL, OnUpdateFileTestTxl)
	ON_COMMAND(ID_FILE_DEBUG, OnFileDebug)
	ON_UPDATE_COMMAND_UI(ID_FILE_DEBUG, OnUpdateFileDebug)
	ON_COMMAND(ID_WIZARD_COLOR_DECAL, OnWizardColorDecal)
	ON_COMMAND(ID_WIZARD_WADMERGE, OnWizardWadmerge)
	ON_COMMAND(ID_WIZARD_TEST, OnWizardTest)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyApp construction

CWallyApp::CWallyApp()
{	
	InitRandom( 111);
}

// The one and only
CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWallyApp initialization

BOOL CWallyApp::InitInstance()
{

#ifndef _DEBUG
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}
#endif

	AfxEnableControlContainer();
	AfxOleInit();

#ifdef _DEBUG
	afxMemDF = checkAlwaysMemDF;
#endif

	// For the package views:
	g_iPackageFormat = RegisterClipboardFormat ("Wally package format");

	// For browse copy/paste moves between directories:
	g_iBrowseCopyPasteFormat = RegisterClipboardFormat ("Wally browse copy-paste format");
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Grab the full path to Wally
	char szAppDir[_MAX_PATH];
	GetModuleFileName (NULL, szAppDir, _MAX_PATH);
	g_szAppDirectory = GetPathToFile(szAppDir);

	g_strPaletteDirectory = g_szAppDirectory + "Palettes";
	g_strDecalDirectory = g_szAppDirectory + "Decals";
	g_strBrowseCacheDirectory = g_szAppDirectory + "Browser Cache";
	g_strTempDirectory = g_szAppDirectory + "Temp";
	
	_mkdir (g_strPaletteDirectory);	
	_mkdir (g_strDecalDirectory);
	_mkdir (g_strBrowseCacheDirectory);
	_mkdir (g_strTempDirectory);

	SetRegistryKey(_T("Team BDP"));
	m_nClipboardFormat = ::RegisterClipboardFormat(_T("Quake2 .wal file"));

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
	
	// This needs to be here, as people can drag BMP or PCX files to the icon
	// and start rebuilding Sub-Mips before the mainframe has even opened.
	RegisterGlobalVariables();
	ReadGlobalVariables();
		
	g_bAutoRemip = GetProfileInt ("ReMipDLX", "AutoRemipOnImport",0);
	g_strQuake2Palette		= theApp.GetProfileString	("Settings",	"Quake2 Palette",	"");
	g_strQuake1Palette		= theApp.GetProfileString	("Settings",	"Quake1 Palette",	"");
	g_iUseDefaultQ2Palette	= theApp.GetProfileInt		("Settings",	"Use Default Q2",	1);
	g_iUseDefaultQ1Palette	= theApp.GetProfileInt		("Settings",	"Use Default Q1",	1);
	

	// Ty- delete the "ShellNew" items for WADs and WALs.  WLY is now the standard Wally format
	char szKeys[2][10] = { ".wad", ".wal"};

	for (int j = 0; j < 2; j++)
	{
		CRegistryHelper rhHelper;
		rhHelper.SetMainKey (HKEY_CLASSES_ROOT);

		CString strRegApp("");
		CString strExtension(szKeys[j]);
		CString strKeyName("");
		
		rhHelper.AddItem (&strRegApp, "", "", strExtension);
		rhHelper.ReadRegistry();

		if (!strRegApp.CompareNoCase ("Wally.Document"))
		{
			// Only yank if Wally is the owner of this type.
			strKeyName.Format ("%s\\ShellNew", strExtension);
			RegDeleteKey (HKEY_CLASSES_ROOT, strKeyName);
		}
	}		
	
	// Load up the palettes from disk		
	LoadQ2PaletteFromDisk();
	LoadQ1PaletteFromDisk();
	
	// Set up some of the Global variables
	g_iDocWidth = 0;
	g_iDocHeight = 0;
	g_iCurrentTool = 0;	

	m_bBrowseOpen = FALSE;
	m_bProgressCreated = FALSE;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	
	WallyDocTemplate = new CWallyDocTemplate(
		IDR_WALLYTYPE,
		RUNTIME_CLASS(CWallyDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CWallyView));
	AddDocTemplate(WallyDocTemplate);

	PackageDocTemplate = new CMultiDocTemplate(
		IDR_PACKAGE_TYPE,
		RUNTIME_CLASS(CPackageDoc),
		RUNTIME_CLASS(CPackageChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CPackageView));
	AddDocTemplate(PackageDocTemplate); 

	BuildDocTemplate = new CMultiDocTemplate(
		IDR_BUILDTYPE,
		RUNTIME_CLASS(CBuildDoc),
		RUNTIME_CLASS(CBuildChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CBuildView));
	AddDocTemplate(BuildDocTemplate);

	BrowseDocTemplate = new CMultiDocTemplate(
		IDR_BROWSETYPE,
		RUNTIME_CLASS(CBrowseDoc),
		RUNTIME_CLASS(CBrowseChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CBrowseView));
	AddDocTemplate(BrowseDocTemplate); 
	
	PakDocTemplate = new CMultiDocTemplate(
		IDR_PAK_TYPE,
		RUNTIME_CLASS(CPakDoc),
		RUNTIME_CLASS(CPakChildFrm),
		RUNTIME_CLASS(CPakListView));
	AddDocTemplate(PakDocTemplate);		

	WLYDocTemplate = new CMultiDocTemplate(
		IDR_WLY_TYPE,
		RUNTIME_CLASS(CWLYDoc),
		RUNTIME_CLASS(CWLYChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CWLYView));
	AddDocTemplate(WLYDocTemplate);

	DebugDibTemplate = new CMultiDocTemplate(
		IDR_WALLYTYPE,
		RUNTIME_CLASS(CDebugDoc),
		RUNTIME_CLASS(CDebugChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CDebugDibView));
	AddDocTemplate(DebugDibTemplate);
	
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;	

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();	
	RegisterShellFileTypes(TRUE);
		
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	CParseCommandLine CommandLineParser;
		
	if ((cmdInfo.m_nShellCommand != CCommandLineInfo::FileDDE) && (!cmdInfo.m_bRunAutomated))
	{
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		if (m_lpCmdLine[0] == '\0')
		{			
		}
		else
		{    
			// Setup the commandline parser class, to be used shortly...
			CommandLineParser.Parse(m_lpCmdLine);	
		}  
	}
		
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	if (!pMainFrame->RestoreWindowState())
		pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();	

	// Set a char pointer to the first string in the command line	
	CommandLineParser.GetFirst();
	char *FileName = CommandLineParser.GetNext();				
	BeginWaitCursor();

	// Spin through the list of filenames, our CommandLineParser routines.
	// This code is for opening of non-Wal files.  We need to be able to determine
	// the file extension type, and call the appropriate loading of the document.

	while (FileName != NULL)			
	{							
		// Go grab the file extension
		CString strFileExtension = GetExtension(FileName);		
		if(
			(strFileExtension == ".wal") || 
			(strFileExtension == ".mip") || 
			(strFileExtension == ".swl") || 
			(strFileExtension == ".m8")			
		)			
		{
			WallyDocTemplate->OpenDocumentFile(FileName);
		}
		else
		if( strFileExtension == ".pak" )
		{
			PakDocTemplate->OpenDocumentFile(FileName);			
		}
		else
		if (strFileExtension == ".wad")
		{
			PackageDocTemplate->OpenDocumentFile( FileName);
		}
		else
		{
			OpenNonWalFile (FileName);
		}
		FileName = CommandLineParser.GetNext();
	}   
	EndWaitCursor();

	return TRUE;
}

int CWallyApp::ExitInstance() 
{
	WriteGlobalVariables();
	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#define HIT_NONE		0
#define HIT_WWW_WALLY	1
#define HIT_BOTH		2
#define HIT_TY			3
#define HIT_NEAL		4
#define HIT_BOARD		5

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_ctrlBitmapArea;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT  HitTest( CPoint ptPos);
	CRect HitRect( UINT uHitCode);

	//{{AFX_MSG(CAboutDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_BMP, m_ctrlBitmapArea);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWallyApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_DLG)
		return (HBRUSH )GetStockObject( BLACK_BRUSH);
	else
		return hbr;
}
/////////////////////////////////////////////////////////////////////////////
// CWallyApp commands

void CWallyApp::OnEditPasteAsNewImage()
{
	CDibSection ClipboardDIB;
	
	if (!ClipboardDIB.InitFromClipboard(m_pMainWnd))
	{
		AfxMessageBox ("Failed to open clipboard!", MB_ICONSTOP);
		return;
	}
	
	UINT iWidth = 0;
	UINT iHeight = 0;
	UINT iBPP = 0;
	UINT iSize = 0;
	UINT iFileType = g_iFileTypeDefault;
	UINT j, b, g, r, x, y;
	UINT iOffset = 0;

	BOOL bNon16 = FALSE;
	div_t dtWidth, dtHeight;

	CString strTitle("");
	CString strName (g_strDefaultTextureName);
	
	CPackageDoc   *pPackageDoc = NULL;
	CWallyDoc     *pWallyDoc   = NULL;
	CWallyPalette *pDocPalette = NULL;
	CWallyView    *pWallyView  = NULL;
	CLayer        *pDocLayer   = NULL;	
	
	BYTE           byClipboardPalette[768];
	BYTE           byDocPalette[768];
	LPBYTE         pbyClipboardData	= NULL;
	CMemBuffer     mbClipboardData;
	CMemBuffer     mbIRGBData;

	iWidth  = ClipboardDIB.GetWidth();
	iHeight = ClipboardDIB.GetHeight();
	iBPP    = ClipboardDIB.GetBitCount();

	if ((iWidth > MAX_TEXTURE_WIDTH) || (iHeight > MAX_TEXTURE_HEIGHT))
	{
		CString strError ("");
		strError.Format ("This texture is too large.  Textures must be less than %d x %d in size.", MAX_TEXTURE_WIDTH, MAX_TEXTURE_HEIGHT);
		AfxMessageBox (strError, MB_ICONSTOP);
		return;
	}
	
	dtWidth  = div (iWidth, 16);
	dtHeight = div (iHeight, 16);
	if ((dtWidth.rem != 0) || (dtHeight.rem != 0))
	{
		bNon16 = TRUE;
	}
			
	g_iDocWidth      = iWidth;
	g_iDocHeight     = iHeight;
	g_iDocColorDepth = iBPP;
	
	if (g_iFileTypeDefault == FILE_TYPE_PROMPT)
	{
		CGameSelectDlg dlgGameSelect;
		if (dlgGameSelect.DoModal() != IDOK)
		{
			return;
		}
		iFileType = dlgGameSelect.GetFileType();
	}	
	
	strTitle.Format ("Untitled%d", g_iUntitledCount++);

	CRenameImageDlg renameDlg;	

	switch (iFileType)
	{		
	case FILE_TYPE_QUAKE1_WAD:
		if (bNon16)
		{
			AfxMessageBox ("Image on clipboard is not evenly divisible by 16.", MB_ICONSTOP);
			return;
		}
		pPackageDoc = (CPackageDoc *)PackageDocTemplate->OpenDocumentFile (NULL, TRUE);		
		pPackageDoc->SetWADType (WAD2_TYPE);
		pPackageDoc->SetTitle (strTitle);

		// Fall through...

	case FILE_TYPE_QUAKE1:			
		if (renameDlg.DoModal() == IDOK)
		{
			strName = renameDlg.GetName();
		}
		else
		{
			return;
		}
		// Go create a new document/view
		g_iDocColorDepth = 8;
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);		
		pWallyDoc->SetGameType (FILE_TYPE_QUAKE1);
		pWallyDoc->SetName (strName);
		pWallyDoc->SetTitle (strName);
		break;
	
	case FILE_TYPE_HALF_LIFE_WAD:
		if (bNon16)
		{
			AfxMessageBox ("Image on clipboard is not evenly divisible by 16.", MB_ICONSTOP);
			return;
		}
		pPackageDoc = (CPackageDoc *)PackageDocTemplate->OpenDocumentFile (NULL, TRUE);
		pPackageDoc->SetTitle (strTitle);
		pPackageDoc->SetWADType (WAD3_TYPE);

		// Fall through...
	
	case FILE_TYPE_HALF_LIFE:			
		if (renameDlg.DoModal() == IDOK)
		{
			strName = renameDlg.GetName();
		}
		else
		{
			return;
		}
		// Go create a new document/view
		g_iDocColorDepth = 8;
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);
		
		pWallyDoc->SetGameType (FILE_TYPE_HALF_LIFE);
		pWallyDoc->SetName (strName);
		pWallyDoc->SetTitle (strName);
		break;		

	case FILE_TYPE_QUAKE2:
		// Go create a new document/view
		g_iDocColorDepth = 8;
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);

		pWallyDoc->SetGameType( iFileType);
		pWallyDoc->SetTitle( strTitle);
		pWallyDoc->SetName( strName);
		break;

	case FILE_TYPE_SIN:
		// Go create a new document/view
		g_iDocColorDepth = 8;
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);
		
		pWallyDoc->SetGameType (iFileType);
		pWallyDoc->SetTitle (strTitle);
		pWallyDoc->SetName (strName);
		break;

	case FILE_TYPE_HERETIC2:
		g_iDocColorDepth = 8;
		// Go create a new document/view
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);
		
		pWallyDoc->SetGameType (iFileType);
		pWallyDoc->SetTitle (strTitle);
		pWallyDoc->SetName (strName);
		break;

	case FILE_TYPE_TGA:
	case FILE_TYPE_PCX:
	case FILE_TYPE_BMP:
	case FILE_TYPE_JPG:
	case FILE_TYPE_PNG:
	case FILE_TYPE_TEX:
		pWallyDoc = (CWallyDoc*)WallyDocTemplate->OpenDocumentFile( NULL, TRUE);
		
		pWallyDoc->SetGameType (iFileType);
		pWallyDoc->SetTitle (strTitle);
		pWallyDoc->SetName (strName);
		break;

	default:
		ASSERT (FALSE);		// Unhandled file type?
		break;
	}

	ASSERT (pWallyDoc);
	pWallyView = pWallyDoc->GetView();

	// Point to the various doc items
	pDocPalette = pWallyDoc->GetPalette();
	pDocLayer   = pWallyDoc->GetCurrentLayer();
		
	// All formats will end up in COLOR_IRGB
	iSize = iWidth * iHeight;
	LPCOLOR_IRGB pIRGBData = (LPCOLOR_IRGB)mbIRGBData.GetBuffer (iSize * sizeof( COLOR_IRGB));		

	if (!pIRGBData)
	{
		AfxMessageBox( "Error: CWallyApp::OnEditPasteAsNewImage() - Out of Memory - pTemp24Bit == NULL");
		return;
	}

	switch (iBPP)		// Color depth
	{
	case 8:
		{
			// Build the buffer so we can work with it
			pbyClipboardData = mbClipboardData.GetBuffer (iSize);
			
			if (!pbyClipboardData)
			{
				AfxMessageBox( "Error: CWallyApp::OnEditPasteAsNewImage() - Out of Memory - pbyClipboardData == NULL");
				return;
			}

			ClipboardDIB.GetRawBits (pbyClipboardData);
			CopyMemory (byClipboardPalette, ClipboardDIB.GetPalette(), 768);

			switch (g_iPaletteConversion)
			{
			case PALETTE_CONVERT_MAINTAIN:
				// Just jam it in, no conversion necessary
				switch (iFileType)
				{
				case FILE_TYPE_QUAKE2:
					{
						pDocPalette->SetPalette (quake2_pal, 256);
					}
					break;

				case FILE_TYPE_QUAKE1:
				case FILE_TYPE_QUAKE1_WAD:
					{
						pDocPalette->SetPalette (quake1_pal, 256);
					}
					break;

				case FILE_TYPE_HERETIC2:
				case FILE_TYPE_SIN:
				case FILE_TYPE_HALF_LIFE:
				case FILE_TYPE_HALF_LIFE_WAD:
				case FILE_TYPE_TGA:
				case FILE_TYPE_BMP:
				case FILE_TYPE_PCX:
				case FILE_TYPE_JPG:
				case FILE_TYPE_PNG:
				case FILE_TYPE_TEX:
					{
						pDocPalette->SetPalette (byClipboardPalette, 256);
					}
					break;

				default:
					ASSERT (FALSE);
					return;
					break;
				}				
				break;
				
			case PALETTE_CONVERT_NEAREST:				
				{					
					switch (iFileType)
					{
					case FILE_TYPE_QUAKE1:
					case FILE_TYPE_QUAKE1_WAD:
					case FILE_TYPE_QUAKE2:

						if (iFileType == FILE_TYPE_QUAKE2)
						{
							pDocPalette->SetPalette( quake2_pal, 256);
						}
						else
						{
							pDocPalette->SetPalette( quake1_pal, 256);
						}

						for (j = 0; j < iSize; j++) 
						{
							r = byClipboardPalette[pbyClipboardData[j] * 3 + 0];
							g = byClipboardPalette[pbyClipboardData[j] * 3 + 1];
							b = byClipboardPalette[pbyClipboardData[j] * 3 + 2];
														
							if (
								(r != 159) || (g != 91) || (b != 83) && (iFileType != FILE_TYPE_QUAKE2)
								)
							{
								pIRGBData[j] = IRGB( 0, r, g, b);
							}
							else
							{
								pIRGBData[j] = IRGB( 255, r, g, b);
							}
						}

						// Convert all of the indexes
						pDocPalette->Convert24BitTo256Color( pIRGBData, 
								pbyClipboardData, iWidth, iHeight, 0, GetDitherType(), FALSE);
				
						break;
					
					case FILE_TYPE_HERETIC2:
					case FILE_TYPE_SIN:
					case FILE_TYPE_HALF_LIFE_WAD:
					case FILE_TYPE_HALF_LIFE:
					case FILE_TYPE_TGA:
					case FILE_TYPE_BMP:
					case FILE_TYPE_PCX:						
					case FILE_TYPE_JPG:
					case FILE_TYPE_PNG:
					case FILE_TYPE_TEX:
						{
							pDocPalette->SetPalette (byClipboardPalette, 256);
						}						
						break;
	
					default:
						ASSERT (FALSE);		// Unhandled file type?
						break;
					}
				}
				break;
			
			default:
				// Unknown palette conversion option
				ASSERT (FALSE);
				return;
			}

			// Build the IRGB data
			pDocPalette->GetPalette (byDocPalette, 256);

			for (y = 0; y < iHeight; y++)
			{
				for (x = 0; x < iWidth; x++)
				{
					iOffset = pbyClipboardData[y * iWidth + x];

					r = byClipboardPalette[iOffset * 3 + 0];
					g = byClipboardPalette[iOffset * 3 + 1];
					b = byClipboardPalette[iOffset * 3 + 2];

					pDocLayer->SetPixel (pWallyView, x, y, IRGB( iOffset, r, g, b));
				}
			}
		}		 // case 8:
		break;

	case 24:
		{
			BYTE byPalette[256 * 3];			
			CColorOptimizer ColorOpt;

			// Build the buffer so we can work with it
			pbyClipboardData = mbClipboardData.GetBuffer (iSize * 3);

			if (!pbyClipboardData)
			{
				AfxMessageBox( "Error: CWallyApp::OnEditPasteAsNewImage() - Out of Memory - pbyClipboardData == NULL");
				return;
			}
			
			ClipboardDIB.GetRawBits (pbyClipboardData);			

			// This is the 8-bit reduced data
			CMemBuffer mb8BitData;
			LPBYTE pby8BitData = mb8BitData.GetBuffer (iSize);

			if (!pby8BitData)
			{
				AfxMessageBox( "Error: CWallyApp::OnEditPasteAsNewImage() - Out of Memory - pbyClipboardData == NULL");				
				return;
			}
			
			// Build the initial IRGB array
			for (j = 0; j < iSize; j++)
			{
				r = pbyClipboardData[j * 3];
				g = pbyClipboardData[j * 3 + 1];
				b = pbyClipboardData[j * 3 + 2];
				

				if (iFileType == FILE_TYPE_QUAKE2)
				{

					if ((r != 159) || (g != 91) || (b != 83))
					{
						pIRGBData[j] = IRGB( 0, r, g, b);
					}
					else
					{
						pIRGBData[j] = IRGB( 255, r, g, b);
					}
				}
				else
				{					
					pIRGBData[j] = IRGB( 0, r, g, b);
				}
			}

			switch (iFileType)
			{
			case FILE_TYPE_QUAKE1:
			case FILE_TYPE_QUAKE1_WAD:
				// Ty- use the default Q1 palette here, so we can still paste 24-bit
				// images to Quake1 docs
				pDocPalette->SetPalette (quake1_pal, 256);
				break;

			case FILE_TYPE_QUAKE2:
				// Ty- use the default Q2 palette here, so we can still paste 24-bit
				// images to Quake2 docs
				pDocPalette->SetPalette (quake2_pal, 256);				
				break;

			case FILE_TYPE_HERETIC2:
			case FILE_TYPE_SIN:
				{
					ColorOpt.Optimize( pIRGBData, iWidth, iHeight, byPalette, 256, TRUE);
					
					// SetPalette ASSERTs with anything other than 256 colors
					pWallyDoc->SetPalette( byPalette, 256);
				}
				break;

			case FILE_TYPE_HALF_LIFE:
			case FILE_TYPE_HALF_LIFE_WAD:
				{
					char cFlag = strName.GetAt(0);
					int iNumColors = (cFlag == '{' ? 255 : 256);

					ColorOpt.Optimize( pIRGBData, iWidth, iHeight, byPalette, iNumColors, TRUE);

					if (iNumColors == 255)
					{
						// Set the last index to pure blue, for transparency
						byPalette[765] = 0;
						byPalette[766] = 0;
						byPalette[767] = 255;
					}
					
					// SetPalette ASSERTs with anything other than 256 colors
					pWallyDoc->SetPalette( byPalette, 256);					
				}
				break;

			case FILE_TYPE_TGA:
			case FILE_TYPE_BMP:
			case FILE_TYPE_PCX:
			case FILE_TYPE_JPG:
			case FILE_TYPE_PNG:
			case FILE_TYPE_TEX:
				{
					// These are true 24-bit images.  For the 8-bit display, we still need to come 
					// up with a palette
					BOOL bPaletteLoaded = FALSE;

					if (! g_bBuildOptimizedPalette)
					{
						bPaletteLoaded = LoadDefaultEditingPalette (NULL, pWallyDoc->GetPalette(), 256);
					}

					if (! bPaletteLoaded)
					{
						ColorOpt.Optimize( pIRGBData, iWidth, iHeight, byPalette, 256, TRUE);
						
						// SetPalette ASSERTs with anything other than 256 colors
						pWallyDoc->SetPalette( byPalette, 256);
					}					
				}
				break;

			default:
				ASSERT (FALSE);		// Unhandled game type?
				return;
				break;
			}

			// Neal - we need to do this for 24 bit images too (builds index values)
			//
			pWallyDoc->Convert24BitTo256Color( pIRGBData, pby8BitData, 
						iWidth, iHeight, 0, GetDitherType(), FALSE);
		
			// Build the IRGB data
			pDocPalette->GetPalette (byDocPalette, 256);
			UINT i8BitOffset = 0;
			
			for (y = 0; y < iHeight; y++)
			{
				for (x = 0; x < iWidth; x++)
				{
					iOffset = y * iWidth + x;
					i8BitOffset = pby8BitData[iOffset];

					SetIValue( pIRGBData[iOffset], i8BitOffset);				
					pDocLayer->SetPixel (pWallyView, x, y, pIRGBData[iOffset]);
				}
			}
		}
		break;	// case 24:
		
	default:
		{
			ASSERT (FALSE);
		}
		break;
	}			// switch (iBPP)

	pWallyDoc->CopyLayerToMip();	// Neal - fix initial repaint bug

	// ReMip?
	if (g_bAutoRemip)
	{
		pWallyDoc->RebuildSubMips();
	}

	// Build the SiN color
	pWallyDoc->CalcImageColorSinHeader();

	pWallyView->UpdateDIBs();
	pWallyView->Invalidate();	
		
	// Link the package doc <-> WallyDoc if there
	if (pPackageDoc)
	{
		CWADItem *pWADItem = pPackageDoc->AddImage (pWallyDoc, strName, FALSE);
		pWallyDoc->SetPackageDoc (pPackageDoc);
		pWADItem->SetWallyDoc (pWallyDoc);
		pWallyDoc->SetModifiedFlag(FALSE);
	}
	else
	{
		pWallyDoc->SetModifiedFlag(TRUE);
	}

	return;
}

void CWallyApp::OnUpdateEditPasteAsNewImage(CCmdUI* pCmdUI) 
{	
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_DIB));	
}

void CWallyApp::UpdateAllDocs(LPARAM lHint /* = HINT_UPDATE_PAL */)
{
	// update all the open docs
	POSITION Pos = GetFirstDocTemplatePosition();

	while (Pos != NULL)
	{
		CDocTemplate* pTemplate = GetNextDocTemplate( Pos);

		POSITION PosDoc = pTemplate->GetFirstDocPosition();

		while (PosDoc != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc( PosDoc);

			CWallyDoc* pWallyDoc = DYNAMIC_DOWNCAST( CWallyDoc, pDoc);
			if (pWallyDoc)
			{
				//pWallyDoc->UpdateAllViews( NULL, HINT_UPDATE_PAL);
				pWallyDoc->UpdateAllViews( NULL, lHint);
			}
		}
	}
}

void CWallyApp::OnViewOptions() 
{
	// neal - yes, this function IS supposed to be in WallyView.cpp also

	CWallyPropertySheet propSheet( AfxGetMainWnd() );	

	propSheet.m_Page1.m_pDoc = NULL;

	propSheet.DoModal();
	UpdateAllDocs();
}

void CWallyApp::OnViewRules() 
{
	CRulesDlg rulesDlg;

	rulesDlg.DoModal();
}


void CWallyApp::OpenImage (LPCTSTR szFileName)
{
	OpenDocumentFile (szFileName);
	return;

	// Ty- none of this stuff should be needed anymore...

	BOOL bNonGameType = FALSE;	
	CString strFileExtension ("");
	
	strFileExtension = GetExtension (szFileName);
	strFileExtension.MakeLower();
	
	if (
		(strFileExtension != ".wal") && 
		(strFileExtension != ".wad") && 
		(strFileExtension != ".mip") && 
		(strFileExtension != ".art") && 
		(strFileExtension != ".swl") && 
		(strFileExtension != ".m8"))
	{
		bNonGameType = TRUE;
	}

	if ((g_iFileTypePaste == FILE_TYPE_PROMPT) && (bNonGameType))
	{
		CGameSelectDlg dlgGameSelect;
		dlgGameSelect.SetMessageType (GAME_SELECT_MSG_OPEN);
		if (dlgGameSelect.DoModal() != IDOK)
		{			
			return;
		}
//		g_iFileTypeDragDrop = g_iFileTypeNew;
	}	
	else
	{
//		g_iFileTypeDragDrop = g_iFileTypePaste;	
	}

	CPackageDoc *pPackageDoc = NULL;
	CPackageView *pPackageView = NULL;

/*	if ((g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD) || (g_iFileTypeDragDrop == FILE_TYPE_QUAKE1_WAD))
	{			
		pPackageDoc = CreatePackageDoc();
		pPackageDoc->SetWADType (g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD ? WAD3_TYPE : WAD2_TYPE);
		pPackageView = pPackageDoc->GetView();			
	}*/
			
	// WAD files can't be added to an existing WAD!
	if ((pPackageView) && (strFileExtension != ".wad"))
	{
		pPackageView->ImportImage (szFileName);
	}
	else
	{	
		OpenDocumentFile (szFileName);
	}	
}

void CWallyApp::OnFileOpen() 
{	
	CImageHelper ihImport;
		
	// Build some strings based on the String Table entries
	CString strWildCard ("All Files (*.*)|*.*|");
	strWildCard += ihImport.GetSupportedImageList(true);
	CString strTitle((LPCTSTR)IDS_FILEOPEN_TITLE);	
		
	// Create a CFileDialog, init with our strings
	CFileDialog	dlgOpen (TRUE, NULL, NULL, OFN_FILEMUSTEXIST | 
		OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_ALLOWMULTISELECT, 
		strWildCard, NULL); 		

	// neal - allow multiple files to be selected at once (up to 8)
	dlgOpen.m_ofn.nMaxFile = _MAX_PATH*8;

	char* szBuffer = (char* )malloc( dlgOpen.m_ofn.nMaxFile);
	szBuffer[0] = '\0';
	dlgOpen.m_ofn.lpstrFile = szBuffer;

	
	// Set some of the CFileDialog vars
	if (g_strFileOpenDirectory != "")
		dlgOpen.m_ofn.lpstrInitialDir = g_strFileOpenDirectory;
	dlgOpen.m_ofn.lpstrTitle = strTitle;	
	dlgOpen.m_ofn.nFilterIndex = g_iFileOpenExtension;
	
	if (dlgOpen.DoModal() == IDOK)
	{
		g_strFileOpenDirectory = dlgOpen.GetPathName().Left(dlgOpen.m_ofn.nFileOffset);
		g_iFileOpenExtension = dlgOpen.m_ofn.nFilterIndex;

		BOOL bNonGameType = FALSE;
		CString strFileName ("");
		CString strFileExtension ("");

		POSITION Pos = dlgOpen.GetStartPosition();
		/*
		while (Pos)
		{			
			strFileName = dlgOpen.GetNextPathName( Pos);
			strFileExtension = GetExtension (strFileName);

			if ((strFileExtension != ".wal") && (strFileExtension != ".wad") && (strFileExtension != ".mip") && (strFileExtension != ".art") && (strFileExtension != ".swl") && (strFileExtension != ".m8") && (strFileExtension != ".pak") && (strFileExtension != ".tga") && (strFileExtension != ".pcx") && (strFileExtension != ".bmp"))
			{
				bNonGameType = true;
			}
		}

		if ((g_iFileTypePaste == FILE_TYPE_PROMPT) && (bNonGameType))
		{
			CGameSelectDlg dlgGameSelect;
			dlgGameSelect.SetMessageType (GAME_SELECT_MSG_OPEN);
			if (dlgGameSelect.DoModal() != IDOK)
			{
				if (szBuffer)		
					delete szBuffer; 
				return;
			}
			g_iFileTypeDragDrop = g_iFileTypeNew;
		}	
		else
		{
			g_iFileTypeDragDrop = g_iFileTypePaste;	
		}

		CPackageDoc *pPackageDoc = NULL;
		CPackageView *pPackageView = NULL;

		if ((g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD) || (g_iFileTypeDragDrop == FILE_TYPE_QUAKE1_WAD))
		{			
			pPackageDoc = CreatePackageDoc();
			pPackageDoc->SetWADType (g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD ? WAD3_TYPE : WAD2_TYPE);
			pPackageView = pPackageDoc->GetView();
		}
		*/
		
		Pos = dlgOpen.GetStartPosition();
		while (Pos)
		{			
			strFileName = dlgOpen.GetNextPathName( Pos);
			strFileExtension = GetExtension (strFileName);

		/*	// WAD files can't be added to an existing WAD!
			if ((pPackageView) && (strFileExtension != ".wad") && (strFileExtension != ".pak"))
			{
				pPackageView->ImportImage (strFileName);
			}
			else			
			{
				OpenDocumentFile (strFileName);
			}
			*/			
			
			OpenDocumentFile (strFileName);
		}	
		
	}	

	dlgOpen.m_ofn.lpstrFile = NULL;
	if (szBuffer)		
		delete szBuffer; 


// Old stuff here:
/*	// Build some strings based on the String Table entries
	CString strWildCard((LPCTSTR)IDS_FILEOPEN_WILDCARD);
	CString strTitle((LPCTSTR)IDS_FILEOPEN_TITLE);		
	CString strAppendExtension((LPCTSTR)IDS_FILEOPEN_EXTENSION);

	// Create a CFileDialog, init with our strings
	CFileDialog	m_OpenWalDialog (TRUE, strAppendExtension, NULL, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON |
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, strWildCard, NULL);

	// neal - allow multiple files to be selected at once (up to 8)
	m_OpenWalDialog.m_ofn.nMaxFile = _MAX_PATH*8;

	char* szBuffer = (char* )malloc( m_OpenWalDialog.m_ofn.nMaxFile);	
	szBuffer[0] = '\0';
	m_OpenWalDialog.m_ofn.lpstrFile = szBuffer;

	// Set some of the CFileDialog vars
	if (g_strFileOpenDirectory != "")
	{
		m_OpenWalDialog.m_ofn.lpstrInitialDir = g_strFileOpenDirectory;
	}

	m_OpenWalDialog.m_ofn.lpstrTitle   = strTitle;	
	m_OpenWalDialog.m_ofn.nFilterIndex = g_iFileOpenExtension;
	
	if (m_OpenWalDialog.DoModal() == IDOK)
	{
		g_strFileOpenDirectory = m_OpenWalDialog.GetPathName().Left(m_OpenWalDialog.m_ofn.nFileOffset);					
		g_iFileOpenExtension = m_OpenWalDialog.m_ofn.nFilterIndex;					

		POSITION Pos = m_OpenWalDialog.GetStartPosition();

		while (Pos)
		{
			CString FileName = m_OpenWalDialog.GetNextPathName( Pos);
			CString FileExtension = GetExtension( FileName);			

			if (FileExtension == ".wad")
			{
				PackageDocTemplate->OpenDocumentFile( FileName);
			}

			if (FileExtension == ".wal")
			{
				WallyDocTemplate->OpenDocumentFile( FileName);
			}
		}
	}
	m_OpenWalDialog.m_ofn.lpstrFile = NULL;
	if (szBuffer)		
		delete szBuffer; */
}

void CWallyApp::OpenNonWalFile (LPCTSTR szFileName)
{
	CImageHelper ihHelper;
	ihHelper.LoadImage (szFileName, IH_LOAD_ONLYIMAGE);

	if (ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
		return;
	}
	
	g_iDocHeight = ihHelper.GetImageHeight();
	g_iDocWidth = ihHelper.GetImageWidth();
	
	// Go build a new document/view based on the WallyDoc Template, then display it
	CWallyDoc* pDoc = (CWallyDoc *)WallyDocTemplate->OpenDocumentFile(NULL,true);
		
	// Set the title to just the filename, no path or extension
	pDoc->SetTitle (GetRawFileName(szFileName));

	// Go load up the image from disk via the Document class
	pDoc->LoadImage (&ihHelper);

	// Neal - now done below
//	// If Auto-ReMip is enabled, go rebuild the subs
//	if (g_bAutoRemip)
//		pDoc->RebuildSubMips();				
	
	// Find that first view so we can tell it the data has changed
	POSITION pos = pDoc->GetFirstViewPosition();
	CWallyView* pFirstView = (CWallyView *)pDoc->GetNextView( pos );
	
	// Flag the data as being modified, invalidate the view (as it has more than
	// likely already appeared, then set the scroll bars
	pDoc->UpdateAllDIBs( g_bAutoRemip);	
}

void CWallyApp::ReturnErrorOnFileNew()
{
	AfxMessageBox( "Failed to create new document.", MB_ICONSTOP);
}

void CWallyApp::OnFileNew() 
{
	CNewImageDlg dlgNewImage;
	if (g_iTextureNameSetting == PRESET_DIR)
	{
		dlgNewImage.SetTextureName (g_strDefaultTextureName);
	}

	if (dlgNewImage.DoModal() == IDOK)	
	{
		CWallyDoc   *pWallyDoc   = NULL;
		CPackageDoc *pPackageDoc = NULL;
		CString      strTitle ("");

		g_iDocWidth  = 0;		// Neal - fixes Chris Sykes' new BMP is wrong size bug
		g_iDocHeight = 0;

		int iFileType    = dlgNewImage.GetFileType();
		int iDefFileType = g_iFileTypeDefault;

		switch (iFileType)
		{
		case FILE_TYPE_QUAKE2:
		case FILE_TYPE_QUAKE1:
		case FILE_TYPE_HALF_LIFE:
		case FILE_TYPE_HERETIC2:
		case FILE_TYPE_SIN:

			// Neal - fixes missing submips on new wal view
			//
			g_iFileTypeDefault = iFileType;
			pWallyDoc = (CWallyDoc *)WallyDocTemplate->OpenDocumentFile(NULL, true);
			g_iFileTypeDefault = iDefFileType;

			if (pWallyDoc)
			{
				ASSERT(pWallyDoc->GetGameType() == iFileType);
				pWallyDoc->SetGameType( iFileType);		// Neal - now done in OpenDocFile above

				if (iFileType = FILE_TYPE_QUAKE1)
				{
					pWallyDoc->SetPalette( quake1_pal, 256, TRUE);
				}
				else if (iFileType = FILE_TYPE_QUAKE2)
				{
					pWallyDoc->SetPalette( quake2_pal, 256, TRUE);
				}
				else
				{
					pWallyDoc->SetPalette( dlgNewImage.GetPalette(), 256, TRUE);
				}
				pWallyDoc->SetName( dlgNewImage.GetTextureName());
			}
			else
			{
				ASSERT( FALSE);
				ReturnErrorOnFileNew();
			}
			break;

		case FILE_TYPE_QUAKE1_WAD:
			pPackageDoc = (CPackageDoc *)PackageDocTemplate->OpenDocumentFile (NULL, true);

			if (pPackageDoc)
			{
				strTitle.Format ("Untitled%d", g_iUntitledCount++);
				pPackageDoc->SetTitle (strTitle);
				pPackageDoc->SetWADType (WAD2_TYPE);
			}			
			else
			{
				ASSERT( FALSE);
				ReturnErrorOnFileNew();
			}
			break;

		case FILE_TYPE_HALF_LIFE_WAD:
			pPackageDoc = (CPackageDoc *)PackageDocTemplate->OpenDocumentFile (NULL, true);

			if (pPackageDoc)
			{
				strTitle.Format ("Untitled%d", g_iUntitledCount++);
				pPackageDoc->SetTitle (strTitle);
				pPackageDoc->SetWADType (WAD3_TYPE);
			}			
			else
			{
				ASSERT( FALSE);
				ReturnErrorOnFileNew();
			}

			break;

		case FILE_TYPE_PAK:
			{
				CPakDoc *pPakDoc = (CPakDoc *)PakDocTemplate->OpenDocumentFile (NULL, true);
				if (pPakDoc)
				{
					strTitle.Format ("Untitled%d", g_iUntitledCount++);
					pPakDoc->SetTitle (strTitle);
				}
				else
				{
					ASSERT( FALSE);
					ReturnErrorOnFileNew();
				}
			}
			break;

		case FILE_TYPE_TGA:
		case FILE_TYPE_PCX:
		case FILE_TYPE_BMP:
		case FILE_TYPE_JPG:
		case FILE_TYPE_PNG:
		case FILE_TYPE_TEX:
			{
				// Neal - part of fix for missing submips on new wal view
				//
				g_iFileTypeDefault = iFileType;
				pWallyDoc = (CWallyDoc *)WallyDocTemplate->OpenDocumentFile(NULL, true);
				g_iFileTypeDefault = iDefFileType;
				
				if (pWallyDoc)
				{
					//pWallyDoc->SetGameType (iFileType);
					ASSERT(pWallyDoc->GetGameType() == iFileType);

					if (!LoadDefaultEditingPalette (NULL, pWallyDoc->GetPalette(), 256))
					{
						CCustomResource crPalette;
						crPalette.UseResourceId ("PALETTE", IDR_LMP_BLEND);
						pWallyDoc->SetPalette (crPalette.GetData(), 256, TRUE);
					}
					pWallyDoc->SetName (dlgNewImage.GetTextureName());
				}
				else
				{
					ASSERT( FALSE);
					ReturnErrorOnFileNew();
				}
			}
			break;

		default:
			ASSERT (FALSE);			// Unhandled game type!
			break;

		}		
		// Neal - bugfix - new layer should be background color, not black
		//        (the problem is that the palette cannot be initialized early
		//        enough, so we need to reinitialize it after the palete has
		//        been created)
		if (pWallyDoc)
		{
			pWallyDoc->CopyMipToLayer();
		}		
	}

	//WallyDocTemplate->OpenDocumentFile(NULL,true);
} 


void CWallyApp::OnFileBrowse() 
{	
	if (!m_bBrowseOpen)
	{
		m_pBrowseDoc = (CBrowseDoc *)BrowseDocTemplate->OpenDocumentFile(NULL,true);	
		if (m_pBrowseDoc != NULL)
		{
			m_bBrowseOpen = true;
		}
		
		if (g_strBrowseDirectory != "")
		{			
			m_pBrowseDoc->UpdateExplorer(g_strBrowseDirectory);
		}
	}	
	else
	{		
		POSITION pos = m_pBrowseDoc->GetFirstViewPosition();
		CBrowseView* pFirstView = (CBrowseView *)m_pBrowseDoc->GetNextView( pos );
		pFirstView->GetParentFrame()->ActivateFrame();
		
		if (pFirstView->GetParentFrame()->IsIconic())
		{
			pFirstView->GetParentFrame()->ShowWindow(SW_SHOWNORMAL);
		}
	}	
}


void CWallyApp::AddToRecentFileList(LPCTSTR lpszPathName) 
{	
	CString strPath = GetPathToFile(lpszPathName);
	strPath = TrimSlashes (strPath);
	CString strFileExtension = GetExtension(lpszPathName);

	// We might be in a subdirectory... see if the first part is our temp dir
	if (strPath.GetLength() > g_strTempDirectory.GetLength())
	{
		strPath = strPath.Left(g_strTempDirectory.GetLength());
	}

	// If the file is under the temp directory, don't add it here
	if (strPath.CompareNoCase (g_strTempDirectory))
	{	
		if (
			(strFileExtension == ".wal") || 
			(strFileExtension == ".wad") || 
			(strFileExtension == ".mip") || 
			(strFileExtension == ".swl") || 
			(strFileExtension == ".m8") || 
			(strFileExtension == ".pak") || 
			(strFileExtension == ".tga") || 
			(strFileExtension == ".pcx") || 
			(strFileExtension == ".bmp") ||
			(strFileExtension == ".png") ||
			(strFileExtension == ".jpg")
			)
		{		
			CWinApp::AddToRecentFileList(lpszPathName);
		}
	}
}


void CWallyApp::OnFileConvert() 
{
	ASSERT (FALSE);		// This should be removed... OnFileOpen handles conversions
	return;

	CImageHelper ihImport;
		
	// Build some strings based on the String Table entries
	CString strWildCard ("All Files (*.*)|*.*|");
	strWildCard += ihImport.GetSupportedImageList();
	CString strTitle((LPCTSTR)IDS_FILEOPEN_TITLE);	
		
	// Create a CFileDialog, init with our strings
	CFileDialog	m_OpenWalDialog (TRUE, NULL, NULL, OFN_FILEMUSTEXIST | 
		OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_ALLOWMULTISELECT, 
		strWildCard, NULL); 		

	// neal - allow multiple files to be selected at once (up to 8)
	m_OpenWalDialog.m_ofn.nMaxFile = _MAX_PATH*8;

	char* szBuffer = (char* )malloc( m_OpenWalDialog.m_ofn.nMaxFile);
	szBuffer[0] = '\0';
	m_OpenWalDialog.m_ofn.lpstrFile = szBuffer;

	
	// Set some of the CFileDialog vars
	if (g_strFileConvertDirectory != "")
		m_OpenWalDialog.m_ofn.lpstrInitialDir = g_strFileConvertDirectory;
	m_OpenWalDialog.m_ofn.lpstrTitle = strTitle;	
	m_OpenWalDialog.m_ofn.nFilterIndex = g_iFileConvertExtension;
	
	if (m_OpenWalDialog.DoModal() == IDOK)
	{
		g_strFileConvertDirectory = m_OpenWalDialog.GetPathName().Left(m_OpenWalDialog.m_ofn.nFileOffset);					
		g_iFileConvertExtension = m_OpenWalDialog.m_ofn.nFilterIndex;

		BOOL bNonGameType = FALSE;
		CString strFileName ("");
		CString strFileExtension ("");

		POSITION Pos = m_OpenWalDialog.GetStartPosition();
		while (Pos)
		{			
			strFileName = m_OpenWalDialog.GetNextPathName( Pos);
			strFileExtension = GetExtension (strFileName);

			if ((strFileExtension != ".wal") && (strFileExtension != ".mip") && (strFileExtension != ".swl") && (strFileExtension != ".m8"))
			{
				bNonGameType = true;
			}
		}

		if ((g_iFileTypePaste == FILE_TYPE_PROMPT) && (bNonGameType))
		{
			CGameSelectDlg dlgGameSelect;
			dlgGameSelect.SetMessageType (GAME_SELECT_MSG_OPEN);
			if (dlgGameSelect.DoModal() != IDOK)
			{
				if (szBuffer)		
					delete szBuffer; 
				return;
			}
//			g_iFileTypeDragDrop = g_iFileTypeNew;
		}	
		else
		{
	//		g_iFileTypeDragDrop = g_iFileTypePaste;	
		}

		CPackageDoc *pPackageDoc = NULL;
		CPackageView *pPackageView = NULL;
		
	/*	if ((g_iFileTypeDragDrop == FILE_TYPE_HALF_LIFE_WAD) || (g_iFileTypeDragDrop == FILE_TYPE_QUAKE1_WAD))
		{
			pPackageDoc = CreatePackageDoc();
			pPackageView = pPackageDoc->GetView();
			
			switch (g_iFileTypeDragDrop)
			{
			case FILE_TYPE_QUAKE1_WAD:
				pPackageDoc->SetWADType(WAD2_TYPE);
				break;

			case FILE_TYPE_HALF_LIFE_WAD:
				pPackageDoc->SetWADType(WAD3_TYPE);
				break;

			default:
				ASSERT (FALSE);
				break;
			}			
		}*/
		
		Pos = m_OpenWalDialog.GetStartPosition();
		while (Pos)
		{			
			strFileName = m_OpenWalDialog.GetNextPathName( Pos);
			strFileExtension = GetExtension (strFileName);
			
			if ((pPackageView) && (strFileExtension != ".wad"))
			{
				pPackageView->ImportImage (strFileName);
			}
			else
			{	
				OpenDocumentFile (strFileName);
			}

			/*if (strFileExtension == ".wal")
			{
				WallyDocTemplate->OpenDocumentFile( strFileName);
			}
			else
			if (strFileExtension == ".wad")
			{
				PackageDocTemplate->OpenDocumentFile( strFileName);
			}
			else
			{
				OpenNonWalFile (strFileName);
			}*/
		}	
		
	}	

	m_OpenWalDialog.m_ofn.lpstrFile = NULL;
	if (szBuffer)		
		delete szBuffer; 
		
	
}

BOOL CWallyApp::PreTranslateMessage( MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	// neal - fix msg slow-down
	if (! CSplashWnd::IsActive())
		return CWinApp::PreTranslateMessage( pMsg);

	if (CSplashWnd::PreTranslateAppMessage( pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage( pMsg);
}

void CWallyApp::OnProperties()
{
	// TODO: The property sheet attached to your project
	// via this function is not hooked up to any message
	// handler.  In order to actually use the property sheet,
	// you will need to associate this function with a control
	// in your project such as a menu item or tool bar button.

	CWallyPropertySheet propSheet;

	propSheet.DoModal();
	UpdateAllDocs();

	// This is where you would retrieve information from the property
	// sheet if propSheet.DoModal() returned IDOK.  We aren't doing
	// anything for simplicity.
}

int BrushSizeToIndex( int iSize)
{
	if (g_iCurrentTool == EDIT_MODE_SELECTION)
	{
		return (iSize);
	}
	else if (g_iCurrentTool == EDIT_MODE_RIVETS)
	{
		switch (iSize)
		{
		case 8:
			return (0);
		case 16:
			return (1);
		case 32:
			return (2);
		case 64:
			return (3);
		case 128:
			return (4);
		default:
			ASSERT( FALSE);
		}
		return (1);
	}
	else
	{
		switch (iSize)
		{
		case 1:
			return (0);
		case 2:
			return (1);
		case 3:
			return (2);
		case 4:
			return (3);
		case 5:
			return (4);
		case 6:
			return (5);
		case 7:
			return (6);
		case 8:
			return (7);
		case 12:
			return (8);
		case 16:
			return (9);
		case 32:
			return (10);
		case 48:
			return (11);
		case 64:
			return (12);
		}
	}
	return (0);
}

int IndexToBrushSize( int iIndex)
{
	if (g_iCurrentTool == EDIT_MODE_SELECTION)
	{
		return (iIndex);
	}
	else if (g_iCurrentTool == EDIT_MODE_RIVETS)
	{
		switch (iIndex)
		{
		case 0:
			return (8);
		case 1:
			return (16);
		case 2:
			return (32);
		case 3:
			return (64);
		case 4:
			return (128);
		default:
			ASSERT( FALSE);
		}
		return (16);
	}
	else
	{
		switch (iIndex)
		{
		case 0:
			return (1);
		case 1:
			return (2);
		case 2:
			return (3);
		case 3:
			return (4);
		case 4:
			return (5);
		case 5:
			return (6);
		case 6:
			return (7);
		case 7:
			return (8);
		case 8:
			return (12);
		case 9:
			return (16);
		case 10:
			return (32);
		case 11:
			return (48);
		case 12:
			return (64);
		default:
			ASSERT( FALSE);
		}
	}
	return (1);
}

int IndexToBrushSizeId( int iIndex)
{
/*
	switch (iIndex)
	{
	case 0:
		return (ID_WIDTH_1_PIXEL);
	case 1:
		return (ID_WIDTH_2_PIXELS);
	case 2:
		return (ID_WIDTH_3_PIXELS);
	case 3:
		return (ID_WIDTH_4_PIXELS);
	case 4:
		return (ID_WIDTH_5_PIXELS);
	case 5:
		return (ID_WIDTH_6_PIXELS);
	case 6:
		return (ID_WIDTH_7_PIXELS);
	case 7:
		return (ID_WIDTH_8_PIXELS);
	case 8:
		return (ID_WIDTH_11_PIXELS);
	case 9:
		return (ID_WIDTH_15_PIXELS);
	case 10:
		return (ID_WIDTH_31_PIXELS);
	default:
		ASSERT( FALSE);
	}
*/
	ASSERT( FALSE);
	return (ID_WIDTH_1_PIXEL);
}

int BrushSizeIdToIndex( int iSize)
{
/*
	switch (iSize)
	{
	case ID_WIDTH_1_PIXEL:
		return (0);
	case ID_WIDTH_2_PIXELS:
		return (1);
	case ID_WIDTH_3_PIXELS:
		return (2);
	case ID_WIDTH_5_PIXELS:
		return (3);
	case ID_WIDTH_7_PIXELS:
		return (4);
	case ID_WIDTH_11_PIXELS:
		return (5);
	case ID_WIDTH_15_PIXELS:
		return (6);
	case ID_WIDTH_31_PIXELS:
		return (7);
	default:
		ASSERT( FALSE);
	}
*/
	ASSERT( FALSE);
	return (-1);
}

int IndexToBrushShape( int iIndex)
{
	switch (iIndex)
	{
	case 0:
		return (SHAPE_SQUARE);
	case 1:
		return (SHAPE_CIRCULAR);
	case 2:
		return (SHAPE_DIAMOND);
	default:
		ASSERT( FALSE);
	}
	return (SHAPE_SQUARE);
}

int IndexToBrushShapeId( int iIndex)
{
	switch (iIndex)
	{
	case 0:
		return (ID_SHAPE_SQUARE);
	case 1:
		return (ID_SHAPE_CIRCULAR);
	case 2:
		return (ID_SHAPE_DIAMOND);
	default:
		ASSERT( FALSE);
	}
	return (SHAPE_SQUARE);
}

int BrushShapeToIndex( int iShape)
{
	switch (iShape)
	{
	case SHAPE_SQUARE:
		return (0);
	case SHAPE_CIRCULAR:
		return (1);
	case SHAPE_DIAMOND:
		return (2);
	default:
		ASSERT( FALSE);
	}
	return (-1);
}

int BrushShapeIdToIndex( int iShape)
{
	switch (iShape)
	{
	case ID_SHAPE_SQUARE:
		return (0);
	case ID_SHAPE_CIRCULAR:
		return (1);
	case ID_SHAPE_DIAMOND:
		return (2);
	default:
		ASSERT( FALSE);
	}
	return (-1);
}

int IndexToToolAmount( int iIndex)
{
	if (ToolHasAmount( g_iCurrentTool))
	{
/*		switch (iIndex)
		{
		case 0:
			return (ID_APPLY_LEAST);
		case 1:
			return (ID_APPLY_A_LITTLE_BIT);
		case 2:
			return (ID_APPLY_MEDIUM);
		case 3:
			return (ID_APPLY_MORE);
		case 4:
			return (ID_APPLY_MOST);
		default:
			ASSERT( FALSE);
		}
*/	}
	return (ID_APPLY_MEDIUM);
}

int IndexToToolAmountId( int iIndex)
{
	if (ToolHasAmount( g_iCurrentTool))
	{
		switch (iIndex)
		{
		case 0:
			return (ID_APPLY_LEAST);
		case 1:
			return (ID_APPLY_A_LITTLE_BIT);
		case 2:
			return (ID_APPLY_MEDIUM);
		case 3:
			return (ID_APPLY_MORE);
		case 4:
			return (ID_APPLY_MOST);
		default:
			ASSERT( FALSE);
		}
	}
	return (ID_APPLY_MEDIUM);
}

int ToolToAmountIndex( int iToolEditMode)
{
	int iAmount = -1;

	if (ToolHasAmount( iToolEditMode))
	{
		switch (iToolEditMode)
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

		case EDIT_MODE_TINT:
			iAmount = g_iTintAmount;
			break;

		case EDIT_MODE_BLEND:
			iAmount = g_iBlendAmount;
			break;

		case EDIT_MODE_SHARPEN:
			iAmount = g_iSharpenAmount;
			break;

		case EDIT_MODE_SPRAY:
			iAmount = g_iSprayAmount;
			break;

		case EDIT_MODE_SCRATCH:
			iAmount = g_iScratchAmount;
			break;

		case EDIT_MODE_RECOLOR:
			iAmount = g_iRecolorAmount;
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
			ASSERT( FALSE);
		}

		if ((iAmount >= 0) && (iAmount <= 4))
			return (iAmount);
		else
		{
			ASSERT( FALSE);
		}
	}

	return -1;
}

int CalcStandardPercent( int iAmount)
{
	switch (iAmount)
	{
	case 0:
		return (10);
	case 1:
		return (20);
	case 2:
		return (30);
	case 3:
		return (40);
	case 4:
		return (50);
	default:
		ASSERT( FALSE);
		return (30);
	}
}

int CalcLightPercent( int iAmount)
{
	switch (iAmount)
	{
	case 0:
		return (5);
	case 1:
		return (10);
	case 2:
		return (20);
	case 3:
		return (30);
	case 4:
		return (50);
	default:
		ASSERT( FALSE);
		return (20);
	}
}

int CalcDecalPercent( int iAmount)
{
	switch (iAmount)
	{
	case 0:
		return (12);
	case 1:
		return (25);
	case 2:
		return (50);
	case 3:
		return (75);
	case 4:
		return (100);
	default:
		ASSERT( FALSE);
		return (50);
	}
}

int ToolAmountToPercent( int iToolEditMode)
{
	if (ToolHasAmount( iToolEditMode))
	{
		switch (iToolEditMode)
		{
		case EDIT_MODE_DARKEN:
			switch (g_iDarkenAmount)
			{
			case 0:
				return (90);
			case 1:
				return (80);
			case 2:
				return (70);
			case 3:
				return (60);
			case 4:
				return (50);
			default:
				ASSERT( FALSE);
				return (70);
			}
			break;

		case EDIT_MODE_LIGHTEN:
			return CalcLightPercent( g_iLightenAmount);
			break;

		case EDIT_MODE_TINT:
			return CalcStandardPercent( g_iTintAmount);
			break;

		case EDIT_MODE_RECOLOR:
			return CalcStandardPercent( g_iRecolorAmount);
			break;
		case EDIT_MODE_SPRAY_RECOLOR:
			return CalcStandardPercent( g_iSprayRecolorAmount);
			break;

		case EDIT_MODE_BLEND:
			return CalcLightPercent( g_iBlendAmount);

		case EDIT_MODE_SHARPEN:
			return CalcLightPercent( g_iSharpenAmount);

		case EDIT_MODE_SPRAY:
			//return CalcStandardPercent( g_iSprayAmount);
			return CalcLightPercent( g_iSprayAmount);

		case EDIT_MODE_SCRATCH:
			return CalcStandardPercent( g_iScratchAmount);

		case EDIT_MODE_SELECTION:
			return CalcDecalPercent( g_iPasteAmount);

		case EDIT_MODE_BULLET_HOLES:
			return CalcDecalPercent( g_iBulletHoleAmount);

		case EDIT_MODE_RIVETS:
			return CalcDecalPercent( g_iRivetAmount);

		case EDIT_MODE_PATTERNED_PAINT:
			return CalcDecalPercent( g_iPatternPaintAmount);

		case EDIT_MODE_DECAL:
			return CalcDecalPercent( g_iDecalAmount);

		case EDIT_MODE_RUBBER_STAMP:
		case EDIT_MODE_CLONE:
			return CalcDecalPercent( g_iCloneAmount);

		default:
			ASSERT( FALSE);
		}
	}
	return 0;
}

double CWallyApp::GetProfileDouble( LPCSTR lpszSection, LPCSTR lpszEntry, double dfDefaultValue)
{
	CString strResult = GetProfileString( lpszSection, lpszEntry, NULL);

	if (strResult.IsEmpty())
		return dfDefaultValue;

	double dfResult = dfDefaultValue;

	dfResult = atof( strResult);

	return dfResult;
}

BOOL CWallyApp::WriteProfileDouble( LPCSTR lpszSection, LPCSTR lpszEntry, double dfValue)
{
	char szTemp[80];
	sprintf( szTemp, "%.10f", dfValue);
	return WriteProfileString( lpszSection, lpszEntry, szTemp);
}

void CWallyApp::OnFileBatchConversion() 
{
	CBatchDlg dlgBatch;
	dlgBatch.DoModal();
}


void CWallyApp::SetProgressBar (int iPosition)
{	
	g_ctProgress.SetPos (iPosition);
}

void CWallyApp::SetProgressBarInfo (int iRangeLow, int iRangeHigh, int iStep)
{	
	g_ctProgress.SetRange (iRangeLow, iRangeHigh);
	g_ctProgress.SetStep(iStep);
	g_ctProgress.SetPos (0);	
}

void CWallyApp::StepProgressBar (int iAmount /* = 1 */)
{	
	if (iAmount > 1)
	{
		g_ctProgress.OffsetPos (iAmount);		
	}
	else
	{
		g_ctProgress.StepIt();
	}
}

/*BOOL CDocTemplate::SaveAllModified()
{
	POSITION pos = GetFirstDocPosition();
	while (pos != NULL)
	{
		CDocument* pDoc = GetNextDoc(pos);
		if (!pDoc->SaveModified())
			return FALSE;
	}
	return TRUE;
}*/

BOOL CWallyApp::GetBackgroundWindowPlacement (LPWINDOWPLACEMENT lpWindow)
{
	ASSERT (lpWindow);
	lpWindow->length = sizeof (WINDOWPLACEMENT);

	if (m_pMainWnd)
	{
		CMainFrame *pMainFrame = (CMainFrame *)m_pMainWnd;

		if (pMainFrame)
		{	
			CFrameWnd *pFrameWnd = (CFrameWnd *) (pMainFrame->GetMdiClient());

			if (pFrameWnd)
			{
				return (pFrameWnd->GetWindowPlacement(lpWindow));
			}
		}
	}	
	return FALSE;	
}

CView *CWallyApp::GetActiveView()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)m_pMainWnd;
	CView *pView = NULL;

	if (pFrame)
	{
		// Get the active MDI child window.
		CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();	

		if (pChild)
		{
			// Get the active view attached to the active MDI child
			// window.
			pView = pChild->GetActiveView();
		}
	}

	return pView;
}

CPackageDoc *CWallyApp::CreatePackageDoc ()
{
	CPackageDoc *pDoc = (CPackageDoc *)PackageDocTemplate->OpenDocumentFile (NULL, TRUE);
	
	CString strTitled("");
	strTitled.Format ("Untitled%d", g_iUntitledCount++);
	
	pDoc->SetTitle (strTitled);

	return pDoc;
}

void CWallyApp::OnFileLoadQ2palette() 
{	
	LoadPalette (FILE_TYPE_QUAKE2);
}

void CWallyApp::OnFileLoadQ1palette() 
{
	LoadPalette (FILE_TYPE_QUAKE1);
}


void CWallyApp::LoadPalette (int iGameType)
{
	CString strWildCard ("All Files (*.*)|*.*|");
	CImageHelper ihHelper;
	CString strAddString = ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, NULL);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath;

	unsigned char *pbyPalette = NULL;
	
	if (dlgPalette.DoModal() == IDOK)
	{
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		g_iOpenPaletteExtension = dlgPalette.m_ofn.nFilterIndex;
		strSourceFilePath         = dlgPalette.GetPathName();		

		ihHelper.LoadImage (strSourceFilePath);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;
		}		

		if (!ihHelper.IsValidPalette())
		{
			if (ihHelper.IsValidImage())
			{
				if (ihHelper.GetColorDepth() == IH_8BIT)
				{
					pbyPalette = ihHelper.GetPalette();
				}
				else
				{
					AfxMessageBox ("Image does not have a palette", MB_ICONSTOP);
					return;
				}
			}
			else
			{
				AfxMessageBox ("Unsupported palette file", MB_ICONSTOP);
				return;
			}
		}
		else
		{
			pbyPalette = ihHelper.GetBits();
		}

		switch (iGameType)
		{
		case FILE_TYPE_QUAKE1:
			memcpy (quake1_pal, pbyPalette, 768);
			break;

		case FILE_TYPE_QUAKE2:
			memcpy (quake2_pal, pbyPalette, 768);
			break;

		default:
			ASSERT (FALSE);
			break;
		}

	}
}

void CWallyApp::OnEditPasteSpecialEffects() 
{
	// TODO: Add your command handler code here
	
}

void CWallyApp::OnUpdateEditPasteSpecialEffects(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

CRect rWwwWally, rBoth, rTy, rNeal, rBoard;

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	rWwwWally.SetRect( 12,  96, 245, 235);
	rBoth.SetRect(     52, 235, 260, 280);
	rTy.SetRect(       52, 246, 138, 264);
	rNeal.SetRect(    164, 246, 260, 264);
	rBoard.SetRect(   315,  96, 395, 220);

	SetTimer( TIMER_WLY_FLICKER, 40, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

static BOOL bPlaying = FALSE;

void CAboutDlg::OnOK() 
{
	if (bPlaying)
	{
		PlaySound( NULL, NULL, SND_SYNC | SND_PURGE | SND_NODEFAULT);
		bPlaying = FALSE;
	}
	PlaySound( MAKEINTRESOURCE( IDR_WAVE_FIRE), AfxGetInstanceHandle(), 
			SND_ASYNC | SND_RESOURCE | SND_NODEFAULT);
	
	CDialog::OnOK();
}

CRect CAboutDlg::HitRect( UINT uHitCode)
{
	CRect Rect;

	switch (uHitCode)
	{
	case HIT_WWW_WALLY:
		Rect = rWwwWally;
		break;

	case HIT_BOTH:
		Rect = rBoth;
		break;

	case HIT_TY:
		Rect = rTy;
		break;

	case HIT_NEAL:
		Rect = rNeal;
		break;

	case HIT_BOARD:
		Rect = rBoard;
		break;

	default:
		break;
	}
	return Rect;
}

UINT CAboutDlg::HitTest( CPoint ptPos)
{
	CRect rBmpArea;
	m_ctrlBitmapArea.GetWindowRect( rBmpArea);
	ScreenToClient( &rBmpArea);

	CRect Rect = rWwwWally;
	Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

	if (Rect.PtInRect( ptPos))
	{
		return HIT_WWW_WALLY;
	}
	else
	{
		Rect = rBoth;
		Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

		if (Rect.PtInRect( ptPos))
		{
			Rect = rTy;
			Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

			if (Rect.PtInRect( ptPos))
			{
				return HIT_TY;
			}
			else
			{
				Rect = rNeal;
				Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

				if (Rect.PtInRect( ptPos))
				{
					return HIT_NEAL;
				}
			}
			return HIT_BOTH;
		}

		Rect = rBoard;
		Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

		if (Rect.PtInRect( ptPos))
		{
			return HIT_BOARD;
		}
	}
	return HIT_NONE;
}

void CAboutDlg::OnMouseMove( UINT /*uFlags*/, CPoint ptPos) 
{
	UINT  uHit            = HitTest( ptPos);
	BOOL  bStartPlaying   = (uHit != HIT_NONE);
	char* szStatusBarText = "";

	if (uHit != HIT_NONE)
	{
		switch (uHit)
		{
		case HIT_WWW_WALLY:
			szStatusBarText = "Navigate to Wally's www site";
			break;
		case HIT_BOTH:
			szStatusBarText = "Send email to both Ty and Neal";
			break;
		case HIT_TY:
			szStatusBarText = "Send email to Ty";
			break;
		case HIT_NEAL:
			szStatusBarText = "Send email to Neal";
			break;
		case HIT_BOARD:
			szStatusBarText = "Navigate to Wally's Discussion Board";
			break;
		default:
			ASSERT( FALSE);
			break;
		}

		if (! bPlaying)
		{
			PlaySound( MAKEINTRESOURCE( IDR_WAVE_HUM), AfxGetInstanceHandle(), 
					SND_ASYNC | SND_RESOURCE | SND_LOOP | SND_NODEFAULT);

			bPlaying = TRUE;
		}
	}
	else
	{
		if (bPlaying)
		{
			PlaySound( NULL, NULL, SND_SYNC | SND_PURGE | SND_NODEFAULT);
			bPlaying = FALSE;

			InvalidateRect( NULL, FALSE);
		}
	}

	m_wndStatusBar.SetPaneText( 0, szStatusBarText, true);
}

void CAboutDlg::OnLButtonDown( UINT /*uFlags*/, CPoint ptPos) 
{
	UINT uHit = HitTest( ptPos);

	if (uHit != HIT_NONE)
	{
		PlaySound( NULL, NULL, SND_SYNC | SND_PURGE | SND_NODEFAULT);

		PlaySound( MAKEINTRESOURCE( IDR_WAVE_FIRE), AfxGetInstanceHandle(), 
				SND_SYNC | SND_RESOURCE | SND_NODEFAULT);
		bPlaying = TRUE;
	}

	if (uHit != HIT_NONE)
	{
		char* szAction = "";

		switch (uHit)
		{
		case HIT_WWW_WALLY:
			szAction = "http://home.telefragged.com/wally/";
			break;
		case HIT_BOTH:
			szAction = "mailto:ty@wwa.com;Neal_White_III@HotMail.com?subject=Wally";
			break;
		case HIT_TY:
			// Auuuuughhhhhhhhh... I'm hit!!  I'm hit!!   Just... *cough*  ...go... on... *cough*  ... w i t h o u t    m e.....
			szAction = "mailto:ty@wwa.com?subject=Wally";
			break;
		case HIT_NEAL:
			szAction = "mailto:Neal_White_III@HotMail.com?subject=Wally";
			break;
		case HIT_BOARD:
			szAction = "http://home.telefragged.com/wally/forum/index.shtml";
			break;
		default:
			ASSERT( FALSE);
			break;
		}
		ShellExecute( m_hWnd, NULL, szAction, NULL, NULL, SW_SHOWDEFAULT);
	}
//	else	// HIT_NONE
//	{
//		// just ignore the mouse click
//	}
}

void CAboutDlg::OnTimer( UINT uIDEvent) 
{
	switch (uIDEvent)
	{
	case TIMER_FLICKER:
		{
			if (GetActiveWindow() != this)
				return;

			CRect rBmpArea;
			m_ctrlBitmapArea.GetWindowRect( rBmpArea);
			ScreenToClient( &rBmpArea);

			CPoint ptPos;
			GetCursorPos( &ptPos);
			ScreenToClient( &ptPos);

			static BOOL bToggle = TRUE;

			bToggle = ! bToggle;

			if (bToggle)
			{
				CRect Rect( rWwwWally.left, rWwwWally.top, rBoard.right, rBoth.bottom);
				Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

				InvalidateRect( &Rect, FALSE);
				UpdateWindow();
			}
			else
			{
				UINT uHit = HitTest( ptPos);

				if (uHit != HIT_NONE)
				{
					CDC* pDC = GetDC();

					if (pDC)
					{
						CBitmap Bmp;
						Bmp.LoadBitmap( IDB_SPLASH_ON);

						CDC MemDC;
						MemDC.CreateCompatibleDC( pDC);
						MemDC.SelectObject( Bmp);

						CRect Rect = HitRect( uHit);
						Rect.OffsetRect( rBmpArea.left, rBmpArea.top);

						pDC->BitBlt( Rect.left, Rect.top, Rect.Width(), Rect.Height(),
								&MemDC, Rect.left - rBmpArea.left, Rect.top - rBmpArea.top, SRCCOPY);

						ReleaseDC( pDC);
					}
				}
			}
		}
		break;
	default:
		CDialog::OnTimer( uIDEvent);
		break;
	}
}

BOOL IsPowerOf2( int iVal)
{
	for (int i = 1; i < 16; i *= 2)
	{
		if (iVal == i)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsValidImageSize( int iWidth, int iHeight, DWORD flgAllowableSizes)
{
	BOOL bRet = FALSE;

	if ((iWidth < MAX_TEXTURE_WIDTH) && (iHeight < MAX_TEXTURE_HEIGHT))
	{
		if ((iWidth <= 0) || (iHeight <= 0))
		{
			ASSERT( FALSE);
		}
		else
		{
			bRet = TRUE;		// ALLOW_ANY_SIZE by default

			if (flgAllowableSizes & FORCE_X_16)
			{
				bRet = ((iWidth & 15) == 0) && ((iHeight & 15) == 0);
			}

			if (flgAllowableSizes & FORCE_X_POWER_OF_2)
			{
				bRet = bRet && IsPowerOf2( iWidth) && IsPowerOf2 (iHeight);
			}
		}
	}
	return bRet;
}


void CWallyApp::OnFileTestTxl() 
{
#if _MSC_VER >= 1200	
#	ifdef GENESIS_3D
	LoadTXLFile();
#	endif 
#else
	// The Genesis3D stuff requires compiling with VC6
	ASSERT (FALSE);
#endif
}

void CWallyApp::OnUpdateFileTestTxl(CCmdUI* pCmdUI) 
{
#ifdef _DEBUG
	pCmdUI->Enable (TRUE);
#else
	pCmdUI->Enable (FALSE);
#endif
}


void CWallyApp::OnFileDebug() 
{
	DebugDibTemplate->OpenDocumentFile (NULL, TRUE);

	/*
	int iReturn = AfxMessageBox ("Do you want to set the string?", MB_YESNO);

	if (iReturn == IDYES)
	{
		TestDoModal("Hehehehehe");
	}
	else
	{
		TestDoModal(NULL);
	}
	*/
}


void CWallyApp::OnUpdateFileDebug(CCmdUI* pCmdUI) 
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


int CWallyApp::Run() 
{
	int iReturn = 0;
	CString strError("");

	try
	{
		iReturn = CWinApp::Run();
	}
	catch (CMemoryException me)
	{
		TCHAR szCause[255];
		
		me.GetErrorMessage(szCause, sizeof(szCause));

		strError.Format( "%s%s%s", 
				"Memory Exception caught in CWallyApp::Run().\n",
				szCause,
				"\nProgram is terminating.");

		::MessageBox( NULL, (LPCTSTR)strError, "Wally", MB_ICONSTOP);		
	}
	catch (CWallyException we)
	{
		::MessageBox( NULL, we.GetErrorMessage(), "Wally", MB_ICONSTOP);		
	}
	/*
	catch(...)
	{
		LPVOID lpMsgBuf;
		DWORD dwError = GetLastError();
		
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);		
		
		strError.Format( "Unhandled exception caught in CWallyApp::Run().\nGetLastError() = %lu\n%s\nPlease contact the authors.\nProgram is terminating.", dwError, (LPCTSTR)lpMsgBuf);
		::MessageBox( NULL, (LPCTSTR)strError, "Wally", MB_ICONSTOP);
		LocalFree( lpMsgBuf );
	}
	*/

	return iReturn;
}

void CWallyApp::OnWizardColorDecal()
{
	CColorDecalDlg::OnWizardColorDecal( this );
}

void CWallyApp::OnWizardWadmerge() 
{
	CWadMergeDlg dlgWadMerge;
	if( dlgWadMerge.DoModal() == IDOK )
	{
	}
}

#if 0
void CWallyApp::OnWizardTest() 
{
	CImageHelper ihHelper;
	CImageHelper ihBump;
	CImageHelper ihWrite;
	int iWidth, iHeight, x;
	double dfRatio = 0.0, r, g, b;
	LPBYTE pSource = NULL;
	LPBYTE pBump = NULL;

	ihHelper.LoadImage( "c:\\source.jpg" );
	if( ihHelper.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	iWidth = ihHelper.GetImageWidth();
	iHeight = ihHelper.GetImageHeight();

	ihBump.LoadImage( "c:\\bump.jpg" );
	if( ihHelper.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	if( (ihBump.GetImageWidth() != iWidth) || (ihBump.GetImageWidth() != iWidth) )
	{
		return;
	}

	pSource = ihHelper.GetBits(); 
	pBump = ihBump.GetBits();	

	LPBYTE pDest = new BYTE[ iWidth * iHeight * 3 ];
	for( x = 0; x < iWidth * iHeight; x++ )
	{
		// Take the bump value, divide it by 128
		dfRatio = (pBump[x * 3] * 1.0) / 128.0;
		dfRatio *= 1.5;		// clamping factor... > 1 means more bump, < 1 means less bump

		// Now multiply that by each of the source pixels
		r = pSource[x * 3 + 0] * 1.0 * dfRatio;
		g = pSource[x * 3 + 1] * 1.0 * dfRatio;
		b = pSource[x * 3 + 2] * 1.0 * dfRatio;

		// Now blend with the source and write it out to the destination
		pDest[x * 3 + 0] = (BYTE)( min( (r + (pSource[x * 3 + 0] * 1.0)) / 2.0, 255.0) );
		pDest[x * 3 + 1] = (BYTE)( min( (g + (pSource[x * 3 + 1] * 1.0)) / 2.0, 255.0) );
		pDest[x * 3 + 2] = (BYTE)( min( (b + (pSource[x * 3 + 2] * 1.0)) / 2.0, 255.0) );
	}

	ihWrite.SaveImage( IH_24BIT, "c:\\dest.jpg", pDest, NULL, iWidth, iHeight );
	delete []pDest;
	pDest = NULL;
}
#endif

#if 0
void CWallyApp::OnWizardTest() 
{
	CImageHelper ihHelper;
	CImageHelper ihInfest;
	CImageHelper ihShade;
	CImageHelper ihWrite;
	int iWidth, iHeight, x;
	double dfInfest = 0.0, dfSource = 0.0;
	BYTE r, g, b;
	double rSo, gSo, bSo;
	double rIf, gIf, bIf;
	LPBYTE pSource = NULL;
	LPBYTE pInfest = NULL;
	LPBYTE pShade = NULL;

	ihHelper.LoadImage( "c:\\source.tga" );
	if( ihHelper.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	iWidth = ihHelper.GetImageWidth();
	iHeight = ihHelper.GetImageHeight();

	ihInfest.LoadImage( "c:\\infest.tga" );
	if( ihInfest.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	if( (ihInfest.GetImageWidth() != iWidth) || (ihInfest.GetImageWidth() != iWidth) )
	{
		return;
	}

	/*ihShade.LoadImage( "c:\\shade.pcx" );
	if( ihShade.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	if( (ihShade.GetImageWidth() != iWidth) || (ihShade.GetImageWidth() != iWidth) )
	{
		return;
	}
	*/

	pSource = ihHelper.GetBits(); 
	pInfest = ihInfest.GetBits();
	//pShade = ihShade.GetBits();

	// Generate the shade image
	int iSize = iWidth * iHeight;
	int iResizeWidth, iResizeHeight;
	int iOffset = 0;
	C2PassScale <CBilinearFilter> ScaleEngine;
	CMemBuffer mbIRGBData( "mbIRGBData" );	
	LPCOLOR_IRGB pIRGBData = ( LPCOLOR_IRGB )mbIRGBData.GetBuffer( iSize * sizeof( COLOR_IRGB ) );

	iResizeWidth = iWidth / 8;
	iResizeHeight = iHeight / 8;

	for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
	{
		r = pInfest[(iOffset * 3) + 0];
		g = pInfest[(iOffset * 3) + 1];
		b = pInfest[(iOffset * 3) + 2];		

		if( (r == 0) && (g == 0) && (b == 255) )
		{
			// Blue turns black
			pIRGBData[iOffset] = IRGB( 0, 0, 0, 0 );
		}
		else
		{
			// Anything else turns white
			pIRGBData[iOffset] = IRGB( 0, 255, 255, 255 );
		}
	}

	COLOR_IRGB* pIRGBDestBuffer = ScaleEngine.AllocAndScale( pIRGBData, iWidth, iHeight, iResizeWidth, iResizeHeight );
	
	// Now reverse it and go back
	pIRGBData = ScaleEngine.AllocAndScale( pIRGBDestBuffer, iResizeWidth, iResizeHeight, iWidth, iHeight );

	LPBYTE pDest = new BYTE[ iWidth * iHeight * 3 ];
	for( x = 0; x < iWidth * iHeight; x++ )
	{
		// Get the shade values
		//r = pShade[x * 3 + 0];
		r = GetRValue( pIRGBData[x] );

		// Calculate the ratios
		dfInfest = (r * 2.0) / 255.0;
		dfSource = (510.0 - (r * 2.0)) / 255.0;

		if( 
			(pInfest[x * 3 + 0] == 0) && 
			(pInfest[x * 3 + 1] == 0) && 
			(pInfest[x * 3 + 2] == 255)
		)
		{
			// regardless of the shade value, always use the source
			dfInfest = 0.0;
			dfSource = 2.0;
		}

		rSo = pSource[x * 3 + 0] * 1.0;
		gSo = pSource[x * 3 + 1] * 1.0;
		bSo = pSource[x * 3 + 2] * 1.0;

		rIf = pInfest[x * 3 + 0] * 1.0;
		gIf = pInfest[x * 3 + 1] * 1.0;
		bIf = pInfest[x * 3 + 2] * 1.0;

#if 1
		// Now blend the source with infest and write it out to the destination
		pDest[x * 3 + 0] = (BYTE)( ((rIf * dfInfest) + (rSo * dfSource)) / 2.0 );
		pDest[x * 3 + 1] = (BYTE)( ((gIf * dfInfest) + (gSo * dfSource)) / 2.0 );
		pDest[x * 3 + 2] = (BYTE)( ((bIf * dfInfest) + (bSo * dfSource)) / 2.0 );
#else
		pDest[x * 3 + 0] = pDest[x * 3 + 1] = pDest[x * 3 + 2] = r;
#endif
	}

	ihWrite.SaveImage( IH_24BIT, "c:\\dest.jpg", pDest, NULL, iWidth, iHeight );
	delete []pDest;
	pDest = NULL;
}
#endif

void CWallyApp::OnWizardTest() 
{
	CImageHelper ihHelper;
	CImageHelper ihWrite;
	CMemBuffer mbIRGBData( "mbIRGBData" );
	int iWidth, iHeight;
	bool bResize = true;
	BYTE r, g, b;
	LPBYTE pSource = NULL;
	LPBYTE pDest = NULL;
	
	CDirectoryList DList;
	CDirectoryEntry *pEntry = NULL;
	CFileList *pList = NULL;
	CFileItem *pFile = NULL;
	DList.AddWildcard( "*.tga" );
	DList.AddDirectory( "g:\\textures\\tga" );

	FILE *fp = fopen( "c:\\output.txt", "w" );
	
	pEntry = DList.GetFirst();
	while( pEntry != NULL )
	{
		pList = pEntry->GetFileList();
		pFile = pList->GetFirst();
		while( pFile != NULL )
		{
			ihHelper.LoadImage( pFile->GetFileName() );			

			iWidth = ihHelper.GetImageWidth();
			iHeight = ihHelper.GetImageHeight();

			int iSize = iWidth * iHeight;
			int iResizeWidth, iResizeHeight;
			int iOffset = 0;
			
			// Figure out the sizes
			int iMax = MAX( iWidth, iHeight );
			double dScale = 0.0;
			if( iMax <= 64 )
			{
				pFile = pFile->GetNext();
				continue;
			}

			bResize = true;

			if( iMax == 128 )
			{
				if( iWidth == 128 )
				{
					iResizeWidth = 64;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 128 )
				{
					iResizeHeight = 64;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 256 )
			{
				if( iWidth == 256 )
				{
					iResizeWidth = 64;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 256 )
				{
					iResizeHeight = 64;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 384 )
			{
				if( iWidth == 384 )
				{
					iResizeWidth = 192;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 384 )
				{
					iResizeHeight = 192;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 512 )
			{
				if( iWidth == 512 )
				{
					iResizeWidth = 128;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 512 )
				{
					iResizeHeight = 128;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 640 )
			{
				if( iWidth == 640 )
				{
					iResizeWidth = 128;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 640 )
				{
					iResizeHeight = 128;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 768 )
			{
				if( iWidth == 768 )
				{
					iResizeWidth = 384;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 768 )
				{
					iResizeHeight = 384;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else if( iMax == 1024 )
			{
				if( iWidth == 1024 )
				{
					iResizeWidth = 256;
					dScale = (iWidth * 1.0) / (iHeight * 1.0);
					iResizeHeight = iResizeWidth / dScale;
				}
				else if( iHeight == 1024 )
				{
					iResizeHeight = 256;
					dScale = (iHeight * 1.0) / (iWidth * 1.0);
					iResizeWidth = iResizeHeight / dScale;
				}				
			}
			else
			{
				bResize = false;
			}

			if( bResize )
			{
				fprintf( fp, "%ld x %ld ==> %ld x %ld\n", iWidth, iHeight, iResizeWidth, iResizeHeight );
				C2PassScale <CBilinearFilter> ScaleEngine;
				LPCOLOR_IRGB pIRGBData = ( LPCOLOR_IRGB )mbIRGBData.GetBuffer( iSize * sizeof( COLOR_IRGB ) );
				pSource = ihHelper.GetBits();

				for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
				{
					if( ihHelper.GetColorDepth() == IH_24BIT )
					{
						r = pSource[(iOffset * 3) + 0];
						g = pSource[(iOffset * 3) + 1];
						b = pSource[(iOffset * 3) + 2];
					}
					else if( ihHelper.GetColorDepth() == IH_32BIT )
					{
						r = pSource[(iOffset * 4) + 0];
						g = pSource[(iOffset * 4) + 1];
						b = pSource[(iOffset * 4) + 2];		
					}
					pIRGBData[iOffset] = IRGB( 0, r, g, b );
				}

				COLOR_IRGB* pIRGBDestBuffer = ScaleEngine.AllocAndScale( pIRGBData, iWidth, iHeight, iResizeWidth, iResizeHeight );	

				pDest = mbIRGBData.GetBuffer( iResizeWidth * iResizeHeight * 3 );

				for( iOffset = 0; iOffset < (iResizeWidth * iResizeHeight); iOffset++ )
				{
					pDest[(iOffset * 3) + 0] = GetRValue( pIRGBDestBuffer[iOffset] );
					pDest[(iOffset * 3) + 1] = GetGValue( pIRGBDestBuffer[iOffset] );
					pDest[(iOffset * 3) + 2] = GetBValue( pIRGBDestBuffer[iOffset] );					
				}
				
				CString strOutput("");
				CString strFileName = GetRawFileName( pFile->GetFileName() );
				strOutput = "g:\\textures\\output\\" + strFileName + ".jpg";
				ihWrite.SaveImage( IH_24BIT, strOutput, pDest, NULL, iResizeWidth, iResizeHeight );
			}
			else
			{
				fprintf( fp, "%ld x %ld ==> %s unchanged\n", iWidth, iHeight, pFile->GetFileName() );
			}			
			
			pFile = pFile->GetNext();			
		}
		pEntry = pEntry->GetNext();
	}

	fclose( fp );
		

#if 0

	ihHelper.LoadImage( "c:\\source.tga" );
	if( ihHelper.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	iWidth = ihHelper.GetImageWidth();
	iHeight = ihHelper.GetImageHeight();

	ihInfest.LoadImage( "c:\\infest.tga" );
	if( ihInfest.GetColorDepth() != IH_24BIT )
	{
		return;
	}

	if( (ihInfest.GetImageWidth() != iWidth) || (ihInfest.GetImageWidth() != iWidth) )
	{
		return;
	}
	
	pSource = ihHelper.GetBits(); 
	pInfest = ihInfest.GetBits();
	//pShade = ihShade.GetBits();

	// Generate the shade image
	int iSize = iWidth * iHeight;
	int iResizeWidth, iResizeHeight;
	int iOffset = 0;
	C2PassScale <CBilinearFilter> ScaleEngine;
	CMemBuffer mbIRGBData( "mbIRGBData" );	
	LPCOLOR_IRGB pIRGBData = ( LPCOLOR_IRGB )mbIRGBData.GetBuffer( iSize * sizeof( COLOR_IRGB ) );

	iResizeWidth = iWidth / 8;
	iResizeHeight = iHeight / 8;

	for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
	{
		r = pInfest[(iOffset * 3) + 0];
		g = pInfest[(iOffset * 3) + 1];
		b = pInfest[(iOffset * 3) + 2];		

		if( (r == 0) && (g == 0) && (b == 255) )
		{
			// Blue turns black
			pIRGBData[iOffset] = IRGB( 0, 0, 0, 0 );
		}
		else
		{
			// Anything else turns white
			pIRGBData[iOffset] = IRGB( 0, 255, 255, 255 );
		}
	}

	COLOR_IRGB* pIRGBDestBuffer = ScaleEngine.AllocAndScale( pIRGBData, iWidth, iHeight, iResizeWidth, iResizeHeight );
	
	// Now reverse it and go back
	pIRGBData = ScaleEngine.AllocAndScale( pIRGBDestBuffer, iResizeWidth, iResizeHeight, iWidth, iHeight );

	LPBYTE pDest = new BYTE[ iWidth * iHeight * 3 ];
	for( x = 0; x < iWidth * iHeight; x++ )
	{
		// Get the shade values
		//r = pShade[x * 3 + 0];
		r = GetRValue( pIRGBData[x] );

		// Calculate the ratios
		dfInfest = (r * 2.0) / 255.0;
		dfSource = (510.0 - (r * 2.0)) / 255.0;

		if( 
			(pInfest[x * 3 + 0] == 0) && 
			(pInfest[x * 3 + 1] == 0) && 
			(pInfest[x * 3 + 2] == 255)
		)
		{
			// regardless of the shade value, always use the source
			dfInfest = 0.0;
			dfSource = 2.0;
		}

		rSo = pSource[x * 3 + 0] * 1.0;
		gSo = pSource[x * 3 + 1] * 1.0;
		bSo = pSource[x * 3 + 2] * 1.0;

		rIf = pInfest[x * 3 + 0] * 1.0;
		gIf = pInfest[x * 3 + 1] * 1.0;
		bIf = pInfest[x * 3 + 2] * 1.0;
	}

	ihWrite.SaveImage( IH_24BIT, "c:\\dest.jpg", pDest, NULL, iWidth, iHeight );
	delete []pDest;
	pDest = NULL;
#endif
}