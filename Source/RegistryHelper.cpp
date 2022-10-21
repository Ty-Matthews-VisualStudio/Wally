// RegistryHelper.cpp: implementation of the CRegistryHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegistryHelper.h"
//#include "MiscFunctions.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistryItem::CRegistryItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	InitMembers();
	m_pInteger = pInteger;
	m_iDefaultValue = iDefaultValue;
	m_iMinValue = iMinValue;
	m_iMaxValue = iMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_INTEGER);
}

CRegistryItem::CRegistryItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	InitMembers();
	m_pDouble = pDouble;
	m_dfDefaultValue = dfDefaultValue;
	m_dfMinValue = dfMinValue;
	m_dfMaxValue = dfMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_DOUBLE);
}

CRegistryItem::CRegistryItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	InitMembers();
	m_lpDWord = lpDWord;
	m_dwDefaultValue = dwDefaultValue;
	m_dwMinValue = dwMinValue;
	m_dwMaxValue = dwMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_DWORD);
}

CRegistryItem::CRegistryItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	InitMembers();
	m_pCString = pCString;
	m_strDefaultValue = szDefaultValue;
	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_CSTRING);
}

CRegistryItem::CRegistryItem (LPTSTR pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	InitMembers();
	m_pString = pString;
	m_strDefaultValue = szDefaultValue;
	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_STRING);
}

CRegistryItem::~CRegistryItem()
{
	SetNext (NULL);
	SetPrevious (NULL);
}

void CRegistryItem::InitMembers()
{
	m_pInteger = NULL;
	m_pCString = NULL;
	m_pString = NULL;
	m_pDouble = NULL;
	m_lpDWord = NULL;

	SetItemType (REGISTRY_TYPE_UNKNOWN);
	SetSubKey ("");
	SetNext (NULL);
	SetPrevious (NULL);
}

void CRegistryItem::GetDefaultValue (CString *pstrDefaultValue)
{
	ASSERT (pstrDefaultValue);

	(*pstrDefaultValue) = m_strDefaultValue;
}

void CRegistryItem::GetDefaultValue (int *piDefaultValue)
{
	ASSERT (piDefaultValue);

	(*piDefaultValue) = m_iDefaultValue;
}

void CRegistryItem::GetDefaultValue (double *pdfDefaultValue)
{
	ASSERT (pdfDefaultValue);

	(*pdfDefaultValue) = m_dfDefaultValue;
}

void CRegistryItem::GetDefaultValue (LPDWORD pdwDefaultValue)
{
	ASSERT (pdwDefaultValue);

	(*pdwDefaultValue) = m_dwDefaultValue;
}

void CRegistryItem::GetItemValue (CString *pstrItemValue)
{
	switch (GetItemType())
	{
	case REGISTRY_TYPE_CSTRING:
		(*pstrItemValue) = (*m_pCString);
		break;

	case REGISTRY_TYPE_STRING:
		(*pstrItemValue) = m_pString;
		break;

	default:
		ASSERT (FALSE);
		break;
	}
}

void CRegistryItem::GetItemValue (int *piItemValue)
{
	ASSERT (piItemValue);
	(*piItemValue) = (*m_pInteger);
}

void CRegistryItem::GetItemValue (double *pdfItemValue)
{
	ASSERT (pdfItemValue);
	(*pdfItemValue) = (*m_pDouble);
}

void CRegistryItem::GetItemValue (LPDWORD pdwItemValue)
{
	ASSERT (pdwItemValue);
	(*pdwItemValue) = (*m_lpDWord);
}

void CRegistryItem::AdjustMinMax()
{
	switch (GetItemType())
	{
	case REGISTRY_TYPE_INTEGER:
		if (m_iMaxValue != m_iMinValue)
		{
			(*m_pInteger) = min ((*m_pInteger), m_iMaxValue);
			(*m_pInteger) = max ((*m_pInteger), m_iMinValue);
		}
		break;

	case REGISTRY_TYPE_DOUBLE:
		if (m_dfMaxValue != m_dfMinValue)
		{
			(*m_pDouble) = min ((*m_pDouble), m_dfMaxValue);
			(*m_pDouble) = max ((*m_pDouble), m_dfMinValue);
		}
		break;

	case REGISTRY_TYPE_DWORD:
		if (m_dwMaxValue != m_dwMinValue)
		{
			(*m_lpDWord) = min ((*m_lpDWord), m_dwMinValue);
			(*m_lpDWord) = max ((*m_lpDWord), m_dwMaxValue);
		}
		break;

	default:
		ASSERT (FALSE);
		break;
	}
}

