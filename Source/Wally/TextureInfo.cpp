/////////////////////////////////////////////////////////////////////////////
//                           Wally the WAL Editor
//---------------------------------------------------------------------------
//                             © Copyright 1998,
//                      Ty Matthews and Neal White III,
//                           All rights reserved.
//---------------------------------------------------------------------------
//
// TextureInfo.cpp : implementation file
//
// Created by Ty Matthews, 2-1-1998
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wally.h"
#include "TextureInfo.h"
#include "ImageHelper.h"
#include "TextureFlags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureInfo dialog


CTextureInfo::CTextureInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextureInfo)
	m_AnimationName = _T("");
	m_TextureHeight = _T("");
	m_TextureName = _T("");
	m_Value = 0;
	m_TextureWidth = _T("");
	m_SkyFlag = FALSE;
	m_FlowingFlag = FALSE;
	m_LightFlag = FALSE;
	m_NoDrawFlag = FALSE;
	m_SlickFlag = FALSE;
	m_Trans33Flag = FALSE;
	m_Trans66Flag = FALSE;
	m_WarpFlag = FALSE;
	m_AuxFlag = FALSE;
	m_LavaFlag = FALSE;
	m_MistFlag = FALSE;
	m_SlimeFlag = FALSE;
	m_SolidFlag = FALSE;
	m_WaterFlag = FALSE;
	m_WindowFlag = FALSE;
	m_LadderFlag = FALSE;
	m_Current0Flag = FALSE;
	m_Current180Flag = FALSE;
	m_Current270Flag = FALSE;
	m_Current90Flag = FALSE;
	m_CurrentUpFlag = FALSE;
	m_DetailFlag = FALSE;
	m_MonsterClipFlag = FALSE;
	m_OriginFlag = FALSE;
	m_PlayerClipFlag = FALSE;
	m_CurrentDnFlag = FALSE;
	m_TranslucentFlag = FALSE;
	m_HintFlag = FALSE;
	m_SkipFlag = FALSE;
	m_AreaPortalFlag = FALSE;
	//}}AFX_DATA_INIT
	ModifiedFlag = FALSE;
}


void CTextureInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureInfo)
	DDX_Text(pDX, IDC_ANIMNAME, m_AnimationName);
	DDV_MaxChars(pDX, m_AnimationName, 31);
	DDX_Text(pDX, IDC_HEIGHT, m_TextureHeight);
	DDX_Text(pDX, IDC_TEXTURE_NAME, m_TextureName);
	DDV_MaxChars(pDX, m_TextureName, 31);
	DDX_Text(pDX, IDC_VALUE, m_Value);
	DDV_MinMaxInt(pDX, m_Value, 0, 32767);
	DDX_Text(pDX, IDC_WIDTH, m_TextureWidth);
	DDX_Check(pDX, IDC_SKY_CHECK, m_SkyFlag);
	DDX_Check(pDX, IDC_FLOWING_CHECK, m_FlowingFlag);
	DDX_Check(pDX, IDC_LIGHT_CHECK, m_LightFlag);
	DDX_Check(pDX, IDC_NODRAW_CHECK, m_NoDrawFlag);
	DDX_Check(pDX, IDC_SLICK_CHECK, m_SlickFlag);
	DDX_Check(pDX, IDC_TRANS33_CHECK, m_Trans33Flag);
	DDX_Check(pDX, IDC_TRANS66_CHECK, m_Trans66Flag);
	DDX_Check(pDX, IDC_WARP_CHECK, m_WarpFlag);
	DDX_Check(pDX, IDC_AUX_CHECK, m_AuxFlag);
	DDX_Check(pDX, IDC_LAVA_CHECK, m_LavaFlag);
	DDX_Check(pDX, IDC_MIST_CHECK, m_MistFlag);
	DDX_Check(pDX, IDC_SLIME_CHECK, m_SlimeFlag);
	DDX_Check(pDX, IDC_SOLID_CHECK, m_SolidFlag);
	DDX_Check(pDX, IDC_WATER_CHECK, m_WaterFlag);
	DDX_Check(pDX, IDC_WINDOW_CHECK, m_WindowFlag);
	DDX_Check(pDX, IDC_LADDER_CHECK, m_LadderFlag);
	DDX_Check(pDX, IDC_CURRENT0_CHECK, m_Current0Flag);
	DDX_Check(pDX, IDC_CURRENT180_CHECK, m_Current180Flag);
	DDX_Check(pDX, IDC_CURRENT270_CHECK, m_Current270Flag);
	DDX_Check(pDX, IDC_CURRENT90_CHECK, m_Current90Flag);
	DDX_Check(pDX, IDC_CURRENTUP_CHECK, m_CurrentUpFlag);
	DDX_Check(pDX, IDC_DETAIL_CHECK, m_DetailFlag);
	DDX_Check(pDX, IDC_MONSTERCLIP_CHECK, m_MonsterClipFlag);
	DDX_Check(pDX, IDC_ORIGIN_CHECK, m_OriginFlag);
	DDX_Check(pDX, IDC_PLAYERCLIP_CHECK, m_PlayerClipFlag);
	DDX_Check(pDX, IDC_CURRENTDN_CHECK, m_CurrentDnFlag);
	DDX_Check(pDX, IDC_TRANSLUCENT_CHECK, m_TranslucentFlag);
	DDX_Check(pDX, IDC_HINT_CHECK, m_HintFlag);
	DDX_Check(pDX, IDC_SKIP_CHECK, m_SkipFlag);
	DDX_Check(pDX, IDC_PORTAL_CHECK, m_AreaPortalFlag);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TEXTUREINFO_Q2ENGINE, m_lstEngines);
}


