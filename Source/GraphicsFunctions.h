#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// GraphicsFunctions.h

#define	GF_TYPE_QUAKE1	0
#define	GF_TYPE_QUAKE2	1

void LoadQ2PaletteFromDisk ();
void LoadQ1PaletteFromDisk ();
void LoadPalette (int iType);
