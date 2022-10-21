#ifndef _SINFLAGSCOLORWND_H_
#define _SINFLAGSCOLORWND_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SinFlagsColorWnd.h : header file
//

#define COLOR_BUTTON_WIDTH		42
#define	COLOR_BUTTON_HEIGHT		40

#define	SINFLAGS_WND_LBUTTON_SELECT		0

extern int WM_SINFLAGSWND_CUSTOM;

/////////////////////////////////////////////////////////////////////////////
// CSinFlagsColorWnd window

class CSinFlagsColorWnd : public CWnd
{
// Construction
public:
	CSinFlagsColorWnd();

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
	void SetColor (float fRed, float fGreen, float fBlue)
	{
		if (fRed > 1.0)
		{
			m_byRed = 255;			
		}
		else
		{
			m_byRed = (BYTE)(fRed * 255.0);
		}

		if (fGreen > 1.0)
		{
			m_byGreen = 255;
		}
		else
		{
			m_byGreen = (BYTE)(fGreen * 255.0);
		}

		if (fBlue > 1.0)
		{
			m_byBlue = 255;
		}
		else
		{
			m_byBlue = (BYTE)(fBlue  * 255.0);
		}
	}
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSinFlagsColorWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSinFlagsColorWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSinFlagsColorWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #ifndef _SINFLAGSCOLORWND_H_
