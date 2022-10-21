/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// DIBToDocHelper.cpp: implementation file
//
// Created by Ty Matthews, 6-23-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DIBToDocHelper.h"
#include "Wally.h"
#include "MiscFunctions.h"
#include "WallyDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CWallyApp theApp;

void CDIBToDocItem::OpenDocument(CString FileName)
{
	CWallyDoc *p_NewWallyDoc = (CWallyDoc *)theApp.WallyDocTemplate->OpenDocumentFile(FileName);
	CWallyDocPTR *p_Temp = new CWallyDocPTR(p_NewWallyDoc);
	p_NewWallyDoc->p_DIBHelper = this;

	if (!p_FirstWallyDoc)
	{
		p_FirstWallyDoc = p_ReadWallyDoc = p_WriteWallyDoc = p_Temp;
	}
	else
	{
		p_WriteWallyDoc->p_Next = p_Temp;
		p_Temp->p_Previous = p_WriteWallyDoc;
		p_WriteWallyDoc = p_Temp;
	}

	p_NewWallyDoc = NULL;
	p_Temp = NULL;
}

void CDIBToDocItem::CloseDocument(CWallyDoc *p_Doc)
{
	CWallyDocPTR *p_TempWallyDoc = p_FirstWallyDoc;

	while (p_TempWallyDoc)
	{
		if (p_TempWallyDoc->p_WallyDoc == p_Doc)
		{
			if (p_TempWallyDoc->p_Previous)
			{
				p_TempWallyDoc->p_Previous->p_Next = p_TempWallyDoc->p_Next;
			}

			if (p_TempWallyDoc->p_Next)
			{
				p_TempWallyDoc->p_Next->p_Previous = p_TempWallyDoc->p_Previous;
			}

			if (p_TempWallyDoc == p_FirstWallyDoc)
			{
				p_FirstWallyDoc = p_TempWallyDoc->p_Next;
			}

			if (p_TempWallyDoc == p_WriteWallyDoc)
			{
				p_WriteWallyDoc = p_TempWallyDoc->p_Previous;
			}

			delete p_TempWallyDoc;
			p_TempWallyDoc = NULL;
		}
		else
		{
			p_TempWallyDoc = p_TempWallyDoc->p_Next;
		}
	}

}


void CDIBToDocItem::FlushList()
{
	if (IsEmpty())
	{
		return;
	}	
	
	CWallyDocPTR *p_TempWallyDoc = NULL;
	p_TempWallyDoc = p_FirstWallyDoc;

	while (p_TempWallyDoc)
	{
		// Break the connection between the doc and us
		p_TempWallyDoc->p_WallyDoc->p_DIBHelper = NULL;
		p_FirstWallyDoc = p_FirstWallyDoc->p_Next;
		delete p_TempWallyDoc;
		p_TempWallyDoc = p_FirstWallyDoc;
	}

	p_TempWallyDoc = NULL;
	p_ReadWallyDoc = p_WriteWallyDoc = p_FirstWallyDoc = NULL;
}

void CDIBToDocItem::UpdateDIB(unsigned char *p_Data, int iSize)
{	
	unsigned char *p_Bits;

	p_Bits = (unsigned char *)(*p_BrowseDIB);
	memcpy (p_Bits, p_Data, iSize);	
	p_BrowseDoc->RefreshView();
}

void CDIBToDocHelper::FlushList()
{
	if (IsEmpty())
	{
		return;
	}	
	
	CDIBToDocItem *p_Temp = NULL;
	p_Temp = p_First;

	while (p_Temp)
	{		
		p_First = p_First->p_Next;
		delete p_Temp;
		p_Temp = p_First;
	}

	p_Temp = NULL;
	p_Read = p_Write = p_First = NULL;
}

void CDIBToDocHelper::AddDIBItem(CBrowseDocument *p_Doc, CBrowseDIBItem *p_DIB)
{
	CDIBToDocItem *p_NewGuy = NULL;

	p_NewGuy = new CDIBToDocItem (p_Doc, p_DIB);
	if (!p_First)
	{
		p_First = p_Read = p_Write = p_NewGuy;
	}
	else
	{
		p_Write->p_Next = p_NewGuy;
		p_NewGuy->p_Previous = p_Write;
		p_Write = p_NewGuy;
	}
	p_NewGuy = NULL;
}

void CDIBToDocHelper::AddWallyItem(CString FileName, CBrowseDIBItem *p_DIB)
{
	if (IsEmpty())
	{
		ASSERT (false);		// List is empty, this is not good
		return;
	}
	p_Read = p_First;

	while (p_Read)
	{
		if (p_Read->p_BrowseDIB == p_DIB)
		{
			p_Read->OpenDocument (FileName);
			p_Read = NULL;
		}
		else
		{
			p_Read = p_Read->p_Next;
		}
	}

}


