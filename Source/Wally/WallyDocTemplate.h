#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WallyDoc.h"

class CWallyDocTemplate : public CMultiDocTemplate
{
private:
    int m_iGameType;

// Methods
public:
	CWallyDocTemplate( 

      UINT nIDResource, CRuntimeClass* pDocClass,
      CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ) :
      CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass), m_iGameType (-1)
      { };
      CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible, int iGameType);
	  CDocument* OpenDocumentFile(LPCTSTR lpszPathName,	BOOL bMakeVisible);
 	  void SetDefaultTitle(CDocument* pDocument);
	
};
