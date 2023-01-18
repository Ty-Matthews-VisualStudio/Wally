#include "stdafx.h"
#include "wally.h"
#include "DebugDibView.h"
#include "DibSection.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugDibView

IMPLEMENT_DYNCREATE(CDebugDibView, CScrollView)

CDebugDibView::CDebugDibView()
{
	CImageHelper ihHelper;
	ihHelper.LoadImage ("c:\\temp\\wally.pcx");

	m_pdsView = new CDibSection;

	if (ihHelper.GetErrorCode() == IH_SUCCESS)
	{
		m_pdsView->Init (ihHelper.GetImageWidth(), ihHelper.GetImageHeight(), ihHelper.GetColorDepth(), ihHelper.GetPalette(), TRUE);
		m_pdsView->SetRawBits (ihHelper.GetBits());
	}

	m_bZoomMode = FALSE;
	m_iZoomLevel = 1;
}

CDebugDibView::~CDebugDibView()
{
	if (m_pdsView)
	{
		delete m_pdsView;
		m_pdsView = NULL;
	}
}


BEGIN_MESSAGE_MAP(CDebugDibView, CScrollView)
	//{{AFX_MSG_MAP(CDebugDibView)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_TP_ZOOM, OnTpZoom)
	ON_UPDATE_COMMAND_UI(ID_TP_ZOOM, OnUpdateTpZoom)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugDibView drawing

void CDebugDibView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	CRect rcItem;	
	GetClientRect( &rcItem);	

	int rcWidth = rcItem.Width();
	int rcHeight = rcItem.Height();	
	
	CDC DC;
	DC.CreateCompatibleDC( pDC);
	CBitmap Bmp;
	Bmp.CreateCompatibleBitmap( pDC, rcWidth, rcHeight);
	DC.SelectObject( Bmp);
	
	// clear background
	HBRUSH hBrush    = CreateSolidBrush( RGB (0, 0, 0));
	HBRUSH hOldBrush = (HBRUSH )SelectObject( DC, hBrush);
	::PatBlt( DC, 0, 0, rcWidth, rcHeight, PATCOPY);
	SelectObject( DC, hOldBrush);
	DeleteObject( hBrush);

	m_pdsView->Show (&DC, 0, 0, m_iZoomLevel);
	pDC->BitBlt( 0, 0, rcWidth, rcHeight, &DC, 0, 0, SRCCOPY);

	DC.DeleteDC();	
}

/////////////////////////////////////////////////////////////////////////////
// CDebugDibView diagnostics

#ifdef _DEBUG
void CDebugDibView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDebugDibView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWLYView message handlers


void CDebugDibView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	SetScrollSizes( MM_TEXT, CSize(100, 100) );	
}

void CDebugDibView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	switch (nChar)
	{
	case VK_DOWN:
	case VK_UP:
	case VK_RIGHT:
	case VK_LEFT:
		{
			int iFactor = 1;

			if (IsKeyPressed( VK_CONTROL))
				iFactor = 16;
			else if (IsKeyPressed( VK_SHIFT))
				iFactor = 8;
			
//			m_pdsView->ShiftBits (nChar, iFactor);
			MessageBeep( MB_ICONQUESTION);
			ASSERT( FALSE);		// no longer available use the Offset filter instead
			InvalidateRect (NULL, FALSE);
		}
		break;

	case 'I':
		{
			CImageHelper ihHelper;
			ihHelper.LoadImage ("c:\\temp\\wally.pcx");

			if (m_pdsView)
			{
				delete m_pdsView;
				m_pdsView = NULL;
			}

			m_pdsView = new CDibSection;

			if (ihHelper.GetErrorCode() == IH_SUCCESS)
			{
				m_pdsView->Init (ihHelper.GetImageWidth(), ihHelper.GetImageHeight(), ihHelper.GetColorDepth(), ihHelper.GetPalette(), TRUE);
				m_pdsView->SetRawBits (ihHelper.GetBits());
			}
			InvalidateRect (NULL, FALSE);
		}
		break;

	case 'R':
		{
			m_pdsView->RotateBits (ROTATETO90);
			InvalidateRect (NULL, FALSE);
		}
		break;

	case 'L':
		{
			m_pdsView->RotateBits (ROTATETO270);
			InvalidateRect (NULL, FALSE);
		}
		break;


	case 'C':
		{
			m_pdsView->WriteToClipboard (this);
		}
		break;

	case 'T':
		{
			m_pdsView->WriteToClipboardTiled (this, 3, 1);
		}
		break;

	case 'M':
		{
			m_pdsView->MirrorBits();
			InvalidateRect (NULL, FALSE);
		}
		break;

	case 'F':
		{
			m_pdsView->FlipBits();	
			InvalidateRect (NULL, FALSE);
		}
		break;

	default:
		break;
	}


	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
} 

BOOL CDebugDibView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	//return CScrollView::OnEraseBkgnd(pDC);
}

void CDebugDibView::OnTpZoom() 
{
	m_bZoomMode = !m_bZoomMode;	
}

void CDebugDibView::OnUpdateTpZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_bZoomMode);	
}

void CDebugDibView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CScrollView::OnLButtonDown(nFlags, point);

	if (m_bZoomMode)
	{
		m_iZoomLevel++;
		NormalizeZoom();
	}
}

void CDebugDibView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	CScrollView::OnLButtonDblClk(nFlags, point);
	
	if (m_bZoomMode)
	{
		m_iZoomLevel++;
		NormalizeZoom();
	}
	else
	{
	}
}

void CDebugDibView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CScrollView::OnRButtonDown(nFlags, point);

	if (m_bZoomMode)
	{
		m_iZoomLevel--;
		NormalizeZoom();
	}
}

void CDebugDibView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{	
	CScrollView::OnRButtonDblClk(nFlags, point);

	if (m_bZoomMode)
	{
		m_iZoomLevel--;
		NormalizeZoom();
	}
}

void CDebugDibView::NormalizeZoom()
{
	m_iZoomLevel = min (m_iZoomLevel, 16);
	m_iZoomLevel = max (m_iZoomLevel, 1);

	InvalidateRect (NULL, FALSE);
}

BOOL CDebugDibView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CString strText("");
	
	strText.Format ("%lu x %lu   Bit Depth: %lu  Zoom: %lu", m_pdsView->GetWidth(), m_pdsView->GetHeight(), m_pdsView->GetBitCount(), m_iZoomLevel);
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_SEPARATOR), strText, true);

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}
