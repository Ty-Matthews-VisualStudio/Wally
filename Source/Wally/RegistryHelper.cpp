// RegistryHelper.cpp: implementation of the CRegistryHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegistryHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistryItem::CRegistryItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	InitMembers();
	m_pInteger = pInteger;
	m_iDefaultValue = iDefaultValue;
	m_iMinValue = iMinValue;
	m_iMaxValue = iMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_INTEGER);
	SetFlags( dwFlags);
}

CRegistryItem::CRegistryItem (UINT *pUInteger, UINT iDefaultValue, UINT iMinValue, UINT iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	InitMembers();
	m_pUInteger = pUInteger;
	m_uiDefaultValue = iDefaultValue;
	m_uiMinValue = iMinValue;
	m_uiMaxValue = iMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_UNSIGNED_INTEGER);
	SetFlags( dwFlags);
}

CRegistryItem::CRegistryItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	InitMembers();
	m_pDouble = pDouble;
	m_dfDefaultValue = dfDefaultValue;
	m_dfMinValue = dfMinValue;
	m_dfMaxValue = dfMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_DOUBLE);
	SetFlags( dwFlags);
}

CRegistryItem::CRegistryItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	InitMembers();
	m_lpDWord = lpDWord;
	m_dwDefaultValue = dwDefaultValue;
	m_dwMinValue = dwMinValue;
	m_dwMaxValue = dwMaxValue;

	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_DWORD);
	SetFlags( dwFlags);
}

