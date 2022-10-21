// DecalToolbar.cpp : implementation file
//


// NOTE!!  This isn't used any more.  Look to ToolSettingsToolbar.cpp/.h for the new class

#include "stdafx.h"
#include "Layer.h"
#include "wally.h"
#include "DecalToolbar.h"
#include "WADList.h"
#include "DibSection.h"
#include "MouseWnd.h"
#include "MiscFunctions.h"
#include "ToolSettingsPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDecalToolbar dialog


CDecalToolbar::CDecalToolbar(CWnd* pParent /*=NULL*/)
//: CDialog(CDecalToolbar::IDD, pParent)	
{
	//{{AFX_DATA_INIT(CDecalToolbar)
	//}}AFX_DATA_INIT

	m_bFirstTimeTab     = TRUE;
	m_bFirstTimePreview = TRUE;
	m_bFirstTimeMouse	= TRUE;
	m_bFirstTimeHiding	= TRUE;

	m_wndPreview.SetType( PP_TYPE_DECAL);
}


void CDecalToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecalToolbar)
	DDX_Control(pDX, IDC_DT2_CT_STATIC, m_scColorTolerance);
	DDX_Control(pDX, IDC_DT2_COLOR_TOLERANCE_SPIN, m_spnColorTolerance);
	DDX_Control(pDX, IDC_DT2_COLOR_TOLERANCE, m_edColorTolerance);
	DDX_Control(pDX, IDC_DT1_STATUS, m_scStatus);
	DDX_Control(pDX, IDC_DT1_LIST_IMAGES, m_lbImages);
	DDX_Control(pDX, IDC_DT1_COMBO_WAD, m_cbWAD);	
	DDX_Control(pDX, IDC_DT1_BUTTON_PREVIEW, m_btnPreview);	
	DDX_Control(pDX, IDC_DT_TAB, m_TabCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDecalToolbar, CDialogBar)
	//{{AFX_MSG_MAP(CDecalToolbar)
	ON_REGISTERED_MESSAGE(WM_DECALLISTBOX_CUSTOM, OnDecalListBoxCustomMessage)
	ON_REGISTERED_MESSAGE(WM_MOUSE_WND_CUSTOM, OnMouseWndCustomMessage)
	ON_WM_PAINT()
	ON_NOTIFY(TCN_SELCHANGE, IDC_DT_TAB, OnSelchangeDtTab)
	ON_CBN_SETFOCUS(IDC_DT1_COMBO_WAD, OnSetfocusDt1ComboWad)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_DT1_COMBO_WAD, OnSelchangeDt1ComboWad)
	ON_LBN_SELCHANGE(IDC_DT1_LIST_IMAGES, OnSelchangeDt1ListImages)
	ON_NOTIFY(TCN_SELCHANGING, IDC_DT_TAB, OnSelchangingDtTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecalToolbar message handlers

void CDecalToolbar::FirstTimeInit()
{
	if (m_bFirstTimeTab)
	{
		// neal - must call this manually (because this isn't a REAL dialog box)
		UpdateData( FALSE);

		CWnd *pWnd = GetDlgItem (IDC_DT_TAB);
		
		if (pWnd)	
		{
			m_bFirstTimeTab = FALSE;
			
			PSTR pszTabItems[] =
			{
				"Images",
				"Settings",
				NULL
			};

			TC_ITEM tcItem;

			for(int i = 0; pszTabItems[i] != NULL; i++)
			{
				tcItem.mask = TCIF_TEXT;
				tcItem.pszText = pszTabItems[i];
				tcItem.cchTextMax = strlen(pszTabItems[i]);
				m_TabCtrl.InsertItem(i,&tcItem);
			}

			m_TabCtrl.AttachControlToTab(&m_btnPreview, 0);			
			m_TabCtrl.AttachControlToTab(&m_cbWAD, 0);
			m_TabCtrl.AttachControlToTab(&m_lbImages, 0);
			m_TabCtrl.AttachControlToTab(&m_wndPreview, 0);
			m_TabCtrl.AttachControlToTab(&m_wndMouseButton, 0);
			m_TabCtrl.AttachControlToTab(&m_scStatus, 0);

			m_TabCtrl.AttachControlToTab(&m_scColorTolerance, 1);
			m_TabCtrl.AttachControlToTab(&m_spnColorTolerance, 1);
			m_TabCtrl.AttachControlToTab(&m_edColorTolerance, 1);
			
			// initialize tab to first page.
			m_TabCtrl.SetCurSel(0);

			SetDlgItemText (IDC_DT1_STATUS, "");

			OnSetfocusDt1ComboWad();
		}
	}

	int iCurSel = m_TabCtrl.GetCurSel();

	switch (iCurSel)
	{
	case -1:
		{
			// No tab selected?
		}
		break;

	case 0:
		{
			CRect Rect, rDlg;
			GetClientRect( &rDlg);
			ClientToScreen( rDlg);
			
			CWnd* pWnd = GetDlgItem( IDC_DT1_BUTTON_PREVIEW);

			if (pWnd)
			{				
				pWnd->GetWindowRect( &Rect);
				Rect.OffsetRect( -rDlg.left, -rDlg.top);
				pWnd->ShowWindow( SW_HIDE);

				if (m_bFirstTimePreview)
				{
					m_wndPreview.Create( NULL, NULL, 
						WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);

					m_wndPreview.SetNonImageText ("");
					m_wndPreview.SetNonImageLocation (12, 50);
					m_wndPreview.SetPreviewWidth (128);
					m_wndPreview.SetPreviewHeight (128);
					m_wndPreview.EnableWindow (false);
					
					m_bFirstTimePreview = FALSE;
				}
			}

			pWnd = GetDlgItem( IDC_DT1_BUTTON_MOUSE_GUI);
				
			if (pWnd)
			{				
				pWnd->GetWindowRect( &Rect);
				Rect.OffsetRect( -rDlg.left, -rDlg.top);
				pWnd->ShowWindow( SW_HIDE);

				if (m_bFirstTimeMouse)
				{
					m_wndMouseButton.Create( NULL, NULL, 
						WS_CHILD | WS_VISIBLE, Rect, this, 0, NULL);
			
					m_bFirstTimeMouse = FALSE;					
				}
			}			
		}
		break;

	case 1:
		{
			CString strText("");
			
			strText.Format ("%lu", g_iColorTolerance);
			SetDlgItemText (IDC_DT2_COLOR_TOLERANCE, strText);

			UDACCEL Spinaccel[4];
			
			Spinaccel[0].nSec = 0;
			Spinaccel[0].nInc = 1;
			Spinaccel[1].nSec = 1;
			Spinaccel[1].nInc = 5;
			Spinaccel[2].nSec = 2;
			Spinaccel[2].nInc = 20;
			Spinaccel[3].nSec = 3;
			Spinaccel[3].nInc = 50;
			m_spnColorTolerance.SetAccel( 4, &Spinaccel[0]);	
			m_spnColorTolerance.SetRange( 0, 255);
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}

	EnableControls (FALSE);
	Update();
}

void CDecalToolbar::Update()
{
	CWnd* pWnd = NULL;

	if (m_bFirstTimeHiding)
	{		
		pWnd = GetDlgItem( IDC_DT1_BUTTON_PREVIEW);	
	
		if (pWnd)
		{		
			if (pWnd->m_hWnd)
				pWnd->InvalidateRect( NULL, FALSE);
			m_bFirstTimeHiding = FALSE;
		}
		else
		{
			ASSERT( FALSE);
		}
	}

	if (m_wndPreview.m_hWnd)
	{
		m_wndPreview.InvalidateRect( NULL, FALSE);
	}

	pWnd = GetDlgItem( IDC_DT1_BUTTON_MOUSE_GUI);
	if (pWnd)
	{		
		if (pWnd->m_hWnd)
			pWnd->InvalidateRect( NULL, FALSE);
	}
	else
	{
		ASSERT( FALSE);
	}

	if (m_wndMouseButton.m_hWnd)
	{
		m_wndMouseButton.InvalidateRect( NULL, FALSE);
	}

}

void CDecalToolbar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bFirstTimeTab)
	{
		FirstTimeInit();
	}

	// Do not call CDialogBar::OnPaint() for painting messages
}

