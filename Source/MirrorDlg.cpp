// MirrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "Filter.h"
#include "MirrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMirrorDlg dialog


CMirrorDlg::CMirrorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMirrorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMirrorDlg)
	//}}AFX_DATA_INIT

	m_pLayer          = NULL;
	m_bPreviewApplied = FALSE;

	m_iMirrorType     = -1;
}


void CMirrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMirrorDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMirrorDlg, CDialog)
	//{{AFX_MSG_MAP(CMirrorDlg)
	ON_BN_CLICKED(ID_FILTER_PREVIEW, OnFilterPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMirrorDlg message handlers

void CMirrorDlg::OnFilterPreview() 
{
	if (m_bPreviewApplied)
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_EDIT_UNDO, 0);

	int iType = MIRROR_LEFT_TO_RIGHT;

	if (IsDlgButtonChecked( IDC_RADIO_LEFT))
		iType = MIRROR_LEFT_TO_RIGHT;
	else if (IsDlgButtonChecked( IDC_RADIO_TOP))
		iType = MIRROR_TOP_TO_BOTTOM;
	else if (IsDlgButtonChecked( IDC_RADIO_UPPER_LEFT))
		iType = MIRROR_UPPER_LEFT_TO_REST;
	else
	{
		ASSERT( FALSE);
	}

	g_Filter.DoPixelFilter( m_pLayer, FILTER_MIRROR, iType);
	m_bPreviewApplied = TRUE;
}

BOOL CMirrorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CheckDlgButton( IDC_RADIO_LEFT,       (m_iMirrorType == MIRROR_LEFT_TO_RIGHT));
	CheckDlgButton( IDC_RADIO_TOP,        (m_iMirrorType == MIRROR_TOP_TO_BOTTOM));
	CheckDlgButton( IDC_RADIO_UPPER_LEFT, (m_iMirrorType == MIRROR_UPPER_LEFT_TO_REST));
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMirrorDlg::OnOK() 
{
	if (IsDlgButtonChecked( IDC_RADIO_LEFT))
		m_iMirrorType = MIRROR_LEFT_TO_RIGHT;
	else if (IsDlgButtonChecked( IDC_RADIO_TOP))
		m_iMirrorType = MIRROR_TOP_TO_BOTTOM;
	else if (IsDlgButtonChecked( IDC_RADIO_UPPER_LEFT))
		m_iMirrorType = MIRROR_UPPER_LEFT_TO_REST;
	else
	{
		ASSERT( FALSE);
	}
	
	CDialog::OnOK();
}
