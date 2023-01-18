// SinFlagsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "SinFlagsDlg.h"
#include "ImageHelper.h"
#include "TextureFlags.h"
#include "SinFlagsColorWnd.h"
#include "WallyPal.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSinFlagsDlg dialog


CSinFlagsDlg::CSinFlagsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSinFlagsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSinFlagsDlg)
	//}}AFX_DATA_INIT
	memset (&m_SinHeader, 0, sizeof (SIN_MIP_S));
	m_bFirstTime = TRUE;

	m_pbyImageData = NULL;
	m_pPalette = NULL;
}


void CSinFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSinFlagsDlg)
	DDX_Control(pDX, IDC_COMBO_VALUE, m_cbValue);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_edValue);
	DDX_Control(pDX, IDC_EDIT_COLOR_R, m_edColorB);
	DDX_Control(pDX, IDC_EDIT_COLOR_G, m_edColorG);
	DDX_Control(pDX, IDC_EDIT_COLOR_B, m_edColorR);
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_btnAuto);
	DDX_Control(pDX, IDC_LIST_FIELDS, m_lbFields);
	DDX_Control(pDX, IDC_COMBO_SURFACE_TYPE, m_cbSurfaceTypes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSinFlagsDlg, CDialog)
	//{{AFX_MSG_MAP(CSinFlagsDlg)
	ON_LBN_SELCHANGE(IDC_LIST_FIELDS, OnSelchangeListFields)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, OnKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_ANIMNAME, OnKillfocusEditAnimname)
	ON_EN_KILLFOCUS(IDC_EDIT_VALUE, OnKillfocusEditValue)
	ON_EN_KILLFOCUS(IDC_EDIT_COLOR_B, OnKillfocusEditColorB)
	ON_EN_KILLFOCUS(IDC_EDIT_COLOR_G, OnKillfocusEditColorG)
	ON_EN_KILLFOCUS(IDC_EDIT_COLOR_R, OnKillfocusEditColorR)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(WM_SINFLAGSWND_CUSTOM, OnChangeColor)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_CBN_KILLFOCUS(IDC_COMBO_VALUE, OnKillfocusComboValue)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSinFlagsDlg message handlers

BOOL CSinFlagsDlg::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{	
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;    
	UINT nID =pNMHDR->idFrom;
    
	if (pTTT->uFlags & TTF_IDISHWND)
	{
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
		if(nID)
		{
            pTTT->lpszText = MAKEINTRESOURCE(nID);
            pTTT->hinst = AfxGetResourceHandle();
			return(TRUE);
        }
	}
	return(FALSE);
}

void CSinFlagsDlg::Update()
{
	m_wndColor.SetColor (m_SinHeader.color[0], m_SinHeader.color[1], m_SinHeader.color[2]);
	
	// Invalidate the IDC_PALETTE button so that it hides itself
	CWnd* pWnd = GetDlgItem( IDC_BUTTON_COLOR);
	if (pWnd)
	{		
		pWnd->InvalidateRect( NULL, FALSE);
	}
	else
	{
		ASSERT( FALSE);
	}

	// Invalidate our custom CPaletteWnd class	
	m_wndColor.Update();
}

void CSinFlagsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bFirstTime)
	{
		m_bFirstTime = false;
		CWnd* pWnd = GetDlgItem(IDC_BUTTON_COLOR);

		if (pWnd)
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			pWnd->GetWindowRect( &Rect);
			Rect.OffsetRect( -rDlg.left, -rDlg.top);
			pWnd->ShowWindow( SW_HIDE);

			m_wndColor.Create( NULL, NULL, 
				WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);			
		}
		Update();		
	}
}


