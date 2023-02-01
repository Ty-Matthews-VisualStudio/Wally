#if !defined(AFX_TEXTUREINFO_H__41634F61_91C4_11D1_8068_5C2203C10627__INCLUDED_)
#define AFX_TEXTUREINFO_H__41634F61_91C4_11D1_8068_5C2203C10627__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TextureInfo.h : header file
//

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

class Q2Engine
{
public:
	std::string m_sFolderName;
	std::string m_sName;
	std::string m_sErrorMessage;

	typedef std::vector< std::pair< std::string, UINT > > _NameBitmaskPair;
	_NameBitmaskPair m_vFlags;
	_NameBitmaskPair m_vContent;

	BOOL LoadJSON(LPCTSTR szFolderName);
	BOOL LoadFile(std::stringstream &ssFileName, _NameBitmaskPair& vPairs);
	static BOOL CreateDefaultJSON();
};

/////////////////////////////////////////////////////////////////////////////
// CTextureInfo dialog

class CTextureInfo : public CDialog
{
// Construction
public:
	CTextureInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextureInfo();

	std::vector< Q2Engine* > m_vEngines;
	std::map<UINT, UINT> m_mID2Flags;
	std::map<UINT, UINT> m_mID2Contents;

// Dialog Data
	//{{AFX_DATA(CTextureInfo)
	enum { IDD = IDD_TEXTURE_INFO };
	CString	m_AnimationName;
	CString	m_TextureHeight;
	CString	m_TextureName;
	int		m_Value;
	CString	m_TextureWidth;	
	//}}AFX_DATA

	bool ModifiedFlag;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Members
	LPQ2_MIP_S DocWalHeader;
	q2_miptex_s m_Q2Header;

// Methods
public:
	void Init (LPQ2_MIP_S WalHeader);
	void LoadJSON();
	void CleanUp();
	void EnableDisableFlagsContent();
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextureInfo)
	afx_msg void OnChangeTextureName();
	afx_msg void OnChangeAnimname();
	afx_msg void OnChangeValue();	
	virtual void OnOK();
	afx_msg bool OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbEngines;
	afx_msg void OnCbnSelchangeTextureinfoQ2engine();
	virtual BOOL OnCommand(WPARAM w, LPARAM l);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREINFO_H__41634F61_91C4_11D1_8068_5C2203C10627__INCLUDED_)