void CDecalToolbar::OnSelchangeDtTab(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
	FirstTimeInit();
}

void CDecalToolbar::OnSelchangingDtTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}


void CDecalToolbar::OnSetfocusDt1ComboWad() 
{	
	if (m_bFirstTimeTab)
		FirstTimeInit();

	struct _finddata_t c_file;
	long hfile;

	CString strWildCard ("");
	CString strFileName ("");

	// neal - WinNT bugfix?  g_szAppDirectory ended in a '\', so there were two
	// Ty-  nah, just my own bug, as I couldn't ever test any of this stuff :)
	
	strWildCard.Format ("%s\\*.wad", g_strDecalDirectory);

	int iListIndex  = 0;
	int iComboIndex = 0;
	int iCurSel		= m_cbWAD.GetCurSel ();
	bool bAtLeastOne = false;

	// neal - bugfix - prevents multiple copies of items in list
	m_cbWAD.ResetContent();
	m_strWADList.RemoveAll();

	if ((hfile = _findfirst(strWildCard, &c_file)) != -1L)	
	{		
		bAtLeastOne = true;

		strFileName.Format ("%s\\%s", g_strDecalDirectory, c_file.name);
		iListIndex = m_strWADList.Add (strFileName);
		iComboIndex = m_cbWAD.AddString (GetRawFileName (c_file.name));
		m_cbWAD.SetItemData (iComboIndex, iListIndex);

		// Keep looking for more files that match
		while (_findnext(hfile, &c_file) == 0)
		{
			strFileName.Format ("%s\\%s", g_strDecalDirectory, c_file.name);
			iListIndex = m_strWADList.Add (strFileName);
			iComboIndex = m_cbWAD.AddString (GetRawFileName (c_file.name));
			m_cbWAD.SetItemData (iComboIndex, iListIndex);
		}
	}

	if (bAtLeastOne)
	{
		if (iCurSel != CB_ERR)
		{
			iCurSel = min (iCurSel, m_cbWAD.GetCount() - 1);
			iCurSel = max (iCurSel, 0);
		}
		else
		{
			iCurSel = 0;
		}
		m_cbWAD.SetCurSel (iCurSel);			
	}



	OnSelchangeDt1ComboWad();	
}

