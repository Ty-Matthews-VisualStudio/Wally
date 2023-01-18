/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyPropertyPage1.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Wally.h"
#include "MainFrm.h"
#include "WallyPropertyPage1.h"
#include "WallyDocTemplate.h"
#include "WallyDoc.h"
#include "WallyUndo.h"
//#include "ReMip.h"
#include "FreeUndoDlg.h"
#include "ImageHelper.h"
#include "PaletteDlg.h"
#include "GraphicsFunctions.h"
#include "CustomResource.h"
#include "MiscFunctions.h"
#include "PaletteEditorDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;
extern BOOL g_bCompressUndo;


IMPLEMENT_DYNCREATE( CWallyPropertyPage1, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage2, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage3, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage4, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage5, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage6, CPropertyPage)
IMPLEMENT_DYNCREATE( CWallyPropertyPage7, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage1 property page

CWallyPropertyPage1::CWallyPropertyPage1() : CPropertyPage(CWallyPropertyPage1::IDD)
{
	m_pDoc = NULL;

	CWallyUndo Undo;

	//{{AFX_DATA_INIT(CWallyPropertyPage1)
	m_iZoomValue           = g_iDefaultZoomValue;
	m_iPaletteConversion   = g_iPaletteConversion;
	m_iMaxUndoItems        = Undo.GetMaxItems();
	m_dfGamma              = g_dfGamma;
	m_bGammaPaletteToolbar = g_bGammaCorrectPaletteToolbar;
	//}}AFX_DATA_INIT

	m_psp.dwFlags          ^= PSP_HASHELP;
	m_bEditZoomValueLockOut = FALSE;

	m_dfOriginalGamma                     = g_dfGamma;
	m_bOriginalGammaCorrectPaletteToolbar = g_bGammaCorrectPaletteToolbar;
}

CWallyPropertyPage1::~CWallyPropertyPage1()
{
}

void CWallyPropertyPage1::DoDataExchange(CDataExchange* pDX)
{
	CWnd* pEdit = GetDlgItem( IDC_EDIT_ZOOMVALUE);

	if (pEdit != NULL)
	{
		CString strText;

		pEdit->GetWindowText( strText);

		if (strText.CompareNoCase( "Auto") == 0)
		{
			m_bEditZoomValueLockOut = TRUE;

			pEdit->SetWindowText( "0");

			m_bEditZoomValueLockOut = FALSE;
		}
	}
	
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage1)
	DDX_Control(pDX, IDC_EDIT_CACHE_GROOM, m_edCacheGroom);
	DDX_Control(pDX, IDC_SLIDER_GAMMA, m_CtrlGamma);
	DDX_Control(pDX, IDC_SPIN_MAX_ITEMS, m_SpinMaxItems);
	DDX_Control(pDX, IDC_SPIN_ZOOMVALUE, m_SpinZoomControl);
	DDX_Text(pDX, IDC_EDIT_ZOOMVALUE, m_iZoomValue);
	DDV_MinMaxInt(pDX, m_iZoomValue, 0, 16);
	DDX_Radio(pDX, IDC_RADIO_PALETTE_MAINTAIN, m_iPaletteConversion);
	DDX_Text(pDX, IDC_EDIT_MAX_ITEMS, m_iMaxUndoItems);
	DDV_MinMaxInt(pDX, m_iMaxUndoItems, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_GAMMA, m_dfGamma);
	DDV_MinMaxDouble(pDX, m_dfGamma, 0.75, 2.75);
	DDX_Check(pDX, IDC_CHECK_GAMMA_PALETTE_TOOLBAR, m_bGammaPaletteToolbar);
	//}}AFX_DATA_MAP

	if (pEdit != NULL)
	{
		CString strText;

		pEdit->GetWindowText( strText);

		if (strText.CompareNoCase( "0") == 0)
		{
			m_bEditZoomValueLockOut = TRUE;

			pEdit->SetWindowText( "Auto");

			m_bEditZoomValueLockOut = FALSE;
		}
	}
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage1)
	ON_EN_CHANGE(IDC_EDIT_GAMMA, OnChangeEditGamma)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_GAMMA_PALETTE_TOOLBAR, OnCheckGammaPaletteToolbar)
	ON_BN_CLICKED(IDC_BTN_FREE_UNDO_MEMORY, OnBtnFreeUndoMemory)
	ON_EN_CHANGE(IDC_EDIT_ZOOMVALUE, OnChangeEditZoomValue)
	ON_BN_CLICKED(IDC_RADIO_DONT_REMOVE_CACHE, OnRadioDontRemoveCache)
	ON_BN_CLICKED(IDC_RADIO_REMOVE_CACHE, OnRadioRemoveCache)
	ON_EN_KILLFOCUS(IDC_EDIT_CACHE_GROOM, OnKillfocusEditCacheGroom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWallyPropertyPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	UDACCEL Spinaccel[4];

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 1;	
	m_SpinZoomControl.SetAccel( 1, &Spinaccel[0]);		
	m_SpinZoomControl.SetRange( 0, MAX_ZOOM);

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 1;
	Spinaccel[1].nSec = 1;
	Spinaccel[1].nInc = 10;
	Spinaccel[2].nSec = 2;
	Spinaccel[2].nInc = 100;
	m_SpinMaxItems.SetAccel( 3, &Spinaccel[0]);	
	m_SpinMaxItems.SetRange( 1, 1000);

	KillTimer( TIMER_WPP_CHANGE_GAMMA);		// don't update yet!

	m_CtrlGamma.SetLineSize( 1);
	m_CtrlGamma.SetPageSize( 10);
	m_CtrlGamma.SetTicFreq( 25);
	m_CtrlGamma.SetRange( 75, 275);
	m_CtrlGamma.SetPos( (int )(m_dfGamma * 100.0));

	if (g_iExportColorDepth == IH_8BIT)
	{
		CheckDlgButton (IDC_RADIO_8BIT, 1);
	}
	else
	{
		CheckDlgButton (IDC_RADIO_24BIT, 1);
	}

	//////////////////////////////////////////////////////
	// check to see if freeing undo memory is possible //
	////////////////////////////////////////////////////

	int iCount = 0;
	POSITION Pos = theApp.GetFirstDocTemplatePosition();

	while (Pos != NULL)
	{
		CDocTemplate* pTemplate = theApp.GetNextDocTemplate( Pos);

		POSITION PosDoc = pTemplate->GetFirstDocPosition();

		while (PosDoc != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc( PosDoc);

			CWallyDoc* pWallyDoc = DYNAMIC_DOWNCAST( CWallyDoc, pDoc);
			if (pWallyDoc)
			{
				iCount++;
			}
		}
	}

	if (iCount > 0)
	{
		CWnd* pCtrl = GetDlgItem( IDC_BTN_FREE_UNDO_MEMORY);

		if (pCtrl)
			pCtrl->EnableWindow( TRUE);
	}
	else
	{
		ASSERT( m_pDoc == NULL);
	}
	CheckDlgButton (IDC_CHECK_COMPRESS,	g_bCompressUndo);

	m_edCacheGroom.EnableWindow (g_iBrowserCacheFileGroom == 0 ? FALSE : TRUE);	
	CheckDlgButton (g_iBrowserCacheFileGroom == 0 ? IDC_RADIO_DONT_REMOVE_CACHE : IDC_RADIO_REMOVE_CACHE, 1);

	CString strText("");
	strText.Format ("%d", g_iBrowserCacheFileGroom == 0 ? 7 : g_iBrowserCacheFileGroom);
	SetDlgItemText (IDC_EDIT_CACHE_GROOM, strText);

	// Ty-  for some reason, the standard text shows up at "0"... why?  Dunno...  can't 
	// find any conflict between resource IDs.
	strText = "Gamma Adjust Palette &Toolbar Bar As Well";
	SetDlgItemText (IDC_CHECK_GAMMA_PALETTE_TOOLBAR, strText);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWallyPropertyPage1::OnOK() 
{
	if (IsDlgButtonChecked( IDC_RADIO_PALETTE_MAINTAIN))	
		g_iPaletteConversion = PALETTE_CONVERT_MAINTAIN;
	
	if (IsDlgButtonChecked( IDC_RADIO_PALETTE_NEAREST))
		g_iPaletteConversion = PALETTE_CONVERT_NEAREST;

	if (IsDlgButtonChecked( IDC_RADIO_DONT_REMOVE_CACHE))
	{
		g_iBrowserCacheFileGroom = 0;
	}
	else
	{
		CString strText("");
		
		GetDlgItemText (IDC_EDIT_CACHE_GROOM, strText);
		g_iBrowserCacheFileGroom = min (atoi (strText), 9999);
		g_iBrowserCacheFileGroom = max (g_iBrowserCacheFileGroom, 1);
	}
	
	if (IsDlgButtonChecked( IDC_RADIO_8BIT))
	{
		g_iExportColorDepth = IH_8BIT;
	}
	else
	{
		g_iExportColorDepth = IH_24BIT;
	}

	g_bGammaCorrectPaletteToolbar = m_bGammaPaletteToolbar;
	g_iDefaultZoomValue         = m_iZoomValue;
	g_dfGamma                = m_dfGamma;

	POSITION Pos = theApp.GetFirstDocTemplatePosition();

	while (Pos != NULL)
	{
		CDocTemplate* pTemplate = theApp.GetNextDocTemplate( Pos);

		POSITION PosDoc = pTemplate->GetFirstDocPosition();

		while (PosDoc != NULL)
		{
			CDocument* pDoc = pTemplate->GetNextDoc( PosDoc);

			CWallyDoc* pWallyDoc = DYNAMIC_DOWNCAST( CWallyDoc, pDoc);
			if (pWallyDoc)
				pWallyDoc->m_Undo.SetMaxItems( m_iMaxUndoItems);
		}
	}
	g_bCompressUndo = IsDlgButtonChecked( IDC_CHECK_COMPRESS);

	CPropertyPage::OnOK();
}

void CWallyPropertyPage1::OnCancel() 
{
	g_dfGamma                     = m_dfOriginalGamma;
	g_bGammaCorrectPaletteToolbar = m_bOriginalGammaCorrectPaletteToolbar;

	theApp.UpdateAllDocs();
	((CMainFrame* )AfxGetMainWnd())->m_wndPaletteToolBar.Update( NULL, FALSE);

	CPropertyPage::OnCancel();
}

void CWallyPropertyPage1::OnKillfocusEditCacheGroom() 
{
	CString strText("");
	int iValue = 0;

	GetDlgItemText (IDC_EDIT_CACHE_GROOM, strText);
	iValue = min (atoi (strText), 9999);
	iValue = max (iValue, 1);

	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_CACHE_GROOM, strText);	
}

