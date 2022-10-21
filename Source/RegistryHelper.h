// RegistryHelper.h: interface for the CRegistryHelper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _REGISTRYHELPER_H_
#define _REGISTRYHELPER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define REGISTRY_TYPE_UNKNOWN		0
#define REGISTRY_TYPE_INTEGER		1
#define REGISTRY_TYPE_DOUBLE		2
#define REGISTRY_TYPE_DWORD			3
#define REGISTRY_TYPE_CSTRING		4
#define REGISTRY_TYPE_STRING		5

class CRegistryItem
{
private:
	int *m_pInteger;
	int m_iMinValue;
	int m_iMaxValue;
	int m_iDefaultValue;
	
	double *m_pDouble;
	double m_dfMinValue;
	double m_dfMaxValue;
	double m_dfDefaultValue;

	LPDWORD m_lpDWord;
	DWORD m_dwMinValue;
	DWORD m_dwMaxValue;
	DWORD m_dwDefaultValue;
	
	CString *m_pCString;
	LPTSTR m_pString;
	CString m_strDefaultValue;

	CString m_strSubKey;
	CString m_strValueName;	
	int m_iItemType;
	
	CRegistryItem *m_pNext;
	CRegistryItem *m_pPrevious;

// Private methods
private:
	void InitMembers();
	void SetItemType(int iType);
	void SetSubKey(LPCTSTR szSubKey);
	void SetValueName(LPCTSTR szValueName);
	CString TrimSlashes (LPCTSTR szSource);

// Public methods
public:
	CRegistryItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	CRegistryItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	CRegistryItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	CRegistryItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	CRegistryItem (LPTSTR pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	~CRegistryItem();

	void GetDefaultValue (CString *pstrDefaultValue);
	void GetDefaultValue (int *piDefaultValue);
	void GetDefaultValue (double *pdfDefaultValue);
	void GetDefaultValue (LPDWORD pdwDefaultValue);

	void GetItemValue (CString *pstrItemValue);
	void GetItemValue (int *piItemValue);
	void GetItemValue (double *pdfItemValue);
	void GetItemValue (LPDWORD pdwItemValue);

	void AdjustMinMax();
	CString GetSubKey();
	CString GetValueName();
	int GetItemType();
	
	void SetItemValue (LPCTSTR szValue);
	void SetItemValue (int iValue);
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
	CString m_strBaseSubKey;
	
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
	CString TrimSlashes (LPCTSTR szSource);
	void AddItem (int *pInteger, int iDefaultValue, int iMinValue, int iMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	void AddItem (double *pDouble, double dfDefaultValue, double dfMinValue, double dfMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	void AddItem (LPDWORD lpDWord, DWORD dwDefaultValue, DWORD dwMinValue, DWORD dwMaxValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	void AddItem (CString *pCString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	void AddItem (LPTSTR pString, LPCTSTR szDefaultValue, LPCTSTR szValueName, LPCTSTR szSubKey);
	void AddItemToList (CRegistryItem *pItem);

	void SetAll (CRegistryItem *pItem);

	void ReadRegistry();
	void WriteRegistry();
};

#endif // #ifndef _REGISTRYHELPER_H_
