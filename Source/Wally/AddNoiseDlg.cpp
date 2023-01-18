/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// AddNoiseDlg.cpp : implementation file
//
// Created by Neal White III, 6-27-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "AddNoiseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddNoiseDlg dialog


CAddNoiseDlg::CAddNoiseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddNoiseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddNoiseDlg)
	m_iAddNoiseAmount = -1;
	m_bMonochromeAddNoise = FALSE;
	m_iAddNoiseDistribution = -1;
	m_iAddNoiseType = -1;
	m_iAddNoiseMinLength = 2;
	m_iAddNoiseMaxLength = 20;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CAddNoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddNoiseDlg)
	DDX_Radio(pDX, IDC_RADIO_VERY_LIGHT, m_iAddNoiseAmount);
	DDX_Check(pDX, IDC_CHECK_MONOCHROME, m_bMonochromeAddNoise);
	DDX_Radio(pDX, IDC_RADIO_UNIFORM, m_iAddNoiseDistribution);
	DDX_Radio(pDX, IDC_RADIO_SINGLE_PIXEL, m_iAddNoiseType);
	DDX_Text(pDX, IDC_EDIT_MIN_LENGTH, m_iAddNoiseMinLength);
	DDV_MinMaxInt(pDX, m_iAddNoiseMinLength, 1, 1024);
	DDX_Text(pDX, IDC_EDIT_MAX_LENGTH, m_iAddNoiseMaxLength);
	DDV_MinMaxInt(pDX, m_iAddNoiseMaxLength, 1, 1024);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddNoiseDlg, CDialog)
	//{{AFX_MSG_MAP(CAddNoiseDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddNoiseDlg message handlers

void CAddNoiseDlg::OnFilterPreview() 
{
	int iAmount = GetFilterDlgAmount( this, m_bPreviewApplied);

	int iDistribution = 0;
	if (IsDlgButtonChecked( IDC_RADIO_UNIFORM))
		iDistribution = 0;
	else if (IsDlgButtonChecked( IDC_RADIO_GAUSSIAN))
		iDistribution = 1;
	else
	{
		ASSERT( FALSE);
	}

	int iNoiseType = 0;
	if (IsDlgButtonChecked( IDC_RADIO_SINGLE_PIXEL))
		iNoiseType = 0;
	else if (IsDlgButtonChecked( IDC_RADIO_HORIZONTAL_LINE))
		iNoiseType = 1;
	else if (IsDlgButtonChecked( IDC_RADIO_VERTICAL_LINE))
		iNoiseType = 2;
	else
	{
		ASSERT( FALSE);
	}

	int iNoiseMinLength = GetDlgItemInt( IDC_EDIT_MIN_LENGTH);
	iNoiseMinLength = min( 1024, iNoiseMinLength);
	iNoiseMinLength = max( 1,    iNoiseMinLength);

	int iNoiseMaxLength = GetDlgItemInt( IDC_EDIT_MAX_LENGTH);
	iNoiseMaxLength = min( 1024, iNoiseMaxLength);
	iNoiseMaxLength = max( 1,    iNoiseMaxLength);

	g_Filter.DoAddNoise( m_pLayer, iAmount, iDistribution, 
			iNoiseType, iNoiseMinLength, iNoiseMaxLength,
			IsDlgButtonChecked( IDC_CHECK_MONOCHROME));

	m_bPreviewApplied = TRUE;
}

void CAddNoiseDlg::OnOK() 
{
	if (!UpdateData(TRUE))
	{
		TRACE0("UpdateData failed during dialog termination.\n");
		// the UpdateData routine will set focus to correct item
		return;
	}

	if (m_iAddNoiseMinLength > m_iAddNoiseMaxLength)
	{
		AfxMessageBox( "MinLength must NOT be larger than MaxLength");
		return;
	}

	CDialog::OnOK();
}