void CDecalToolbar::OnSelchangeDt1ComboWad() 
{
	int iCurSel		= m_cbWAD.GetCurSel ();
	int iListIndex	= m_cbWAD.GetItemData (iCurSel);
	int iItemAdded = 0;
	CString strFileName("");
	CWADItem *pItem = NULL;
	m_lbImages.ResetContent();
	m_ihHelper.ResetContent();
	bool bAtLeastOne = false;
	
	if (iCurSel != CB_ERR)
	{
		strFileName = m_strWADList.GetAt (iListIndex);		
		m_ihHelper.LoadImage (strFileName, IH_LOAD_ONLYPACKAGE);

		if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (m_ihHelper.GetErrorText(), MB_ICONSTOP);

			// Clear out the selection from the combo box
			m_cbWAD.SetCurSel (-1);
			return;
		}

		pItem = m_ihHelper.GetFirstImage();
		bAtLeastOne = (pItem != NULL);

		while (pItem)
		{
			iItemAdded = m_lbImages.AddString (pItem->GetName());
			m_lbImages.SetItemData (iItemAdded, (DWORD)pItem);
			pItem = m_ihHelper.GetNextImage();
		}

		if (bAtLeastOne)
		{
			m_lbImages.SetCurSel(0);
			OnSelchangeDt1ListImages();
		}
	}	
}


void CDecalToolbar::OnSize( UINT uType, int cx, int cy) 
{
	CDialogBar::OnSize( uType, cx, cy);

	// neal - TODO: add resizing - the problem is getting the
	// handle of the popup and changing its style to be resizable

	// resize the toolbar

//	if (m_TabCtrl.m_hWnd)
//	{
//		m_TabCtrl.SetWindowPos( NULL, 0, 0, cx, cy, 
//			SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
//	}
}


void CDecalToolbar::OnSelchangeDt1ListImages() 
{
	int iCurSel = m_lbImages.GetCurSel();
	CWADItem *pItem = NULL;

	if (iCurSel != LB_ERR)
	{
		pItem = (CWADItem *)(m_lbImages.GetItemData(iCurSel));
		m_wndPreview.InitImage (pItem->GetWidth(), pItem->GetHeight(), pItem->GetBits(), pItem->GetPalette());
		
		CString strText ("");
		strText.Format ("%d x %d", pItem->GetWidth(), pItem->GetHeight());

		SetStatus (strText, 1);
		pItem = NULL;
	}
	else
	{
		m_wndPreview.InitImage (0, 0, NULL, NULL);
	}
	Update();	
}


