// PakList.h: interface for the CPakItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _PAKLIST_H_
#define _PAKLIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define	SERIALIZE_READ				0x1
#define SERIALIZE_WRITE				0x2
#define SERIALIZE_MODIFIED_UNKNOWN	0x4
#define SERIALIZE_MODIFIED_NAME		0x8
#define SERIALIZE_MODIFIED_DATA		0x10
#define SERIALIZE_SAVE_AS			0x20

#define SERIALIZE_STAGE_DATA		0
#define SERIALIZE_STAGE_ENTRY		1
#define SERIALIZE_STAGE_CLEANUP		2

// AddFile Flags
#define PAK_ADD_FLAG_RECURSE			0x1
#define PAK_ADD_FLAG_REPLACE			0x2
#define PAK_ADD_FLAG_RETAIN				0x4

// Export Flags
#define PAK_EXPORT_FLAG_RECURSE			0x1
#define PAK_EXPORT_FLAG_REPLACE			0x2
#define PAK_EXPORT_FLAG_RETAIN			0x4
#define PAK_EXPORT_FLAG_ADD_TEMP		0x8

// Types
#define PAK_TYPE_ITEM			0
#define PAK_TYPE_DIRECTORY		1

// Count flags
#define PAK_COUNT_SELF_ONLY					0x1

// Error codes
#define PAK_ERROR_START					0
#define PAK_ERROR_SUCCESS				(PAK_ERROR_START + 0)
#define PAK_ERROR_MALFORMED				(PAK_ERROR_START + 1)
#define PAK_ERROR_FILEOPEN				(PAK_ERROR_START + 2)
#define PAK_ERROR_OUTOFMEMORY			(PAK_ERROR_START + 3)
#define PAK_ERROR_NAME_EXCEED_LENGTH	(PAK_ERROR_START + 4)
#define PAK_ERROR_READ_FAILURE			(PAK_ERROR_START + 5)
#define PAK_ERROR_WRITE_FAILURE			(PAK_ERROR_START + 6)
#define PAK_ERROR_NO_ASSOC				(PAK_ERROR_START + 7)
#define PAK_ERROR_COPY_FILE				(PAK_ERROR_START + 8)
#define PAK_ERROR_DUPLICATE_NAME		(PAK_ERROR_START + 9)
#define PAK_ERROR_INVALID_PAK			(PAK_ERROR_START + 10)
#define PAK_ERROR_ZERO_LENGTH			(PAK_ERROR_START + 11)
#define PAK_ERROR_WALLY_ERROR			(PAK_ERROR_START + 12)
#define PAK_ERROR_MAX_FILES				(PAK_ERROR_START + 13)

#define IDPAKHEADER		(('K'<<24)+('C'<<16)+('A'<<8)+'P')
#define	MAX_FILES_IN_PACK	4096

class CPakItem;
class CPakDirectory;
class CPakBase;
class CProgressBar;

// This struct is what is used to fill out the lParam for the List items.  Because we might have
// either CPakDirectory or CPakItem as a list item, we need a common, unrelated struct as the pointer
// to be used.  Each CPakDirectory and CPakItem will contain one of these structs, filled out
// accordingly.
typedef struct
{
	int iPakType;
	CPakItem *pPakItem;
	CPakDirectory *pPakDirectory;
	CPakBase *pPakBase;
} PAK_PARAM, *lpPAK_PARAM;

typedef struct
{
	char	name[56];
	int		filepos, filelen;
} packfile_t, *lp_packfile_t;

typedef struct
{
	int		ident;		// == IDPAKHEADER
	int		dirofs;
	int		dirlen;
} packheader_t, *lp_packheader_t;



class CPakPasteItem
{
private:
	CPakBase *m_pPakBase;
	
	CPakPasteItem *m_pNext;
	CPakPasteItem *m_pPrevious;

public:
	CPakPasteItem (CPakBase *pPakBase)
	{
		SetNext (NULL);
		SetPrevious (NULL);
		m_pPakBase = pPakBase;
	}
	~CPakPasteItem()
	{
		SetNext (NULL);
		SetPrevious (NULL);
	}
	CPakBase *GetPakBase()
	{
		return m_pPakBase;
	}
	
	void SetNext(CPakPasteItem *pNext);
	CPakPasteItem *GetNext();
	void SetPrevious(CPakPasteItem *pPrevious);
	CPakPasteItem *GetPrevious();
};

class CPakBase 
{	
protected:
	CString m_strError;
	int m_iErrorCode;

