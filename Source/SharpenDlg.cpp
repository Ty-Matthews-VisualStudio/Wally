/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// SharpenDlg.cpp : implementation file
//
// Created by Neal White III, 1-10-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "SharpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharpenDlg dialog


CSharpenDlg::CSharpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSharpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSharpenDlg)
	m_iSharpenAmount = -1;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CSharpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharpenDlg)
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iSharpenAmount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharpenDlg, CDialog)
	//{{AFX_MSG_MAP(CSharpenDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharpenDlg message handlers

void CSharpenDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);

	g_Filter.DoSharpen( m_pLayer, iAmount);
	m_bPreviewApplied = TRUE;
}
