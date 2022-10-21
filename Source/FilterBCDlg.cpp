/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// FilterBCDlg.cpp : implementation file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "FilterBCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterBrightnessContrastDlg dialog


CFilterBrightnessContrastDlg::CFilterBrightnessContrastDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterBrightnessContrastDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilterBrightnessContrastDlg)
	m_iFilterBrightnessAmount = 0;
	m_iFilterContrastAmount = 0;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CFilterBrightnessContrastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilterBrightnessContrastDlg)
	DDX_Control(pDX, IDC_SPIN_CONTRAST, m_ctrlSpinContrast);
	DDX_Control(pDX, IDC_SPIN_BRIGHTNESS, m_ctrlSpinBrightness);
	DDX_Text(pDX, IDC_EDIT_BRIGHTNESS, m_iFilterBrightnessAmount);
	DDV_MinMaxInt(pDX, m_iFilterBrightnessAmount, -255, 255);
	DDX_Text(pDX, IDC_EDIT_CONTRAST, m_iFilterContrastAmount);
	DDV_MinMaxInt(pDX, m_iFilterContrastAmount, -100, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilterBrightnessContrastDlg, CDialog)
	//{{AFX_MSG_MAP(CFilterBrightnessContrastDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterBrightnessContrastDlg message handlers

void CFilterBrightnessContrastDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int iBrightenAmount = GetDlgItemInt( IDC_EDIT_BRIGHTNESS);
	int iContrastAmount = GetDlgItemInt( IDC_EDIT_CONTRAST);

	g_Filter.DoBrightnessContrast( m_pLayer, iBrightenAmount, iContrastAmount);
	m_bPreviewApplied = TRUE;
}

BOOL CFilterBrightnessContrastDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlSpinContrast.SetRange( -100, 100);
	m_ctrlSpinContrast.SetPos( 0);
	m_ctrlSpinBrightness.SetRange( -255, 255);
	m_ctrlSpinBrightness.SetPos( 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}
