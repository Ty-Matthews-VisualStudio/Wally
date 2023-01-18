
#ifndef _STABCTRL_H_
#define _STABCTRL_H_

#include <afxtempl.h>



// STabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSTabCtrl window
//
// Implements a tab control that automatically handles the showing and hiding
// of objects attached to a tab's various pages eliminating the need to do so
// via the ON_NOTIFY(TCN_SELCHANGE... ) message.
//
// 1.	Simply replace any instance of a CTabCtrl with CSTabCtrl,
//		initialize it as you would an MFC CTabCtrl.
// 2.	Use the AttachControlToTab member to attach you objects 
//		to the various avaliable pages.
// 
// 3.	(optional) Use the SetCurSel member to programaticly show
//		a tabs particular page.
//
// Once done the tab control will show and hide the attached objects depending
// on the users tab selection.

// example of CSTabCtrl's use.
//
//

/*
	// file : SomeDialogClass.h

	class CSomeDialogClass : public CDialog
	{
		CSTabCtrl m_TabCtrl;
		CTreeCtrl m_TreeCtrl;
		CListCtrl m_ListCtrl;
		CComboBox m_ComboCtrl;

		virtual BOOL OnInitDialog();
	};


	// file : SomeDialogClass.cpp	

	BOOL CSomeDialogClass::OnInitDialog()
	{
		CDialog::OnInitDialog();

		////////////////////////////////////////////////////////
		// set up tabs.
		
		PSTR pszTabItems[] =
		{
			"Tab Sheet 1 : Tree control",
			"Tab Sheet 2 : List control",
			"Tab Sheet 3 : Combobox control",
			NULL
		};

		TC_ITEM tcItem;

		for(INT i = 0;
			pszTabItems[i] != NULL;
			i++)
		{
			tcItem.mask = TCIF_TEXT;
			tcItem.pszText = pszTabItems[i];
			tcItem.cchTextMax = strlen(pszTabItems[i]);
			m_TabCtrl.InsertItem(i,&tcItem);
		}

		// attach controls to tabs pages.

		m_TabCtrl.AttachControlToTab(&m_TreeCtrl,0);	// attach tree control to first page
		m_TabCtrl.AttachControlToTab(&m_ListCtrl,1);	// attach list control to second page
		m_TabCtrl.AttachControlToTab(&m_ComboCtrl,2);	// attach combo box control to third page

		// initialize tab to first page.
		m_TabCtrl.SetCurSel(0);
		////////////////////////////////////////////////////////
	}


 */

class CTabDlgItem
{
private:
	CTabDlgItem *m_pNext;
	CTabDlgItem *m_pPrevious;

	CWnd *m_pWnd;
	CWnd *m_pParent;
	int m_iResourceID;
	int m_iTabNumber;
	BOOL m_bAlwaysHide;

public:	
	CTabDlgItem (CWnd *pParent, int iResourceID, int iTabNumber);
	CTabDlgItem (CWnd *pWnd, int iTabNumber);
	~CTabDlgItem();

	void SetNext (CTabDlgItem *pNext);
	CTabDlgItem *GetNext();
	void SetPrevious (CTabDlgItem *pPrevious);
	CTabDlgItem *GetPrevious();

	CWnd *GetWnd()
	{
		return m_pWnd;
	}
	CWnd *GetParent()
	{
		return m_pParent;
	}
	int GetResourceID()
	{
		return m_iResourceID;
	}
	int GetTabNumber()
	{
		return m_iTabNumber;
	}
	void SetAlwaysHide (BOOL bAlwaysHide = TRUE)
	{
		m_bAlwaysHide = bAlwaysHide;
	}
	BOOL AlwaysHide()
	{
		return m_bAlwaysHide;
	}
};

class CTabDlgList
{
private:
	CTabDlgItem *m_pFirst;
	CTabDlgItem *m_pWrite;

public:
	CTabDlgList();
	~CTabDlgList();

	void PurgeList();
	BOOL AddItem(CWnd *pParent, int iResourceID, int iTabNumber);
	BOOL AddItem(CWnd *pWnd, int iTabNumber);
	void NeverShowControl (int iID);
	void HideShowItems (int iTabNumber);

	void SetFirst (CTabDlgItem *pFirst);
	CTabDlgItem *GetFirst();
	void SetWrite (CTabDlgItem *pWrite);
	CTabDlgItem *GetWrite();
};

class CSTabCtrl : public CTabCtrl
{
// Construction
public:
	CSTabCtrl();

// Members
private:
	CTabDlgList m_TabItemList;

// Operations
public:	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSTabCtrl();
	virtual BOOL AttachControlToTab(CWnd *pParent, INT iID, INT iTabNum);
	virtual BOOL AttachControlToTab(CWnd *pWnd, INT iTabNum);
	virtual int SetCurSel( int nItem );
	void NeverShowControl (int iID);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSTabCtrl)
	afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	CMap <INT, INT&,CList <CWnd *, CWnd *> *, CList<CWnd *, CWnd *> *> m_PageToCtrlListMap;
};

/////////////////////////////////////////////////////////////////////////////

#endif		// #ifndef _STABCTRL_H_