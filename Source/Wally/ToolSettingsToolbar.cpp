// ToolSettingsToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "Layer.h"
#include "wally.h"
#include "ToolSettingsToolbar.h"
#include "WADList.h"
#include "DibSection.h"
#include "MouseWnd.h"
#include "MiscFunctions.h"
#include "WallyView.h"
#include "WndList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolSettingsToolbar dialog

extern CWallyApp theApp;


CToolSettingsToolbar::CToolSettingsToolbar(CWnd* pParent /*=NULL*/)
// : CDialogBar(CToolSettingsToolbar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolSettingsToolbar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_Page1.Initialize (this, &m_TabCtrl, "Images", IDD_TOOL_SETTINGS_PAGE1, 0);
	m_Page2.Initialize (this, &m_TabCtrl, "Settings", IDD_TOOL_SETTINGS_PAGE2, 1);

	m_bFirstTimeTab     = TRUE;
	m_bFirstTimePreview = TRUE;
	m_bFirstTimeMouse	= TRUE;
	m_bFirstTimeHiding	= TRUE;
	m_bFirstTimeWADList = TRUE;

	m_wndPreview.SetType( PP_TYPE_DECAL);
}


void CToolSettingsToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolSettingsToolbar)
	DDX_Control(pDX, IDC_TST_TAB, m_TabCtrl);
	DDX_Control(pDX, IDC_TST2_COLOR_TOLERANCE_SPIN, m_spnTST2_ColorTolerance);
	DDX_Control(pDX, IDC_TST1_LIST_IMAGES, m_lbTST1_Images);
	DDX_Control(pDX, IDC_TST1_COMBO_WAD, m_cbTST1_WAD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolSettingsToolbar, CDialogBar)
	//{{AFX_MSG_MAP(CToolSettingsToolbar)
	ON_REGISTERED_MESSAGE(WM_DECALLISTBOX_CUSTOM, OnDecalListBoxCustomMessage)
	ON_REGISTERED_MESSAGE(WM_MOUSE_WND_CUSTOM, OnMouseWndCustomMessage)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TST_TAB, OnSelchangeTstTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TST_TAB, OnSelchangingTstTab)
	ON_CBN_SETFOCUS(IDC_TST1_COMBO_WAD, OnSetfocusTst1ComboWad)
	ON_LBN_SELCHANGE(IDC_TST1_LIST_IMAGES, OnSelchangeTst1ListImages)
	ON_EN_KILLFOCUS(IDC_TST2_COLOR_TOLERANCE, OnKillfocusTst2ColorTolerance)
	ON_CBN_SELCHANGE(IDC_TST2_COMBO_SIZE, OnSelchangeTst2ComboSize)
	ON_CBN_SELCHANGE(IDC_TST2_COMBO_SHAPE, OnSelchangeTst2ComboShape)
	ON_CBN_SELCHANGE(IDC_TST2_COMBO_AMOUNT, OnSelchangeTst2ComboAmount)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_TST2_COMBO_ZOOM, OnSelchangeTst2ComboZoom)
	ON_UPDATE_COMMAND_UI(IDC_TST2_COMBO_ZOOM, OnUpdateTst2ComboZoom)
	ON_CBN_SELENDOK(IDC_TST1_COMBO_WAD, OnSelendokTst1ComboWad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolSettingsToolbar message handlers

void CToolSettingsToolbar::FirstTimeInit()
{
	if (m_bFirstTimeTab)
	{
		// neal - must call this manually (because this isn't a REAL dialog box)
		UpdateData( FALSE);

		CWnd *pWnd = GetDlgItem (IDC_TST_TAB);
		
		if (pWnd)	
		{
			m_bFirstTimeTab = FALSE;
			
			if (!m_Page1.AddToTabCtrl())
			{
				ASSERT (FALSE);
			}

			if (!m_Page2.AddToTabCtrl())
			{
				ASSERT (FALSE);
			}

			m_TabCtrl.NeverShowControl (IDC_TST1_BUTTON_PREVIEW);
			m_TabCtrl.NeverShowControl (IDC_TST1_BUTTON_MOUSE_GUI);

			m_TabCtrl.AttachControlToTab ((CWnd *)(&m_wndPreview), 0);
			m_TabCtrl.AttachControlToTab ((CWnd *)(&m_wndMouseButton), 0);
			
			SetDlgItemText (IDC_TST1_STATUS, "");

			OnSetfocusTst1ComboWad();
		}
	}

	CRect Rect, rDlg;
	GetClientRect( &rDlg);
	ClientToScreen( rDlg);
	
	CWnd* pWnd = GetDlgItem( IDC_TST1_BUTTON_PREVIEW);

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

	pWnd = GetDlgItem( IDC_TST1_BUTTON_MOUSE_GUI);
		
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

	CComboBox* pCombo  = (CComboBox* )(GetDlgItem( IDC_TST2_COMBO_ZOOM));

	CString strZoom("");
	int iItemAdded = 0;
	int j = 0;
	
	for (j = MIN_ZOOM; j < -1; j++)
	{
		strZoom.Format ("1:%d", abs(j));
		iItemAdded = pCombo->AddString (strZoom);
		pCombo->SetItemData (iItemAdded, j);
	}

	for (j = 1; j <= MAX_ZOOM; j++)
	{
		strZoom.Format ("%d:1", j);
		iItemAdded = pCombo->AddString (strZoom);
		pCombo->SetItemData (iItemAdded, j);

		if (j == 1)
		{
			pCombo->SetCurSel (iItemAdded);
		}
	}

	EnableControls (FALSE);	
	
	// Set tab to last used page.
	g_iLastToolSettingsTab = min (g_iLastToolSettingsTab, 1);
	g_iLastToolSettingsTab = max (g_iLastToolSettingsTab, 0);
	m_TabCtrl.SetCurSel(g_iLastToolSettingsTab);

	UpdateDlgItems();
	Update();
}

void CToolSettingsToolbar::UpdateDlgItems()
{
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
		}
		break;

	case 1:
		{
			CString strText("");
			
			strText.Format ("%lu", g_iColorTolerance);
			SetDlgItemText (IDC_TST2_COLOR_TOLERANCE, strText);

			UDACCEL Spinaccel[4];
			
			Spinaccel[0].nSec = 0;
			Spinaccel[0].nInc = 1;
			Spinaccel[1].nSec = 1;
			Spinaccel[1].nInc = 5;
			Spinaccel[2].nSec = 2;
			Spinaccel[2].nInc = 20;
			Spinaccel[3].nSec = 3;
			Spinaccel[3].nInc = 50;
			m_spnTST2_ColorTolerance.SetAccel( 4, &Spinaccel[0]);	
			m_spnTST2_ColorTolerance.SetRange( 0, 255);
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}
}

