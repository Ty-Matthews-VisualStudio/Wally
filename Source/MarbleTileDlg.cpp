// MarbleTileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "Filter.h"
#include "Remip.h"
#include "PaletteDlg.h"
#include "MarbleDlg.h"
#include "MarbleTileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMarbleTileDlg dialog


CMarbleTileDlg::CMarbleTileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMarbleTileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMarbleTileDlg)
	m_iRandomSeed      = 111;
	m_iMarbleTileColor = 0;
	m_iMarbleTileShape = 0;
	m_iMarbleTileSize  = 0;
	m_iMarbleTileStyle = 0;
	m_iGroutType       = 0;
	m_iBevelType       = 0;
	//}}AFX_DATA_INIT

	m_pLayer                = NULL;
	m_pFilter               = NULL;
	m_bPreviewApplied       = FALSE;
}


void CMarbleTileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarbleTileDlg)
	DDX_Text(pDX, IDC_EDIT_RANDOM_SEED, m_iRandomSeed);
	DDV_MinMaxInt(pDX, m_iRandomSeed, 0, 100000);
	DDX_Radio(pDX, IDC_RADIO_COLOR_ANY, m_iMarbleTileColor);
	DDX_Radio(pDX, IDC_RADIO_SHAPE_ANY, m_iMarbleTileShape);
	DDX_Radio(pDX, IDC_RADIO_SIZE_ANY, m_iMarbleTileSize);
	DDX_Radio(pDX, IDC_RADIO_STYLE_ANY, m_iMarbleTileStyle);
	DDX_CBIndex(pDX, IDC_COMBO_GROUT, m_iGroutType);
	DDX_CBIndex(pDX, IDC_COMBO_BEVEL, m_iBevelType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarbleTileDlg, CDialog)
	//{{AFX_MSG_MAP(CMarbleTileDlg)
	ON_BN_CLICKED(ID_FILTER_FAST_PREVIEW, OnFilterFastPreview)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	ON_BN_CLICKED(ID_MARBLE_PROPERTIES, OnMarbleProperties)
	ON_BN_CLICKED(IDC_BTN_GROUT_COLOR, OnBtnGroutColor)
	ON_BN_CLICKED(IDC_BTN_RANDOM_SEED, OnBtnRandomSeed)
	ON_BN_CLICKED(IDC_BTN_JUST_DO_IT, OnBtnJustDoIt)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// helper function to read dlg field values
