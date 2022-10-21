////////////////////////////////////////////////////////////////////////////////////
//  PrefabImages.cpp
//  (c) 1998 Ty Matthews, all rights reserved
////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "PrefabImages.h"
#include "CustomResource.h"

CPrefabImage::CPrefabImage()
{
	p_DecodedData = NULL;
	p_EncodedData = NULL;	
	m_iWidth = 0;
	m_iHeight = 0;
	m_iRawDataLength = 0;
}

CPrefabImage::~CPrefabImage()
{
	p_EncodedData = NULL;
	
	if (p_DecodedData)
	{
		delete [] p_DecodedData;	// neal - must use array delete to delete arrays!
		p_DecodedData = NULL;		
	}
}
	
void CPrefabImage::CreatePrefabImage (int iType, unsigned char **p_ucData, unsigned char **p_ucPalette, DWORD dwSysColor /* = COLOR_BTNFACE */)
{
	int iPcxItem  = 0;

	p_EncodedData = NULL;
	
	if (p_DecodedData)
	{
		delete [] p_DecodedData;	// neal - must use array delete to delete arrays!
		p_DecodedData = NULL;
	}

	switch (iType)
	{
	case PI_MOUSE:
//		p_EncodedData = PrefabMouse;
//		SetRawDataLength(PREFAB_MOUSE_SIZE);

		iPcxItem = IDR_PCX_MOUSE;
		break;

	case PI_MOUSE_LEFT:
//		p_EncodedData = PrefabMouseL;
//		SetRawDataLength(PREFAB_MOUSEL_SIZE);

		iPcxItem = IDR_PCX_MOUSE_L;
		break;

	case PI_MOUSE_RIGHT:
//		p_EncodedData = PrefabMouseR;
//		SetRawDataLength(PREFAB_MOUSER_SIZE);

		iPcxItem = IDR_PCX_MOUSE_R;
		break;
	
	case PI_MOUSE_DISABLED:
//		p_EncodedData = PrefabMouseX;
//		SetRawDataLength(PREFAB_MOUSEX_SIZE);

		iPcxItem = IDR_PCX_MOUSE_X;
		break;

	case PI_MOUSE_RBUTTON_DISABLED:
//		p_EncodedData = PrefabMouseRX;
//		SetRawDataLength(PREFAB_MOUSERX_SIZE);

		iPcxItem = IDR_PCX_MOUSE_RX;
		break;

	case PI_MOUSE_LEFT_RBUTTON_DISABLED:
//		p_EncodedData = PrefabMouseLRX;
//		SetRawDataLength(PREFAB_MOUSELRX_SIZE);

		iPcxItem = IDR_PCX_MOUSE_LRX;
		break;

	case PI_BUILDVIEW_BUTTON:
		iPcxItem = IDR_PCX_BUILDVIEW_BUTTON;
		break;

	case PI_WAD_THUMBNAIL:
		iPcxItem = IDR_PCX_WAD_THUMB;
		break;

	case PI_WALLY_LOGO:
		iPcxItem = IDR_PCX_WALLY_LOGO;
		break;

	default:
		ASSERT(false);	// You're sending an invalid type, man!
		iPcxItem = IDR_PCX_MOUSE_X;
		break;
	}

	CCustomResource crMouse;
	
	crMouse.UseResourceId ("PCX", iPcxItem);
	p_EncodedData = crMouse.GetData();

	if (p_EncodedData)
	{
		SetRawDataLength( crMouse.GetDataSize());

		DecodePCX( dwSysColor );

		*p_ucData    = GetBits();
		*p_ucPalette = GetPalette();
	}
}

void CPrefabImage::CreatePrefabImageFromID (int iResourceID, BYTE **pbyData, BYTE **pbyPalette, DWORD dwSysColor /* = COLOR_BTNFACE */)
{
	CCustomResource crData;
	
	crData.UseResourceId ("PCX", iResourceID);
	p_EncodedData = crData.GetData();

	SetRawDataLength( crData.GetDataSize());

	DecodePCX( dwSysColor );
	(*pbyData)		= GetBits();
	(*pbyPalette)	= GetPalette();
}

