/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// WallyOptions.cpp : implementation file
//
// Created by Ty Matthews, 3-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWallyOptions dialog


CWallyOptions::CWallyOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CWallyOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWallyOptions)
	m_ZoomValue = g_iDefaultZoomValue;
	m_Width = g_iDefaultTextureWidth;
	m_Height = g_iDefaultTextureHeight;
	m_PaletteConversion = g_iPaletteConversion;
	m_RetainSizeCheck = (g_bRetainTextureSize);
	//}}AFX_DATA_INIT
}


void CWallyOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWallyOptions)
	DDX_Control(pDX, IDC_STATIC_HEIGHT, m_HeightString);
	DDX_Control(pDX, IDC_STATIC_WIDTH, m_WidthString);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_SpinHeight);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_SpinWidth);
	DDX_Control(pDX, IDC_SPIN_ZOOMVALUE, m_SpinZoomControl);
	DDX_Text(pDX, IDC_EDIT_ZOOMVALUE, m_ZoomValue);
	DDV_MinMaxInt(pDX, m_ZoomValue, 1, 10);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_Width);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_Height);
	DDX_Radio(pDX, IDC_RADIO_PALETTE_MAINTAIN, m_PaletteConversion);
	DDX_Check(pDX, IDC_CHECK_RETAIN_SIZE, m_RetainSizeCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWallyOptions, CDialog)
	//{{AFX_MSG_MAP(CWallyOptions)
	ON_EN_KILLFOCUS(IDC_EDIT_ZOOMVALUE, OnKillfocusEditZoomvalue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWallyOptions message handlers

BOOL CWallyOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UDACCEL Spinaccel[4];

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 1;	
	m_SpinZoomControl.SetAccel (1,&Spinaccel[0]);		
	m_SpinZoomControl.SetRange (1,10);


	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 16;
	Spinaccel[1].nSec = 1;
	Spinaccel[1].nInc = 32;
	Spinaccel[2].nSec = 2;
	Spinaccel[2].nInc = 64;
	Spinaccel[2].nSec = 3;
	Spinaccel[2].nInc = 128;
	m_SpinWidth.SetAccel (4,&Spinaccel[0]);
	m_SpinHeight.SetAccel (4,&Spinaccel[0]);

	m_SpinWidth.SetRange (16,2048);
	m_SpinHeight.SetRange (16,2048);

		
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWallyOptions::OnKillfocusEditZoomvalue() 
{
	UpdateData(true);
	// TODO: Add your control notification handler code here
	
}

void CWallyOptions::OnOK() 
{
	// nw - done in CWallyPropertyPage1::OnOK()
/*
	g_bRetainTextureSize	= IsDlgButtonChecked(IDC_CHECK_RETAIN_SIZE);


	if (IsDlgButtonChecked( IDC_RADIO_PALETTE_MAINTAIN))
	{
		g_iPaletteConversion = PALETTE_CONVERT_MAINTAIN;
	}
	else 
	{
		if (IsDlgButtonChecked( IDC_RADIO_PALETTE_NEAREST))
			g_iPaletteConversion = PALETTE_CONVERT_NEAREST;
	}
	
	
	DefaultZoomValue     = m_ZoomValue;
	DefaultTextureWidth  = m_Width;
	DefaultTextureHeight = m_Height;
*/

	CDialog::OnOK();
}

