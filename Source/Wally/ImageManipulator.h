// ImageManipulator.h: interface for the CImageManipulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEMANIPULATOR_H__4795FB46_4CC0_495B_9A7C_C6037C963463__INCLUDED_)
#define AFX_IMAGEMANIPULATOR_H__4795FB46_4CC0_495B_9A7C_C6037C963463__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImageManipulator  
{
private:
	CMemBuffer m_mbIRGBData;
	CMemBuffer m_mbSourceData;

	DWORD m_dwMaxWidth;
	DWORD m_dwMinWidth;
	DWORD m_dwMaxHeight;
	DWORD m_dwMinHeight;
	DWORD m_dwMaxSize;
	DWORD m_dwMinSize;
	
public:
	CImageManipulator();
	virtual ~CImageManipulator();

public:
	typedef enum 
	{
		IMExtentMaxWidth,
		IMExtentMinWidth,
		IMExtentMaxHeight,
		IMExtentMinHeight,
		IMExtentMaxSize,
		IMExtentMinSize
	} eImageManipulatorExtent;

	void LoadFromClipboard();
	void LoadFromFile( LPCTSTR szFileName );
	void SetExtents( eImageManipulatorExtent eExtent, DWORD dwExtent );

};

#endif // !defined(AFX_IMAGEMANIPULATOR_H__4795FB46_4CC0_495B_9A7C_C6037C963463__INCLUDED_)
