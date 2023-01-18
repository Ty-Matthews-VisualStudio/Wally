//////////////////////////////////////////////////////////////////////
// ToolSettingsPage.h: interface for the CToolSettingsPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TOOLSETTINGSPAGE_H_
#define _TOOLSETTINGSPAGE_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSTabCtrl;

class CToolSettingsPage  
{
private:
	CToolSettingsPage *m_pNext;
	CToolSettingsPage *m_pPrevious;

	CWnd *m_pParent;
	CSTabCtrl *m_pTabCtrl;
	CString m_strTitle;
	int m_iResourceID;
	int m_iTabNumber;	

public:
	CToolSettingsPage();
	CToolSettingsPage(CWnd *pParent, CSTabCtrl *pTabCtrl, LPCTSTR szTitle, int iResourceID, int iTabNumber);
	virtual ~CToolSettingsPage();

	void Initialize(CWnd *pParent, CSTabCtrl *pTabCtrl, LPCTSTR szTitle, int iResourceID, int iTabNumber);
	
	CWnd *GetParent()
	{
		return m_pParent;
	}
	CSTabCtrl *GetTabCtrl()
	{
		return m_pTabCtrl;
	}
	LPCTSTR GetTitle()
	{
		return m_strTitle;
	}
	int GetResourceID()
	{
		return m_iResourceID;
	}
	int GetTabNumber()
	{
		return m_iTabNumber;
	}

	void SetNext (CToolSettingsPage *pNext);
	CToolSettingsPage *GetNext();
	void SetPrevious (CToolSettingsPage *pPrevious);
	CToolSettingsPage *GetPrevious();

	BOOL AddToTabCtrl();
	BOOL BindControls();
};

#endif // #ifndef _TOOLSETTINGSPAGE_H_
