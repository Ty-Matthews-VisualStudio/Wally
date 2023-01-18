#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WallyDoc.h"

class CWallyDocTemplate : public CMultiDocTemplate
{
// Methods
public:
	CWallyDocTemplate( 

      UINT nIDResource, CRuntimeClass* pDocClass,
      CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ) :
      CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
      { };
	  CDocument* OpenDocumentFile(LPCTSTR lpszPathName,	BOOL bMakeVisible);
 	  void SetDefaultTitle(CDocument* pDocument);
	
};
