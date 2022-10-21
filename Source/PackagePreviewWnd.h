#ifndef	_PACKAGE_PREVIEW_WND_H_
#define _PACKAGE_PREVIEW_WND_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PackagePreviewWnd.h : header file
//

#define PP_TYPE_NORMAL	0
#define PP_TYPE_DECAL	1

/////////////////////////////////////////////////////////////////////////////
// CPackagePreviewWnd window

class CPackagePreviewWnd : public CWnd
{
// Construction
public:
	CPackagePreviewWnd();

// Members
private:
	BYTE m_byPalette[768];
	BYTE *m_byBits;
	BYTE *m_byDecalBits;
	CString m_strNonImageText;
	CPoint m_ptNonImageLocation;
	int m_iWidth;
	int m_iHeight;
	int m_iPreviewWidth;
	int m_iPreviewHeight;
	int m_iPreviewType;
	bool m_bNotRegistered;

// Operations
public:
	void SetWidth	(int iWidth);
	void SetHeight	(int iHeight);	
	int GetWidth	();
	int GetHeight	();
	void SetPreviewWidth (int iWidth);
	void SetPreviewHeight (int iHeight);
	int GetPreviewWidth ();
	int GetPreviewHeight ();
	void SetNonImageText (LPCTSTR szText);
	CString GetNonImageText ();
	void SetNonImageLocation (int x, int y);
	CPoint GetNonImageLocation ();
	BYTE *GetBits();
	BYTE *GetDecalBits();
	BYTE *GetPalette();
	void InitImage	(int iWidth, int iHeight, unsigned char *pbyBits, unsigned char *pbyPalette);
	void SetType (int iType);
	int GetType ();
		

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPackagePreviewWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPackagePreviewWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPackagePreviewWnd)
	afx_msg void OnPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _PACKAGE_PREVIEW_WND_H_
