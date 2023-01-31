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
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class pred
{
public:
	static bool ischar(char c)
	{
		return c == '"';
	};
};

BOOL Q2Engine::LoadJSON(LPCTSTR szFolderName)
{
	std::stringstream ssTemp;
	std::stringstream ssMsg;
	std::string sDir;
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFileName[_MAX_FNAME];
	char szExt[_MAX_EXT];

	m_sFolderName = szFolderName;
	_splitpath_s(szFolderName, szDrive, sizeof(szDrive), szDir, sizeof(szDir), szFileName, sizeof(szFileName), szExt, sizeof(szExt));
	m_sName = szFileName;
	sDir = szDir;
	boost::algorithm::trim_if(m_sName, pred::ischar);
	boost::algorithm::trim_if(sDir, pred::ischar);

	// Parse the JSON and return TRUE if it's valid, otherwise FALSE and set ErrorMessage.
	// Find flags.json and content.json, parse them separately
	ssTemp << sDir << m_sName << "\\flags.json";
	if (LoadFile(ssTemp, m_vFlags))
	{
		ssTemp.str("");
		ssTemp << sDir << m_sName << "\\content.json";
		return LoadFile(ssTemp, m_vContent);
	}
	return FALSE;
}

BOOL Q2Engine::LoadFile(std::stringstream& ssFileName, _NameBitmaskPair& vPairs)
{

	class JSONException : public std::exception
	{
	private:
		std::string m_sWhat;
	public:
		JSONException(LPCTSTR szMessage) { m_sWhat = szMessage; }
		JSONException(std::stringstream &sMessage) { m_sWhat = sMessage.str(); }
		const char* What()
		{
			return m_sWhat.c_str();
		}
	};

	class PairSort
	{
	public:
		static bool sort(std::pair< std::string, UINT > a, std::pair< std::string, UINT > b)
		{
			return a.second < b.second;
		}
	};
	
	CMemBuffer mbJSON;
	std::stringstream ssMsg;
	if (!mbJSON.InitFromFile(ssFileName))
	{		
		ssMsg << "Error loading " << ssFileName.str();
		m_sErrorMessage = ssMsg.str();
		return FALSE;
	}
	boost::json::error_code ec;

	// Validate the JSON and build a vector of std::pair<std::string,int>
	auto const jv = mbJSON.ParseJson(ec);
	if (ec)
	{
		ssMsg << "Error parsing " << ssFileName.str() << " JSON error code: " << ec;
		m_sErrorMessage = ssMsg.str();
		return FALSE;
	}
	else
	{
		std::stringstream ssTemp;
		try
		{			
			if (jv.kind() == boost::json::kind::object)
			{
				auto const& obj = jv.get_object();
				if (!obj.empty())
				{
					auto it = obj.begin();
					for (;;)
					{
						ssTemp.str("");
						ssTemp << boost::json::serialize(it->key());
						if (!_stricmp(ssTemp.str().c_str(), "\"entries\""))
						{
							if (it->value().kind() != boost::json::kind::array)
							{
								ssMsg << "Error parsing " << ssFileName.str() <<
									" : Unrecognized data type for 'entries' object," << 
									"  expected array received " << it->value().kind();
								throw JSONException(ssMsg);
							}
							else
							{
								auto const& arr = it->value().get_array();
								if (arr.empty())
								{
									ssMsg << "Error parsing " << ssFileName.str() <<
										" : 'entries' array is empty";
									throw JSONException(ssMsg);
								}
								else
								{
									auto itArr = arr.begin();
									for (int index = 0;;index++)
									{	
										std::string sName;
										unsigned int iBitmask = 0;
										
										if ((*itArr).kind() != boost::json::kind::object)
										{
											ssMsg << "Error parsing " << ssFileName.str() <<
												" : Unrecognized data type for array entry index[" << index <<
												"]  Expected object received " << (*itArr).kind();
											throw JSONException(ssMsg);
										}
										else
										{
											auto const& oEntry = (*itArr).get_object();
											if (!obj.empty())
											{
												auto itEntry = oEntry.begin();
												for (;;)
												{
													std::stringstream ssKey;
													std::stringstream ssName;
													ssKey << boost::json::serialize(itEntry->key());
													if (!_stricmp(ssKey.str().c_str(), "\"name\""))
													{
														ssName << boost::json::serialize(itEntry->value());
														sName = ssName.str();
														boost::algorithm::trim_if(sName, pred::ischar);
													}
													else
													{
														if (!_stricmp(ssKey.str().c_str(), "\"bitmask\""))
														{
															switch (itEntry->value().kind())
															{
															case boost::json::kind::uint64:
															{
																iBitmask = itEntry->value().get_uint64();
															}
															break;

															case boost::json::kind::int64:
															{
																iBitmask = itEntry->value().get_int64();
															}
															break;

															default:
															{
																ssMsg << "Error parsing " << ssFileName.str() <<
																	" : Unrecognized bitmask data type for entry with name '" << sName.c_str() <<
																	"'  Received kind " << itEntry->value().kind();
																throw JSONException(ssMsg);
															}
															break;
															}

															// Do some bounds checking on the bitmask															
															if (ceil(log2(iBitmask)) != floor(log2(iBitmask)))
															{
																ssMsg << "Error parsing " << ssFileName.str() <<
																	" : Bitmask with name '" << sName.c_str() <<
																	"' is not a power of 2";
																throw JSONException(ssMsg);
															}

															if ((iBitmask <= 0) || (iBitmask > pow(2,32)))
															{
																ssMsg << "Error parsing " << ssFileName.str() <<
																	" : Bitmask with name '" << sName.c_str() <<
																	"' must be >0 and <=" << std::fixed << std::setprecision(0) << pow(2,32);
																throw JSONException(ssMsg);
															}
														}
														else
														{
															ssMsg << "Error parsing " << ssFileName.str() <<
																" : Unrecognized key/value pair <" << boost::json::serialize(itEntry->key()) << 
																" :  " << boost::json::serialize(itEntry->value()) << ">";
															throw JSONException(ssMsg);
														}
													}													
													if (++itEntry == oEntry.end())
														break;												
												}
											}
										}
										
										if (sName.size() == 0 || iBitmask == 0)
										{
											ssMsg << "Error parsing " << ssFileName.str() <<
												" : Malformed array entry with index[" << index << "]";												
											throw JSONException(ssMsg);
										}
										else
										{
											// Make sure this bitmask isn't already in the vector
											for (std::pair< std::string, UINT > p : vPairs)
											{
												if (p.second == iBitmask)
												{
													ssMsg << "Error parsing " << ssFileName.str() <<
														" : Entry with name '" << sName << "' has a duplicate bitmask ('" << p.first << "')";
													throw JSONException(ssMsg);
												}
											}
											vPairs.push_back(std::make_pair(sName, iBitmask));
										}										
										if (++itArr == arr.end())
											break;
									}
								}
							}
						}
						if (++it == obj.end())
							break;
					}
				}
			}
		}
		catch (JSONException e)
		{
			m_sErrorMessage = e.What();
			return FALSE;
		}
	}

	// Sort by bitmask value
	std::sort(vPairs.begin(), vPairs.end(), PairSort::sort);
	return TRUE;
}

