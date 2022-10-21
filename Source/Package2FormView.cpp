// Package2FormView.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "Package2FormView.h"
#include "DibSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormView

IMPLEMENT_DYNCREATE(CPackage2FormView, CFormView)

CPackage2FormView::CPackage2FormView()
	: CFormView(CPackage2FormView::IDD)
{
	//{{AFX_DATA_INIT(CPackage2FormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//m_brDlgColor.CreateSolidBrush (RGB (128, 64, 64));
	m_brDlgColor.CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
	m_bLBImagesShowing = FALSE;
	m_bEDFilterShowing = FALSE;

	m_iOldRCWidth = 0;
	m_iOldRCHeight = 0;
}

CPackage2FormView::~CPackage2FormView()
{
	m_brDlgColor.DeleteObject();
}

void CPackage2FormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPackage2FormView)
	DDX_Control(pDX, IDC_BUTTON_SELECT_ALL, m_btnSelectAll);
	DDX_Control(pDX, IDC_CHECK_FILTER, m_ckFilter);
	DDX_Control(pDX, IDC_STATIC_NUMITEMS, m_csNumItems);
	DDX_Control(pDX, IDC_LIST_IMAGES, m_lbImages);
	DDX_Control(pDX, IDC_EDIT_FILTER, m_edFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPackage2FormView, CFormView)
	//{{AFX_MSG_MAP(CPackage2FormView)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CHECK_FILTER, OnCheckFilter)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnButtonSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormView diagnostics

#ifdef _DEBUG
void CPackage2FormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPackage2FormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPackage2FormView message handlers

HBRUSH CPackage2FormView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
		
	if (nCtlColor == CTLCOLOR_DLG)
	{		
		return (HBRUSH )m_brDlgColor;
	}
	
	return hbr;	
}

BOOL CPackage2FormView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CPackage2FormView::OnEraseBkgnd( CDC* pDC)
{
	return TRUE;
}


