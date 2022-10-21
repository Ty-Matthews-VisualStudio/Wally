// EnlargeCanvasDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "EnlargeCanvasDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnlargeCanvasDlg dialog


CEnlargeCanvasDlg::CEnlargeCanvasDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnlargeCanvasDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnlargeCanvasDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	SetDimensions (0,0);

	m_bCenterHorizontally = g_bEnlargeCenterHorizontally;
	m_bCenterVertically = g_bEnlargeCenterVertically;
}


void CEnlargeCanvasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnlargeCanvasDlg)
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spnWidth);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spnHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnlargeCanvasDlg, CDialog)
	//{{AFX_MSG_MAP(CEnlargeCanvasDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_HEIGHT, OnKillfocusEditHeight)
	ON_EN_KILLFOCUS(IDC_EDIT_WIDTH, OnKillfocusEditWidth)
	ON_BN_CLICKED(IDC_CHECK_HORIZONTAL, OnCheckHorizontal)
	ON_BN_CLICKED(IDC_CHECK_VERTICAL, OnCheckVertical)
	ON_BN_CLICKED(IDC_BUTTON_AUTO16, OnButtonAuto16)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnlargeCanvasDlg message handlers

BOOL CEnlargeCanvasDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strText("");
	strText.Format ("%d", m_iWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);

	strText.Format ("%d", m_iHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strText);

	#define NUM_ACCELS		10

	UDACCEL spnAccel[NUM_ACCELS];

	for (int j = 0; j < NUM_ACCELS; j++)
	{
		spnAccel[j].nSec = j;
		spnAccel[j].nInc = (int)(pow (2, j));	}
	
	m_spnWidth.SetAccel (NUM_ACCELS, &spnAccel[0]);
	m_spnHeight.SetAccel (NUM_ACCELS, &spnAccel[0]);

	m_spnWidth.SetRange (1, MAX_TEXTURE_WIDTH);
	m_spnHeight.SetRange (1, MAX_TEXTURE_HEIGHT);

	CheckDlgButton (IDC_CHECK_HORIZONTAL, m_bCenterHorizontally);
	CheckDlgButton (IDC_CHECK_VERTICAL, m_bCenterVertically);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEnlargeCanvasDlg::OnOK() 
{
	OnKillfocusEditWidth();
	OnKillfocusEditHeight();
	CDialog::OnOK();
}

void CEnlargeCanvasDlg::OnKillfocusEditHeight() 
{
	CString strValue("");
	GetDlgItemText (IDC_EDIT_HEIGHT, strValue);

	m_iHeight = atoi (strValue);
	m_iHeight = min (m_iHeight, MAX_TEXTURE_HEIGHT);
	m_iHeight = max (m_iHeight, 1);

	strValue.Format ("%d", m_iHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strValue);
}

void CEnlargeCanvasDlg::OnKillfocusEditWidth() 
{
	CString strValue("");
	GetDlgItemText (IDC_EDIT_WIDTH, strValue);

	m_iWidth = atoi (strValue);
	m_iWidth = min (m_iWidth, MAX_TEXTURE_WIDTH);
	m_iWidth = max (m_iWidth, 1);

	strValue.Format ("%d", m_iWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strValue);
}

BOOL CEnlargeCanvasDlg::CenterHorizontally()
{
	return m_bCenterHorizontally;
}

BOOL CEnlargeCanvasDlg::CenterVertically()
{
	return m_bCenterVertically;
}

void CEnlargeCanvasDlg::OnCheckHorizontal() 
{
	m_bCenterHorizontally = IsDlgButtonChecked (IDC_CHECK_HORIZONTAL);	
}

void CEnlargeCanvasDlg::OnCheckVertical() 
{
	m_bCenterVertically = IsDlgButtonChecked (IDC_CHECK_VERTICAL);	
}

void CEnlargeCanvasDlg::OnButtonAuto16() 
{
	OnKillfocusEditWidth();
	OnKillfocusEditHeight();
	
	div_t dtX;
	CString strText("");

	dtX = div (m_iWidth + 15, 16);
	m_iWidth = dtX.quot * 16;
	strText.Format ("%d", m_iWidth);
	SetDlgItemText (IDC_EDIT_WIDTH, strText);

	dtX = div (m_iHeight + 15, 16);
	m_iHeight = dtX.quot * 16;
	strText.Format ("%d", m_iHeight);
	SetDlgItemText (IDC_EDIT_HEIGHT, strText);
}
