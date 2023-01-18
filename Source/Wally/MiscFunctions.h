#ifndef __MISC_FUNCTIONS_H_
#define __MISC_FUNCTIONS_H_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _WALLY_H_
	#include "Wally.h"
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WANT_FULLPATH	0x1
#define WANT_FILENAME	0x2

// MiscFunctions.h

CString GetExtension (CString FileName);
CString GetRawFileName (CString FileName);
CString GetRawFileNameWExt (CString FileName);
CString SetModifiedTitle (CString Title);
CString GetWildCardExtension (CString WildCard, int Index);
CString GetWildCardFromList (CString strImageList, int Index);
CString GetParentDirectory (CString PathName);
CString GetPathToFile (CString strFile);
CString BrowseForFolder (LPCTSTR szTitle, LPCTSTR szStartDirectory = NULL);
int GetFileLength (FILE *f);
void FindAllFiles (LPCTSTR szPath, CStringArray *pStrArray, int iFlags);
CString TrimSlashes (LPCTSTR szSource);
CString TrimLeadingSlashes (LPCTSTR szSource);
CString TrimLeadingCharacters (LPCTSTR szSource, char cCharacter);
CString TrimFromLeft (LPCTSTR szSource, LPCTSTR szTrimString);
CString ConvertAllCharacters (LPCTSTR szSource, char cMatch, char cChange);
int GetNumCharacters (LPCTSTR szString, char cMatch);
void GetAllWildCards( LPSTR szWildCard, CStringArray *psaWildCards );
BOOL MatchesWildCard (char *szSource, CStringArray *psaWildCards);
LPVOID MakeFile( LPCTSTR szFileName, DWORD dwFileSize );
void CloseFile( LPVOID lpFile );

#ifndef _FNMATCH_H
#define _FNMATCH_H      1

/* Bits set in the FLAGS argument to `fnmatch'.  */
#define FNM_PATHNAME    (1 << 0)/* No wildcard can ever match `\'.  */
#define FNM_NOESCAPE    (1 << 1)/* Backslashes don't quote special chars.  */
#define FNM_PERIOD      (1 << 2)/* Leading `.' is matched only explicitly.  */
#define __FNM_FLAGS     (FNM_PATHNAME | FNM_NOESCAPE | FNM_PERIOD)

/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define FNM_NOMATCH     1

/* Match STRING against the filename pattern PATTERN,
   returning zero if it matches, FNM_NOMATCH if not.  */
extern int fnmatch(char *pattern, char *string, int flags);

#endif /* fnmatch.h */


void StripNewLine (char *Source);
void DebugOut (char *text, bool overwrite);
inline int PadDWORD (int x)  { return ((x+3) & ~3); }
inline int PadWORD (int x)   { return ((x+1) & ~1); }

inline bool IsValidSize( int iFileType, int iLength)
{
	if ((iFileType >= FILE_TYPE_QUAKE2) && (iFileType <= FILE_TYPE_HALF_LIFE_WAD))
	{
		div_t result;
		result = div( iLength, 16);

		return (result.rem == 0);
	}
	else
	{
		ASSERT ((iFileType >= FILE_TYPE_START) && (iFileType < FILE_TYPE_END));
		return true;		// assume game allows any sized texture
	}
}

BOOL CalcImageColor256 (int iWidth, int iHeight, BYTE *pbyImageData, BYTE *pbyPalette, float *pfR, float *pfG, float *pfB, BOOL bNormalize, BOOL bIgnoreBlue = FALSE );
BOOL CalcImageColor24 (int iWidth, int iHeight, COLOR_IRGB *pirgbImageData, float *pfR, float *pfG, float *pfB, BOOL bNormalize = FALSE, BOOL bIgnoreBlue = FALSE );
BOOL CalcImageColor24( int iWidth, int iHeight, LPBYTE pbyImageData, float *pfR, float *pfG, float *pfB, BOOL bNormalize = FALSE, BOOL bIgnoreBlue = FALSE );
BOOL BlendPalette (BYTE *pbyPalette, int iIndexLeft, int iIndexRight);

class CCommandLineItem
{
// Members
public:
	char FileName[_MAX_PATH * 5];
	CCommandLineItem *p_Next;

// Methods
public:
	CCommandLineItem (char *p_FileName);
};	


class CParseCommandLine
{
// Members
public:
	CCommandLineItem *p_First;
	CCommandLineItem *p_Read;
	CCommandLineItem *p_Write;
	CCommandLineItem *p_Temp;

// Methods
public:
	CParseCommandLine ();
	void Parse (char *p_CommandLine);
	void AddItem (char *NewItem);			
	bool IsEmpty ();	
	void GetFirst();
	char *GetNext();
};

#endif		// __MISC_FUNCTIONS_H_