void CWallyPropertyPage1::OnRadioDontRemoveCache() 
{
	m_edCacheGroom.EnableWindow (FALSE);	
}

void CWallyPropertyPage1::OnRadioRemoveCache() 
{
	m_edCacheGroom.EnableWindow (TRUE);	
}


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage2 property page

CWallyPropertyPage2::CWallyPropertyPage2() : CPropertyPage(CWallyPropertyPage2::IDD)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags ^= PSP_HASHELP;
}

CWallyPropertyPage2::~CWallyPropertyPage2()
{
}

void CWallyPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage2)
	ON_BN_CLICKED(IDC_BTN_USE_RECOMMENDED, OnBtnUseRecommended)
	ON_BN_CLICKED(IDC_BTN_USE_QUAKE, OnBtnUseQuake)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWallyPropertyPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CheckDlgButton( IDC_CHECK_REBUILD_SUBMIPS, g_bRebuildSubMipsOnSave);
	CheckDlgButton( IDC_CHECK_AUTO_REMIP,      g_bAutoRemip);
	CheckDlgButton( IDC_CHECK_AUTO_REMIP2,     g_bAutoRemipAfterEdit);
	CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    g_bWeightedAverage);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, g_bErrorDiffusion);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     g_bNoise);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       !( g_bErrorDiffusion | g_bNoise));

	CheckDlgButton( IDC_RADIO_HEAVY,           (g_iSharpen == SHARPEN_HEAVY));
	CheckDlgButton( IDC_RADIO_MEDIUM,          (g_iSharpen == SHARPEN_MEDIUM));
	CheckDlgButton( IDC_RADIO_LIGHT,           (g_iSharpen == SHARPEN_LIGHT));
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   (g_iSharpen == SHARPEN_NONE));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWallyPropertyPage2::OnOK() 
{
	g_bRebuildSubMipsOnSave = IsDlgButtonChecked( IDC_CHECK_REBUILD_SUBMIPS);
	g_bAutoRemip            = IsDlgButtonChecked( IDC_CHECK_AUTO_REMIP);
	g_bAutoRemipAfterEdit   = IsDlgButtonChecked( IDC_CHECK_AUTO_REMIP2);
	g_bWeightedAverage      = IsDlgButtonChecked( IDC_CHECK_USE_WEIGHTED);

	g_bErrorDiffusion       = IsDlgButtonChecked( IDC_RADIO_ERROR_DIFUSSION);
	g_bNoise                = IsDlgButtonChecked( IDC_RADIO_WHITE_NOISE);

	if (IsDlgButtonChecked( IDC_RADIO_HEAVY))
		g_iSharpen = SHARPEN_HEAVY;
	else if (IsDlgButtonChecked( IDC_RADIO_MEDIUM))
		g_iSharpen = SHARPEN_MEDIUM;
	else if (IsDlgButtonChecked( IDC_RADIO_LIGHT))
		g_iSharpen = SHARPEN_LIGHT;
	else
		g_iSharpen = SHARPEN_NONE;

	CPropertyPage::OnOK();
}

void CWallyPropertyPage2::OnBtnUseRecommended() 
{
	CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    1);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, 1);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     0);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       0);

	CheckDlgButton( IDC_RADIO_HEAVY,           0);
	CheckDlgButton( IDC_RADIO_MEDIUM,          1);
	CheckDlgButton( IDC_RADIO_LIGHT,           0);
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   0);	
}

void CWallyPropertyPage2::OnBtnUseQuake() 
{
CheckDlgButton( IDC_CHECK_USE_WEIGHTED,    0);

	CheckDlgButton( IDC_RADIO_ERROR_DIFUSSION, 1);
	CheckDlgButton( IDC_RADIO_WHITE_NOISE,     0);
	CheckDlgButton( IDC_RADIO_NO_DITHER,       0);

	CheckDlgButton( IDC_RADIO_HEAVY,           0);
	CheckDlgButton( IDC_RADIO_MEDIUM,          0);
	CheckDlgButton( IDC_RADIO_LIGHT,           0);
	CheckDlgButton( IDC_RADIO_NO_SHARPENING,   1);	
}

