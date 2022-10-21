#ifndef _MIRRORDLG_H_
#define _MIRRORDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MirrorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMirrorDlg dialog

class CMirrorDlg : public CDialog
{
// Construction
public:
	CMirrorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMirrorDlg)
	enum { IDD = IDD_MIRROR_DLG };
	//}}AFX_DATA

	CLayer* m_pLayer;
	BOOL    m_bPreviewApplied;
	int     m_iMirrorType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMirrorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMirrorDlg)
	afx_msg void OnFilterPreview();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _MIRRORDLG_H_
