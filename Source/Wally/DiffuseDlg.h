#if !defined(AFX_DIFFUSEDLG_H__C9F512A1_79C7_11D2_ACC5_444553540000__INCLUDED_)
#define AFX_DIFFUSEDLG_H__C9F512A1_79C7_11D2_ACC5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DiffuseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDiffuseDlg dialog

class CDiffuseDlg : public CDialog
{
// Construction
public:
	CDiffuseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiffuseDlg)
	enum { IDD = IDD_DIFFUSE_DLG };
	int		m_iDiffuseAmount;
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;

	long    m_lRandomSeed;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiffuseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDiffuseDlg)
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIFFUSEDLG_H__C9F512A1_79C7_11D2_ACC5_444553540000__INCLUDED_)