void CMarbleTileDlg::GetParams( MarbleParams* pParams)
{
	ASSERT( (m_pLayer != NULL) && (m_pFilter != NULL));
	pParams->pLayer = m_pLayer;

	pParams->iRandSeed        = m_pFilter->m_iMarbleSeed;
	pParams->iLevels          = m_pFilter->m_iMarbleLevels;
	pParams->iNumThickStripes = m_pFilter->m_iMarbleNumThickStripes;
	pParams->iNumThinStripes  = m_pFilter->m_iMarbleNumThinStripes;

	pParams->ptOffset = CPoint( 0, 0);
	pParams->sizeTile = CSize( 0, 0);
	pParams->iShape   = TILE_SQUARE;
	pParams->iStyle   = PLAIN_ALIGNED;
	pParams->bMirrorX = FALSE;
	pParams->bMirrorY = FALSE;

	pParams->iARGB_PrimaryField  = NULL;
	pParams->iARGB_ContrastField = NULL;
	pParams->iARGB_AccentField   = NULL;
	pParams->pbyBlockStencilData = NULL;

	pParams->vScale[0]  = m_pFilter->m_vMarbleScale[0];
	pParams->vScale[1]  = m_pFilter->m_vMarbleScale[1];
	pParams->vScale[2]  = m_pFilter->m_vMarbleScale[2];
	pParams->vOffset[0] = m_pFilter->m_vMarbleOffset[0];
	pParams->vOffset[1] = m_pFilter->m_vMarbleOffset[1];
	pParams->vOffset[2] = m_pFilter->m_vMarbleOffset[2];

	pParams->dfAmplitude = m_pFilter->m_dfMarbleAmplitude;

	pParams->iColorVariance   = m_pFilter->m_iMarbleColorVariance;
	pParams->bSeamless        = m_pFilter->m_bMarbleSeamlessTexture;
	pParams->bExtraSmooth     = m_pFilter->m_bMarbleExtraSmooth;

	pParams->iTileRandSeed  = GetDlgItemInt( IDC_EDIT_RANDOM_SEED);

	// TODO: read remaining params from dlg box

	pParams->iTileBevelType = TILE_BEVEL_STANDARD;
	pParams->iTileGroutType = TILE_GROUT_THIN_ROUGH;
	pParams->iTileSize      = TILE_MEDIUM;
	pParams->iShape         = TILE_SQUARE;
	pParams->iTileColor     = TILE_COLOR_MULTI;
	pParams->iTileStyle     = TILE_STYLE_FANCY;

	CComboBox* pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_BEVEL);

	if (pCombo)
	{
		switch (pCombo->GetCurSel())
		{
			case 0:
				pParams->iTileBevelType = TILE_BEVEL_ANY;
				break;
			case 1:
				pParams->iTileBevelType = TILE_BEVEL_NONE;
				break;
			case 2:
				pParams->iTileBevelType = TILE_BEVEL_SHALLOW;
				break;
			case 3:
				pParams->iTileBevelType = TILE_BEVEL_STANDARD;
				break;
			case 4:
				pParams->iTileBevelType = TILE_BEVEL_DEEP;
				break;
			default:
				ASSERT( FALSE);
		}
	}

	pCombo = (CComboBox* )GetDlgItem( IDC_COMBO_GROUT);

	if (pCombo)
	{
		switch (pCombo->GetCurSel())
		{
			case 0:
				pParams->iTileGroutType = TILE_GROUT_ANY;
				break;
			case 1:
				pParams->iTileGroutType = TILE_GROUT_NONE;
				break;
			case 2:
				pParams->iTileGroutType = TILE_GROUT_THIN_SMOOTH;
				break;
			case 3:
				pParams->iTileGroutType = TILE_GROUT_THIN_ROUGH;
				break;
			case 4:
				pParams->iTileGroutType = TILE_GROUT_THICK_SMOOTH;
				break;
			case 5:
				pParams->iTileGroutType = TILE_GROUT_THICK_ROUGH;
				break;
			default:
				ASSERT( FALSE);
		}
	}

	if (IsDlgButtonChecked( IDC_RADIO_SIZE_ANY))
		pParams->iTileSize = TILE_SIZE_ANY;
	else if (IsDlgButtonChecked( IDC_RADIO_SIZE_TINY))
		pParams->iTileSize = TILE_TINY;
	else if (IsDlgButtonChecked( IDC_RADIO_SIZE_SMALL))
		pParams->iTileSize = TILE_SMALL;
	else if (IsDlgButtonChecked( IDC_RADIO_SIZE_MEDIUM))
		pParams->iTileSize = TILE_MEDIUM;
	else if (IsDlgButtonChecked( IDC_RADIO_SIZE_LARGE))
		pParams->iTileSize = TILE_LARGE;
	else
	{
		ASSERT( FALSE);
	}

	if (IsDlgButtonChecked( IDC_RADIO_SHAPE_ANY))
		pParams->iShape = TILE_ANY;
	else if (IsDlgButtonChecked( IDC_RADIO_SHAPE_SQUARE))
		pParams->iShape = TILE_SQUARE;
	else if (IsDlgButtonChecked( IDC_RADIO_SHAPE_RECTANGULAR))
		pParams->iShape = TILE_RECTANGULAR;
	else if (IsDlgButtonChecked( IDC_RADIO_SHAPE_HEXAGONAL))
		pParams->iShape = TILE_HEXAGONAL;
	else if (IsDlgButtonChecked( IDC_RADIO_SHAPE_OTHER))
		pParams->iShape = TILE_OTHER_SHAPE;
	else
	{
		ASSERT( FALSE);
	}

	if (IsDlgButtonChecked( IDC_RADIO_COLOR_ANY))
		pParams->iTileColor = TILE_COLOR_ANY;
	else if (IsDlgButtonChecked( IDC_RADIO_COLOR_SOLID))
		pParams->iTileColor = TILE_COLOR_SOLID;
	else if (IsDlgButtonChecked( IDC_RADIO_COLOR_ACCENTED))
		pParams->iTileColor = TILE_COLOR_ACCENTED;
	else if (IsDlgButtonChecked( IDC_RADIO_COLOR_TWO))
		pParams->iTileColor = TILE_COLOR_TWO_TONE;
	else if (IsDlgButtonChecked( IDC_RADIO_COLOR_MULTI))
		pParams->iTileColor = TILE_COLOR_MULTI;
	else
	{
		ASSERT( FALSE);
	}

	if (IsDlgButtonChecked( IDC_RADIO_STYLE_ANY))
		pParams->iTileStyle = TILE_STYLE_ANY;
	else if (IsDlgButtonChecked( IDC_RADIO_STYLE_PLAIN))
		pParams->iTileStyle = TILE_STYLE_PLAIN;
	else if (IsDlgButtonChecked( IDC_RADIO_STYLE_FANCY))
		pParams->iTileStyle = TILE_STYLE_FANCY;
	else if (IsDlgButtonChecked( IDC_RADIO_STYLE_EXTRA_FANCY))
		pParams->iTileStyle = TILE_STYLE_EXTRA_FANCY;
	else
	{
		ASSERT( FALSE);
	}

	pParams->bFastPreview = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMarbleTileDlg message handlers

