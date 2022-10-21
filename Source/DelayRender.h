///////////////////////////////////////////////////////////////////////////////
// DelayRender.h: interface for the CDelayRender & CDelayRenderView classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DELAYRENDER_H_
#define _DELAYRENDER_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDelayRenderView
{
public:
	virtual HGLOBAL RenderData();
};

class CDelayRender : public COleDataSource
{
private:
	CDelayRenderView *m_pView;
	
public:
	CDelayRender(CDelayRenderView *pView);	
	~CDelayRender();	
		
	BOOL OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
	BOOL DelayRenderHDROP ( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
	BOOL DelayRenderDROPEFFECT ( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium );
	virtual DROPEFFECT DoDragDrop(DWORD dwEffects = DROPEFFECT_COPY, LPCRECT lpRectStartDrag = NULL, COleDropSource* pDropSource = NULL);
	HGLOBAL RenderHDROP();
	HGLOBAL RenderDROPEFFECT();
	void HDROPFormat();
	void DROPEFFECT_Format();	
	void SetClipboard();
};


#endif // #ifndef _DELAYRENDER_H_
