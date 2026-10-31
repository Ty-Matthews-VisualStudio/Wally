// WadMergeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "WadMergeDlg.h"
#include "WadMergeAddDlg.h"
#include "WadMergeJob.h"
#include "MiscFunctions.h"
#include "ImageHelper.h"
#include "WADList.h"
#include "PackageDoc.h"
#include "PackageView.h"
#include "WadMergeStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWadMergeDlg dialog


CWadMergeDlg::CWadMergeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWadMergeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWadMergeDlg)
	m_strExistingWAD = g_strMergeDestinationWAD;
	//}}AFX_DATA_INIT
}

CWadMergeDlg::~CWadMergeDlg()
{
}


void CWadMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWadMergeDlg)
	DDX_Control(pDX, IDC_COMBO_OPEN_WAD, m_cbOpenWAD);
	DDX_Control(pDX, IDC_COMBO_WAD_TYPE, m_cbWADType);
	DDX_Control(pDX, IDC_LIST_WADS, m_lstWADs);
	DDX_Control(pDX, IDC_BUTTON_EXISTING_WAD, m_btnExisting);
	DDX_Control(pDX, IDC_EDIT_EXISTING_WAD, m_edExisting);
	DDX_Text(pDX, IDC_EDIT_EXISTING_WAD, m_strExistingWAD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWadMergeDlg, CDialog)
	//{{AFX_MSG_MAP(CWadMergeDlg)
	ON_BN_CLICKED(IDC_BUTTON_EXISTING_WAD, OnButtonExistingWad)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_BN_CLICKED(IDC_RADIO_EXISTING_WAD, OnRadioExistingWad)
	ON_BN_CLICKED(IDC_RADIO_NEW_WAD, OnRadioNewWad)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_PROPERTIES, OnButtonProperties)
	ON_BN_CLICKED(IDC_RADIO_OPEN_WAD, OnRadioOpenWad)
	ON_BN_CLICKED(IDC_BUTTON_QUICK_ADD, OnButtonQuickAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_WADS, OnDblclkListWads)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWadMergeDlg message handlers

BOOL CWadMergeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CheckDlgButton( g_iMergeDestinationChoice, 1 );
	CheckDlgButton( g_iMergeDuplicationOption, 1 );
		
	int iInsert = m_cbWADType.InsertString( -1, "Quake1 (WAD2)" );
	if( iInsert != CB_ERR )
	{
		m_cbWADType.SetItemData( iInsert, WAD2_TYPE );
	}

	iInsert = m_cbWADType.InsertString( -1, "Half-Life (WAD3)" );
	if( iInsert != CB_ERR )
	{
		m_cbWADType.SetItemData( iInsert, WAD3_TYPE );
	}

	for( int j = 0; j < m_cbWADType.GetCount(); j++ )
	{
		if( m_cbWADType.GetItemData( j ) == g_iMergeNewWADType )
		{
			m_cbWADType.SetCurSel( j );
		}
	}

	m_lstWADs.InsertColumn (0, "Name", LVCFMT_LEFT, 120, 0);
	m_lstWADs.InsertColumn (1, "WildCards", LVCFMT_LEFT, 80, 1);
	m_lstWADs.InsertColumn (2, "Restrictions", LVCFMT_LEFT, 68, 2);	

	FillOpenPackages( NULL );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CWadMergeDlg::OnButtonExistingWad() 
{
	UpdateData( TRUE );

	CImageHelper ihHelper;	

	// Build some strings based on the String Table entries
	CString strWildCard ( ihHelper.GetSupportedWADList() );	
	CString strTitle("Select the destination WAD file");

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
		
		m_strExistingWAD = dlgOpen.GetPathName();
		UpdateData( FALSE );
	}
}

void CWadMergeDlg::OnButtonAdd() 
{
	CWadMergeAddDlg AddDlg;
	CWadMergeJob *pNewJob = new CWadMergeJob;

	// Initialize to some defaults
	pNewJob->SetWildcard( g_strMergeWildCard );

	if( pNewJob )
	{
		AddDlg.SetMergeJob( pNewJob );
		
		if( AddDlg.DoModal() == IDOK )
		{
			AddJob( pNewJob );
			UpdateList();
		}
		else
		{
			delete pNewJob;
		}
	}
	
}

void CWadMergeDlg::AddJob( CWadMergeJob *pNewJob )
{
	if( pNewJob )
	{
		m_MergeJobs.push_back( unique_ptr<CWadMergeJob>( pNewJob ) );
	}
}

