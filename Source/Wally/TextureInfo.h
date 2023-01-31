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

	typedef std::vector< std::pair< std::string, int > > _NameBitmaskPair;
	_NameBitmaskPair m_vFlags;
	_NameBitmaskPair m_vContent;

	BOOL LoadJSON(LPCTSTR szFolderName);
	BOOL LoadFile(std::stringstream &ssFileName, _NameBitmaskPair& vPairs);
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

// Dialog Data
	//{{AFX_DATA(CTextureInfo)
	enum { IDD = IDD_TEXTURE_INFO };
	CString	m_AnimationName;
	CString	m_TextureHeight;
	CString	m_TextureName;
	int		m_Value;
	CString	m_TextureWidth;
	BOOL	m_SkyFlag;
	BOOL	m_FlowingFlag;
	BOOL	m_LightFlag;
	BOOL	m_NoDrawFlag;
	BOOL	m_SlickFlag;
	BOOL	m_Trans33Flag;
	BOOL	m_Trans66Flag;
	BOOL	m_WarpFlag;
	BOOL	m_AuxFlag;
	BOOL	m_LavaFlag;
	BOOL	m_MistFlag;
	BOOL	m_SlimeFlag;
	BOOL	m_SolidFlag;
	BOOL	m_WaterFlag;
	BOOL	m_WindowFlag;
	BOOL	m_LadderFlag;
	BOOL	m_Current0Flag;
	BOOL	m_Current180Flag;
	BOOL	m_Current270Flag;
	BOOL	m_Current90Flag;
	BOOL	m_CurrentUpFlag;
	BOOL	m_DetailFlag;
	BOOL	m_MonsterClipFlag;
	BOOL	m_OriginFlag;
	BOOL	m_PlayerClipFlag;
	BOOL	m_CurrentDnFlag;
	BOOL	m_TranslucentFlag;
	BOOL	m_HintFlag;
	BOOL	m_SkipFlag;
	BOOL	m_AreaPortalFlag;
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

// Methods
public:
	void Init (LPQ2_MIP_S WalHeader);
	void LoadJSON();
	void CleanUp();
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextureInfo)
	afx_msg void OnChangeTextureName();
	afx_msg void OnChangeAnimname();
	afx_msg void OnChangeValue();
	afx_msg void OnAuxCheck();
	afx_msg void OnFlowingCheck();
	afx_msg void OnLavaCheck();
	afx_msg void OnLightCheck();
	afx_msg void OnMistCheck();
	afx_msg void OnNodrawCheck();
	afx_msg void OnSkyCheck();
	afx_msg void OnSlickCheck();
	afx_msg void OnSlimeCheck();
	afx_msg void OnSolidCheck();
	afx_msg void OnTrans33Check();
	afx_msg void OnTrans66Check();
	afx_msg void OnWarpCheck();
	afx_msg void OnWaterCheck();
	afx_msg void OnWindowCheck();
	afx_msg void OnCurrent0Check();
	afx_msg void OnCurrent180Check();
	afx_msg void OnCurrent270Check();
	afx_msg void OnCurrent90Check();
	afx_msg void OnCurrentdnCheck();
	afx_msg void OnCurrentupCheck();
	afx_msg void OnDetailCheck();
	afx_msg void OnLadderCheck();
	afx_msg void OnOriginCheck();
	afx_msg void OnMonsterclipCheck();
	afx_msg void OnPlayerclipCheck();
	afx_msg void OnPortalCheck();
	afx_msg void OnSkipCheck();
	afx_msg void OnTranslucentCheck();
	virtual void OnOK();
	afx_msg void OnHintCheck();
	afx_msg bool OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbEngines;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREINFO_H__41634F61_91C4_11D1_8068_5C2203C10627__INCLUDED_)
