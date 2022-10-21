#if !defined(AFX_PALETTENAMEDLG_H__6D5FB0A0_8D30_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_PALETTENAMEDLG_H__6D5FB0A0_8D30_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PaletteNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaletteNameDlg dialog

class CPaletteNameDlg : public CDialog
{
// Members
private:
	CString m_strName;

// Construction
public:
	CPaletteNameDlg(CWnd* pParent = NULL, LPCTSTR szName = NULL);   // standard constructor

// Methods
public:
	CString GetName()
	{
		return m_strName;
	}
	

// Dialog Data
	//{{AFX_DATA(CPaletteNameDlg)
	enum { IDD = IDD_PALETTE_NAME_DLG };
	CEdit	m_edName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaletteNameDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALETTENAMEDLG_H__6D5FB0A0_8D30_11D2_8410_F4900FC10000__INCLUDED_)
