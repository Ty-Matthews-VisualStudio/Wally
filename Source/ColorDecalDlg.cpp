// ColorDecalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "ColorDecalDlg.h"
#include "MiscFunctions.h"
#include "ImageHelper.h"
#include "2PassScale.h"
#include "PackageDoc.h"
#include "WallyPal.h"
#include "ColorOpt.h"
#include "CustomResource.h"
#include "WallyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorDecalDlg dialog


CColorDecalDlg::CColorDecalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDecalDlg::IDD, pParent), m_bFirstTime( true )
{
	//{{AFX_DATA_INIT(CColorDecalDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CColorDecalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorDecalDlg)
	DDX_Control(pDX, IDC_COMBO_MOD, m_cbMod);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edHeight);
	DDX_Control(pDX, IDC_EDIT_FILE, m_edFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorDecalDlg, CDialog)
	//{{AFX_MSG_MAP(CColorDecalDlg)
	ON_BN_CLICKED(IDC_RADIO_CLIPBOARD, OnRadioClipboard)
	ON_BN_CLICKED(IDC_RADIO_FILE, OnRadioFile)
	ON_BN_CLICKED(IDC_RADIO_LET_ME, OnRadioLetMe)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_BN_CLICKED(IDC_BUTTON_HL_DIR, OnButtonHlDir)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_RADIO_ERROR, OnRadioError)
	ON_BN_CLICKED(IDC_RADIO_CROP, OnRadioCrop)
	ON_BN_CLICKED(IDC_RADIO_RESIZE, OnRadioResize)
	ON_EN_KILLFOCUS(IDC_EDIT_HL_DIRECTORY, OnKillfocusEditHlDirectory)
	ON_BN_CLICKED(IDC_RADIO_MAINTAIN_ASPECT, OnRadioMaintainAspect)
	ON_BN_CLICKED(IDC_RADIO_STRETCH_TO_FIT, OnRadioStretchToFit)
	ON_BN_CLICKED(IDC_CHECK_PURE_BLUE_PIXELS, OnCheckPureBluePixels)
	ON_REGISTERED_MESSAGE(WM_DECALCOLORCHOOSERWND_CUSTOM, OnChangeColor)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIO_BLEND_OVERALL_COLOR, OnRadioBlendOverallColor)
	ON_BN_CLICKED(IDC_RADIO_BLEND_SPECIFIC_COLOR, OnRadioBlendSpecificColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorDecalDlg message handlers

BOOL CColorDecalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_dwOptions = 0;

	switch( g_iColorDecalSelection & COLOR_DECAL_SOURCE_MASK )
	{
	case COLOR_DECAL_FILE:
		{
			CheckDlgButton( IDC_RADIO_FILE, 1);
			OnRadioFile();
		}
		break;

	case COLOR_DECAL_EDIT:
		{
			CheckDlgButton( IDC_RADIO_LET_ME, 1);
			OnRadioLetMe();
		}
		break;

	case COLOR_DECAL_CLIPBOARD:
	default:
		{
			CheckDlgButton( IDC_RADIO_CLIPBOARD, 1);
			OnRadioClipboard();
		}
		break;
	}

	switch( g_iColorDecalSelection & COLOR_DECAL_DIMENSION_MASK )
	{
	case COLOR_DECAL_CROP:
		{
			CheckDlgButton( IDC_RADIO_CROP, 1);
			OnRadioCrop();
		}
		break;
	
	case COLOR_DECAL_ERROR:
		{
			CheckDlgButton( IDC_RADIO_ERROR, 1);
			OnRadioError();
		}
		break;

	case COLOR_DECAL_RESIZE:
	default:
		{
			CheckDlgButton( IDC_RADIO_RESIZE, 1);
			OnRadioResize();
		}
		break;		
	}

	EnableDisableResizeOptions();

	switch( g_iColorDecalSelection & COLOR_DECAL_RESIZE_OPTION_MASK )
	{		
	case COLOR_DECAL_RESIZE_STRETCH_FIT:
		{
			CheckDlgButton( IDC_RADIO_STRETCH_TO_FIT, 1 );
			OnRadioStretchToFit();
		}
		break;

	case COLOR_DECAL_RESIZE_ASPECT_RATIO:
	default:
		{
			CheckDlgButton( IDC_RADIO_MAINTAIN_ASPECT, 1 );
			OnRadioMaintainAspect();
		}
		break;
	}

	CheckDlgButton( IDC_CHECK_PURE_BLUE_PIXELS, g_iColorDecalSelection & COLOR_DECAL_RESIZE_TRANSPARENT_BLUE );
	OnCheckPureBluePixels();

	switch( g_iColorDecalSelection & COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK )
	{	
	case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC:
		{
			CheckDlgButton( IDC_RADIO_BLEND_SPECIFIC_COLOR, 1 );
			OnRadioBlendSpecificColor();
		}
		break;
	
	case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL:
	default:
		{
			CheckDlgButton( IDC_RADIO_BLEND_OVERALL_COLOR, 1 );
			OnRadioBlendOverallColor();
		}
		break;
	}	
	
	SetDlgItemText( IDC_EDIT_HL_DIRECTORY, g_strHalfLifeDirectory);
	VerifyHalfLifeDirectory();
	
	SetDlgItemText( IDC_EDIT_WIDTH, "16");
	SetDlgItemText( IDC_EDIT_HEIGHT, "16");

	m_byRed = g_iColorDecalBlendRed;
	m_byGreen = g_iColorDecalBlendGreen;
	m_byBlue = g_iColorDecalBlendBlue;	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorDecalDlg::OnRadioClipboard() 
{
	m_btnFile.EnableWindow( FALSE);
	m_edFile.EnableWindow( FALSE);
	m_edWidth.EnableWindow( FALSE);
	m_edHeight.EnableWindow( FALSE);

	m_dwOptions |= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions ^= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions |= COLOR_DECAL_CLIPBOARD;
}

void CColorDecalDlg::OnRadioFile() 
{
	m_btnFile.EnableWindow( TRUE);
	m_edFile.EnableWindow( TRUE);
	m_edWidth.EnableWindow( FALSE);
	m_edHeight.EnableWindow( FALSE);

	m_dwOptions |= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions ^= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions |= COLOR_DECAL_FILE;	
}

void CColorDecalDlg::OnRadioLetMe() 
{
	m_btnFile.EnableWindow( FALSE);
	m_edFile.EnableWindow( FALSE);	
	m_edWidth.EnableWindow( TRUE);
	m_edHeight.EnableWindow( TRUE);	

	m_dwOptions |= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions ^= COLOR_DECAL_SOURCE_MASK;
	m_dwOptions |= COLOR_DECAL_EDIT;	
}

void CColorDecalDlg::OnKillfocusEditWidth() 
{
	VerifyDivisibleBy16( IDC_EDIT_WIDTH);
	VerifyWidthHeight( IDC_EDIT_WIDTH);
}

void CColorDecalDlg::OnKillfocusEditHeight() 
{
	VerifyDivisibleBy16( IDC_EDIT_HEIGHT);
	VerifyWidthHeight( IDC_EDIT_HEIGHT);
}

void CColorDecalDlg::VerifyDivisibleBy16( UINT iID)
{
	CString strValue("");
	int iValue = 0;

	GetDlgItemText( iID, strValue);
	iValue = abs(atoi( strValue));

	if ((iValue % 16) != 0)
	{
		iValue += 16 - (iValue % 16);
	}

	if (iValue == 0)
	{
		iValue = 16;
	}

	strValue.Format( "%lu", iValue);
	SetDlgItemText( iID, strValue);	
}

int CColorDecalDlg::GetDlgInt( UINT iID)
{
	CString strValue("");	
	GetDlgItemText( iID, strValue);
	return atoi(strValue);
}

void CColorDecalDlg::VerifyWidthHeight( UINT iID)
{
	CString strValue("");
	int iWidth = 0;
	int iHeight = 0;
	int iAdjust = 0;
	
	iWidth = abs(GetDlgInt(IDC_EDIT_WIDTH));	
	iHeight = abs(GetDlgInt(IDC_EDIT_HEIGHT));

	if ((iWidth * iHeight) > COLOR_DECAL_MAX_SIZE)
	{
		if (iID == IDC_EDIT_WIDTH)
		{
			iAdjust = (int)(COLOR_DECAL_MAX_SIZE / iHeight);

			if ((iAdjust % 16) != 0)
			{
				iAdjust -= (iAdjust % 16);
			}

			strValue.Format( "%lu", iAdjust);
			SetDlgItemText( IDC_EDIT_WIDTH, strValue);
		}
		else
		{
			iAdjust = (int)(COLOR_DECAL_MAX_SIZE / iWidth);

			if ((iAdjust % 16) != 0)
			{
				iAdjust -= (iAdjust % 16);
			}
			
			strValue.Format( "%lu", iAdjust);
			SetDlgItemText( IDC_EDIT_HEIGHT, strValue);
		}
	}
}

void CColorDecalDlg::OnButtonHlDir() 
{
	CString strDirectory("");
	GetDlgItemText( IDC_EDIT_HL_DIRECTORY, strDirectory);

	strDirectory = BrowseForFolder( "Select your Half-Life folder", strDirectory);

	if (strDirectory != "")
	{		
		SetDlgItemText( IDC_EDIT_HL_DIRECTORY, strDirectory);
		g_strHalfLifeDirectory = strDirectory;
		VerifyHalfLifeDirectory();
	}
}

BOOL CColorDecalDlg::VerifyHalfLifeDirectory()
{
	CString strDirectory("");
	GetDlgItemText( IDC_EDIT_HL_DIRECTORY, strDirectory);
	
	FILE *fp = fopen( TrimSlashes(strDirectory) + "\\hl.exe", "r");

	if( !fp )
	{
		fp = fopen( TrimSlashes(strDirectory) + "\\cstrike.exe", "r");
	}

	if( !fp )
	{
		fp = fopen( TrimSlashes(strDirectory) + "\\bshift.exe", "r");
	}

	if( fp )
	{
		fclose(fp);
		m_cbMod.EnableWindow( TRUE);
		m_cbMod.ResetContent();

		// Get a list of all folders
		CFileFind fFinder;
		CString strSearch("");

		// Build a list of all the files in this directory
		strSearch = TrimSlashes (strDirectory) + "\\*";
	
		BOOL bScanning = fFinder.FindFile(strSearch);
	
		while (bScanning)
		{
			bScanning = fFinder.FindNextFile();
			if ( fFinder.IsDirectory() && !fFinder.IsDots() )
			{
				// See if it has a liblist.gam file in it
				fp = fopen( TrimSlashes( fFinder.GetFilePath()) + "\\liblist.gam", "r");
				
				if (fp)
				{
					fclose(fp);
					m_cbMod.AddString( fFinder.GetFileName());
				}
			}			
		}

		int iSuccess = CB_ERR;

		if (g_strModSelection != "")
		{
			iSuccess = m_cbMod.SelectString( 0, g_strModSelection);
		}

		if (iSuccess == CB_ERR)
		{
			// Try and find the Counter-Strike folder and select it
			iSuccess = m_cbMod.SelectString( 0, "cstrike");
		}

		if (iSuccess == CB_ERR)
		{
			// Try and find the TFC folder and select it
			iSuccess = m_cbMod.SelectString( 0, "tfc");
		}

		if (iSuccess == CB_ERR)
		{
			// Try and find the Valve folder and select it
			iSuccess = m_cbMod.SelectString( 0, "valve");
		}
		
		return TRUE;
	}

	m_cbMod.EnableWindow( FALSE);
	return FALSE;
}

void CColorDecalDlg::OnOK() 
{
	if( GetOptions() & COLOR_DECAL_CLIPBOARD )
	{
		if (!::IsClipboardFormatAvailable(CF_DIB))
		{
			AfxMessageBox( "There is no image on the clipboard.  Please copy an image to the clipboard and try again.", MB_ICONWARNING);
			return;
		}
	}

	GetDlgItemText( IDC_EDIT_FILE, m_strFileName);

	if( GetOptions() & COLOR_DECAL_FILE )
	{
		if( m_strFileName.GetLength() == 0 )
		{
			AfxMessageBox( "Please specify the filename of the image you wish to load.", MB_ICONWARNING);
			OnButtonFile();
			return;
		}
	}
	
	GetDlgItemText( IDC_EDIT_HL_DIRECTORY, g_strHalfLifeDirectory);	
	m_dwWidth = abs(GetDlgInt(IDC_EDIT_WIDTH));	
	m_dwHeight = abs(GetDlgInt(IDC_EDIT_HEIGHT));

	int iCurSel = m_cbMod.GetCurSel();
	if (iCurSel != CB_ERR)
	{
		CString strMod("");
		m_cbMod.GetLBText( iCurSel, strMod);
		m_strDirectory = TrimSlashes(g_strHalfLifeDirectory) + "\\" + strMod;

		g_strModSelection = strMod;	
	}	

	g_iColorDecalSelection = GetOptions();
	g_iColorDecalBlendRed = m_byRed;
	g_iColorDecalBlendGreen = m_byGreen;
	g_iColorDecalBlendBlue = m_byBlue;	
	
	CDialog::OnOK();
}

DWORD CColorDecalDlg::GetWidth()
{
	return m_dwWidth;
}

DWORD CColorDecalDlg::GetHeight()
{
	return m_dwHeight;
}

DWORD CColorDecalDlg::GetOptions()
{
	return m_dwOptions;
}

LPCTSTR CColorDecalDlg::GetDirectory()
{
	return m_strDirectory;
}

LPCTSTR CColorDecalDlg::GetFileName()
{
	return m_strFileName;
}

void CColorDecalDlg::OnButtonFile() 
{
	CImageHelper ihImport;
		
	// Build some strings based on the String Table entries
	CString strWildCard ("All Files (*.*)|*.*|");
	strWildCard += ihImport.GetSupportedImageList( FALSE);
	CString strTitle("Select the file you wish to import as a decal");

	// Create a CFileDialog, init with our strings
	CFileDialog	dlgOpen (TRUE, NULL, NULL, OFN_FILEMUSTEXIST | 
		OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON, strWildCard, NULL);

	// Set some of the CFileDialog vars
	if (g_strFileOpenDirectory != "")
		dlgOpen.m_ofn.lpstrInitialDir = g_strFileOpenDirectory;
	dlgOpen.m_ofn.lpstrTitle = strTitle;	
	dlgOpen.m_ofn.nFilterIndex = g_iFileOpenExtension;

	if (dlgOpen.DoModal() == IDOK)
	{
		g_strFileOpenDirectory = dlgOpen.GetPathName().Left(dlgOpen.m_ofn.nFileOffset);
		g_iFileOpenExtension = dlgOpen.m_ofn.nFilterIndex;

		SetDlgItemText( IDC_EDIT_FILE, dlgOpen.GetPathName());
	}
}

void CColorDecalDlg::OnRadioError() 
{
	m_dwOptions |= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions ^= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions |= COLOR_DECAL_ERROR;
	EnableDisableResizeOptions();
}

void CColorDecalDlg::OnRadioCrop() 
{
	m_dwOptions |= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions ^= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions |= COLOR_DECAL_CROP;
	EnableDisableResizeOptions();
}

void CColorDecalDlg::OnRadioResize() 
{
	m_dwOptions |= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions ^= COLOR_DECAL_DIMENSION_MASK;
	m_dwOptions |= COLOR_DECAL_RESIZE;
	EnableDisableResizeOptions();
}

void CColorDecalDlg::EnableDisableResizeOptions()
{
	BOOL bEnable = IsDlgButtonChecked( IDC_RADIO_RESIZE );
	GetDlgItem( IDC_RADIO_MAINTAIN_ASPECT )->EnableWindow( bEnable );
	GetDlgItem( IDC_RADIO_STRETCH_TO_FIT )->EnableWindow( bEnable );
	GetDlgItem( IDC_CHECK_PURE_BLUE_PIXELS )->EnableWindow( bEnable );

	BOOL bBlue = IsDlgButtonChecked( IDC_CHECK_PURE_BLUE_PIXELS );	
	GetDlgItem( IDC_RADIO_BLEND_OVERALL_COLOR )->EnableWindow( bBlue && bEnable );
	GetDlgItem( IDC_RADIO_BLEND_SPECIFIC_COLOR )->EnableWindow( bBlue && bEnable );

	if( !m_bFirstTime )
	{
		m_wndColor.EnableWindow( bBlue && bEnable );
	}
}

void CColorDecalDlg::OnKillfocusEditHlDirectory() 
{
	VerifyHalfLifeDirectory();	
}

void CColorDecalDlg::OnRadioMaintainAspect() 
{
	m_dwOptions |= COLOR_DECAL_RESIZE_OPTION_MASK;
	m_dwOptions ^= COLOR_DECAL_RESIZE_OPTION_MASK;
	m_dwOptions |= COLOR_DECAL_RESIZE_ASPECT_RATIO;		
}

void CColorDecalDlg::OnRadioStretchToFit() 
{
	m_dwOptions |= COLOR_DECAL_RESIZE_OPTION_MASK;
	m_dwOptions ^= COLOR_DECAL_RESIZE_OPTION_MASK;
	m_dwOptions |= COLOR_DECAL_RESIZE_STRETCH_FIT;	
}

void CColorDecalDlg::OnCheckPureBluePixels() 
{
	EnableDisableResizeOptions();
	m_dwOptions |= COLOR_DECAL_RESIZE_TRANSPARENT_BLUE;
	if( !IsDlgButtonChecked( IDC_CHECK_PURE_BLUE_PIXELS ) )
	{
		m_dwOptions ^= COLOR_DECAL_RESIZE_TRANSPARENT_BLUE;
	}	
}

void CColorDecalDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if( m_bFirstTime )
	{
		m_bFirstTime = false;
		CWnd* pWnd = GetDlgItem( IDC_BUTTON_COLOR );

		if( pWnd )
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg );
			ClientToScreen( rDlg );
			pWnd->GetWindowRect( &Rect );
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
			pWnd->ShowWindow( SW_HIDE );

			m_wndColor.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL );
			m_wndColor.SetColor( g_iColorDecalBlendRed, g_iColorDecalBlendGreen, g_iColorDecalBlendBlue );

			pWnd->InvalidateRect( NULL, FALSE );
			m_wndColor.Update();
		}		
	}
	EnableDisableResizeOptions();
}

