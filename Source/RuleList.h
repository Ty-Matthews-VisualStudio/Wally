// RuleList.h: interface for the CRuleItem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RULELIST_H_
#define _RULELIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _IMAGE_HELPER_H_
	#include "ImageHelper.h"
#endif

#define RULE_OPTION_STRIP				0x1
#define RULE_OPTION_OUTPUT_BATCH		0x2
#define RULE_OPTION_OUTPUT_ALTERNATE	0x4

class CRuleItem  
{
public:
	CRuleItem();
	virtual ~CRuleItem();

	LPSIN_MIP_S GetSinHeader();	
	void SetSinHeader (LPSIN_MIP_S lpSinHeader = NULL);
	LPSIN_RULE_S GetSinRuleHeader();
	void SetSinRuleHeader (LPSIN_RULE_S lpSinRuleHeader = NULL);
	

	LPQ2_MIP_S GetQ2Header();	
	void SetQ2Header (LPQ2_MIP_S lpQ2Header = NULL);
	LPQ2_RULE_S GetQ2RuleHeader();	
	void SetQ2RuleHeader (LPQ2_RULE_S lpQ2RuleHeader = NULL);

	void SetNext (CRuleItem *pNext);
	CRuleItem *GetNext ();
	void SetPrevious (CRuleItem *pPrevious);
	CRuleItem *GetPrevious();

	CString GetOutputDirectory();
	void SetOutputDirectory (LPCTSTR szDirectory);
	CString GetMatch();
	void SetMatch (LPCTSTR szMatch);

	void AddOption (int iOption);
	void RemoveOption (int iOption);
	int GetOptions();

private:
	SIN_MIP_S m_SinHeader;
	SIN_RULE_S m_SinRuleHeader;
	Q2_MIP_S m_Q2Header;
	Q2_RULE_S m_Q2RuleHeader;

	CRuleItem *m_pNext;
	CRuleItem *m_pPrevious;

	CString m_strMatch;
	CString m_strDescription;
	CString m_strOutputDirectory;

	int m_iGameType;
	int m_iOptions;
};


class CRuleList  
{
public:
	CRuleList();
	virtual ~CRuleList();

};

#endif // #ifndef _RULELIST_H_
