/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// MiscFunctions.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiscFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The functions within here are just miscellaneous functions
// that can be used by any class.  IE string manipulation, etc.


void DebugOut(const char* text, bool overwrite)
{
#ifdef _DEBUG
	FILE* wp = NULL;
	errno_t err = 0;
	if (overwrite)
		err = fopen_s(&wp, "D:\\temp\\wallyout.txt", "w");
	else
		err = fopen_s(&wp, "D:\\temp\\wallyout.txt", "a");

	if (err == 0)
	{ 
		fprintf(wp, "%s\n", text);
		fclose(wp);
	}	
#endif 
}

///////////////////////////////////////////////////////////////////////
// Name:		GetExtension
// Action:		Return the extension from a specified FileName
//
// Returns:		".pcx", ".mip", etc.
///////////////////////////////////////////////////////////////////////
CString GetExtension( CString strFileName)
{
	// neal - this is a more robust way of getting the extension

	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFileName[_MAX_FNAME];
	char szExt[_MAX_EXT];

	_splitpath_s( strFileName, szDrive, sizeof(szDrive), szDir, sizeof(szDir), szFileName, sizeof(szFileName), szExt, sizeof(szExt));

	// make it lowercase for easy comparisons
	CString strExtension( szExt);
	strExtension.MakeLower();

	return strExtension;
}

///////////////////////////////////////////////////////////////////////
// Name:		GetRawFileNameWExt
// Action:		Strip the path from the specified FileName, leaving the 
//				extension on.  Return the resultant string.
///////////////////////////////////////////////////////////////////////
CString GetRawFileNameWExt (CString FileName)
{
	// Plop in the existing FileName, so there's always enough room
	CString RawName;
	int j;

	if (FileName.Find('\\') != -1)
	{
		// Start at the end of the string, working backwards until a \ is found
		for (j = FileName.GetLength() - 1; (FileName.GetAt(j) != '\\') && (j >= 0); j--);
		j++;
	}
	else
		j = 0;
	
	// From the character after the '\', read up until the end of the string, adding to our string
	while (j < FileName.GetLength())
	{
		RawName += FileName.GetAt(j++);	
	}
	
	return RawName;
}
///////////////////////////////////////////////////////////////////////
// Name:		GetRawFileName
// Action:		Strip the path and extension from the specified FileName,
//				and return the resultant string.
///////////////////////////////////////////////////////////////////////
CString GetRawFileName( CString FileName )
{
	// Plop in the existing FileName, so there's always enough room
	CString RawName;
	int j;

	if (FileName.Find('.') == -1)
	{
		return FileName;
	}

	if (FileName.Find('\\') != -1)
	{
		// Start at the end of the string, working backwards until a \ is found
		for (j = FileName.GetLength() - 1; (FileName.GetAt(j) != '\\') && (j >= 0); j--);
		j++;
	}
	else
		j = 0;
	
	// From the character after the '\', read up until the '.', adding to our string
	while ((FileName.GetAt(j) != '.') && (j < FileName.GetLength()))
		RawName += FileName.GetAt(j++);	
	
	return RawName;
}

CString GetPathToFile (CString strFile)
{
	// Plop in the existing FileName, so there's always enough room
	CString strPath;
	int j;

	if (strFile.Find('\\') != -1)
	{
		// Start at the end of the string, working backwards until a \ is found
		for (j = strFile.GetLength() - 1; (strFile.GetAt(j) != '\\') && (j >= 0); j--);
		j++;
	}
	else
	{
		return "";
	}
	
	// Grab everything but the stuff after our last '\'

	strPath = strFile.Left(j);	
	return strPath;

}


///////////////////////////////////////////////////////////////////////
// Name:		SetModifiedTitle
// Action:		Add an asterisk to the end of the string specified, so
//				long as there isn't already one there
///////////////////////////////////////////////////////////////////////
CString SetModifiedTitle (CString Title)
{
	int j;
	CString NewTitle(Title);

	for (j = 0; j < NewTitle.GetLength() - 1; j++)
	{
		if (NewTitle.GetAt(j) == '*')
			return NewTitle;		
	}
	NewTitle += '*';
	return NewTitle;

}


