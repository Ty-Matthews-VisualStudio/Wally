// ColorTranslatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "ColorTranslatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorTranslatorDlg dialog


CColorTranslatorDlg::CColorTranslatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorTranslatorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorTranslatorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	SetSourceIndex (0);
	SetDestinationIndex (255);
	SetRGB (RGB (0, 0, 255));
	memset (m_byPalette, 0, 768);

	m_iTranslateChoice = COLOR_TRANSLATE_BYINDEX;
	m_bSwapIndexes = false;
}


void CColorTranslatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorTranslatorDlg)
	DDX_Control(pDX, IDC_CHECK_SWAP_INDEXES, m_btnSwapIndexes);
	DDX_Control(pDX, IDC_EDIT_SOURCE_INDEX, m_edSourceIndex);
	DDX_Control(pDX, IDC_EDIT_RED, m_edRed);
	DDX_Control(pDX, IDC_EDIT_GREEN, m_edGreen);
	DDX_Control(pDX, IDC_EDIT_BLUE, m_edBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorTranslatorDlg, CDialog)
	//{{AFX_MSG_MAP(CColorTranslatorDlg)
	ON_BN_CLICKED(IDC_RADIO_INDEX, OnRadioIndex)
	ON_BN_CLICKED(IDC_RADIO_RGB_VALUE, OnRadioRgbValue)
	ON_EN_KILLFOCUS(IDC_EDIT_BLUE, OnKillfocusEditBlue)
	ON_EN_KILLFOCUS(IDC_EDIT_GREEN, OnKillfocusEditGreen)
	ON_EN_KILLFOCUS(IDC_EDIT_RED, OnKillfocusEditRed)
	ON_EN_KILLFOCUS(IDC_EDIT_DEST_INDEX, OnKillfocusEditDestIndex)
	ON_EN_KILLFOCUS(IDC_EDIT_SOURCE_INDEX, OnKillfocusEditSourceIndex)
	ON_BN_CLICKED(IDC_CHECK_SWAP_INDEXES, OnCheckSwapIndexes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorTranslatorDlg message handlers

BOOL CColorTranslatorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_edRed.EnableWindow (false);
	m_edBlue.EnableWindow (false);
	m_edGreen.EnableWindow (false);

	CString strText ("");

	int iIndex = GetDestinationIndex();
	strText.Format ("%d", iIndex);
	SetDlgItemText (IDC_EDIT_DEST_INDEX, strText);

	iIndex = GetSourceIndex();
	strText.Format ("%d", GetSourceIndex());
	SetDlgItemText (IDC_EDIT_SOURCE_INDEX, strText);

	m_iRValue = m_byPalette[iIndex * 3 + 0];
	m_iGValue = m_byPalette[iIndex * 3 + 1];
	m_iBValue = m_byPalette[iIndex * 3 + 2];

	strText.Format ("%d", m_iRValue);
	SetDlgItemText (IDC_EDIT_RED, strText);
	
	strText.Format ("%d", m_iGValue);
	SetDlgItemText (IDC_EDIT_GREEN, strText);

	strText.Format ("%d", m_iBValue);
	SetDlgItemText (IDC_EDIT_BLUE, strText);

	CheckDlgButton (IDC_RADIO_INDEX, 1);
	CheckDlgButton (IDC_RADIO_RGB_VALUE, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorTranslatorDlg::OnRadioIndex() 
{
	m_iTranslateChoice = IsDlgButtonChecked( IDC_RADIO_INDEX) ? COLOR_TRANSLATE_BYINDEX : COLOR_TRANSLATE_BYCOLOR;
	bool bEnableRGB = !IsDlgButtonChecked( IDC_RADIO_INDEX);
	
	m_edRed.EnableWindow (bEnableRGB);
	m_edBlue.EnableWindow (bEnableRGB);
	m_edGreen.EnableWindow (bEnableRGB);
	m_edSourceIndex.EnableWindow (!bEnableRGB);
	m_btnSwapIndexes.EnableWindow (!bEnableRGB);
	
	switch (m_iTranslateChoice)
	{
	case COLOR_TRANSLATE_BYINDEX:
		break;

	case COLOR_TRANSLATE_BYCOLOR:
		break;

	default:
		ASSERT (false);		// Unhandled option!
		break;
	}
}

void CColorTranslatorDlg::OnRadioRgbValue() 
{	
	OnRadioIndex();	
}


void CColorTranslatorDlg::OnKillFocusControls(bool bUsePaletteValues /* = false */)
{
	CString strText ("");
	int iValue = 0;
	int r, g, b;

	GetDlgItemText (IDC_EDIT_SOURCE_INDEX, strText);
	iValue = atol (strText);
	iValue = min (iValue, 255);
	iValue = max (iValue, 0);
	SetSourceIndex (iValue);
	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_SOURCE_INDEX, strText);

	if (bUsePaletteValues)
	{
		r = m_byPalette[iValue * 3 + 0];
		g = m_byPalette[iValue * 3 + 1];
		b = m_byPalette[iValue * 3 + 2];

		strText.Format ("%d", r);
		SetDlgItemText (IDC_EDIT_RED, strText);

		strText.Format ("%d", g);
		SetDlgItemText (IDC_EDIT_GREEN, strText);

		strText.Format ("%d", b);
		SetDlgItemText (IDC_EDIT_BLUE, strText);
	}
	else
	{

		GetDlgItemText (IDC_EDIT_RED, strText);
		iValue = atol (strText);
		iValue = min (iValue, 255);
		iValue = max (iValue, 0);
		m_iRValue = iValue;
		strText.Format ("%d", iValue);
		SetDlgItemText (IDC_EDIT_RED, strText);

		
		GetDlgItemText (IDC_EDIT_GREEN, strText);
		iValue = atol (strText);
		iValue = min (iValue, 255);
		iValue = max (iValue, 0);
		m_iGValue = iValue;
		strText.Format ("%d", iValue);
		SetDlgItemText (IDC_EDIT_GREEN, strText);

		
		GetDlgItemText (IDC_EDIT_BLUE, strText);
		iValue = atol (strText);
		iValue = min (iValue, 255);
		iValue = max (iValue, 0);
		m_iBValue = iValue;
		strText.Format ("%d", iValue);
		SetDlgItemText (IDC_EDIT_BLUE, strText);
	}

	GetDlgItemText (IDC_EDIT_DEST_INDEX, strText);
	iValue = atol (strText);
	iValue = min (iValue, 255);
	iValue = max (iValue, 0);
	SetDestinationIndex (iValue);
	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_DEST_INDEX, strText);

}

void CColorTranslatorDlg::OnKillfocusEditBlue() 
{
	OnKillFocusControls();
}

void CColorTranslatorDlg::OnKillfocusEditGreen() 
{
	OnKillFocusControls();	
}

void CColorTranslatorDlg::OnKillfocusEditRed() 
{
	OnKillFocusControls();
}

void CColorTranslatorDlg::OnKillfocusEditDestIndex() 
{
	OnKillFocusControls();
}

void CColorTranslatorDlg::OnKillfocusEditSourceIndex() 
{
	OnKillFocusControls(true);
}

void CColorTranslatorDlg::OnCheckSwapIndexes() 
{
	m_bSwapIndexes = IsDlgButtonChecked (IDC_CHECK_SWAP_INDEXES);
}
