// ToolSettingsPage.cpp: implementation of the CToolSettingsPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "ToolSettingsPage.h"
#include "STabCtrl.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolSettingsPage::CToolSettingsPage()
{
	SetNext (NULL);
	SetPrevious (NULL);	

	Initialize (NULL, NULL, "", 0, -1);	
}

CToolSettingsPage::CToolSettingsPage(CWnd *pParent, CSTabCtrl *pTabCtrl, LPCTSTR szTitle, int iResourceID, int iTabNumber)
{
	SetNext (NULL);
	SetPrevious (NULL);
	
	Initialize(pParent, pTabCtrl, szTitle, iResourceID, iTabNumber);
}

CToolSettingsPage::~CToolSettingsPage()
{
	SetNext (NULL);
	SetPrevious (NULL);
}

void CToolSettingsPage::Initialize(CWnd *pParent, CSTabCtrl *pTabCtrl, LPCTSTR szTitle, int iResourceID, int iTabNumber)
{
	m_pParent = pParent;
	m_pTabCtrl = pTabCtrl;
	m_strTitle = szTitle;
	m_iResourceID = iResourceID;
	m_iTabNumber = iTabNumber;
}

void CToolSettingsPage::SetNext (CToolSettingsPage *pNext)
{
	m_pNext = pNext;
}

CToolSettingsPage *CToolSettingsPage::GetNext()
{
	return m_pNext;
}

void CToolSettingsPage::SetPrevious (CToolSettingsPage *pPrevious)
{
	m_pPrevious = pPrevious;
}

CToolSettingsPage *CToolSettingsPage::GetPrevious()
{
	return m_pPrevious;
}

BOOL CToolSettingsPage::AddToTabCtrl()
{
	TC_ITEM tcItem;
	CString strTitle (GetTitle());

	int iLength = strTitle.GetLength();
	LPTSTR szTitle = strTitle.GetBuffer(iLength);

	tcItem.mask = TCIF_TEXT;
	tcItem.pszText = szTitle;
	tcItem.cchTextMax = iLength;
	GetTabCtrl()->InsertItem(GetTabNumber(), &tcItem);
	
	return BindControls();
}

BOOL CToolSettingsPage::BindControls()
{
	// Dialog base unit -> pixel unit conversion macros
	#define DLGUNIT_TO_PIXEL_X(dlgX)	( (dlgX * LOWORD( ::GetDialogBaseUnits() )) / 4)
	#define DLGUNIT_TO_PIXEL_Y(dlgY)	( (dlgY * HIWORD( ::GetDialogBaseUnits() )) / 8)
	#define DWORD_ALIGN(x)  x = ((x + 3) & ~3)

	// find resource handle
	LPVOID lpResource = NULL;
	HGLOBAL hResource = NULL;
	int iResourceID = GetResourceID();

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hDlgInit = ::FindResource(hInst, MAKEINTRESOURCE(iResourceID), RT_DIALOG);

	if (!hDlgInit)
	{
		return FALSE;
	}
	
	// load it
	hResource = LoadResource(hInst, hDlgInit);
	if (hResource == NULL)
	{
		return FALSE;
	}

	// lock it
	lpResource = LockResource(hResource);
	ASSERT(lpResource != NULL);
	
	if (lpResource != NULL)
	{
		DWORD dwDataSize = SizeofResource (hInst, hDlgInit);

		LPDLGTEMPLATE lpTemplate;
		LPDLGITEMTEMPLATE lpItemTemplate;
		
		int iDlgTemplateSize = sizeof (DLGTEMPLATE);		
		int iDlgItemTemplateSize = sizeof (DLGITEMTEMPLATE);		

		BYTE *pbyGlobalData = (BYTE *)(lpResource);
		
		lpTemplate = (LPDLGTEMPLATE)lpResource;
		int iDataPosition = iDlgTemplateSize;
		int iNumControls = lpTemplate->cdit;
		
		int iStrLen = 0;
		WORD *wPointer = NULL;

		// Look for the menu item
		wPointer = (WORD *)(pbyGlobalData + iDataPosition);

		if ((*wPointer) == 0x0000)
		{
			// No other elements
			wPointer++;
		}
		else
		{
			// One other element which specifies the ordinal value of the menu
			wPointer += 2;
		}

		// Look for the window class		
		if ((*wPointer) == 0x0000)
		{
			// No other elements
			wPointer++;
		}
		else
		{
			// One other element which specifies the ordinal value of a predefined system window class
			wPointer += 2;
		}

		// Look for the unicode title string 
		if ((*wPointer) == 0x0000)
		{
			// No other elements
			wPointer++;
		}
		else
		{
			// Adjust for the string			
			while ((*wPointer++) != 0x0000);
		}

		// Check for the font point size and typeface members
		if (lpTemplate->style & DS_SETFONT)
		{
			wPointer++;
			
			// Adjust for the typeface string
			while ((*wPointer++) != 0x0000);
		}

		iDataPosition = PadDWORD (((BYTE *)wPointer) - pbyGlobalData);

		// The resource is not in pixel values; need to map the values properly
		int iWidth = DLGUNIT_TO_PIXEL_X(lpTemplate->cx);
		int iHeight = DLGUNIT_TO_PIXEL_Y(lpTemplate->cy);

		DWORD dwItemSize = 0;
		
		// Spin through all of the controls
		for (int j = 0; j < iNumControls; j++)
		{
			// First point to the item's template
			lpItemTemplate = (DLGITEMTEMPLATE *)(pbyGlobalData + iDataPosition);

			// Fields for DLGITEMTEMPLATE:
			// DWORD style
			// DWORD dwExtendedStyle
			// short x
			// short y
			// short cx
			// short cy
			// WORD id

			wPointer = (WORD *)(pbyGlobalData + iDataPosition + iDlgItemTemplateSize);
			
			// Check out the window class of this control			
			if ((*wPointer) == 0xffff)
			{
				// One other element... it's a predefined system class (button, edit box, static, 
				// list box, scroll bar, or combo box.)
				wPointer += 2;				
			}
			else
			{
				// This is the name of a registered windows class
				while ((*wPointer++) != 0x0000);
			}
			
			// Check out the title array
			if (*wPointer == 0xffff)
			{
				// There's one other element which represents the ordinal value of a resource, 
				// such as an icon				
				wPointer += 2;
			}
			else
			{
				// This is the initial text for the resource				
				while ((*wPointer++) != 0x0000);
			}

			WORD wCADataSize = 0;

			// Check out the creation array
			if (*wPointer == 0x0000)
			{
				// There is no creation array
				wCADataSize = sizeof(WORD);
			}
			else
			{
				// The first item represents the size of the array (which includes
				// the first item, too!)
				wCADataSize = (*wPointer);
			}

			GetTabCtrl()->AttachControlToTab( GetParent(), lpItemTemplate->id, GetTabNumber());

			// Figure out how big the item is; it needs to be DWORD-aligned			
			dwItemSize = (((BYTE *)wPointer) - ((BYTE *)lpItemTemplate)) + wCADataSize;
			dwItemSize = PadDWORD (dwItemSize);

			iDataPosition += dwItemSize;
		}		
	}	
	
	// cleanup
	if (lpResource != NULL && hResource != NULL)
	{
		UnlockResource(hResource);
		FreeResource(hResource);
	}
	return TRUE;
}