///////////////////////////////////////////////////////////////////////
// Name:		StripNewLine
// Action:		Remove the NewLine character ('\n') from the specified
//				string, and terminate the string at that location
///////////////////////////////////////////////////////////////////////
void StripNewLine (char *Source)
{        
    for (int j = 0; Source[j] != '\0'; j++)
    {
		if (Source[j] == '\n')
        Source[j] = '\0';
     }
     return;
}



///////////////////////////////////////////////////////////////////////
// Name:		CCommandLineItem Constructor
// Action:		Copy the specified string into the FileName member 
//				variable, and set the linked list p_Next pointer to NULL
///////////////////////////////////////////////////////////////////////
CParseCommandLine::CParseCommandLine ()
{
}

void CParseCommandLine::Parse (char *p_CommandLine)
{		
	int x = 0;
	unsigned int j;
	char TempItem[256];
	for (j = 0; j < (strlen(p_CommandLine) + 1); j++)
	{
		if ((p_CommandLine[j] == ' ') || (p_CommandLine[j] == '\0'))
		{	
			TempItem[x] = '\0';
			AddItem (TempItem);
			x = 0;
		}
		else
		{
			TempItem[x] = p_CommandLine[j];
			x++;
		}
	}
}


void CParseCommandLine::AddItem (char *NewItem)
{
	m_FileNames.push_back(CString(NewItem));
}


//int PadDWORD (int x)
//{
//	div_t xdiv = div ((x + 3), 4);	
//	return (4 * xdiv.quot);
//}

CString GetWildCardExtension (CString WildCard, int Index)
{
	int j = 0;
	int x = 0;
	int iPos = WildCard.Find('|');
	if (iPos == -1)
	{
		ASSERT (false);		// Something's wrong with your wildcard
		return "";
	}
		
	// Find the first '|'
	while ((WildCard.GetAt(j++) != '|') && (j < WildCard.GetLength()));

	if (j == WildCard.GetLength())
	{
		return "";
	}

	// Find the second '|'	
	while ((WildCard.GetAt(j++) != '|')  && (j < WildCard.GetLength()));

	if (j == WildCard.GetLength())
	{
		return "";
	}

	for (x = 0; x < Index; x++)	
	{
		// Find the first '|'
		while ((WildCard.GetAt(j++) != '|') && (j < WildCard.GetLength()));  
		if (j == WildCard.GetLength())
		{
			return "";
		}

		// Find the second '|'	
		while ((WildCard.GetAt(j++) != '|')  && (j < WildCard.GetLength()));
	
		if (j == WildCard.GetLength())
		{
			return "";
		}
	}
	j -= 4;

	CString rString (WildCard.Mid(j, 3));
	return rString;

}

CString GetWildCardFromList (CString strImageList, int Index)
{
	int j = 0;
	int x = 0;
	int iStart = 0;

	if (strImageList.Find('|') == -1)
	{
		ASSERT (false);		// Something's wrong with your wildcard
		return "";
	}

	iStart = 0;
	// Find the first '|'
	while ((strImageList.GetAt(j++) != '|') && (j < strImageList.GetLength()));

	if (j == strImageList.GetLength())
	{
		return "";
	}

	// Find the second '|'	
	while ((strImageList.GetAt(j++) != '|')  && (j < strImageList.GetLength()));
	
	if (j == strImageList.GetLength())
	{
		return "";
	}


	for (x = 0; x < Index; x++)	
	{
		iStart = j;
		// Find the first '|'
		while ((strImageList.GetAt(j++) != '|') && (j < strImageList.GetLength()));

		if (j == strImageList.GetLength())
		{
			return "";
		}

		// Find the second '|'	
		while ((strImageList.GetAt(j++) != '|')  && (j < strImageList.GetLength()));
		
		if (j == strImageList.GetLength())
		{
			return "";
		}

	}

	CString rString (strImageList.Mid(iStart, iStart + j));
	return rString;
}