BEGIN_MESSAGE_MAP(CTextureInfo, CDialog)
	//{{AFX_MSG_MAP(CTextureInfo)
	ON_EN_CHANGE(IDC_TEXTURE_NAME, OnChangeTextureName)
	ON_EN_CHANGE(IDC_ANIMNAME, OnChangeAnimname)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)	
	ON_BN_CLICKED(IDC_AUX_CHECK, OnAuxCheck)
	ON_BN_CLICKED(IDC_FLOWING_CHECK, OnFlowingCheck)
	ON_BN_CLICKED(IDC_LAVA_CHECK, OnLavaCheck)
	ON_BN_CLICKED(IDC_LIGHT_CHECK, OnLightCheck)
	ON_BN_CLICKED(IDC_MIST_CHECK, OnMistCheck)
	ON_BN_CLICKED(IDC_NODRAW_CHECK, OnNodrawCheck)
	ON_BN_CLICKED(IDC_SKY_CHECK, OnSkyCheck)
	ON_BN_CLICKED(IDC_SLICK_CHECK, OnSlickCheck)
	ON_BN_CLICKED(IDC_SLIME_CHECK, OnSlimeCheck)
	ON_BN_CLICKED(IDC_SOLID_CHECK, OnSolidCheck)
	ON_BN_CLICKED(IDC_TRANS33_CHECK, OnTrans33Check)
	ON_BN_CLICKED(IDC_TRANS66_CHECK, OnTrans66Check)
	ON_BN_CLICKED(IDC_WARP_CHECK, OnWarpCheck)
	ON_BN_CLICKED(IDC_WATER_CHECK, OnWaterCheck)
	ON_BN_CLICKED(IDC_WINDOW_CHECK, OnWindowCheck)
	ON_BN_CLICKED(IDC_CURRENT0_CHECK, OnCurrent0Check)
	ON_BN_CLICKED(IDC_CURRENT180_CHECK, OnCurrent180Check)
	ON_BN_CLICKED(IDC_CURRENT270_CHECK, OnCurrent270Check)
	ON_BN_CLICKED(IDC_CURRENT90_CHECK, OnCurrent90Check)
	ON_BN_CLICKED(IDC_CURRENTDN_CHECK, OnCurrentdnCheck)
	ON_BN_CLICKED(IDC_CURRENTUP_CHECK, OnCurrentupCheck)
	ON_BN_CLICKED(IDC_DETAIL_CHECK, OnDetailCheck)
	ON_BN_CLICKED(IDC_LADDER_CHECK, OnLadderCheck)
	ON_BN_CLICKED(IDC_ORIGIN_CHECK, OnOriginCheck)
	ON_BN_CLICKED(IDC_MONSTERCLIP_CHECK, OnMonsterclipCheck)
	ON_BN_CLICKED(IDC_PLAYERCLIP_CHECK, OnPlayerclipCheck)
	ON_BN_CLICKED(IDC_PORTAL_CHECK, OnPortalCheck)
	ON_BN_CLICKED(IDC_SKIP_CHECK, OnSkipCheck)
	ON_BN_CLICKED(IDC_TRANSLUCENT_CHECK, OnTranslucentCheck)
	ON_BN_CLICKED(IDC_HINT_CHECK, OnHintCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void pretty_print( boost::json::value const& jv )
{
	std::stringstream ss;
	switch (jv.kind())
	{
	case boost::json::kind::object:
	{
		auto const& obj = jv.get_object();
		if (!obj.empty())
		{
			auto it = obj.begin();
			for (;;)
			{
				ss << boost::json::serialize(it->key());
				TRACE(ss.str().c_str());
				pretty_print(it->value());
				if (++it == obj.end())
					break;
				TRACE("\n");
			}
		}
		TRACE("\n");
		break;
	}

	case boost::json::kind::array:
	{
		TRACE("\n");
		auto const& arr = jv.get_array();
		if (!arr.empty())
		{
			auto it = arr.begin();
			for (;;)
			{
				pretty_print(*it);
				if (++it == arr.end())
					break;
				TRACE("\n");
			}
		}
		TRACE("\n");
		break;
	}

	case boost::json::kind::string:
	{
		ss << boost::json::serialize(jv.get_string());
		TRACE(ss.str().c_str());
	}
	break;

	case boost::json::kind::uint64:
	{
		ss << jv.get_uint64();
		TRACE(ss.str().c_str());
	}
	break;

	case boost::json::kind::int64:
	{
		ss << jv.get_int64();
		TRACE(ss.str().c_str());
	}
	break;

	case boost::json::kind::double_:
	{
		ss << jv.get_double();
		TRACE(ss.str().c_str());
	}
	break;

	case boost::json::kind::bool_:
	{
		if (jv.get_bool())
			ss << "true";
		else
			ss << "false";
		TRACE(ss.str().c_str());
	}
	break;

	case boost::json::kind::null:
	{
		ss << "null";
		TRACE(ss.str().c_str());
	}
	break;
	}
	
}

/////////////////////////////////////////////////////////////////////////////
// CTextureInfo message handlers
BOOL CTextureInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadJSON();
	return true;
}