/*static */ BOOL Q2Engine::CreateDefaultJSON()
{
	CMemBuffer mbData;
	std::stringstream ssFolder;
	std::stringstream ssTemp;
	std::stringstream ssFile;
	
	ssTemp << g_strJSONDirectory.GetBuffer() << "\\.wal";
	_mkdir(ssTemp.str().c_str());
	ssTemp << "\\baseq2";
	_mkdir(ssTemp.str().c_str());

	ssFile << ssTemp.str() << "\\flags.json";	
	if (mbData.InitFromResource(IDR_DEFAULT_Q2FLAGS_JSON))
	{
		mbData.WriteToFile(ssFile.str().c_str());
		ssFile.str("");
		ssFile << ssTemp.str() << "\\content.json";
		if (mbData.InitFromResource(IDR_DEFAULT_Q2CONTENTS_JSON))
		{
			mbData.WriteToFile(ssFile.str().c_str());
			return TRUE;
		}
	}
	return FALSE;
}


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
	//}}AFX_DATA_INIT
	ModifiedFlag = FALSE;
}

CTextureInfo::~CTextureInfo()
{
	CleanUp();
}

void CTextureInfo::CleanUp()
{
	for (Q2Engine* p : m_vEngines)
	{
		if( p ) delete p;
	}
	m_vEngines.erase(m_vEngines.begin(), m_vEngines.end());
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
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TEXTUREINFO_Q2ENGINE, m_cbEngines);
}


