// NewPackageImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "NewPackageImageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewPackageImageDlg dialog


CNewPackageImageDlg::CNewPackageImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewPackageImageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewPackageImageDlg)
	//}}AFX_DATA_INIT
}


void CNewPackageImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewPackageImageDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edName);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spnWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spnHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewPackageImageDlg, CDialog)
	//{{AFX_MSG_MAP(CNewPackageImageDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	ON_EN_UPDATE(IDC_EDIT_NAME, OnUpdateEditName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewPackageImageDlg message handlers

BOOL CNewPackageImageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UDACCEL SpinAccel[4];

	SpinAccel[0].nSec = 0;
	SpinAccel[0].nInc = 16;
	SpinAccel[1].nSec = 1;
	SpinAccel[1].nInc = 32;
	SpinAccel[2].nSec = 2;
	SpinAccel[2].nInc = 64;
	SpinAccel[3].nSec = 3;
	SpinAccel[3].nInc = 128;
	
	m_spnWidth.SetAccel (4,&SpinAccel[0]);				
	m_spnHeight.SetAccel (4,&SpinAccel[0]);				

	m_spnWidth.SetRange (16, MAX_TEXTURE_WIDTH);
	m_spnHeight.SetRange (16, MAX_TEXTURE_HEIGHT);

	CString strText ("");
		
	if (g_bRetainTextureSize)
	{		
		strText.Format ("%d", g_iPreviousTextureWidth);
		SetDlgItemText (IDC_EDIT_WIDTH, strText);

		strText.Format ("%d", g_iPreviousTextureHeight);
		SetDlgItemText (IDC_EDIT_HEIGHT, strText);	
	}
	else
	{	
		strText.Format ("%d", g_iDefaultTextureWidth);
		SetDlgItemText (IDC_EDIT_WIDTH, strText);

		strText.Format ("%d", g_iDefaultTextureHeight);
		SetDlgItemText (IDC_EDIT_HEIGHT, strText);	
	}	
	
	m_btnOK.EnableWindow (FALSE);
	m_edName.SetFocus ();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE	
}

void CNewPackageImageDlg::OnKillfocusEditHeight() 
{
	CString strText ("");
	int iHeight = 0;
	div_t x;

	GetDlgItemText (IDC_EDIT_HEIGHT, strText);
	iHeight = atol (strText);

	x = div (iHeight + 15, 16);
	iHeight = x.quot * 16;

	iHeight = max (iHeight, 16);
	iHeight = min (iHeight, MAX_TEXTURE_HEIGHT);

	g_iPreviousTextureHeight  = iHeight;
	SetHeight (iHeight);

	strText.Format ("%d", g_iPreviousTextureHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strText);
	
}

void CNewPackageImageDlg::OnKillfocusEditWidth() 
{
	CString strText ("");
	int iWidth = 0;
	div_t x;

	GetDlgItemText (IDC_EDIT_WIDTH, strText);
	iWidth = atol (strText);

	x = div (iWidth + 15, 16);
	iWidth = x.quot * 16;

	iWidth = max (iWidth, 16);
	iWidth = min (iWidth, MAX_TEXTURE_WIDTH);

	g_iPreviousTextureWidth  = iWidth;
	SetWidth (iWidth);
	
	strText.Format ("%d", g_iPreviousTextureWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);
	
}

void CNewPackageImageDlg::OnOK() 
{
	OnKillfocusEditWidth();
	OnKillfocusEditHeight();
	ValidateNameField();

	CDialog::OnOK();
}

void CNewPackageImageDlg::OnUpdateEditName() 
{
	CString strText ("");
	GetDlgItemText (IDC_EDIT_NAME, strText);

	m_btnOK.EnableWindow (strText.GetLength() > 0);	
}

void CNewPackageImageDlg::ValidateNameField()
{
	CString strTemp ("");
	GetDlgItemText (IDC_EDIT_NAME, strTemp);
	int iLength = strTemp.GetLength();
	m_strName = "";

	// HL texture names can't have spaces
	for (int j = 0; j < iLength; j++)
	{
		if (strTemp.GetAt(j) != ' ')
		{
			m_strName += strTemp.GetAt(j);
		}
	}
}
