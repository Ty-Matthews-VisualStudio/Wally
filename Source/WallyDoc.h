/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
// WallyDoc.h : interface of the CWallyDoc class
//
// Created by Ty Matthews, 1-1-1998
/////////////////////////////////////////////////////////////////////////////

#ifndef _WALLYDOC_H_
#define _WALLYDOC_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TextureFlags.h"

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

#ifndef _WALLYVIEW_H_
	#include "WallyView.h"
#endif

#ifndef _WALLYUNDO_H_
	#include "WallyUndo.h"
#endif

#ifndef _WALLYPAL_H_
	#include "WallyPal.h"
#endif

#ifndef _PACKAGEDOC_H_
	#include "PackageDoc.h"
#endif

#ifndef _WALLY_H_
	#include "Wally.h"
#endif

#ifndef _MAINFRM_H_
	#include "MainFrm.h"
#endif

#ifndef _SELECTION_H_
	#include "Selection.h"
#endif

#define FILTER_VERY_LIGHT	0
#define FILTER_LIGHT		1
#define FILTER_MEDIUM		2
#define FILTER_HEAVY		3
#define FILTER_VERY_HEAVY	4

#define NOISE_UNIFORM		0
#define NOISE_GAUSSIAN		1

class CImageHelper;

class CWallyDoc : public CDocument
{
protected: // create from serialization only
	CWallyDoc();
	DECLARE_DYNCREATE(CWallyDoc)

// Members
public:	
	
	// For Quake2
	Q2_MIP_S Q2Header;
	struct mipdata
	{
		unsigned char* mip_pointer;		
		int width;
		int height;
	} MipArray[4];

	// For Quake1
	Q1_MIP_S Q1Header;

	// For SiN
	SIN_MIP_S SinHeader;

	// For Heretic2
	M8_MIPTEX_S M8Header;

	int            m_iGameType;
	CWallyUndo     m_Undo;
	CWallyUndo     m_DrawingModeUndo;	// used to "erase" temporary lines

//	struct mipdata m_EffectsMipData;
	CLayer*        m_pEffectLayer;

	UINT		GetNumMips();
	BOOL        HasSelection() { return (m_pSelection != NULL); };
	CSelection* GetSelection()
		{
			if (m_pSelection)
				return (m_pSelection);
			else
				return (&m_Base);
		};
	CSelection* GetBase()
		{
			return (&m_Base);
		};
	CSelection* GetCutout()
		{
			return (&m_Cutout);
		};

	CLayer* GetCurrentLayer( void);
	void    MergeSelectionIntoBase( LPCTSTR szUndoMsg);

	CSelection* m_pSelection;
	CSelection  m_Base;
	CSelection  m_Cutout;
	int         m_LastGameType;

	CPoint      m_ptCloneStart;
	CPoint      m_ptCloneOffset;
	bool        m_bCloneReset;
	bool        m_bCloneInitialized;
	bool        m_bFreeEffectsData;

protected:
	CWallyPalette  m_Palette;
	CPackageDoc*   m_pPackageDoc;		// For Half-Life textures, this points
										// to the currently open WAD.  MUST be closed out
										// if the WAD view gets closed.
	
	CString m_strPackageFile;			// Original location of the .WAD file
	CString m_strPackageMipName;		// Name of the image in the WAD itself

	int     m_iOriginalBitDepth;		// 8, 24, etc
//	int     m_iWidth;
//	int     m_iHeight;

// Operations
public:	
//	void RebuildMipsArray( bool bFillWithBackground);
//	void InitMipArray();
	// Neal - renamed them because of functionality change
	void RebuildMipArray( void);
	void RebuildLayerAndMips( bool bFillWithBackground);
	void ClearMipArray( COLOR_IRGB irgbColor);
	void ClearMipArray( COLOR_IRGB irgbColor, int MipNumber);

	void FreeEffectsData( void);
	void FreeCutAndPasteData( void);
	bool LoadFromImageHelper (CImageHelper *pImageHelper);
	bool LoadImage(CString FileName);
	bool LoadImage(CImageHelper *pImageHelper);	
	void SetName (LPCTSTR szName);
	void CalcImageColorSinHeader();

	void SerializeQuake2 (CArchive& ar);
	void SerializeQuake1 (CArchive& ar);
	void SerializeSin(CArchive& ar);
	void SerializeHeretic2(CArchive& ar);
	void SerializePackage (bool bSaveAs, LPCTSTR szPath);
	void SerializeImage (CArchive& ar);