void CToolSettingsToolbar::RetrieveDlgItems()
{
	if (m_TabCtrl.m_hWnd != NULL)	// Neal - was asserting with no image loaded
	{
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
				
			}
			break;

		case 1:
			{
				CString strText("");			
				
				GetDlgItemText (IDC_TST2_COLOR_TOLERANCE, strText);

				g_iColorTolerance = atoi (strText);
				g_iColorTolerance = min (g_iColorTolerance, 255);
				g_iColorTolerance = max (g_iColorTolerance, 0);			
			}
			break;

		default:
			ASSERT (FALSE);
			break;
		}
	}
}

void CToolSettingsToolbar::Update(BOOL bToolSwitch /* = FALSE */)
{
/*	CWnd* pWnd = NULL;

	if (m_bFirstTimeHiding)
	{		
		pWnd = GetDlgItem( IDC_TST1_BUTTON_PREVIEW);	
	
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

	pWnd = GetDlgItem( IDC_TST1_BUTTON_MOUSE_GUI);
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
	}*/

	BOOL       bEnable = FALSE;
	CComboBox* pCombo  = (CComboBox* )(GetDlgItem( IDC_TST2_COMBO_SIZE));

	if (pCombo)
	{
		bEnable = ToolHasWidth( g_iCurrentTool) && 
				(GetDrawingMode( g_iCurrentTool) != MODE_SOLID_RECT);

		pCombo->EnableWindow( bEnable);

		//if (bToolSwitch)
		{
//			pCombo->LockWindowUpdate();
			pCombo->ResetContent();

			if (bEnable)
			{
				if (g_iCurrentTool == EDIT_MODE_SELECTION)
				{
					pCombo->AddString( "Opaque Bkgnd");
					pCombo->AddString( "Invisible Bkgnd");
				}
				else if (g_iCurrentTool == EDIT_MODE_RIVETS)
				{
					pCombo->AddString( "8 Pixel Gap");
					pCombo->AddString( "16 Pixel Gap");
					pCombo->AddString( "32 Pixel Gap");
					pCombo->AddString( "64 Pixel Gap");
					pCombo->AddString( "128 Pixel Gap");
				}
				else
				{
					pCombo->AddString( "1 Pixel");
					pCombo->AddString( "2 Pixels");
					pCombo->AddString( "3 Pixels");
					pCombo->AddString( "4 Pixels");
					pCombo->AddString( "5 Pixels");
					pCombo->AddString( "6 Pixels");
					pCombo->AddString( "8 Pixels");
					pCombo->AddString( "12 Pixels");
					pCombo->AddString( "16 Pixels");
					pCombo->AddString( "32 Pixels");
					pCombo->AddString( "48 Pixels");
					pCombo->AddString( "64 Pixels");
				}
			}
		}
		pCombo->SetCurSel( BrushSizeToIndex( GetFreehandToolWidth()));
//		pCombo->UnlockWindowUpdate();
	}
	else
	{
		ASSERT( FALSE);
	}

	pCombo = (CComboBox* )(GetDlgItem( IDC_TST2_COMBO_SHAPE));
	if (pCombo)
	{
		bEnable = ToolHasShape( g_iCurrentTool);
		pCombo->EnableWindow( bEnable);

		if (bToolSwitch)
		{
//			pCombo->LockWindowUpdate();
			pCombo->ResetContent();

			if (bEnable)
			{
				pCombo->AddString( "Square");
				pCombo->AddString( "Round");
				pCombo->AddString( "Diamond");
			}
		}
		pCombo->SetCurSel( BrushShapeToIndex( g_iBrushShape));
//		pCombo->UnlockWindowUpdate();
	}
	else
	{
		ASSERT( FALSE);
	}

	pCombo = (CComboBox* )(GetDlgItem( IDC_TST2_COMBO_AMOUNT));
	if (pCombo)
	{
		bEnable = ToolHasAmount( g_iCurrentTool);
		pCombo->EnableWindow( bEnable);

		if (bToolSwitch)
		{
			pCombo->ResetContent();

			if (bEnable)
			{
				pCombo->AddString( "Least");
				pCombo->AddString( "A Little Bit");
				pCombo->AddString( "Medium");
				pCombo->AddString( "More");
				pCombo->AddString( "Most");
			}
		}
		pCombo->SetCurSel( ToolToAmountIndex( g_iCurrentTool));
	}
	else
	{
		ASSERT( FALSE);
	}
}

