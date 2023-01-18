// WadMergeStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "WadMergeStatusDlg.h"
#include "WadMergeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int WM_WAD_MERGE_STATUS_CUSTOM = RegisterWindowMessage( "WM_WAD_MERGE_STATUS_CUSTOM" );

/////////////////////////////////////////////////////////////////////////////
// CWadMergeStatusDlg dialog


CWadMergeStatusDlg::CWadMergeStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWadMergeStatusDlg::IDD, pParent), m_lpThreadParameter( NULL ), m_hThreadHandle( NULL )
{
	//{{AFX_DATA_INIT(CWadMergeStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWadMergeStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWadMergeStatusDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_PROGRESS_STATUS, m_ctlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWadMergeStatusDlg, CDialog)
	//{{AFX_MSG_MAP(CWadMergeStatusDlg)
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(WM_WAD_MERGE_STATUS_CUSTOM, OnWadMergeStatusDlgCustomMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWadMergeStatusDlg message handlers

BOOL CWadMergeStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemText( IDC_STATUS, "" );
	SetDlgItemText( IDC_STATUS2, "" );
	
	if( m_lpThreadParameter )
	{
		CWadMergeDlg::LPTHREAD_PARAM lpParam = (CWadMergeDlg::LPTHREAD_PARAM)m_lpThreadParameter;		
		lpParam->pMergeStatus = this;
		
		unsigned uiThreadID;
		m_hThreadHandle = (HANDLE)_beginthreadex( NULL, 0, CWadMergeDlg::GoThread, (LPVOID)m_lpThreadParameter, 0, &uiThreadID );
		
		m_iTimerID = SetTimer( TIMER_WMS_THREAD, 1000, NULL);		
		m_ctlProgress.SetStep( 1 );

		m_btnOK.EnableWindow( FALSE );
	}
	else
	{
		SetDlgItemText( IDC_STATUS, "FATAL: Invalid parameter passed to CWadMergeStatusDlg::OnInitDialog()" );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWadMergeStatusDlg::OnTimer(UINT nIDEvent) 
{	
	CDialog::OnTimer(nIDEvent);

	switch( nIDEvent )
	{
	case TIMER_WMS_THREAD:
		{
			if( WaitForSingleObject( m_hThreadHandle, 0 ) == WAIT_OBJECT_0 )
			{
				KillTimer( nIDEvent );
				m_btnOK.EnableWindow( TRUE );

				SetDlgItemText( IDC_STATUS, "Finished" );
				SetDlgItemText( IDC_STATUS2, "" );
			}
		}
		break;

	default:
		break;
	}
}


LRESULT CWadMergeStatusDlg::OnWadMergeStatusDlgCustomMessage( WPARAM nType, LPARAM nFlags )
{
	LPMERGE_STATUS lpStatus = (LPMERGE_STATUS)nFlags;

	if( !lpStatus )
	{
		return 0;
	}
	CString strText("");

	switch( nType )
	{
	case WAD_MERGE_STATUS_IMAGE_COUNT:
		{
			m_ctlProgress.SetRange( 0, lpStatus->dwNumImages );

			strText.Format( "Merging %s (%lu images)", lpStatus->strImage, lpStatus->dwNumImages );
			SetDlgItemText( IDC_STATUS2, strText );
		}
		break;

	case WAD_MERGE_STATUS_STEP:
		{
			strText.Format( "%s ==> %s", lpStatus->strImage, lpStatus->strStatus );

			SetDlgItemText( IDC_STATUS, strText );
			m_ctlProgress.StepIt();			
		}
		break;

	default:
		break;
	}

	delete lpStatus;
	return 0;
}