	CString m_strDisplayName;
	int m_iDisplaySize;
	CString m_strDisplayType;

	int m_iPakType;
	PAK_PARAM m_PakParam;
	
public:	
	CPakBase();
	operator lpPAK_PARAM()
	{
		return &m_PakParam;
	}
	CString GetDisplayName()
	{
		return m_strDisplayName;
	}
	void SetDisplayName(LPCTSTR szDisplayName)
	{
		m_strDisplayName = szDisplayName;
	}
	int GetDisplaySize()
	{
		return m_iDisplaySize;
	}
	void SetDisplaySize (int iDisplaySize)
	{
		m_iDisplaySize = iDisplaySize;
	}
	CString GetDisplayType()
	{
		return m_strDisplayType;
	}
	void SetDisplayType(LPCTSTR szDisplayType)
	{
		m_strDisplayType = szDisplayType;
	}	
	void SetPakType(int iPakType)
	{
		m_PakParam.iPakType = iPakType;
	}
	
	void SetErrorString(LPCTSTR szError)
	{
		m_strError = szError;
	}
	CString GetErrorString()
	{
		return m_strError;
	}
	CString GetError();
	int GetErrorCode()
	{
		return m_iErrorCode;
	}
	void SetErrorCode(int iErrorCode)
	{
		m_iErrorCode = iErrorCode;
	}	
};

class CPakItem : public CPakBase
{
private:
	CPakItem *m_pNext;
	CPakItem *m_pPrevious;

	packfile_t m_PackFileHeader;
	CString m_strDataFile;
	CString m_strShortName;
	CString m_strType;

	int m_iIconIndex;

	// The CTime member is to check whether the file has actually been modified since
	// we wrote it out to disk.  The m_bModified member is in case one of our parent
	// directories was renamed
	CTime m_ctLastModified;
	BOOL m_bNameModified;

	// These are temporary values during Serialize().  We don't want to modify the current 
	// entries if something goes wrong outside of our scope that prevents the Serialize() from
	// finishing properly.
	UINT m_iNewFileLen;
	UINT m_iNewFilePos;

	BYTE *m_pbyTempBuffer;

public:
	CPakItem();
	CPakItem(lp_packfile_t lpPackFile);	
	virtual ~CPakItem();
	void Initialize();

	BOOL Create (LPCTSTR szDataFile, LPCTSTR szDirectory, LPCTSTR szTempDirectory);
	BOOL CreateTempFile (LPCTSTR szPakFile, LPCTSTR szDirectory, int iFlags = 0);
	BOOL ExportFile (LPCTSTR szPakFile, LPCTSTR szDirectory, CStringArray *psaWildCards, CProgressBar *pProgressBar);
	BOOL ReplaceDataFile (LPCTSTR szFileName);
	BOOL OpenItem();
	BOOL QuickPlay (LPCTSTR szPakFile);
	BOOL IsModified(int *piFlags);

	int GetIconIndex()
	{
		return m_iIconIndex;
	}
	void SetIconIndex (int iIconIndex)
	{
		m_iIconIndex = iIconIndex;
	}
	CString GetDataFile()
	{
		return m_strDataFile;
	}

	void SetNext (CPakItem *pNext);
	CPakItem *GetNext();
	void SetPrevious (CPakItem *pPrevious);
	CPakItem *GetPrevious();

	BOOL Serialize (int iStage, CPakDirectory *pParent, CFile *pNewPakFile, CFile *pOldPakFile, UINT *piPosition, UINT *piCount, CProgressBar *pProgressBar);
	
	void SetType(LPCTSTR szType)
	{
		m_strType = szType;
	}
	CString GetType()
	{
		return m_strType;
	}
	CString GetName()
	{
		return m_PackFileHeader.name;
	}
	CString GetShortName()
	{
		return m_strShortName;
	}
	void SetShortName (LPCTSTR szShortName);
	void RenameDataFile (LPCTSTR szTempDirectory);
	int GetFilePos()
	{
		return m_PackFileHeader.filepos;
	}
	int GetFileLen()
	{
		return m_PackFileHeader.filelen;
	}
	operator lp_packfile_t ()
	{
		return &m_PackFileHeader;
	}
};

class CPakDirectory : public CPakBase
{
private:
	CPakItem *m_pFirstItem;
	CPakItem *m_pWriteItem;
	CPakItem *m_pReadItem;

	CPakDirectory *m_pFirstDirectory;
	CPakDirectory *m_pWriteDirectory;
	