void CToolSettingsToolbar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bFirstTimeTab)
	{
		FirstTimeInit();
	}
}

BOOL CToolSettingsToolbar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CToolSettingsToolbar::OnSelchangeTstTab(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	*pResult = 0;
	UpdateDlgItems();

	int iCurSel = m_TabCtrl.GetCurSel();

	if (iCurSel != CB_ERR)
	{
		g_iLastToolSettingsTab = iCurSel;
	}
}

void CToolSettingsToolbar::OnSelchangingTstTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	RetrieveDlgItems();
	*pResult = 0;
}


void CToolSettingsToolbar::OnSetfocusTst1ComboWad() 
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
	int iCurSel		= m_cbTST1_WAD.GetCurSel ();
	bool bAtLeastOne = false;

	// neal - bugfix - prevents multiple copies of items in list
	m_cbTST1_WAD.ResetContent();
	m_saWADList.RemoveAll();

	if ((hfile = _findfirst(strWildCard, &c_file)) != -1L)	
	{		
		bAtLeastOne = true;

		strFileName.Format ("%s\\%s", g_strDecalDirectory, c_file.name);
		iListIndex = m_saWADList.Add (strFileName);
		iComboIndex = m_cbTST1_WAD.AddString (GetRawFileName (c_file.name));
		m_cbTST1_WAD.SetItemData (iComboIndex, iListIndex);

		// Keep looking for more files that match
		while (_findnext(hfile, &c_file) == 0)
		{
			strFileName.Format ("%s\\%s", g_strDecalDirectory, c_file.name);
			iListIndex = m_saWADList.Add (strFileName);
			iComboIndex = m_cbTST1_WAD.AddString (GetRawFileName (c_file.name));
			m_cbTST1_WAD.SetItemData (iComboIndex, iListIndex);
		}
	}

	if (bAtLeastOne)
	{
		if (iCurSel != CB_ERR)
		{
			iCurSel = min (iCurSel, m_cbTST1_WAD.GetCount() - 1);
			iCurSel = max (iCurSel, 0);
		}
		else
		{
			iCurSel = 0;
		}
		m_cbTST1_WAD.SetCurSel (iCurSel);
	}

	if (m_bFirstTimeWADList)
	{
		OnSelendokTst1ComboWad();
		m_bFirstTimeWADList = FALSE;
	}
}