BEGIN_MESSAGE_MAP(CTextureInfo, CDialog)
	//{{AFX_MSG_MAP(CTextureInfo)
	ON_EN_CHANGE(IDC_TEXTURE_NAME, OnChangeTextureName)
	ON_EN_CHANGE(IDC_ANIMNAME, OnChangeAnimname)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)		
	ON_CBN_SELCHANGE(IDC_TEXTUREINFO_Q2ENGINE, &CTextureInfo::OnCbnSelchangeTextureinfoQ2engine)
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextureInfo message handlers
BOOL CTextureInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadJSON();	
	return true;
}

void CTextureInfo::EnableDisableFlagsContent()
{
	int i = m_cbEngines.GetCurSel();
	Q2Engine* pEngine = reinterpret_cast<Q2Engine *>(m_cbEngines.GetItemData(i));

	m_mID2Flags.erase(m_mID2Flags.begin(), m_mID2Flags.end());
		
	UINT iFirstID = IDC_FLAGS_0X1;
	UINT iLastID = IDC_FLAGS_0X80000000;
	UINT iID = IDC_FLAGS_0X1;

	ASSERT(iFirstID == 1388);	// If this ASSERT fails, it's possible resource.h was fudged by Visual Studio.  These checkbox IDs need to be sequential for this code to not be a horrible mess

	for (iID = iFirstID; iID <= iLastID; iID++)
	{
		CWnd* pCheck = GetDlgItem(iID);
		pCheck->ShowWindow(SW_HIDE);
	}
	iID = iFirstID;
	for (std::pair< std::string, UINT > p : pEngine->m_vFlags)
	{
		CWnd* pCheck = GetDlgItem(iID);
		pCheck->ShowWindow(SW_SHOWNORMAL);
		SetDlgItemTextA(iID, p.first.c_str());
		CheckDlgButton(iID, m_Q2Header.flags & p.second);

		m_mID2Flags[iID] = p.second;
		iID++;
	}

	iFirstID = IDC_CONTENT_0X1;
	iLastID = IDC_CONTENT_0X80000000;
	iID = IDC_CONTENT_0X1;

	ASSERT(iFirstID == 1420);	// If this ASSERT fails, it's possible resource.h was fudged by Visual Studio.  These checkbox IDs need to be sequential for this code to not be a horrible mess

	for (iID = iFirstID; iID <= iLastID; iID++)
	{
		CWnd* pCheck = GetDlgItem(iID);
		pCheck->ShowWindow(SW_HIDE);
	}
	iID = iFirstID;
	for (std::pair< std::string, UINT > p : pEngine->m_vContent)
	{
		CWnd* pCheck = GetDlgItem(iID);
		pCheck->ShowWindow(SW_SHOWNORMAL);
		SetDlgItemTextA(iID, p.first.c_str());
		CheckDlgButton(iID, m_Q2Header.contents & p.second);

		m_mID2Contents[iID] = p.second;
		iID++;
	}

}

void CTextureInfo::Init (LPQ2_MIP_S WalHeader)
{
	char buffer[50];
	DocWalHeader = WalHeader;
	
	memcpy((void*)&m_Q2Header, WalHeader, sizeof(q2_miptex_s));

	m_TextureName = WalHeader->name;	
	
	_itoa_s (WalHeader->width, buffer, sizeof(buffer), 10);	
	m_TextureWidth = buffer;
	
	_itoa_s (WalHeader->height, buffer, sizeof(buffer), 10);
	m_TextureHeight = buffer;
	
	m_AnimationName = WalHeader->animname;
	m_Value = WalHeader->value;	
}