void CTextureInfo::Init (LPQ2_MIP_S WalHeader)
{
	char buffer[50];
	DocWalHeader = WalHeader;

	m_TextureName = WalHeader->name;	
	
	_itoa_s (WalHeader->width, buffer, sizeof(buffer), 10);	
	m_TextureWidth = buffer;
	
	_itoa_s (WalHeader->height, buffer, sizeof(buffer), 10);
	m_TextureHeight = buffer;
	
	m_AnimationName = WalHeader->animname;
	m_Value = WalHeader->value;	

	m_LightFlag		= (WalHeader->flags & TF_LIGHT)   ? true : false;		
	m_SlickFlag		= (WalHeader->flags & TF_SLICK)   ? true : false; 	
	m_SkyFlag		= (WalHeader->flags & TF_SKY)     ? true : false;		
	m_WarpFlag		= (WalHeader->flags & TF_WARP)    ? true : false;
	m_Trans33Flag	= (WalHeader->flags & TF_TRANS33) ? true : false;	
	m_Trans66Flag	= (WalHeader->flags & TF_TRANS66) ? true : false;
	m_FlowingFlag	= (WalHeader->flags & TF_FLOWING) ? true : false;
	m_NoDrawFlag	= (WalHeader->flags & TF_NODRAW)  ? true : false;
	m_HintFlag		= (WalHeader->flags & TF_HINT)    ? true : false;	
	m_SkipFlag		= (WalHeader->flags & TF_SKIP)    ? true : false;	
	
	m_SolidFlag			= (WalHeader->contents & TC_SOLID)			? true : false;
	m_WindowFlag		= (WalHeader->contents & TC_WINDOW)			? true : false;
	m_AuxFlag			= (WalHeader->contents & TC_AUX)			? true : false;
	m_LavaFlag			= (WalHeader->contents & TC_LAVA)			? true : false;
	m_SlimeFlag			= (WalHeader->contents & TC_SLIME)			? true : false;
	m_WaterFlag			= (WalHeader->contents & TC_WATER)			? true : false;
	m_MistFlag			= (WalHeader->contents & TC_MIST)			? true : false;
	m_AreaPortalFlag	= (WalHeader->contents & TC_AREAPORTAL)		? true : false;
	m_PlayerClipFlag	= (WalHeader->contents & TC_PLAYERCLIP)		? true : false;
	m_MonsterClipFlag	= (WalHeader->contents & TC_MONSTERCLIP)	? true : false;
	m_Current0Flag		= (WalHeader->contents & TC_CURRENT_0)		? true : false;
	m_Current90Flag		= (WalHeader->contents & TC_CURRENT_90)		? true : false;
	m_Current180Flag	= (WalHeader->contents & TC_CURRENT_180)	? true : false;
	m_Current270Flag	= (WalHeader->contents & TC_CURRENT_270)	? true : false;
	m_CurrentUpFlag		= (WalHeader->contents & TC_CURRENT_UP)		? true : false;
	m_CurrentDnFlag		= (WalHeader->contents & TC_CURRENT_DN)		? true : false;
	m_OriginFlag		= (WalHeader->contents & TC_ORIGIN)			? true : false;	
	m_DetailFlag		= (WalHeader->contents & TC_DETAIL)			? true : false;
	m_TranslucentFlag	= (WalHeader->contents & TC_TRANSLUCENT)	? true : false;
	m_LadderFlag		= (WalHeader->contents & TC_LADDER)			? true : false;	
}

