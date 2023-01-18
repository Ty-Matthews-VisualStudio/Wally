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


	// Get all of the currently open packages
	POSITION pos = theApp.PackageDocTemplate->GetFirstDocPosition();	
	
	m_bOpenPackage = pos != NULL ? true : false;
	
	CPackageDoc *pDoc = NULL;
	
	while( pos )
	{
		pDoc = (CPackageDoc *)theApp.PackageDocTemplate->GetNextDoc( pos );
		int iItemAdded = m_cbOpenWAD.AddString( pDoc->GetTitle() );
		m_cbOpenWAD.SetItemData( iItemAdded, (DWORD)pDoc );	
	}

	if( m_bOpenPackage )
	{
		// Now find the one that is currently active
		CPackageView *pPackageView = NULL;
		CView *pView = theApp.GetActiveView();		
		if (pView)
		{
			pPackageView = DYNAMIC_DOWNCAST (CPackageView, pView);			
		}
		
		int iMatch = 0;

		if( pPackageView )
		{
			iMatch = m_cbOpenWAD.FindString( -1, pPackageView->GetName() );
			if( iMatch != CB_ERR )
			{
				m_cbOpenWAD.SetCurSel( iMatch );				
			}
			else
			{
				ASSERT( FALSE );			// How can an item be added to the list, and then not found?
				m_cbOpenWAD.SetCurSel (0);				
			}
		}
		else
		{			
			m_cbOpenWAD.SetCurSel( 0 );			
		}
	}
	else
	{
		if( g_iMergeDestinationChoice == IDC_RADIO_OPEN_WAD )
		{
			CheckDlgButton( IDC_RADIO_NEW_WAD, 1 );
			CheckDlgButton( IDC_RADIO_OPEN_WAD, 0 );
		}
	}

	EnableDisable();
	
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
	m_MergeJobs.push_back( pNewJob ); 
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
}


UINT WINAPI CWadMergeDlg::GoThread( LPVOID lpParameter )
{
	if( !lpParameter )
	{
		_endthreadex( 1 );
		return 1;
	}

	LPTHREAD_PARAM lpParam = (LPTHREAD_PARAM)lpParameter;
	
	itMergeJob itJob = lpParam->lpMergeJobs->begin();
	BOOL bFirstWAD = TRUE;
	DWORD dwFlags = lpParam->dwFlags;
	CWADList *pWADList = lpParam->pWADList;	
	CPackageDoc *pDoc = NULL;
	
	while( itJob != lpParam->lpMergeJobs->end() )
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
			dwFlags ^= WAD_MERGE_FIRST_WAD;			
		}
		
		if( (*itJob)->LoadWAD() )
		{
			(*itJob)->MergeWAD( pWADList, dwFlags, lpParam );
		}

		bFirstWAD = FALSE;		
		itJob++;

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

	_endthreadex( 0 );
	return 0;
}

int CWadMergeDlg::DoModal() 
{
	int iReturn = CDialog::DoModal();

	itMergeJob itJob = m_MergeJobs.begin();
	
	while( itJob != m_MergeJobs.end() )
	{
		delete (*itJob);
		(*itJob) = NULL;

		itJob++;
	}

	return iReturn;
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

void CWadMergeDlg::UpdateList()
{
	m_lstWADs.DeleteAllItems();

	itMergeJob itJob = m_MergeJobs.begin();
	LV_ITEM lvText;
	LPSTR szText = NULL;
	CString strText("");
	int iItem = 0;
	
	while( itJob != m_MergeJobs.end() )
	{
		szText = (*itJob)->GetShortWadFile();

		lvText.mask = LVIF_TEXT | LVIF_PARAM;
		lvText.iSubItem = 0;
		lvText.state = 0;
		lvText.stateMask = 0;
		lvText.pszText = szText;
		lvText.cchTextMax = strlen( szText );
		lvText.lParam = (ULONG)(*itJob);
		
		iItem = m_lstWADs.InsertItem( &lvText );

		szText = (*itJob)->GetWildcard();		
		m_lstWADs.SetItemText( iItem, 1, szText );

		strText = (*itJob)->HasMinMaxRestrictions() ? "Yes" : "No";
		szText = strText.GetBuffer( strText.GetLength() );
		
		m_lstWADs.SetItemText( iItem, 2, szText );

		itJob++;
	}
}

void CWadMergeDlg::OnButtonRemove() 
{
	POSITION pos = m_lstWADs.GetFirstSelectedItemPosition();
	int iItem = 0;
	LV_ITEM lvItem;
	CWadMergeJob *pJob = NULL;
	itMergeJob itJob;
      
	if( pos )
	{	
		iItem = m_lstWADs.GetNextSelectedItem( pos );
		
		lvItem.mask = LVIF_PARAM;	
		lvItem.iItem = iItem;

		// Make sure we can get the item
		if( m_lstWADs.GetItem( &lvItem ))
		{
			if( AfxMessageBox( "Are you sure you want to remove this WAD from the list?", MB_YESNO ) == IDNO )
			{
				return;
			}

			pJob = (CWadMergeJob *)lvItem.lParam;			

			// Have to find the item in our internal list
			itJob = m_MergeJobs.begin();
	
			while( itJob != m_MergeJobs.end() )
			{
				if( (*itJob) == pJob )
				{
					delete (*itJob);
					m_MergeJobs.erase( itJob );
					break;
				}
				else
				{
					itJob++;
				}
			}

			m_lstWADs.DeleteItem( iItem );
			UpdateList();
		}
		
	}
		
}

void CWadMergeDlg::OnButtonProperties() 
{
	POSITION pos = m_lstWADs.GetFirstSelectedItemPosition();
	int iItem = 0;
	LV_ITEM lvItem;
	CWadMergeJob *pJob = NULL;
	CWadMergeAddDlg AddDlg;
      
	if( pos )
	{	
		iItem = m_lstWADs.GetNextSelectedItem( pos );
		
		lvItem.mask = LVIF_PARAM;	
		lvItem.iItem = iItem;

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
	itMergeJob itJob = m_MergeJobs.begin();
	
	while( itJob != m_MergeJobs.end() )
	{
		dwNumImages += (*itJob)->GetNumImages();
		itJob++;
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
