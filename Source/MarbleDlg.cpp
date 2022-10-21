// MarbleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Filter.h"
#include "MarbleDlg.h"
#include "Remip.h"
#include "PaletteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMarbleDlg dialog


CMarbleDlg::CMarbleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMarbleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarbleDlg)
	m_bMarbleExtraSmooth = TRUE;
	m_bMarbleSeamlessTexture = TRUE;
	m_iMarbleColorVariance = 0;
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_pFilter         = NULL;
	m_pTileParams     = NULL;
	m_bPreviewApplied = FALSE;

	m_iMarbleSeed            = 111;
	m_iMarbleLevels          = 7;
	m_iMarbleNumThickStripes = 1;
	m_iMarbleNumThinStripes  = 5;

	m_dfMarbleAmplitude      = 1.5;

	m_vMarbleScale[0]        = 1.0;
	m_vMarbleScale[1]        = 1.0;
	m_vMarbleScale[2]        = 1.0;

	m_vMarbleOffset[0]       = 0.0;
	m_vMarbleOffset[1]       = 0.0;
	m_vMarbleOffset[2]       = 0.0;

	m_iShape                 = TILE_SEAMLESS;
}


void CMarbleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarbleDlg)
	DDX_Control(pDX, IDC_SPIN_SCALE_Z, m_ctrlSpinScaleZ);
	DDX_Control(pDX, IDC_SPIN_SCALE_Y, m_ctrlSpinScaleY);
	DDX_Control(pDX, IDC_SPIN_SCALE_X, m_ctrlSpinScaleX);
	DDX_Control(pDX, IDC_SPIN_OFFSET_Z, m_ctrlSpinOffsetZ);
	DDX_Control(pDX, IDC_SPIN_OFFSET_Y, m_ctrlSpinOffsetY);
	DDX_Control(pDX, IDC_SPIN_OFFSET_X, m_ctrlSpinOffsetX);
	DDX_Control(pDX, IDC_SPIN_NUM_THIN_STRIPES, m_ctrlSpinNumThinStripes);
	DDX_Control(pDX, IDC_SPIN_LEVELS, m_ctrlSpinLevels);
	DDX_Control(pDX, IDC_SPIN_COLOR_VARIANCE, m_ctrlSpinColorVariance);
	DDX_Control(pDX, IDC_SPIN_AMPLITUDE, m_ctrlSpinAmplitude);
	DDX_Control(pDX, IDC_SPIN_NUM_THICK_STRIPES, m_ctrlSpinNumThickStripes);
	DDX_Check(pDX, IDC_CHECK_EXTRA_SMOOTH, m_bMarbleExtraSmooth);
	DDX_Check(pDX, IDC_CHECK_SEAMLESS, m_bMarbleSeamlessTexture);
	DDX_Text(pDX, IDC_EDIT_COLOR_VARIANCE, m_iMarbleColorVariance);
	DDV_MinMaxUInt(pDX, m_iMarbleColorVariance, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarbleDlg, CDialog)
	//{{AFX_MSG_MAP(CMarbleDlg)
	ON_BN_CLICKED(IDC_BTN_JUST_DO_IT, OnBtnJustDoIt)
	ON_BN_CLICKED(IDC_BTN_RANDOM_SEED, OnBtnRandomSeed)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	ON_BN_CLICKED(ID_FILTER_FAST_PREVIEW, OnFilterFastPreview)
	ON_BN_CLICKED(IDC_BTN_BACKGROUND_COLOR, OnBtnBackgroundColor)
	ON_BN_CLICKED(IDC_BTN_THICK_STRIPE_COLOR, OnBtnThickStripeColor)
	ON_BN_CLICKED(IDC_BTN_THIN_STRIPE_COLOR, OnBtnThinStripeColor)
	ON_WM_DRAWITEM()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_AMPLITUDE, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_X, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_Y, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_Z, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_X, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_Y, OnDeltaPos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SCALE_Z, OnDeltaPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarbleDlg message handlers