#ifdef _UNICODE
CRegistryItem::CRegistryItem(std::wstring *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
#else
CRegistryItem::CRegistryItem (std::string *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
#endif
{
	InitMembers();
	m_pString = pString;
	m_strDefaultValue = szDefaultValue;
	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_STRING);
	m_bEncrypt = bEncrypt;
	SetFlags( dwFlags);
}

#ifdef __AFX_H__
CRegistryItem::CRegistryItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
{
	InitMembers();
	m_pCString = pCString;
	m_cstrDefaultValue = szDefaultValue;
	SetSubKey (szSubKey);
	SetValueName (szValueName);
	SetItemType (REGISTRY_TYPE_CSTRING);
	m_bEncrypt = bEncrypt;
	SetFlags( dwFlags);
}
#endif		// #ifdef __AFX_H__

CRegistryItem::~CRegistryItem()
{
	SetNext (NULL);
	SetPrevious (NULL);
}

void CRegistryItem::InitMembers()
{
	m_pInteger = NULL;
	m_pUInteger = NULL;
	m_pString = NULL;
	m_pDouble = NULL;
	m_lpDWord = NULL;

	SetItemType (REGISTRY_TYPE_UNKNOWN);
	SetSubKey (_T(""));
	SetNext (NULL);
	SetPrevious (NULL);
	m_bEncrypt = FALSE;
}

#ifdef _UNICODE
void CRegistryItem::GetDefaultValue(std::wstring *pstrDefaultValue)
#else
void CRegistryItem::GetDefaultValue (std::string *pstrDefaultValue)
#endif
{
	(*pstrDefaultValue) = m_strDefaultValue;
}

void CRegistryItem::GetDefaultValue (int *piDefaultValue)
{
	(*piDefaultValue) = m_iDefaultValue;
}

void CRegistryItem::GetDefaultValue (UINT *puiDefaultValue)
{
	(*puiDefaultValue) = m_uiDefaultValue;
}

void CRegistryItem::GetDefaultValue (double *pdfDefaultValue)
{
	(*pdfDefaultValue) = m_dfDefaultValue;
}

void CRegistryItem::GetDefaultValue (LPDWORD pdwDefaultValue)
{
	(*pdwDefaultValue) = m_dwDefaultValue;
}

#ifdef __AFX_H__
void CRegistryItem::GetDefaultValue (CString *pcstrDefaultValue)
{
	(*pcstrDefaultValue) = m_cstrDefaultValue;
}
#endif		// #ifdef __AFX_H__

#ifdef _UNICODE
void CRegistryItem::GetItemValue(std::wstring *pstrItemValue)
#else
void CRegistryItem::GetItemValue (std::string *pstrItemValue)
#endif
{
#ifdef _UNICODE
	std::wstring strValue;
#else
	std::string strValue;
#endif
	
	switch (GetItemType())
	{
	case REGISTRY_TYPE_STRING:
		{
			strValue = (*m_pString);			
			(*pstrItemValue) = strValue;
		}
		break;

	default:		
		break;
	}
}

#ifdef __AFX_H__
void CRegistryItem::GetItemValue (CString *pcstrItemValue)
{
#ifdef _UNICODE
	std::wstring strValue;
#else
	std::string strValue;
#endif
	
	strValue = (*m_pCString);
	
	(*pcstrItemValue) = strValue.c_str();
}
#endif		// #ifdef __AFX_H__

void CRegistryItem::GetItemValue (int *piItemValue)
{
	(*piItemValue) = (*m_pInteger);
}

void CRegistryItem::GetItemValue (UINT *puiItemValue)
{
	(*puiItemValue) = (*m_pUInteger);
}

void CRegistryItem::GetItemValue (double *pdfItemValue)
{
	(*pdfItemValue) = (*m_pDouble);
}

void CRegistryItem::GetItemValue (LPDWORD pdwItemValue)
{
	(*pdwItemValue) = (*m_lpDWord);
}

void CRegistryItem::SetFlags( DWORD dwFlags /* = 0 */)
{
	m_dwFlags = dwFlags;
}

DWORD CRegistryItem::GetFlags()
{
	return m_dwFlags;
}

void CRegistryItem::AdjustMinMax()
{
	DWORD dwFlags = GetFlags();
	
	switch (GetItemType())
	{
	case REGISTRY_TYPE_INTEGER:
		if (dwFlags & REGISTRY_MAX)
		{
			(*m_pInteger) = min ((*m_pInteger), m_iMaxValue);
		}

		if (dwFlags & REGISTRY_MIN)
		{
			(*m_pInteger) = max ((*m_pInteger), m_iMinValue);
		}
		break;

	case REGISTRY_TYPE_UNSIGNED_INTEGER:
		if (dwFlags & REGISTRY_MAX)
		{
			(*m_pUInteger) = min ((*m_pUInteger), m_uiMaxValue);
		}

		if (dwFlags & REGISTRY_MIN)
		{
			(*m_pUInteger) = max ((*m_pUInteger), m_uiMinValue);
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
			(*m_lpDWord) = min ((*m_lpDWord), m_dwMaxValue);
			(*m_lpDWord) = max ((*m_lpDWord), m_dwMinValue);
		}
		break;

	default:
		break;
	}
}

LPCTSTR CRegistryItem::GetSubKey()
{
	return m_strSubKey.c_str();
}

void CRegistryItem::SetValueName(LPCTSTR szValueName)
{
	m_strValueName = szValueName;
}

LPCTSTR CRegistryItem::GetValueName()
{
	return m_strValueName.c_str();
}

void CRegistryItem::SetSubKey(LPCTSTR szSubKey)
{
	m_strSubKey = szSubKey;
	TrimSlashes( m_strSubKey );
}

#ifdef _UNICODE
std::wstring CRegistryItem::TrimSlashes(std::wstring &strTrim)
{
	int iPos = 0;

	if ((iPos = strTrim.find_last_not_of('\\')) == std::wstring::npos)
	{
		strTrim.erase();
	}
	else
	{
		strTrim.erase(iPos + 1);
	}
	return(strTrim);
}
#else
std::string CRegistryItem::TrimSlashes( std::string &strTrim )
{
	int iPos = 0;

	if ((iPos = strTrim.find_last_not_of('\\')) == std::string::npos)
	{
		strTrim.erase();
	}
	else
	{
		strTrim.erase(iPos + 1);
	}
	return(strTrim);
}
#endif


int CRegistryItem::GetItemType()
{
	return m_iItemType;
}

void CRegistryItem::SetItemType(int iType)
{
	m_iItemType = iType;
}

void CRegistryItem::SetItemValue (LPCTSTR szValue, BOOL bEncrypted /* = FALSE */)
{
#ifdef _UNICODE
	std::wstring strValue;
#else
	std::string strValue;
#endif
	strValue = szValue;
	
	switch (GetItemType())
	{
	case REGISTRY_TYPE_STRING:
		(*m_pString) = strValue;
		if( (*m_pString).length() == 0 )
		{
			(*m_pString) = m_strDefaultValue;
		}
		break;

	case REGISTRY_TYPE_CSTRING:
#ifdef __AFX_H__
		*m_pCString = szValue;
		if( (*m_pCString).GetLength() == 0 )
		{
			(*m_pCString) = m_cstrDefaultValue;
		}
#endif
		break;

	default:
		break;
	}
}

void CRegistryItem::SetItemValue (int iValue)
{
	(*m_pInteger) = iValue;
	AdjustMinMax();
}

void CRegistryItem::SetItemValue (UINT iValue)
{
	(*m_pUInteger) = iValue;
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
	SetBaseSubKey (_T(""));
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
	m_strBaseSubKey = szBaseSubKey;
	TrimSlashes( m_strBaseSubKey );
}

#ifdef _UNICODE
std::wstring CRegistryHelper::TrimSlashes(std::wstring &strTrim)
{
	int iPos = 0;

	if ((iPos = strTrim.find_last_not_of('\\')) == std::wstring::npos)
	{
		strTrim.erase();
	}
	else
	{
		strTrim.erase(iPos + 1);
	}
	return(strTrim);
}
#else
std::string CRegistryHelper::TrimSlashes( std::string &strTrim )
{
	int iPos = 0;

	if ( (iPos = strTrim.find_last_not_of( '\\' ) ) == std::string::npos )
	{
		strTrim.erase();
	}
	else
	{
		strTrim.erase( iPos + 1 );
	}
	return( strTrim );
}
#endif

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

void CRegistryHelper::AddItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	CRegistryItem *pNewItem = new CRegistryItem(pInteger, iDefaultValue, iMinValue, iMaxValue, szValueName, szSubKey, dwFlags);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (UINT *pInteger, UINT iDefaultValue, UINT iMinValue, UINT iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	CRegistryItem *pNewItem = new CRegistryItem(pInteger, iDefaultValue, iMinValue, iMaxValue, szValueName, szSubKey, dwFlags);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	CRegistryItem *pNewItem = new CRegistryItem(pDouble, dfDefaultValue, dfMinValue, dfMaxValue, szValueName, szSubKey, dwFlags);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

void CRegistryHelper::AddItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags /* = 0 */)
{
	CRegistryItem *pNewItem = new CRegistryItem(lpDWord, dwDefaultValue, dwMinValue, dwMaxValue, szValueName, szSubKey, dwFlags);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}
	
#ifdef _UNICODE
void CRegistryHelper::AddItem(std::wstring *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
#else
void CRegistryHelper::AddItem (std::string *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
#endif
{
	CRegistryItem *pNewItem = new CRegistryItem(pString, szDefaultValue, szValueName, szSubKey, bEncrypt, dwFlags);
	AddItemToList(pNewItem);
	pNewItem = NULL;
}

#ifdef __AFX_H__
void CRegistryHelper::AddItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt /* = FALSE */, DWORD dwFlags /* = 0 */)
{
	CRegistryItem *pNewItem = new CRegistryItem( pCString, szDefaultValue, szValueName, szSubKey, bEncrypt, dwFlags );
	AddItemToList( pNewItem );
	pNewItem = NULL;
}
#endif		// #ifdef __AFX_H__

void CRegistryHelper::AddItemToList (CRegistryItem *pItem)
{	
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

BOOL CRegistryHelper::ReadRegistry(DWORD dwSAMDesired /* = 0 */)
{
	// First, always create the base key (no harm done)
	HKEY hKey;
	long lResult;	
	DWORD dwDisp;
	DWORD dwSize;
	DWORD dwActualSize;	
	CRegistryItem *pItem = GetFirst();

#ifdef _UNICODE
	std::wstring strItemSubKey;
	std::wstring strFullKey;
	std::wstring strValueName;
#else
	std::string strItemSubKey;
	std::string strFullKey;
	std::string strValueName;
#endif
	LPCTSTR szValueName = NULL;

	if( m_strBaseSubKey.length() != 0 )
	{
		lResult = RegCreateKeyEx( m_hMainKey, m_strBaseSubKey.c_str(), 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | dwSAMDesired, NULL, &hKey, &dwDisp );
		if( lResult != ERROR_SUCCESS )
		{
			return false;
		}
		RegCloseKey( hKey );
	}

	while( pItem )
	{
		strItemSubKey = pItem->GetSubKey();
		strFullKey = m_strBaseSubKey;
		strFullKey += (m_strBaseSubKey.length() != 0 ? _T("\\") : _T(""));
		strFullKey += strItemSubKey;
		strValueName = pItem->GetValueName();

		szValueName = strValueName.c_str();

		// Always Create the subkey under our main base key, and grab the handle for it
		lResult = RegCreateKeyEx (m_hMainKey, strFullKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | dwSAMDesired, NULL, &hKey, &dwDisp );

		if( lResult != ERROR_SUCCESS )
		{
			return false;
		}
		else
		{
			switch (pItem->GetItemType())
			{
			case REGISTRY_TYPE_INTEGER:
				{					
					int iValue = 0;
					
					pItem->GetDefaultValue (&iValue);

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);
					if( dwActualSize > 0 )
					{
						ASSERT( dwActualSize == sizeof(int) );
					}

					// If the value is there, read in the data; if not, set it
					if( lResult == ERROR_SUCCESS && dwActualSize == sizeof(int) )
					{
						// Go read it in
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (&iValue), &dwActualSize);
					}
					
					pItem->SetItemValue (iValue);
				}
				break;

			case REGISTRY_TYPE_UNSIGNED_INTEGER:
				{					
					UINT iValue = 0;
					pItem->GetDefaultValue (&iValue);

					DWORD dwType = REG_DWORD;

					// Query the key to figure out how big it is
					lResult = ::RegQueryValueEx(hKey, szValueName, NULL, &dwType, NULL, &dwActualSize);
					if( dwActualSize > 0 )
					{
						ASSERT( dwActualSize == sizeof(UINT) );
					}

					// If the value is there, read in the data; if not, set it
					if( lResult == ERROR_SUCCESS && dwActualSize == sizeof(UINT))
					{
						// Go read it in
						lResult = ::RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE) (&iValue), &dwActualSize);
					}

					pItem->SetItemValue (iValue);
				}
				break;

			case REGISTRY_TYPE_DOUBLE:
				{
					// MFC stores doubles in the registry as strings
					double dfValue = 0.0;
					LPCTSTR szTempBuffer = NULL;

					pItem->GetDefaultValue( &dfValue );

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);
					
					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Allocate space for it
#ifdef _UNICODE
						szTempBuffer = new WCHAR[dwActualSize];						
						dwSize = dwActualSize * sizeof(WCHAR);
						memset((void *)szTempBuffer, 0, dwSize);
#else
						szTempBuffer = new char[dwActualSize];
						dwSize = dwActualSize * sizeof(char);
						memset((void *)szTempBuffer, 0, dwSize);
#endif
						
						// Go read it in
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (szTempBuffer), &dwSize);

#ifdef _UNICODE
						dfValue = _wtof(szTempBuffer);
#else
						dfValue = atof(szTempBuffer);
#endif
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
					if( dwActualSize > 0 )
					{
						ASSERT( dwActualSize == sizeof(DWORD) );
					}

					// If the value is there, read in the data; if not, set it
					if( lResult == ERROR_SUCCESS && dwActualSize == sizeof(DWORD))
					{
						// Go read it in
						lResult = ::RegQueryValueEx(hKey, szValueName, NULL, &dwType, (LPBYTE) (&dwValue), &dwActualSize);
					}

					pItem->SetItemValue (dwValue);
				}
				break;

			case REGISTRY_TYPE_STRING:
				{
#ifdef _UNICODE
					std::wstring strValue;
#else
					std::string strValue;
#endif
					LPCTSTR szTempBuffer = NULL;
					
					pItem->GetDefaultValue( &strValue );

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL, &dwActualSize);

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Allocate space for it
#ifdef _UNICODE
						szTempBuffer = new WCHAR[dwActualSize];
						dwSize = dwActualSize * sizeof(WCHAR);
						memset((void *)szTempBuffer, 0, dwSize);						
#else
						szTempBuffer = new char[dwActualSize];						
						dwSize = dwActualSize * sizeof(char);
						memset((void *)szTempBuffer, 0, dwSize);
#endif
						
						// Go read it in						
						lResult = RegQueryValueEx(hKey, szValueName, NULL, NULL, (LPBYTE) (szTempBuffer), &dwSize);
						pItem->SetItemValue (szTempBuffer, TRUE);
					}
					else
					{
						pItem->SetItemValue( strValue.c_str() );
					}					

					if (szTempBuffer)
					{
						delete [] szTempBuffer;
						szTempBuffer = NULL;
					}
				}
				break;			

			case REGISTRY_TYPE_CSTRING:
				{
#ifdef __AFX_H__
					CString strValue("");
					LPCTSTR szTempBuffer = NULL;
					
					pItem->GetDefaultValue( &strValue );

					// Query the key to figure out how big it is
					lResult = RegQueryValueEx( hKey, szValueName, NULL, NULL, NULL, &dwActualSize );

					// If the value is there, read in the data; if not, set it
					if (lResult == ERROR_SUCCESS)
					{
						// Allocate space for it
#ifdef _UNICODE
						szTempBuffer = new WCHAR[dwActualSize];
						dwSize = dwActualSize * sizeof(WCHAR);
						memset((void *)szTempBuffer, 0, dwSize);
#else
						szTempBuffer = new char[dwActualSize];
						dwSize = dwActualSize * sizeof(char);
						memset((void *)szTempBuffer, 0, dwSize);
#endif
						
						lResult = RegQueryValueEx( hKey, szValueName, NULL, NULL, (LPBYTE) (szTempBuffer), &dwSize);

						pItem->SetItemValue( szTempBuffer, TRUE );
					}
					else
					{
						pItem->SetItemValue( strValue );
					}					

					if (szTempBuffer)
					{
						delete [] szTempBuffer;
						szTempBuffer = NULL;
					}
#endif		// #ifdef __AFX_H__
				}
				break;			
			
			default:
				break;
			}				
			
			RegCloseKey (hKey);
		}

		pItem = pItem->GetNext();
	}	
	return true;
}