void CWadMergeDlg::OnButtonGo() 
{
	UpdateData( TRUE );
	g_strMergeDestinationWAD = m_strExistingWAD;

	CWADList WADList;
	CWADList *pWADList;
	DWORD dwFlags = WAD_MERGE_NONE;
	CPackageDoc *pDoc = NULL;
	
	if( IsDlgButtonChecked( IDC_RADIO_EXISTING_WAD ) )
	{
		g_iMergeDestinationChoice = IDC_RADIO_EXISTING_WAD;
		
		if( WADList.Serialize( m_strExistingWAD, NULL, NULL, FALSE ) != IH_SUCCESS )
		{
			AfxMessageBox( "Failed to load WAD file", MB_ICONSTOP );
			return;
		}

		pWADList = &WADList;
	}
	else
	{
		if( IsDlgButtonChecked( IDC_RADIO_NEW_WAD ) )
		{
			int iCurSel = m_cbWADType.GetCurSel();

			if( iCurSel == CB_ERR )
			{
				AfxMessageBox( "Did not specify WAD type", MB_ICONSTOP );
				return;
			}

			g_iMergeDestinationChoice = IDC_RADIO_NEW_WAD;
			g_iMergeNewWADType = m_cbWADType.GetItemData( iCurSel );

			pDoc = theApp.CreatePackageDoc();

			if( pDoc )
			{
				pDoc->SetWADType( g_iMergeNewWADType );
				pWADList = pDoc->GetWADList();
			}
			else
			{
				AfxMessageBox( "Failed to create new Package", MB_ICONSTOP );
				return;
			}
		}
		else
		{
			if( IsDlgButtonChecked( IDC_RADIO_OPEN_WAD ) )
			{
				int iCurSel = m_cbOpenWAD.GetCurSel();

				if( iCurSel == CB_ERR )
				{
					AfxMessageBox( "Did not specify WAD type", MB_ICONSTOP );
					return;
				}

				pDoc = (CPackageDoc *)m_cbOpenWAD.GetItemData( iCurSel );
				
				g_iMergeDestinationChoice = IDC_RADIO_OPEN_WAD;
				pWADList = pDoc->GetWADList();
			}
			else
			{
				AfxMessageBox( "Unknown option", MB_ICONSTOP );
				ASSERT( FALSE );
				return;
			}
		}
	}	

	if( IsDlgButtonChecked( IDC_RADIO_IGNORE ) )
	{
		g_iMergeDuplicationOption = IDC_RADIO_IGNORE;
		dwFlags |= WAD_MERGE_DUP_IGNORE;
	}
	else
	{
		if( IsDlgButtonChecked( IDC_RADIO_RENAME ) )
		{
			g_iMergeDuplicationOption = IDC_RADIO_RENAME;
			dwFlags |= WAD_MERGE_DUP_RENAME;
		}
		else
		{	
			if( IsDlgButtonChecked( IDC_RADIO_OVERWRITE ) )
			{
				g_iMergeDuplicationOption = IDC_RADIO_OVERWRITE;
				dwFlags |= WAD_MERGE_DUP_OVERWRITE;
			}
			else
			{
				AfxMessageBox( "Unknown option", MB_ICONSTOP );
				ASSERT( FALSE );
				return;
			}
		}
	}

	LPTHREAD_PARAM lpParam = new THREAD_PARAM;
	
	lpParam->iDestinationChoice = g_iMergeDestinationChoice;
	lpParam->lpMergeJobs = &m_MergeJobs;
	lpParam->pWADList = pWADList;
	lpParam->szExistingWAD = m_strExistingWAD.GetBuffer( m_strExistingWAD.GetLength() );
	lpParam->dwFlags = dwFlags;
	lpParam->iNewWADType = g_iMergeNewWADType;
	lpParam->pPackageDoc = pDoc;
	lpParam->dwNumImages = GetNumImages();

	CWadMergeStatusDlg dlgStatus;
	dlgStatus.SetThreadParameter( (LPVOID)lpParam );

	dlgStatus.DoModal();

	FillOpenPackages( pDoc );
}


UINT WINAPI CWadMergeDlg::GoThread( LPVOID lpParameter )
{
	if( !lpParameter )
	{
		return 1;
	}

	LPTHREAD_PARAM lpParam = (LPTHREAD_PARAM)lpParameter;
	vector< CWadMergeJob * > jobs;

	jobs.reserve( lpParam->lpMergeJobs->size() );
	for( auto& pOwned : *lpParam->lpMergeJobs )
	{
		jobs.push_back( pOwned.get() );
	}

	BOOL bFirstWAD = TRUE;
	DWORD dwFlags = lpParam->dwFlags;
	CWADList *pWADList = lpParam->pWADList;
	
	for( CWadMergeJob *pJob : jobs )
	{		
		if( bFirstWAD )
		{
			if( pWADList->GetNumLumps() == 0 )
			{
				dwFlags |= WAD_MERGE_FIRST_WAD;
			}
		}
		else
		{
			dwFlags &= ~WAD_MERGE_FIRST_WAD;
		}
		
		if( pJob->LoadWAD() )
		{
			pJob->MergeWAD( pWADList, dwFlags, lpParam );
		}

		bFirstWAD = FALSE;

		if( 
			(lpParam->iDestinationChoice == IDC_RADIO_NEW_WAD ) ||
			(lpParam->iDestinationChoice == IDC_RADIO_OPEN_WAD ) )
		{
			lpParam->pPackageDoc->SetModifiedFlag( TRUE );
			lpParam->pPackageDoc->UpdateViews();
		}	
	}

	switch( lpParam->iDestinationChoice )
	{
	case IDC_RADIO_EXISTING_WAD:
		{
			pWADList->Serialize( lpParam->szExistingWAD, NULL, NULL, TRUE );
		}
		break;
		
	case IDC_RADIO_NEW_WAD:
	case IDC_RADIO_OPEN_WAD:		
		break;

	default:
		ASSERT( FALSE );
		break;
	}
	
	delete lpParameter;
	return 0;
}