	void SetPackageDoc (CPackageDoc *pPackageDoc)
	{
		ASSERT (pPackageDoc);
		m_pPackageDoc = pPackageDoc;
		
		if (pPackageDoc->GetPathName() != "")
		{
			SetPackagePath (pPackageDoc->GetPathName());
		}
	}

	bool HasPackageDoc()
	{
		return (m_pPackageDoc != NULL);
	}

	void SetPackagePath (LPCTSTR szPath)
	{		
		m_strPackageFile = szPath;
		//SetPathName (m_strPackageFile, false);
		// store the path fully qualified
		//TCHAR szFullPath[_MAX_PATH];
		//AfxFullPath(szFullPath, szPath);
		m_strPathName = m_strPackageMipName;
		ASSERT(!m_strPathName.IsEmpty());       // must be set to something
		m_bEmbedded = FALSE;	
	}

	void DisconnectPackage (bool bLocalDisconnect)
	{
		if (bLocalDisconnect)
		{
			m_pPackageDoc->BreakDocConnection(this);
		}
		m_pPackageDoc = NULL;
		m_LastGameType = FILE_TYPE_START - 1000;
	}

	CString GetPackageMipName()
	{
		return m_strPackageMipName;
	}

	bool HasAlphaChannel()
	{
		return false;
	}
	bool IsAlphaChannelSelected()
	{
		return false;
	}

	CWallyPalette* GetPalette( void)
	{
		return &m_Palette;
	}
	CWallyView *GetView();

	void GetPalette( BYTE* pPackedPal, int iNumColors)
	{
		m_Palette.GetPalette( pPackedPal, iNumColors);
	}

	void SetPalette( BYTE* pPackedPal, int iNumColors, BOOL bUpdate = TRUE);
	void SetPalette( CWallyPalette* pPal, BOOL bUpdate = TRUE);

	void ClearPalette()
	{
		m_Palette.ClearPalette();
	}

	int FindNearestColor( int iRed, int iGreen, int iBlue, BOOL bIsFullBright)
	{
		return m_Palette.FindNearestColor( iRed, iGreen, iBlue, bIsFullBright);
	}

	void Convert24BitTo256Color( COLOR_IRGB* pSrcData, 
			BYTE* byDestData, int iWidth, int iHeight, int iBuild, 
			UINT uDitherType, BOOL bSharpen)
	{
		m_Palette.Convert24BitTo256Color( pSrcData, byDestData, 
			iWidth, iHeight, iBuild, uDitherType, bSharpen);
	}

	///////////////////////////////////
	// coordinate wrapping routines //
	/////////////////////////////////

	// neal - inlined for speed

	int GetWrappedIndex( int iX, int iY)
		{
			return GetCurrentLayer()->GetWrappedIndex( iX, iY);
		};

	int GetWrappedX( int iX)
		{
			return GetCurrentLayer()->GetWrappedX( iX);
		};

	int GetWrappedY( int iY)
		{
			return GetCurrentLayer()->GetWrappedY( iY);
		};

	////////////////////////////
	// pixel access routines //
	//////////////////////////
	
	// neal - inlined for speed
protected:
	void SetLayerPixel( CWallyView* pWallyView, int iX, int iY, COLOR_IRGB irgbColor)
	{
#ifdef _DEBUG
//		int iVal   = GetIValue( irgbColor);
//		int iColor = m_Palette.FindNearestColor( irgbColor, (iVal != 255));
//
//		ASSERT( iColor == iVal);
#endif

		if (HasSelection())
		{
			if (pWallyView && (! pWallyView->m_bTile))
			{
				CSelection* pSel = GetSelection();
				CRect       Rect = pSel->GetBoundsRect();

				if ((iY < Rect.top) || (iY >= Rect.bottom) ||
							(iX < Rect.left) || (iX >= Rect.right))
					return;
			}
		}
		GetCurrentLayer()->SetPixel( pWallyView, iX, iY, irgbColor);
	}

	////////////////////////////////
	// New pixel access routines //
	//////////////////////////////
public:
	COLOR_IRGB GetPixel( int iX, int iY)
		{
			return GetCurrentLayer()->GetPixel( iX, iY);
		};
	COLOR_IRGB GetWrappedPixel( int iX, int iY)
		{
			return GetPixel( GetWrappedX( iX), GetWrappedY( iY));
		};

