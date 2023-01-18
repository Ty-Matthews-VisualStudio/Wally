/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// PaletteEditorDlg.cpp : interface of the CPaletteEditorDlg class
//
// Created by Ty Matthews, 1-13-1999
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "PaletteEditorDlg.h"
#include "PaletteWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg dialog


CPaletteEditorDlg::CPaletteEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPaletteEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaletteEditorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	SetPalette(); // Init to black for now
	m_bFirstTime = TRUE;
}


void CPaletteEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaletteEditorDlg)
	DDX_Control(pDX, IDC_EDIT_INDEX, m_edIndex);
	DDX_Control(pDX, IDC_EDIT_RED, m_edRed);
	DDX_Control(pDX, IDC_EDIT_GREEN, m_edGreen);
	DDX_Control(pDX, IDC_EDIT_BLUE, m_edBlue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaletteEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CPaletteEditorDlg)
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(WM_PALETTEWND_CUSTOM, OnPaletteWndCustomMessage)
	ON_BN_CLICKED(IDC_BUTTON_BLEND, OnButtonBlend)
	ON_EN_KILLFOCUS(IDC_EDIT_BLUE, OnKillfocusEditBlue)
	ON_EN_KILLFOCUS(IDC_EDIT_GREEN, OnKillfocusEditGreen)
	ON_EN_KILLFOCUS(IDC_EDIT_RED, OnKillfocusEditRed)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_EN_KILLFOCUS(IDC_EDIT_INDEX, OnKillfocusEditIndex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaletteEditorDlg message handlers

BOOL CPaletteEditorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ChangeSelection();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPaletteEditorDlg::Update()
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

void CPaletteEditorDlg::OnPaint() 
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

LRESULT CPaletteEditorDlg::OnPaletteWndCustomMessage(WPARAM nType, LPARAM nFlags)
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

void CPaletteEditorDlg::OnButtonBlend() 
{
	m_wndPalette.Blend();
}


void CPaletteEditorDlg::ChangeSelection()
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

void CPaletteEditorDlg::OnKillfocusEditIndex() 
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

void CPaletteEditorDlg::OnKillfocusEditBlue() 
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

void CPaletteEditorDlg::OnKillfocusEditGreen() 
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

void CPaletteEditorDlg::OnKillfocusEditRed() 
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

void CPaletteEditorDlg::OnButtonDefault() 
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

void CPaletteEditorDlg::OnOK() 
{
	memcpy (m_byPalette, m_wndPalette.GetPalette(), 768);	
	CDialog::OnOK();
}

