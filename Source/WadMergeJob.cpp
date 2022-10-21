// WadMergeJob.cpp: implementation of the CWadMergeJob class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "WadMergeJob.h"
#include "WadMergeDlg.h"
#include "ImageHelper.h"
#include "MiscFunctions.h"
#include "WadMergeStatusDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWadMergeJob::CWadMergeJob()
{
	SetMaxWidth( 0 );
	SetMaxHeight( 0 );
	SetMinWidth( 0 );
	SetMinHeight( 0 );
}

CWadMergeJob::~CWadMergeJob()
{

}

LPSTR CWadMergeJob::GetWildcard()
{
	return m_strWildcard.GetBuffer( m_strWildcard.GetLength() );
}

void CWadMergeJob::SetWildcard( LPCTSTR szWildcard )
{
	m_strWildcard = szWildcard;
}

LPSTR CWadMergeJob::GetShortWadFile()
{
	return m_strShortWadFile.GetBuffer( m_strShortWadFile.GetLength() );
}

LPSTR CWadMergeJob::GetWadFile()
{
	return m_strWadFile.GetBuffer( m_strWadFile.GetLength() );
}

void CWadMergeJob::SetWadFile( LPCTSTR szWadFile )
{
	m_strWadFile = szWadFile;
	m_strShortWadFile = GetRawFileName( szWadFile );
}

DWORD CWadMergeJob::GetMaxHeight()
{
	return m_dwMaxHeight;
}

void CWadMergeJob::SetMaxHeight( DWORD dwMaxHeight )
{
	m_dwMaxHeight = dwMaxHeight;
}

DWORD CWadMergeJob::GetMaxWidth()
{
	return m_dwMaxWidth;
}

void CWadMergeJob::SetMaxWidth( DWORD dwMaxWidth )
{
	m_dwMaxWidth = dwMaxWidth;
}

DWORD CWadMergeJob::GetMinHeight()
{
	return m_dwMinHeight;
}

void CWadMergeJob::SetMinHeight( DWORD dwMinHeight )
{
	m_dwMinHeight = dwMinHeight;
}

DWORD CWadMergeJob::GetMinWidth()
{
	return m_dwMinWidth;
}

void CWadMergeJob::SetMinWidth( DWORD dwMinWidth )
{
	m_dwMinWidth = dwMinWidth;
}

BOOL CWadMergeJob::HasMinMaxRestrictions()
{
	return (
		(GetMinWidth() != 0 ) ||
		(GetMaxWidth() != 0 ) ||
		(GetMinHeight() != 0 ) ||
		(GetMaxHeight() != 0 )
	);
}

BOOL CWadMergeJob::MeetsMinMaxRestrictions( DWORD dwWidth, DWORD dwHeight )
{
	BOOL bMeets = TRUE;

	if( GetMinWidth() )
	{
		if( GetMinWidth() > dwWidth )
		{
			bMeets = FALSE;
		}
	}

	if( GetMaxWidth() )
	{
		if( GetMaxWidth() < dwWidth )
		{
			bMeets = FALSE;
		}
	}

	if( GetMinHeight() )
	{
		if( GetMinHeight() > dwHeight )
		{
			bMeets = FALSE;
		}
	}

	if( GetMaxHeight() )
	{
		if( GetMaxHeight() < dwHeight )
		{
			bMeets = FALSE;
		}
	}
	
	return bMeets;
}