BOOL CRegistryHelper::WriteRegistry()
{
	// First, always create the base key (no harm done)
	HKEY hKey;
	long lResult;	
	DWORD dwDisp;
	DWORD dwSize;

	if( m_strBaseSubKey.length() != 0 )
	{
		lResult = RegCreateKeyEx (m_hMainKey, m_strBaseSubKey.c_str(), 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);
		if( lResult != ERROR_SUCCESS )
		{			
			return false;			
		}
		RegCloseKey( hKey );
	}

	CRegistryItem *pItem = GetFirst();
#ifdef _UNICODE
	std::wstring strItemSubKey;
	std::wstring strFullKey;
	std::wstring strValueName;
#else
	std::string strItemSubKey;
	std::string strFullKey;
	std::string strValueName;	
#endif

	LPCTSTR szValueName = NULL;
	
	while (pItem)
	{
		strItemSubKey = pItem->GetSubKey();
		strFullKey = m_strBaseSubKey;
		strFullKey += (m_strBaseSubKey.length() != 0 ? _T("\\") : _T(""));
		strFullKey += strItemSubKey;
		strValueName = pItem->GetValueName();

		szValueName = strValueName.c_str();

		// Always Create the subkey under our main base key, and grab the handle for it
		lResult = RegCreateKeyEx (m_hMainKey, strFullKey.c_str(), 0,  NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp);
		
		if( lResult != ERROR_SUCCESS )
		{	
			return false;			
		}
		else
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

			case REGISTRY_TYPE_UNSIGNED_INTEGER:
				{
					UINT iItemValue = 0;
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
					pItem->GetItemValue(&dfItemValue);

#ifdef _UNICODE
					std::wstringstream szFormat;
					szFormat << dfItemValue;
					
					// Set the key
					lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (LPBYTE)(szFormat.str().c_str()), szFormat.str().size() + 1);
#else
					char szFormat[256];
					sprintf_s(szFormat, sizeof(szFormat), "%.13lf", dfItemValue);
					
					// Set the key
					lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (LPBYTE)(szFormat), strlen(szFormat) + 1);
#endif
					
					
					
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

			case REGISTRY_TYPE_STRING:			
				{
					int iStringLength = 0;
#ifdef _UNICODE
					std::wstring strItemValue;
					pItem->GetItemValue(&strItemValue);
					iStringLength = strItemValue.size();
					dwSize = iStringLength * sizeof(wchar_t);
#else
					std::string strItemValue;
					pItem->GetItemValue(&strItemValue);
					iStringLength = strItemValue.size();
					dwSize = iStringLength * sizeof(char);
#endif			
					// Set the key
					lResult = RegSetValueEx (hKey, szValueName, 0, REG_SZ, (LPBYTE)(strItemValue.c_str()), dwSize + 1);
				}
				break;			

			case REGISTRY_TYPE_CSTRING:
				{
#ifdef __AFX_H__
					CString strItemValue("");
					LPCTSTR szTempBuffer = NULL;
					int iStringLength = 0;

					pItem->GetItemValue (&strItemValue);
									
					iStringLength = strItemValue.GetLength();
					szTempBuffer = strItemValue.GetBuffer(iStringLength);

					// Set the key
#ifdef _UNICODE
					lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (LPBYTE)(szTempBuffer), iStringLength * sizeof(wchar_t) + 1);
#else
					lResult = RegSetValueEx( hKey, szValueName, 0, REG_SZ, (LPBYTE)(szTempBuffer), iStringLength + 1 );
#endif

					strItemValue.ReleaseBuffer();
#endif		// #ifdef __AFX_H__
				}
				break;
			
			default:
				break;
			}				
			
			RegCloseKey (hKey);
		}

		pItem = pItem->GetNext();
	}	
	return true;
}

	
