/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// GraphicsFunctions.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraphicsFunctions.h"
#include "Wally.h"
#include "ImageHelper.h"
#include "CustomResource.h"

#define	GF_TYPE_QUAKE1	0
#define	GF_TYPE_QUAKE2	1

void LoadQ2PaletteFromDisk ()
{	
	LoadPalette (GF_TYPE_QUAKE2);
}

void LoadQ1PaletteFromDisk ()
{
	LoadPalette (GF_TYPE_QUAKE1);
}

void LoadPalette (int iType)
{
	CImageHelper ihHelper;
	unsigned char *pbyPalette = NULL;

	CCustomResource crPalette;

	switch (iType)
	{
	case GF_TYPE_QUAKE1:	
		if (g_iUseDefaultQ1Palette)
		{
			crPalette.UseResourceId ("PALETTE", IDR_LMP_QUAKE1);
			memcpy (quake1_pal, crPalette.GetData(), 768);
		}
		else
		{
			ihHelper.LoadImage (g_strQuake1Palette, IH_LOAD_ONLYPALETTE);

			if (ihHelper.GetErrorCode() != IH_SUCCESS)
			{
				AfxMessageBox (ihHelper.GetErrorText());			
			}
			else
			{		
				pbyPalette = ihHelper.GetBits();
				memcpy (quake1_pal, pbyPalette, 768);
			}
		}
		break;

	case GF_TYPE_QUAKE2:
		if (g_iUseDefaultQ2Palette)
		{
			crPalette.UseResourceId ("PALETTE", IDR_LMP_QUAKE2);
			memcpy (quake2_pal, crPalette.GetData(), 768);
		}
		else
		{
			ihHelper.LoadImage (g_strQuake2Palette, IH_LOAD_ONLYPALETTE);

			if (ihHelper.GetErrorCode() != IH_SUCCESS)
			{
				AfxMessageBox (ihHelper.GetErrorText());			
			}
			else
			{		
				pbyPalette = ihHelper.GetBits();
				memcpy (quake2_pal, pbyPalette, 768);
			}
		}
	}
}