void CWallyPropertyPage1::OnChangeEditGamma() 
{
	SetTimer( TIMER_WPP_CHANGE_GAMMA, 500, NULL);
}

void CWallyPropertyPage1::OnTimer( UINT uIDEvent) 
{
	switch (uIDEvent)
	{
	case TIMER_WPP_CHANGE_GAMMA:
		{
			KillTimer( TIMER_WPP_CHANGE_GAMMA);

			//double  dfGamma                     = g_dfGamma;
			//BOOL    bGammaCorrectPaletteToolbar = g_bGammaCorrectPaletteToolbar;
			CString strGamma; 

			g_bGammaCorrectPaletteToolbar = 
						IsDlgButtonChecked( IDC_CHECK_GAMMA_PALETTE_TOOLBAR);

			GetDlgItemText( IDC_EDIT_GAMMA, strGamma);

			if (strGamma.GetLength() > 0)
			{
				g_dfGamma = atof( strGamma);
				g_dfGamma = max( 0.75, g_dfGamma);
				g_dfGamma = min( 2.75, g_dfGamma);
			}

			theApp.UpdateAllDocs();
			((CMainFrame* )AfxGetMainWnd())->m_wndPaletteToolBar.Update( NULL, FALSE);

			//g_dfGamma                     = dfGamma;
			//g_bGammaCorrectPaletteToolbar = bGammaCorrectPaletteToolbar;
		}
		break;

	default:
		ASSERT( FALSE);
	}
}

void CWallyPropertyPage1::OnHScroll( UINT uSBCode, UINT uPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_GAMMA)
	{
		char szText[40];
		sprintf_s( szText, sizeof(szText), "%.2f", m_CtrlGamma.GetPos() / 100.0);
		SetDlgItemText( IDC_EDIT_GAMMA, szText);
	}
}

void CWallyPropertyPage1::OnCheckGammaPaletteToolbar() 
{
	SetTimer( TIMER_WPP_CHANGE_GAMMA, 10, NULL);
}

void CWallyPropertyPage1::OnBtnFreeUndoMemory() 
{
	CFreeUndoDlg Dlg;

	Dlg.m_pDoc = m_pDoc;

	CWnd* pParent = GetParent();

	if (! pParent)
		pParent = this;

	pParent->ShowWindow( SW_HIDE);

	Dlg.DoModal();

	pParent->ShowWindow( SW_SHOW);
}

void CWallyPropertyPage1::OnChangeEditZoomValue() 
{
	if (! m_bEditZoomValueLockOut)
	{
		CWnd* pEdit = GetDlgItem( IDC_EDIT_ZOOMVALUE);

		CString strText;
		pEdit->GetWindowText( strText);

		if (strText == "0")
		{
			m_bEditZoomValueLockOut = TRUE;

			if (m_SpinZoomControl.m_hWnd != NULL)
				m_SpinZoomControl.SetPos( 0);

			pEdit->SetWindowText( "Auto");

			m_bEditZoomValueLockOut = FALSE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage3 dialog

CWallyPropertyPage3::CWallyPropertyPage3() : CPropertyPage(CWallyPropertyPage3::IDD)

//CWallyPropertyPage3::CWallyPropertyPage3(CWnd* pParent /*=NULL*/)
	//: CDialog(CWallyPropertyPage3::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage3)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags          ^= PSP_HASHELP;
}


void CWallyPropertyPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage3)
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_cbFileType);
	DDX_Control(pDX, IDC_EDIT_PRESETDIR, m_PresetEditWnd);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_SpinHeight);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage3, CDialog)
	//{{AFX_MSG_MAP(CWallyPropertyPage3)
	ON_BN_CLICKED(IDC_RADIO_BLANKDIR, OnRadioBlankdir)
	ON_BN_CLICKED(IDC_RADIO_PRESETDIR, OnRadioPresetdir)
	ON_BN_CLICKED(IDC_RADIO_PARENTDIR, OnRadioParentdir)
	ON_EN_KILLFOCUS(IDC_EDIT_PRESETDIR, OnKillfocusEditPresetdir)
	ON_BN_CLICKED(IDC_CHECK_NON16, OnCheckNon16)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage3 message handlers

