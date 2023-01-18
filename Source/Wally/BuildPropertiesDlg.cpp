// BuildPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "BuildPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBuildPropertiesDlg dialog


CBuildPropertiesDlg::CBuildPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBuildPropertiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBuildPropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_iTileNumber = 0;
	m_iNumAnimateTiles = 0;
	m_iAnimateType = 0;
	m_iAnimateSpeed = 0;
	m_XOffset = 0;
	m_YOffset = 0;
}


void CBuildPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBuildPropertiesDlg)
	DDX_Control(pDX, IDC_COMBO_ANIMATE_TYPE, m_cbAnimateType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBuildPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CBuildPropertiesDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_ANIMATE_COUNT, OnKillfocusEditAnimateCount)
	ON_EN_KILLFOCUS(IDC_EDIT_ANIMATE_SPEED, OnKillfocusEditAnimateSpeed)
	ON_EN_KILLFOCUS(IDC_EDIT_XOFFSET, OnKillfocusEditXoffset)
	ON_EN_KILLFOCUS(IDC_EDIT_YOFFSET, OnKillfocusEditYoffset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuildPropertiesDlg message handlers

BOOL CBuildPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cbAnimateType.AddString ("None");
	m_cbAnimateType.AddString ("Oscillate");
	m_cbAnimateType.AddString ("Forward");
	m_cbAnimateType.AddString ("Reverse");

	CString strText ("");
	strText.Format ("%d", m_iTileNumber);
	SetDlgItemText (IDC_EDIT_TILENUM, strText);

	strText.Format ("%d", m_iNumAnimateTiles);
	SetDlgItemText (IDC_EDIT_ANIMATE_COUNT, strText);

	strText.Format ("%d", m_iAnimateSpeed);
	SetDlgItemText (IDC_EDIT_ANIMATE_SPEED, strText);
	
	strText.Format ("%d", m_XOffset);
	SetDlgItemText (IDC_EDIT_XOFFSET, strText);
	
	strText.Format ("%d", m_YOffset);
	SetDlgItemText (IDC_EDIT_YOFFSET, strText);

	m_cbAnimateType.SetCurSel (m_iAnimateType);

	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBuildPropertiesDlg::OnOK() 
{
	OnKillfocusEditAnimateCount();
	OnKillfocusEditAnimateSpeed();
	OnKillfocusEditXoffset();
	OnKillfocusEditYoffset();
	
	m_iAnimateType = m_cbAnimateType.GetCurSel ();

	CDialog::OnOK();
}

void CBuildPropertiesDlg::OnKillfocusEditAnimateCount() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_ANIMATE_COUNT, strText);

	m_iNumAnimateTiles = atol(strText);
	m_iNumAnimateTiles = max (m_iNumAnimateTiles, 0);
	m_iNumAnimateTiles = min (m_iNumAnimateTiles, 63);

	strText.Format ("%d", m_iNumAnimateTiles);

	SetDlgItemText (IDC_EDIT_ANIMATE_COUNT, strText);
}

void CBuildPropertiesDlg::OnKillfocusEditAnimateSpeed() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_ANIMATE_SPEED, strText);

	m_iAnimateSpeed = atol(strText);
	m_iAnimateSpeed = max (m_iAnimateSpeed, 0);
	m_iAnimateSpeed = min (m_iAnimateSpeed, 15);

	strText.Format ("%d", m_iAnimateSpeed);

	SetDlgItemText (IDC_EDIT_ANIMATE_SPEED, strText);
	
}

void CBuildPropertiesDlg::OnKillfocusEditXoffset() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_XOFFSET, strText);

	int iOffset = atol(strText);	
	iOffset = max (iOffset, -128);
	iOffset = min (iOffset, 127);

	m_XOffset = (signed char)iOffset;

	strText.Format ("%d", iOffset);

	SetDlgItemText (IDC_EDIT_XOFFSET, strText);
}

void CBuildPropertiesDlg::OnKillfocusEditYoffset() 
{
	CString strText("");
	GetDlgItemText (IDC_EDIT_YOFFSET, strText);

	int iOffset = atol(strText);	
	iOffset = max (iOffset, -128);
	iOffset = min (iOffset, 127);

	m_YOffset = (signed char)iOffset;

	strText.Format ("%d", iOffset);

	SetDlgItemText (IDC_EDIT_YOFFSET, strText);
	
}
