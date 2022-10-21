#ifndef _SELECTPAKTREEDLG_H_
#define _SELECTPAKTREEDLG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectPakTreeDlg.h : header file
//

class CPakDoc;
class CPakDirectory;

/////////////////////////////////////////////////////////////////////////////
// CSelectPakTreeDlg dialog

class CSelectPakTreeDlg : public CDialog
{
private:
	BOOL m_bImageListInitialized;
	CImageList m_ImageList;
	CPakDoc *m_pDoc;
	CPakDirectory *m_pPakDirectory;

	int m_iBlankIcon;
	int m_iClosedFolderIcon;
	int m_iOpenFolderIcon;	

// Construction
public:
	CSelectPakTreeDlg(CWnd* pParent = NULL);   // standard constructor
	void InitializeImageList();
	CTreeCtrl &GetTreeCtrl()
	{
		return m_TreeCtrl;
	}
	void SetDocument(CPakDoc *pDoc)
	{
		m_pDoc = pDoc;
	}
	CPakDoc *GetDocument()
	{
		return m_pDoc;
	}

	void SetPakDirectory(CPakDirectory *pPakDirectory)
	{
		m_pPakDirectory = pPakDirectory;
	}
	CPakDirectory *GetPakDirectory()
	{
		return m_pPakDirectory;
	}

// Dialog Data
	//{{AFX_DATA(CSelectPakTreeDlg)
	enum { IDD = IDD_PAK_TREE_DLG };
	CTreeCtrl	m_TreeCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPakTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectPakTreeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _SELECTPAKTREEDLG_H_
