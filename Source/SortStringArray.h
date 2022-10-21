// SortStringArray.h: interface for the CSortStringArray class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SORTSTRINGARRAY_H__
#define _SORTSTRINGARRAY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSortStringArray : public CStringArray  
{
public:
	void Sort();
	int HasString (LPCTSTR szString);

private: 
	BOOL CompareAndSwap(int iPos);	
};




#endif // #ifndef _SORTSTRINGARRAY_H__