BOOL CSinFlagsDlg::OnInitDialog() 
{
	EnableToolTips(TRUE);
	CDialog::OnInitDialog();
		
	CString strText ("");
	SetDlgItemText (IDC_DESCRIPTION, strText);
	SetDlgItemText (IDC_EDIT_NAME, m_SinHeader.name);
	SetDlgItemText (IDC_EDIT_ANIMNAME, m_SinHeader.animname);

	strText.Format ("%d", m_SinHeader.width);
	SetDlgItemText (IDC_WIDTH, strText);

	strText.Format ("%d", m_SinHeader.height);
	SetDlgItemText (IDC_HEIGHT, strText);

	InitSurfaceTypes();	
	InitFields();
	UpdateColorFields();	
	ProcessChecks(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSinFlagsDlg::InitSurfaceTypes()
{
	AddSurfaceType ("None", SIN_SURF_TYPE_NONE);
	AddSurfaceType ("Wood", SIN_SURF_TYPE_WOOD);
	AddSurfaceType ("Metal", SIN_SURF_TYPE_METAL);
	AddSurfaceType ("Stone", SIN_SURF_TYPE_STONE);
	AddSurfaceType ("Concrete", SIN_SURF_TYPE_CONCRETE);
	AddSurfaceType ("Dirt", SIN_SURF_TYPE_DIRT);
	AddSurfaceType ("Flesh", SIN_SURF_TYPE_FLESH);
	AddSurfaceType ("Grill", SIN_SURF_TYPE_GRILL);
	AddSurfaceType ("Glass", SIN_SURF_TYPE_GLASS);
	AddSurfaceType ("Fabric", SIN_SURF_TYPE_FABRIC);
	AddSurfaceType ("Monitor", SIN_SURF_TYPE_MONITOR);
	AddSurfaceType ("Gravel", SIN_SURF_TYPE_GRAVEL);
	AddSurfaceType ("Vegetation", SIN_SURF_TYPE_VEGETATION);
	AddSurfaceType ("Paper", SIN_SURF_TYPE_PAPER);
	AddSurfaceType ("Duct", SIN_SURF_TYPE_DUCT);
	AddSurfaceType ("Water", SIN_SURF_TYPE_WATER);

	int iSurfaceType = SIN_SURF_TYPE_SHIFT (SIN_SURFACETYPE_FROM_FLAGS(m_SinHeader.flags));
	int j = 0;
	
	for (j = 0; j < m_cbSurfaceTypes.GetCount(); j++)
	{
		if ((int)(m_cbSurfaceTypes.GetItemData(j)) == iSurfaceType)
		{
			m_cbSurfaceTypes.SetCurSel (j);
			j = m_cbSurfaceTypes.GetCount();
		}
	}

}

void CSinFlagsDlg::InitFields()
{
	AddField ("Radiosity", "Radiosity light value", &m_SinHeader.value);
	AddField ("Direct Value", "Lighting value for direct surfaces", &m_SinHeader.direct);
	AddField ("Direct Angle", "The angle of the light coming off the direct surface", &m_SinHeader.directangle);
	AddField ("Animation Time", "The animation time to the next frame of animation", &m_SinHeader.animtime);
	AddField ("Nonlit", "A nonlit value used for surfaces that don't have lightmaps [0..1]", &m_SinHeader.nonlit);	

	unsigned int piTMPredefined[] = { 0, 1, 2, 4, 8, 16, 24, 32, 40, 48, 56, 64, 128};
	unsigned int piTAPredefined[] = { 0, 30, 45, 60, 90, 120, 135, 150, 180, 210, 225, 240, 270, 300, 315, 330};
	
	AddField ("Translate Angle", "The angle that a surface should translate along.  This is added on top of the base rotation of the texture [0..359]", &m_SinHeader.trans_angle, (sizeof (piTAPredefined) / sizeof (unsigned int)), piTAPredefined);
	AddField ("Translate Magnitude", "The rate at which a surface scrolls in pixels per second", &m_SinHeader.trans_mag, (sizeof (piTMPredefined) / sizeof (unsigned int)), piTMPredefined);
	AddField ("Transluscence", "The translucence of the texture [0 = opaque, 1 = transparent]", &m_SinHeader.translucence);
	AddField ("Friction", "How slick the surface is (default 1.0) [0..4]", &m_SinHeader.friction);
	AddField ("Restitution", "How bouncy a surface is (default 0)", &m_SinHeader.restitution);
	m_lbFields.SetCurSel (0);
	OnSelchangeListFields();
}

void CSinFlagsDlg::AddSurfaceType (LPCTSTR szText, int iValue)
{
	int iItemAdded = m_cbSurfaceTypes.AddString (szText);

	if (iItemAdded != CB_ERR)
	{
		m_cbSurfaceTypes.SetItemData (iItemAdded, iValue);
	}
}

void CSinFlagsDlg::AddField (LPCTSTR szText, LPCTSTR szDescription, unsigned short *pucValue, int iPredefinedCount /* = 0 */, unsigned int *pPredefined /* = NULL */)
{
	int iItemAdded = m_lbFields.AddString (szText);

	LPSIN_FIELD lpSinField = new SIN_FIELD;
	memset ((BYTE *)lpSinField, 0, SIN_FIELD_SIZE);
	
	lpSinField->iType = SIN_FIELD_TYPE_UC;
	lpSinField->pucValue = pucValue;
	lpSinField->puiValue = NULL;
	lpSinField->pfValue = NULL;	
	strcpy_s (lpSinField->szDescription, sizeof(lpSinField->szDescription), szDescription);

	if (iPredefinedCount != 0)
	{
		ASSERT (pPredefined);
		lpSinField->iPredefinedCount = iPredefinedCount;
		lpSinField->piPreDefine = new unsigned int[iPredefinedCount];

		memcpy (lpSinField->piPreDefine, pPredefined, iPredefinedCount * sizeof (unsigned int));
	}
	
	m_lbFields.SetItemData (iItemAdded, (ULONG)lpSinField);
}

void CSinFlagsDlg::AddField (LPCTSTR szText, LPCTSTR szDescription, unsigned int *puiValue, int iPredefinedCount /* = 0 */, unsigned int *pPredefined /* = NULL */)
{
	int iItemAdded = m_lbFields.AddString (szText);

	LPSIN_FIELD lpSinField = new SIN_FIELD;
	memset ((BYTE *)lpSinField, 0, SIN_FIELD_SIZE);
	
	lpSinField->iType = SIN_FIELD_TYPE_UINT;
	lpSinField->puiValue = puiValue;
	lpSinField->pucValue = NULL;
	lpSinField->pfValue = NULL;	
	strcpy_s (lpSinField->szDescription, sizeof(lpSinField->szDescription), szDescription);

	if (iPredefinedCount != 0)
	{
		ASSERT (pPredefined);
		lpSinField->iPredefinedCount = iPredefinedCount;
		lpSinField->piPreDefine = new unsigned int[iPredefinedCount];

		memcpy (lpSinField->piPreDefine, pPredefined, iPredefinedCount * sizeof (unsigned int));
	}
	
	m_lbFields.SetItemData (iItemAdded, (ULONG)lpSinField);
}

void CSinFlagsDlg::AddField (LPCTSTR szText, LPCTSTR szDescription, float *pfValue)
{
	int iItemAdded = m_lbFields.AddString (szText);

	LPSIN_FIELD lpSinField = new SIN_FIELD;
	memset ((BYTE *)lpSinField, 0, SIN_FIELD_SIZE);

	lpSinField->iType = SIN_FIELD_TYPE_FLOAT;
	lpSinField->pucValue = NULL;
	lpSinField->puiValue = NULL;
	lpSinField->pfValue = pfValue;
	strcpy_s (lpSinField->szDescription, sizeof(lpSinField->szDescription), szDescription);
	
	m_lbFields.SetItemData (iItemAdded, (ULONG)lpSinField);
}


BOOL CSinFlagsDlg::DestroyWindow() 
{
	LPSIN_FIELD lpSinField = NULL;
	unsigned short *pUC = NULL;
	float *pFLOAT = NULL;
	int j = 0;

	for (j = 0; j < m_lbFields.GetCount(); j++)
	{
		lpSinField = (LPSIN_FIELD)(m_lbFields.GetItemData(j));

		if (lpSinField)
		{			
			lpSinField->pucValue = NULL;
			lpSinField->puiValue = NULL;
			lpSinField->pfValue = NULL;
			
			if (lpSinField->piPreDefine)
			{
				delete []lpSinField->piPreDefine;
				lpSinField->piPreDefine = NULL;
			}

			delete lpSinField;
			lpSinField = NULL;
		}
	}
	return CDialog::DestroyWindow();
}

void CSinFlagsDlg::OnSelchangeListFields() 
{
	CString strText("");
	int iSelection = 0;
	int j = 0;
	
	iSelection = m_lbFields.GetCurSel();

	if (iSelection != LB_ERR)
	{
		m_lpCurrentField = (LPSIN_FIELD)(m_lbFields.GetItemData(iSelection));
		strText = m_lpCurrentField->szDescription;

		SetDlgItemText (IDC_DESCRIPTION, strText);

		BOOL bPredefined = (m_lpCurrentField->piPreDefine != NULL);
		
		if (bPredefined)
		{
			m_cbValue.ShowWindow (SW_NORMAL);
			m_edValue.ShowWindow (SW_HIDE);
		}
		else
		{
			m_cbValue.ShowWindow (SW_HIDE);
			m_edValue.ShowWindow (SW_NORMAL);
		}
		
		m_cbValue.ResetContent();
				
		switch (m_lpCurrentField->iType)
		{
		case SIN_FIELD_TYPE_UC:
			{
				if (bPredefined)
				{
					for (j = 0; j < m_lpCurrentField->iPredefinedCount; j++)
					{
						strText.Format ("%d", m_lpCurrentField->piPreDefine[j]);
						m_cbValue.AddString (strText);
					}					
				}
				
				strText.Format ("%d", *(m_lpCurrentField->pucValue));				
			}
			break;

		case SIN_FIELD_TYPE_UINT:
			{
				if (bPredefined)
				{
					for (j = 0; j < m_lpCurrentField->iPredefinedCount; j++)
					{
						strText.Format ("%d", m_lpCurrentField->piPreDefine[j]);
						m_cbValue.AddString (strText);
					}					
				}
				strText.Format ("%d", *(m_lpCurrentField->puiValue));								
			}
			break;

		case SIN_FIELD_TYPE_FLOAT:
			{
				strText.Format ("%01.6f", *(m_lpCurrentField->pfValue));
			}
			break;

		default:
			ASSERT (FALSE);
			break;
		}
		SetDlgItemText (IDC_EDIT_VALUE, strText);
		SetDlgItemText (IDC_COMBO_VALUE, strText);
	}
}

LRESULT CSinFlagsDlg::OnChangeColor (WPARAM nType, LPARAM nFlags)
{
	switch (nType)
	{
	case SINFLAGS_WND_LBUTTON_SELECT:
		{
			int r = (int)(m_SinHeader.color[0] * 255.0);
			int g = (int)(m_SinHeader.color[1] * 255.0);
			int b = (int)(m_SinHeader.color[2] * 255.0);

			COLORREF rgbColor (RGB (r, g, b));
			
			CColorDialog dlgColor (rgbColor, CC_FULLOPEN);		
			
			if (dlgColor.DoModal() == IDOK)
			{	
				rgbColor = dlgColor.GetColor();

				float fR = (float)((1.0 * GetRValue (rgbColor)) / 255.0);
				float fG = (float)((1.0 * GetGValue (rgbColor)) / 255.0);
				float fB = (float)((1.0 * GetBValue (rgbColor)) / 255.0);

				m_SinHeader.color[0] = fR;
				m_SinHeader.color[1] = fG;
				m_SinHeader.color[2] = fB;

				UpdateColorFields();
			}	

			m_lbFields.SetFocus();
			Update();
		}
		break;

	default:
		ASSERT (FALSE);
	}
	return 0;
}

void CSinFlagsDlg::UpdateColorFields()
{
	CString strColor("");
	strColor.Format ("%01.6f", m_SinHeader.color[0]);
	SetDlgItemText (IDC_EDIT_COLOR_R, strColor);

	strColor.Format ("%01.6f", m_SinHeader.color[1]);
	SetDlgItemText (IDC_EDIT_COLOR_G, strColor);

	strColor.Format ("%01.6f", m_SinHeader.color[2]);
	SetDlgItemText (IDC_EDIT_COLOR_B, strColor);			
}

void CSinFlagsDlg::OnKillfocusEditName() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_NAME, strText);

	strText = strText.Left (63);
	SetDlgItemText (IDC_EDIT_NAME, strText);

	memset (m_SinHeader.name, 0, 64);
	strcpy_s (m_SinHeader.name, sizeof(m_SinHeader.name), strText.GetBuffer(strText.GetLength()));
}

