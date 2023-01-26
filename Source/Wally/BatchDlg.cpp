// BatchTempDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "BatchDlg.h"
#include "MiscFunctions.h"
#include "WallyPropertySheet.h"
#include "ReMip.h"
#include "ImageHelper.h"
#include "BatchJob.h"
#include "BatchSummaryDlg.h"
#include "PackageDoc.h"
#include "PackageView.h"
#include "BatchAdvancedDlg.h"
#include "DirectoryList.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

static UINT NEAR WM_BATCHDLG_CUSTOM = RegisterWindowMessage( "WM_WALLY_BATCHDLG_CUSTOM" );

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog


CBatchDlg::CBatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchDlg)
	//}}AFX_DATA_INIT
	m_strSourceDir = "";
	m_strDestinationDir = "";
	//m_pThreadList = NULL;
	SetMaxThreads( g_iMaxConversionThreads );
	m_strStatusText = "";
	m_iJobCount = 0;
	m_iFinishedCount = 0;
	m_ThreadMessage.SetAppSpecific( (LPVOID)this );
	m_ThreadManager.RegisterCallBack( ThreadMessageCallBack, &m_ThreadMessage );
}

CBatchDlg::~CBatchDlg()
{
	for (LPVOID pJob : m_vBatchJobs)
	{
		if (pJob)
		{
			delete (CBatchJob *)pJob;
			pJob = NULL;
		}
	}
	m_vBatchJobs.erase(m_vBatchJobs.begin(), m_vBatchJobs.end());
}


void CBatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchDlg)
	DDX_Control(pDX, IDC_COMBO_NEW_PACKAGE, m_cbWADType);
	DDX_Control(pDX, IDC_EDIT_WILDCARDS, m_edWildcards);
	DDX_Control(pDX, IDC_BUTTON_DEST, m_btnDestinationDirectory);
	DDX_Control(pDX, IDC_EDIT_DEST_DIR, m_edDestinationDirectory);
	DDX_Control(pDX, IDC_COMBO_PACKAGE, m_cbPackages);
	DDX_Control(pDX, IDC_COMBO_OUTPUT_FORMAT, m_cbOutputFormat);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_pbStatus);
	DDX_Control(pDX, IDOK, m_btnOk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchDlg)
	ON_BN_CLICKED(IDC_BUTTON_DEST, OnButtonDest)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE, OnButtonSource)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_OPTIONS, OnButtonViewOptions)
	ON_BN_CLICKED(IDC_RADIO_CURRENT_PACKAGE, OnRadioCurrentPackage)
	ON_BN_CLICKED(IDC_RADIO_TEXTURES, OnRadioTextures)
	ON_CBN_SELCHANGE(IDC_COMBO_PACKAGE, OnSelchangeComboPackage)
	ON_EN_KILLFOCUS(IDC_EDIT_DEST_DIR, OnKillfocusEditDestDir)
	ON_EN_KILLFOCUS(IDC_EDIT_SOURCE_DIR, OnKillfocusEditSourceDir)
	ON_REGISTERED_MESSAGE(WM_BATCHDLG_CUSTOM, OnBatchDlgCustomMessage)
	ON_BN_CLICKED(IDC_RADIO_NEW_PACKAGE, OnRadioNewPackage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg message handlers

BOOL CBatchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateWallyOptions ();	
	
	//m_pThreadList = new CThreadList (this);
	//m_pThreadList->SetMaxThreads (GetMaxThreads());

	m_iButtonState = BUTTON_GO;

	SetDlgItemText (IDC_EDIT_DEST_DIR, g_szDestinationConvertDirectory);
	SetDlgItemText (IDC_EDIT_SOURCE_DIR, g_szSourceConvertDirectory);
	SetDlgItemText (IDC_EDIT_WILDCARDS, g_szConvertWildCard);

	SetDlgItemText (IDC_TEMP_STATUS, "");
	CheckDlgButton (IDC_CHECK_OVERWRITE, g_bOverWriteFiles);	

	// TODO:  When adding new games, change this to read CImageHelper, etc	
	CImageHelper ihHelper;

	CWildCardItem *pItem = ihHelper.GetFirstSupportedImage();
	CString strAddString;
	int iItemAdded = 0;

	while (pItem)
	{
		strAddString = pItem->GetDescription();	
		iItemAdded = m_cbOutputFormat.AddString (strAddString);
		m_cbOutputFormat.SetItemData (iItemAdded, (DWORD)pItem);
		pItem = ihHelper.GetNextSupportedImage();
	}

	if (m_cbOutputFormat.GetCount() > g_iFileExportExtension)
	{
		m_cbOutputFormat.SetCurSel(g_iFileExportExtension);
	}
	else
	{
		m_cbOutputFormat.SetCurSel(0);
	}	

	POSITION pos = theApp.PackageDocTemplate->GetFirstDocPosition();	
	
	bool bAtLeastOne = pos != NULL ? true : false;	
	
	CheckDlgButton( IDC_RADIO_CURRENT_PACKAGE, bAtLeastOne);
	CheckDlgButton( IDC_RADIO_TEXTURES, !bAtLeastOne);
	m_cbPackages.EnableWindow(bAtLeastOne);
	m_cbOutputFormat.EnableWindow(!bAtLeastOne);
	m_edDestinationDirectory.EnableWindow(!bAtLeastOne);
	m_btnDestinationDirectory.EnableWindow(!bAtLeastOne);
	m_btnDestinationDirectory.SetWindowText ("...");
	CPackageDoc *pDoc = NULL;
	
	while (pos != NULL)
	{
		pDoc = (CPackageDoc *)theApp.PackageDocTemplate->GetNextDoc(pos);
		int iItemAdded = m_cbPackages.AddString (pDoc->GetTitle());
		m_cbPackages.SetItemData (iItemAdded, (DWORD)pDoc);	
	}

	if (bAtLeastOne)
	{
		CPackageView *pPackageView = NULL;
		CView *pView = theApp.GetActiveView();		
		if (pView)
		{
			pPackageView = DYNAMIC_DOWNCAST (CPackageView, pView);			
		}
		
		int iMatch = 0;

		if (pPackageView)
		{
			iMatch = m_cbPackages.FindString (-1, pPackageView->GetName());			
			if (iMatch != CB_ERR)
			{
				m_cbPackages.SetCurSel (iMatch);
				pDoc = (CPackageDoc *)(m_cbPackages.GetItemData(iMatch));
				//m_pThreadList->SetDestinationType (DESTINATION_PACKAGE, pDoc);
				m_iDestinationType = DESTINATION_PACKAGE;
				m_pDestinationPackage = pDoc;
			}
			else
			{
				ASSERT (false);			// How can an item be added to the list, and then not found?
				m_cbPackages.SetCurSel (0);
				pDoc = (CPackageDoc *)(m_cbPackages.GetItemData(0));				
				//m_pThreadList->SetDestinationType (DESTINATION_PACKAGE, pDoc);
				m_iDestinationType = DESTINATION_PACKAGE;
				m_pDestinationPackage = pDoc;
			}
		}
		else
		{
			m_cbPackages.SetCurSel (0);
			pDoc = (CPackageDoc *)(m_cbPackages.GetItemData(0));
			//m_pThreadList->SetDestinationType (DESTINATION_PACKAGE, pDoc);
			m_iDestinationType = DESTINATION_PACKAGE;
			m_pDestinationPackage = pDoc;
		}
	}
	else
	{
		m_iDestinationType = DESTINATION_TEXTURES;
		m_pDestinationPackage = NULL;
	}

	CWnd *pWnd = GetDlgItem (IDC_RADIO_CURRENT_PACKAGE);

	if (pWnd)
	{
		pWnd->EnableWindow(bAtLeastOne);
	}


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

	m_cbWADType.EnableWindow( FALSE );

	m_edWildcards.SetFocus();	
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchDlg::OnButtonDest() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_DEST_DIR, strDirectory);

	strDirectory = BrowseForFolder("Please select the destination directory:", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_DEST_DIR, strDirectory);
		m_strDestinationDir = strDirectory;
	}
}

