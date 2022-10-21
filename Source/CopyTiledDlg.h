#if !defined(AFX_COPYTILEDDLG_H__757E05C1_CA76_11D1_8068_5C2203C10627__INCLUDED_)
#define AFX_COPYTILEDDLG_H__757E05C1_CA76_11D1_8068_5C2203C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CopyTiledDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCopyTiledDlg dialog

class CCopyTiledDlg : public CDialog
{
// Construction
public:
	CCopyTiledDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCopyTiledDlg)
	enum { IDD = IDD_COPYTILED };
	CSpinButtonCtrl	m_ctrlSpinVertical;
	CSpinButtonCtrl	m_ctrlSpinHorizontal;
	int		m_iHorizontalTiles;
	int		m_iVerticalTiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyTiledDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCopyTiledDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COPYTILEDDLG_H__757E05C1_CA76_11D1_8068_5C2203C10627__INCLUDED_)
