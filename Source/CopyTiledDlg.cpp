/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// CopyTiledDlg.cpp : implementation file
//
// Created by Ty Matthews, 6-23-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "CopyTiledDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyTiledDlg dialog


CCopyTiledDlg::CCopyTiledDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyTiledDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyTiledDlg)
	m_iHorizontalTiles = 0;
	m_iVerticalTiles = 0;
	//}}AFX_DATA_INIT
}


void CCopyTiledDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyTiledDlg)
	DDX_Control(pDX, IDC_SPIN_VERTICAL, m_ctrlSpinVertical);
	DDX_Control(pDX, IDC_SPIN_HORIZONTAL, m_ctrlSpinHorizontal);
	DDX_Text(pDX, IDC_EDIT_VERTICAL, m_iVerticalTiles);
	DDV_MinMaxInt(pDX, m_iVerticalTiles, 1, 16);
	DDX_Text(pDX, IDC_EDIT_HORIZONTAL, m_iHorizontalTiles);
	DDV_MinMaxInt(pDX, m_iHorizontalTiles, 1, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyTiledDlg, CDialog)
	//{{AFX_MSG_MAP(CCopyTiledDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopyTiledDlg message handlers

BOOL CCopyTiledDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlSpinHorizontal.SetRange( 1, 16);
	m_ctrlSpinVertical.SetRange( 1, 16);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}