void CBatchDlg::OnButtonSource() 
{
	CString strDirectory("");
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);
	
	strDirectory = BrowseForFolder("Please select the source directory:", strDirectory);

	if (strDirectory != "")
	{
		SetDlgItemText (IDC_EDIT_SOURCE_DIR, strDirectory);
		m_strSourceDir = strDirectory;
	}	
}

void CBatchDlg::OnButtonGo() 
{
	BeginWaitCursor();
	
	int x = 0;
	int x1 = 0;
	
	CString strText("");
	CString strWildCards("");
	std::string sWildCards;
	std::vector<std::string> vWildCards;
	char *szWildCard = NULL;	
	int iStrLength = 0;
	int iStrPosition = 0;
	int iImageType = 0;

	bool bFilesToConvert = false;
	bool bMoreWildCards = true;
	
	CString strFileName("");
	iImageType = m_cbOutputFormat.GetCurSel();

	switch (m_iButtonState)
	{
	case BUTTON_GO:	
		{			
			m_iJobCount = 0;
			m_iFinishedCount = 0;
			m_strStatusText = "";		

			m_ThreadManager.SetMaxThreads( GetMaxThreads() );
			GetDlgItemText (IDC_EDIT_DEST_DIR, m_strDestinationDir);
			GetDlgItemText (IDC_EDIT_SOURCE_DIR, m_strSourceDir);	
		
			m_strStatusText = "";
			SetDlgItemText (IDC_EDIT_STATUS, m_strStatusText);
			m_pbStatus.SetRange (0, m_iJobCount);

			bool bNoSource = (m_strSourceDir == "");
			bool bNoDest = (m_iDestinationType == DESTINATION_TEXTURES ? m_strDestinationDir == "" : false);

			if (bNoSource || bNoDest) 
			{
				strText.Format ("Please provide a %s%s%s directory.", m_strSourceDir == "" ? "source" : "", ((bNoSource && bNoDest) ? " and " : ""), (m_iDestinationType == DESTINATION_TEXTURES ? (m_strDestinationDir == "" ? "destination" : "") : ""));
				AfxMessageBox (strText, MB_ICONSTOP);
				return;
			}		

			SetDlgItemText (IDC_TEMP_STATUS, "Retrieving list of files to convert...");

			GetDlgItemText (IDC_EDIT_WILDCARDS, strWildCards);	
		
			sWildCards = strWildCards.GetBuffer();
			boost::trim(sWildCards);  // Strip leading and trailing spaces
			iStrLength = sWildCards.size();			
			if (iStrLength == 0)
			{
				sWildCards = _T("*.*");
			}
			CDirectoryList dirList (g_bRecurseSubdirectories);
			boost::split(vWildCards, sWildCards, boost::is_any_of(" "));
			for( std::string sWC : vWildCards )
			{
				dirList.AddWildcard (sWC.c_str());				
			}
			dirList.SetRoot (m_strSourceDir);
			dirList.SearchDirectories ();

			if( m_iDestinationType == DESTINATION_TEXTURES )
			{
				// Make sure the destination directory is created
				CString strRemainingPath("");
				CString strMkDir("");
				int iPosition = 0;

				strRemainingPath = TrimSlashes (m_strDestinationDir);
							
				while (strRemainingPath != "")
				{
					iPosition = strRemainingPath.Find ("\\");

					if (iPosition == -1)
					{
						strMkDir += strRemainingPath;
						strRemainingPath = "";						
					}
					else
					{
						strMkDir += strRemainingPath.Left(iPosition);
						strMkDir += "\\";							
						strRemainingPath = 	strRemainingPath.Right (strRemainingPath.GetLength() - (iPosition + 1));
					}			
					
					_mkdir (strMkDir);
				}
			}

			if( IsDlgButtonChecked( IDC_RADIO_NEW_PACKAGE ) )
			{
				int iCurSel = m_cbWADType.GetCurSel();

				if( iCurSel == CB_ERR )
				{
					AfxMessageBox( "Did not specify new package type", MB_ICONSTOP );
					return;
				}
				
				g_iMergeNewWADType = m_cbWADType.GetItemData( iCurSel );
				m_pDestinationPackage = theApp.CreatePackageDoc();

				if( m_pDestinationPackage )
				{
					m_pDestinationPackage->SetWADType( g_iMergeNewWADType );					
				}
				else
				{
					AfxMessageBox( "Failed to create new package", MB_ICONSTOP );
				}
			}

			CDirectoryEntry *pDir = dirList.GetFirst();
			CFileList *pList = NULL;
			CFileItem *pFile = NULL;

			while (pDir)
			{
				pList = pDir->GetFileList();
				pFile = pList->GetFirst();

				while (pFile)
				{
					CBatchJob *pJob = new CBatchJob( pFile->GetFileName(), m_strDestinationDir, m_strSourceDir, m_iDestinationType, iImageType, m_pDestinationPackage );
					m_vBatchJobs.push_back(pJob);
					m_ThreadManager.AddJob( (LPVOID)pJob );
					m_iJobCount++;
					SetDlgItemText (IDC_TEMP_STATUS, pFile->GetFileName());
					
					pFile = pFile->GetNext();
				}

				pDir = pDir->GetNext();
			}

			if (m_iJobCount)
			{			
				SetDlgItemText (IDC_BUTTON_GO, "&Stop");
				m_iButtonState = BUTTON_STOP;
				strText.Format ("Found %d files for conversion%s", m_iJobCount, CRLF);
				SetDlgItemText (IDC_TEMP_STATUS, strText);
				m_pbStatus.SetRange (0, m_iJobCount);

				m_ThreadManager.Start();
			}
			else
			{
				SetDlgItemText (IDC_TEMP_STATUS, "Nothing to convert!");		
			}

			break;
		}

	case BUTTON_STOP:		
		//m_pThreadList->SetSpoolerMessage (THREADSPOOLER_STOP);
		m_ThreadManager.Stopped( TRUE );
		strText.Format ("Threads cancelled!%s", CRLF);
		m_strStatusText += strText;
		break;

	default:
		ASSERT(false);		// Missed implementation?
		break;
	}
	EndWaitCursor();
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OLD_BATCH_CONVERSION

void CBatchDlg::OnButtonGo() 
{	
	BeginWaitCursor();
	
	int x = 0;
	int x1 = 0;
	
	CString strText("");
	CString strWildCards("");
	char *szWildCard = NULL;	
	int iStrLength = 0;
	int iStrPosition = 0;

	bool bFilesToConvert = false;
	bool bMoreWildCards = true;
	
	CString strFileName("");
	int iDestType = m_pThreadList->GetDestinationType();
	m_pThreadList->SetImageType (m_cbOutputFormat.GetCurSel());

	switch (m_iButtonState)
	{
	case BUTTON_GO:	
		{			
			m_iJobCount = 0;
			m_iFinishedCount = 0;
			m_strStatusText = "";		

			m_pThreadList->SetMaxThreads (GetMaxThreads());
			GetDlgItemText (IDC_EDIT_DEST_DIR, m_strDestinationDir);
			GetDlgItemText (IDC_EDIT_SOURCE_DIR, m_strSourceDir);	
		
			m_strStatusText = "";
			SetDlgItemText (IDC_EDIT_STATUS, m_strStatusText);
			m_pbStatus.SetRange (0, m_iJobCount);

			bool bNoSource = (m_strSourceDir == "");
			bool bNoDest = (iDestType == DESTINATION_TEXTURES ? m_strDestinationDir == "" : false);

			if (bNoSource || bNoDest) 
			{
				strText.Format ("Please provide a %s%s%s directory.", m_strSourceDir == "" ? "source" : "", ((bNoSource && bNoDest) ? " and " : ""), (iDestType == DESTINATION_TEXTURES ? (m_strDestinationDir == "" ? "destination" : "") : ""));
				AfxMessageBox (strText, MB_ICONSTOP);
				return;
			}		

			SetDlgItemText (IDC_TEMP_STATUS, "Retrieving list of files to convert...");

			GetDlgItemText (IDC_EDIT_WILDCARDS, strWildCards);	
		
			iStrLength = strWildCards.GetLength();
			if (iStrLength <= 0)
			{
				AfxMessageBox ("Nothing to convert!", MB_ICONSTOP);
				return;
			}

			szWildCard = strWildCards.GetBuffer(iStrLength);
			iStrPosition = 0;

			// Strip out any spaces at the front		
			while ((*szWildCard == ' ') && (iStrPosition < iStrLength))
			{
				szWildCard++;
				iStrPosition++;
			}

			if (iStrPosition == iStrLength)
			{
				AfxMessageBox ("Nothing to convert!", MB_ICONSTOP);
				return;
			}

			// Change any other spaces to NULL
			for (x = iStrPosition, x1 = 0; x < iStrLength; x++, x1++)
			{
				if (szWildCard[x1] == ' ')
				{
					szWildCard[x1] = 0;
				}
			}
			CDirectoryList dirList (g_bRecurseSubdirectories);

			while (bMoreWildCards)
			{
				dirList.AddWildcard (szWildCard);
				szWildCard += (strlen(szWildCard) + 1);
				iStrPosition += (strlen(szWildCard) + 1);

				if (iStrPosition >= iStrLength)
				{
					bMoreWildCards = FALSE;
				}				
			}

			strWildCards.ReleaseBuffer();

			dirList.SetRoot (m_strSourceDir);
			dirList.SearchDirectories ();

			// Make sure the destination directory is created			
			CString strRemainingPath("");
			CString strMkDir("");
			int iPosition = 0;

			strRemainingPath = TrimSlashes (m_strDestinationDir);
						
			while (strRemainingPath != "")
			{
				iPosition = strRemainingPath.Find ("\\");

				if (iPosition == -1)
				{
					strMkDir += strRemainingPath;
					strRemainingPath = "";						
				}
				else
				{
					strMkDir += strRemainingPath.Left(iPosition);
					strMkDir += "\\";							
					strRemainingPath = 	strRemainingPath.Right (strRemainingPath.GetLength() - (iPosition + 1));
				}			
				
				_mkdir (strMkDir);
			}				

			CDirectoryEntry *pDir = dirList.GetFirst();
			CFileList *pList = NULL;
			CFileItem *pFile = NULL;

			while (pDir)
			{
				pList = pDir->GetFileList();
				pFile = pList->GetFirst();

				while (pFile)
				{					
					m_pThreadList->AddJob (pFile->GetFileName(), m_strDestinationDir, m_strSourceDir);
					m_iJobCount++;
					SetDlgItemText (IDC_TEMP_STATUS, pFile->GetFileName());
					
					pFile = pFile->GetNext();
				}

				pDir = pDir->GetNext();
			}

			if (m_iJobCount)
			{			
				SetDlgItemText (IDC_BUTTON_GO, "&Stop");
				m_iButtonState = BUTTON_STOP;
				strText.Format ("Found %d files for conversion%s", m_iJobCount, CRLF);
				SetDlgItemText (IDC_TEMP_STATUS, strText);
				m_pbStatus.SetRange (0, m_iJobCount);
				m_pThreadList->SetSpoolerMessage (THREADSPOOLER_START);			
			}
			else
			{
				SetDlgItemText (IDC_TEMP_STATUS, "Nothing to convert!");		
			}

			break;
		}

	case BUTTON_STOP:		
		m_pThreadList->SetSpoolerMessage (THREADSPOOLER_STOP);
		strText.Format ("Threads cancelled!%s", CRLF);
		m_strStatusText += strText;
		break;

	default:
		ASSERT(false);		// Missed implementation?
		break;
	}
	EndWaitCursor();
}

#endif		// #ifdef OLD_BATCH_CONVERSION
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


void CBatchDlg::SpoolerStopped()
{
	CBatchSummaryDlg dlgSummary;	

	dlgSummary.SetTitle ("Conversion Summary");
	dlgSummary.SetStatusText (m_strStatusText);
	dlgSummary.DoModal();
	
	m_strStatusText = "";
	
	SetDlgItemText (IDC_BUTTON_GO, "&Go");
	m_iButtonState = BUTTON_GO;
	m_pbStatus.SetPos (0);
	
	SetDlgItemText (IDC_TEMP_STATUS, "");
}

LRESULT CBatchDlg::OnBatchDlgCustomMessage(WPARAM nType, LPARAM nFlags)
{
	switch( nType )
	{
	case THREAD_MGR_MESSAGE_STOPPED:
		{
			SpoolerStopped();
		}
		break;

	case THREAD_MGR_MESSAGE_STARTED:
		{			
		}
		break;

	case THREAD_MGR_MESSAGE_STATUS:
		{
			m_strStatusText += m_ThreadMessage.GetMessage();
			m_strStatusText += CRLF;
			m_strStatusText += CRLF;
			SetDlgItemText( IDC_TEMP_STATUS, m_ThreadMessage.GetMessage() );
			
			m_iFinishedCount++;
			m_pbStatus.SetPos( m_iFinishedCount );
		}
		break;

	default:
		break;
	}
	return 0;
}

void WINAPI CBatchDlg::ThreadMessageCallBack( CThreadMessage *lpMessage )
{
	if( lpMessage )
	{
		CBatchDlg *pThis = (CBatchDlg *)lpMessage->GetAppSpecific();

		if( pThis )
		{
			pThis->m_ThreadMessage = (*lpMessage);
			
			// We want to just pass this along
			::PostMessage( pThis->m_hWnd, WM_BATCHDLG_CUSTOM, pThis->m_ThreadMessage.GetMessageID(), 0 );		
		}
	}
}

void CBatchDlg::OnButtonViewOptions() 
{
	m_btnOk.SetFocus();
	CWallyPropertySheet propSheet;

	propSheet.m_Page1.m_pDoc = NULL;
	propSheet.m_Page6.m_bCalledFromBatchDlg = TRUE;

	propSheet.SetActivePage (&propSheet.m_Page6);

	propSheet.DoModal();
	SetMaxThreads (g_iMaxConversionThreads);

	theApp.UpdateAllDocs();
	
	UpdateWallyOptions ();
}


void CBatchDlg::UpdateWallyOptions ()
{
	m_strWallyOptions = "";
	CString strText("");

	switch (g_iTextureNameSetting)
	{
	case BLANK_DIR:
		strText = "none";
		break;

	case PRESET_DIR :		
		strText = g_strDefaultTextureName;
		break;

	case PARENT_DIR :
		strText = "parent";
		break;

	default:
		ASSERT (false);		// Unhandled Wally option
		break;
	}

	m_strWallyOptions = "Texture directory:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += strText;
	m_strWallyOptions += "\n";	


	switch (g_iPaletteConversion)
	{
	case PALETTE_CONVERT_MAINTAIN:		
		strText = "Maintain Indexes";
		break;
				
	case PALETTE_CONVERT_NEAREST:
		strText = "Nearest Color";
		break;

	default:
		ASSERT (false);		
		break;
	}

	m_strWallyOptions += "Palette Option:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += strText;
	m_strWallyOptions += "\n";

	m_strWallyOptions += "Export Color Depth:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += g_iExportColorDepth == IH_8BIT ? "8-bit" : "24-bit";
	m_strWallyOptions += "\n";

	m_strWallyOptions += "Recurse Subdirectories:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += g_bRecurseSubdirectories ? "Yes" : "No";
	m_strWallyOptions += "\n";

	m_strWallyOptions += "Retain Directory Structure:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += g_bRetainDirectoryStructure ? "Yes" : "No";
	m_strWallyOptions += "\n";

	m_strWallyOptions += "Overwrite Existing Files:";
	m_strWallyOptions += "\n\t";
	m_strWallyOptions += g_bOverWriteFiles ? "Yes" : "No";
	m_strWallyOptions += "\n";


	SetDlgItemText (IDC_STATIC_TEXTURE_DIR, m_strWallyOptions);

}


void CBatchDlg::AddToFinishedTotal(int iStatus, LPCTSTR szSourceFile, LPCTSTR szDestinationFile, LPCTSTR szErrorMessage)
{
#if 0
	CString strText("");
	CString strStatusText("");
	CString strDest ("");

	CString strSource( GetRawFileNameWExt( szSourceFile ));
	if( m_iDestinationType == DESTINATION_TEXTURES )
	{
		strDest =( GetRawFileNameWExt( szDestinationFile ));
	}
	else
	{
		strDest =( GetRawFileName( szSourceFile ));
	}

	if( iStatus == THREADJOB_SUCCESS )
	{		
		strText.Format ("Convert %s --> %s%s === OK ===%s%s", strSource, strDest, CRLF, CRLF, CRLF);
		strStatusText.Format ("%s-->%s... OK", strSource, strDest);		
	}
	else
	{
		strText.Format ("Convert %s --> %s%s === FAIL ===%s%s%s%s", strSource, strDest, CRLF, CRLF, szErrorMessage, CRLF, CRLF);
		strStatusText.Format ("%s-->%s... FAIL", strSource, strDest);
	}
	
	m_strStatusText += strText;
	SetDlgItemText (IDC_TEMP_STATUS, strStatusText);
	
	m_iFinishedCount++;
#endif
}

void CBatchDlg::UpdateThreadStatus(int iProcessingCount, int iQueuedCount)
{
	if ((iProcessingCount + iQueuedCount) > 0)
	{
		// Do nothing, no processing going on
	}	
	else
	{
		SetDlgItemText (IDC_BUTTON_GO, "&Go");
		m_iButtonState = BUTTON_GO;
		SetDlgItemText (IDC_TEMP_STATUS, "");
	}
	
	m_pbStatus.SetPos (m_iFinishedCount);
}

int CBatchDlg::DoModal() 
{
	int iReturn = CDialog::DoModal();

	/*
	if (m_pThreadList)
	{
		delete m_pThreadList;
		m_pThreadList = NULL;
	}
	*/

	return iReturn;
}

int CBatchDlg::GetMaxThreads()
{
	return m_iMaxThreads;
}

void CBatchDlg::SetMaxThreads(int iMaxThreads)
{
	m_iMaxThreads = iMaxThreads;

	/*
	if (m_pThreadList)
	{
		m_pThreadList->SetMaxThreads (iMaxThreads);
	}
	*/
}

void CBatchDlg::OnOK() 
{	
	g_iFileExportExtension = m_cbOutputFormat.GetCurSel();

	GetDlgItemText (IDC_EDIT_DEST_DIR, g_szDestinationConvertDirectory);
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, g_szSourceConvertDirectory);
	GetDlgItemText (IDC_EDIT_WILDCARDS, g_szConvertWildCard);	

	/*
	m_pThreadList->SetSpoolerMessage(THREADSPOOLER_FINISH);
	while (m_pThreadList->GetSpoolerStatus() != THREADSPOOLER_END_THREAD)
	{
		Sleep (500);	// Wait for all threads to cancel out
	}
	*/
	
	CDialog::OnOK();
}

