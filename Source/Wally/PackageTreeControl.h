#if !defined(AFX_PACKAGETREECONTROL_H__FA6DB953_E2CA_4C1E_8C7B_169339C9B942__INCLUDED_)
#define AFX_PACKAGETREECONTROL_H__FA6DB953_E2CA_4C1E_8C7B_169339C9B942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PackageTreeControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPackageTreeControl window

#define TVGN_EX_ALL			0x000F
#define TCEX_EDITLABEL		1		// Edit label timer event
#define	PACKAGETREECONTROL_SELCHANGE		0
#define	PACKAGETREECONTROL_OPENTEXTURE		1

extern int WM_PACKAGETREECONTROL_CUSTOM;

#include <vector>

class CWADItem;
class CPackageDoc;
class CPackageTreeControl;

typedef enum
{
	ePackageTreeEntryCategory,
	ePackageTreeEntryImage
} PackageTreeEntryEnum;

typedef std::pair< std::string, CWADItem * > _NameItemPair;
typedef std::vector< _NameItemPair > _NameItemVector;
typedef _NameItemVector::iterator _itNameItem;

class CPackageTreeEntryBase
{
protected:
	_NameItemVector m_NameItemVector;
	_NameItemVector m_FilteredNameItemVector;
	_NameItemVector *m_pCurrentNameItemVector;

	std::string m_strPreviousFilter;
	CPackageTreeEntryBase *m_pParentEntry;
	HTREEITEM m_htItem;

public:
	CPackageTreeEntryBase();
	virtual ~CPackageTreeEntryBase();
	virtual PackageTreeEntryEnum GetType() = 0;
	virtual void FilterItems( const char *szFilter );
	virtual void RefreshFilter();
	virtual CPackageTreeEntryBase *GetParent();
	virtual void SetParent( CPackageTreeEntryBase *pParent );
	virtual void AddNameItem( const char *szName, CWADItem *pItem, bool bCheckForDuplicate = false );
	virtual const char *GetName() = 0;
	virtual void SetName( const char *szName ) = 0;
	virtual UINT GetItemCount() = 0;
	virtual CWADItem *GetItem() = 0;
	virtual CWADItem *GetItemAtPosition( UINT iDesiredPosition ) = 0;
	virtual void SetSelected( UINT nState ) = 0;
	virtual bool ItemExists( CWADItem *pItem );
	virtual void SetTreeItem( HTREEITEM hItem ) { m_htItem = hItem; }; 
	virtual HTREEITEM GetTreeItem() { return m_htItem; };
	virtual void RenameItem( CPackageDoc *pDoc, CPackageTreeControl *pTreeCtrl ) = 0;
};

typedef std::vector< CPackageTreeEntryBase * > _PackageTreeEntryVector;
typedef _PackageTreeEntryVector::iterator _itPackageTreeEntry;

class CPackageTreeEntryCategory : public CPackageTreeEntryBase
{
private:
	_PackageTreeEntryVector m_ChildVector; // This is for sub-categories
	std::string m_strName;
	
protected:
	virtual void FilterItems( const char *szFilter );

public:
	CPackageTreeEntryCategory( const char *szName, CPackageTreeEntryBase *pParent );
	virtual ~CPackageTreeEntryCategory();
	virtual PackageTreeEntryEnum GetType() { return ePackageTreeEntryCategory; }
	virtual const char *GetName();
	virtual void SetName( const char *szName );
	virtual CPackageTreeEntryBase *AddItem( const char *szName, CWADItem *pItem, bool bCheckForDuplicate = false );
	virtual CWADItem *GetItem();
	virtual CWADItem *GetItemAtPosition( UINT iDesiredPosition );
	virtual UINT GetItemCount();
	virtual void SetSelected( UINT nState ){};
	virtual void RenameItem( CPackageDoc *pDoc, CPackageTreeControl *pTreeCtrl );
};

class CPackageTreeEntryImage : public CPackageTreeEntryBase
{
private:	
	_NameItemPair m_NameItemPair;

public:
	CPackageTreeEntryImage( const char *szName, CWADItem *pItem, CPackageTreeEntryBase *pParent );
	virtual ~CPackageTreeEntryImage();
	virtual PackageTreeEntryEnum GetType() { return ePackageTreeEntryImage; }
	virtual const char *GetName();
	virtual void SetName( const char *szName );	
	virtual void SetItem( CWADItem *pItem );
	virtual CWADItem *GetItem();
	virtual CWADItem *GetItemAtPosition( UINT iDesiredPosition );
	virtual UINT GetItemCount();
	virtual void SetSelected( UINT nState );
	virtual void RenameItem( CPackageDoc *pDoc, CPackageTreeControl *pTreeCtrl );
};


class CPackageTreeControl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CPackageTreeControl)

// Construction
public:
	CPackageTreeControl();
	BOOL Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:
	UINT GetSelectedCount() const;
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode);
	HTREEITEM GetFirstSelectedItem();
	HTREEITEM GetNextSelectedItem(HTREEITEM hItem);
	HTREEITEM GetPrevSelectedItem(HTREEITEM hItem);
	HTREEITEM ItemFromData(DWORD dwData, HTREEITEM hStartAtItem=NULL) const;
	BOOL SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask ); 

	void RenameEntry( HTREEITEM hItem, const char *szName );
	UINT GetVisibleCount();
	UINT CountImages( HTREEITEM hItem );
	CWADItem *GetImageAtPosition( UINT iDesiredPosition );
	CWADItem *GetSelectedImage();
	CWADItem *GetImageFromHTREEITEM( HTREEITEM hItem );
	BOOL SelectItemEx(HTREEITEM hItem, BOOL bSelect=TRUE);
	BOOL SelectItem(HTREEITEM hItem);
	BOOL SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem);
	void ClearSelection(BOOL bMultiOnly=FALSE);

	void OnInitialUpdate();
	void InitializeImageList();
	void AddToAllImages( CWADItem *pItem );
	void RenameSelectedItem( CPackageDoc *pDoc );

protected:
	void SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point );	
	UINT CountImages( HTREEITEM hItem ) const;
	CWADItem *_GetImageAtPosition( HTREEITEM hItem, UINT iDesiredPosition, UINT &iPosition );
	CPackageTreeEntryCategory *CreateCategory( const char *szCategory, CPackageTreeEntryBase *pParent );
	CPackageTreeEntryImage *CreateImage( const char *szName, CWADItem *pItem, CPackageTreeEntryBase *pParent );
	void SetSelected( HTREEITEM hItem );

// Attributes
private:
	_PackageTreeEntryVector m_PackageTreeVector;
	bool m_bImageListInitialized;
	int m_iBlankIcon;
	int m_iClosedFolderIcon;
	int m_iOpenFolderIcon;
	CImageList m_ImageList;
	HTREEITEM m_htAllImages;
	HTREEITEM m_htCategories;
	HTREEITEM m_htTransparent;
	HTREEITEM m_htTransparentFloors;

	BOOL		m_bSelectPending;
	CPoint		m_ptClick;
	HTREEITEM	m_hClickedItem;
	HTREEITEM	m_hFirstSelectedItem;
	BOOL		m_bSelectionComplete;
	BOOL		m_bEditLabelPending;
	UINT		m_idTimer;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageTreeControl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackageTreeControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackageTreeControl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
};

HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD dwData, HTREEITEM hStartAtItem=NULL);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGETREECONTROL_H__FA6DB953_E2CA_4C1E_8C7B_169339C9B942__INCLUDED_)