BOOL CMarbleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_strDlgTitle.GetLength() > 0)
		SetWindowText( m_strDlgTitle);

	UDACCEL Spinaccel[4];

	Spinaccel[0].nSec = 0;
	Spinaccel[0].nInc = 1;
	Spinaccel[1].nSec = 1;
	Spinaccel[1].nInc = 10;
	Spinaccel[2].nSec = 2;
	Spinaccel[2].nInc = 100;

	//OnBtnRandomSeed();
	SetDlgItemInt( IDC_EDIT_RANDOM_SEED,       m_iMarbleSeed);
	SetDlgItemInt( IDC_EDIT_LEVELS,            m_iMarbleLevels);
	SetDlgItemInt( IDC_EDIT_NUM_THICK_STRIPES, m_iMarbleNumThickStripes);
	SetDlgItemInt( IDC_EDIT_NUM_THIN_STRIPES,  m_iMarbleNumThinStripes);

	char szTemp[20];

	sprintf( szTemp, "%.2f", m_dfMarbleAmplitude);
	SetDlgItemText( IDC_EDIT_AMPLITUDE, szTemp);

	sprintf( szTemp, "%.0f.00", m_vMarbleScale[0]);
	SetDlgItemText( IDC_EDIT_SCALE_X, szTemp);
	sprintf( szTemp, "%.0f.00", m_vMarbleScale[1]);
	SetDlgItemText( IDC_EDIT_SCALE_Y, szTemp);
	sprintf( szTemp, "%.0f.00", m_vMarbleScale[2]);
	SetDlgItemText( IDC_EDIT_SCALE_Z, szTemp);

	sprintf( szTemp, "%.0f.00", m_vMarbleOffset[0]);
	SetDlgItemText( IDC_EDIT_OFFSET_X, szTemp);
	sprintf( szTemp, "%.0f.00", m_vMarbleOffset[1]);
	SetDlgItemText( IDC_EDIT_OFFSET_Y, szTemp);
	sprintf( szTemp, "%.0f.00", m_vMarbleOffset[2]);
	SetDlgItemText( IDC_EDIT_OFFSET_Z, szTemp);

	m_ctrlSpinLevels.SetRange( 1, 10);
	m_ctrlSpinNumThickStripes.SetRange( 0, 10);
	m_ctrlSpinNumThinStripes.SetRange( 0, 20);

	m_ctrlSpinAmplitude.SetRange( 1, 100);
	m_ctrlSpinColorVariance.SetAccel( 2, Spinaccel);
	m_ctrlSpinColorVariance.SetRange( 0, 255);

	m_ctrlSpinScaleY.SetRange( 1, 100);
	m_ctrlSpinScaleX.SetRange( 1, 100);
	m_ctrlSpinScaleZ.SetRange( 1, 100);

	m_ctrlSpinOffsetX.SetAccel( 3, Spinaccel);
	m_ctrlSpinOffsetX.SetRange( -10000, 10000);
//	m_ctrlSpinOffsetX.SetPos( (int )dfValue);

	m_ctrlSpinOffsetY.SetAccel( 3, Spinaccel);
	m_ctrlSpinOffsetY.SetRange( -10000, 10000);

	m_ctrlSpinOffsetZ.SetAccel( 3, Spinaccel);
	m_ctrlSpinOffsetZ.SetRange( -10000, 10000);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMarbleDlg::OnBtnJustDoIt()
{
	OnBtnRandomSeed();

	SetDlgItemInt( IDC_EDIT_LEVELS,            (int )(Random() * 7) + 2);
	SetDlgItemInt( IDC_EDIT_NUM_THICK_STRIPES, (int )(Random() * 3) + 1);
	SetDlgItemInt( IDC_EDIT_NUM_THIN_STRIPES,  (int )(Random() * 5) + 2);
	SetDlgItemInt( IDC_EDIT_COLOR_VARIANCE,    (int )(Random() * 64));

	char szTemp[20];

	sprintf( szTemp, "%.2f", Random() * 5.0 + 1.5);
	SetDlgItemText( IDC_EDIT_AMPLITUDE, szTemp);

	sprintf( szTemp, "%.0f.00", Random() * 4.0 + 1.0);
	SetDlgItemText( IDC_EDIT_SCALE_X, szTemp);
	SetDlgItemText( IDC_EDIT_SCALE_Y, szTemp);
	SetDlgItemText( IDC_EDIT_SCALE_Z, szTemp);

	sprintf( szTemp, "%.0f.00", Random() * 1001.0);
	SetDlgItemText( IDC_EDIT_OFFSET_X, szTemp);
	sprintf( szTemp, "%.0f.00", Random() * 1001.0);
	SetDlgItemText( IDC_EDIT_OFFSET_Y, szTemp);
	sprintf( szTemp, "%.0f.00", Random() * 1001.0);
	SetDlgItemText( IDC_EDIT_OFFSET_Z, szTemp);

	UpdateWindow();
	//OnFilterPreview();
	OnFilterFastPreview();
}

