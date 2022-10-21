#ifndef _BUILD_PROPERTIESDLG_H_
#define _BUILD_PROPERTIESDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BuildPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBuildPropertiesDlg dialog

class CBuildPropertiesDlg : public CDialog
{
// Construction
public:
	CBuildPropertiesDlg(CWnd* pParent = NULL);   // standard constructor

// Members
private:
	int m_iTileNumber;
	int m_iNumAnimateTiles;
	int m_iAnimateType;
	int m_iAnimateSpeed;
	signed char m_XOffset;
	signed char m_YOffset;

// Methods
public:
	void SetTileNumber (int iTileNumber)
	{
		m_iTileNumber = iTileNumber;
	}
	void SetNumAnimateTiles (int iNumTiles)
	{
		m_iNumAnimateTiles = iNumTiles;
	}
	int GetNumAnimateTiles ()
	{
		return m_iNumAnimateTiles;
	}
	void SetAnimateType (int iAnimateType)
	{
		m_iAnimateType = iAnimateType;
	}
	int GetAnimateType ()
	{
		return m_iAnimateType;
	}
	void SetXOffset (signed char cOffset)
	{
		m_XOffset = cOffset;
	}
	signed char GetXOffset ()
	{
		return m_XOffset;
	}
	void SetYOffset (signed char cOffset)
	{
		m_YOffset = cOffset;
	}
	signed char GetYOffset ()
	{
		return m_YOffset;
	}
	void SetAnimateSpeed (int iSpeed)
	{
		m_iAnimateSpeed = iSpeed;
	}
	int GetAnimateSpeed ()
	{
		return m_iAnimateSpeed;
	}



// Dialog Data
	//{{AFX_DATA(CBuildPropertiesDlg)
	enum { IDD = IDD_BUILD_PROPERTIES_DLG };
	CComboBox	m_cbAnimateType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBuildPropertiesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditAnimateCount();
	afx_msg void OnKillfocusEditAnimateSpeed();
	afx_msg void OnKillfocusEditXoffset();
	afx_msg void OnKillfocusEditYoffset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // ifndef _BUILD_PROPERTIESDLG_H_