void CMarbleTileDlg::OnFilterFastPreview() 
{
	DoFilterPreview( TRUE);
}

void CMarbleTileDlg::DoFilterPreview( BOOL bFastPreview)
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	MarbleParams Params;
	GetParams( &Params);

	Params.bFastPreview = bFastPreview;

	g_Filter.DoMarbleTex( &Params);

	m_bPreviewApplied = TRUE;
}

void CMarbleTileDlg::OnFilterPreview() 
{
	DoFilterPreview( FALSE);
}

void CMarbleTileDlg::OnMarbleProperties() 
{
	ASSERT( (m_pLayer != NULL) && (m_pFilter != NULL));

	if (m_pFilter->m_iMarbleColorIndexBackground == -1)
		m_pFilter->m_iMarbleColorIndexBackground = m_pLayer->GetDoc()->GetPalette()->FindNearestColor( 255, 255, 255, FALSE);

	if (m_pFilter->m_iMarbleColorIndexThickStripes == -1)
		m_pFilter->m_iMarbleColorIndexThickStripes = m_pLayer->GetDoc()->GetPalette()->FindNearestColor( 128, 128, 128, FALSE);

	if (m_pFilter->m_iMarbleColorIndexThinStripes == -1)
		m_pFilter->m_iMarbleColorIndexThinStripes = m_pLayer->GetDoc()->GetPalette()->FindNearestColor(  32,  32,  32, FALSE);

	CMarbleDlg Dlg;

	Dlg.m_strDlgTitle = "Set Marble Properties (Tile In Progress)";
	Dlg.m_pLayer      = m_pLayer;
	Dlg.m_pFilter     = m_pFilter;

	Dlg.m_iMarbleSeed            = m_pFilter->m_iMarbleSeed;
	Dlg.m_iMarbleLevels          = m_pFilter->m_iMarbleLevels;
	Dlg.m_iMarbleNumThickStripes = m_pFilter->m_iMarbleNumThickStripes;
	Dlg.m_iMarbleNumThinStripes  = m_pFilter->m_iMarbleNumThinStripes;
	Dlg.m_dfMarbleAmplitude      = m_pFilter->m_dfMarbleAmplitude;
	Dlg.m_bMarbleExtraSmooth     = m_pFilter->m_bMarbleExtraSmooth;
	Dlg.m_bMarbleSeamlessTexture = m_pFilter->m_bMarbleSeamlessTexture;
	Dlg.m_iMarbleColorVariance   = m_pFilter->m_iMarbleColorVariance;
	Dlg.m_vMarbleScale[0]        = m_pFilter->m_vMarbleScale[0];
	Dlg.m_vMarbleScale[1]        = m_pFilter->m_vMarbleScale[1];
	Dlg.m_vMarbleScale[2]        = m_pFilter->m_vMarbleScale[2];
	Dlg.m_vMarbleOffset[0]       = m_pFilter->m_vMarbleOffset[0];
	Dlg.m_vMarbleOffset[1]       = m_pFilter->m_vMarbleOffset[1];
	Dlg.m_vMarbleOffset[2]       = m_pFilter->m_vMarbleOffset[2];

	MarbleParams Params;
	GetParams( &Params);
	Dlg.m_pTileParams = &Params;

	ShowWindow( SW_HIDE);
	int iRet = Dlg.DoModal();
	ShowWindow( SW_SHOW);

	if (iRet == IDOK)
	{
		m_pFilter->m_iMarbleSeed            = Dlg.m_iMarbleSeed;
		m_pFilter->m_iMarbleLevels          = Dlg.m_iMarbleLevels;
		m_pFilter->m_iMarbleNumThickStripes = Dlg.m_iMarbleNumThickStripes;
		m_pFilter->m_iMarbleNumThinStripes  = Dlg.m_iMarbleNumThinStripes;
		m_pFilter->m_dfMarbleAmplitude      = Dlg.m_dfMarbleAmplitude;
		m_pFilter->m_bMarbleExtraSmooth     = Dlg.m_bMarbleExtraSmooth;
		m_pFilter->m_bMarbleSeamlessTexture = Dlg.m_bMarbleSeamlessTexture;
		m_pFilter->m_iMarbleColorVariance   = Dlg.m_iMarbleColorVariance;
		m_pFilter->m_vMarbleScale[0]        = Dlg.m_vMarbleScale[0];
		m_pFilter->m_vMarbleScale[1]        = Dlg.m_vMarbleScale[1];
		m_pFilter->m_vMarbleScale[2]        = Dlg.m_vMarbleScale[2];
		m_pFilter->m_vMarbleOffset[0]       = Dlg.m_vMarbleOffset[0];
		m_pFilter->m_vMarbleOffset[1]       = Dlg.m_vMarbleOffset[1];
		m_pFilter->m_vMarbleOffset[2]       = Dlg.m_vMarbleOffset[2];
	}
	m_bPreviewApplied = Dlg.m_bPreviewApplied;
}

