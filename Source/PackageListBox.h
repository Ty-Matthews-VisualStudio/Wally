#if !defined(AFX_PACKAGELISTBOX_H__016D8EA1_907B_11D2_8410_F4900FC10000__INCLUDED_)
#define AFX_PACKAGELISTBOX_H__016D8EA1_907B_11D2_8410_F4900FC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackageListBox.h : header file
//

// Custom message map flags
#define	PACKAGELISTBOX_SELCHANGE		0
#define	PACKAGELISTBOX_OPENTEXTURE		1
#define PFORM_LB_ITEMHEIGHT				16
extern int WM_PACKAGELISTBOX_CUSTOM;


// This is for the custom WAD drag-n-drop piece:

struct WallyCustomWADHeader
{
	int iNumImages;
	int iDataOffset;
};

struct WallyCustomMIPHeader
{
	char szName[16];	
	int iWidth;
	int iHeight;
	int iDataOffset;
	char Palette[768];
};

/////////////////////////////////////////////////////////////////////////////
// CPackageListBox window

class CWADItem;

class CPackageListBox : public CListBox
{
// Construction
public:
	CPackageListBox();

// Members
private:
	int m_iBaseShiftIndex;
	bool m_bRightButtonDown;
	bool m_bDragging;
	CPoint m_ptRightButtonDown;
	COleDropSource m_oleDropSource;
	COleDataSource m_oleDataSource;
	COleDropTarget m_oleDropTarget;
	CBrush m_brColor;

// Operations
public:
	int CalcItemSize (CWADItem *pItem);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackageListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackageListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackageListBox)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PACKAGELISTBOX_H__016D8EA1_907B_11D2_8410_F4900FC10000__INCLUDED_)