void CColorDecalDlg::OnChangeColor (UINT nType, UINT nFlags)
{
	switch( nType )
	{
	case DECAL_COLOR_CHOOSER_WND_LBUTTON_SELECT:
		{	
			COLORREF rgbColor( RGB (m_byRed, m_byGreen, m_byBlue));
			
			CColorDialog dlgColor( rgbColor, CC_FULLOPEN );			
			if( dlgColor.DoModal() == IDOK )
			{	
				rgbColor = dlgColor.GetColor();

				m_byRed = GetRValue( rgbColor );
				m_byGreen = GetGValue( rgbColor );
				m_byBlue = GetBValue( rgbColor );	
				m_wndColor.SetColor( m_byRed, m_byGreen, m_byBlue );	
				
				CWnd* pWnd = GetDlgItem( IDC_BUTTON_COLOR);
				if( pWnd )
				{		
					pWnd->InvalidateRect( NULL, FALSE );
				}
				// Invalidate our custom CPaletteWnd class	
				m_wndColor.Update();
			}			
		}
		break;

	default:
		ASSERT (FALSE);
	}
}

void CColorDecalDlg::OnRadioBlendOverallColor() 
{
	m_dwOptions |= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK;
	m_dwOptions ^= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK;
	m_dwOptions |= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL;	
}