void CMarbleTileDlg::OnBtnRandomSeed() 
{
	InitRandom( GetTickCount());

	double dIgnore = Random();		// just to prime the generator

	SetDlgItemInt( IDC_EDIT_RANDOM_SEED, GetRandomSeed() % 100000);
}

void CMarbleTileDlg::OnBtnJustDoIt() 
{
	OnBtnRandomSeed();

	UpdateWindow();
	//OnFilterPreview();
	OnFilterFastPreview();
}

BOOL CMarbleTileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMarbleTileDlg::OnBtnGroutColor() 
{
	CPalettePickerDlg Dlg;
	Dlg.m_iIndex = m_pFilter->m_iMarbleTileColorGrout;

	if (IDOK == Dlg.DoModal())
	{
		m_pFilter->m_iMarbleTileColorGrout = Dlg.m_iIndex;

		GetDlgItem( IDC_BTN_GROUT_COLOR)->InvalidateRect( NULL, FALSE);
	}
}

void CMarbleTileDlg::OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int iColorIndex = -1;

	switch (nIDCtl)
	{
	case IDC_BTN_GROUT_COLOR:
		if (m_pFilter)
			iColorIndex = m_pFilter->m_iMarbleTileColorGrout;
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

void CMarbleTileDlg::OnOK() 
{
	MarbleParams Params;
	GetParams( &Params);

	if (m_pFilter)
	{
		m_pFilter->m_iMarbleTileSeed      = Params.iTileRandSeed;
		m_pFilter->m_iMarbleTileSize      = Params.iTileSize;
		m_pFilter->m_iMarbleTileShape     = Params.iShape;
 		m_pFilter->m_iMarbleTileColor     = Params.iTileColor;
		m_pFilter->m_iMarbleTileStyle     = Params.iTileStyle;
		m_pFilter->m_iMarbleTileGroutType = Params.iTileGroutType;
		m_pFilter->m_iMarbleTileBevelType = Params.iTileBevelType;
	}
	else
	{
		ASSERT( FALSE);
	}
	
	CDialog::OnOK();
}