void CBatchDlg::OnRadioCurrentPackage() 
{
	EnableDisable();
}

void CBatchDlg::EnableDisable()
{
	if (IsDlgButtonChecked( IDC_RADIO_TEXTURES))	
	{
		m_cbOutputFormat.EnableWindow(true);
		m_cbPackages.EnableWindow(false);
		m_cbWADType.EnableWindow( FALSE );
		m_iDestinationType = DESTINATION_TEXTURES;
		//m_pThreadList->SetDestinationType (DESTINATION_TEXTURES);
		m_edDestinationDirectory.EnableWindow(true);
		m_btnDestinationDirectory.EnableWindow(true);
	}
		
	if( IsDlgButtonChecked( IDC_RADIO_CURRENT_PACKAGE ) )
	{
		int iCount = m_cbPackages.GetCount();
		if ((iCount == 0) || (iCount == CB_ERR))
		{
			CheckDlgButton( IDC_RADIO_NEW_PACKAGE, true);
			CheckDlgButton( IDC_RADIO_CURRENT_PACKAGE, false);
			EnableDisable();
			return;
		}

		m_cbOutputFormat.EnableWindow(false);
		m_cbPackages.EnableWindow(true);
		m_cbWADType.EnableWindow( FALSE );
		int iSelection = m_cbPackages.GetCurSel();
		//m_pThreadList->SetDestinationType (DESTINATION_PACKAGE, (CPackageDoc *)(m_cbPackages.GetItemData(iSelection)));
		m_iDestinationType = DESTINATION_PACKAGE;
		m_pDestinationPackage = (CPackageDoc *)(m_cbPackages.GetItemData(iSelection));
		m_edDestinationDirectory.EnableWindow(false);
		m_btnDestinationDirectory.EnableWindow(false);
	}

	if( IsDlgButtonChecked( IDC_RADIO_NEW_PACKAGE ) )	
	{
		m_cbOutputFormat.EnableWindow( FALSE );
		m_cbPackages.EnableWindow( FALSE );
		m_cbWADType.EnableWindow( TRUE );
		m_iDestinationType = DESTINATION_PACKAGE;
		//m_pThreadList->SetDestinationType (DESTINATION_TEXTURES);
		m_edDestinationDirectory.EnableWindow(false);
		m_btnDestinationDirectory.EnableWindow(false);
	}
}