void CToolSettingsToolbar::OnSelendokTst1ComboWad() 
{
	int iCurSel		= m_cbTST1_WAD.GetCurSel ();
	int iListIndex	= m_cbTST1_WAD.GetItemData (iCurSel);
	int iItemAdded = 0;
	CString strFileName("");
	CWADItem *pItem = NULL;
	m_lbTST1_Images.ResetContent();
	m_ihHelper.ResetContent();
	bool bAtLeastOne = false;
	
	if (iCurSel != CB_ERR)
	{
		strFileName = m_saWADList.GetAt (iListIndex);		
		m_ihHelper.LoadImage (strFileName, IH_LOAD_ONLYPACKAGE);

		if (m_ihHelper.GetErrorCode() != IH_SUCCESS)
		{
			AfxMessageBox (m_ihHelper.GetErrorText(), MB_ICONSTOP);

			// Clear out the selection from the combo box
			m_cbTST1_WAD.SetCurSel (-1);
			return;
		}

		pItem = m_ihHelper.GetFirstImage();
		bAtLeastOne = (pItem != NULL);

		while (pItem)
		{
			iItemAdded = m_lbTST1_Images.AddString (pItem->GetName());
			m_lbTST1_Images.SetItemData (iItemAdded, (DWORD)pItem);
			pItem = m_ihHelper.GetNextImage();
		}

		if (bAtLeastOne)
		{
			m_lbTST1_Images.SetCurSel(0);
			OnSelchangeTst1ListImages();
		}
	}	
	
}

void CToolSettingsToolbar::OnSelchangeTst1ListImages() 
{
	int iCurSel = m_lbTST1_Images.GetCurSel();
	CWADItem *pItem = NULL;

	if (iCurSel != LB_ERR)
	{
		pItem = (CWADItem *)(m_lbTST1_Images.GetItemData(iCurSel));
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

	// Flag for update (no longer in Update() because of the caching mechanism)	
	g_InvalidateList.InvalidateAll (INVALIDATE_DECAL_SELECTION_CHANGE);
	Update();	
}


LRESULT CToolSettingsToolbar::OnDecalListBoxCustomMessage(WPARAM nType, LPARAM nFlags)
{
	switch (nType)
	{
	case DECALLISTBOX_SELCHANGE:
		OnSelchangeTst1ListImages();
		break;

	case DECALLISTBOX_ENTER:
		LoadImageToTool (LEFT_BUTTON);
		break;

	default:
		ASSERT (false);		// Unhandled type?
		break;
	}	
	return 0;
}

LRESULT CToolSettingsToolbar::OnMouseWndCustomMessage (WPARAM nType, LPARAM nFlags)
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
	return 0;
}

bool CToolSettingsToolbar::GetCurrentSelection()
{
	int iCurSel = m_lbTST1_Images.GetCurSel();
	CWADItem *pItem = NULL;

	if (iCurSel != LB_ERR)
	{
		m_pWADItem = (CWADItem *)(m_lbTST1_Images.GetItemData(iCurSel));
		return true;
	}
	else
	{
		m_pWADItem = NULL;
		return false;
	}

	return false;
}


void CToolSettingsToolbar::LoadImageToTool (int iMouseButton)
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


void CToolSettingsToolbar::EnableControls (bool bState /* = TRUE */)
{
	m_bEnableControls = bState;

	CWnd *pWnd = NULL;

	pWnd = GetDlgItem (IDC_TST1_LIST_IMAGES);
	if (pWnd)
	{
		pWnd->EnableWindow (m_bEnableControls);
	}
	
	pWnd = GetDlgItem (IDC_TST1_COMBO_WAD);
	if (pWnd)
	{
		pWnd->EnableWindow (m_bEnableControls);
	}

	if (::IsWindow (m_wndPreview.m_hWnd))
	{
		m_wndPreview.EnableWindow (m_bEnableControls);
	}

	if (::IsWindow (m_wndMouseButton.m_hWnd))
	{
		m_wndMouseButton.EnableWindow (m_bEnableControls);
	}
}

void CToolSettingsToolbar::EnableRightButton (bool bEnable /* = TRUE */)
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

void CToolSettingsToolbar::SetStatus (LPCTSTR szText, int iPageNumber /* = 1 */)
{
	if (m_bFirstTimeTab)
	{
		FirstTimeInit();
	}

	switch (iPageNumber)
	{
	case 1:
		SetDlgItemText (IDC_TST1_STATUS, szText);
		break;

	default:
		ASSERT (false);
		break;
	}
}

