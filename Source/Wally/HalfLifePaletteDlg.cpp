// HalfLifePaletteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "HalfLifePaletteDlg.h"
#include "PaletteWnd.h"
#include "NewPaletteDlg.h"
#include "ImageHelper.h"
#include "PaletteNameDlg.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteDlg dialog


CHalfLifePaletteDlg::CHalfLifePaletteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHalfLifePaletteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHalfLifePaletteDlg)
	//}}AFX_DATA_INIT
	m_bFirstTime = true;
	m_strPath = g_strPaletteDirectory + "\\";
	
	for (int j = 0; j < 256; j++)
	{
		memset (m_byPalette + (j * 3), j, 3);			
	}	
}

void CHalfLifePaletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHalfLifePaletteDlg)
	DDX_Control(pDX, IDC_COMBO_PALETTE, m_cbPalette);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_edIndex);
	DDX_Control(pDX, IDC_EDIT_RED, m_edRed);
	DDX_Control(pDX, IDC_EDIT_GREEN, m_edGreen);
	DDX_Control(pDX, IDC_EDIT_BLUE, m_edBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHalfLifePaletteDlg, CDialog)
	//{{AFX_MSG_MAP(CHalfLifePaletteDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PALETTE, OnSelchangeComboPalette)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_EDIT_INDEX, OnKillfocusEditIndex)
	ON_EN_KILLFOCUS(IDC_EDIT_BLUE, OnKillfocusEditBlue)
	ON_EN_KILLFOCUS(IDC_EDIT_GREEN, OnKillfocusEditGreen)
	ON_EN_KILLFOCUS(IDC_EDIT_RED, OnKillfocusEditRed)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_BLEND, OnButtonBlend)
	ON_REGISTERED_MESSAGE(WM_PALETTEWND_CUSTOM, OnPaletteWndCustomMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHalfLifePaletteDlg message handlers

BOOL CHalfLifePaletteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Go find the available palettes in the current directory
	struct _finddata_t c_file;
	long hfile;
	CString strWildcard ("");
	CString strFileName ("");
	CString strFilePath ("");
	int iCount = 0;
	
	// Make the directory just to be sure we can write there
	_mkdir (m_strPath);	
	strWildcard = m_strPath + "*.lmp";
	
	if ((hfile = _findfirst(strWildcard, &c_file)) != -1L)
	{		
		m_cbPalette.AddString (c_file.name);
		// Keep looking for more files that match
		while (_findnext(hfile, &c_file) == 0)
		{
			m_cbPalette.AddString (c_file.name);
		}
	}

	strWildcard = m_strPath + "*.pal";
	
	if ((hfile = _findfirst(strWildcard, &c_file)) != -1L)
	{		
		m_cbPalette.AddString (c_file.name);
		// Keep looking for more files that match
		while (_findnext(hfile, &c_file) == 0)
		{
			m_cbPalette.AddString (c_file.name);			
		}
	}

	strWildcard = m_strPath + "*.act";
	
	if ((hfile = _findfirst(strWildcard, &c_file)) != -1L)
	{		
		m_cbPalette.AddString (c_file.name);
		// Keep looking for more files that match
		while (_findnext(hfile, &c_file) == 0)
		{
			m_cbPalette.AddString (c_file.name);			
		}
	}

	iCount = m_cbPalette.GetCount();

	if ((iCount != CB_ERR) && (iCount > 0))
	{
		m_cbPalette.SetCurSel (0);
		m_cbPalette.GetLBText (0, strFileName);
		strFilePath = m_strPath + strFileName;

		CImageHelper ihHelper;

		ihHelper.LoadImage (strFilePath, IH_LOAD_ONLYPALETTE);
		
		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText(), MB_ICONSTOP);
			return TRUE;
		}
		
		memcpy (m_byPalette, ihHelper.GetBits(), 768);
		ChangeSelection();
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHalfLifePaletteDlg::OnSelchangeComboPalette() 
{
	CString strFilePath ("");
	CString strFileName ("");
	int iSelection = m_cbPalette.GetCurSel();

	if (iSelection != CB_ERR)
	{	
		CImageHelper ihHelper;

		memset (m_byPalette, 0, 768);
		m_cbPalette.GetLBText (iSelection, strFileName);
		strFilePath = m_strPath + strFileName;
		
		ihHelper.LoadImage (strFilePath);
		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;			
		}
		memcpy (m_byPalette, ihHelper.GetBits(), 768);
		Update();		
	}
}