BOOL CWallyPropertyPage3::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UDACCEL Spinaccel[4];

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 16;
	Spinaccel[1].nSec = 1;
	Spinaccel[1].nInc = 32;
	Spinaccel[2].nSec = 2;
	Spinaccel[2].nInc = 64;
	Spinaccel[3].nSec = 3;
	Spinaccel[3].nInc = 128;
	m_SpinWidth.SetAccel(4, &Spinaccel[0]);
	m_SpinHeight.SetAccel(4, &Spinaccel[0]);

	m_SpinHeight.SetRange(16, MAX_TEXTURE_HEIGHT);
	m_SpinWidth.SetRange(16, MAX_TEXTURE_WIDTH);

	CheckDlgButton( IDC_RADIO_BLANKDIR,   (g_iTextureNameSetting == BLANK_DIR));
	CheckDlgButton( IDC_RADIO_PRESETDIR,  (g_iTextureNameSetting == PRESET_DIR));
	CheckDlgButton( IDC_RADIO_PARENTDIR,  (g_iTextureNameSetting == PARENT_DIR));
	m_PresetEditWnd.EnableWindow (g_iTextureNameSetting == PRESET_DIR);

	SetDlgItemText (IDC_EDIT_PRESETDIR, g_strDefaultTextureName);

	CString strText ("");
	
	strText.Format ("%d", g_iDefaultTextureWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);
	strText.Format ("%d", g_iDefaultTextureHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strText);

	CheckDlgButton (IDC_CHECK_RETAIN_SIZE, g_bRetainTextureSize);
	CheckDlgButton (IDC_CHECK_NON16, g_bAllowNon16);

	int iItemAdded = 0;

	iItemAdded = m_cbFileType.AddString ("Half-Life Texture");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE);

	iItemAdded = m_cbFileType.AddString ("Half-Life Package [wad3] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE_WAD);

	iItemAdded = m_cbFileType.AddString ("Heretic2 Texture (.m8)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HERETIC2);

	iItemAdded = m_cbFileType.AddString ("JPEG - JFIF (.jpg)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_JPG);

	iItemAdded = m_cbFileType.AddString ("Portable Network Graphics (.png)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PNG);
	
	iItemAdded = m_cbFileType.AddString ("Quake2 Texture (.wal)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE2);

	iItemAdded = m_cbFileType.AddString ("Quake1 Texture (.mip)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1);

	iItemAdded = m_cbFileType.AddString ("Quake1 Package [wad2] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1_WAD);

	iItemAdded = m_cbFileType.AddString ("Serious Sam Texture (.tex)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_TEX);	

	iItemAdded = m_cbFileType.AddString ("SiN Texture (.swl)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_SIN);	

	iItemAdded = m_cbFileType.AddString ("Truevision Targa (.tga)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_TGA);

	iItemAdded = m_cbFileType.AddString ("Windows Bitmap (.bmp)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_BMP);

	iItemAdded = m_cbFileType.AddString ("Zsoft PC Paintbrush (.pcx)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PCX);
	

	// Always have this at the top
	m_cbFileType.InsertString (0, "Prompt");
	m_cbFileType.SetItemData (0, FILE_TYPE_PROMPT);

	m_cbFileType.SetCurSel(0);
	for (int j = 0; j < m_cbFileType.GetCount(); j++)
	{
		if ((int)m_cbFileType.GetItemData (j) == g_iFileTypeDefault)
		{
			m_cbFileType.SetCurSel(j);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWallyPropertyPage3::OnOK() 
{
	if (m_cbFileType.GetCurSel() != CB_ERR)
	{
		g_iFileTypeDefault = m_cbFileType.GetItemData (m_cbFileType.GetCurSel());
	}
	
	if (IsDlgButtonChecked( IDC_RADIO_BLANKDIR))
		g_iTextureNameSetting = BLANK_DIR;

	if (IsDlgButtonChecked(  IDC_RADIO_PRESETDIR))
		g_iTextureNameSetting = PRESET_DIR;

	if (IsDlgButtonChecked( IDC_RADIO_PARENTDIR))
		g_iTextureNameSetting = PARENT_DIR;

	CString strText ("");

	GetDlgItemText (IDC_EDIT_WIDTH, strText);
	g_iDefaultTextureWidth    = atol (strText);

	GetDlgItemText (IDC_EDIT_HEIGHT, strText);
	g_iDefaultTextureHeight   = atol (strText);
	
	GetDlgItemText (IDC_EDIT_PRESETDIR, g_strDefaultTextureName);

	g_bRetainTextureSize = IsDlgButtonChecked( IDC_CHECK_RETAIN_SIZE);	
	g_bAllowNon16 = IsDlgButtonChecked( IDC_CHECK_NON16);
}

void CWallyPropertyPage3::OnRadioBlankdir() 
{
	m_PresetEditWnd.EnableWindow (false);
}

void CWallyPropertyPage3::OnRadioPresetdir() 
{	
	m_PresetEditWnd.EnableWindow (true);
}

void CWallyPropertyPage3::OnRadioParentdir() 
{
	m_PresetEditWnd.EnableWindow (false);
	
}

void CWallyPropertyPage3::OnKillfocusEditPresetdir() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_PRESETDIR, strText);

	strText = TrimSlashes(strText);
	strText += "/";

	SetDlgItemText (IDC_EDIT_PRESETDIR, strText);
}

void CWallyPropertyPage3::OnCheckNon16() 
{
	if (IsDlgButtonChecked( IDC_CHECK_NON16))
	{
		CString strWarning("");
		strWarning.Format ("%s%s%s%s%s%s%s",
			"Quake-engine games do not support texture dimensions that are not\n",
			"multiples of 16.  Enabling this option will allow you to create\n",
			"textures that do not meet this requirement, HOWEVER, we take no\n",
			"responsibility for what might happen if you try and use these odd-sized\n",
			"textures in any game.  This feature is for advanced users who accept\n",
			"this risk and understand the consequences.\n\n",
			"Are you sure you want to enable this option?");
		if (AfxMessageBox (strWarning, MB_YESNO) != IDYES)
		{
			CheckDlgButton (IDC_CHECK_NON16, FALSE);
		}
	}	
}

void CWallyPropertyPage3::OnKillfocusEditHeight() 
{
	CString strText ("");
	int iHeight = 0;

	GetDlgItemText (IDC_EDIT_HEIGHT, strText);
	iHeight = atol (strText);
	
	iHeight = max (iHeight, 1);
	iHeight = min (iHeight, MAX_TEXTURE_HEIGHT);
	
	strText.Format ("%d", iHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strText);	
}

void CWallyPropertyPage3::OnKillfocusEditWidth() 
{
	CString strText ("");
	int iWidth = 0;

	GetDlgItemText (IDC_EDIT_WIDTH, strText);
	iWidth = atol (strText);
	
	iWidth = max (iWidth, 1);
	iWidth = min (iWidth, MAX_TEXTURE_WIDTH);
	
	strText.Format ("%d", iWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);	
}


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage4 dialog


CWallyPropertyPage4::CWallyPropertyPage4() : CPropertyPage(CWallyPropertyPage4::IDD)

//CWallyPropertyPage4::CWallyPropertyPage4(CWnd* pParent /*=NULL*/)
	//: CPropertyPage(CWallyPropertyPage4::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage4)
	m_iGridHorizontal = 0;
	m_iGridVertical = 0;
	//}}AFX_DATA_INIT
	m_psp.dwFlags          ^= PSP_HASHELP;
}


void CWallyPropertyPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage4)
	DDX_Text(pDX, IDC_EDIT_HORIZONTAL, m_iGridHorizontal);
	DDV_MinMaxInt(pDX, m_iGridHorizontal, 1, 1024);
	DDX_Text(pDX, IDC_EDIT_VERTICAL, m_iGridVertical);
	DDV_MinMaxInt(pDX, m_iGridVertical, 1, 1024);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage4, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage4)
	ON_BN_CLICKED(IDC_BTN_EVERY_1, OnBtnEvery1)
	ON_BN_CLICKED(IDC_BTN_EVERY_2, OnBtnEvery2)
	ON_BN_CLICKED(IDC_BTN_EVERY_4, OnBtnEvery4)
	ON_BN_CLICKED(IDC_BTN_EVERY_8, OnBtnEvery8)
	ON_BN_CLICKED(IDC_BTN_EVERY_10, OnBtnEvery10)
	ON_BN_CLICKED(IDC_BTN_EVERY_12, OnBtnEvery12)
	ON_BN_CLICKED(IDC_BTN_EVERY_16, OnBtnEvery16)
	ON_BN_CLICKED(IDC_BTN_EVERY_32, OnBtnEvery32)
	ON_BN_CLICKED(IDC_BTN_EVERY_64, OnBtnEvery64)
	ON_BN_CLICKED(IDC_CHECK_COURSE_GRID, OnCheckCourseGrid)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage4 message handlers

void CWallyPropertyPage4::OnBtnEvery1()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 1);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   1);
}

void CWallyPropertyPage4::OnBtnEvery2()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 2);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   2);
}

void CWallyPropertyPage4::OnBtnEvery4()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 4);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   4);
}

void CWallyPropertyPage4::OnBtnEvery8()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 8);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   8);
}

void CWallyPropertyPage4::OnBtnEvery10()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 10);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   10);
}

void CWallyPropertyPage4::OnBtnEvery12()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 12);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   12);
}

void CWallyPropertyPage4::OnBtnEvery16()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 16);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   16);
}

void CWallyPropertyPage4::OnBtnEvery32()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 32);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   32);
}

void CWallyPropertyPage4::OnBtnEvery64()
{
	SetDlgItemInt( IDC_EDIT_HORIZONTAL, 64);
	SetDlgItemInt( IDC_EDIT_VERTICAL,   64);
}

void CWallyPropertyPage4::OnCheckCourseGrid()
{
	BOOL bEnabled = IsDlgButtonChecked( IDC_CHECK_COURSE_GRID);

	EnableDlgItem( IDC_STATIC_EVERY_H,  bEnabled);
	EnableDlgItem( IDC_EDIT_HORIZONTAL, bEnabled);
	EnableDlgItem( IDC_STATIC_H_PIXELS, bEnabled);

	EnableDlgItem( IDC_STATIC_EVERY_V,  bEnabled);
	EnableDlgItem( IDC_EDIT_VERTICAL,   bEnabled);
	EnableDlgItem( IDC_STATIC_V_PIXELS, bEnabled);

	EnableDlgItem( IDC_BTN_EVERY_1,     bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_2,     bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_4,     bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_8,     bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_10,    bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_12,    bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_16,    bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_32,    bEnabled);
	EnableDlgItem( IDC_BTN_EVERY_64,    bEnabled);
}