void CSinFlagsDlg::OnKillfocusEditAnimname() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_ANIMNAME, strText);

	strText = strText.Left (63);
	SetDlgItemText (IDC_EDIT_ANIMNAME, strText);

	memset (m_SinHeader.animname, 0, 64);
	strcpy_s (m_SinHeader.animname, sizeof(m_SinHeader.animname), strText.GetBuffer(strText.GetLength()));
}

void CSinFlagsDlg::ProcessCheck (BOOL bSetting, int iID, int iType, int iValue)
{
	switch (iType)
	{
	case SIN_CHECK_FLAGS:
		if (bSetting)
		{
			CheckDlgButton (iID, (m_SinHeader.flags & iValue) > 0);
		}
		else
		{
			if (IsDlgButtonChecked (iID) > 0)
			{
				m_SinHeader.flags |= iValue;				
			}			
		}
		break;

	case SIN_CHECK_CONTENTS:
		if (bSetting)
		{
			CheckDlgButton (iID, (m_SinHeader.contents & iValue) > 0);
		}
		else
		{
			if (IsDlgButtonChecked (iID) > 0)
			{
				m_SinHeader.contents |= iValue;
			}
		}
		break;

	default:
		ASSERT (FALSE);
	}	
}

void CSinFlagsDlg::ProcessChecks(BOOL bSetting /* = TRUE */)
{
	ProcessCheck (bSetting, IDC_CHECK_LIGHT, SIN_CHECK_FLAGS, SIN_TF_LIGHT);
	ProcessCheck (bSetting, IDC_CHECK_MASKED, SIN_CHECK_FLAGS, SIN_TF_MASKED);
	ProcessCheck (bSetting, IDC_CHECK_SKY, SIN_CHECK_FLAGS, SIN_TF_SKY);
	ProcessCheck (bSetting, IDC_CHECK_WARP, SIN_CHECK_FLAGS, SIN_TF_WARP);
	ProcessCheck (bSetting, IDC_CHECK_NONLIT, SIN_CHECK_FLAGS, SIN_TF_NONLIT);
	ProcessCheck (bSetting, IDC_CHECK_NOFILTER, SIN_CHECK_FLAGS, SIN_TF_NOFILTER);
	ProcessCheck (bSetting, IDC_CHECK_CONVEYOR, SIN_CHECK_FLAGS, SIN_TF_CONVEYOR);
	ProcessCheck (bSetting, IDC_CHECK_NODRAW, SIN_CHECK_FLAGS, SIN_TF_NODRAW);
	ProcessCheck (bSetting, IDC_CHECK_HINT, SIN_CHECK_FLAGS, SIN_TF_HINT);
	ProcessCheck (bSetting, IDC_CHECK_SKIP, SIN_CHECK_FLAGS, SIN_TF_SKIP);	
	ProcessCheck (bSetting, IDC_CHECK_WAVY, SIN_CHECK_FLAGS, SIN_TF_WAVY);
	ProcessCheck (bSetting, IDC_CHECK_RICOCHET, SIN_CHECK_FLAGS, SIN_TF_RICOCHET);
	ProcessCheck (bSetting, IDC_CHECK_PRELIT, SIN_CHECK_FLAGS, SIN_TF_PRELIT);
	ProcessCheck (bSetting, IDC_CHECK_MIRROR, SIN_CHECK_FLAGS, SIN_TF_MIRROR);
	ProcessCheck (bSetting, IDC_CHECK_CONSOLE, SIN_CHECK_FLAGS, SIN_TF_CONSOLE);
	ProcessCheck (bSetting, IDC_CHECK_USECOLOR, SIN_CHECK_FLAGS, SIN_TF_USECOLOR);
	ProcessCheck (bSetting, IDC_CHECK_HWONLY, SIN_CHECK_FLAGS, SIN_TF_HWONLY);
	ProcessCheck (bSetting, IDC_CHECK_DAMAGE, SIN_CHECK_FLAGS, SIN_TF_DAMAGE);
	ProcessCheck (bSetting, IDC_CHECK_WEAK, SIN_CHECK_FLAGS, SIN_TF_WEAK);
	ProcessCheck (bSetting, IDC_CHECK_NORMAL, SIN_CHECK_FLAGS, SIN_TF_NORMAL);
	ProcessCheck (bSetting, IDC_CHECK_ADDBLEND, SIN_CHECK_FLAGS, SIN_TF_ADDBLEND);
	ProcessCheck (bSetting, IDC_CHECK_ENVMAP, SIN_CHECK_FLAGS, SIN_TF_ENVMAP);
	ProcessCheck (bSetting, IDC_CHECK_RANDOMANIMATE, SIN_CHECK_FLAGS, SIN_TF_RANDOMANIMATE);
	ProcessCheck (bSetting, IDC_CHECK_ANIMATE, SIN_CHECK_FLAGS, SIN_TF_ANIMATE);
	ProcessCheck (bSetting, IDC_CHECK_RNDTIME, SIN_CHECK_FLAGS, SIN_TF_RNDTIME);
	ProcessCheck (bSetting, IDC_CHECK_TRANSLATE, SIN_CHECK_FLAGS, SIN_TF_TRANSLATE);
	ProcessCheck (bSetting, IDC_CHECK_NOMERGE, SIN_CHECK_FLAGS, SIN_TF_NOMERGE);

	ProcessCheck (bSetting, IDC_CHECK_SOLID, SIN_CHECK_CONTENTS, SIN_TC_SOLID);
	ProcessCheck (bSetting, IDC_CHECK_WINDOW, SIN_CHECK_CONTENTS, SIN_TC_WINDOW);
	ProcessCheck (bSetting, IDC_CHECK_FENCE, SIN_CHECK_CONTENTS, SIN_TC_FENCE);
	ProcessCheck (bSetting, IDC_CHECK_LAVA, SIN_CHECK_CONTENTS, SIN_TC_LAVA);
	ProcessCheck (bSetting, IDC_CHECK_SLIME, SIN_CHECK_CONTENTS, SIN_TC_SLIME);
	ProcessCheck (bSetting, IDC_CHECK_WATER, SIN_CHECK_CONTENTS, SIN_TC_WATER);
	ProcessCheck (bSetting, IDC_CHECK_MIST, SIN_CHECK_CONTENTS, SIN_TC_MIST);
	ProcessCheck (bSetting, IDC_CHECK_AREAPORTAL, SIN_CHECK_CONTENTS, SIN_TC_AREAPORTAL);
	ProcessCheck (bSetting, IDC_CHECK_PLAYERCLIP, SIN_CHECK_CONTENTS, SIN_TC_PLAYERCLIP);
	ProcessCheck (bSetting, IDC_CHECK_MONSTERCLIP, SIN_CHECK_CONTENTS, SIN_TC_MONSTERCLIP);
	ProcessCheck (bSetting, IDC_CHECK_CURRENT0, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_0);
	ProcessCheck (bSetting, IDC_CHECK_CURRENT90, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_90);
	ProcessCheck (bSetting, IDC_CHECK_CURRENT180, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_180);
	ProcessCheck (bSetting, IDC_CHECK_CURRENT270, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_270);
	ProcessCheck (bSetting, IDC_CHECK_CURRENTUP, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_UP);
	ProcessCheck (bSetting, IDC_CHECK_CURRENTDN, SIN_CHECK_CONTENTS, SIN_TC_CURRENT_DN);
	ProcessCheck (bSetting, IDC_CHECK_ORIGIN, SIN_CHECK_CONTENTS, SIN_TC_ORIGIN);
	ProcessCheck (bSetting, IDC_CHECK_DETAIL, SIN_CHECK_CONTENTS, SIN_TC_DETAIL);
	ProcessCheck (bSetting, IDC_CHECK_SOLID, SIN_CHECK_CONTENTS, SIN_TC_SOLID);
	ProcessCheck (bSetting, IDC_CHECK_TRANSLUCENT, SIN_CHECK_CONTENTS, SIN_TC_TRANSLUCENT);
	ProcessCheck (bSetting, IDC_CHECK_LADDER, SIN_CHECK_CONTENTS, SIN_TC_LADDER);
}

