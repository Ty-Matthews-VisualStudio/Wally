// WadMergeAddDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "WadMergeAddDlg.h"
#include "WadMergeJob.h"
#include "ReMip.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWadMergeAddDlg dialog


CWadMergeAddDlg::CWadMergeAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWadMergeAddDlg::IDD, pParent), m_pMergeJob( NULL )
{
	//{{AFX_DATA_INIT(CWadMergeAddDlg)
	m_strWadFile = _T("");
	m_strWildcard = _T("");
	//}}AFX_DATA_INIT

	m_dwMaxHeight = 0;
	m_dwMaxWidth = 0;
	m_dwMinHeight = 0;
	m_dwMinWidth = 0;

}


void CWadMergeAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWadMergeAddDlg)
	DDX_Text(pDX, IDC_EDIT_WAD_FILE, m_strWadFile);
	DDX_Text(pDX, IDC_EDIT_WILDCARD, m_strWildcard);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWadMergeAddDlg, CDialog)
	//{{AFX_MSG_MAP(CWadMergeAddDlg)
	ON_BN_CLICKED(IDC_BUTTON_WAD, OnButtonWad)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXIMUM_HEIGHT, OnKillfocusEditMaximumHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_MAXIMUM_WIDTH, OnKillfocusEditMaximumWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_MINIMUM_HEIGHT, OnKillfocusEditMinimumHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_MINIMUM_WIDTH, OnKillfocusEditMinimumWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWadMergeAddDlg message handlers

BOOL CWadMergeAddDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT( GetMergeJob() );

	SetField( IDC_EDIT_MAXIMUM_HEIGHT, m_dwMaxHeight );	
	SetField( IDC_EDIT_MAXIMUM_WIDTH, m_dwMaxWidth );	
	SetField( IDC_EDIT_MINIMUM_HEIGHT, m_dwMinHeight );
	SetField( IDC_EDIT_MINIMUM_WIDTH, m_dwMinWidth );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWadMergeAddDlg::OnButtonWad() 
{
	UpdateData( TRUE );

	CImageHelper ihHelper;

	// Build some strings based on the String Table entries
	CString strWildCard( ihHelper.GetSupportedWADList() );	
	CString strTitle("Select the source WAD file");

	// Create a CFileDialog, init with our strings
	CFileDialog	dlgOpen (TRUE, NULL, NULL, OFN_FILEMUSTEXIST | 
		OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON, strWildCard, NULL);

	// Set some of the CFileDialog vars
	if( g_strFileOpenDirectory != "" )
	{
		dlgOpen.m_ofn.lpstrInitialDir = g_strFileOpenDirectory;
	}
	dlgOpen.m_ofn.lpstrTitle = strTitle;	
	
	if (dlgOpen.DoModal() == IDOK)
	{
		g_strFileOpenDirectory = dlgOpen.GetPathName().Left(dlgOpen.m_ofn.nFileOffset);

		m_strWadFile = dlgOpen.GetPathName();		
		UpdateData( FALSE );
	}
}

void CWadMergeAddDlg::SetMergeJob( CWadMergeJob *pMergeJob )
{
	if( pMergeJob )
	{
		m_pMergeJob = pMergeJob;
		
		m_strWadFile = pMergeJob->GetWadFile();
		m_strWildcard = pMergeJob->GetWildcard();
		m_dwMaxHeight = pMergeJob->GetMaxHeight();
		m_dwMaxWidth = pMergeJob->GetMaxWidth();
		m_dwMinHeight = pMergeJob->GetMinHeight();
		m_dwMinWidth = pMergeJob->GetMinWidth();
	}
	else
	{
		ASSERT( FALSE );
	}
}


CWadMergeJob *CWadMergeAddDlg::GetMergeJob()
{
	return m_pMergeJob;
}

int CWadMergeAddDlg::DoModal() 
{	
	int iReturn = CDialog::DoModal();

	if( iReturn == IDOK )
	{
		if( GetMergeJob() )
		{
			GetMergeJob()->SetWadFile( m_strWadFile );
			GetMergeJob()->SetWildcard( m_strWildcard );
			GetMergeJob()->SetMaxWidth( m_dwMaxWidth );
			GetMergeJob()->SetMaxHeight( m_dwMaxHeight );
			GetMergeJob()->SetMinWidth( m_dwMinWidth );
			GetMergeJob()->SetMinHeight( m_dwMinHeight );
		}
	}

	return iReturn;
}

void CWadMergeAddDlg::OnKillfocusEditMaximumHeight() 
{
	ValidateField( IDC_EDIT_MAXIMUM_HEIGHT, &m_dwMaxHeight );	
}

void CWadMergeAddDlg::OnKillfocusEditMaximumWidth() 
{
	ValidateField( IDC_EDIT_MAXIMUM_WIDTH, &m_dwMaxWidth );	
}

void CWadMergeAddDlg::OnKillfocusEditMinimumHeight() 
{
	ValidateField( IDC_EDIT_MINIMUM_HEIGHT, &m_dwMinHeight );
}

void CWadMergeAddDlg::OnKillfocusEditMinimumWidth() 
{
	ValidateField( IDC_EDIT_MINIMUM_WIDTH, &m_dwMinWidth );
}

void CWadMergeAddDlg::SetField( int iID, DWORD dwValue )
{
	if( dwValue != 0 )
	{
		CString strText("");
		strText.Format( "%lu", dwValue );
		SetDlgItemText( iID, strText );
	}
}

void CWadMergeAddDlg::ValidateField( int iID, DWORD *pDWORD )
{
	CString strText("");
	GetDlgItemText( iID, strText );
	DWORD dwValue = 0;
	div_t dt;

	if( strText.GetLength() == 0 )
	{
		if( pDWORD )
		{
			(*pDWORD) = 0;
		}
		return;
	}

	dwValue = abs( atol( strText ));

	if( dwValue == 0 )
	{
		if( pDWORD )
		{
			(*pDWORD) = 0;
		}
		strText = "";
		SetDlgItemText( iID, strText );
		return;
	}
	
	dwValue = max( dwValue, 16 );
	dwValue = min( dwValue, MAX_MIP_SIZE );

	dt = div( dwValue, 16 );
	dwValue = dt.quot * 16;
	
	strText.Format( "%lu", dwValue );
	SetDlgItemText( iID, strText );

	if( pDWORD )
	{
		(*pDWORD) = dwValue;
	}
}

void CWadMergeAddDlg::OnOK() 
{
	UpdateData( TRUE );
	OnKillfocusEditMaximumHeight();
	OnKillfocusEditMaximumWidth();
	OnKillfocusEditMinimumHeight();
	OnKillfocusEditMinimumWidth();

	if( m_strWildcard == "" )
	{
		m_strWildcard = "*";
	}

	g_strMergeWildCard = m_strWildcard;
	
	CDialog::OnOK();
}
