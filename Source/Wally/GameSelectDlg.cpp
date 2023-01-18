// GameSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "GameSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameSelectDlg dialog


CGameSelectDlg::CGameSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	SetMessageType(GAME_SELECT_MSG_PASTE);
}


void CGameSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameSelectDlg)
	DDX_Control(pDX, IDC_COMBO_FILE_TYPE, m_cbFileType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CGameSelectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameSelectDlg message handlers

BOOL CGameSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT (g_iFileTypeDefault == FILE_TYPE_PROMPT);	// If it's not set to prompt, why are you here?

	int iItemAdded = 0;
	
	iItemAdded = m_cbFileType.AddString ("Quake2 Texture (.wal)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE2);

	iItemAdded = m_cbFileType.AddString ("Quake1 Texture (.mip)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1);

	iItemAdded = m_cbFileType.AddString ("Quake1 Package [wad2] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_QUAKE1_WAD);

	iItemAdded = m_cbFileType.AddString ("SiN Texture (.swl)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_SIN);

	iItemAdded = m_cbFileType.AddString ("Half-Life Texture");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE);

	iItemAdded = m_cbFileType.AddString ("Half-Life Package [wad3] (.wad)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HALF_LIFE_WAD);

	iItemAdded = m_cbFileType.AddString ("Heretic2 Texture (.m8)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_HERETIC2);

	iItemAdded = m_cbFileType.AddString ("Truevision Targa (.tga)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_TGA);

	iItemAdded = m_cbFileType.AddString ("Windows Bitmap (.bmp)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_BMP);

	iItemAdded = m_cbFileType.AddString ("Zsoft PC Paintbrush (.pcx)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PCX);

	iItemAdded = m_cbFileType.AddString ("Portable Network Graphics (.png)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_PNG);

	iItemAdded = m_cbFileType.AddString ("JPEG - JFIF (.jpg)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_JPG);

	iItemAdded = m_cbFileType.AddString ("Serious Sam Texture (.tex)");
	m_cbFileType.SetItemData (iItemAdded, FILE_TYPE_TEX);
	

	// Choose what they might have used recently

	m_cbFileType.SetCurSel(0);
	for (int j = 0; j < m_cbFileType.GetCount(); j++)
	{
		if ((int)m_cbFileType.GetItemData (j) == g_iFileTypeLastUsed)
		{
			m_cbFileType.SetCurSel(j);
		}
	}

	CString strText ("");

	switch (GetMessageType())
	{
	case GAME_SELECT_MSG_PASTE:
		strText = "What type of file would you like to create?";		
		break;

	case GAME_SELECT_MSG_DRAGDROP:
		strText = "You have dragged some non-game images to Wally.  What type of texture(s) would you like to create?";
		break;

	case GAME_SELECT_MSG_OPEN:
		strText = "You have selected some non-game images to open.  What type of texture(s) would you like to create?";
		break;
	
	default:
		ASSERT (false);		// Unhandled message type!
		break;
	}
	SetDlgItemText (IDC_STATIC_MESSAGE, strText);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGameSelectDlg::OnOK() 
{
	int iSelection = m_cbFileType.GetCurSel();

	if (iSelection != CB_ERR)
	{
		m_iFileType = g_iFileTypeLastUsed = m_cbFileType.GetItemData (iSelection);
	}
	
	if (IsDlgButtonChecked( IDC_CHECK_ALWAYS))
	{
		g_iFileTypeDefault = m_iFileType;
	}

	CDialog::OnOK();
}