void CSinFlagsDlg::OnOK() 
{
	m_SinHeader.flags = 0;
	m_SinHeader.contents = 0;

	ProcessChecks(FALSE);
	
	int iSurfaceSelection = m_cbSurfaceTypes.GetCurSel();

	if (iSurfaceSelection != CB_ERR)
	{
		int iSurfaceType = (int)(m_cbSurfaceTypes.GetItemData (iSurfaceSelection));
		m_SinHeader.flags |= iSurfaceType;
	}
	
	CDialog::OnOK();
}

void CSinFlagsDlg::OnKillfocusComboValue() 
{
	CString strText("");
	GetDlgItemText (IDC_COMBO_VALUE, strText);

	switch (m_lpCurrentField->iType)
	{
	case SIN_FIELD_TYPE_UC:
		{
			*(m_lpCurrentField->pucValue) = (unsigned short)(atoi (strText));
			strText.Format ("%d", *(m_lpCurrentField->pucValue));
		}
		break;

	case SIN_FIELD_TYPE_UINT:
		{
			*(m_lpCurrentField->puiValue) = (unsigned short)(atoi (strText));
			strText.Format ("%d", *(m_lpCurrentField->puiValue));
		}
		break;

	case SIN_FIELD_TYPE_FLOAT:
		{
			*(m_lpCurrentField->pfValue) = (float)(atof (strText));
			strText.Format ("%01.6f", *(m_lpCurrentField->pfValue));
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}
	SetDlgItemText (IDC_COMBO_VALUE, strText);	
}


void CSinFlagsDlg::OnKillfocusEditValue() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_VALUE, strText);

	switch (m_lpCurrentField->iType)
	{
	case SIN_FIELD_TYPE_UC:
		{
			*(m_lpCurrentField->pucValue) = (unsigned short)(atoi (strText));
			strText.Format ("%d", *(m_lpCurrentField->pucValue));
		}
		break;

	case SIN_FIELD_TYPE_UINT:
		{
			*(m_lpCurrentField->puiValue) = (unsigned short)(atoi (strText));
			strText.Format ("%d", *(m_lpCurrentField->puiValue));
		}
		break;

	case SIN_FIELD_TYPE_FLOAT:
		{
			*(m_lpCurrentField->pfValue) = (float)(atof (strText));
			strText.Format ("%01.6f", *(m_lpCurrentField->pfValue));
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}
	SetDlgItemText (IDC_EDIT_VALUE, strText);
}