bool CPrefabImage::DecodePCX( DWORD dwSysColor )
{
	if ((!p_EncodedData))
	{
		return false;
	}
	pcx_t *pcx						= NULL;
	unsigned char *p_RawData		= NULL;	
	unsigned char *p_TempPalette	= NULL;
	unsigned char *p_Pixel			= NULL;
		
	int iSize		= 0;
	int dataByte	= 0;
	int y, x;
	int j = 0;
	int iRunLength	= 0;		
	int iLength = GetRawDataLength();

	pcx = (pcx_t *)p_EncodedData;
	p_RawData = &pcx->data;
	
	int iWidth = (pcx->xmax) + 1;		
	int iHeight = (pcx->ymax) + 1;
	
	SetImageWidth (iWidth);
	SetImageHeight (iHeight);
	
	memcpy( m_byPalette, p_EncodedData + iLength - 768, 768);
	iSize = iWidth * iHeight;
		
	p_DecodedData = new BYTE[iSize];
	if (!p_DecodedData)
	{			
		return false;
	}
	memset (p_DecodedData, 0, iSize);

	p_Pixel = p_DecodedData;
	
	// Decode the PCX data
		
	for (y = 0; y < iHeight; y++, p_Pixel += iWidth)
	{
		for (x = 0; x < iWidth; )
		{
			dataByte = *p_RawData++;

			if((dataByte & 0xC0) == 0xC0)
			{
				iRunLength = dataByte & 0x3F;
				dataByte = *p_RawData++;
			}
			else
				iRunLength = 1;
	
			while(iRunLength-- > 0)
				p_Pixel[x++] = dataByte;
		}

	}

	DWORD dwRGB = GetSysColor( dwSysColor );
	int r = GetRValue( dwRGB);
	int g = GetGValue( dwRGB);
	int b = GetBValue( dwRGB);

	for (j = 0; j < 256; j++)
	{
		// Change RGB (255, 0, 255) to COLOR_BTNFACE.  This bright purple 
		// color is reserved for "transparent"
		if ((m_byPalette[j * 3 + 0] == 255) && (m_byPalette[j * 3 + 1] == 0) && (m_byPalette[j * 3 + 2] == 255))
		{
			// Neal - BoundsChecker thinks there is a problem here (AccessViolation)
			// but I think it is mistaken...

			// BoundsChecker was right!  This stupid palette was pointing at temporary RAM.  Meaning, we had a lock on 
			// the custom resource and it was held open until it got reused or dumped.  Luckily, the resource wasn't
			// dumped right away, and the pieces that used the palette were able to get their work done.  HUGE stupid
			// error on my part.  So now I'm building a buffer in CCustomResource, and stowing away the resource object
			// into that buffer.  Then, the lock on the resource itself can go away whenever, and we've still got the data.
			// In accordance with that, this pointer for the palette had to be changed to a member buffer, as well, since
			// we can't trust the buffer to always be there for us.  Just like the decoded data.

			m_byPalette[j * 3 + 0] = r;
			m_byPalette[j * 3 + 1] = g;
			m_byPalette[j * 3 + 2] = b;
		}
	}

	
/*

  // TODO: add support for non-WORD aligned widths?

  // TODO: add support for 24-bit images?
		
		  
		case PCX_TYPE_24BIT :
		
		unsigned char *ScanLine[3];		
		ScanLine[RED]   = new unsigned char[iWidth];
		ScanLine[GREEN] = new unsigned char[iWidth];
		ScanLine[BLUE]  = new unsigned char[iWidth];

		for (y = 0; y < iHeight; y++, p_Pixel += (iWidth * 3))
		{
			for (j = 0; j < 3; j++)  // Read in the three scan lines
			{
				for (x = 0; x < iWidth; )
				{
					dataByte = *p_RawData++;

					if((dataByte & 0xC0) == 0xC0)
					{
						iRunLength = dataByte & 0x3F;
						dataByte = *p_RawData++;
					}
					else
						iRunLength = 1;
	
					while(iRunLength-- > 0)
						ScanLine[j][x++] = dataByte;
				}				
			}

			for (x = 0; x < iWidth; x++)
			{
				p_Pixel[x * 3]	   = ScanLine[RED][x];
				p_Pixel[x * 3 + 1] = ScanLine[GREEN][x];
				p_Pixel[x * 3 + 2] = ScanLine[BLUE][x];
			}

		}

		for (j = 0; j < 3; j++)
			delete ScanLine[j];
		break;

	default :
		ASSERT (false);		// Unsupported PCX version... missed implementation?
		break;
	}  */

	p_Pixel = NULL;
	p_RawData = NULL;
	pcx = NULL;
	return true;
}

unsigned char *CPrefabImage::GetBits()
{
	return p_DecodedData;
}

unsigned char *CPrefabImage::GetPalette ()
{
	return m_byPalette;
}

void CPrefabImage::SetImageWidth (int iWidth)
{
	m_iWidth = iWidth;
}

int CPrefabImage::GetImageWidth ()
{
	return m_iWidth;
}

void CPrefabImage::SetImageHeight (int iHeight)
{
	m_iHeight = iHeight;
}

int CPrefabImage::GetImageHeight ()
{
	return m_iHeight;
}

int CPrefabImage::GetRawDataLength ()
{
	return m_iRawDataLength;
}

void CPrefabImage::SetRawDataLength (int iSize)
{
	m_iRawDataLength = iSize;
}