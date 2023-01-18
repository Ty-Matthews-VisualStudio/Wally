/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1999,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// ReduceColorsDlg.cpp : implementation file
//
// Created by Neal White III, 1-29-1999
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Wally.h"
#include "Filter.h"
#include "ReduceColorsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReduceColorsDlg dialog


CReduceColorsDlg::CReduceColorsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReduceColorsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReduceColorsDlg)
	m_iReserveColor1 = 0;
	m_iReserveColor2 = 0;
	m_iReserveColor3 = 0;
	m_iReserveColor4 = 0;
	m_iMaxColors = 0;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;
}


void CReduceColorsDlg::DoDataExchange(CDataExchange* pDX)
{
	CString strText;
	GetDlgItemText( IDC_EDIT_RESERVED1, strText);
	if (strText.IsEmpty())
		SetDlgItemInt( IDC_EDIT_RESERVED1, -1);

	GetDlgItemText( IDC_EDIT_RESERVED2, strText);
	if (strText.IsEmpty())
		SetDlgItemInt( IDC_EDIT_RESERVED2, -1);

	GetDlgItemText( IDC_EDIT_RESERVED3, strText);
	if (strText.IsEmpty())
		SetDlgItemInt( IDC_EDIT_RESERVED3, -1);

	GetDlgItemText( IDC_EDIT_RESERVED4, strText);
	if (strText.IsEmpty())
		SetDlgItemInt( IDC_EDIT_RESERVED4, -1);

	CDialog::DoDataExchange(pDX);

	TRY
	{
			//{{AFX_DATA_MAP(CReduceColorsDlg)
			DDX_Text(pDX, IDC_EDIT_RESERVED1, m_iReserveColor1);
			DDV_MinMaxInt(pDX, m_iReserveColor1, -1, 255);
			DDX_Text(pDX, IDC_EDIT_RESERVED2, m_iReserveColor2);
			DDV_MinMaxInt(pDX, m_iReserveColor2, -1, 255);
			DDX_Text(pDX, IDC_EDIT_RESERVED3, m_iReserveColor3);
			DDV_MinMaxInt(pDX, m_iReserveColor3, -1, 255);
			DDX_Text(pDX, IDC_EDIT_RESERVED4, m_iReserveColor4);
			DDV_MinMaxInt(pDX, m_iReserveColor4, -1, 255);
			DDX_Text(pDX, IDC_EDIT_MAX_COLORS, m_iMaxColors);
			DDV_MinMaxInt(pDX, m_iMaxColors, 1, 255);
			//}}AFX_DATA_MAP
	}
	CATCH_ALL( e)
	{
		if (GetDlgItemInt( IDC_EDIT_RESERVED1) == -1)
			SetDlgItemText( IDC_EDIT_RESERVED1, "");

		if (GetDlgItemInt( IDC_EDIT_RESERVED2) == -1)
			SetDlgItemText( IDC_EDIT_RESERVED2, "");

		if (GetDlgItemInt( IDC_EDIT_RESERVED3) == -1)
			SetDlgItemText( IDC_EDIT_RESERVED3, "");

		if (GetDlgItemInt( IDC_EDIT_RESERVED4) == -1)
			SetDlgItemText( IDC_EDIT_RESERVED4, "");

		THROW_LAST();
	}
	END_CATCH_ALL

	if (GetDlgItemInt( IDC_EDIT_RESERVED1) == -1)
		SetDlgItemText( IDC_EDIT_RESERVED1, "");

	if (GetDlgItemInt( IDC_EDIT_RESERVED2) == -1)
		SetDlgItemText( IDC_EDIT_RESERVED2, "");

	if (GetDlgItemInt( IDC_EDIT_RESERVED3) == -1)
		SetDlgItemText( IDC_EDIT_RESERVED3, "");

	if (GetDlgItemInt( IDC_EDIT_RESERVED4) == -1)
		SetDlgItemText( IDC_EDIT_RESERVED4, "");
}


BEGIN_MESSAGE_MAP(CReduceColorsDlg, CDialog)
	//{{AFX_MSG_MAP(CReduceColorsDlg)
	ON_BN_CLICKED(IDC_BTN_HL_LIQUID, OnBtnHalfLifeLiquid)
	ON_BN_CLICKED(IDC_BTN_HL_TRANSPARENT, OnBtnHalfLifeTransparent)
	ON_BN_CLICKED(IDC_BTN_UNREAL_TRANSPARENT, OnBtnUnrealTransparent)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CReduceColorsDlg::GetReserved( int iReserved[4])
{
	CString strTemp;

	GetDlgItemText( IDC_EDIT_RESERVED1, strTemp);
	if (strTemp.IsEmpty())
		iReserved[0] = -1;
	else
		iReserved[0] = GetDlgItemInt( IDC_EDIT_RESERVED1);

	GetDlgItemText( IDC_EDIT_RESERVED2, strTemp);
	if (strTemp.IsEmpty())
		iReserved[1] = -1;
	else
		iReserved[1] = GetDlgItemInt( IDC_EDIT_RESERVED2);

	GetDlgItemText( IDC_EDIT_RESERVED3, strTemp);
	if (strTemp.IsEmpty())
		iReserved[2] = -1;
	else
		iReserved[2] = GetDlgItemInt( IDC_EDIT_RESERVED3);

	GetDlgItemText( IDC_EDIT_RESERVED4, strTemp);
	if (strTemp.IsEmpty())
		iReserved[3] = -1;
	else
		iReserved[3] = GetDlgItemInt( IDC_EDIT_RESERVED4);
}

