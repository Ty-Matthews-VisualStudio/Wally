// BrowseDIBList.cpp: implementation of the CBrowseDIBList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "BrowseDIBList.h"
#include "MiscFunctions.h"
#include "ImageHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Global variable to flag out of memory
bool	g_bOutOfMemory;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBrowseDIBItem::CBrowseDIBItem(CString *m_WalFileName)
{
	p_Next = NULL;
	p_Previous = NULL;	
	unsigned char *p_RawData;
	unsigned char *p_OriginalPointer;
	struct q2_miptex_s *m_WalHeader;	
	unsigned char *p_Mip0;
	int m_FileLength;
	int Pos;
	char m_ErrorMessage[100];
	int HdrSize  = sizeof( struct q2_miptex_s);
	int mipsize, mipsize4, mipsize16, mipsize64;

	m_Name = GetRawFileName(*m_WalFileName);

	FILE *fp;		
	fp = fopen((LPCTSTR)*m_WalFileName,"rb");
		
	Pos = ftell (fp);
	fseek (fp, 0, SEEK_END);
	m_FileLength = ftell (fp);
	fseek (fp, Pos, SEEK_SET);	
	
	if (HdrSize > m_FileLength)
	{
		m_Width = m_Height = 0;		
		return;
	}

	p_OriginalPointer = p_RawData = new unsigned char[m_FileLength];	
	
	if (fread (p_RawData, 1, m_FileLength, fp) != (size_t)m_FileLength)
	{
		sprintf (m_ErrorMessage,"Error reading %s",(LPCTSTR)m_WalFileName);
		AfxMessageBox (m_ErrorMessage,MB_ICONSTOP);
		fclose(fp);
		p_RawData = NULL;
		delete p_OriginalPointer;
		p_OriginalPointer = NULL;	
		return;
	}	
	fclose(fp);

	m_WalHeader = (struct q2_miptex_s *)p_RawData;
	m_Width = m_WalHeader->width;
	m_Height = m_WalHeader->height;	

	
	mipsize   = m_Width * m_Height;
	mipsize4  = mipsize / 4;
	mipsize16 = mipsize / 16;
	mipsize64 = mipsize / 64;

	div_t x_result, y_result;
   
	x_result = div(m_Width, 16);
	y_result = div(m_Height, 16);
	
	
	if ( ((mipsize + mipsize4 + mipsize16 + mipsize64 + HdrSize) > m_FileLength) 
		   ||
		 (((int)m_WalHeader->offsets[0] + mipsize) > m_FileLength)
		   ||
		 ((x_result.rem != 0) || (y_result.rem != 0))
	   )
	{
		sprintf (m_ErrorMessage,"%s is corrupt",(LPCTSTR)m_WalFileName);
		AfxMessageBox (m_ErrorMessage,MB_ICONSTOP);
		m_Width = m_Height = 0;
		p_RawData = NULL;
		delete p_OriginalPointer;
		p_OriginalPointer = NULL;
		return;
	}
	
	
	
	if (!Init (m_Width, m_Height, quake2_pal, 256))
	{
		g_bOutOfMemory = true;		
	}
	else
	{
		// Point to the largest Mip
		p_Mip0 = p_RawData + m_WalHeader->offsets[0];   

		// Bits is defined in the base class
		memcpy (Bits, p_Mip0, m_Width * m_Height);
	}

	p_RawData = NULL;
	delete p_OriginalPointer;
	p_OriginalPointer = NULL;
} 

CBrowseDIBItem::~CBrowseDIBItem()
{
	
}

void CBrowseDIBItem::ShowRestricted (CDC* ActiveWindow, int X, int Y, int Size)
{

	if ((m_Width == 0) || (m_Height == 0))
		return;

	CDC Context;
	Context.CreateCompatibleDC (ActiveWindow);
	HBITMAP p_OldBM = (HBITMAP)Context.SelectObject (BMHan);	 	

	if (m_Width == m_Height)
	{
		// It's square, no need to find the ratio
		StretchBlt (ActiveWindow->m_hDC, X, Y, Size, Size, Context.m_hDC, 0, 0, m_OriginalWidth, m_OriginalHeight, SRCCOPY);
	}
	else
	{
		float YSquish = (float)0.0;
		float XSquish = (float)0.0;
		float fWidth = (float)m_OriginalWidth;
		float fHeight = (float)m_OriginalHeight;
		float fSize = (float)Size;
		int YRatio, XRatio;		

		// Wider than taller?
		if (m_OriginalWidth > m_OriginalHeight)
		{			
			XSquish = (fSize / fWidth);				
			XRatio = (int) (XSquish * fWidth);			
			YRatio = (int) (XSquish * fHeight);			
		}
		else
		{	
			YSquish = (fSize / fHeight);							
			YRatio = (int) (YSquish * fHeight);			
			XRatio = (int) (YSquish * fWidth);			
		}		

		StretchBlt (ActiveWindow->m_hDC, X, Y, XRatio, YRatio , Context.m_hDC, 0, 0, m_OriginalWidth, m_OriginalHeight, SRCCOPY);
	}
				
	Context.SelectObject (p_OldBM);
	Context.DeleteDC();
}



