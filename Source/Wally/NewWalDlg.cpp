/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// NewWalDlg.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "NewWalDlg.h"
#include "ReMip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewWalDlg dialog


CNewWalDlg::CNewWalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewWalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewWalDlg)
	m_strNewName = _T("");	
	//}}AFX_DATA_INIT

	if (g_bRetainTextureSize)
	{
		m_uWidth  = g_iPreviousTextureWidth;
		m_uHeight = g_iPreviousTextureHeight;
	}
	else
	{
		m_uWidth  = g_iDefaultTextureWidth;
		m_uHeight = g_iDefaultTextureHeight;
	}
	


}


void CNewWalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWalDlg)
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_ctrlSpinWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_ctrlSpinHeight);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strNewName);
	DDV_MaxChars(pDX, m_strNewName, 31);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_uWidth);
	DDV_MinMaxUInt(pDX, m_uWidth, 1, 2048);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_uHeight);
	DDV_MinMaxUInt(pDX, m_uHeight, 1, 2048);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWalDlg, CDialog)
	//{{AFX_MSG_MAP(CNewWalDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWalDlg message handlers


BOOL CNewWalDlg::OnInitDialog() 
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
	m_ctrlSpinWidth.SetAccel (4,&Spinaccel[0]);				
	m_ctrlSpinHeight.SetAccel (4,&Spinaccel[0]);				

	m_ctrlSpinWidth.SetRange (16, MAX_MIP_SIZE);
	m_ctrlSpinHeight.SetRange (16, MAX_MIP_SIZE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CNewWalDlg::OnOK() 
{
	// Call UpdateData, because sometimes the m_Width and m_Height values are wrong
	UpdateData(TRUE);
	g_iPreviousTextureWidth  = m_uWidth;
	g_iPreviousTextureHeight = m_uHeight;		
	CDialog::OnOK();
}