	CPakDirectory *m_pNext;
	CPakDirectory *m_pPrevious;

	CPakDirectory *m_pParent;

	CString m_strDirectoryName;
	CString *m_pstrRootTempDirectory;
	CString *m_pstrPakFile;
	CString m_strTempDirectoryName;

	BOOL m_bModified;

public:
	CPakDirectory();
	CPakDirectory(LPCTSTR szDirectory, CPakDirectory *pParent, CString *pstrTempDirectory, CString *pstrPakFile);
	virtual ~CPakDirectory();

	void PurgeAll();
	int AddToList (LPCTSTR szFileName, lp_packfile_t lpPackFile);
	CString GetDirectoryName();
	void SetDirectoryName (LPCTSTR szDirectory = NULL);
	CString GetTempDirectoryName();

	void SetFirstItem(CPakItem *pFirst);
	CPakItem *GetFirstItem();
	void SetWriteItem(CPakItem *pWrite);
	CPakItem *GetWriteItem();

	void SetNext(CPakDirectory *pNext);
	CPakDirectory *GetNext();
	void SetPrevious(CPakDirectory *pPrevious);
	CPakDirectory *GetPrevious();

	void SetFirstDirectory(CPakDirectory *pFirst);
	CPakDirectory *GetFirstDirectory();
	void SetWriteDirectory(CPakDirectory *pWrite);
	CPakDirectory *GetWriteDirectory();	

	CPakItem *GetFirstPakItem();
	CPakItem *GetNextPakItem();	
	void GetItemCount(UINT *piCount, UINT *piSize = NULL, CStringArray *psaWildCards = NULL, int iFlags = 0);

	void BuildTree (CTreeCtrl& List, HTREEITEM htParent, int iClosedIcon, int iOpenIcon);
	BOOL Serialize (int iStage, CFile *pNewPakFile, CFile *pOldPakFile, UINT *piPosition, UINT *piCount, CProgressBar *pProgressBar);

	CPakDirectory *AddChildDirectory (LPCTSTR szDirectory);
	BOOL AddFileDirectory (LPCTSTR szDirectory, CStringArray *psaWildcards, int iFlags);
	BOOL AddFile (LPCTSTR szFile, LPCTSTR szOffsetDirectory = NULL, int iFlags = 0);
	void DeleteItem (CPakItem *pItem);
	
	BOOL CreateTempDirectory (LPCTSTR szPakFile, LPCTSTR szTempDirectory);
	void DeleteDirectory (CPakDirectory *pDirectory);
	CPakDirectory *GetParent();
	
	CString GetPathFromHere(char cSeparator);
	CPakItem *FindNameInList (LPCTSTR szFileName);
	CPakDirectory *FindDirectoryInList (LPCTSTR szDirectory);
	CPakDirectory *FindDirectory (LPCTSTR szDirectory);
	BOOL ExportFiles (LPCTSTR szDirectory, CStringArray *psaWildCards, CProgressBar *pProgressBar);

	BOOL IsModified(int *piFlags);
};

class CPakList : public CPakBase
{
private:
	CPakDirectory *m_pRoot;
	CString m_strFileName;
	CString m_strTempDirectory;

	CPakPasteItem *m_pFirstPasteItem;
	CPakPasteItem *m_pWritePasteItem;


public:
	CPakList();
	virtual ~CPakList();
	void CreateTempDirectory();
	void CreateTempName(char *szTempName, int iLength);

	BOOL Serialize(CFile *pFile, LPCTSTR szActualFileName, int iFlags);
	BOOL Serialize(LPCTSTR szPakFile, int iFlags);
	UINT GetItemCount();

	CString GetFileName();
	CString GetTempDirectory();
	CPakDirectory *GetRootDirectory();
	
	BOOL CreateDirectory (CPakDirectory *pDirectory);
	BOOL CreateItem (CPakItem *pItem);
	BOOL OpenItem (CPakItem *pItem);
	
	BOOL IsModified(int *piFlags);

	void AddToPasteList(CPakBase *pItem);
	void PurgePasteList();
	int GetPasteCount();
	
	CPakDirectory *FindDirectory (LPCTSTR szDirectory);
	void SetFirstPasteItem (CPakPasteItem *pFirst);
	CPakPasteItem *GetFirstPasteItem();
	void SetWritePasteItem (CPakPasteItem *pWrite);
	CPakPasteItem *GetWritePasteItem();
};


#endif // #ifndef _PAKLIST_H_