void CWadMergeDlg::OnRadioExistingWad() 
{
	EnableDisable();	
}

void CWadMergeDlg::OnRadioNewWad() 
{
	EnableDisable();
}

void CWadMergeDlg::EnableDisable()
{
	m_edExisting.EnableWindow( IsDlgButtonChecked( IDC_RADIO_EXISTING_WAD ) );
	m_btnExisting.EnableWindow( IsDlgButtonChecked( IDC_RADIO_EXISTING_WAD ) );
	m_cbWADType.EnableWindow( IsDlgButtonChecked( IDC_RADIO_NEW_WAD ) );

	if( m_bOpenPackage )
	{
		m_cbOpenWAD.EnableWindow( IsDlgButtonChecked( IDC_RADIO_OPEN_WAD) );
	}
	else
	{
		m_cbOpenWAD.EnableWindow( FALSE );
	}
}

void CWadMergeDlg::FillOpenPackages( CPackageDoc *pSelectDoc )
{
	CPackageDoc *pPreferred = pSelectDoc;

	if( !pPreferred )
	{
		int iCurSel = m_cbOpenWAD.GetCurSel();
		if( iCurSel != CB_ERR )
		{
			pPreferred = (CPackageDoc *)m_cbOpenWAD.GetItemData( iCurSel );
		}
	}

	m_cbOpenWAD.ResetContent();

	POSITION pos = theApp.PackageDocTemplate->GetFirstDocPosition();
	m_bOpenPackage = ( pos != NULL );

	while( pos )
	{
		CPackageDoc *pDoc = (CPackageDoc *)theApp.PackageDocTemplate->GetNextDoc( pos );
		int iItemAdded = m_cbOpenWAD.AddString( pDoc->GetTitle() );
		m_cbOpenWAD.SetItemData( iItemAdded, (DWORD)pDoc );
	}

	if( !m_bOpenPackage )
	{
		if( IsDlgButtonChecked( IDC_RADIO_OPEN_WAD ) )
		{
			CheckDlgButton( IDC_RADIO_NEW_WAD, 1 );
			CheckDlgButton( IDC_RADIO_OPEN_WAD, 0 );
		}

		EnableDisable();
		return;
	}

	int iMatch = CB_ERR;

	if( pPreferred )
	{
		for( int j = 0; j < m_cbOpenWAD.GetCount(); j++ )
		{
			if( (CPackageDoc *)m_cbOpenWAD.GetItemData( j ) == pPreferred )
			{
				iMatch = j;
				break;
			}
		}
	}

	if( iMatch == CB_ERR )
	{
		CView *pView = theApp.GetActiveView();
		CPackageView *pPackageView = NULL;

		if( pView )
		{
			pPackageView = DYNAMIC_DOWNCAST( CPackageView, pView );
		}

		if( pPackageView )
		{
			iMatch = m_cbOpenWAD.FindString( -1, pPackageView->GetName() );
		}
	}

	if( iMatch != CB_ERR )
	{
		m_cbOpenWAD.SetCurSel( iMatch );
	}
	else
	{
		m_cbOpenWAD.SetCurSel( 0 );
	}

	EnableDisable();
}

void CWadMergeDlg::UpdateList()
{
	m_lstWADs.DeleteAllItems();

	LV_ITEM lvText = {};
	LPSTR szText = NULL;
	CString strText("");
	int iItem = 0;
	
	for( auto& pOwned : m_MergeJobs )
	{
		CWadMergeJob *pJob = pOwned.get();
		szText = pJob->GetShortWadFile();

		lvText.mask = LVIF_TEXT | LVIF_PARAM;
		lvText.iItem = m_lstWADs.GetItemCount();
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = szText;
		lvText.cchTextMax = strlen( szText );
		lvText.lParam = (LPARAM)pJob;
		
		iItem = m_lstWADs.InsertItem( &lvText );

		szText = pJob->GetWildcard();		
		m_lstWADs.SetItemText( iItem, 1, szText );

		strText = pJob->HasMinMaxRestrictions() ? "Yes" : "No";
		szText = strText.GetBuffer( strText.GetLength() );
		
		m_lstWADs.SetItemText( iItem, 2, szText );
	}
}

