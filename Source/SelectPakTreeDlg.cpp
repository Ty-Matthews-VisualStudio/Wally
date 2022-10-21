// SelectPakTreeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "SelectPakTreeDlg.h"
#include "PakList.h"
#include "PakDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPakTreeDlg dialog


CSelectPakTreeDlg::CSelectPakTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPakTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectPakTreeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bImageListInitialized = FALSE;
	SetDocument (NULL);
	SetPakDirectory (NULL);

	m_iBlankIcon = -1;
	m_iClosedFolderIcon = -1;
	m_iOpenFolderIcon = -1;
}


void CSelectPakTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPakTreeDlg)
	DDX_Control(pDX, IDC_PAK_TREE, m_TreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectPakTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectPakTreeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSelectPakTreeDlg::InitializeImageList()
{
	m_bImageListInitialized = TRUE;

	GetTreeCtrl().SetImageList (NULL, 0);

	m_ImageList.Create( GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON), 
		ILC_COLOR24, 50, 50);

	m_ImageList.SetBkColor( GetSysColor(COLOR_WINDOW) );

	CBitmap Bmp;
    CBitmap *pOld;
    CBitmap *pNewBmp;
    CDC DC;

    DC.CreateCompatibleDC(NULL);
    Bmp.CreateBitmap (32,32,1,24,NULL);
    pOld=DC.SelectObject (&Bmp);
    
	DC.FillSolidRect (0,0,31,31,GetSysColor(COLOR_WINDOW));
    pNewBmp = DC.SelectObject (pOld);
    DC.DeleteDC ();
    
	m_iBlankIcon = m_ImageList.Add (pNewBmp, COLORREF(0x00));
	
	HICON hIcon = NULL;
	ExtractIconEx( "shell32.dll", 3, NULL, &hIcon, 1);
	m_iClosedFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	ExtractIconEx( "shell32.dll", 4, NULL, &hIcon, 1);
	m_iOpenFolderIcon = m_ImageList.Add (hIcon);
	DestroyIcon (hIcon);

	GetTreeCtrl().SetImageList (&m_ImageList, LVSIL_NORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// CSelectPakTreeDlg message handlers

BOOL CSelectPakTreeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_bImageListInitialized)
	{
		InitializeImageList();
	}

	ASSERT (GetDocument());

	CString strCurrentDirectory("");
	CPakDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CPakDirectory *pDirectory = pDoc->GetRootDirectory();

		pDirectory->BuildTree (GetTreeCtrl(), TVI_ROOT, m_iClosedFolderIcon, m_iOpenFolderIcon);

		// Always expand the root item
		HTREEITEM htItem = GetTreeCtrl().GetRootItem();
		GetTreeCtrl().Expand (htItem, TVE_EXPAND);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectPakTreeDlg::OnOK() 
{
	HTREEITEM htItem = GetTreeCtrl().GetSelectedItem();

	TV_ITEM tvItem;
	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = htItem;

	if (GetTreeCtrl().GetItem (&tvItem))
	{
		SetPakDirectory ((CPakDirectory *)tvItem.lParam);
	}
		
	CDialog::OnOK();
}