void CHalfLifePaletteDlg::Update()
{
	m_wndPalette.SetPalette (m_byPalette);
	
	// Invalidate the IDC_PALETTE button so that it hides itself
	CWnd* pWnd = GetDlgItem( IDC_PALETTE);
	if (pWnd)
	{		
		pWnd->InvalidateRect( NULL, FALSE);
	}
	else
	{
		ASSERT( FALSE);
	}

	// Invalidate our custom CPaletteWnd class	
	m_wndPalette.InvalidateRect( NULL, FALSE);
	
}

void CHalfLifePaletteDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		CWnd* pWnd = GetDlgItem(IDC_PALETTE);

		if (pWnd)
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			pWnd->GetWindowRect( &Rect);
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
			pWnd->ShowWindow( SW_HIDE);

			m_wndPalette.Create( NULL, NULL, 
				WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);			
		}
		Update();		
	}

}

void CHalfLifePaletteDlg::ChangeSelection()
{
	COLORREF rgbColor;
	int iIndex = m_wndPalette.GetSelectedIndex(BUTTON_LEFT);
	rgbColor = m_wndPalette.GetSelectedIndexColor(BUTTON_LEFT);

	CString strText ("");
	strText.Format ("%d", iIndex);
	SetDlgItemText (IDC_EDIT_INDEX, strText);
	
	strText.Format ("%d", GetRValue (rgbColor));
	SetDlgItemText (IDC_EDIT_RED, strText);

	strText.Format ("%d", GetGValue (rgbColor));
	SetDlgItemText (IDC_EDIT_GREEN, strText);

	strText.Format ("%d", GetBValue (rgbColor));
	SetDlgItemText (IDC_EDIT_BLUE, strText);	
}

void CHalfLifePaletteDlg::OnKillfocusEditIndex() 
{
	CString strText ("");

	GetDlgItemText (IDC_EDIT_INDEX, strText);

	int iIndex = atol (strText);

	iIndex = max (iIndex, 0);
	iIndex = min (iIndex, 255);

	strText.Format ("%d", iIndex);
	SetDlgItemText (IDC_EDIT_INDEX, strText);	

	m_wndPalette.SetSelectedIndex (iIndex, BUTTON_LEFT);
	ChangeSelection();
}

void CHalfLifePaletteDlg::OnKillfocusEditBlue() 
{
	CString strText ("");

	GetDlgItemText (IDC_EDIT_BLUE, strText);

	int iValue = atol (strText);

	iValue = max (iValue, 0);
	iValue = min (iValue, 255);

	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_BLUE, strText);

	m_wndPalette.SetPaletteIndex (COLOR_BLUE, iValue, BUTTON_LEFT);
	
}

void CHalfLifePaletteDlg::OnKillfocusEditGreen() 
{
	CString strText ("");

	GetDlgItemText (IDC_EDIT_GREEN, strText);

	int iValue = atol (strText);

	iValue = max (iValue, 0);
	iValue = min (iValue, 255);

	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_GREEN, strText);

	m_wndPalette.SetPaletteIndex (COLOR_GREEN, iValue, BUTTON_LEFT);
}

void CHalfLifePaletteDlg::OnKillfocusEditRed() 
{
	CString strText ("");

	GetDlgItemText (IDC_EDIT_RED, strText);

	int iValue = atol (strText);

	iValue = max (iValue, 0);
	iValue = min (iValue, 255);

	strText.Format ("%d", iValue);
	SetDlgItemText (IDC_EDIT_RED, strText);

	m_wndPalette.SetPaletteIndex (COLOR_RED, iValue, BUTTON_LEFT);
}

void CHalfLifePaletteDlg::OnButtonDefault() 
{
	CWnd *pFocus = GetFocus();

	if (
		(pFocus == &m_edBlue) ||
		(pFocus == &m_edRed) ||
		(pFocus == &m_edGreen)
		)
	{		
		OnKillfocusEditRed();
		OnKillfocusEditGreen();
		OnKillfocusEditBlue();
	}

	if (pFocus == &m_edIndex)
	{
		OnKillfocusEditIndex();
	}
}