void CMarbleDlg::OnBtnRandomSeed()
{
	InitRandom( GetTickCount());

	double dIgnore = Random();		// just to prime the generator

	SetDlgItemInt( IDC_EDIT_RANDOM_SEED, GetRandomSeed() % 100000);
}

void CMarbleDlg::DoFilterPreview( BOOL bFastPreview)
{
	ASSERT( (m_pLayer != NULL) && (m_pFilter != NULL));

	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	MarbleParams Params;

	Params.pLayer           = m_pLayer;
	Params.iRandSeed        = GetDlgItemInt( IDC_EDIT_RANDOM_SEED);
	Params.iLevels          = GetDlgItemInt( IDC_EDIT_LEVELS);
	Params.iNumThickStripes = GetDlgItemInt( IDC_EDIT_NUM_THICK_STRIPES);
	Params.iNumThinStripes  = GetDlgItemInt( IDC_EDIT_NUM_THIN_STRIPES);
	Params.iColorVariance   = GetDlgItemInt( IDC_EDIT_COLOR_VARIANCE);

	Params.ptOffset = CPoint( 0, 0);
	Params.sizeTile = CSize( 0, 0);
	//Params.iShape = TILE_SQUARE;
	Params.iShape   = m_iShape;

	Params.iStyle   = PLAIN_ALIGNED;
	Params.bMirrorX = FALSE;
	Params.bMirrorY = FALSE;

	Params.bSeamless    = IsDlgButtonChecked( IDC_CHECK_SEAMLESS);
	Params.bExtraSmooth = IsDlgButtonChecked( IDC_CHECK_EXTRA_SMOOTH);

	CString strTemp; 
	GetDlgItemText( IDC_EDIT_AMPLITUDE, strTemp);

	Params.dfAmplitude = atof( strTemp);

	Params.vScale[0] = 1.0;
	Params.vScale[1] = 1.0;
	Params.vScale[2] = 1.0;

	GetDlgItemText( IDC_EDIT_SCALE_X, strTemp);
	Params.vScale[0] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_SCALE_Y, strTemp);
	Params.vScale[1] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_SCALE_Z, strTemp);
	Params.vScale[2] = atof( strTemp);

	Params.vOffset[0] = 0.0;
	Params.vOffset[1] = 0.0;
	Params.vOffset[2] = 0.0;

	GetDlgItemText( IDC_EDIT_OFFSET_X, strTemp);
	Params.vOffset[0] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_OFFSET_Y, strTemp);
	Params.vOffset[1] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_OFFSET_Z, strTemp);
	Params.vOffset[2] = atof( strTemp);

	Params.iARGB_PrimaryField  = NULL;
	Params.iARGB_ContrastField = NULL;
	Params.iARGB_AccentField   = NULL;
	Params.pbyBlockStencilData = NULL;

	if (m_pTileParams)
	{
		Params.iTileRandSeed  = m_pTileParams->iTileRandSeed;
		Params.iTileSize      = m_pTileParams->iTileSize;
		Params.iShape         = m_pTileParams->iShape;
		Params.iTileColor     = m_pTileParams->iTileColor;
		Params.iTileStyle     = m_pTileParams->iTileStyle;
		Params.iTileGroutType = m_pTileParams->iTileGroutType;
		Params.iTileBevelType = m_pTileParams->iTileBevelType;
	}
	else
	{
		Params.iTileRandSeed  = m_pFilter->m_iMarbleTileSeed;
		Params.iTileSize      = m_pFilter->m_iMarbleTileSize;
		//Params.iShape         = m_pFilter->m_iMarbleTileShape;
		Params.iTileColor     = m_pFilter->m_iMarbleTileColor;
		Params.iTileStyle     = m_pFilter->m_iMarbleTileStyle;
		Params.iTileGroutType = m_pFilter->m_iMarbleTileGroutType;
		Params.iTileBevelType = m_pFilter->m_iMarbleTileBevelType;
	}

	Params.bFastPreview = bFastPreview;

	g_Filter.DoMarbleTex( &Params);

	m_bPreviewApplied = TRUE;
}

