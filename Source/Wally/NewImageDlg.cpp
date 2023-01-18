// NewImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "NewImageDlg.h"
#include "ReMip.h"
#include "HalfLifePaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewImageDlg dialog


CNewImageDlg::CNewImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewImageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewImageDlg)
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edHeight);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edName);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spnWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spnHeight);
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_cbFileType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewImageDlg, CDialog)
	//{{AFX_MSG_MAP(CNewImageDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	ON_CBN_SELCHANGE(IDC_COMBO_FILE_TYPE, OnSelchangeComboGameType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewImageDlg message handlers

BOOL CNewImageDlg::OnInitDialog() 
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
	m_spnWidth.SetAccel (4,&Spinaccel[0]);				
	m_spnHeight.SetAccel (4,&Spinaccel[0]);				

	m_spnWidth.SetRange (16, MAX_TEXTURE_WIDTH);
	m_spnHeight.SetRange (16, MAX_TEXTURE_HEIGHT);

	int iItemAdded = 0;	

	iItemAdded = m_cbFileType.AddString ("Half-Life Package [wad3] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE_WAD);

	iItemAdded = m_cbFileType.AddString ("Half-Life Texture");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE);

	iItemAdded = m_cbFileType.AddString ("Heretic2 Texture (.m8)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HERETIC2);

	iItemAdded = m_cbFileType.AddString ("JPEG - JFIF (.jpg)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_JPG);

	iItemAdded = m_cbFileType.AddString ("PAK Package (.pak)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PAK );

	iItemAdded = m_cbFileType.AddString ("Portable Network Graphics (.png)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PNG);

	iItemAdded = m_cbFileType.AddString ("Quake1 Texture (.mip)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1);

	iItemAdded = m_cbFileType.AddString ("Quake1 Package [wad2] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1_WAD);

	iItemAdded = m_cbFileType.AddString ("Quake2 Texture (.wal)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE2);

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

	
	m_cbFileType.SetCurSel(0);
	for (int j = 0; j < m_cbFileType.GetCount(); j++)
	{
		if ((int)m_cbFileType.GetItemData (j) == g_iFileTypeLastUsed)
		{
			m_cbFileType.SetCurSel(j);
		}
	}	
	
	bool bIsPackage = ((g_iFileTypeLastUsed == FILE_TYPE_HALF_LIFE_WAD) || (g_iFileTypeLastUsed == FILE_TYPE_QUAKE1_WAD) || (g_iFileTypeLastUsed == FILE_TYPE_PAK)) ? true : false;

	m_edWidth.EnableWindow (!bIsPackage);
	m_spnWidth.EnableWindow (!bIsPackage);
	m_edHeight.EnableWindow (!bIsPackage);
	m_spnHeight.EnableWindow (!bIsPackage);
	m_edName.EnableWindow (!bIsPackage);

	if (g_bRetainTextureSize)
	{
		CString strText ("");
		strText.Format ("%d", g_iPreviousTextureWidth);
		SetDlgItemText (IDC_EDIT_WIDTH, strText);

		strText.Format ("%d", g_iPreviousTextureHeight);
		SetDlgItemText (IDC_EDIT_HEIGHT, strText);	
	}
	else
	{
		CString strText ("");
		strText.Format ("%d", g_iDefaultTextureWidth);
		SetDlgItemText (IDC_EDIT_WIDTH, strText);

		strText.Format ("%d", g_iDefaultTextureHeight);
		SetDlgItemText (IDC_EDIT_HEIGHT, strText);	
	}

	SetDlgItemText (IDC_EDIT_NAME, m_strTextureName);

	m_edName.SetSel (0, -1, true);
	m_edName.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewImageDlg::OnOK() 
{
	GetDlgItemText (IDC_EDIT_NAME, m_strTextureName);
	int iCurSel = m_cbFileType.GetCurSel();
	
	if (iCurSel != CB_ERR)
	{
		m_iFileType = m_cbFileType.GetItemData(iCurSel);

		// Set some globals
		g_iFileTypeLastUsed = m_iFileType;
		
		CHalfLifePaletteDlg dlgPalettePicker;

		switch (m_iFileType)
		{	
		case FILE_TYPE_QUAKE2:			
			g_iDocColorDepth = 8;
			break;

		case FILE_TYPE_QUAKE1:			
			g_iDocColorDepth = 8;
			break;

		case FILE_TYPE_QUAKE1_WAD:			
			break;

		case FILE_TYPE_SIN:
			dlgPalettePicker.DoModal();
			memcpy (m_byPalette, dlgPalettePicker.GetPalette(), 768);
			g_iDocColorDepth = 8;
			break;

		case FILE_TYPE_HERETIC2:
			dlgPalettePicker.DoModal();
			memcpy (m_byPalette, dlgPalettePicker.GetPalette(), 768);
			g_iDocColorDepth = 8;
			break;

		case FILE_TYPE_HALF_LIFE:
			dlgPalettePicker.DoModal();
			memcpy (m_byPalette, dlgPalettePicker.GetPalette(), 768);
			g_iDocColorDepth = 8;
			break;

		case FILE_TYPE_HALF_LIFE_WAD:
			// TODO:  Any game-specific init here
			break;

		case FILE_TYPE_PAK:
			// TODO:  Any game-specific init here
			break;

		case FILE_TYPE_TGA:	
		case FILE_TYPE_PCX:		
		case FILE_TYPE_BMP:			
		case FILE_TYPE_PNG:			
		case FILE_TYPE_JPG:
		case FILE_TYPE_TEX:
			g_iDocColorDepth = 24;
			break;

		default:
			ASSERT(false);		// Missed implementation?
			break;
		}
	}
	
	OnKillfocusEditHeight();
	OnKillfocusEditWidth();

	CString strText ("");
	GetDlgItemText (IDC_EDIT_HEIGHT, strText);
	g_iPreviousTextureHeight  = atol (strText);

	GetDlgItemText (IDC_EDIT_WIDTH, strText);
	g_iPreviousTextureWidth  = atol (strText);
		
	CDialog::OnOK();
}

void CNewImageDlg::OnKillfocusEditHeight() 
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

void CNewImageDlg::OnKillfocusEditWidth() 
{	
	CString strText ("");
	int iWidth = 0;

	GetDlgItemText (IDC_EDIT_WIDTH, strText);
	iWidth = atol (strText);

	iWidth = max (iWidth, 1);
	iWidth = min (iWidth, MAX_TEXTURE_WIDTH);

	//g_iPreviousTextureWidth  = iWidth;
	
	strText.Format ("%d", iWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);
}

void CNewImageDlg::SetTextureName (LPCTSTR szName)
{
	m_strTextureName = szName;
}

CString CNewImageDlg::GetTextureName ()
{
	return m_strTextureName;
}

void CNewImageDlg::OnSelchangeComboGameType() 
{
	int iCurSel = m_cbFileType.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		int iFileType = m_cbFileType.GetItemData (iCurSel);
		
		BOOL bIsPackage = ((iFileType == FILE_TYPE_HALF_LIFE_WAD) || (iFileType == FILE_TYPE_QUAKE1_WAD) || (iFileType == FILE_TYPE_PAK)) ? true : false;

		m_edWidth.EnableWindow (!bIsPackage);	
		m_spnWidth.EnableWindow (!bIsPackage);
		m_edHeight.EnableWindow (!bIsPackage);	
		m_spnHeight.EnableWindow (!bIsPackage);
		m_edName.EnableWindow (!bIsPackage);
	}
}
