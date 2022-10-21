/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// OffsetDlg.cpp : implementation file
//
// Created by Neal White III, 1-3-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Filter.h"
#include "OffsetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg dialog


COffsetDlg::COffsetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COffsetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COffsetDlg)
	m_iOffsetX = 0;
	m_iOffsetY = 0;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void COffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COffsetDlg)
	DDX_Control(pDX, IDC_SPIN_Y_OFFSET, m_ctrlSpinOffsetY);
	DDX_Control(pDX, IDC_SPIN_X_OFFSET, m_ctrlSpinOffsetX);
	DDX_Text(pDX, IDC_EDIT_X_OFFSET, m_iOffsetX);
	DDV_MinMaxInt(pDX, m_iOffsetX, -1024, 1024);
	DDX_Text(pDX, IDC_EDIT_Y_OFFSET, m_iOffsetY);
	DDV_MinMaxInt(pDX, m_iOffsetY, -1024, 1024);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COffsetDlg, CDialog)
	//{{AFX_MSG_MAP(COffsetDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COffsetDlg message handlers

void COffsetDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int iOffsetX = GetDlgItemInt( IDC_EDIT_X_OFFSET);
	int iOffsetY = GetDlgItemInt( IDC_EDIT_Y_OFFSET);

	g_Filter.DoPixelFilter( m_pLayer, FILTER_OFFSET, iOffsetX, iOffsetY);
	m_bPreviewApplied = TRUE;
}

BOOL COffsetDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int iPosX = 0;
	int iPosY = 0;

	ASSERT( m_pLayer);

	if (m_pLayer)
	{
		iPosX = m_pLayer->GetWidth()  / 2;
		iPosY = m_pLayer->GetHeight() / 2;
	}

	m_ctrlSpinOffsetX.SetRange( -1024, 1024);
	m_ctrlSpinOffsetX.SetPos( iPosX);
	m_ctrlSpinOffsetY.SetRange( -1024, 1024);
	m_ctrlSpinOffsetY.SetPos( iPosY);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}