void CMarbleDlg::OnFilterFastPreview() 
{
	DoFilterPreview( TRUE);
}

void CMarbleDlg::OnFilterPreview()
{
	DoFilterPreview( FALSE);
}

void CMarbleDlg::OnOK() 
{
	// TODO: Add extra validation here

	m_iMarbleSeed            = GetDlgItemInt( IDC_EDIT_RANDOM_SEED);
	m_iMarbleLevels          = GetDlgItemInt( IDC_EDIT_LEVELS);
	m_iMarbleNumThickStripes = GetDlgItemInt( IDC_EDIT_NUM_THICK_STRIPES);
	m_iMarbleNumThinStripes  = GetDlgItemInt( IDC_EDIT_NUM_THIN_STRIPES);

	CString strTemp; 
	GetDlgItemText( IDC_EDIT_AMPLITUDE, strTemp);
	m_dfMarbleAmplitude = atof( strTemp);

	GetDlgItemText( IDC_EDIT_SCALE_X, strTemp);
	m_vMarbleScale[0] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_SCALE_Y, strTemp);
	m_vMarbleScale[1] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_SCALE_Z, strTemp);
	m_vMarbleScale[2] = atof( strTemp);

	GetDlgItemText( IDC_EDIT_OFFSET_X, strTemp);
	m_vMarbleOffset[0] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_OFFSET_Y, strTemp);
	m_vMarbleOffset[1] = atof( strTemp);
	GetDlgItemText( IDC_EDIT_OFFSET_Z, strTemp);
	m_vMarbleOffset[2] = atof( strTemp);

	CDialog::OnOK();
}

void CMarbleDlg::OnBtnBackgroundColor()
{
	CPalettePickerDlg Dlg;
	Dlg.m_iIndex = m_pFilter->m_iMarbleColorIndexBackground;

	// Neal - TODO: make sure layer palette is always up-to-date
	//g_CurrentPalette = *(m_pLayer->GetPalette());
	g_CurrentPalette = *(m_pLayer->GetDoc()->GetPalette());

	if (IDOK == Dlg.DoModal())
	{
		m_pFilter->m_iMarbleColorIndexBackground = Dlg.m_iIndex;

		GetDlgItem( IDC_BTN_BACKGROUND_COLOR)->InvalidateRect( NULL, FALSE);
	}
}

void CMarbleDlg::OnBtnThickStripeColor()
{
	CPalettePickerDlg Dlg;
	Dlg.m_iIndex = m_pFilter->m_iMarbleColorIndexThickStripes;

	// Neal - TODO: make sure layer palette is always up-to-date
	//g_CurrentPalette = *(m_pLayer->GetPalette());
	g_CurrentPalette = *(m_pLayer->GetDoc()->GetPalette());

	if (IDOK == Dlg.DoModal())
	{
		m_pFilter->m_iMarbleColorIndexThickStripes = Dlg.m_iIndex;

		GetDlgItem( IDC_BTN_THICK_STRIPE_COLOR)->InvalidateRect( NULL, FALSE);
	}
}

void CMarbleDlg::OnBtnThinStripeColor()
{
	CPalettePickerDlg Dlg;
	Dlg.m_iIndex = m_pFilter->m_iMarbleColorIndexThinStripes;

	// Neal - TODO: make sure layer palette is always up-to-date
	//g_CurrentPalette = *(m_pLayer->GetPalette());
	g_CurrentPalette = *(m_pLayer->GetDoc()->GetPalette());

	if (IDOK == Dlg.DoModal())
	{
		m_pFilter->m_iMarbleColorIndexThinStripes = Dlg.m_iIndex;

		GetDlgItem( IDC_BTN_THIN_STRIPE_COLOR)->InvalidateRect( NULL, FALSE);
	}
}

void CMarbleDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int iColorIndex = -1;

	switch (nIDCtl)
	{
	case IDC_BTN_BACKGROUND_COLOR:
		if (m_pFilter)
			iColorIndex = m_pFilter->m_iMarbleColorIndexBackground;
		else
		{
			ASSERT( FALSE);
			iColorIndex = 0;
			::InvalidateRect( lpDrawItemStruct->hwndItem, NULL, FALSE);
		}
		break;

	case IDC_BTN_THICK_STRIPE_COLOR:
		if (m_pFilter)
			iColorIndex = m_pFilter->m_iMarbleColorIndexThickStripes;
		else
		{
			ASSERT( FALSE);
			iColorIndex = 0;
			::InvalidateRect( lpDrawItemStruct->hwndItem, NULL, FALSE);
		}
		break;

	case IDC_BTN_THIN_STRIPE_COLOR:
		if (m_pFilter)
			iColorIndex = m_pFilter->m_iMarbleColorIndexThinStripes;
		else
		{
			ASSERT( FALSE);
			iColorIndex = 0;
			::InvalidateRect( lpDrawItemStruct->hwndItem, NULL, FALSE);
		}
		break;

	default:
		CDialog::OnDrawItem( nIDCtl, lpDrawItemStruct);
		return;
	}

	// Neal - TODO: make sure layer palette is always up-to-date
	//CBrush brButtonColor( m_pLayer->GetPalette()->GetRGB( iColorIndex));
	CBrush brButtonColor( m_pLayer->GetDoc()->GetPalette()->GetRGB( iColorIndex));

	UINT uState = DFCS_BUTTONPUSH;
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uState |= DFCS_PUSHED;

	CRect rButton( lpDrawItemStruct->rcItem);

	DrawFrameControl( lpDrawItemStruct->hDC, &rButton,
			DFC_BUTTON, uState);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		rButton += CPoint( 1, 1);

	rButton += CRect( -4, -4, -5, -5);
	PatBlt( lpDrawItemStruct->hDC, rButton.left, rButton.top,
			rButton.Width(), rButton.Height(), BLACKNESS);

	InflateRect( &rButton, -1, -1);
	FillRect( lpDrawItemStruct->hDC, &rButton, brButtonColor);

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		InflateRect( &rButton, 3, 3);
		DrawFocusRect( lpDrawItemStruct->hDC, rButton);
	}
}

void CMarbleDlg::OnDeltaPos( NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN *)pNMHDR;

	CSpinButtonCtrl* pSpin = 
			(CSpinButtonCtrl *)CSpinButtonCtrl::FromHandle( pNMUpDown->hdr.hwndFrom);
	CWnd* pWndBuddy = pSpin->GetBuddy();

	// this is required because these fields are floating point numbers

	switch (pWndBuddy->GetDlgCtrlID())
	{
	case IDC_EDIT_AMPLITUDE:
	case IDC_EDIT_SCALE_X:
	case IDC_EDIT_SCALE_Y:
	case IDC_EDIT_SCALE_Z:
	case IDC_EDIT_OFFSET_X:
	case IDC_EDIT_OFFSET_Y:
	case IDC_EDIT_OFFSET_Z:
		{
			CString strText;
			pWndBuddy->GetWindowText( strText);
			double dfValue = atof( strText);

			int iPos   = pNMUpDown->iPos;
			int iDelta = pNMUpDown->iDelta;

			if ((double )(pNMUpDown->iPos) != dfValue)
			{
				iPos = (int )dfValue;

				// do we just drop fractional part?

				if ((iDelta < 0) && (dfValue > 0.0))
					iPos++;
				else if ((iDelta > 0) && (dfValue < 0.0))
					iPos--;

				pNMUpDown->iPos = iPos;
				pSpin->SetPos( iPos);
			}
		}
		break;

	default:
		break;
	}

	*pResult = 0;
}