void CWallyPropertyPage4::OnOK() 
{
	CPropertyPage::OnOK();

	g_bShowFineGrid   = IsDlgButtonChecked( IDC_CHECK_FINE_GRID);
	g_bShowCourseGrid = IsDlgButtonChecked( IDC_CHECK_COURSE_GRID);

	g_iGridHorizontal = m_iGridHorizontal;
	g_iGridVertical   = m_iGridVertical;
}

BOOL CWallyPropertyPage4::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CheckDlgButton( IDC_CHECK_FINE_GRID,   g_bShowFineGrid);
	CheckDlgButton( IDC_CHECK_COURSE_GRID, g_bShowCourseGrid);

	SetDlgItemInt(  IDC_EDIT_HORIZONTAL,   g_iGridHorizontal);
	SetDlgItemInt(  IDC_EDIT_VERTICAL,     g_iGridVertical);

	OnCheckCourseGrid();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CWallyPropertyPage4::OnBtnColor() 
{
	CPalettePickerDlg Dlg;

	CWallyPalette OldPal;
	OldPal = g_CurrentPalette;

	// Neal - grid color assert bugfix
	//
	g_CurrentPalette.ClearPalette();

	int i, j;

	double dfHue, dfSaturation, dfValue;
	dfSaturation = 1.0;

	for (j = 0; j < 16; j++)
	{
		dfHue = (j * 360.0) / 16.0;

		for (i = 0; i < 16; i++)
		{
			dfValue = i / 15.0;

			g_CurrentPalette.SetHSV( j*16+i, dfHue, dfSaturation, dfValue);
		}
	}

	// add a gray strip running down left side

	for (j = 0; j < 16; j++)
	{
		i = (j * 255) / 15;
		g_CurrentPalette.SetRGB( j*16, i, i, i);
	}


	Dlg.m_iIndex = g_CurrentPalette.FindNearestColor( GetRValue( g_rgbGridColor), 
			GetGValue( g_rgbGridColor), GetBValue( g_rgbGridColor), FALSE);

	if (IDOK == Dlg.DoModal())
	{
		g_rgbGridColor = g_CurrentPalette.GetRGB( Dlg.m_iIndex);

		GetDlgItem( IDC_BTN_COLOR)->InvalidateRect( NULL, FALSE);
	}

	g_CurrentPalette = OldPal;
}

void CWallyPropertyPage4::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl != IDC_BTN_COLOR)
	{
		ASSERT( FALSE);
		CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct);
		return;
	}

	CBrush brButtonColor( g_rgbGridColor);

	UINT uState = DFCS_BUTTONPUSH;
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uState |= DFCS_PUSHED;

	CRect rButton( lpDrawItemStruct->rcItem);

	DrawFrameControl( lpDrawItemStruct->hDC, &rButton,
			DFC_BUTTON, uState);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		rButton += CPoint( 1, 1);

	rButton += CRect( -4, -4, -5, -5);
	PatBlt( lpDrawItemStruct->hDC, rButton.left, rButton.top,
			rButton.Width(), rButton.Height(), BLACKNESS);

	InflateRect( &rButton, -1, -1);
	FillRect( lpDrawItemStruct->hDC, &rButton, brButtonColor);

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		InflateRect( &rButton, 3, 3);
		DrawFocusRect( lpDrawItemStruct->hDC, rButton);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage4 dialog


CWallyPropertyPage5::CWallyPropertyPage5() : CPropertyPage(CWallyPropertyPage5::IDD)

//CWallyPropertyPage5::CWallyPropertyPage5(CWnd* pParent /*=NULL*/)
	//: CPropertyPage(CWallyPropertyPage5::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage5)	
	//}}AFX_DATA_INIT
	m_psp.dwFlags          ^= PSP_HASHELP;
}


void CWallyPropertyPage5::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage5)	
	DDX_Control(pDX, IDC_BUTTON_Q2_CUSTOM, m_btnQ2);
	DDX_Control(pDX, IDC_BUTTON_Q1_CUSTOM, m_btnQ1);
	DDX_Control(pDX, IDC_EDIT_Q2_CUSTOM, m_edQ2Custom);
	DDX_Control(pDX, IDC_EDIT_Q1_CUSTOM, m_edQ1Custom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage5, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage5)	
	ON_BN_CLICKED(IDC_RADIO_Q1_DEFAULT, OnRadioQ1Default)
	ON_BN_CLICKED(IDC_RADIO_Q2_DEFAULT, OnRadioQ2Default)
	ON_BN_CLICKED(IDC_RADIO_Q1_CUSTOM, OnRadioQ1Custom)
	ON_BN_CLICKED(IDC_RADIO_Q2_CUSTOM, OnRadioQ2Custom)
	ON_BN_CLICKED(IDC_BUTTON_Q1_CUSTOM, OnButtonQ1Custom)
	ON_BN_CLICKED(IDC_BUTTON_Q2_CUSTOM, OnButtonQ2Custom)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT_PALETTE, OnButtonDefaultPalette)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_PALETTE, OnButtonEditPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage5 message handlers

BOOL CWallyPropertyPage5::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CheckDlgButton( IDC_RADIO_Q1_DEFAULT, g_iUseDefaultQ1Palette);
	CheckDlgButton( IDC_RADIO_Q1_CUSTOM, !g_iUseDefaultQ1Palette);	
	OnRadioQ1Default();

	SetDlgItemText (IDC_EDIT_Q1_CUSTOM, g_strQuake1Palette);
	
	CheckDlgButton( IDC_RADIO_Q2_DEFAULT, g_iUseDefaultQ2Palette);
	CheckDlgButton( IDC_RADIO_Q2_CUSTOM, !g_iUseDefaultQ2Palette);	
	OnRadioQ2Default();

	SetDlgItemText (IDC_EDIT_Q2_CUSTOM, g_strQuake2Palette);

	SetDlgItemText (IDC_EDIT_DEFAULT_PALETTE, g_strDefaultEditingPalette);
	CheckDlgButton (IDC_CHECK_BUILD_OPTIMIZED, g_bBuildOptimizedPalette);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWallyPropertyPage5::OnOK() 
{
	g_iUseDefaultQ1Palette = (IsDlgButtonChecked( IDC_RADIO_Q1_DEFAULT));	
	GetDlgItemText ( IDC_EDIT_Q1_CUSTOM, g_strQuake1Palette);

	g_iUseDefaultQ2Palette = (IsDlgButtonChecked( IDC_RADIO_Q2_DEFAULT));	
	GetDlgItemText ( IDC_EDIT_Q2_CUSTOM, g_strQuake2Palette);

	g_bBuildOptimizedPalette = IsDlgButtonChecked( IDC_CHECK_BUILD_OPTIMIZED);
	GetDlgItemText (IDC_EDIT_DEFAULT_PALETTE, g_strDefaultEditingPalette);

	CPropertyPage::OnOK();
}


void CWallyPropertyPage5::OnRadioQ1Default() 
{
	bool bEnable = IsDlgButtonChecked( IDC_RADIO_Q1_DEFAULT);
	m_edQ1Custom.EnableWindow (!bEnable);
	m_btnQ1.EnableWindow (!bEnable);
}

