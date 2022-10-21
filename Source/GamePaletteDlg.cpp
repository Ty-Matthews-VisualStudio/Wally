// GamePaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "GamePaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGamePaletteDlg dialog


CGamePaletteDlg::CGamePaletteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGamePaletteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGamePaletteDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGamePaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGamePaletteDlg)
	DDX_Control(pDX, IDC_COMBO_GAME, m_cbGame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGamePaletteDlg, CDialog)
	//{{AFX_MSG_MAP(CGamePaletteDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGamePaletteDlg message handlers

BOOL CGamePaletteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int iItemAdded = 0;
	
	iItemAdded = m_cbGame.AddString ("Duke3d");
	m_cbGame.SetItemData (iItemAdded, IDR_LMP_DUKE3D);

	iItemAdded = m_cbGame.AddString ("Redneck Rampage");
	m_cbGame.SetItemData (iItemAdded, IDR_LMP_REDNECK);

	iItemAdded = m_cbGame.AddString ("Shadow Warrior");
	m_cbGame.SetItemData (iItemAdded, IDR_LMP_SHADOW);

	m_cbGame.SetCurSel (0);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGamePaletteDlg::OnOK() 
{
	int iSelection = m_cbGame.GetCurSel();
	m_iGameSelection = m_cbGame.GetItemData (iSelection);

	CDialog::OnOK();
}

int CGamePaletteDlg::GetGameSelection()
{
	return m_iGameSelection;
}