void CTextureInfo::LoadJSON()
{
	CString sSourceFolder;
	sSourceFolder.Format("%s\\.wal\\", g_strJSONDirectory.GetBuffer());
	bool bOnce = false;
	bool bSelected = false;
	bool bValid = false;
	CleanUp(); // Erase any existing.  This function should only ever be called once, but just in case a refresh button is added later
	
	for (auto& p : boost::filesystem::directory_iterator(sSourceFolder.GetBuffer()))
	{
		std::stringstream ss;		
		ss << p;
		Q2Engine* pNew = new Q2Engine();
		if (pNew)
		{
			if (pNew->LoadJSON(ss.str().c_str()))
			{
				m_vEngines.push_back(pNew);
				int i = m_cbEngines.AddString(pNew->m_sName.c_str());
				m_cbEngines.SetItemDataPtr(i, (void*)pNew);
				bValid = true;

				if (!_stricmp(pNew->m_sName.c_str(), g_strDefaultTexInfoQ2Engine.GetBuffer()))
				{
					m_cbEngines.SetCurSel(i);
					EnableDisableFlagsContent();
					bSelected = true;
				}
			}
			else
			{
				if (!bOnce)
				{
					// We'll ignore any additional failures, correct them one-by-one
					AfxMessageBox(pNew->m_sErrorMessage.c_str(), MB_ICONSTOP);
					bOnce = true;
				}				
				delete pNew;				
			}
		}
		else
		{
			ASSERT(FALSE);
		}		
	}

	if (!bSelected)
	{
		if (m_cbEngines.GetCount() > 0)
		{
			m_cbEngines.SetCurSel(0);
			EnableDisableFlagsContent();
		}		
	}

	if (!bValid)
	{
		// No valid JSON was found, create the default and try again		
		if( Q2Engine::CreateDefaultJSON() )
		{
			return LoadJSON();
		}
		else
		{
			// Couldn't make the default flags and content.json for some reason?
			ASSERT(FALSE);
		}
	}
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

void CTextureInfo::OnOK() 
{
	
	UpdateData(true);
	strcpy_s (DocWalHeader->animname, sizeof(DocWalHeader->animname), (LPCSTR)m_AnimationName);
	strcpy_s (DocWalHeader->name, sizeof(DocWalHeader->name), (LPCSTR)m_TextureName);
	DocWalHeader->value = m_Value;	
		
#if 1
	if (DocWalHeader->flags != m_Q2Header.flags)
	{
		ModifiedFlag = TRUE;
		DocWalHeader->flags = m_Q2Header.flags;
	}

	if (DocWalHeader->contents != m_Q2Header.contents)
	{
		ModifiedFlag = TRUE;
		DocWalHeader->contents = m_Q2Header.contents;
	}


#else
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
#endif

	int i = m_cbEngines.GetCurSel();
	Q2Engine* pSel = reinterpret_cast<Q2Engine * >(m_cbEngines.GetItemData(i));
	if (pSel)
	{
		g_strDefaultTexInfoQ2Engine = pSel->m_sName.c_str();
	}
	
	CDialog::OnOK();
}



void CTextureInfo::OnCbnSelchangeTextureinfoQ2engine()
{
	EnableDisableFlagsContent();
}

BOOL CTextureInfo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_COMMAND)
	{
		UINT iID = LOWORD(pMsg->wParam);
		UINT iStatus = HIWORD(pMsg->wParam);
	}
	int i = 0;
	return CDialog::PreTranslateMessage(pMsg);	
}

BOOL CTextureInfo::OnCommand(WPARAM w, LPARAM l)
{
	UINT iID = LOWORD(w);
	UINT iStatus = 0;	
	if ((iID >= IDC_FLAGS_0X1) && (iID <= IDC_FLAGS_0X80000000))
	{
		// Find the corresponding bitmask from the map
		UINT iBit = m_mID2Flags[iID];
		iStatus = IsDlgButtonChecked(iID);		
		if (iStatus)
		{
			m_Q2Header.flags |= iBit;
		}
		else
		{
			m_Q2Header.flags &= ~(iBit);
		}		
	}
	if ((iID >= IDC_CONTENT_0X1) && (iID <= IDC_CONTENT_0X80000000))
	{
		// Find the corresponding bitmask from the map
		UINT iBit = m_mID2Contents[iID];
		iStatus = IsDlgButtonChecked(iID);
		if (iStatus)
		{
			m_Q2Header.contents |= iBit;
		}
		else
		{
			m_Q2Header.contents &= ~(iBit);
		}
	}
	return CDialog::OnCommand(w, l);
}