CString CRegistryItem::GetSubKey()
{
	return m_strSubKey;
}

void CRegistryItem::SetValueName(LPCTSTR szValueName)
{
	m_strValueName = szValueName;
}

CString CRegistryItem::GetValueName()
{
	return m_strValueName;
}

void CRegistryItem::SetSubKey(LPCTSTR szSubKey)
{
	m_strSubKey = TrimSlashes (szSubKey);	
}

CString CRegistryItem::TrimSlashes (LPCTSTR szSource)
{
	CString strSource(szSource);

	int iPosition = strSource.GetLength() - 1;
	BOOL bDone = FALSE;

	while ((iPosition >= 0) && (!bDone))
	{
		if ((strSource.GetAt(iPosition) == '\\') || (strSource.GetAt(iPosition) == '/'))
		{
			iPosition--;
		}
		else
		{
			bDone = TRUE;
		}
	}
	iPosition++;

	if (iPosition > 0)
	{
		strSource = strSource.Left (iPosition);
	}
	else
	{
		strSource = "";
	}

	return strSource;
}

int CRegistryItem::GetItemType()
{
	return m_iItemType;
}

void CRegistryItem::SetItemType(int iType)
{
	m_iItemType = iType;
}

void CRegistryItem::SetItemValue (LPCTSTR szValue)
{
	switch (GetItemType())
	{
	case REGISTRY_TYPE_CSTRING:		
		(*m_pCString) = szValue;
		if ((*m_pCString) == "")
		{
			(*m_pCString) = m_strDefaultValue;
		}
		break;

	case REGISTRY_TYPE_STRING:
		strcpy (m_pString, szValue);
		break;

	default:
		ASSERT (FALSE);
		break;
	}
}

void CRegistryItem::SetItemValue (int iValue)
{
	(*m_pInteger) = iValue;
	AdjustMinMax();
}

void CRegistryItem::SetItemValue (double dfValue)
{
	(*m_pDouble) = dfValue;
	AdjustMinMax();
}

void CRegistryItem::SetItemValue (DWORD dwValue)
{
	(*m_lpDWord) = dwValue;
	AdjustMinMax();
}

void CRegistryItem::SetNext (CRegistryItem *pNext)
{
	m_pNext = pNext;
}

CRegistryItem *CRegistryItem::GetNext()
{
	return m_pNext;
}

void CRegistryItem::SetPrevious (CRegistryItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CRegistryItem *CRegistryItem::GetPrevious()
{
	return m_pPrevious;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistryHelper::CRegistryHelper()
{
	SetMainKey (NULL);
	SetBaseSubKey ("");
	SetAll (NULL);
}

CRegistryHelper::~CRegistryHelper()
{
	PurgeList();
	SetAll (NULL);
}

void CRegistryHelper::SetMainKey(HKEY hMainKey)
{	
	m_hMainKey = hMainKey;
}

void CRegistryHelper::SetBaseSubKey (LPCTSTR szBaseSubKey)
{
	m_strBaseSubKey = TrimSlashes (szBaseSubKey);	
}

CString CRegistryHelper::TrimSlashes (LPCTSTR szSource)
{
	CString strSource(szSource);

	int iPosition = strSource.GetLength() - 1;
	BOOL bDone = FALSE;

	while ((iPosition >= 0) && (!bDone))
	{
		if ((strSource.GetAt(iPosition) == '\\') || (strSource.GetAt(iPosition) == '/'))
		{
			iPosition--;
		}
		else
		{
			bDone = TRUE;
		}
	}
	iPosition++;

	if (iPosition > 0)
	{
		strSource = strSource.Left (iPosition);
	}
	else
	{
		strSource = "";
	}

	return strSource;
}

void CRegistryHelper::SetFirst (CRegistryItem *pFirst)
{
	m_pFirst = pFirst;
}

CRegistryItem *CRegistryHelper::GetFirst()
{
	return m_pFirst;
}

void CRegistryHelper::SetWrite (CRegistryItem *pWrite)
{
	m_pWrite = pWrite;
}

CRegistryItem *CRegistryHelper::GetWrite()
{
	return m_pWrite;
}

void CRegistryHelper::PurgeList()
{
	CRegistryItem *pTemp = GetFirst();

	while (pTemp)
	{
		SetFirst (GetFirst()->GetNext());
		delete pTemp;
		pTemp = GetFirst();
	}

	SetAll (NULL);
}

void CRegistryHelper::AddItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (szValueName[0] != '\0'));	// Neal - we're getting blank keys!

	CRegistryItem *pNewItem = new CRegistryItem(pInteger, iDefaultValue, iMinValue, iMaxValue, szValueName, szSubKey);	
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (szValueName[0] != '\0'));	// Neal - we're getting blank keys!

	CRegistryItem *pNewItem = new CRegistryItem(pDouble, dfDefaultValue, dfMinValue, dfMaxValue, szValueName, szSubKey);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (szValueName[0] != '\0'));	// Neal - we're getting blank keys!

	CRegistryItem *pNewItem = new CRegistryItem(lpDWord, dwDefaultValue, dwMinValue, dwMaxValue, szValueName, szSubKey);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}
	