void CSinFlagsDlg::OnKillfocusEditColorB() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_COLOR_B, strText);
	
	m_SinHeader.color[2] = (float)(atof (strText));

	strText.Format ("%01.6f", m_SinHeader.color[2]);
	
	SetDlgItemText (IDC_EDIT_COLOR_B, strText);
	Update();
}

void CSinFlagsDlg::OnKillfocusEditColorG() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_COLOR_G, strText);
	
	m_SinHeader.color[1] = (float)(atof (strText));

	strText.Format ("%01.6f", m_SinHeader.color[1]);
	
	SetDlgItemText (IDC_EDIT_COLOR_G, strText);	
	Update();
}

void CSinFlagsDlg::OnKillfocusEditColorR() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_COLOR_R, strText);
	
	m_SinHeader.color[0] = (float)(atof (strText));

	strText.Format ("%01.6f", m_SinHeader.color[0]);
	
	SetDlgItemText (IDC_EDIT_COLOR_R, strText);
	Update();
}


void CSinFlagsDlg::OnButtonAuto() 
{
	ASSERT (m_pbyImageData);
	ASSERT (m_pPalette);

	BYTE byPalette[768];
	m_pPalette->GetPalette (byPalette, 256);

	CalcImageColor256 (m_SinHeader.width, m_SinHeader.height, m_pbyImageData, byPalette, &(m_SinHeader.color[0]), &(m_SinHeader.color[1]), &(m_SinHeader.color[2]), TRUE);

	UpdateColorFields();
	Update();
}


void CSinFlagsDlg::OnButtonDefault() 
{
	int iReturn = AfxMessageBox ("Are you sure you want to reset all flags to their default values?", MB_YESNO);

	if (iReturn == IDYES)
	{
		m_SinHeader.flags = 0;
		m_SinHeader.contents = 0;
		m_SinHeader.value = 0;
		m_SinHeader.direct = 0;	
		m_SinHeader.nonlit = (float)0.5;
		m_SinHeader.friction = (float)1.0;
		m_SinHeader.animtime = (float)0.2;
		m_SinHeader.directangle = 0;
		m_SinHeader.trans_angle = 0;
		m_SinHeader.directstyle = (float)0.0;
		m_SinHeader.translucence = (float)0.0;	
		m_SinHeader.restitution = (float)0.0;
		m_SinHeader.trans_mag = 0;		
		OnButtonAuto();
		ProcessChecks(TRUE);
		OnSelchangeListFields();
	}	
}