void CHalfLifePaletteDlg::OnButtonLoad() 
{
	CString strWildCard ("Palette files (*.lmp, *.pal, *.act)|*.lmp;*.pal;*.act||");
	
	CFileDialog dlgPalette (true, ".lmp", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strWildCard, this);	

	if (g_strOpenPaletteDirectory != "")
	{
		dlgPalette.m_ofn.lpstrInitialDir = g_strOpenPaletteDirectory;
	}
	dlgPalette.m_ofn.lpstrTitle = "Choose a Palette";
	dlgPalette.m_ofn.nFilterIndex = g_iOpenPaletteExtension;
	int iSelection = 0;
	CString strSourceFilePath;
	CString strSourceFileName;
	CString strExtension;
	CString strSourceExtension;
	
	if (dlgPalette.DoModal() == IDOK)
	{
		CImageHelper ihHelper;
		CString strPalettePath;
		CString strError;	

		FILE *fp;
		
		g_strOpenPaletteDirectory = dlgPalette.GetPathName().Left( dlgPalette.m_ofn.nFileOffset);		
		strSourceFilePath         = dlgPalette.GetPathName();
		strSourceFileName         = dlgPalette.GetFileName();
		strSourceExtension        = GetExtension( strSourceFileName);

		ihHelper.LoadImage (strSourceFilePath);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
			return;
		}

		memcpy (m_byPalette, ihHelper.GetBits(), 768);

		strPalettePath = m_strPath + strSourceFileName;
		errno_t err = fopen_s (&fp, strPalettePath, "r");

		while (err == 0)
		{
			fclose (fp);
			CPaletteNameDlg dlgName (this, strSourceFileName);
			
			if (dlgName.DoModal() == IDOK)
			{
				strExtension      = GetExtension( dlgName.GetName());
				strPalettePath    = m_strPath + dlgName.GetName() + (strExtension.IsEmpty() ? strSourceExtension : "");
				strSourceFileName = GetRawFileNameWExt (strPalettePath);
				err = fopen_s(&fp, strPalettePath, "r");
			}
			else
			{
				return;
			}
		}
		
		ihHelper.SaveImage(0, strPalettePath, NULL, m_byPalette, 0, 0);		

		iSelection = m_cbPalette.AddString (strSourceFileName);
		m_cbPalette.SetCurSel(iSelection);		
		Update();
	}
}

void CHalfLifePaletteDlg::OnButtonNew() 
{
	CNewPaletteDlg dlgNewPalette;
	CString strPaletteName ("");
	CString strFileName ("");	
	CString strError ("");
	int iIndex = 0;
	int j = 0;
	FILE *fp = NULL;
	bool bFinished = false;

	while (!bFinished)
	{
		if (dlgNewPalette.DoModal() == IDOK)
		{
			strPaletteName = dlgNewPalette.GetPaletteName() + ".lmp";			
			strFileName = m_strPath + strPaletteName;

			// Make the palettes directory in case it's not there
			_mkdir (m_strPath);

			errno_t err = fopen_s( &fp, strFileName, "r");

			if (err == 0)
			{
				strError.Format ("%s already exists.  Please choose a unique name.", strFileName);
				AfxMessageBox (strError, MB_ICONEXCLAMATION);
				fclose (fp);
			}
			else
			{
				err = fopen_s(&fp, strFileName, "wb");
				if (err != 0)
				{
					strError.Format ("Cannot create %s!", strFileName);
					AfxMessageBox (strError, MB_ICONSTOP);
					return;
				}
				else
				{
					iIndex = m_cbPalette.AddString (strPaletteName);
					m_cbPalette.SetCurSel (iIndex);

					for (j = 0; j < 256; j++)
					{
						memset (m_byPalette + (j * 3), j, 3);			
					}

					fwrite (m_byPalette, 1, 768, fp);
					fclose (fp);
					
					Update();
					return;
				}				
			}			
		}
		else
		{
			bFinished = true;
		}
	}
}

void CHalfLifePaletteDlg::OnButtonSave() 
{	
	CString strFileName ("");
	CString strFileFullPath ("");
	CString strError("");

	int iSelection = m_cbPalette.GetCurSel();

	if (iSelection != CB_ERR)		
	{
		CImageHelper ihHelper;

		m_cbPalette.GetLBText (iSelection, strFileName);
		memcpy (m_byPalette, m_wndPalette.GetPalette(), 768);	
		strFileFullPath = m_strPath + strFileName;

		ihHelper.SaveImage (0, strFileFullPath, NULL, m_byPalette, 0, 0);

		if (ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (ihHelper.GetErrorText());
		}		
	}	
}

void CHalfLifePaletteDlg::OnButtonBlend() 
{
	m_wndPalette.Blend();
}

void CHalfLifePaletteDlg::OnOK() 
{
	memcpy (m_byPalette, m_wndPalette.GetPalette(), 768);
	CDialog::OnOK();
}


LRESULT CHalfLifePaletteDlg::OnPaletteWndCustomMessage(WPARAM nType, LPARAM nFlags)
{
	switch (nType)
	{
	case PALETTEWND_LBUTTONDOWN:
	case PALETTEWND_LBUTTONDBLCLK:
		ChangeSelection();
		break;

	case PALETTEWND_RBUTTONDOWN:		
		break;

	case PALETTEWND_RBUTTONDBLCLK:		
		break;

	default:
		ASSERT (false);		// Unhandled type?
		break;
	}	
	return 0;
}
