#ifndef _SINRULEFLAGDLG_H_
#define _SINRULEFLAGDLG_H_

#ifndef _SINFLAGSDLG_H_
	#include "SinFlagsDlg.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SinRuleFlagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSinRuleFlagDlg dialog

class CSinRuleFlagDlg : public CDialog
{
// Construction
public:
	CSinRuleFlagDlg(CWnd* pParent = NULL);   // standard constructor

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
	void AddField (LPCTSTR szText, LPCTSTR szDescription, unsigned short *pucValue);
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
	SIN_RULE_S m_SinRuleHeader;
	LPSIN_FIELD m_lpCurrentField;
	BOOL m_bFirstTime;

	CSinFlagsColorWnd m_wndColor;
	BYTE *m_pbyImageData;
	CWallyPalette *m_pPalette;

// Dialog Data
	//{{AFX_DATA(CSinRuleFlagDlg)
	enum { IDD = IDD_SIN_RULE_FLAGS_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSinRuleFlagDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSinRuleFlagDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _SINRULEFLAGDLG_H_
