#if !defined(AFX_GAMEPALETTEDLG_H__EF5380F5_E070_11D2_BB04_00104BCBA50D__INCLUDED_)
#define AFX_GAMEPALETTEDLG_H__EF5380F5_E070_11D2_BB04_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GamePaletteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGamePaletteDlg dialog

class CGamePaletteDlg : public CDialog
{
// Construction
public:
	CGamePaletteDlg(CWnd* pParent = NULL);   // standard constructor
	int GetGameSelection();

// Members
private:
	int m_iGameSelection;



// Dialog Data
	//{{AFX_DATA(CGamePaletteDlg)
	enum { IDD = IDD_GAME_PALETTE_DLG };
	CComboBox	m_cbGame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGamePaletteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGamePaletteDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEPALETTEDLG_H__EF5380F5_E070_11D2_BB04_00104BCBA50D__INCLUDED_)