LPVOID CTextureInfo::LoadEngineJSON(LPCTSTR szFolderName)
{
	class pred
	{
	public:
		static bool ischar(char c)
		{
			return c == '"';
		};
	};

	std::string sEngine;
	CMemBuffer mbJSON;
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFileName[_MAX_FNAME];
	char szExt[_MAX_EXT];

	_splitpath_s(szFolderName, szDrive, sizeof(szDrive), szDir, sizeof(szDir), szFileName, sizeof(szFileName), szExt, sizeof(szExt));
	sEngine = szFileName;
	boost::algorithm::trim_if(sEngine, pred::ischar);
#if 0
	// Parse the JSON and return a void * if it's valid, otherwise NULL.
	// Find flags.json and content.json, parse each of them
	mbJSON.InitFromFile();
	boost::json::error_code ec;
	auto const jv = mbJSON.ParseJson(ec);
	if (ec)
	{
		std::stringstream ss;
		ss << "Error parsing " << szFileName << " JSON error code: " << ec;
		AfxMessageBox(ss.str().c_str(), MB_ICONSTOP);
		return nullptr;
	}
	else
	{
		// Validate the JSON and build a vector of std::pair<std::string,int>

		if (jv.kind() != boost::json::kind::object)
		{
			auto const& obj = jv.get_object();
			if (!obj.empty())
			{
				auto it = obj.begin();
				for (;;)
				{
					ss << boost::json::serialize(it->key());
					pretty_print(it->value());
					if (++it == obj.end())
						break;
				}
			}
		}
	}
#endif
	return nullptr;
}

void CTextureInfo::LoadJSON()
{
	CString sSourceFolder;
	sSourceFolder.Format("%s\\.wal\\", g_strJSONDirectory);
		
	for (auto& p : boost::filesystem::directory_iterator(sSourceFolder.GetBuffer()))
	{
		std::stringstream ss;		
		ss << p;
		LPVOID pItemData = LoadEngineJSON(ss.str().c_str());
		if( pItemData)
		{
			//m_lstEngines.AddString(sEngine.c_str());
		}		
	}
}

