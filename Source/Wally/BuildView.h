#if !defined(AFX_BUILDVIEW_H__90F3C0A2_DE3A_11D2_BB02_00104BCBA50D__INCLUDED_)
#define AFX_BUILDVIEW_H__90F3C0A2_DE3A_11D2_BB02_00104BCBA50D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BuildView.h : header file
//

#include "BuildDoc.h"
#include "DibSection.h"

#define BUILD_IMAGE_HEIGHT	160
#define BUILD_IMAGE_WIDTH	132
#define BUILD_TEXTURE_LIMIT	128
#define BUILD_IMAGE_XOFFSET	2
#define BUILD_IMAGE_YOFFSET	2



/////////////////////////////////////////////////////////////////////////////
// CBuildView view

class CBuildView : public CScrollView
{
protected:
	CBuildView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBuildView)

// Attributes
public:
	int m_iTileNumber;
	CSize m_DocSize;
	int m_iMaxVImages;
	int m_iMaxHImages;
	int m_iPopupMenuTile;
	BYTE m_byPalette[768];
	CDibSection *m_pdsImage;
	CDibSection m_dsButton;
	int m_iLastWidth;
	int m_iLastHeight;

// Operations
public:
	CBuildDoc* GetDocument()
	{
		return (CBuildDoc *)m_pDocument;
	}
	int GetTileNumber(CPoint ptMouse);
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBuildView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CBuildView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBuildviewPopupCopy();
	afx_msg void OnBuildviewPopupProperties();
	afx_msg void OnBuildviewPopupPaste();
	afx_msg void OnUpdateBuildviewPopupPaste(CCmdUI* pCmdUI);
	afx_msg void OnBuildviewPopupDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDVIEW_H__90F3C0A2_DE3A_11D2_BB02_00104BCBA50D__INCLUDED_)