void CPackage2FormView::OnPaint() 
{
	CPaintDC PaintDC(this); // device context for painting

	RECT rcClient;
	GetClientRect( &rcClient);
	CRect rcLB (rcClient);
	CRect rcED (rcClient);
	CRect rcST (rcClient);

	int iRCWidth = rcLB.Width();
	int iRCHeight = rcLB.Height();
	
	BOOL bReposition = FALSE;

	if ((m_iOldRCWidth != iRCWidth) || (m_iOldRCHeight != iRCHeight))
	{
		m_iOldRCWidth = iRCWidth;
		m_iOldRCHeight = iRCHeight;
		bReposition = TRUE;
	}
	
	CDC DC;
	DC.CreateCompatibleDC( &PaintDC);
	CBitmap Bmp;	
	Bmp.CreateCompatibleBitmap( &PaintDC, iRCWidth, iRCHeight);
	DC.SelectObject( Bmp);

	WINDOWPLACEMENT wpLB;
	wpLB.length = sizeof (WINDOWPLACEMENT);
	
	WINDOWPLACEMENT wpED;
	wpED.length = sizeof (WINDOWPLACEMENT);

	WINDOWPLACEMENT wpCK;
	wpCK.length = sizeof (WINDOWPLACEMENT);

	WINDOWPLACEMENT wp;
	wp.length = sizeof (WINDOWPLACEMENT);

	int iHeight = 0;
	int iWidth = 0;

	if (bReposition)
	{	
		if (!m_bLBImagesShowing)
		{			
			m_bLBImagesShowing = TRUE;
			
			CString strText("");

			for (int j = 0; j < 20; j++)
			{
				strText.Format ("String %d", j);
				m_lbImages.AddString (strText);
			}			
		}

		
		m_lbImages.GetWindowPlacement (&wpLB);
		wpLB.rcNormalPosition.left		=	rcLB.left			+ P2_FORMVIEW_LB_OFFSET_LEFT;
		wpLB.rcNormalPosition.right		=	rcLB.right			- P2_FORMVIEW_LB_OFFSET_RIGHT;
		wpLB.rcNormalPosition.top		=	rcLB.top			+ P2_FORMVIEW_LB_OFFSET_TOP;
		wpLB.rcNormalPosition.bottom	=	max ((rcLB.bottom	- P2_FORMVIEW_LB_OFFSET_BOTTOM), (wpLB.rcNormalPosition.top + 2 + 16));
		m_lbImages.SetWindowPlacement (&wpLB);
	}

	m_lbImages.GetWindowPlacement (&wpLB);
	
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, (HBRUSH)m_brDlgColor);	
		
	::PatBlt( DC, 0, 0, iRCWidth, iRCHeight, PATCOPY);
	
	int iB = wpLB.rcNormalPosition.bottom;
	int iT = wpLB.rcNormalPosition.top;
	int iL = wpLB.rcNormalPosition.left;
	int iR = wpLB.rcNormalPosition.right;
		
	// clear background, around our listbox (eliminates flashing)
	//PaintDC.BitBlt( 0, 0, iRCWidth, iRCHeight, &DC, 0, 0, SRCCOPY);	
	PaintDC.BitBlt( 0,		0,		iRCWidth,		iT,				&DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt( 0,		0,		iL,				iRCHeight,		&DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt( iR,		0,		iRCWidth - iR,	iRCHeight,		&DC, 0, 0, SRCCOPY);
	PaintDC.BitBlt( iL,		iB,		iR - iL,		iRCHeight - iB,	&DC, 0, 0, SRCCOPY);

	DC.SelectStockObject( NULL_BRUSH);
	SelectObject( DC, hOldBrush);

	DC.DeleteDC();

	if (bReposition)
	{
		m_btnSelectAll.GetWindowPlacement (&wp);
		iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
		iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
		wp.rcNormalPosition.right = wpLB.rcNormalPosition.right;
		wp.rcNormalPosition.left = wp.rcNormalPosition.right - iWidth;
		wp.rcNormalPosition.bottom = wpLB.rcNormalPosition.top - 6;
		wp.rcNormalPosition.top = wp.rcNormalPosition.bottom - iHeight;
		m_btnSelectAll.SetWindowPlacement (&wp);

		m_edFilter.GetWindowPlacement (&wpED);
		iHeight = wpED.rcNormalPosition.bottom - wpED.rcNormalPosition.top;
		wpED.rcNormalPosition.left =		rcED.left			+ P2_FORMVIEW_ED_OFFSET_LEFT;
		wpED.rcNormalPosition.right =		wpLB.rcNormalPosition.right;
		wpED.rcNormalPosition.top =			wpLB.rcNormalPosition.bottom + 6;
		wpED.rcNormalPosition.bottom =		wpED.rcNormalPosition.top + iHeight;
		m_edFilter.SetWindowPlacement (&wpED);

		m_ckFilter.GetWindowPlacement (&wpCK);
		iHeight = wpCK.rcNormalPosition.bottom - wpCK.rcNormalPosition.top;
		wpCK.rcNormalPosition.left =		rcLB.left			+ P2_FORMVIEW_LB_OFFSET_LEFT;
		wpCK.rcNormalPosition.right =		wpED.rcNormalPosition.left - 4;
		wpCK.rcNormalPosition.top =			wpED.rcNormalPosition.top + 2;
		wpCK.rcNormalPosition.bottom =		wpCK.rcNormalPosition.top + iHeight;
		m_ckFilter.SetWindowPlacement (&wpCK);
		
		m_csNumItems.GetWindowPlacement (&wpLB);		
		wpLB.rcNormalPosition.left		=	wpCK.rcNormalPosition.left;
		wpLB.rcNormalPosition.right		=	rcST.right		- P2_FORMVIEW_LB_OFFSET_RIGHT;
		wpLB.rcNormalPosition.top		=	wpED.rcNormalPosition.bottom + 6;
		wpLB.rcNormalPosition.bottom	=	wpLB.rcNormalPosition.top + 12;
		m_csNumItems.SetWindowPlacement (&wpLB);
	}

	// Do not call CFormView::OnPaint() for painting messages
}


void CPackage2FormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	CString strText ("Filter:");
	SetDlgItemText (IDC_CHECK_FILTER, strText);	

	m_edFilter.EnableWindow(FALSE);
}

void CPackage2FormView::OnCheckFilter() 
{
	if (IsDlgButtonChecked (IDC_CHECK_FILTER))
	{
		m_edFilter.EnableWindow(TRUE);
		m_edFilter.SetSel (0, -1, FALSE);
		m_edFilter.SetFocus();
	}
	else
	{
		m_edFilter.EnableWindow(FALSE);
	}
}

void CPackage2FormView::OnEditPaste() 
{
	if (::IsClipboardFormatAvailable(CF_TEXT) && (::GetFocus() == m_edFilter.m_hWnd))
	{
		if (this->OpenClipboard())
		{
			// GetClipboardFile		
			HGLOBAL hg = ::GetClipboardData(CF_TEXT);
			if (hg == NULL)
			{
				AfxMessageBox ("Error reading from clipboard");
			}

			// Determine the data size
			int iDataSize = GlobalSize(hg); 
			
			// Alloc memory
			BYTE *byTextData = (BYTE *)GlobalAlloc(GMEM_FIXED, iDataSize); 

			// Grab the data					
			CopyMemory(byTextData, GlobalLock(hg), iDataSize); 
			
			// Get out as fast as we can from the clipboard
			GlobalUnlock(hg);
			CloseClipboard();

			char *szText = (char *)byTextData;

			m_edFilter.ReplaceSel (szText, TRUE);			
		}
	}	
}

void CPackage2FormView::OnButtonSelectAll() 
{
	m_lbImages.SelItemRange (TRUE, 0, m_lbImages.GetCount() - 1);
}