void CRegistryHelper::AddItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (szValueName[0] != '\0'));	// Neal - we're getting blank keys!

	CRegistryItem *pNewItem = new CRegistryItem(pCString, szDefaultValue, szValueName, szSubKey);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (LPTSTR pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey)
{
	ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (szValueName[0] != '\0'));	// Neal - we're getting blank keys!

	CRegistryItem *pNewItem = new CRegistryItem(pString, szDefaultValue, szValueName, szSubKey);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItemToList (CRegistryItem *pItem)
{
	ASSERT (pItem);

	if (!GetFirst())
	{
		SetAll (pItem);
	}
	else
	{
		GetWrite()->SetNext (pItem);
		pItem->SetPrevious(GetWrite());
		SetWrite (pItem);
	}
}

void CRegistryHelper::SetAll (CRegistryItem *pItem)
{
	SetFirst (pItem);
	SetWrite (pItem);
}

void CRegistryHelper::ReadRegistry()
{
	ASSERT (m_hMainKey);				// Must call SetMainKey first
	
	// First, always create the base key (no harm done)
	HKEY hKey;
	long lResult;	
	DWORD dwDisp;
	DWORD dwActualSize;
	
	if (m_strBaseSubKey != "")
	{
		lResult = RegCreateKeyEx (m_hMainKey, m_strBaseSubKey, 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
	}
		
	
	CRegistryItem *pItem = GetFirst();
	CString strItemSubKey("");
	CString strFullKey("");
	CString strValueName("");
	LPTSTR szValueName = NULL;

	while (pItem)
	{
		strItemSubKey = pItem->GetSubKey();
		strFullKey =  m_strBaseSubKey + (m_strBaseSubKey != "" ? "\\" : "");
		strFullKey += strItemSubKey;
		strValueName = pItem->GetValueName();

		szValueName = strValueName.GetBuffer (strValueName.GetLength());

		// Always Create the subkey under our main base key, and grab the handle for it
		lResult = RegCreateKeyEx (m_hMainKey, strFullKey, 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);

		if (lResult == NOERROR)
		{
			switch (pItem->GetItemType())
			{
			case REGISTRY_TYPE_INTEGER:
				{					
					int iValue = 0;
					
					pItem->GetDefaultValue (&iValue);

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Go read it in
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (&iValue), &dwActualSize);
					}
					
					pItem->SetItemValue (iValue);
				}
				break;

			case REGISTRY_TYPE_DOUBLE:
				{
					// MFC stores doubles in the registry as strings
					CString strValue("");
					double dfValue = 0.0;
					char *szTempBuffer = NULL;

					pItem->GetDefaultValue (&dfValue);

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Allocate space for it
						szTempBuffer = new char[dwActualSize];
						
						// Go read it in
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (szTempBuffer), &dwActualSize);

						dfValue = atof(szTempBuffer);						
					}

					if (szTempBuffer)
					{
						delete [] szTempBuffer;
						szTempBuffer = NULL;
					}

					pItem->SetItemValue (dfValue);					
				}
				break;

			case REGISTRY_TYPE_DWORD:
				{
					DWORD dwValue = 0;

					pItem->GetDefaultValue (&dwValue);
					DWORD dwType = REG_DWORD;

					// Query the key to figure out how big it is
					lResult = ::RegQueryValueEx(hKey, szValueName, NULL, &dwType, NULL, &dwActualSize);

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Go read it in
						lResult = ::RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE) (&dwValue), &dwActualSize);
					}

					pItem->SetItemValue (dwValue);
				}
				break;

			case REGISTRY_TYPE_CSTRING:
			case REGISTRY_TYPE_STRING:
				{
					CString strValue("");
					char *szTempBuffer = NULL;
					
					pItem->GetDefaultValue (&strValue);

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Allocate space for it
						szTempBuffer = new char[dwActualSize];
						
						// Go read it in
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (szTempBuffer), &dwActualSize);

						pItem->SetItemValue (szTempBuffer);
					}
					else
					{
						pItem->SetItemValue (strValue);
					}					

					if (szTempBuffer)
					{
						delete [] szTempBuffer;
						szTempBuffer = NULL;
					}
				}
				break;			
			
			default:
				ASSERT (FALSE);
				break;
			}				
			
			RegCloseKey (hKey);
		}

		pItem = pItem->GetNext();
	}	
}

