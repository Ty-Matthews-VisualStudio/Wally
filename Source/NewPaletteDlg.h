#if !defined(AFX_NEWPALETTEDLG_H__25F18560_8BC1_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_NEWPALETTEDLG_H__25F18560_8BC1_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NewPaletteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewPaletteDlg dialog

class CNewPaletteDlg : public CDialog
{
// Members
private:
	CString m_strPaletteName;

// Construction
public:
	CNewPaletteDlg(CWnd* pParent = NULL);   // standard constructor
	CString GetPaletteName ()
	{
		return m_strPaletteName;
	}

// Dialog Data
	//{{AFX_DATA(CNewPaletteDlg)
	enum { IDD = IDD_NEW_PALETTE_DLG };
	CEdit	m_edPaletteName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPaletteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPaletteDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPALETTEDLG_H__25F18560_8BC1_11D2_8410_F4900FC10000__INCLUDED_)