void CToolSettingsToolbar::OnKillfocusTst2ColorTolerance() 
{
	CString strText("");
	GetDlgItemText (IDC_TST2_COLOR_TOLERANCE, strText);

	int iValue = atoi(strText);

	iValue = min (iValue, 255);
	iValue = max (iValue, 0);

	g_iColorTolerance = iValue;

	strText.Format ("%lu", iValue);

	SetDlgItemText (IDC_TST2_COLOR_TOLERANCE, strText);
}

void CToolSettingsToolbar::OnSelchangeTst2ComboSize() 
{
	if (ToolHasWidth( g_iCurrentTool))
	{
		CComboBox* pCombo  = (CComboBox* )GetDlgItem( IDC_TST2_COMBO_SIZE);
		int        iCurSel = pCombo->GetCurSel();

		if (g_iCurrentTool == EDIT_MODE_SELECTION)
		{
			g_bPasteInvisibleBackground = IndexToBrushSize( iCurSel);
		}
		else if (g_iCurrentTool == EDIT_MODE_RIVETS)
		{
			g_iRivetDistance = IndexToBrushSize( iCurSel);
		}
		else
		{
			g_iDrawingWidth = IndexToBrushSize( iCurSel);
		}
	}	
}

void CToolSettingsToolbar::OnSelchangeTst2ComboShape() 
{
	CComboBox* pCombo = (CComboBox* )GetDlgItem( IDC_TST2_COMBO_SHAPE);
	WPARAM     wID    = IndexToBrushShapeId( pCombo->GetCurSel());

	AfxGetMainWnd()->SendMessage( WM_COMMAND, wID, 0);	
}

void CToolSettingsToolbar::OnSelchangeTst2ComboAmount() 
{
	CComboBox* pCombo = (CComboBox* )GetDlgItem( IDC_TST2_COMBO_AMOUNT);
	WPARAM     wID    = IndexToToolAmountId( pCombo->GetCurSel());

	AfxGetMainWnd()->SendMessage( WM_COMMAND, wID, 0);	
}

void CToolSettingsToolbar::OnDestroy() 
{
	CDialogBar::OnDestroy();

	// Make sure we grab the globals this last time
	RetrieveDlgItems();
}

void CToolSettingsToolbar::OnSelchangeTst2ComboZoom() 
{
	CComboBox *pCombo  = (CComboBox *)(GetDlgItem( IDC_TST2_COMBO_ZOOM));

	if (pCombo)
	{		
		CView *pView = theApp.GetActiveView();

		if (pView)
		{
			CWallyView *pWallyView = NULL;
			pWallyView = DYNAMIC_DOWNCAST (CWallyView, pView);
		
			if (pWallyView)
			{
				int iCurSel = pCombo->GetCurSel();

				if (iCurSel != CB_ERR)
				{
					UINT iZoomValue = pCombo->GetItemData(iCurSel);

					pWallyView->SetZoomValue (iZoomValue);
				}
			}		// if (pWallyView)			
		}			// if (pView)
	}				// if (pCombo)

}

void CToolSettingsToolbar::OnUpdateTst2ComboZoom(CCmdUI* pCmdUI)
{
	CComboBox *pCombo  = (CComboBox *)(GetDlgItem( IDC_TST2_COMBO_ZOOM));
	
	HWND hFocus = ::GetFocus();

	if (pCombo)
	{		
		CView *pView = theApp.GetActiveView();

		if (pView)
		{
			// Don't go changing the selection if the focus isn't on a view
			if (hFocus == pView->m_hWnd)
			{	
				CWallyView *pWallyView = NULL;
				pWallyView = DYNAMIC_DOWNCAST (CWallyView, pView);
				
				// Only CWallyView has zoom right now
				pCmdUI->Enable (pWallyView != NULL);
			
				if (pWallyView)
				{					
					UINT iZoomValue = pWallyView->GetZoomValue();
					UINT iCount = pCombo->GetCount();

					for (UINT j = 0; j < iCount; j++)
					{
						if (pCombo->GetItemData(j) == iZoomValue)
						{
							pCombo->SetCurSel(j);
							j = iCount;
						}
					}

				}	// if (pWallyView)
			}		// if (hFocus == pView->m_hWnd)
		}			// if (pView)
		else
		{
			pCmdUI->Enable (FALSE);
		}
	}				// if (pCombo)
}