void CWallyPropertyPage5::OnRadioQ1Custom() 
{
	OnRadioQ1Default();
	m_edQ1Custom.SetFocus();
	m_edQ1Custom.SetSel (0, -1, false);
}


void CWallyPropertyPage5::OnRadioQ2Default() 
{
	bool bEnable = IsDlgButtonChecked( IDC_RADIO_Q2_DEFAULT);
	m_edQ2Custom.EnableWindow (!bEnable);
	m_btnQ2.EnableWindow (!bEnable);
	
}

void CWallyPropertyPage5::OnRadioQ2Custom() 
{
	OnRadioQ2Default();
	m_edQ2Custom.SetFocus();
	m_edQ2Custom.SetSel (0, -1, false);
	
}

void CWallyPropertyPage5::OnButtonQ1Custom() 
{
	SelectPalette ( IDC_EDIT_Q1_CUSTOM);
}

void CWallyPropertyPage5::OnButtonQ2Custom() 
{
	SelectPalette ( IDC_EDIT_Q2_CUSTOM);
	
}


void CWallyPropertyPage5::SelectPalette (int iID)
{
	CString strWildCard ("All Files (*.*)|*.*|");
	CImageHelper ihHelper;
	CString strAddString = ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath ("");
	
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
			AfxMessageBox ("Unsupported palette file", MB_ICONSTOP);
			return;			
		}
		else
		{
			SetDlgItemText ( iID, strSourceFilePath);
		}
	}
	
}


void CWallyPropertyPage5::OnButtonApply() 
{
	g_iUseDefaultQ1Palette = (IsDlgButtonChecked( IDC_RADIO_Q1_DEFAULT));	
	GetDlgItemText ( IDC_EDIT_Q1_CUSTOM, g_strQuake1Palette);

	g_iUseDefaultQ2Palette = (IsDlgButtonChecked( IDC_RADIO_Q2_DEFAULT));	
	GetDlgItemText ( IDC_EDIT_Q2_CUSTOM, g_strQuake2Palette);

	CImageHelper ihHelper;
	unsigned char *pbyPalette = NULL;

	CCustomResource crPalette;

	if (g_iUseDefaultQ1Palette)
	{
		//SetDefaultQ1Palette();
		crPalette.UseResourceId ("PALETTE", IDR_LMP_QUAKE1);
		memcpy (quake1_pal, crPalette.GetData(), 768);
	}
	else
	{
		ihHelper.LoadImage (g_strQuake1Palette, IH_LOAD_ONLYPALETTE);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());			
		}
		else
		{		
			pbyPalette = ihHelper.GetBits();
			memcpy (quake1_pal, pbyPalette, 768);
		}
	}


	if (g_iUseDefaultQ2Palette)
	{
		//SetDefaultQ2Palette();
		crPalette.UseResourceId ("PALETTE", IDR_LMP_QUAKE1);
		memcpy (quake1_pal, crPalette.GetData(), 768);
	}
	else
	{
		ihHelper.LoadImage (g_strQuake2Palette, IH_LOAD_ONLYPALETTE);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());			
		}
		else
		{		
			pbyPalette = ihHelper.GetBits();
			memcpy (quake2_pal, pbyPalette, 768);
		}
	}		
}



void CWallyPropertyPage5::OnButtonDefaultPalette() 
{	
	CString strWildCard ("All Files (*.*)|*.*|");
	CImageHelper ihHelper;
	CString strAddString = ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath;	

	if (dlgPalette.DoModal() == IDOK)
	{
		g_strOpenPaletteDirectory	= dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		g_iOpenPaletteExtension		= dlgPalette.m_ofn.nFilterIndex;
		strSourceFilePath			= dlgPalette.GetPathName();		

		ihHelper.LoadImage (strSourceFilePath);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;
		}
		
		if (ihHelper.IsValidPalette())
		{
			SetDlgItemText (IDC_EDIT_DEFAULT_PALETTE, strSourceFilePath);			
		}
		else
		{
			AfxMessageBox ("This doesn't appear to be a valid palette file.", MB_ICONSTOP);
		}
	}	
}

void CWallyPropertyPage5::OnButtonEditPalette() 
{
	CString strPaletteFile("");
	GetDlgItemText (IDC_EDIT_DEFAULT_PALETTE, strPaletteFile);

	CImageHelper ihHelper;
	ihHelper.LoadImage (strPaletteFile);

	if (ihHelper.GetErrorCode() != IH_SUCCESS)
	{
		AfxMessageBox (ihHelper.GetErrorText());
		return;
	}
		
	if (ihHelper.IsValidPalette())
	{
		CPaletteEditorDlg dlgEditPalette;
		dlgEditPalette.SetPalette (ihHelper.GetBits());

		if (dlgEditPalette.DoModal() == IDOK)
		{
			BYTE *pbyPalette = dlgEditPalette.GetPalette();

			CImageHelper ihWrite;			
			ihWrite.SaveImage (IH_8BIT, strPaletteFile, pbyPalette, pbyPalette, 0, 0);
		}		
	}
	else
	{
		AfxMessageBox ("This doesn't appear to be a valid palette file.", MB_ICONSTOP);
	}
	
}



/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage6 property page

CWallyPropertyPage6::CWallyPropertyPage6() : CPropertyPage(CWallyPropertyPage6::IDD)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage6)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags          ^= PSP_HASHELP;

	m_bCalledFromBatchDlg = FALSE;
}

CWallyPropertyPage6::~CWallyPropertyPage6()
{
}

void CWallyPropertyPage6::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage6)
	DDX_Control(pDX, IDC_CHECK_RETAIN_SETTINGS, m_btnRetainSettings);
	DDX_Control(pDX, IDC_SPIN_MAXTHREADS, m_spnMaxThreads);
	DDX_Control(pDX, IDC_EDIT_SOURCE_PALETTE, m_edSourcePalette);
	DDX_Control(pDX, IDC_EDIT_DEST_PALETTE, m_edDestPalette);
	DDX_Control(pDX, IDC_COMBO_IMAGE_TYPE, m_cbImages);
	DDX_Control(pDX, IDC_BUTTON_SOURCE, m_btnSource);
	DDX_Control(pDX, IDC_BUTTON_DEST, m_btnDest);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage6, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage6)
	ON_BN_CLICKED(IDC_BUTTON_DEST, OnButtonDest)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	ON_BN_CLICKED(IDC_RADIO_DEST_CURRENT, OnRadioDestCurrent)
	ON_BN_CLICKED(IDC_RADIO_DEST_CUSTOM, OnRadioDestCustom)
	ON_BN_CLICKED(IDC_RADIO_SOURCE_CURRENT, OnRadioSourceCurrent)
	ON_BN_CLICKED(IDC_RADIO_SOURCE_CUSTOM, OnRadioSourceCustom)
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_TYPE, OnSelchangeComboImageType)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_THREADS, OnKillfocusEditMaxThreads)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage6 message handlers