CString GetParentDirectory (CString PathName)
{
	// Input will look like this:  c:\quake2\baseq2\textures\level1\nice.wal
	// We want to return the "level1\"

	int length = PathName.GetLength();
	int MarkerRight = 0;
	int MarkerLeft = 0;
	int j = 0;

	// There is no directory path, get out
	if (PathName.Find('\\') == -1)
	{
		return "";
	}

	// Start at the end of the string, working backwards until the first \ is found
	for (j = length - 1; (j >= 0) && (PathName.GetAt(j) != '\\'); j--);	
	MarkerRight = j;

	if (MarkerRight == 0)
		return "";
	
	// Go find the next '\' 
	for (j = MarkerRight - 1; (j >= 0) && (PathName.GetAt(j) != '\\'); j--);	
	MarkerLeft = j;

	if (MarkerLeft <= 0)
		return "";
	
	CString Directory (PathName.Mid (MarkerLeft + 1, (MarkerRight - MarkerLeft) - 1));
	return Directory;
	
}

int CALLBACK BrowseCallbackProc(
	HWND hwnd,	
	UINT uMsg,	
	LPARAM lParam,	
	LPARAM lpData	
   )
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData );
		break;
		}
	return 0;
}

CString BrowseForFolder (LPCTSTR szTitle, LPCTSTR szStartDirectory /* = NULL */)
{
	char szName[_MAX_PATH * 5] = "";	// temp storage for partial pathname
	
	if (szStartDirectory)
	{
		CString strStartDirectory ("");
		strStartDirectory = TrimSlashes (szStartDirectory);
		
		strcpy_s (szName, sizeof(szName), strStartDirectory.GetBuffer(strStartDirectory.GetLength()));
	}

	BROWSEINFO BrowseInfo;

	BrowseInfo.hwndOwner      = AfxGetMainWnd()->m_hWnd;
	BrowseInfo.pidlRoot       = NULL;	
	BrowseInfo.pszDisplayName = szName;
	BrowseInfo.lpszTitle      = szTitle;
	BrowseInfo.ulFlags        = BIF_RETURNONLYFSDIRS;
	BrowseInfo.lpfn           = BrowseCallbackProc;
	BrowseInfo.lParam         = (LPARAM)szName;
	BrowseInfo.iImage         = 0;

	LPITEMIDLIST pItemList = SHBrowseForFolder( &BrowseInfo);
	CString      strPathName ("");	

	LPMALLOC pMalloc = NULL;
	if (SHGetMalloc( &pMalloc) == NOERROR)
	{
		if (SHGetPathFromIDList( pItemList, szName))
		{
			int iLen = strlen( szName);

			// start at 1 so drive letter is capitalized
			for (int i = 1; i < iLen; i++)
				szName[i] = (char )tolower( szName[i]);

			strPathName = szName;
		}	
		pMalloc->Free( pItemList);
	}
	else
		MessageBeep( 0);		// neal - need a msg box?

  
	return strPathName;
}