void CWadMergeDlg::OnButtonRemove() 
{
	POSITION pos = m_lstWADs.GetFirstSelectedItemPosition();
	int iItem = 0;
	LV_ITEM lvItem = {};
	CWadMergeJob *pJob = NULL;
      
	if( pos )
	{	
		iItem = m_lstWADs.GetNextSelectedItem( pos );
		
		lvItem.mask = LVIF_PARAM;	
		lvItem.iItem = iItem;
		lvItem.iSubItem = 0;

		// Make sure we can get the item
		if( m_lstWADs.GetItem( &lvItem ))
		{
			if( AfxMessageBox( "Are you sure you want to remove this WAD from the list?", MB_YESNO ) == IDNO )
			{
				return;
			}

			pJob = (CWadMergeJob *)lvItem.lParam;			

			for( auto itJob = m_MergeJobs.begin(); itJob != m_MergeJobs.end(); ++itJob )
			{
				if( itJob->get() == pJob )
				{
					m_MergeJobs.erase( itJob );
					break;
				}
			}

			m_lstWADs.DeleteItem( iItem );
			UpdateList();
		}
		
	}
		
}

void CWadMergeDlg::OnDblclkListWads(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnButtonProperties();
	*pResult = 0;
}

void CWadMergeDlg::OnButtonProperties() 
{
	POSITION pos = m_lstWADs.GetFirstSelectedItemPosition();
	int iItem = 0;
	LV_ITEM lvItem = {};
	CWadMergeJob *pJob = NULL;
	CWadMergeAddDlg AddDlg;
      
	if( pos )
	{	
		iItem = m_lstWADs.GetNextSelectedItem( pos );
		
		lvItem.mask = LVIF_PARAM;	
		lvItem.iItem = iItem;
		lvItem.iSubItem = 0;

		// Make sure we can get the item
		if( m_lstWADs.GetItem( &lvItem ))
		{
			pJob = (CWadMergeJob *)lvItem.lParam;			
			AddDlg.SetMergeJob( pJob );
		
			if( AddDlg.DoModal() == IDOK )
			{
				UpdateList();
			}
		}
	}		
}

void CWadMergeDlg::OnRadioOpenWad() 
{
	if( !m_bOpenPackage )
	{
		CheckDlgButton( IDC_RADIO_NEW_WAD, 1 );
		CheckDlgButton( IDC_RADIO_OPEN_WAD, 0 );
	}
	
	EnableDisable();	
}

DWORD CWadMergeDlg::GetNumImages()
{
	DWORD dwNumImages = 0;
	
	for( auto& pOwned : m_MergeJobs )
	{
		dwNumImages += pOwned->GetNumImages();
	}

	return dwNumImages;
}

void CWadMergeDlg::OnButtonQuickAdd() 
{
	CImageHelper ihHelper;
		
	// Build some strings based on the String Table entries
	CString strWildCard( ihHelper.GetSupportedWADList() );	
	CString strTitle("Select the source WAD file(s)");
		
	// Create a CFileDialog, init with our strings
	CFileDialog	dlgOpen (TRUE, NULL, NULL, OFN_FILEMUSTEXIST | 
		OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON | OFN_ALLOWMULTISELECT, 
		strWildCard, NULL); 		
	
	char szBuffer[_MAX_PATH * 256];
	dlgOpen.m_ofn.nMaxFile = sizeof( szBuffer );
	
	ZeroMemory( szBuffer, sizeof( szBuffer ) );
	dlgOpen.m_ofn.lpstrFile = szBuffer;
	
	// Set some of the CFileDialog vars
	if( g_strFileOpenDirectory != "" )
	{
		dlgOpen.m_ofn.lpstrInitialDir = g_strFileOpenDirectory;
	}

	dlgOpen.m_ofn.lpstrTitle = strTitle;
	
	if (dlgOpen.DoModal() == IDOK)
	{
		g_strFileOpenDirectory = dlgOpen.GetPathName().Left(dlgOpen.m_ofn.nFileOffset);
		
		CString strFileName("");		

		POSITION Pos = dlgOpen.GetStartPosition();
	
		Pos = dlgOpen.GetStartPosition();
		while (Pos)
		{
			strFileName = dlgOpen.GetNextPathName( Pos );
			CWadMergeJob *pNewJob = new CWadMergeJob;

			if( pNewJob )
			{
				// Initialize to some defaults
				pNewJob->SetWildcard( "*" );
				pNewJob->SetWadFile( strFileName );
				AddJob( pNewJob );
			}
		}
		
		UpdateList();
	}
}