CBrowseDIBList::CBrowseDIBList()
{
	p_First = p_Read = p_Write = NULL;	
}

CBrowseDIBList::~CBrowseDIBList()
{

}

CBrowseDIBItem* CBrowseDIBList::AddItem(CString *FileName)
{
	CBrowseDIBItem* p_Temp1;
	CBrowseDIBItem* p_Temp2;
	CBrowseDIBItem* p_NewItem;
	
	CString Compare1;
	CString Compare2 (GetRawFileName(*FileName));

	if (p_First == NULL)
	{
		p_First = p_Read = p_Write = new CBrowseDIBItem(FileName);
		if ((p_First == NULL) || (p_First->m_Width == 0))
			return NULL;
		else
			return p_First;
	}
	else
	{
		p_NewItem = new CBrowseDIBItem(FileName);
		if ((p_NewItem == NULL) || (p_NewItem->m_Width == 0))
			return NULL;

		p_Temp1 = NULL;
        p_Temp2 = NULL;
        
		p_Read = p_First;		
		
		// Insert the item into the sorted list 
        while (p_Read != NULL)
        {
			Compare1 = p_Read->m_Name;
			Compare1.MakeLower();
			Compare2.MakeLower();

	        if (Compare1 > Compare2)
			{                           
				p_Temp1 = p_Read->p_Previous;
                p_Temp2 = p_Read;
                p_Read = NULL;
            }
            else
				p_Read = p_Read->p_Next;
        }
        
        //  Item belongs at the end 
        if ((p_Temp2 == NULL) && (p_Temp1 == NULL))
        {                   
            p_Write->p_Next = p_NewItem;
            p_NewItem->p_Previous = p_Write;                      
            p_Write = p_NewItem;            
        }

        //  Item is the first in the list 
        if ((p_Temp1 == NULL) && (p_Temp2 != NULL))
		{		    
			p_NewItem->p_Next = p_First;			
			p_First->p_Previous = p_NewItem;                      			
			p_First = p_NewItem;                               
		}

		//  Item is somewhere in the middle of the list 
		if ((p_Temp1 != NULL) && (p_Temp2 != NULL))
		{			
			p_Temp1->p_Next = p_NewItem;
			p_NewItem->p_Previous = p_Temp1;
			p_NewItem->p_Next = p_Temp2;
			p_Temp2->p_Previous = p_NewItem;            
        }
		
		return p_NewItem;
	
	}
}

int CBrowseDIBList::GetLength()
{	
	CBrowseDIBItem* p_Temp;
	p_Temp = p_First;
	int count = 0;	

	while (p_Temp != NULL)
	{
		count++;
		p_Temp = p_Temp->p_Next;
	}
	
	return count;
}

CBrowseDIBItem* CBrowseDIBList::GetFirst()
{
	if (IsEmpty())
		return NULL;
	else
	{
		p_Read = p_First;
		return p_First;
	}
}

CBrowseDIBItem* CBrowseDIBList::GetNext()
{
	p_Read = p_Read->p_Next;
	if (p_Read == NULL)
		return NULL;
	else
		return p_Read;
}

CBrowseDIBItem* CBrowseDIBList::GetAtPosition(int Position)
{
	CBrowseDIBItem* p_Temp;
	p_Temp = p_First;
	int Marker = 0;
	
	if (IsEmpty() || GetLength() < Position)
		return NULL;
	else
	{
		while ((p_Temp != NULL) && (Marker != Position))
		{
			Marker++;
			p_Temp = p_Temp->p_Next;
		}
	}
	
	return p_Temp;

}

void CBrowseDIBList::PurgeList()
{
	CBrowseDIBItem* p_Temp;
	if (IsEmpty())
		return;

	p_Read = p_First;
	p_First = NULL;
		
	while (p_Read != NULL)
	{
		p_Temp = p_Read->p_Next;
		delete p_Read;
		p_Read = p_Temp;
	}
	p_First = p_Read = p_Write = NULL;
	
}