void CDecalToolbar::OnDecalListBoxCustomMessage(UINT nType, UINT nFlags)
{
	switch (nType)
	{
	case DECALLISTBOX_SELCHANGE:
		OnSelchangeDt1ListImages();
		break;

	case DECALLISTBOX_ENTER:
		LoadImageToTool (LEFT_BUTTON);
		break;

	default:
		ASSERT (false);		// Unhandled type?
		break;
	}	
}

void CDecalToolbar::OnMouseWndCustomMessage (UINT nType, UINT nFlags)
{
	switch (nType)
	{
	case MOUSE_WND_LBUTTON_SELECT:
		LoadImageToTool (LEFT_BUTTON);
		break;

	case MOUSE_WND_RBUTTON_SELECT:
		LoadImageToTool (RIGHT_BUTTON);
		break;

	default:
		ASSERT (false);
		break;
	}
}

bool CDecalToolbar::GetCurrentSelection()
{
	int iCurSel = m_lbImages.GetCurSel();
	CWADItem *pItem = NULL;

	if (iCurSel != LB_ERR)
	{
		m_pWADItem = (CWADItem *)(m_lbImages.GetItemData(iCurSel));
		return true;
	}
	else
	{
		m_pWADItem = NULL;
		return false;
	}

	return false;
}


void CDecalToolbar::LoadImageToTool (int iMouseButton)
{
	if (!GetCurrentSelection())
	{
		return;
	}

	int w = m_pWADItem->GetWidth();
	int h = m_pWADItem->GetHeight();

	CDibSection dsImage;
	dsImage.Init( w, h, 8, m_pWADItem->GetPalette());
	dsImage.SetRawBits( m_pWADItem->GetBits());
	dsImage.WriteToClipboard( this);

	switch (g_iCurrentTool)
	{
	case EDIT_MODE_CLONE:
		g_CloneSourceLayerInfo.LoadFromClipboard( FALSE, this);
		break;

	case EDIT_MODE_RIVETS:
		g_RivetToolLayerInfo.LoadFromClipboard( FALSE, this);
		break;

	case EDIT_MODE_BULLET_HOLES:
		
		if (iMouseButton == LEFT_BUTTON)
		{
			g_LeftBulletLayerInfo.LoadFromClipboard( FALSE, this);
		}
		else
		{
			g_RightBulletLayerInfo.LoadFromClipboard( FALSE, this);
		}
		break;

	case EDIT_MODE_DECAL:
		if (iMouseButton == LEFT_BUTTON)
		{
			g_LeftDecalToolLayerInfo.LoadFromClipboard( FALSE, this);
		}
		else
		{
			g_RightDecalToolLayerInfo.LoadFromClipboard( FALSE, this);
		}
		break;

	case EDIT_MODE_PATTERNED_PAINT:
		if (iMouseButton == LEFT_BUTTON)
		{
			g_LeftPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
		}
		else
		{
			g_RightPatternToolLayerInfo.LoadFromClipboard( FALSE, this);
		}
		break;

	default:
		ASSERT (false);
		break;
	
	}	
}


void CDecalToolbar::EnableControls (bool bState /* = TRUE */)
{
	m_bEnableControls = bState;

	CWnd *pWnd = NULL;

	pWnd = GetDlgItem (IDC_DT1_LIST_IMAGES);
	if (pWnd)
	{
		pWnd->EnableWindow (m_bEnableControls);
	}
	
	pWnd = GetDlgItem (IDC_DT1_COMBO_WAD);
	if (pWnd)
	{
		pWnd->EnableWindow (m_bEnableControls);
	}

	pWnd = GetDlgItem (IDC_DT1_BUTTON_PREVIEW);
	if (pWnd)
	{
		pWnd->EnableWindow (m_bEnableControls);
	}	
	m_wndMouseButton.EnableWindow (m_bEnableControls);	
}

void CDecalToolbar::EnableRightButton (bool bEnable /* = TRUE */)
{
	if (bEnable)
	{
		m_wndMouseButton.RemoveButtonState (MOUSE_STATE_RBUTTON_DISABLED);
	}
	else
	{
		m_wndMouseButton.SetButtonState (MOUSE_STATE_RBUTTON_DISABLED);
	}
	m_wndMouseButton.Update();
}

void CDecalToolbar::SetStatus (LPCTSTR szText, int iPageNumber /* = 1 */)
{
	if (m_bFirstTimeTab)
	{
		FirstTimeInit();
	}

	switch (iPageNumber)
	{
	case 1:
		SetDlgItemText (IDC_DT1_STATUS, szText);
		break;

	default:
		ASSERT (false);
		break;
	}
}