/////////////////////////////////////////////////////////////////////////////
// CReduceColorsDlg message handlers

void CReduceColorsDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int     iAmount = GetDlgItemInt( IDC_EDIT_MAX_COLORS);
	int     iReserved[4];

	GetReserved( iReserved);

	g_Filter.DoReduceColors( m_pLayer, iAmount, iReserved);
	m_bPreviewApplied = TRUE;
}

void CReduceColorsDlg::OnBtnHalfLifeLiquid() 
{
	SetDlgItemInt( IDC_EDIT_MAX_COLORS, 256-2);

	SetDlgItemText( IDC_EDIT_RESERVED1, "3");	// fog color	// average color
	SetDlgItemText( IDC_EDIT_RESERVED2, "4");	// fog density	// R value only
	SetDlgItemText( IDC_EDIT_RESERVED3, "");
	SetDlgItemText( IDC_EDIT_RESERVED4, "");

	// Fog color is handled by certain color registers in the texture: 
	// a color register is a palette location.  Color register 0 is the
	// first color of the palette, color register 255 is the last.  Fog 
	// color is handled by color register 3 in a water texture, and the 
	// fog density is handled by color register 4.  The density value is 
	// drawn from the R value of that register (G and B are ignored), so 
	// pure red is the densest fog and black is the least dense fog.
}

void CReduceColorsDlg::OnBtnHalfLifeTransparent() 
{
	SetDlgItemInt( IDC_EDIT_MAX_COLORS, 256-1);

	SetDlgItemText( IDC_EDIT_RESERVED1, "255");
	SetDlgItemText( IDC_EDIT_RESERVED2, "");
	SetDlgItemText( IDC_EDIT_RESERVED3, "");
	SetDlgItemText( IDC_EDIT_RESERVED4, "");
}

void CReduceColorsDlg::OnBtnUnrealTransparent() 
{
	SetDlgItemInt( IDC_EDIT_MAX_COLORS, 256-1);

	SetDlgItemText( IDC_EDIT_RESERVED1, "0");
	SetDlgItemText( IDC_EDIT_RESERVED2, "");
	SetDlgItemText( IDC_EDIT_RESERVED3, "");
	SetDlgItemText( IDC_EDIT_RESERVED4, "");
}

void CReduceColorsDlg::OnOK() 
{
	int     iNumReserved = 4;
	int     iReserved[4];
	CString strTemp;

	GetReserved( iReserved);

	if (((iReserved[0] != -1) && ((iReserved[0] == iReserved[1]) || (iReserved[0] == iReserved[2]) || (iReserved[0] == iReserved[3]))) || 
			((iReserved[1] != -1) && ((iReserved[1] == iReserved[2]) || (iReserved[1] == iReserved[3]))) || 
			((iReserved[2] != -1) && ((iReserved[2] == iReserved[3]))))
	{
		AfxMessageBox( "You cannot have duplicate color palette slots reserved.");
		return;
	}

	GetDlgItemText( IDC_EDIT_RESERVED1, strTemp);
	if (strTemp.IsEmpty() || (strTemp == "-1"))
		iNumReserved--;

	GetDlgItemText( IDC_EDIT_RESERVED2, strTemp);
	if (strTemp.IsEmpty() || (strTemp == "-1"))
		iNumReserved--;

	GetDlgItemText( IDC_EDIT_RESERVED3, strTemp);
	if (strTemp.IsEmpty() || (strTemp == "-1"))
		iNumReserved--;

	GetDlgItemText( IDC_EDIT_RESERVED4, strTemp);
	if (strTemp.IsEmpty() || (strTemp == "-1"))
		iNumReserved--;

	int iMaxColors = GetDlgItemInt( IDC_EDIT_MAX_COLORS);

	if ((iNumReserved > 0) &&( iMaxColors > (256 - iNumReserved)))
	{
		CString strMsg;

		ASSERT( iNumReserved <= 4);

		// neal - i love completely absurd code, don't you?  ;-)

		const char* szNumName[4] = { "a", "a couple of", "three", "four" };

		CString strPleural;
		if (iNumReserved > 1)
			strPleural = "s";

		strMsg.Format( "Max Colors must be %d or less if you want to reserve %s color palette slot%s.", 
				(256 - iNumReserved), szNumName[ iNumReserved-1], strPleural);

		AfxMessageBox( strMsg);
	}
	else
		CDialog::OnOK();
}
