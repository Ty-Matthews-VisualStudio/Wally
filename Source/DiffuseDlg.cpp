/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// DiffuseDlg.cpp : implementation file
//
// Created by Neal White III, 11-12-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "DiffuseDlg.h"
#include "ReMip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiffuseDlg dialog


CDiffuseDlg::CDiffuseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiffuseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiffuseDlg)
	m_iDiffuseAmount = -1;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CDiffuseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiffuseDlg)
	DDX_Radio(pDX, IDC_RADIO_LIGHT, m_iDiffuseAmount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiffuseDlg, CDialog)
	//{{AFX_MSG_MAP(CDiffuseDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiffuseDlg message handlers

void CDiffuseDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	// we want a fresh sequence each time
	// (can't put it in actual filter or final won't match preview)

	m_lRandomSeed = GetTickCount();
	InitRandom( m_lRandomSeed);

	int iAmount = 1;
	if (IsDlgButtonChecked( IDC_RADIO_LIGHT))
		iAmount = 0;
	else if (IsDlgButtonChecked( IDC_RADIO_MEDIUM))
		iAmount = 1;
	else if (IsDlgButtonChecked( IDC_RADIO_HEAVY))
		iAmount = 2;
	else if (IsDlgButtonChecked( IDC_RADIO_VERY_HEAVY))
		iAmount = 3;
	else
	{
		ASSERT( FALSE);
	}

	g_Filter.DoPixelFilter( m_pLayer, FILTER_DIFFUSE, iAmount);
	m_bPreviewApplied = TRUE;
}

BOOL CDiffuseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// we want a fresh sequence each time
	// (can't put it in actual filter or final won't match preview)

	m_lRandomSeed = GetTickCount();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDiffuseDlg::OnOK() 
{
	// we want a fresh sequence each time
	// (can't put it in actual filter or final won't match preview)

	InitRandom( m_lRandomSeed);
	
	CDialog::OnOK();
}
