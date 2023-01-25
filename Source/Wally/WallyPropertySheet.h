// WallyPropertySheet.h : header file
//
// This class defines custom modal property sheet 
// CWallyPropertySheet.
 
#ifndef __WALLYPROPERTYSHEET_H__
#define __WALLYPROPERTYSHEET_H__

#include "WallyPropertyPage1.h"

/////////////////////////////////////////////////////////////////////////////
// CWallyPropertySheet

class CWallyPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWallyPropertySheet)

// Construction
public:
	CWallyPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CWallyPropertyPage1 m_Page1;
	CWallyPropertyPage2 m_Page2;
	CWallyPropertyPage3 m_Page3;
	CWallyPropertyPage4 m_Page4;
	CWallyPropertyPage5 m_Page5;
	CWallyPropertyPage6 m_Page6;
	// 2023-01-24: Removed the property page for file associations, as this functionality no longer works with later versions of Windows
	//CWallyPropertyPage7 m_Page7;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWallyPropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CWallyPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __WALLYPROPERTYSHEET_H__
