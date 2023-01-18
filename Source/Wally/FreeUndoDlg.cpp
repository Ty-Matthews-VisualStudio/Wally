// FreeUndoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Wally.h"
#include "WallyDoc.h"
#include "WallyUndo.h"
#include "FreeUndoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CWallyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFreeUndoDlg dialog


CFreeUndoDlg::CFreeUndoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFreeUndoDlg::IDD, pParent)
{
	m_pDoc = NULL;

	//{{AFX_DATA_INIT(CFreeUndoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFreeUndoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFreeUndoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFreeUndoDlg, CDialog)
	//{{AFX_MSG_MAP(CFreeUndoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreeUndoDlg message handlers

BOOL CFreeUndoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CheckDlgButton( IDC_RADIO_DONT_DO_IT, TRUE);

	CWnd* pCtrl = GetDlgItem( IDC_RADIO_JUST_DOC);

	if (pCtrl)
		pCtrl->EnableWindow( m_pDoc != NULL);


	pCtrl = GetDlgItem( IDC_RADIO_ALL_EXCEPT);

	if (pCtrl)
	{
		int iCount = 0;

		POSITION Pos = theApp.GetFirstDocTemplatePosition();

		while (Pos != NULL)
		{
			CDocTemplate* pTemplate = theApp.GetNextDocTemplate( Pos);

			POSITION PosDoc = pTemplate->GetFirstDocPosition();

			while (PosDoc != NULL)
			{
				CDocument* pDoc = pTemplate->GetNextDoc( PosDoc);

				CWallyDoc* pWallyDoc = DYNAMIC_DOWNCAST( CWallyDoc, pDoc);

				if (pWallyDoc)
					iCount++;
			}
		}

		pCtrl->EnableWindow( iCount >= 2);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFreeUndoDlg::OnOK() 
{
	if (IsDlgButtonChecked( IDC_RADIO_ALL_DOCS) ||
			(IsDlgButtonChecked( IDC_RADIO_ALL_EXCEPT)))
	{
		CWallyDoc* pCurrentDoc = m_pDoc;

		if (IsDlgButtonChecked( IDC_RADIO_ALL_DOCS))
			pCurrentDoc = NULL;		// NULL won't match any open doc!

		POSITION Pos = theApp.GetFirstDocTemplatePosition();

		while (Pos != NULL)
		{
			CDocTemplate* pTemplate = theApp.GetNextDocTemplate( Pos);

			POSITION PosDoc = pTemplate->GetFirstDocPosition();

			while (PosDoc != NULL)
			{
				CDocument* pDoc = pTemplate->GetNextDoc( PosDoc);

				CWallyDoc* pWallyDoc = DYNAMIC_DOWNCAST( CWallyDoc, pDoc);

				if (pWallyDoc && (pWallyDoc != pCurrentDoc))
				{
					pWallyDoc->m_Undo.ClearUndoBuffer();
					pWallyDoc->m_Undo.ClearRedoBuffer();
				}
			}
		}
	}
	else if (IsDlgButtonChecked( IDC_RADIO_JUST_DOC))
	{
		if (m_pDoc)
		{
			m_pDoc->m_Undo.ClearUndoBuffer();
			m_pDoc->m_Undo.ClearRedoBuffer();
		}
		else
		{
			MessageBeep( MB_ICONEXCLAMATION);
			ASSERT( FALSE);
		}
	}
	
	CDialog::OnOK();
}