void CTextureInfo::LoadEngineFromJSON(LPCTSTR szFileName)
{

}

void CTextureInfo::OnChangeTextureName() 
{
	ModifiedFlag = TRUE;	
}

void CTextureInfo::OnChangeAnimname() 
{
	ModifiedFlag = TRUE;	
}

void CTextureInfo::OnChangeValue() 
{
	ModifiedFlag = TRUE;		
}


void CTextureInfo::OnAuxCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnFlowingCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnLavaCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnLightCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnMistCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnNodrawCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnSkyCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnSlickCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnSlimeCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnSolidCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnTrans33Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnTrans66Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnWarpCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnWaterCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnWindowCheck() 
{
	ModifiedFlag = TRUE;
}

void CTextureInfo::OnCurrent0Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnCurrent180Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnCurrent270Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnCurrent90Check() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnCurrentdnCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnCurrentupCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnDetailCheck() 
{
	ModifiedFlag = TRUE;		
	
}

void CTextureInfo::OnLadderCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnOriginCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnMonsterclipCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnPlayerclipCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnPortalCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnSkipCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnTranslucentCheck() 
{
	ModifiedFlag = TRUE;		
}

void CTextureInfo::OnHintCheck() 
{
	ModifiedFlag = TRUE;	
}


void CTextureInfo::OnOK() 
{
	
	UpdateData(true);
	strcpy_s (DocWalHeader->animname, sizeof(DocWalHeader->animname), (LPCSTR)m_AnimationName);
	strcpy_s (DocWalHeader->name, sizeof(DocWalHeader->name), (LPCSTR)m_TextureName);
	DocWalHeader->value = m_Value;	
		
	DocWalHeader->flags = 0;
	DocWalHeader->flags |= (m_LightFlag);
	DocWalHeader->flags |= (m_SlickFlag		<< 1);
	DocWalHeader->flags |= (m_SkyFlag		<< 2);
	DocWalHeader->flags |= (m_WarpFlag		<< 3);
	DocWalHeader->flags |= (m_Trans33Flag	<< 4);
	DocWalHeader->flags |= (m_Trans66Flag	<< 5);
	DocWalHeader->flags |= (m_FlowingFlag	<< 6);
	DocWalHeader->flags |= (m_NoDrawFlag	<< 7);
	DocWalHeader->flags |= (m_HintFlag		<< 8);
	DocWalHeader->flags |= (m_SkipFlag		<< 9);


	DocWalHeader->contents = 0;
	DocWalHeader->contents |= (m_SolidFlag);
	DocWalHeader->contents |= (m_WindowFlag			<< 1);
	DocWalHeader->contents |= (m_AuxFlag			<< 2);
	DocWalHeader->contents |= (m_LavaFlag			<< 3);
	DocWalHeader->contents |= (m_SlimeFlag			<< 4);
	DocWalHeader->contents |= (m_WaterFlag			<< 5);
	DocWalHeader->contents |= (m_MistFlag			<< 6);
	DocWalHeader->contents |= (m_AreaPortalFlag		<< 15);		
	DocWalHeader->contents |= (m_PlayerClipFlag		<< 16);
	DocWalHeader->contents |= (m_MonsterClipFlag	<< 17);
	DocWalHeader->contents |= (m_Current0Flag		<< 18);
	DocWalHeader->contents |= (m_Current90Flag		<< 19);
	DocWalHeader->contents |= (m_Current180Flag		<< 20);
	DocWalHeader->contents |= (m_Current270Flag		<< 21);
	DocWalHeader->contents |= (m_CurrentUpFlag		<< 22);
	DocWalHeader->contents |= (m_CurrentDnFlag		<< 23);
	DocWalHeader->contents |= (m_OriginFlag			<< 24);
	DocWalHeader->contents |= (m_DetailFlag			<< 27);
	DocWalHeader->contents |= (m_TranslucentFlag	<< 28);
	DocWalHeader->contents |= (m_LadderFlag			<< 29);	
	
	CDialog::OnOK();
}

