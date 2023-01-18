#if !defined(AFX_DECALCOLORCHOOSERWND_H__E47F97DE_1413_488D_9E2A_B22C509B46CA__INCLUDED_)
#define AFX_DECALCOLORCHOOSERWND_H__E47F97DE_1413_488D_9E2A_B22C509B46CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DecalColorChooserWnd.h : header file
//

#define	DECAL_COLOR_CHOOSER_WND_LBUTTON_SELECT		0
extern int WM_DECALCOLORCHOOSERWND_CUSTOM;

/////////////////////////////////////////////////////////////////////////////
// CDecalColorChooserWnd window

class CDecalColorChooserWnd : public CWnd
{
// Construction
public:
	CDecalColorChooserWnd();

//static:	
	static int DECAL_COLOR_CHOOSER_BUTTON_WIDTH;
	static int DECAL_COLOR_CHOOSER_BUTTON_HEIGHT;	

// Members
private:
	BYTE m_byRed;
	BYTE m_byGreen;
	BYTE m_byBlue;

	BOOL m_bLeftButtonDown;

// Attributes
public:

// Operations
public:
	void SetColor( BYTE byRed, BYTE byGreen, BYTE byBlue );
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecalColorChooserWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDecalColorChooserWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDecalColorChooserWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECALCOLORCHOOSERWND_H__E47F97DE_1413_488D_9E2A_B22C509B46CA__INCLUDED_)
