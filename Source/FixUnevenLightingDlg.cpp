/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                           © Copyright 1998-2001,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// FixUnevenLightingDlg.cpp : implementation file
//
// Created by Neal White III, 9-23-2001
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "Filter.h"
#include "FixUnevenLightingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixUnevenLightingDlg dialog


CFixUnevenLightingDlg::CFixUnevenLightingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFixUnevenLightingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixUnevenLightingDlg)
	m_iFixUnevenLightingAmount = -1;
	//}}AFX_DATA_INIT
}


void CFixUnevenLightingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixUnevenLightingDlg)
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iFixUnevenLightingAmount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixUnevenLightingDlg, CDialog)
	//{{AFX_MSG_MAP(CFixUnevenLightingDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixUnevenLightingDlg message handlers

void CFixUnevenLightingDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);

	g_Filter.DoFixUnevenLighting( m_pLayer, iAmount);
	m_bPreviewApplied = TRUE;
}