void CColorDecalDlg::OnRadioBlendSpecificColor() 
{
	m_dwOptions |= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK;
	m_dwOptions ^= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK;
	m_dwOptions |= COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC;
}

void CColorDecalDlg::GetBlendColor( BYTE &byRed, BYTE &byGreen, BYTE &byBlue )
{
	byRed = m_byRed;
	byGreen = m_byGreen;
	byBlue = m_byBlue;
}

/* static */ void CColorDecalDlg::CalcNewDecalImageDimensions( UINT iWidth, UINT iHeight, UINT &iNewWidth, UINT &iNewHeight, UINT &iResizeWidth, UINT &iResizeHeight )
{
	UINT adjWidth = 0;
	UINT adjHeight = 0;
	double dfRatio = 0.0;
	double dfW = 0.0;
	double dfH = 0.0;
	BOOL bDone = FALSE;
	UINT adjTotal = 0;
	BOOL bWidthModified = FALSE;	
	BOOL bNon16 = FALSE;
	div_t dtWidth, dtHeight;

	iNewWidth = iWidth;
	iNewHeight = iHeight;

	iResizeWidth = iWidth;
	iResizeHeight = iHeight;

	// Are we too large?
	if( (iWidth * iHeight) > COLOR_DECAL_MAX_SIZE )
	{
		dfRatio = (iWidth * 1.0) / (iHeight * 1.0);
		dfW = sqrt( COLOR_DECAL_MAX_SIZE * dfRatio );
		dfH = dfW / dfRatio;
		
		dtWidth = div( dfW, 16 );
		dtHeight = div( dfH, 16 );		
		
		iNewWidth = dtWidth.quot * 16;
		iNewHeight = dtHeight.quot * 16;
		
		// Now recalculate the resize dimensions using this new total size
		adjTotal = 0;
		while( !bDone )
		{		
			dfW = sqrt( (iNewWidth * iNewHeight - adjTotal) * dfRatio );
			dfH = dfW / dfRatio;

			iResizeWidth = (UINT)dfW;
			iResizeHeight = (UINT)dfH;

			if( (iResizeWidth <= iNewWidth) && (iResizeHeight <= iNewHeight) )
			{
				bDone = true;
			}
			else
			{
				adjTotal++;
			}
		}		
	}

	// Are we not divisible by 16 but we're the right size or smaller?
	if( 
		(!IsValidImageSize( iWidth, iHeight, FORCE_X_16 )) &&
		((iWidth * iHeight) <= COLOR_DECAL_MAX_SIZE)
	)
	{
		// Try to enlarge the canvas		
		// Round up to the next closest multiple of 16
		dtWidth = div( iWidth + 15, 16 );
		dtHeight = div( iHeight + 15, 16 );

		iNewWidth = dtWidth.quot * 16;
		iNewHeight = dtHeight.quot * 16;

		iResizeWidth = iNewWidth;
		iResizeHeight = iNewHeight;

		// Are we too big now?
		if( (iNewWidth * iNewHeight) > COLOR_DECAL_MAX_SIZE )
		{
			dfRatio = (iWidth * 1.0) / (iHeight * 1.0);
			dfW = sqrt( COLOR_DECAL_MAX_SIZE * dfRatio );
			dfH = dfW / dfRatio;
			
			dtWidth = div( dfW, 16 );
			dtHeight = div( dfH, 16 );		
			
			iNewWidth = dtWidth.quot * 16;
			iNewHeight = dtHeight.quot * 16;
			
			// Now recalculate the resize dimensions using this new total size
			adjTotal = 0;
			while( !bDone )
			{		
				dfW = sqrt( (iNewWidth * iNewHeight - adjTotal) * dfRatio );
				dfH = dfW / dfRatio;

				iResizeWidth = (UINT)dfW;
				iResizeHeight = (UINT)dfH;

				if( (iResizeWidth <= iNewWidth) && (iResizeHeight <= iNewHeight) )
				{
					bDone = true;
				}
				else
				{
					adjTotal++;
				}
			}		
		}
	}
}

