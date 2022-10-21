/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ResizeDlg.cpp : implementation file
//
// Created by Neal White III, 1-13-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Filter.h"
#include "ResizeDlg.h"
#include "Remip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool bLockOut = false;

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog


CResizeDlg::CResizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResizeDlg)
	m_bProportial = TRUE;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iScaleX = 100;
	m_iScaleY = 100;
	m_iResizeType = 0;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeDlg)
	DDX_Check(pDX, IDC_CHK_PROPORTIAL, m_bProportial);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	DDV_MinMaxInt(pDX, m_iWidth, 1, MAX_MIP_SIZE);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDV_MinMaxInt(pDX, m_iHeight, 1, MAX_MIP_SIZE);
	DDX_Text(pDX, IDC_EDIT_SCALE_X, m_iScaleX);
	DDV_MinMaxInt(pDX, m_iScaleX, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_SCALE_Y, m_iScaleY);
	DDV_MinMaxInt(pDX, m_iScaleY, 1, 10000);
	DDX_Radio(pDX, IDC_RADIO_ANY_SIZE, m_iResizeType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
	//{{AFX_MSG_MAP(CResizeDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	ON_BN_CLICKED(IDC_RADIO_QUAD_SIZE, OnRadioClick)
	ON_BN_CLICKED(IDC_CHK_PROPORTIAL, OnCheckProportial)
	ON_BN_CLICKED(IDC_RADIO_QUAD_WIDTH, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_QUAD_HEIGHT, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_DOUBLE_SIZE, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_DOUBLE_WIDTH, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_DOUBLE_HEIGHT, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_HALF_SIZE, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_HALF_WIDTH, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_HALF_HEIGHT, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_QUARTER_SIZE, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_QUARTER_WIDTH, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_QUARTER_HEIGHT, OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_ANY_SIZE, OnRadioClick)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeEditWidth)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_SCALE_X, OnChangeEditScaleX)
	ON_EN_CHANGE(IDC_EDIT_SCALE_Y, OnChangeEditScaleY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg message handlers

void CResizeDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int	iResizeType = 0;

	if (IsDlgButtonChecked( IDC_RADIO_ANY_SIZE))
		iResizeType = 0;
	else if (IsDlgButtonChecked( IDC_RADIO_QUAD_SIZE))
		iResizeType = 1;
	else if (IsDlgButtonChecked( IDC_RADIO_DOUBLE_SIZE))
		iResizeType = 2;
	else if (IsDlgButtonChecked( IDC_RADIO_HALF_SIZE))
		iResizeType = 3;
	else if (IsDlgButtonChecked( IDC_RADIO_QUARTER_SIZE))
		iResizeType = 4;

	else if (IsDlgButtonChecked( IDC_RADIO_QUAD_WIDTH))
		iResizeType = 5;
	else if (IsDlgButtonChecked( IDC_RADIO_DOUBLE_WIDTH))
		iResizeType = 6;
	else if (IsDlgButtonChecked( IDC_RADIO_HALF_WIDTH))
		iResizeType = 7;
	else if (IsDlgButtonChecked( IDC_RADIO_QUARTER_WIDTH))
		iResizeType = 8;

	else if (IsDlgButtonChecked( IDC_RADIO_QUAD_HEIGHT))
		iResizeType = 9;
	else if (IsDlgButtonChecked( IDC_RADIO_DOUBLE_HEIGHT))
		iResizeType = 10;
	else if (IsDlgButtonChecked( IDC_RADIO_HALF_HEIGHT))
		iResizeType = 11;
	else if (IsDlgButtonChecked( IDC_RADIO_QUARTER_HEIGHT))
		iResizeType = 12;
	else
	{
		ASSERT( FALSE);
	}

	// neal - TODO: implement percent resize

	int iResizeAmount = 200;

	if (m_pLayer)
	{
//		g_Filter.DoResize( m_pLayer, iResizeType, iResizeAmount);

		if (iResizeType == 0)		// any size
		{
			int iWidth  = GetDlgItemInt(IDC_EDIT_WIDTH);
			int iHeight = GetDlgItemInt(IDC_EDIT_HEIGHT);

			g_Filter.DoArbitraryResize( m_pLayer, iWidth, iHeight);
		}
		else
		{
			g_Filter.DoResize( m_pLayer, iResizeType, iResizeAmount);
		}
		m_bPreviewApplied = TRUE;
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CResizeDlg::EnableDlgButton( int iCtrlID, BOOL bEnable)
{
	HWND hWndCtrl = ::GetDlgItem( m_hWnd, iCtrlID);
	::EnableWindow( hWndCtrl, bEnable);
}

BOOL CResizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_pLayer)
	{
		#define MAX_SIZE  MAX_MIP_SIZE
		#define MIN_SIZE  16

		int iWidth  = m_pLayer->GetWidth();
		int iHeight = m_pLayer->GetHeight();

		if (iWidth*4 > MAX_SIZE)
		{
			EnableDlgButton( IDC_RADIO_QUAD_WIDTH,     FALSE);
			EnableDlgButton( IDC_RADIO_QUAD_SIZE,      FALSE);
		}
		if (iHeight*4 > MAX_SIZE)
		{
			EnableDlgButton( IDC_RADIO_QUAD_HEIGHT,    FALSE);
			EnableDlgButton( IDC_RADIO_QUAD_SIZE,      FALSE);
		}
		if (iWidth*2 > MAX_SIZE)
		{
			EnableDlgButton( IDC_RADIO_DOUBLE_WIDTH,   FALSE);
			EnableDlgButton( IDC_RADIO_DOUBLE_SIZE,    FALSE);
		}
		if (iHeight*2 > MAX_SIZE)
		{
			EnableDlgButton( IDC_RADIO_DOUBLE_HEIGHT,  FALSE);
			EnableDlgButton( IDC_RADIO_DOUBLE_SIZE,    FALSE);
		}
		//if ((iWidth/2 < MIN_SIZE) || (!IsValidSize(iWidth/2)))
		if ((iWidth/2 < MIN_SIZE) || (iWidth&1 != 0))
		{
			EnableDlgButton( IDC_RADIO_HALF_WIDTH,     FALSE);
			EnableDlgButton( IDC_RADIO_HALF_SIZE,      FALSE);
		}
		//if ((iHeight/2 < MIN_SIZE) || (!IsValidSize(iHeight/2)))
		if ((iHeight/2 < MIN_SIZE) || (iHeight&1 != 0))
		{
			EnableDlgButton( IDC_RADIO_HALF_HEIGHT,    FALSE);
			EnableDlgButton( IDC_RADIO_HALF_SIZE,      FALSE);
		}
		//if ((iWidth/4 < MIN_SIZE) || (!IsValidSize(iWidth/4)))
		if ((iWidth/4 < MIN_SIZE) || (iWidth&3 != 0))
		{
			EnableDlgButton( IDC_RADIO_QUARTER_WIDTH,  FALSE);
			EnableDlgButton( IDC_RADIO_QUARTER_SIZE,   FALSE);
		}
		//if ((iHeight/4 < MIN_SIZE) || (!IsValidSize(iHeight/4)))
		if ((iHeight/4 < MIN_SIZE) || (iHeight&3 != 0))
		{
			EnableDlgButton( IDC_RADIO_QUARTER_HEIGHT, FALSE);
			EnableDlgButton( IDC_RADIO_QUARTER_SIZE,   FALSE);
		}
	}
	else
	{
		ASSERT( FALSE);

		EnableDlgButton( IDC_RADIO_QUAD_SIZE,      FALSE);
		EnableDlgButton( IDC_RADIO_DOUBLE_SIZE,    FALSE);
		EnableDlgButton( IDC_RADIO_HALF_SIZE,      FALSE);
		EnableDlgButton( IDC_RADIO_QUARTER_SIZE,   FALSE);
		EnableDlgButton( IDC_RADIO_QUAD_WIDTH,     FALSE);
		EnableDlgButton( IDC_RADIO_DOUBLE_WIDTH,   FALSE);
		EnableDlgButton( IDC_RADIO_HALF_WIDTH,     FALSE);
		EnableDlgButton( IDC_RADIO_QUARTER_WIDTH,  FALSE);
		EnableDlgButton( IDC_RADIO_QUAD_HEIGHT,    FALSE);
		EnableDlgButton( IDC_RADIO_DOUBLE_HEIGHT,  FALSE);
		EnableDlgButton( IDC_RADIO_HALF_HEIGHT,    FALSE);
		EnableDlgButton( IDC_RADIO_QUARTER_HEIGHT, FALSE);
	}

	// buttons not enabled until user selects a resize amount

//	EnableDlgButton( IDOK,              FALSE);
//	EnableDlgButton( ID_FILTER_PREVIEW, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CResizeDlg::OnRadioClick() 
{
	EnableDlgButton( IDOK,              TRUE);
	EnableDlgButton( ID_FILTER_PREVIEW, TRUE);

	bool bEnable = IsDlgButtonChecked(IDC_RADIO_ANY_SIZE);

	int iIDs[] =
	{ IDC_CHK_PROPORTIAL, 
		IDC_STATIC_WIDTH,   IDC_EDIT_WIDTH,   IDC_STATIC_PIXELS_W,
		IDC_STATIC_HEIGHT,  IDC_EDIT_HEIGHT,  IDC_STATIC_PIXELS_H,
		IDC_STATIC_SCALE_X, IDC_EDIT_SCALE_X, IDC_STATIC_PERCENT_X,
		IDC_STATIC_SCALE_Y, IDC_EDIT_SCALE_Y, IDC_STATIC_PERCENT_Y
	};

	// Neal - all the sizeof stuff is just to calc the max count of the array
	for (int i = 0; i < (sizeof(iIDs)/sizeof(iIDs[0])); i++)
	{
		CWnd *pCtrl = GetDlgItem(iIDs[i]);
		ASSERT (pCtrl != NULL);

		if (pCtrl)
		{
			pCtrl->EnableWindow( bEnable);
		}
	}
}

void CResizeDlg::OnCheckProportial()
{
	OnChangeEditWidth();
}

void CResizeDlg::RecalcProportionalNumbers(int iCtrlID)
{
	if (! bLockOut)
	{
		int iValue  = GetDlgItemInt(iCtrlID);
		int iWidth  = iValue;
		int iHeight = iValue;
		int iScaleX = iValue;
		int iScaleY = iValue;

		bLockOut = true;
		BOOL bProportional = IsDlgButtonChecked(IDC_CHK_PROPORTIAL);

		if (bProportional)
		{
			switch (iCtrlID)
			{
				case IDC_EDIT_WIDTH:
					iHeight = iWidth * m_pLayer->GetHeight() / m_pLayer->GetWidth();
					iScaleX = 100 * iWidth  / m_pLayer->GetWidth();
					iScaleY = 100 * iHeight / m_pLayer->GetHeight();
					break;

				case IDC_EDIT_HEIGHT:
					iWidth  = iHeight * m_pLayer->GetWidth() / m_pLayer->GetHeight();
					iScaleX = 100 * iWidth  / m_pLayer->GetWidth();
					iScaleY = 100 * iHeight / m_pLayer->GetHeight();
					break;

				case IDC_EDIT_SCALE_X:
				case IDC_EDIT_SCALE_Y:
					iWidth  = iScaleX * m_pLayer->GetWidth() / 100;
					iHeight = iScaleY * m_pLayer->GetHeight() / 100;
					break;

				default:
					ASSERT(FALSE);
			}

			if (iCtrlID != IDC_EDIT_WIDTH)
				SetDlgItemInt(IDC_EDIT_WIDTH, iWidth);
			if (iCtrlID != IDC_EDIT_HEIGHT)
				SetDlgItemInt(IDC_EDIT_HEIGHT, iHeight);
			if (iCtrlID != IDC_EDIT_SCALE_X)
				SetDlgItemInt(IDC_EDIT_SCALE_X, iScaleX);
			if (iCtrlID != IDC_EDIT_SCALE_Y)
				SetDlgItemInt(IDC_EDIT_SCALE_Y, iScaleY);
		}
		else	// non-proportial
		{
			switch (iCtrlID)
			{
				case IDC_EDIT_WIDTH:
					iScaleX = 100 * iWidth  / m_pLayer->GetWidth();
					SetDlgItemInt(IDC_EDIT_SCALE_X, iScaleX);
					break;

				case IDC_EDIT_HEIGHT:
					iScaleY = 100 * iHeight / m_pLayer->GetHeight();
					SetDlgItemInt(IDC_EDIT_SCALE_Y, iScaleY);
					break;

				case IDC_EDIT_SCALE_X:
					iWidth  = iScaleX * m_pLayer->GetWidth() / 100;
					SetDlgItemInt(IDC_EDIT_WIDTH, iWidth);
					break;
				case IDC_EDIT_SCALE_Y:
					iHeight = iScaleY * m_pLayer->GetHeight() / 100;
					SetDlgItemInt(IDC_EDIT_HEIGHT, iHeight);
					break;

				default:
					ASSERT(FALSE);
			}
		}
		bLockOut = false;
	}
}

void CResizeDlg::OnChangeEditWidth()
{
/*
	BOOL bProportional = IsDlgButtonChecked(IDC_CHK_PROPORTIAL);

	if (bProportional && (! bLockOut))
	{
		bLockOut = true;

		int iWidth  = GetDlgItemInt(IDC_EDIT_WIDTH);
		int iHeight = iWidth * m_pLayer->GetHeight() / m_pLayer->GetWidth();
		int iScaleX = 100 * iWidth / m_pLayer->GetWidth();
		int iScaleY = 100 * iHeight / m_pLayer->GetHeight();

		SetDlgItemInt(IDC_EDIT_HEIGHT,  iHeight);
		SetDlgItemInt(IDC_EDIT_SCALE_X, iScaleX);
		SetDlgItemInt(IDC_EDIT_SCALE_Y, iScaleY);

		bLockOut = false;
	}
*/
	RecalcProportionalNumbers(IDC_EDIT_WIDTH);
}

void CResizeDlg::OnChangeEditHeight()
{
	RecalcProportionalNumbers(IDC_EDIT_HEIGHT);
}

void CResizeDlg::OnChangeEditScaleX()
{
	RecalcProportionalNumbers(IDC_EDIT_SCALE_X);
}

void CResizeDlg::OnChangeEditScaleY()
{
	RecalcProportionalNumbers(IDC_EDIT_SCALE_Y);
}
