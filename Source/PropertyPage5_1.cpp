// PropertyPage5_1.cpp : implementation file
//

#include "stdafx.h"
#include "wally.h"
#include "PropertyPage5_1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPage5_1

IMPLEMENT_DYNAMIC(CPropertyPage5_1, CPropertySheet)

CPropertyPage5_1::CPropertyPage5_1(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CPropertyPage5_1::CPropertyPage5_1(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CPropertyPage5_1::~CPropertyPage5_1()
{
}


BEGIN_MESSAGE_MAP(CPropertyPage5_1, CPropertySheet)
	//{{AFX_MSG_MAP(CPropertyPage5_1)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPage5_1 message handlers