/* static */ void CColorDecalDlg::OnWizardColorDecal( CWallyApp *pWallyApp ) 
{
	CColorDecalDlg dlgWizard;
	CMemBuffer		mb8BitData( "mb8BitData" );
	CMemBuffer		mb24BitData( "mb24BitData" );
	CMemBuffer		mbIRGBData( "mbIRGBData" );	
	CMemBuffer		mb8BitDestData( "mb8BitDestData" );
	LPBYTE			pby8BitSourceData = NULL;
	LPBYTE			pby24BitSourceData = NULL;
	LPBYTE			pby8BitDestData = NULL;
	LPBYTE			pbyClipboardData = NULL;
	LPCOLOR_IRGB	pIRGBData = NULL;
	LPBYTE			pbyImageBits[4];	
	
	BYTE			by8BitPalette[768];
	BYTE			byOptimizedPalette[768];
	CWallyPalette	WallyPalette;
	CColorOptimizer ColorOpt;
	CWallyDoc		*pWallyDoc = NULL;
	CImageHelper	ihHelper;
	CPackageDoc		*pPackageDoc = NULL;

	UINT iWidth = 0;
	UINT iHeight = 0;
	UINT iNewWidth = 0;
	UINT iNewHeight = 0;
	UINT iResizeWidth = 0;
	UINT iResizeHeight = 0;
	UINT iBPP = 0;
	UINT iSize = 0;
	int i, j, b, g, r, x, y, x1, y1;
	UINT xSourceUL = 0;
	UINT ySourceUL = 0;
	UINT xDestUL = 0;
	UINT yDestUL = 0;
	UINT iOffset = 0;
	UINT iIndex = 0;
	UINT adjWidth = 0;
	UINT adjHeight = 0;
	BOOL bDone = FALSE;
	BOOL bWidthModified = FALSE;
	BOOL bContainsBluePixels = FALSE;
	BOOL bAlreadyBuilt = FALSE;

	BYTE byRedHold = 0;
	BYTE byGreenHold = 0;
	BYTE byBlueHold = 0;
	list<DWORD> lstBlueIndexes;
	list<DWORD>::iterator itIndex;
	BOOL bNonBlueColor = FALSE;
	BOOL bSwitchBluePixels = FALSE;

	BOOL bNon16 = FALSE;
	div_t dtWidth, dtHeight;
	DWORD dwOptions = 0;
	CString strPath("");
	CString strName("{LOGO");
	CString strTitle("");
	CString strError("");

	if( dlgWizard.DoModal() != IDOK )
	{
		return;
	}
	dwOptions = dlgWizard.GetOptions();		
	
	_mkdir( dlgWizard.GetDirectory() );
	strPath.Format( "%s\\pldecal.wad", dlgWizard.GetDirectory());

	// If the file isn't there, make one.  Avoids the "SaveAs" dialog when user goes to 
	// save the doc and it isn't already there.

	FILE *fp = fopen( strPath, "r");

	if (!fp)
	{
		fp = fopen( strPath, "wb");

		if (fp)
		{
			CCustomResource crWAD;	
			crWAD.UseResourceId( "WAD", IDR_WAD_PLDECAL);

			fwrite( crWAD.GetData(), 1, crWAD.GetDataSize(), fp);
		}
	}

	if (fp)
	{		
		fclose(fp);
	}

	// If it's just an edit, create it now and get out
	if( (dwOptions & COLOR_DECAL_SOURCE_MASK) == COLOR_DECAL_EDIT )
	{
		iWidth = dlgWizard.GetWidth();
		iHeight = dlgWizard.GetHeight();

		pPackageDoc = (CPackageDoc *)( pWallyApp->PackageDocTemplate->OpenDocumentFile( NULL, TRUE ) );
		
		if( !pPackageDoc )
		{
			AfxMessageBox( "Failed to create Half-Life WAD.", MB_ICONSTOP );
			return;
		}
		pPackageDoc->SetTitle( "pldecal.wad" );
		pPackageDoc->SetWADType( WAD3_TYPE );

		g_iDocWidth      = iWidth;
		g_iDocHeight     = iHeight;
		g_iDocColorDepth = 8;

		pWallyDoc = ( CWallyDoc *)( pWallyApp->WallyDocTemplate->OpenDocumentFile( NULL, TRUE ) );
		if( !pWallyDoc )
		{
			AfxMessageBox( "Failed to create Half-Life texture.", MB_ICONSTOP );
			return;
		}
		pWallyDoc->SetGameType( FILE_TYPE_HALF_LIFE );
		pWallyDoc->SetName( strName );
		pWallyDoc->SetTitle( strName );

		CCustomResource crPalette;
		crPalette.UseResourceId( "PALETTE", IDR_LMP_BLEND );
		LPBYTE pbyPalette = crPalette.GetData();
		pbyPalette[ 255 * 3 + 0 ] = 0;
		pbyPalette[ 255 * 3 + 1 ] = 0;
		pbyPalette[ 255 * 3 + 2 ] = 255;
		pWallyDoc->SetPalette( pbyPalette, 256, TRUE );

		// Link the package doc <-> WallyDoc				
		CWADItem *pWADItem = pPackageDoc->AddImage( pWallyDoc, strName, FALSE );
		pWallyDoc->SetPackageDoc( pPackageDoc );
		pWADItem->SetWallyDoc( pWallyDoc );
		pWallyDoc->SetModifiedFlag( FALSE );

		pPackageDoc->SetModifiedFlag( TRUE );
		pPackageDoc->OverridePathName( strPath );
		pPackageDoc->ReMipAll();

		return;
	}		

	switch( dwOptions & COLOR_DECAL_SOURCE_MASK )
	{
	case COLOR_DECAL_CLIPBOARD:
		{
			CDibSection ClipboardDIB;

			if( !ClipboardDIB.InitFromClipboard( pWallyApp->m_pMainWnd) )
			{
				AfxMessageBox ("Failed to open clipboard!", MB_ICONSTOP);
				return;
			}				
			
			iWidth  = ClipboardDIB.GetWidth();
			iHeight = ClipboardDIB.GetHeight();
			iBPP    = ClipboardDIB.GetBitCount();

			switch( iBPP )		// Color depth
			{
			case 8:
				{
					// Build the buffer so we can work with it
					pby8BitSourceData = mb8BitData.GetBuffer( iWidth * iHeight );
					
					if( !pby8BitSourceData )
					{
						AfxMessageBox( "Error: CWallyApp::OnWizardColorDecal() - Out of Memory - pby8BitSourceData == NULL");
						return;
					}						

					ClipboardDIB.GetRawBits( pby8BitSourceData );
					CopyMemory( by8BitPalette, ClipboardDIB.GetPalette(), 768 );
				}
				break;

			case 24:
				{
					// Build the buffer so we can work with it
					pby24BitSourceData = mb24BitData.GetBuffer( iWidth * iHeight * 3 );

					if( !pby24BitSourceData )
					{
						AfxMessageBox( "Error: CWallyApp::OnWizardColorDecal() - Out of Memory - pby24BitSourceData == NULL");
						return;
					}
					
					ClipboardDIB.GetRawBits( pby24BitSourceData );
				}
				break;

			default:
				{
					AfxMessageBox( "Image on clipboard is not of a supported bit depth.  Supported depths are 8- and 24-bit.", MB_ICONSTOP );
					return;
				}
				break;
			}
		}
		break;

	case COLOR_DECAL_FILE:
		{				
			ihHelper.LoadImage( dlgWizard.GetFileName(), IH_LOAD_ONLYIMAGE );

			if( ihHelper.GetErrorCode() != IH_SUCCESS )
			{
				AfxMessageBox( ihHelper.GetErrorText() );
				return;
			}

			iWidth = ihHelper.GetImageWidth();
			iHeight = ihHelper.GetImageHeight();
			iBPP = ihHelper.GetColorDepth();

			switch( iBPP )		// Color depth
			{
			case 8:
				{
					pby8BitSourceData = ihHelper.GetBits();
					CopyMemory( by8BitPalette, ihHelper.GetPalette(), 768 );						
				}
				break;

			case 24:
				{
					pby24BitSourceData = ihHelper.GetBits();
				}
				break;

			default:
				{
					AfxMessageBox( "Image specified is not of a supported bit depth.  Supported depths are 8- and 24-bit.", MB_ICONSTOP );
					return;
				}
				break;
			}
		}
		break;

	default:
		{
			// Did you add a new source type?
			ASSERT( false );
			return;
		}
		break;
	}
	
	// Check to make sure the image dimensions are okay				
	if( (dwOptions & COLOR_DECAL_DIMENSION_MASK) == COLOR_DECAL_ERROR )
	{
		if ((iWidth * iHeight) > COLOR_DECAL_MAX_SIZE)
		{				
			strError.Format( "This image is too large.  Width * Height must be less than or equal to %lu.\n\nYou can have Wally automatically crop or resize the image by selecting that option in the Wizard dialog box.", COLOR_DECAL_MAX_SIZE );
			AfxMessageBox (strError, MB_ICONSTOP);
			return;
		}

		dtWidth  = div (iWidth, 16);
		dtHeight = div (iHeight, 16);
		if ((dtWidth.rem != 0) || (dtHeight.rem != 0))
		{
			AfxMessageBox( "Dimensions of image on clipboard are not evenly divisible by 16.\n\nYou can have Wally fix this for you by selecting crop or resize in the Wizard dialog box.", MB_ICONSTOP );
			return;
		}

		// Else we know it's good to go
		xSourceUL = 0;
		ySourceUL = 0;
		xDestUL = 0;
		yDestUL = 0;
	}

	// Find the most appropriate size
	CalcNewDecalImageDimensions( iWidth, iHeight, iNewWidth, iNewHeight, iResizeWidth, iResizeHeight );

	// Do we need to resize the image first?
	if( ((dwOptions & COLOR_DECAL_DIMENSION_MASK) == COLOR_DECAL_RESIZE) && ((iResizeWidth != iWidth) || (iResizeHeight != iHeight)) )
	{
		if( (dwOptions & COLOR_DECAL_RESIZE_OPTION_MASK) == COLOR_DECAL_RESIZE_STRETCH_FIT )
		{
			// Don't bother with the transparent margins
			iResizeWidth = iNewWidth;
			iResizeHeight = iNewHeight;
		}
		float fRed, fGreen, fBlue;
		BYTE byNewRed, byNewGreen, byNewBlue;
		C2PassScale <CBilinearFilter> ScaleEngine;
		
		// Build the IRGB buffer		
		iSize = iWidth * iHeight;
		pIRGBData = ( LPCOLOR_IRGB )mbIRGBData.GetBuffer( iSize * sizeof( COLOR_IRGB ) );

		// Copy in the data
		switch( iBPP )		// Color depth
		{
		case 8:
			{
				bSwitchBluePixels = false;
				if( dwOptions & COLOR_DECAL_RESIZE_TRANSPARENT_BLUE )
				{					
					switch( dwOptions & COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK )
					{
					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL:
						{
							// Figure out what the overall color of the image is.  Since we are considering pure blue
							// to be transparent pixels, we do not want those pixels to be part of the anti-aliasing.
							// In order for a decent anti-aliasing to occur, we have to choose some kind of color.  
							// Picking the overall average color will create a reasonable anti-aliasing.  There is no
							// feasible way to return those pixels to pure blue, however, since we cannot be certain of 
							// what their new location is.  Thus it must be left to the user to manually fill in the
							// transparent areas once again.  At least we've anti-aliased to a more reasonable color
							// so that they do not have to fix those with a blue tint to them.					
							CalcImageColor256( iWidth, iHeight, pby8BitSourceData, by8BitPalette, &fRed, &fGreen, &fBlue, FALSE, TRUE );
							byNewRed = (BYTE)( 255.0 * fRed );
							byNewGreen = (BYTE)( 255.0 * fGreen );
							byNewBlue = (BYTE)( 255.0 * fBlue );
							bSwitchBluePixels = true;
						}
						break;

					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC:
						{
							dlgWizard.GetBlendColor( byNewRed, byNewGreen, byNewBlue );
							bSwitchBluePixels = true;
						}
						break;

					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SURROUND:
						{
						}
						break;
					}
					
				}

				for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
				{
					iIndex = pby8BitSourceData[iOffset];
					r = by8BitPalette[iIndex * 3 + 0];
					g = by8BitPalette[iIndex * 3 + 1];
					b = by8BitPalette[iIndex * 3 + 2];

					pIRGBData[iOffset] = IRGB( 0, r, g, b );					
					
					if( (r == 0) && (g == 0) && (b == 255) && bSwitchBluePixels )
					{
						// The user does not want to consider pure blue as being part of the image
						pIRGBData[iOffset] = IRGB( 0, byNewRed, byNewGreen, byNewBlue );
						bContainsBluePixels = TRUE;						
					}								
				}					
			}
			break;

		case 24:
			{
				bSwitchBluePixels = false;
				bAlreadyBuilt = false;
				
				if( dwOptions & COLOR_DECAL_RESIZE_TRANSPARENT_BLUE )
				{
					switch( dwOptions & COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_MASK )
					{
					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_OVERALL:
						{
							CalcImageColor24( iWidth, iHeight, pby24BitSourceData, &fRed, &fGreen, &fBlue, FALSE, TRUE );
							byNewRed = (BYTE)( 255.0 * fRed );
							byNewGreen = (BYTE)( 255.0 * fGreen );
							byNewBlue = (BYTE)( 255.0 * fBlue );
							bSwitchBluePixels = true;
						}
						break;

					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC:
						{
							dlgWizard.GetBlendColor( byNewRed, byNewGreen, byNewBlue );
							bSwitchBluePixels = true;
						}
						break;

					case COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SURROUND:
						{
							// This isn't accessible, as I'm not sure it is actually useful.  The
							// idea is to avoid having to force a specific color to blend to when blue
							// pixels are involved.
							bSwitchBluePixels = false;
							bAlreadyBuilt = true;
							
							// Get the overall color to fill in the ones where we can't find surrounding pixels
							CalcImageColor24( iWidth, iHeight, pby24BitSourceData, &fRed, &fGreen, &fBlue, FALSE, TRUE );
							byNewRed = (BYTE)( 255.0 * fRed );
							byNewGreen = (BYTE)( 255.0 * fGreen );
							byNewBlue = (BYTE)( 255.0 * fBlue );

							for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
							{
								// Start them all off at blue
								pIRGBData[iOffset] = IRGB( 0, 0, 0, 255 );
							}
#define IS_BLUE(x)			( (pby24BitSourceData[(x * 3) + 0] == 0) && (pby24BitSourceData[(x * 3) + 1] == 0) && (pby24BitSourceData[(x * 3) + 2] == 255) )
#define IS_RGBBLUE(x)		( (GetRValue(pIRGBData[x]) == 0) && (GetGValue(pIRGBData[x]) == 0) && (GetBValue(pIRGBData[x]) == 255) )
#define BLEND(i, r, g, b)	pIRGBData[i] = IRGB( 0, (r + GetRValue(pIRGBData[i])) / 2, (g + GetGValue(pIRGBData[i])) / 2, (b + GetBValue(pIRGBData[i])) / 2 )

							// Now search for non-blue pixels and use them to fill in the blue ones
							for( y = 0; y < iHeight; y++ )
							{
								for( x = 0; x < iWidth; x++ )
								{
									iOffset = y * iWidth + x;
									r = pby24BitSourceData[(iOffset * 3) + 0];
									g = pby24BitSourceData[(iOffset * 3) + 1];
									b = pby24BitSourceData[(iOffset * 3) + 2];
									
									if( (r != 0) || (g != 0) || (b != 255) )
									{
										pIRGBData[iOffset] = IRGB( 0, r, g, b );
										
										// We're not blue, look for blue neighbors
										
										// First look to all those on the left
										x1 = x - 1;
										while( x1 >= 0 )
										{
											i = y * iWidth + x1;
											if( IS_BLUE( i ) )
											{
												// Is our destination color blue?  If so, override it
												if( IS_RGBBLUE(i) )
												{
													pIRGBData[i] = IRGB( 0, r, g, b );
												}
												else
												{
													// Blend it
													BLEND(i, r, g, b );
												}
												x1--;
											}
											else
											{
												// Stop right there, as we can't go any further
												x1 = -1;
											}
										}

										// Next one to the right
										x1 = x + 1;
										while( x1 < iWidth )
										{
											i = y * iWidth + x1;
											if( IS_BLUE( i ) )
											{
												// Is our destination color blue?  If so, override it
												if( IS_RGBBLUE(i) )
												{
													pIRGBData[i] = IRGB( 0, r, g, b );
												}
												else
												{
													// Blend it
													BLEND(i, r, g, b );
												}
												x1++;
											}
											else
											{
												// Stop right there, as we can't go any further
												x1 = iWidth;
											}
										}

										// Next one to the top
										y1 = y - 1;
										while( y1 >= 0 )
										{
											i = y1 * iWidth + x;
											if( IS_BLUE( i ) )
											{
												// Is our destination color blue?  If so, override it
												if( IS_RGBBLUE(i) )
												{
													pIRGBData[i] = IRGB( 0, r, g, b );
												}
												else
												{
													// Blend it
													BLEND(i, r, g, b );
												}
												y1--;
											}
											else
											{
												// Stop right there, as we can't go any further
												y1 = -1;
											}
										}

										// Next one to the bottom
										y1 = y + 1;
										while( y1 < iHeight )
										{
											i = y1 * iWidth + x;
											if( IS_BLUE( i ) )
											{
												// Is our destination color blue?  If so, override it
												if( IS_RGBBLUE(i) )
												{
													pIRGBData[i] = IRGB( 0, r, g, b );
												}
												else
												{
													// Blend it
													BLEND(i, r, g, b );
												}
												y1++;
											}
											else
											{
												// Stop right there, as we can't go any further
												y1 = iHeight;
											}
										}
									}
									else
									{
										bContainsBluePixels = TRUE;
									}
								}
							}
							// Now put those that are blue to the overall image color
							for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
							{
								if( IS_RGBBLUE(iOffset) )
								{									
									pIRGBData[iOffset] = IRGB( 0, byNewRed, byNewGreen, byNewBlue );
								}
							}
						}
						break;
					}					
				}

				if( !bAlreadyBuilt )
				{
					for( iOffset = 0; iOffset < (iWidth * iHeight); iOffset++ )
					{
						r = pby24BitSourceData[(iOffset * 3) + 0];
						g = pby24BitSourceData[(iOffset * 3) + 1];
						b = pby24BitSourceData[(iOffset * 3) + 2];

						pIRGBData[iOffset] = IRGB( 0, r, g, b );

						if( (r == 0) && (g == 0) && (b == 255) && bSwitchBluePixels )
						{
							// The user does not want to consider pure blue as being part of the image
							pIRGBData[iOffset] = IRGB( 0, byNewRed, byNewGreen, byNewBlue );
							bContainsBluePixels = TRUE;
						}
					}
				}
			}
			break;
		}

		if( bContainsBluePixels )
		{
			if( dwOptions & COLOR_DECAL_RESIZE_TRANSPARENT_OPTION_BLEND_SPECIFIC )
			{
				AfxMessageBox( "Please note: your source image contains pure blue pixels.  These are being changed to the color you have chosen so that blue is not used during the anti-aliasing phase as the image is resized.\n\nYou will need to manually repaint your transparent areas in blue again once this process has finished.", MB_ICONWARNING );
			}
			else
			{
				AfxMessageBox( "Please note: your source image contains pure blue pixels.  These are being changed to match the overall color of the image so that blue is not used during the anti-aliasing phase as the image is resized.\n\nYou will need to manually repaint your transparent areas in blue again once this process has finished.", MB_ICONWARNING );
			}
		}

		COLOR_IRGB* pIRGBDestBuffer = ScaleEngine.AllocAndScale( pIRGBData, iWidth, iHeight, iResizeWidth, iResizeHeight );

		if( !pIRGBDestBuffer )
		{
			AfxMessageBox( "Failed to allocate memory for resize operation.", MB_ICONSTOP);
			return;
		}
		else
		{
			switch( iBPP )
			{
			case 8:
				{
					// Since we've converted to 24 and resized, we can change ourselves now
					iBPP = 24;						
				}

				// Fall through...
			case 24:
				{
					// Re-allocate this in case we've grown or (in the case of 8-bit) it doesn't exist at all
					pby24BitSourceData = mb24BitData.GetBuffer( iResizeWidth * iResizeHeight * 3 );
					BYTE i = 0;
					
					// Now go back and fix our source data
					for( iOffset = 0; iOffset < (iResizeWidth * iResizeHeight); iOffset++ )
					{
						pby24BitSourceData[(iOffset * 3) + 0] = GetRValue( pIRGBDestBuffer[iOffset] );
						pby24BitSourceData[(iOffset * 3) + 1] = GetGValue( pIRGBDestBuffer[iOffset] );
						pby24BitSourceData[(iOffset * 3) + 2] = GetBValue( pIRGBDestBuffer[iOffset] );
					}
				}
				break;
			}
		}
		
		// We are now resized.  Note that at this point we are resized to something that may not by in
		// multiples of 16.  Therefore we may still have to add in the margins and whatnot.
		iWidth = iResizeWidth;
		iHeight = iResizeHeight;
	}

	if( (iWidth != iNewWidth) || (iHeight != iNewHeight) )
	{
		if( iNewWidth > iWidth )
		{
			xSourceUL = 0;
			xDestUL = (UINT)( 
				((iNewWidth * 1.0) / 2.0)		// Center
				- 
				((iWidth * 1.0) / 2.0)			// Offset
			);
		}
		else
		{
			if( iNewWidth < iWidth )
			{
				xSourceUL = (UINT)( 
					((iWidth * 1.0) / 2.0)			// Center
					- 
					((iNewWidth * 1.0) / 2.0)		// Offset
				);
			}
			else
			{
				xSourceUL = 0;
				xDestUL = 0;
			}
		}

		if( iNewHeight > iHeight )
		{
			ySourceUL = 0;
			yDestUL = (UINT)( 
				((iNewHeight * 1.0) / 2.0)		// Center
				- 
				((iHeight * 1.0) / 2.0)			// Offset
			);
		}
		else
		{
			if( iNewHeight < iHeight )
			{
				ySourceUL = (UINT)( 
					((iHeight * 1.0) / 2.0)			// Center
					- 
					((iNewHeight * 1.0) / 2.0)		// Offset
				);
			}
			else
			{
				ySourceUL = 0;
				yDestUL = 0;
			}
		}
	}

	// Build the IRGB buffer		
	iSize = iNewWidth * iNewHeight;
	pIRGBData = ( LPCOLOR_IRGB )mbIRGBData.GetBuffer( iSize * sizeof( COLOR_IRGB ) );
	if( !pIRGBData )
	{
		AfxMessageBox( "Error: CWallyApp::OnWizardColorDecal() - Out of Memory - pIRGBData == NULL" );
		return;
	}

	// Did we enlarge canvas?  If so, set the edges to pure blue
	if( iNewWidth > iWidth )
	{
		for( x = 0; x < xDestUL; x++ )
		{
			for( y = 0; y < iNewHeight; y++ )
			{
				iOffset = (y * iNewWidth) + x;
				lstBlueIndexes.push_back(iOffset);
			}
		}

		for( x = iNewWidth - 1; x >= xDestUL + iWidth; x-- )
		{
			for( y = 0; y < iNewHeight; y++ )
			{
				iOffset = (y * iNewWidth) + x;
				lstBlueIndexes.push_back(iOffset);
			}
		}
	}

	if( iNewHeight > iHeight )
	{
		for( y = 0; y < yDestUL; y++ )
		{
			for( x = 0; x < iNewWidth; x++ )
			{
				iOffset = (y * iNewWidth) + x;
				lstBlueIndexes.push_back(iOffset);
			}
		}

		for( y = iNewHeight - 1; y >= yDestUL + iHeight; y-- )
		{
			for( x = 0; x < iNewWidth; x++ )
			{
				iOffset = (y * iNewWidth) + x;
				lstBlueIndexes.push_back(iOffset);
			}
		}
	}

	// Copy in the source data to the IRGB buffer
	switch( iBPP )		// Color depth
	{
	case 8:			
		{
			// Build the IRGB array
			for( y = ySourceUL, y1 = yDestUL; (y < (ySourceUL + iNewHeight)) && (y1 < (yDestUL + iHeight)); y++, y1++ )
			{
				for( x = xSourceUL, x1 = xDestUL; (x < (xSourceUL + iNewWidth)) && (x1 < (xDestUL + iWidth)); x++, x1++ )
				{
					iOffset = (y * iWidth) + x;
					iIndex = pby8BitSourceData[iOffset];

					r = by8BitPalette[iIndex * 3 + 0];
					g = by8BitPalette[iIndex * 3 + 1];
					b = by8BitPalette[iIndex * 3 + 2];

					// We don't want to consider pure blue as being part of the image
					// yet... so it won't build that as part of the palette during
					// optimization
					
					iOffset = (y1 * iNewWidth) + x1;
					if ((r == 0) && (g == 0) && (b == 255))
					{
						// Store these away, we'll set them to the hold color in a bit
						lstBlueIndexes.push_back(iOffset);
					}
					else
					{
						// Only set these values once... save some time
						if( !bNonBlueColor )
						{
							bNonBlueColor = TRUE;
							byRedHold = r;
							byGreenHold = g;
							byBlueHold = b;
						}									
						pIRGBData[iOffset] = IRGB( 0, r, g, b );
					}								
				}
			}
		}		 // case 8:
		break;

	case 24:
		{						
			// Build the IRGB array
			for( y = ySourceUL, y1 = yDestUL; (y < (ySourceUL + iNewHeight)) && (y1 < (yDestUL + iHeight)); y++, y1++ )
			{
				for( x = xSourceUL, x1 = xDestUL; (x < (xSourceUL + iNewWidth)) && (x1 < (xDestUL + iWidth)); x++, x1++ )
				{
					iOffset = (y * iWidth) + x;
					r = pby24BitSourceData[(iOffset * 3) + 0];
					g = pby24BitSourceData[(iOffset * 3) + 1];
					b = pby24BitSourceData[(iOffset * 3) + 2];

					// We don't want to consider pure blue as being part of the image
					// yet... so it won't build that as part of the palette during
					// optimization

					iOffset = (y1 * iNewWidth) + x1;
					if ((r == 0) && (g == 0) && (b == 255))
					{
						// Store these away, we'll set them to the hold color in a bit
						lstBlueIndexes.push_back(iOffset);									
					}
					else
					{
						// Only set these values once... save some time
						if (!bNonBlueColor)
						{
							bNonBlueColor = TRUE;
							byRedHold = r;
							byGreenHold = g;
							byBlueHold = b;
						}
						pIRGBData[iOffset] = IRGB( 0, r, g, b);
					}
				}
			}
		}
		break;

	default:
		{
			ASSERT (FALSE);
			AfxMessageBox( "Error: CWallyApp::OnWizardColorDecal() - Unhandled bit depth.", MB_ICONSTOP);
			return;
		}
		break;
	}			// switch (iBPP)

	// Now set the blue indexes to the hold color
	for( itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++ )
	{				
		pIRGBData[*itIndex] = IRGB( 0, byRedHold, byGreenHold, byBlueHold );
	}

	// This is the 8-bit reduced data
	pby8BitDestData = mb8BitDestData.GetBuffer( iSize );

	if( !pby8BitDestData )
	{
		AfxMessageBox( "Error: CWallyApp::OnWizardColorDecal() - Out of Memory - pby8BitDestData == NULL" );
		return;
	}

	// Only 255 colors, so we can reserve #256 for pure blue
	ColorOpt.Optimize( pIRGBData, iNewWidth, iNewHeight, byOptimizedPalette, 255, TRUE );

	byOptimizedPalette[765] = 0;
	byOptimizedPalette[766] = 0;
	byOptimizedPalette[767] = 255;

	WallyPalette.SetPalette( byOptimizedPalette, 256);

	// Put our indexes back to blue now
	for( itIndex = lstBlueIndexes.begin(); itIndex != lstBlueIndexes.end(); itIndex++ )
	{
		pIRGBData[*itIndex] = IRGB( 0, 0, 0, 255);
	}
			
	// Neal - we need to do this for 24 bit images too (builds index values)
	WallyPalette.Convert24BitTo256Color( pIRGBData, pby8BitDestData, iNewWidth, iNewHeight, 0, GetDitherType(), FALSE );		

	for (j = 0; j < 4; j++)
	{
		pbyImageBits[j] = pby8BitDestData;
	}

	// Now create the document and insert the image
	pPackageDoc = (CPackageDoc *)( pWallyApp->PackageDocTemplate->OpenDocumentFile( NULL, TRUE ) );
	
	if( !pPackageDoc )
	{
		AfxMessageBox( "Failed to create Half-Life WAD.", MB_ICONSTOP );
		return;
	}
	
	pPackageDoc->SetTitle( "pldecal.wad");
	pPackageDoc->SetWADType( WAD3_TYPE);

	pPackageDoc->AddImage( pbyImageBits, &WallyPalette, strName, iNewWidth, iNewHeight, TRUE );
	pPackageDoc->OverridePathName( strPath );
	pPackageDoc->ReMipAll();
}
