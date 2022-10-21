// RuleList.cpp: implementation of the CRuleItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wally.h"
#include "RuleList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRuleItem::CRuleItem()
{
	SetNext (NULL);
	SetPrevious (NULL);
	SetOutputDirectory ("");
	SetMatch ("");
}

CRuleItem::~CRuleItem()
{

}

void CRuleItem::SetNext (CRuleItem *pNext)
{
	m_pNext = pNext;
}

CRuleItem *CRuleItem::GetNext ()
{
	return m_pNext;
}

void CRuleItem::SetPrevious (CRuleItem *pPrevious)
{
	m_pPrevious = pPrevious;
}

CRuleItem *CRuleItem::GetPrevious()
{
	return m_pPrevious;
}

CString CRuleItem::GetOutputDirectory()
{
	return m_strOutputDirectory;
}

void CRuleItem::SetOutputDirectory (LPCTSTR szDirectory)
{
	m_strOutputDirectory = szDirectory;
}

CString CRuleItem::GetMatch()
{
	return m_strMatch;
}

void CRuleItem::SetMatch (LPCTSTR szMatch)
{
	m_strMatch = szMatch;
}

void CRuleItem::AddOption (int iOption)
{
	m_iOptions |= iOption;
}

void CRuleItem::RemoveOption (int iOption)
{
	m_iOptions ^= iOption;
}

int CRuleItem::GetOptions()
{
	return m_iOptions;
}

LPSIN_MIP_S CRuleItem::GetSinHeader()
{
	return &m_SinHeader;
}

void CRuleItem::SetSinHeader (LPSIN_MIP_S lpSinHeader /* = NULL */)
{
	if (!lpSinHeader)
	{
		memset (&m_SinHeader, 0, SIN_HEADER_SIZE);
	}
	else
	{
		memcpy (&m_SinHeader, lpSinHeader, SIN_HEADER_SIZE);
	}
}

LPSIN_RULE_S CRuleItem::GetSinRuleHeader()
{
	return &m_SinRuleHeader;
}

void CRuleItem::SetSinRuleHeader (LPSIN_RULE_S lpSinRuleHeader /* = NULL */)
{
	if (!lpSinRuleHeader)
	{
		memset (&m_SinRuleHeader, 0, SINRULE_HEADER_SIZE);
	}
	else
	{
		memcpy (&m_SinRuleHeader, lpSinRuleHeader, SINRULE_HEADER_SIZE);
	}
}

LPQ2_MIP_S CRuleItem::GetQ2Header()
{
	return &m_Q2Header;
}

void CRuleItem::SetQ2Header (LPQ2_MIP_S lpQ2Header /* = NULL */) 
{
	if (!lpQ2Header)
	{
		memset (&m_Q2Header, 0, Q2_HEADER_SIZE);
	}
	else
	{
		memcpy (&m_Q2Header, lpQ2Header, Q2_HEADER_SIZE);
	}
}

LPQ2_RULE_S CRuleItem::GetQ2RuleHeader()
{
	return &m_Q2RuleHeader;
}

void CRuleItem::SetQ2RuleHeader (LPQ2_RULE_S lpQ2RuleHeader /* = NULL */)
{
	if (!lpQ2RuleHeader)
	{
		memset (&m_Q2RuleHeader, 0, Q2RULE_HEADER_SIZE);
	}
	else
	{
		memcpy (&m_Q2RuleHeader, lpQ2RuleHeader, Q2RULE_HEADER_SIZE);
	}
}

//////////////////////////////////////////////////////////////////////
// CRuleList Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRuleList::CRuleList()
{

}

CRuleList::~CRuleList()
{

}