BOOL CWadMergeJob::LoadWAD()
{
	if( m_WADList.Serialize( this ) == IH_SUCCESS )
	{
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//  Name:	MergeWAD
//	Action:	Merges the WAD contained within this Job with a passed-in CWADList object
//
//	Parameters:
//			pWADList		Pointer to the CWADList object
//			dwFlags			Flags that alter how the images should be merged
//////////////////////////////////////////////////////////////////////////////////////////
BOOL CWadMergeJob::MergeWAD( CWADList *pWADList, DWORD dwFlags, CWadMergeDlg::LPTHREAD_PARAM lpParam )
{
	if( !pWADList || !lpParam )
	{
		return FALSE;
	}

	DWORD dwNumImages = m_WADList.GetNumImages();
	CWADItem *pItem = m_WADList.GetFirstImage();
	CWADItem *pDuplicate = NULL;

	CString strName("");
	CString strNewName("");
	DWORD dwCounter = 0;
	BOOL bDelete = FALSE;
	BOOL bDeleteDuplicate = FALSE;

	CWadMergeStatusDlg::LPMERGE_STATUS lpStatus = NULL;

	lpStatus = new CWadMergeStatusDlg::MERGE_STATUS;
	lpStatus->strImage = GetShortWadFile();
	lpStatus->dwNumImages = dwNumImages;
	::PostMessage( lpParam->pMergeStatus->m_hWnd, WM_WAD_MERGE_STATUS_CUSTOM, WAD_MERGE_STATUS_IMAGE_COUNT, (UINT)lpStatus );

	while( pItem )
	{
		// Yank it out of the list first
		m_WADList.RemoveImage( pItem, FALSE );

		// Yes, this is correct... we allocate them, but they don't get destroyed here.  They get destroyed
		// in CWadMergeStatusDlg::OnWadMergeStatusDlgCustomMessage()
		lpStatus = new CWadMergeStatusDlg::MERGE_STATUS;

		lpStatus->strImage = pItem->GetName();
		lpStatus->strStatus = "Merged OK";

		bDelete = FALSE;

		// Make sure it's a type we want to bring over
		if( (pItem->GetType() != WAD2_TYPE_MIP) && (pItem->GetType() != WAD3_TYPE_MIP) )
		{
			lpStatus->strStatus = "Unsupported type";
			bDelete = TRUE;
		}
		else
		{		
			strName = pItem->GetName();

			if( dwFlags & WAD_MERGE_FIRST_WAD )
			{
				// We didn't have any images in the WAD when we started, so no sense looking for duplicates
				pDuplicate = NULL;
			}
			else
			{
				pDuplicate = pWADList->IsNameInList( strName );
			}			
			
			if( pDuplicate )
			{
				if( dwFlags & WAD_MERGE_DUP_IGNORE )
				{
					lpStatus->strStatus = "Duplicate ignored";
					bDelete = TRUE;
				}
				else
				{
					if( dwFlags & WAD_MERGE_DUP_RENAME )
					{
						dwCounter = 1;
						strNewName.Format( "%s~%02x", strName.Left(12), dwCounter );
						while( (pWADList->IsNameInList( strNewName )) && (dwCounter < 256) )
						{
							strNewName.Format( "%s~%02x", strName.Left(12), dwCounter++ );
						}

						if( dwCounter < 256 )
						{
							pItem->SetName( strNewName );
							lpStatus->strStatus = "Merged OK, renamed to ";
							lpStatus->strStatus += strNewName;
						}
						else
						{							
							// Couldn't add it; we need to do the clean-up
							bDelete = TRUE;
							lpStatus->strStatus = "Failed to rename";							
						}					
					}
					else
					{
						if( dwFlags & WAD_MERGE_DUP_OVERWRITE )
						{
							// Make sure the item we're replacing isn't open for editing
							if( !pDuplicate->HasWallyDoc() )
							{
								pWADList->RemoveImage( pDuplicate, FALSE );
								bDeleteDuplicate = TRUE;
								lpStatus->strStatus = "Merged OK, overwrote original";
							}
							else
							{
								// Can't overwrite it when it's open for editing
								bDelete = TRUE;
								lpStatus->strStatus = "Duplicate is open";
							}
						}
					}
				}
			}			
		}

		if( bDelete )
		{
			delete pItem;
			pItem = NULL;
		}
		else
		{
			// We were added.  Let's check to see if the destination WAD is the same type
			// as the source
			if( m_WADList.GetWADType() != pWADList->GetWADType() )
			{
				// Whoops, better convert it
				if( pItem->ConvertToType( pWADList->GetWADType() ) )
				{
					pWADList->AddToList( pItem );
				}
				else
				{
					// Couldn't convert it... that's bad news
					delete pItem;
					pItem = NULL;
					lpStatus->strStatus = "Failed to convert";

					if( bDeleteDuplicate )
					{
						// Better put this one back
						pWADList->AddToList( pDuplicate );
						bDeleteDuplicate = FALSE;
					}
				}
			}
			else
			{
				// Just go add it, no conversion necessary
				pWADList->AddToList( pItem );
			}
		}

		if( bDeleteDuplicate )
		{
			delete pDuplicate;
			pDuplicate = NULL;
		}

		::PostMessage( lpParam->pMergeStatus->m_hWnd, WM_WAD_MERGE_STATUS_CUSTOM, WAD_MERGE_STATUS_STEP, (UINT)lpStatus );		
		pItem = m_WADList.GetFirstImage();		
	}

	return TRUE;
}

DWORD CWadMergeJob::GetNumImages()
{
	return m_WADList.GetNumImages( this );
}