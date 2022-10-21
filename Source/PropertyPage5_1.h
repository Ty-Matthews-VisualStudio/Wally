#if !defined(AFX_PROPERTYPAGE5_1_H__C3C5B600_C832_11D2_A7F3_0000C0D88D2D__INCLUDED_)
#define AFX_PROPERTYPAGE5_1_H__C3C5B600_C832_11D2_A7F3_0000C0D88D2D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PropertyPage5_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPage5_1

class CPropertyPage5_1 : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertyPage5_1)

// Construction
public:
	CPropertyPage5_1(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropertyPage5_1(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPage5_1)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyPage5_1();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyPage5_1)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGE5_1_H__C3C5B600_C832_11D2_A7F3_0000C0D88D2D__INCLUDED_)
