/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// BlendDlg.cpp : implementation file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "BlendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlendDlg dialog


CBlendDlg::CBlendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBlendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlendDlg)
	m_iBlendAmount = 1;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CBlendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlendDlg)
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iBlendAmount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlendDlg, CDialog)
	//{{AFX_MSG_MAP(CBlendDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlendDlg message handlers

int GetFilterDlgAmount(CDialog *pDlg, BOOL bPreviewApplied)
{
	if (bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int iAmount = 1;

	if (pDlg->IsDlgButtonChecked( IDC_RADIO_VERY_LIGHT))
		iAmount = 0;
	else if (pDlg->IsDlgButtonChecked( IDC_RADIO_LIGHT))
		iAmount = 1;
	else if (pDlg->IsDlgButtonChecked( IDC_RADIO_MEDIUM))
		iAmount = 2;
	else if (pDlg->IsDlgButtonChecked( IDC_RADIO_HEAVY))
		iAmount = 3;
	else if (pDlg->IsDlgButtonChecked( IDC_RADIO_VERY_HEAVY))
		iAmount = 4;
	else
	{
		ASSERT( FALSE);
	}
	return iAmount;
}


void CBlendDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);

	g_Filter.DoBlend( m_pLayer, iAmount);
	m_bPreviewApplied = TRUE;
}
