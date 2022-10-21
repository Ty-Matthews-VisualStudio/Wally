#ifndef _MOUSE_WND_H_
#define _MOUSE_WND_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MouseWnd.h : header file
//

// These defines coincide with the prefab image numbers
#define MOUSE_STATE_NORMAL				0x1
#define MOUSE_STATE_LBUTTON_DOWN		0x2
#define MOUSE_STATE_RBUTTON_DOWN		0x4
#define MOUSE_STATE_DISABLED			0x8
#define MOUSE_STATE_RBUTTON_DISABLED	0x10

// Custom message map flags
#define	MOUSE_WND_LBUTTON_SELECT			0
#define	MOUSE_WND_RBUTTON_SELECT			1
extern int WM_MOUSE_WND_CUSTOM;

/////////////////////////////////////////////////////////////////////////////
// CMouseWnd window

class CMouseWnd : public CWnd
{
// Construction
public:
	CMouseWnd();

// Attributes
private:
	int m_iButtonState;	

// Operations
public:
	void SetButtonState (int iState = MOUSE_STATE_NORMAL)
	{
		// These two states offset each other.
		switch (iState)
		{
		case MOUSE_STATE_NORMAL:
			{
				if (IsDisabled())
				{
					RemoveButtonState(MOUSE_STATE_DISABLED);
				}				
			}
			break;

		case MOUSE_STATE_DISABLED:
			{
				if (!IsDisabled())
				{
					RemoveButtonState(MOUSE_STATE_NORMAL);
				}
			}
			break;

		default:
			break;
		}
		
		m_iButtonState |= iState;
	}
	void RemoveButtonState (int iState)
	{
		m_iButtonState |= iState;
		m_iButtonState ^= iState;
	}
	int GetButtonState ()
	{
		return m_iButtonState;
	}
	void Update();
	bool IsDisabled()
	{
		return (GetButtonState() & MOUSE_STATE_DISABLED);
	}
	bool IsRightButtonDisabled()
	{
		return (GetButtonState() & MOUSE_STATE_RBUTTON_DISABLED);
	}
	bool IsLeftButtonDown()
	{
		return (GetButtonState() & MOUSE_STATE_LBUTTON_DOWN);
	}
	bool IsRightButtonDown()
	{
		return (GetButtonState() & MOUSE_STATE_RBUTTON_DOWN);
	}
	void HandleLButtonDown (CPoint point);
	void HandleRButtonDown (CPoint point);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMouseWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMouseWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMouseWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _MOUSE_WND_H_
