// RegistryHelper.h: interface for the CRegistryHelper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _REGISTRYHELPER_H_
#define _REGISTRYHELPER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>

#define REGISTRY_TYPE_UNKNOWN				0
#define REGISTRY_TYPE_INTEGER				1
#define REGISTRY_TYPE_UNSIGNED_INTEGER		2
#define REGISTRY_TYPE_DOUBLE				4
#define REGISTRY_TYPE_DWORD					5
#define REGISTRY_TYPE_STRING				6
#define REGISTRY_TYPE_CSTRING				7

#define REGISTRY_MIN						0x1
#define REGISTRY_MAX						0x2

class CRegistryItem
{
private:
	int *m_pInteger;
	int m_iMinValue;
	int m_iMaxValue;
	int m_iDefaultValue;

	UINT *m_pUInteger;
	UINT m_uiMinValue;
	UINT m_uiMaxValue;
	UINT m_uiDefaultValue;
	
	double *m_pDouble;
	double m_dfMinValue;
	double m_dfMaxValue;
	double m_dfDefaultValue;

	LPDWORD m_lpDWord;
	DWORD m_dwMinValue;
	DWORD m_dwMaxValue;
	DWORD m_dwDefaultValue;

#ifdef _UNICODE
	std::wstring *m_pString;
	std::wstring m_strDefaultValue;
	std::wstring m_strSubKey;
	std::wstring m_strValueName;
#else
	std::string *m_pString;
	std::string m_strDefaultValue;
	std::string m_strSubKey;
	std::string m_strValueName;
#endif

#ifdef __AFX_H__
	CString *m_pCString;
	CString m_cstrDefaultValue;
#endif		// #ifdef __AFX_H__

	int m_iItemType;
	DWORD m_dwFlags;
	
	CRegistryItem *m_pNext;
	CRegistryItem *m_pPrevious;
	BOOL m_bEncrypt;

// Private methods
private:
	void InitMembers();
	void SetItemType(int iType);
	void SetSubKey(LPCTSTR szSubKey);
	void SetValueName(LPCTSTR szValueName);
	void SetFlags( DWORD dwFlags = 0);	
		
#ifdef _UNICODE	
	std::wstring TrimSlashes(std::wstring &strTrim);
#else	
	std::string TrimSlashes(std::string &strTrim);
#endif

// Public methods
public:
	CRegistryItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	CRegistryItem (UINT *pUInteger, UINT iDefaultValue, UINT iMinValue, UINT iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	CRegistryItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	CRegistryItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);

#ifdef _UNICODE	
	CRegistryItem (std::wstring *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
	void GetDefaultValue(std::wstring *pstrDefaultValue);
	void GetItemValue(std::wstring *pstrItemValue);
#else
	CRegistryItem(std::string *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
	void GetDefaultValue(std::string *pstrDefaultValue);
	void GetItemValue(std::string *pstrItemValue);
#endif

#ifdef __AFX_H__
	CRegistryItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
#endif		// #ifdef __AFX_H__
	~CRegistryItem();
	
	void GetDefaultValue (int *piDefaultValue);
	void GetDefaultValue (UINT *puiDefaultValue);
	void GetDefaultValue (double *pdfDefaultValue);
	void GetDefaultValue (LPDWORD pdwDefaultValue);
#ifdef __AFX_H__
	void GetDefaultValue (CString *pcstrDefaultValue);
#endif		// #ifdef __AFX_H__
	
	void GetItemValue (int *piItemValue);
	void GetItemValue (UINT *puiItemValue);
	void GetItemValue (double *pdfItemValue);
	void GetItemValue (LPDWORD pdwItemValue);
#ifdef __AFX_H__
	void GetItemValue (CString *pcstrItemValue);
#endif		// #ifdef __AFX_H__

	void AdjustMinMax();
	LPCTSTR GetSubKey();
	LPCTSTR GetValueName();
	int GetItemType();
	DWORD GetFlags();
	
	void SetItemValue (LPCTSTR szValue, BOOL bEncrypted = FALSE);
	void SetItemValue (int iValue);
	void SetItemValue (UINT iValue);
	void SetItemValue (double dfValue);
	void SetItemValue (DWORD dwValue);

	void SetNext (CRegistryItem *pNext);
	CRegistryItem *GetNext();
	void SetPrevious (CRegistryItem *pPrevious);
	CRegistryItem *GetPrevious();	
};

class CRegistryHelper  
{
private:
	HKEY m_hMainKey;

#ifdef _UNICODE
	std::wstring m_strBaseSubKey;
#else
	std::string m_strBaseSubKey;
#endif
	
	CRegistryItem *m_pFirst;
	CRegistryItem *m_pWrite;

public:
	CRegistryHelper();
	virtual ~CRegistryHelper();

	void SetMainKey (HKEY hMainKey);
	void SetBaseSubKey (LPCTSTR szBaseSubKey);

	void SetFirst (CRegistryItem *pFirst);
	CRegistryItem *GetFirst();
	void SetWrite (CRegistryItem *pWrite);
	CRegistryItem *GetWrite();

	void PurgeList();
#ifdef _UNICODE
	std::wstring TrimSlashes(std::wstring &strTrim);
#else
	std::string TrimSlashes( std::string &strTrim );
#endif
	void AddItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	void AddItem (UINT *pInteger, UINT iDefaultValue, UINT iMinValue, UINT iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	void AddItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
	void AddItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey, DWORD dwFlags = 0);
#ifdef _UNICODE
	void AddItem (std::wstring *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
#else
	void AddItem(std::string *pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
#endif

#ifdef __AFX_H__
	void AddItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey, BOOL bEncrypt = FALSE, DWORD dwFlags = 0);
#endif		// #ifdef __AFX_H__
	void AddItemToList (CRegistryItem *pItem);

	void SetAll (CRegistryItem *pItem);

	BOOL ReadRegistry(DWORD dwSAMDesired = 0);
	BOOL WriteRegistry();
};

#endif // #ifndef _REGISTRYHELPER_H_
