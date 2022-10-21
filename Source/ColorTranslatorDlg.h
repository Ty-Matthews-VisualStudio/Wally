#if !defined(AFX_COLORTRANSLATORDLG_H__6B6E6D00_ABF2_11D2_8410_D4900FC10000__INCLUDED_)
#define AFX_COLORTRANSLATORDLG_H__6B6E6D00_ABF2_11D2_8410_D4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ColorTranslatorDlg.h : header file
//

#define		COLOR_TRANSLATE_BYINDEX		0
#define		COLOR_TRANSLATE_BYCOLOR		1

/////////////////////////////////////////////////////////////////////////////
// CColorTranslatorDlg dialog

class CColorTranslatorDlg : public CDialog
{
// Members
private:
	int m_iTranslateChoice;
	int m_iSourceIndex;
	int m_iDestIndex;
	int m_iRValue;
	int m_iGValue;
	int m_iBValue;
	bool m_bSwapIndexes;
	unsigned char m_byPalette[768];

// Methods
public:
	void SetPalette (unsigned char *pbyPalette)
	{
		ASSERT (pbyPalette);
		memcpy (m_byPalette, pbyPalette, 768);
	}
	int GetChoice()
	{
		return m_iTranslateChoice;	
	}
	void SetSourceIndex(int iIndex)
	{
		ASSERT ((iIndex >= 0) && (iIndex <= 255));
		m_iSourceIndex = iIndex;
	}
	int GetSourceIndex()
	{
		return m_iSourceIndex;
	}
	void SetDestinationIndex (int iIndex)
	{
		ASSERT ((iIndex >= 0) && (iIndex <= 255));
		m_iDestIndex = iIndex;
	}
	int GetDestinationIndex ()
	{
		return m_iDestIndex;
	}
	COLORREF GetRGB()
	{
		COLORREF rgb (RGB (m_iRValue, m_iGValue, m_iBValue));
		return rgb;
	}
	void SetRGB(COLORREF rgb)
	{
		m_iRValue = GetRValue (rgb);
		m_iGValue = GetGValue (rgb);
		m_iBValue = GetBValue (rgb);
	}
	void OnKillFocusControls(bool bUsePaletteValues = false);
	bool SwapIndexes()
	{
		return m_bSwapIndexes;
	}


// Construction
public:
	CColorTranslatorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorTranslatorDlg)
	enum { IDD = IDD_COLOR_TRANSLATOR_DLG };
	CButton	m_btnSwapIndexes;
	CEdit	m_edSourceIndex;
	CEdit	m_edRed;
	CEdit	m_edGreen;
	CEdit	m_edBlue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorTranslatorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorTranslatorDlg)
	afx_msg void OnRadioIndex();
	afx_msg void OnRadioRgbValue();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusEditBlue();
	afx_msg void OnKillfocusEditGreen();
	afx_msg void OnKillfocusEditRed();
	afx_msg void OnKillfocusEditDestIndex();
	afx_msg void OnKillfocusEditSourceIndex();
	afx_msg void OnCheckSwapIndexes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORTRANSLATORDLG_H__6B6E6D00_ABF2_11D2_8410_D4900FC10000__INCLUDED_)