BOOL CWallyPropertyPage6::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	int iItemAdded = 0;
	BOOL bAtLeastOne = FALSE;

	CImageHelper ihHelper;

	CWildCardItem *pItem = ihHelper.GetFirstSupportedImage();

	while (pItem)
	{
		iItemAdded = m_cbImages.AddString (pItem->GetDescription());
		m_cbImages.SetItemData (iItemAdded, (DWORD)pItem);
		pItem = ihHelper.GetNextSupportedImage();
		bAtLeastOne = TRUE;
	}

	if (bAtLeastOne)
	{
		m_cbImages.SetCurSel(0);		
	}

	g_WildCardList.PrepareForChanges();

	CString strText ("");
	strText.Format ("%d", g_iMaxConversionThreads);
	SetDlgItemText (IDC_EDIT_MAX_THREADS, strText);	

	UDACCEL Spinaccel[4];

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 1;
	Spinaccel[1].nSec = 2;
	Spinaccel[1].nInc = 5;
	Spinaccel[2].nSec = 4;
	Spinaccel[2].nInc = 10;
	Spinaccel[3].nSec = 6;
	Spinaccel[3].nInc = 20;
	
	m_spnMaxThreads.SetAccel (4, Spinaccel);
	m_spnMaxThreads.SetRange (1, MAX_CONVERSION_THREADS);

	OnSelchangeComboImageType();

	CheckDlgButton (IDC_CHECK_OVERWRITE, g_bOverWriteFiles);
	CheckDlgButton (IDC_CHECK_RECURSE, g_bRecurseSubdirectories);
	CheckDlgButton (IDC_CHECK_RETAIN_DIRECTORY, g_bRetainDirectoryStructure);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWallyPropertyPage6::OnButtonDest() 
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		CString strPaletteFile = SelectPalette ();
		if (strPaletteFile != "")
		{
			SetDlgItemText (IDC_EDIT_DEST_PALETTE, strPaletteFile);
		
			CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
			pItem->SetDestPaletteFile (strPaletteFile);
		}
	}
}

void CWallyPropertyPage6::OnButtonSource() 
{
	int iCurSel = m_cbImages.GetCurSel();

	if (iCurSel != CB_ERR)
	{		
		CString strPaletteFile = SelectPalette ();
		if (strPaletteFile != "")
		{
			SetDlgItemText (IDC_EDIT_SOURCE_PALETTE, strPaletteFile);

			CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
			pItem->SetSourcePaletteFile (strPaletteFile);
		}
	}	
}

void CWallyPropertyPage6::OnRadioDestCurrent() 
{
	EnableDisableControls();	
}

void CWallyPropertyPage6::OnRadioDestCustom() 
{
	EnableDisableControls();	
}

void CWallyPropertyPage6::OnRadioSourceCurrent() 
{
	EnableDisableControls();	
}

void CWallyPropertyPage6::OnRadioSourceCustom() 
{
	EnableDisableControls();	
}

void CWallyPropertyPage6::OnSelchangeComboImageType() 
{
	int iCurSel = m_cbImages.GetCurSel();
	CImageHelper ihHelper;

	if (iCurSel != CB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));

		CWnd *pWnd = NULL;

		pWnd = GetDlgItem (IDC_RADIO_SOURCE_CURRENT);
		if (pWnd)
		{			
			pWnd->EnableWindow (pItem->GetSourceRegKey() != "");
		}

		pWnd = GetDlgItem (IDC_RADIO_SOURCE_CUSTOM);
		if (pWnd)
		{
			pWnd->EnableWindow (pItem->GetSourceRegKey() != "");
		}
		
		bool bUseCurrentSource = pItem->UseCurrentSourcePalette();

		CheckDlgButton (IDC_RADIO_SOURCE_CURRENT, bUseCurrentSource);
		CheckDlgButton (IDC_RADIO_SOURCE_CUSTOM, !bUseCurrentSource);

		pWnd = GetDlgItem (IDC_RADIO_DEST_CURRENT);
		if (pWnd)
		{			
			pWnd->EnableWindow (pItem->GetDestRegKey() != "");
		}

		pWnd = GetDlgItem (IDC_RADIO_DEST_CUSTOM);
		if (pWnd)
		{
			pWnd->EnableWindow (pItem->GetDestRegKey() != "");
		}

		bool bUseCurrentDest = pItem->UseCurrentDestPalette();

		CheckDlgButton (IDC_RADIO_DEST_CURRENT, bUseCurrentDest);
		CheckDlgButton (IDC_RADIO_DEST_CUSTOM, !bUseCurrentDest);

		SetDlgItemText ( IDC_EDIT_DEST_PALETTE, pItem->GetDestPaletteFile());
		SetDlgItemText ( IDC_EDIT_SOURCE_PALETTE, pItem->GetSourcePaletteFile());
	}
	EnableDisableControls();	
}

void CWallyPropertyPage6::EnableDisableControls()
{
	int iCurSel = m_cbImages.GetCurSel();
	CImageHelper ihHelper;

	if (iCurSel != CB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)(m_cbImages.GetItemData(iCurSel));
		
		bool bHasSourceRegKey = (pItem->GetSourceRegKey() != "");
		//= ihHelper.IsGameType (pItem->GetImageType());
		
		bool bUseCurrentSource = IsDlgButtonChecked (IDC_RADIO_SOURCE_CURRENT);
		pItem->m_bTempUseCurrentSourcePalette = bUseCurrentSource;

		m_edSourcePalette.EnableWindow (bHasSourceRegKey && !bUseCurrentSource);
		m_btnSource.EnableWindow (bHasSourceRegKey && !bUseCurrentSource);

		bool bUseCurrentDest = IsDlgButtonChecked (IDC_RADIO_DEST_CURRENT);
		pItem->m_bTempUseCurrentDestPalette = bUseCurrentDest;

		m_edDestPalette.EnableWindow (!bUseCurrentDest);
		m_btnDest.EnableWindow (!bUseCurrentDest);
	}

	m_btnRetainSettings.EnableWindow (m_bCalledFromBatchDlg);	
}

CString CWallyPropertyPage6::SelectPalette ()
{
	CImageHelper ihHelper;
	
	CString strWildCard ("All Files (*.*)|*.*|");	
	CString strAddString = ihHelper.GetSupportedPaletteList();
	strWildCard += strAddString;

	CFileDialog dlgPalette (true, ".pal", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;	
	CString strSourceFilePath ("");
	
	if (dlgPalette.DoModal() == IDOK)
	{
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);
		g_iOpenPaletteExtension = dlgPalette.m_ofn.nFilterIndex;
		strSourceFilePath         = dlgPalette.GetPathName();		

		ihHelper.LoadImage (strSourceFilePath);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return "";
		}

		if (!ihHelper.IsValidPalette())
		{
			AfxMessageBox ("Unsupported palette file", MB_ICONSTOP);
			return "";
		}		
	}
	
	return strSourceFilePath;
}

void CWallyPropertyPage6::OnOK() 
{	
	CString strValue("");

	GetDlgItemText (IDC_EDIT_MAX_THREADS, strValue);
	int iMaxThreads = atoi(strValue);

	g_iMaxConversionThreads = min (iMaxThreads, MAX_CONVERSION_THREADS);
	g_iMaxConversionThreads = max (g_iMaxConversionThreads, 0);
	
	g_WildCardList.SolidifyAllChanges();

	if (m_bCalledFromBatchDlg)
	{
		if (IsDlgButtonChecked (IDC_CHECK_RETAIN_SETTINGS))
		{		
			g_WildCardList.WriteRegistry();
		}
	}
	else
	{
		g_WildCardList.WriteRegistry();
	}

	g_bOverWriteFiles = (IsDlgButtonChecked(IDC_CHECK_OVERWRITE) > 0);
	g_bRecurseSubdirectories = (IsDlgButtonChecked(IDC_CHECK_RECURSE) > 0);
	g_bRetainDirectoryStructure = (IsDlgButtonChecked(IDC_CHECK_RETAIN_DIRECTORY) > 0);

	CPropertyPage::OnOK();
}

