#ifndef _ENLARGECANVASDLG_H_
#define _ENLARGECANVASDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// EnlargeCanvasDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnlargeCanvasDlg dialog

class CEnlargeCanvasDlg : public CDialog
{
// Construction
public:
	CEnlargeCanvasDlg(CWnd* pParent = NULL);   // standard constructor

	void SetDimensions (int iWidth, int iHeight)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;
	}
	void GetDimensions (int *piWidth, int *piHeight)
	{
		ASSERT (piWidth);
		ASSERT (piHeight);

		(*piWidth) = m_iWidth;
		(*piHeight) = m_iHeight;
	}

	BOOL CenterHorizontally();
	BOOL CenterVertically();


private:
	int m_iWidth;
	int m_iHeight;
	BOOL m_bCenterHorizontally;
	BOOL m_bCenterVertically;

// Dialog Data
	//{{AFX_DATA(CEnlargeCanvasDlg)
	enum { IDD = IDD_ENLARGE_CANVAS_DLG };
	CSpinButtonCtrl	m_spnWidth;
	CSpinButtonCtrl	m_spnHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnlargeCanvasDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnlargeCanvasDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditHeight();
	afx_msg void OnKillfocusEditWidth();
	afx_msg void OnCheckHorizontal();
	afx_msg void OnCheckVertical();
	afx_msg void OnButtonAuto16();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _ENLARGECANVASDLG_H_