int GetFileLength (FILE *f)
{
	int		pos;
	int		end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

CString TrimFromLeft (LPCTSTR szSource, LPCTSTR szTrimString)
{
	CString strSource(szSource);
	CString strTrim(szTrimString);

	if (strTrim.GetLength() >= strSource.GetLength())
	{
		return "";
	}

	int iTrimPosition = strTrim.GetLength();
	int j = 0;

	for (j = 0; (j < strTrim.GetLength()) && (j < strSource.GetLength()); j++)
	{
		if (strTrim.GetAt(j) != strSource.GetAt(j))
		{
			iTrimPosition = j + 1;
			
			// Stop looping
			j = strTrim.GetLength();
		}
	}

	strSource = strSource.Right (strSource.GetLength() - iTrimPosition);

	return strSource;
}

CString TrimLeadingCharacters (LPCTSTR szSource, char cCharacter)
{
	CString strSource(szSource);

	int iPosition = 0;
	BOOL bDone = FALSE;

	while ((iPosition < strSource.GetLength()) && (!bDone))
	{
		if (strSource.GetAt(iPosition) == cCharacter)
		{
			iPosition++;
		}
		else
		{
			bDone = TRUE;
		}
	}
	
	if (iPosition < strSource.GetLength())
	{
		strSource = strSource.Right (strSource.GetLength() - iPosition);
	}
	else
	{
		strSource = "";
	}

	return strSource;
}

CString TrimLeadingSlashes (LPCTSTR szSource)
{
	CString strSource(szSource);

	int iPosition = 0;
	BOOL bDone = FALSE;

	while ((iPosition < strSource.GetLength()) && (!bDone))
	{
		if ((strSource.GetAt(iPosition) == '\\') || (strSource.GetAt(iPosition) == '/'))
		{
			iPosition++;
		}
		else
		{
			bDone = TRUE;
		}
	}
	
	if (iPosition < strSource.GetLength())
	{
		strSource = strSource.Right (strSource.GetLength() - iPosition);
	}
	else
	{
		strSource = "";
	}

	return strSource;
}

CString TrimSlashes (LPCTSTR szSource)
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

CString ConvertAllCharacters (LPCTSTR szSource, char cMatch, char cChange)
{
	if (cMatch == cChange)
	{
		return szSource;
	}

	CString strConvert (szSource);

	while (strConvert.Find (cMatch) != -1)
	{
		strConvert.SetAt (strConvert.Find (cMatch), cChange);
	}

	return strConvert;
}

int GetNumCharacters (LPCTSTR szString, char cMatch)
{
	int iCount = 0;

	for (UINT j = 0; j < strlen (szString) + 1; j++)
	{
		if (szString[j] == cMatch)
		{
			iCount++;
		}
	}

	return iCount;
}

void GetAllWildCards( LPSTR szWildCard, CStringArray *psaWildCards )
{
	int iStrLength = 0;
	int iStrPosition = 0;
	int j = 0;
	int k = 0;
	BOOL bMoreWildCards = TRUE;

	if( !psaWildCards )
	{
		return;
	}

	iStrLength = strlen( szWildCard );
	
	// Strip out any spaces at the front		
	while( (*szWildCard == ' ') && (iStrPosition < iStrLength) )
	{
		szWildCard++;
		iStrPosition++;
	}

	if( iStrPosition == iStrLength )
	{
		// No wildcards
		return;
	}

	// Change any other spaces to NULL
	for( j = iStrPosition, k = 0; j < iStrLength; j++, k++ )
	{
		if( szWildCard[k] == ' ' )
		{
			szWildCard[k] = '\0';
		}
	}
	
	while( bMoreWildCards )
	{
		psaWildCards->Add( szWildCard );
		szWildCard += ( strlen(szWildCard) + 1 );
		iStrPosition += ( strlen(szWildCard) + 1 );

		if( iStrPosition >= iStrLength )
		{
			bMoreWildCards = FALSE;
		}
	}
}

BOOL MatchesWildCard (char *szSource, CStringArray *psaWildCards)
{
	char *n;
	char *szUpperSource;

	CString strWildCard("");
	CString strSource("");

	strSource = szSource;
	strSource.MakeUpper();

	szUpperSource = strSource.GetBuffer( strSource.GetLength() );
	
	int j = 0;

	for (j = 0; j < psaWildCards->GetSize(); j++)
	{
		strWildCard = psaWildCards->GetAt(j);
		strWildCard.MakeUpper();
		
		n = strWildCard.GetBuffer( strWildCard.GetLength() );

		if( fnmatch( n, szUpperSource, FNM_PATHNAME | FNM_PERIOD) != FNM_NOMATCH )
		{
			return TRUE;
		}
	}

	return FALSE;
}

int fnmatch (char *pattern, char *string, int flags)
{
/*
	http://src.openresources.com/debian/src/base/HTML/S/bash_2.01.1.orig%20bash-2.01.1%20lib%20glob%20fnmatch.c.html
	http://math.unice.fr/laboratoire/help/info/glibc/libc_276.html

	Copyright (C) 1991 Free Software Foundation, Inc.
	This file is part of the GNU C Library.

	The GNU C Library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of the
	License, or (at your option) any later version.

	The GNU C Library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with the GNU C Library; see the file COPYING.LIB.  If
	not, write to the Free Software Foundation, Inc., 675 Mass Ave,
	Cambridge, MA 02139, USA.
	
*/

	char *p = pattern, *n = string;
	char c;

	if ((flags & ~__FNM_FLAGS) != 0)
	{		
		return (-1);
    }

	while ((c = *p++) != '\0')
    {
      switch (c)
        {
        case '?':
          if (*n == '\0')
            return (FNM_NOMATCH);
          else if ((flags & FNM_PATHNAME) && *n == '\\')
            /* If we are matching a pathname, `?' can never match a `/'. */
            return (FNM_NOMATCH);
          else if ((flags & FNM_PERIOD) && *n == '.' &&
                   (n == string || ((flags & FNM_PATHNAME) && n[-1] == '\\')))
            /* `?' cannot match a `.' if it is the first character of the
               string or if it is the first character following a slash and
               we are matching a pathname. */
            return (FNM_NOMATCH);
          break;

        case '\\':
          if (!(flags & FNM_NOESCAPE))
            {
              c = *p++;
              if (c == '\0')
                return (FNM_NOMATCH);
            }
          if (*n != c)
            return (FNM_NOMATCH);
          break;

        case '*':
          if ((flags & FNM_PERIOD) && *n == '.' &&
              (n == string || ((flags & FNM_PATHNAME) && n[-1] == '\\')))
            /* `*' cannot match a `.' if it is the first character of the
               string or if it is the first character following a slash and
               we are matching a pathname. */
            return (FNM_NOMATCH);

          /* Collapse multiple consecutive, `*' and `?', but make sure that
             one character of the string is consumed for each `?'. */
          for (c = *p++; c == '?' || c == '*'; c = *p++)
            {
              if ((flags & FNM_PATHNAME) && *n == '\\')
                /* A slash does not match a wildcard under FNM_PATHNAME. */
                return (FNM_NOMATCH);
              else if (c == '?')
                {
                  if (*n == '\0')
                    return (FNM_NOMATCH);
                  /* One character of the string is consumed in matching
                     this ? wildcard, so *??? won't match if there are
                     fewer than three characters. */
                  n++;
                }
            }

          if (c == '\0')
            return (0);

          /* General case, use recursion. */
          {
            char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
            for (--p; *n != '\0'; ++n)
              /* Only call fnmatch if the first character indicates a
                 possible match. */
              if ((c == '[' || *n == c1) &&
                  fnmatch (p, n, flags & ~FNM_PERIOD) == 0)
                return (0);
            return (FNM_NOMATCH);
          }

        case '[':
          {
            /* Nonzero if the sense of the character class is inverted.  */
            int iNot;

            if (*n == '\0')
              return (FNM_NOMATCH);

            /* A character class cannot match a `.' if it is the first
               character of the string or if it is the first character
               following a slash and we are matching a pathname. */
            if ((flags & FNM_PERIOD) && *n == '.' &&
                (n == string || ((flags & FNM_PATHNAME) && n[-1] == '\\')))
              return (FNM_NOMATCH);

            /* POSIX.2 2.8.3.1.2 says: `An expression containing a `[' that
               is not preceded by a backslash and is not part of a bracket
               expression produces undefined results.'  This implementation
               treats the `[' as just a character to be matched if there is
               not a closing `]'.  This code will have to be changed when
               POSIX.2 character classes are implemented. */
            {
              char *np;

              for (np = p; np && *np && *np != ']'; np++)
                ;

              if (np && !*np)
                {
                  if (*n != '[')
                    return (FNM_NOMATCH);
                  break;
                }
            }
              
			iNot = (*p == '!' || *p == '^');
            if (iNot)
              ++p;

            c = *p++;
            for (;;)
              {
                char cstart, cend;

                /* Initialize cstart and cend in case `-' is the last
                   character of the pattern. */
                cstart = cend = c;

                if (!(flags & FNM_NOESCAPE) && c == '\\')
                  {
                    if (*p == '\0')
                      return FNM_NOMATCH;
                    cstart = cend = *p++;
                  }

                if (c == '\0')
                  /* [ (unterminated) loses.  */
                  return (FNM_NOMATCH);

                c = *p++;

                if ((flags & FNM_PATHNAME) && c == '\\')
                  /* [/] can never match.  */
                  return (FNM_NOMATCH);

                /* This introduces a range, unless the `-' is the last
                   character of the class.  Find the end of the range
                   and move past it. */
                if (c == '-' && *p != ']')
                  {
                    cend = *p++;
                    if (!(flags & FNM_NOESCAPE) && cend == '\\')
                      cend = *p++;
                    if (cend == '\0')
                      return (FNM_NOMATCH);

                    c = *p++;
                  }

                if (*n >= cstart && *n <= cend)
                  goto matched;

                if (c == ']')
                  break;
              }
            if (!iNot)
              return (FNM_NOMATCH);
            break;

          matched:
            /* Skip the rest of the [...] that already matched.  */
            while (c != ']')
              {
                if (c == '\0')
                  /* [... (unterminated) loses.  */
                  return (FNM_NOMATCH);

                c = *p++;
                if (!(flags & FNM_NOESCAPE) && c == '\\')
                  {
                    if (*p == '\0')
                      return FNM_NOMATCH;
                    /* XXX 1003.2d11 is unclear if this is right. */
                    ++p;
                  }
              }
            if (iNot)
              return (FNM_NOMATCH);
          }
          break;

        default:
          if (c != *n)
            return (FNM_NOMATCH);
        }

      ++n;
    }

  if (*n == '\0')
    return (0);

  return (FNM_NOMATCH);
}

void FindAllFiles (LPCTSTR szPath, CStringArray *pStrArray, int iFlags)
{
	struct _finddata_t c_file;
	long hFile;
	CString strCompare("");
	
	// Trim off the trailing \ marks
	CString strFullPath (szPath);
	CString strPathWildCard("");

	if (strFullPath != "")
	{
		int iPos = strFullPath.GetLength() - 1;

		while (strFullPath.GetAt(iPos) == '\\')
		{
			iPos--;
		}

		iPos++;
		strFullPath = strFullPath.Left(iPos);
		strFullPath += "\\";
	}
	strPathWildCard = strFullPath + "*.*";

	if( (hFile = _findfirst( strPathWildCard, &c_file )) != -1L )	
	{
		strCompare = c_file.name;

		if ((strCompare != ".") && (strCompare != ".."))
		{
			if ((c_file.attrib & _A_SUBDIR) == 0)
			{
				if (iFlags & WANT_FULLPATH)
				{					
					pStrArray->Add (strFullPath + strCompare);
				}

				if (iFlags & WANT_FILENAME)
				{					
					pStrArray->Add (strCompare);
				}
			}
		}
	
		while( _findnext( hFile, &c_file ) == 0 )
		{
			strCompare = c_file.name;
			
			if ((strCompare != ".") && (strCompare != ".."))
			{
				if ((c_file.attrib & _A_SUBDIR) == 0)
				{						
					if (iFlags & WANT_FULLPATH)
					{				
						pStrArray->Add (strFullPath + strCompare);
					}

					if (iFlags & WANT_FILENAME)
					{					
						pStrArray->Add (strCompare);
					}
				}
			}
		}
	}
	
}


BOOL CalcImageColor24 (int iWidth, int iHeight, COLOR_IRGB *pirgbImageData, float *pfR, float *pfG, float *pfB, BOOL bNormalize /* = FALSE */, BOOL bIgnoreBlue /* = FALSE */)
{
	ASSERT (pirgbImageData);	
	ASSERT (pfR);
	ASSERT (pfG);
	ASSERT (pfB);

	int j = 0;
	int iSize = iWidth * iHeight;

	double dfRed = 0.0;
	double dfGreen = 0.0;
	double dfBlue = 0.0;

	COLOR_IRGB *p = NULL;
	int r = 0;
	int g = 0;
	int b = 0;

	for (j = 0; j < iSize; j++)
	{
		p = (COLOR_IRGB *)( ((BYTE *)pirgbImageData) + (j * sizeof(COLOR_IRGB)));
		
		r = GetRValue (*p);
		g = GetGValue (*p);
		b = GetBValue (*p);

		if( (r != 0) || (g != 0) || (b != 255) || !bIgnoreBlue )
		{		
			dfRed = ((dfRed * (j * 1.0)) + ((r * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfGreen = ((dfGreen * (j * 1.0)) + ((g * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfBlue = ((dfBlue * (j * 1.0)) + ((b * 1.0) / 255.0)) / (1.0 * (j + 1));
		}
	}

	if (bNormalize)
	{
		double dfMax = 0.0;
		dfMax = max (dfRed, dfGreen);
		dfMax = max (dfMax, dfBlue);

		dfRed = dfRed / dfMax;
		dfGreen = dfGreen / dfMax;
		dfBlue = dfBlue / dfMax;
	}

	(*pfR) = (float)dfRed;
	(*pfG) = (float)dfGreen;
	(*pfB) = (float)dfBlue;

	return TRUE;
}

BOOL CalcImageColor24( int iWidth, int iHeight, LPBYTE pbyImageData, float *pfR, float *pfG, float *pfB, BOOL bNormalize /* = FALSE */, BOOL bIgnoreBlue /* = FALSE */ )
{
	ASSERT (pbyImageData);	
	ASSERT (pfR);
	ASSERT (pfG);
	ASSERT (pfB);

	int j = 0;
	int iSize = iWidth * iHeight;

	double dfRed = 0.0;
	double dfGreen = 0.0;
	double dfBlue = 0.0;
	
	int r = 0;
	int g = 0;
	int b = 0;

	for( j = 0; j < iSize; j++ )
	{		
		r = pbyImageData[(j * 3) + 0];
		g = pbyImageData[(j * 3) + 1];
		b = pbyImageData[(j * 3) + 2];

		if( (r != 0) || (g != 0) || (b != 255) || !bIgnoreBlue )
		{		
			dfRed = ((dfRed * (j * 1.0)) + ((r * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfGreen = ((dfGreen * (j * 1.0)) + ((g * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfBlue = ((dfBlue * (j * 1.0)) + ((b * 1.0) / 255.0)) / (1.0 * (j + 1));
		}		
	}

	if (bNormalize)
	{
		double dfMax = 0.0;
		dfMax = max (dfRed, dfGreen);
		dfMax = max (dfMax, dfBlue);

		dfRed = dfRed / dfMax;
		dfGreen = dfGreen / dfMax;
		dfBlue = dfBlue / dfMax;
	}

	(*pfR) = (float)dfRed;
	(*pfG) = (float)dfGreen;
	(*pfB) = (float)dfBlue;

	return TRUE;
}


BOOL CalcImageColor256 (int iWidth, int iHeight, BYTE *pbyImageData, BYTE *pbyPalette, float *pfR, float *pfG, float *pfB, BOOL bNormalize, BOOL bIgnoreBlue /* = FALSE */)
{
	ASSERT (pbyImageData);
	ASSERT (pbyPalette);
	ASSERT (pfR);
	ASSERT (pfG);
	ASSERT (pfB);

	int j = 0;
	int iSize = iWidth * iHeight;

	double dfRed = 0.0;
	double dfGreen = 0.0;
	double dfBlue = 0.0;

	BYTE *p = NULL;
	int r = 0;
	int g = 0;
	int b = 0;	

	for (j = 0; j < iSize; j++)
	{
		p = pbyImageData + j;
		r = pbyPalette[(*p) * 3 + 0];
		g = pbyPalette[(*p) * 3 + 1];
		b = pbyPalette[(*p) * 3 + 2];

		if( (r != 0) || (g != 0) || (b != 255) || !bIgnoreBlue )
		{
			dfRed = ((dfRed * (j * 1.0)) + ((r * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfGreen = ((dfGreen * (j * 1.0)) + ((g * 1.0) / 255.0)) / (1.0 * (j + 1));
			dfBlue = ((dfBlue * (j * 1.0)) + ((b * 1.0) / 255.0)) / (1.0 * (j + 1));
		}
	}

	if (bNormalize)
	{
		double dfMax = 0.0;
		dfMax = max (dfRed, dfGreen);
		dfMax = max (dfMax, dfBlue);

		dfRed = dfRed / dfMax;
		dfGreen = dfGreen / dfMax;
		dfBlue = dfBlue / dfMax;
	}

	(*pfR) = (float)dfRed;
	(*pfG) = (float)dfGreen;
	(*pfB) = (float)dfBlue;

	return TRUE;
}

BOOL BlendPalette (BYTE *pbyPalette, int iIndexLeft, int iIndexRight)
{
	float rl = 0;
	float gl = 0;
	float bl = 0;
	float rh = 0;
	float gh = 0;
	float bh = 0;

	float rAdj = 0.0;
	float gAdj = 0.0;
	float bAdj = 0.0;

	float fSeparation = 0.0;
	int iSeparation = 0;

	float r = 0.0;
	float g = 0.0;
	float b = 0.0;	
		
	int iIndexLow	= 0;
	int iIndexHigh	= 0;	

	int j = 0;
	int k = 0;
	
	iSeparation = (abs(iIndexRight - iIndexLeft));
	fSeparation = (float)(iSeparation);

	iIndexLow	= min (iIndexRight, iIndexLeft);
	iIndexHigh	= max (iIndexRight, iIndexLeft);
	
	rl = (float)(pbyPalette[iIndexLow * 3]);
	gl = (float)(pbyPalette[iIndexLow * 3 + 1]);
	bl = (float)(pbyPalette[iIndexLow * 3 + 2]);
	
	rh = (float)(pbyPalette[iIndexHigh * 3]);
	gh = (float)(pbyPalette[iIndexHigh * 3 + 1]);
	bh = (float)(pbyPalette[iIndexHigh * 3 + 2]);

	// Determine the adjustment amounts
	rAdj = (float)((rh - rl) / fSeparation);

	gAdj = (float)((gh - gl) / fSeparation);	

	bAdj = (float)((bh - bl) / fSeparation);	

	r = rl;
	g = gl;
	b = bl;

	for (j = (iIndexLow + 1); j < iIndexHigh; j++)
	{
		r += rAdj;
		g += gAdj;
		b += bAdj;
		pbyPalette[j * 3]		= (int)r;
		pbyPalette[j * 3 + 1]	= (int)g;
		pbyPalette[j * 3 + 2]	= (int)b;
	}	
	
	return TRUE;
}


LPVOID MakeFile( LPCTSTR szFileName, DWORD dwFileSize)
{
	HANDLE hFile = NULL;
	HANDLE hFileMapping = NULL;
	LPVOID lpView = NULL;
	DWORD dwError = 0;

	hFile = CreateFile( 
		szFileName,							// Name of file
		GENERIC_READ | GENERIC_WRITE,		// Desired access
		0,									// Share mode
		NULL,								// Security attributes
		CREATE_ALWAYS,						// Creation disposition
		FILE_ATTRIBUTE_NORMAL,				// Attributes and flags
		NULL);								// Template file	

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// CreateFile() does not return NULL on error
		dwError = ::GetLastError();
		CWallyException WLYException;
		WLYException.SetLastErrorCode( dwError );
		
		throw WLYException;
	}

	hFileMapping = CreateFileMapping( 
		hFile,						// Handle to file
		NULL,						// Security attributes
		PAGE_READWRITE,				// Protection
		0,							// Max size high
		dwFileSize,					// Max size low
		NULL);						// Name of mapping object	
	
	if (hFileMapping == NULL)
	{
		dwError = ::GetLastError();
		CloseHandle( hFile);		
		CWallyException WLYException;
		WLYException.SetLastErrorCode( dwError );
		
		throw WLYException;
	}

	// We don't need this anymore
	CloseHandle( hFile);

	// Map to the entire file
	lpView = MapViewOfFile(
		hFileMapping,				// Handle to the mapping
		FILE_MAP_WRITE,				// Desired access
		0,							// Offset high
		0,							// Offset low
		dwFileSize);				// Number of bytes

	if (lpView == NULL)
	{
		dwError = ::GetLastError();
		CloseHandle( hFileMapping);
		CWallyException WLYException;
		WLYException.SetLastErrorCode( dwError );

		throw WLYException;
	}

	// We don't need this anymore
	CloseHandle( hFileMapping);

	return lpView;
}

void CloseFile( LPVOID lpFile )
{
	if( lpFile )
	{
		UnmapViewOfFile( lpFile );
	}
}