void CWallyPropertyPage6::OnCancel() 
{
	g_WildCardList.CancelAllChanges();
	CPropertyPage::OnCancel();
}

void CWallyPropertyPage6::OnKillfocusEditMaxThreads() 
{
	CString strValue;

	GetDlgItemText (IDC_EDIT_MAX_THREADS, strValue);
	int iMaxThreads = atoi(strValue);

	iMaxThreads = min (iMaxThreads, MAX_CONVERSION_THREADS);
	iMaxThreads = max (iMaxThreads, 0);

	strValue.Format ("%d", iMaxThreads);		
	SetDlgItemText (IDC_EDIT_MAX_THREADS, strValue);	
}


/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage7 dialog


CWallyPropertyPage7::CWallyPropertyPage7() : CPropertyPage(CWallyPropertyPage7::IDD)
{
	//{{AFX_DATA_INIT(CWallyPropertyPage7)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags          ^= PSP_HASHELP;
}

CWallyPropertyPage7::~CWallyPropertyPage7()
{
}

void CWallyPropertyPage7::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyPropertyPage7)
	DDX_Control(pDX, IDC_LIST_WILDCARDS, m_lbWildCards);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyPropertyPage7, CPropertyPage)
	//{{AFX_MSG_MAP(CWallyPropertyPage7)
	ON_LBN_SELCHANGE(IDC_LIST_WILDCARDS, OnSelchangeListWildcards)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ALL, OnButtonCheckAll)
	ON_BN_CLICKED(IDC_BUTTON_UNCHECK_ALL, OnButtonUncheckAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertyPage7 message handlers

BOOL CWallyPropertyPage7::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CImageHelper ihHelper;
	CWildCardItem *pItem = g_WildCardList.GetFirstSupportedImage();
	CWildCardItem *pCheck = NULL;	
	int j = 0;
	int iItemAdded = 0;
	BOOL bSkip = FALSE;

	while (pItem)
	{
		if (
			(ihHelper.IsValidImage ( pItem->GetImageType() ))  || 
			(ihHelper.IsValidPackage (pItem->GetImageType() )) ||
			(ihHelper.IsValidPak (pItem->GetImageType() ))
			)
		{
			// yank out any duplicates
			bSkip = FALSE;
			for (j = 0; j < m_lbWildCards.GetCount(); j++)
			{
				pCheck = (CWildCardItem *)m_lbWildCards.GetItemData(j);
				if (pCheck->GetWildCardExtension() == pItem->GetWildCardExtension())
				{
					j = m_lbWildCards.GetCount();
					bSkip = TRUE;
				}
			}

			if (!bSkip)
			{
				iItemAdded = m_lbWildCards.AddString (pItem->GetDescription());
				m_lbWildCards.SetItemData (iItemAdded, (ULONG)pItem);

				if (pItem->GetAssociatedProgram() == "Wally Document")
				{
					m_lbWildCards.SetCheck (iItemAdded, 1);
				}
			}
		}

		pItem = g_WildCardList.GetNextSupportedImage();
	}

	SetDlgItemText (IDC_CURRENT_ASSOC, "");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWallyPropertyPage7::OnSelchangeListWildcards() 
{
	int iSelection = m_lbWildCards.GetCurSel();

	if (iSelection != LB_ERR)
	{
		CWildCardItem *pItem = (CWildCardItem *)m_lbWildCards.GetItemData (iSelection);

		if (pItem)
		{
			CString strAssociation("");		
			CString strWildCard ("");
			strWildCard.Format (".%s", pItem->GetWildCardExtension());
			strWildCard.MakeUpper();

			if (pItem->GetAssociatedProgram() != "")
			{
				strAssociation.Format ("%s is currently a %s.", strWildCard, pItem->GetAssociatedProgram());				
			}
			else
			{
				strAssociation.Format ("%s is not currently associated with an application.", strWildCard);
			}
			SetDlgItemText (IDC_CURRENT_ASSOC, strAssociation);
		}
	}
}

void CWallyPropertyPage7::OnButtonCheckAll() 
{
	int j = 0;
	
	for (j = 0; j < m_lbWildCards.GetCount(); j++)
	{
		m_lbWildCards.SetCheck (j, 1);		
	}	
}

void CWallyPropertyPage7::OnButtonUncheckAll() 
{
	int j = 0;
	
	for (j = 0; j < m_lbWildCards.GetCount(); j++)
	{
		m_lbWildCards.SetCheck (j, 0);
	}	
}

void CWallyPropertyPage7::OnOK() 
{
	int j = 0;
	CWildCardItem *pItem = NULL;
	
	for (j = 0; j < m_lbWildCards.GetCount(); j++)
	{
		pItem = (CWildCardItem *)m_lbWildCards.GetItemData(j);

		if (pItem)
		{
			if (m_lbWildCards.GetCheck (j) == 1)
			{		
				if (pItem->GetAssociatedProgram() != "Wally Document")
				{
					pItem->AssociateWithWally();
				}
			}
			else
			{
				pItem->RemoveIfAssociatedWithWally();
			}
		}		
	}	
	CPropertyPage::OnOK();
}
/*
----------------------------------------------------------------
Neal - TODO - think of some kind of fix for the following issue:
----------------------------------------------------------------

I'm sorry you're having trouble.  The way Windows normally works
is that a file extension like ".bmp" is "owned" by whichever program
last asked to be the owner.  When you set the checkbox and click OK,
Wally asks for ownership, which is nicer than most Windows programs 
that just take ownership of every kind of file that they can read.

The problem is that there's no real way to "unrequest" ownership.
You can do that with RegEdit, but then when you double-click a bmp
file Windows will just tell you that it's an unrecognized file type.
That's just the way Microsoft designed Windows to work.  Wally could
do the same thing, but you still won't be able to double-click the 
file to run it in another program.  Wally just can't know what 
program should now have ownership.  It's a Windows design problem.

What you need to do is re-install the program you want to "own" bmp
files. That's the Microsoft recommended way.  You can also run 
RegEdit, if you're comfortable with the risk (you can easily trash 
Windows to the point of needing to format your hard disk and 
reinstalling Windows from scratch, if you make a mistake.)  I've 
enclosed a REG file that will set the file association to run 
Windows Paint.  I *CANNOT* guarantee that it will work for you,
because your Windows installation is different from mine and you
may have your paint program in a different directory than I do.
You also might want a different program than Microsoft Paint to
own bmp files.

Double-click the bmp.reg file at your own risk.  It's the best I
can do (sorry).  I wish there were a better way, but Windows is 
actually not a very well designed operating system.

- Neal

>From: "Brandon" <predtech@mdvl.net>
>To: <ty@wwa.com>, <Neal_White_III@HotMail.com>
>Subject: BMP Problems
>Date: Thu, 24 Aug 2000 11:57:52 -0400
>
>Hello when I go to the Options menu and try to take the BMP off the file association and hit ok and when I go back into the file association its still there checked I do not want it checked I tried to uncheck it 10 times and it didn't take it off please help
>
>Thanks for your time Brandon
>predtech@mdvl.net
*/