void CRegistryHelper::WriteRegistry()
{
	ASSERT (m_hMainKey);				// Must call SetMainKey first

	// First, always create the base key (no harm done)
	HKEY hKey;
	long lResult;	
	DWORD dwDisp;

	if (m_strBaseSubKey != "")
	{
		lResult = RegCreateKeyEx (m_hMainKey, m_strBaseSubKey, 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
	}

	CRegistryItem *pItem = GetFirst();
	CString strItemSubKey("");
	CString strFullKey("");
	CString strValueName("");
	LPTSTR szValueName = NULL;
	
	while (pItem)
	{
		strItemSubKey = pItem->GetSubKey();
		strFullKey =  m_strBaseSubKey + (m_strBaseSubKey != "" ? "\\" : "");
		strFullKey += strItemSubKey;
		strValueName = pItem->GetValueName();

		ASSERT( (m_hMainKey == HKEY_CLASSES_ROOT) || (strValueName.GetLength() > 0));	// Neal - we're getting blank keys!

		szValueName = strValueName.GetBuffer (strValueName.GetLength());

		// Always Create the subkey under our main base key, and grab the handle for it
		lResult = RegCreateKeyEx (m_hMainKey, strFullKey, 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
		
		if (lResult == NOERROR)
		{
			switch (pItem->GetItemType())
			{
			case REGISTRY_TYPE_INTEGER:
				{						
					int iItemValue = 0;					
					pItem->GetItemValue (&iItemValue);

					// Set the key
					lResult = RegSetValueEx (hKey, szValueName, 0, REG_DWORD, (LPBYTE)(&iItemValue), sizeof (iItemValue));
				}
				break;

			case REGISTRY_TYPE_DOUBLE:
				{
					// MFC stores doubles in the registry as strings, so we'll be nice
					// and play along
					double dfItemValue = 0.0;
					pItem->GetItemValue (&dfItemValue);

					CString strItemValue("");
					strItemValue.Format ("%.13f", dfItemValue);

					char *szTempBuffer = NULL;
					int iStringLength = 0;
									
					iStringLength = strItemValue.GetLength();
					szTempBuffer = strItemValue.GetBuffer(iStringLength);
					
					// Set the key
					lResult = RegSetValueEx (hKey, szValueName, 0, REG_SZ, (LPBYTE)(szTempBuffer), iStringLength + 1);

					strItemValue.ReleaseBuffer();
				}
				break;

			case REGISTRY_TYPE_DWORD:
				{
					DWORD dwItemValue = 0;
					pItem->GetItemValue (&dwItemValue);

					// Set the key
					lResult = RegSetValueEx (hKey, szValueName, 0, REG_DWORD, (LPBYTE)(&dwItemValue), sizeof (dwItemValue));
				}
				break;

			case REGISTRY_TYPE_CSTRING:
			case REGISTRY_TYPE_STRING:
				{
					CString strItemValue("");
					char *szTempBuffer = NULL;
					int iStringLength = 0;

					pItem->GetItemValue (&strItemValue);
									
					iStringLength = strItemValue.GetLength();
					szTempBuffer = strItemValue.GetBuffer(iStringLength);

					// Set the key
					lResult = RegSetValueEx (hKey, szValueName, 0, REG_SZ, (LPBYTE)(szTempBuffer), iStringLength + 1);

					strItemValue.ReleaseBuffer();
				}
				break;			
			
			default:
				ASSERT (FALSE);
				break;
			}				
			
			RegCloseKey (hKey);
		}

		pItem = pItem->GetNext();
	}	
}



