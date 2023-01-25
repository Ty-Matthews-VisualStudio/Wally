// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _STDAFX_H__
#define _STDAFX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#pragma warning(disable: 4786)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>
#include <afxpriv.h>
#include <io.h>
#include <shlobj.h>
#include <iostream>
#include <afxcoll.h>
#include <afx.h>
#include <direct.h>
#include <afxole.h>
#include <math.h>
#include <time.h>
#include <process.h>
#include <boost/property_tree/json_parser.hpp>

#include <list>
#include <map>
#include <vector>
using namespace std;

#include "RegistryHelper.h"
#include "Globals.h"
#include "WallyException.h"
#include "DelayRender.h"
#include "ProgressBar.h"
#include "MemBuffer.h"

#define bool  BOOL
#define true  TRUE
#define false FALSE

#pragma comment( lib, "winmm.lib" )

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _STDAFX_H__
