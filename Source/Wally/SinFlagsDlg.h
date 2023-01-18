#ifndef _SINFLAGSDLG_H_
#define _SINFLAGSDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SinFlagsDlg.h : header file
//

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

#include "SinFlagsColorWnd.h"

#define SIN_FIELD_TYPE_UC		0
#define SIN_FIELD_TYPE_UINT		1
#define SIN_FIELD_TYPE_FLOAT	2

#define SIN_CHECK_FLAGS			0
#define SIN_CHECK_CONTENTS		1

typedef struct
{
	int iType;
	unsigned short *pucValue;
	unsigned int *puiValue;
	float *pfValue;
	int iPredefinedCount;
	unsigned int *piPreDefine;
	char szDescription[256];	
} SIN_FIELD, *LPSIN_FIELD;

#define SIN_FIELD_SIZE  sizeof (SIN_FIELD)

class CWallyPalette;

/////////////////////////////////////////////////////////////////////////////
// CSinFlagsDlg dialog

class CSinFlagsDlg : public CDialog
{
// Construction
public:
	CSinFlagsDlg(CWnd* pParent = NULL);   // standard constructor

// Methods
public:
	void SetSinHeader (LPSIN_MIP_S lpSinHeader)
	{
		ASSERT (lpSinHeader);
		memcpy (&m_SinHeader, lpSinHeader, SIN_HEADER_SIZE);
	}
	LPSIN_MIP_S GetSinHeader()
	{
		return &m_SinHeader;
	}
	void SetDocData (BYTE *pbyImageData, CWallyPalette *pPalette)
	{
		m_pbyImageData = pbyImageData;
		m_pPalette = pPalette;
	}

private:
	void AddSurfaceType (LPCTSTR szText, int iValue);
	void AddField (LPCTSTR szText, LPCTSTR szDescription, unsigned short *pucValue, int iPredefinedCount = 0, unsigned int *pPredefined = NULL);
	void AddField (LPCTSTR szText, LPCTSTR szDescription, unsigned int *puiValue, int iPredefinedCount = 0, unsigned int *pPredefined = NULL);
	void AddField (LPCTSTR szText, LPCTSTR szDescription, float *pfValue);	
	void UpdateColorFields();
	void ProcessChecks(BOOL bSetting = TRUE);	
	void ProcessCheck (BOOL bSetting, int iID, int iType, int iValue);
	void InitSurfaceTypes();
	void InitFields();
	void Update();

// Members
private:
	SIN_MIP_S m_SinHeader;
	LPSIN_FIELD m_lpCurrentField;
	BOOL m_bFirstTime;

	CSinFlagsColorWnd m_wndColor;
	BYTE *m_pbyImageData;
	CWallyPalette *m_pPalette;

// Dialog Data
	//{{AFX_DATA(CSinFlagsDlg)
	enum { IDD = IDD_SIN_FLAGS_DLG };
	CComboBox	m_cbValue;
	CEdit	m_edValue;
	CEdit	m_edColorB;
	CEdit	m_edColorG;
	CEdit	m_edColorR;
	CButton	m_btnAuto;
	CListBox	m_lbFields;
	CComboBox	m_cbSurfaceTypes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSinFlagsDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSinFlagsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListFields();
	afx_msg void OnKillfocusEditName();
	afx_msg void OnKillfocusEditAnimname();
	virtual void OnOK();
	afx_msg void OnKillfocusEditValue();
	afx_msg void OnKillfocusEditColorB();
	afx_msg void OnKillfocusEditColorG();
	afx_msg void OnKillfocusEditColorR();
	afx_msg void OnPaint();
	afx_msg LRESULT OnChangeColor (WPARAM nType, LPARAM nFlags);
	afx_msg void OnButtonAuto();
	afx_msg void OnKillfocusComboValue();
	afx_msg BOOL OnToolTipNotify ( UINT id, NMHDR * pTTTStruct, LRESULT * pResult );	
	afx_msg void OnButtonDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _SINFLAGSDLG_H_
