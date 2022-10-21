#if !defined(AFX_BATCHADVANCEDDLG_H__B46F1100_CF40_11D2_A7F3_0000C0D88D2D__INCLUDED_)
#define AFX_BATCHADVANCEDDLG_H__B46F1100_CF40_11D2_A7F3_0000C0D88D2D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BatchAdvancedDlg.h : header file
//

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchAdvancedDlg dialog

class CBatchAdvancedDlg : public CDialog
{
// Construction
public:
	CBatchAdvancedDlg(CWnd* pParent = NULL);   // standard constructor
	void EnableDisableControls();
	CString SelectPalette ();

private:
	CImageHelper m_ihHelper;

// Dialog Data
	//{{AFX_DATA(CBatchAdvancedDlg)
	enum { IDD = IDD_BATCH_ADV_DLG };
	CButton	m_btnDest;
	CButton	m_btnSource;
	CEdit	m_edDestPalette;
	CEdit	m_edSourcePalette;
	CComboBox	m_cbImages;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchAdvancedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchAdvancedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboImageType();
	afx_msg void OnRadioDestCurrent();
	afx_msg void OnRadioDestCustom();
	afx_msg void OnRadioSourceCurrent();
	afx_msg void OnRadioSourceCustom();
	afx_msg void OnButtonDest();
	afx_msg void OnButtonSource();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHADVANCEDDLG_H__B46F1100_CF40_11D2_A7F3_0000C0D88D2D__INCLUDED_)