void CBatchDlg::OnRadioTextures() 
{
	EnableDisable();
}

void CBatchDlg::OnSelchangeComboPackage() 
{
	int iSelection = m_cbPackages.GetCurSel();

	if (iSelection != CB_ERR)
	{
		//m_pThreadList->SetDestinationType (DESTINATION_PACKAGE, (CPackageDoc *)(m_cbPackages.GetItemData(iSelection)));
		m_iDestinationType = DESTINATION_PACKAGE;
		m_pDestinationPackage = (CPackageDoc *)(m_cbPackages.GetItemData(iSelection));
	}
}


void CBatchDlg::OnKillfocusEditDestDir() 
{
	int x = 0;
	int iLength = 0;
	
	GetDlgItemText (IDC_EDIT_DEST_DIR, m_strDestinationDir);

	iLength = m_strDestinationDir.GetLength();
	x = iLength - 1;

	while (m_strDestinationDir.GetAt(x) == '\\')
	{
		x--;
	}
	x++;

	m_strDestinationDir = m_strDestinationDir.Left(x);
	SetDlgItemText (IDC_EDIT_DEST_DIR, m_strDestinationDir);
}

void CBatchDlg::OnKillfocusEditSourceDir() 
{	
	int x = 0;
	int iLength = 0;
	
	GetDlgItemText (IDC_EDIT_SOURCE_DIR, m_strSourceDir);

	iLength = m_strSourceDir.GetLength();
	x = iLength - 1;

	while (m_strSourceDir.GetAt(x) == '\\')
	{
		x--;
	}
	x++;

	m_strSourceDir = m_strSourceDir.Left(x);
	SetDlgItemText (IDC_EDIT_SOURCE_DIR, m_strSourceDir);
	
}

void CBatchDlg::OnRadioNewPackage() 
{
	EnableDisable();	
}
