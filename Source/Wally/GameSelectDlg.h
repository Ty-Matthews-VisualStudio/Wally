#if !defined(AFX_GAMESELECTDLG_H__8BD5A532_99BB_11D2_BAA3_00104BCBA50D__INCLUDED_)
#define AFX_GAMESELECTDLG_H__8BD5A532_99BB_11D2_BAA3_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GameSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameSelectDlg dialog

#define GAME_SELECT_MSG_PASTE		1
#define GAME_SELECT_MSG_DRAGDROP	2
#define GAME_SELECT_MSG_OPEN		3

class CGameSelectDlg : public CDialog
{
// Construction
public:
	CGameSelectDlg(CWnd* pParent = NULL);   // standard constructor

private:
	int m_iMessageType;
	int m_iFileType;

public:
	void SetMessageType (int iType)
	{
		m_iMessageType = iType;
	}
	int GetMessageType ()
	{
		return m_iMessageType;
	}
	int GetFileType()
	{
		return m_iFileType;
	}

// Dialog Data
	//{{AFX_DATA(CGameSelectDlg)
	enum { IDD = IDD_SELECT_GAME_DLG };
	CComboBox	m_cbFileType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGameSelectDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMESELECTDLG_H__8BD5A532_99BB_11D2_BAA3_00104BCBA50D__INCLUDED_)