	// use these if you have the full IRGB value
	void SetPixel( CWallyView* pWallyView, int iX, int iY, COLOR_IRGB irgbColor)
		{
			SetLayerPixel( pWallyView, iX, iY, irgbColor);
		};
	void SetWrappedPixel( CWallyView* pWallyView, int iX, int iY, COLOR_IRGB irgbColor)
		{
			SetPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), irgbColor);
		};

	// use these if you just have the color palette index
	void SetIndexColorPixel( CWallyView* pWallyView, int iX, int iY, int iColor)
		{
			int r = m_Palette.GetR( iColor);
			int g = m_Palette.GetG( iColor);
			int b = m_Palette.GetB( iColor);
			COLOR_IRGB irgbColor = IRGB( iColor, r, g, b);

			SetPixel( pWallyView, iX, iY, irgbColor);
		};
	void SetIndexColorWrappedPixel( CWallyView* pWallyView, int iX, int iY, int iColor)
		{
			SetIndexColorPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), iColor);
		};

	// use these if you just have the RGB color and need to calc index color too
	void SetNearestColorPixel( CWallyView* pWallyView, int iX, int iY, COLORREF rgbColor, BOOL bIsFullBright = FALSE)
		{
			int iColor = m_Palette.FindNearestColor( rgbColor, bIsFullBright);
			int r      = GetRValue( rgbColor);
			int g      = GetGValue( rgbColor);
			int b      = GetBValue( rgbColor);

			SetPixel( pWallyView, iX, iY, IRGB( iColor, r, g, b));
		};
	void SetNearestColorWrappedPixel( CWallyView* pWallyView, int iX, int iY, COLORREF rgbColor, BOOL bIsFullBright = FALSE)
		{
			SetNearestColorPixel( pWallyView, GetWrappedX( iX), GetWrappedY( iY), rgbColor, bIsFullBright);
		};

	void ClearLayer( COLOR_IRGB irgbColor);
	void CopyLayerToMip( BYTE* pbyRawMipData = NULL);
	void CopyMipToLayer( BYTE* pbyRawMipData = NULL);
	void RebuildSubMips();	
	BOOL DoSave( LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	void SetDimensions( int Width, int Height);	
	
	int Width()     { return m_Base.GetLayer()->GetWidth(); }
	int Height()    { return m_Base.GetLayer()->GetHeight(); }
	int GetWidth(int iMipNumber = 0)  { return MipArray[iMipNumber].width; }
	int GetHeight(int iMipNumber = 0) { return MipArray[iMipNumber].height; }
	int GetSize(int iMipNumber = 0)   { return (GetWidth(iMipNumber) * GetHeight(iMipNumber)); }

//	inline operator unsigned char* ()
//	{
//		return MipArray[0].mip_pointer;
//	}
	BYTE* GetBits( int iMipNumber = 0)
	{
		// Neal - TODO - this may be a problem for 24-bit
		return MipArray[iMipNumber].mip_pointer;
	}

	UINT GetNumBits( void)		// 8 or 24
	{
		int iNumBits = m_Base.GetLayer()->GetNumBits();
		ASSERT( (iNumBits == 8) || (iNumBits == 24) || (iNumBits == 32));

		return 	m_iOriginalBitDepth;
	}
	UINT GetOriginalBitDepth()
	{
		return m_iOriginalBitDepth;
	}
	void SetOriginalBitDepth(UINT iBitDepth)
	{
		m_iOriginalBitDepth = iBitDepth;
	}


	void UpdateSubMipDIBs( void);
	void UpdateAllDIBs( BOOL bRebuildSubMips);

	void SetGameType (int iGameType)
	{
		if (iGameType == FILE_TYPE_HALF_LIFE_WAD)
		{
			iGameType = FILE_TYPE_HALF_LIFE;
		}
		m_iGameType = iGameType;
	}

	int GetGameType()		{ return m_iGameType; }

	int HasAdjustablePalette()
	{
		return (m_iGameType != FILE_TYPE_QUAKE2);
	}

	BOOL AreImageDimensionsValid( int iGameType, bool bShowWarningBox = TRUE);

	void DeleteMips( void);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

	virtual void SetModifiedFlag( BOOL bModified = TRUE)
	{
		bool bNeedToUpdateTitle = (bModified != IsModified());
		CDocument::SetModifiedFlag( bModified);

		if (bNeedToUpdateTitle)
			SetTitle( NULL);
	}

// Implementation
public:
	virtual ~CWallyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWallyDoc)
	afx_msg void OnTpFlags();
	afx_msg void OnViewToolOptions();
	afx_msg void OnFileExport();
	afx_msg void OnUpdateTpFlags(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif  // _WALLYDOC_H_

