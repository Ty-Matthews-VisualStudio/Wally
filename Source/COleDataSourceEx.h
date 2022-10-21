
/**************************************************************************
* @Doc
* @Module		COleDataSourceEx.h
* |				This module contains classes intended to 
*				simplify sourcing and sinking clipboard transfers.
* @Ver      	1.0
* @Modified		Aug. 26, 1998 - Keith Rule
**************************************************************************/


#ifndef __COLEDATASOURCEEX_H_
#define __COLEDATASOURCEEX_H_


// &&VCASECustomHeader
#include <afxole.h>
#include <afxadv.h>
#include <afxtempl.h>

const long FORMAT_EDITABLE = (1 << 1);
const long FORMAT_SET = (1 << 0);

const long FORMAT_ENABLED = FORMAT_EDITABLE | FORMAT_SET;
const long FORMAT_DISABLED = FORMAT_EDITABLE;

// &&VCASECustomHeaderEnd


// Forward Declarations
class COleDataSourceEx;
class COleDataSourceView;
class COleDropBase;
class COleDropTargetEx;



///////////////////////////////////////////////////////////////////////////
// @Class		COleDataSourceEx
//
// @Resp		This class simplifies the usage of COleDataSource by 
//				routing calls to any class derived from COleDataSourceView.
//				This class also manages the most common types of data transferred
//				for both immediate and delayed rendering types of transfers. 
//
// @Cat			COleDataSourceEx
//
// @Base		Public | COleDataSource
class COleDataSourceEx : public COleDataSource
{


// Constructor(s) & destructor
public:
	COleDataSourceEx(COleDataSourceView* view = NULL, BOOL bDelayRendered = TRUE);
	virtual ~COleDataSourceEx();


// ClassWizard complient virtual overrides
	//{{AFX_VIRTUAL(COleDataSourceEx)
	//}}AFX_VIRTUAL


// CObject Debug functions overrides
	#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	#endif // _DEBUG


// Attributes
// @Access public attributes
public:


// @Access protected attributes
protected:
	// @cmember Points to COleDataSourceView instance passed in the construtor
	COleDataSourceView* m_pView;

	// @cmember Holds current bitmap (used when rendering a DIB to the clipboard)
	CBitmap*			m_bitmap;

	// @cmember Passed in the constructor. TRUE means clipboard is delay rendered, FALSE means clipboard is rendered immediately.
	BOOL				m_bDelayRendered;

	// @cmember Stored supported custom format IDs.
	CArray<UINT, UINT&>	m_customFormat;


// @Access private attributes
private:



// Operations
// @Access public operations
public:
	// @cmember Overrides same function in COleDataSource. It's used to route delay rendered calls to non-textual formats.
	virtual BOOL COleDataSourceEx::OnRenderData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);
	
	// @cmember Overrides same functin in COleDataSource. It's used to route delay rendered calls to textual formats.
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
	
	// @cmember Renders DIB format by calling DoDraw() in the view and placing the resulting bitmap onto the clipboard as a DIB.
	virtual BOOL DelayRenderDIB(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium);

	// @cmember Initiates the begin of a Drag/Drop.
	virtual DROPEFFECT DoDragDrop(DWORD dwEffects = DROPEFFECT_COPY|DROPEFFECT_MOVE|DROPEFFECT_LINK, LPCRECT lpRectStartDrag = NULL, COleDropSource* pDropSource = NULL);

	// @cmember Generically renders the bitmap. Used by both immediate and delayed rendering.
	HGLOBAL RenderDIB();

	// @cmember Removes any storage left after a SetClipboard or DoDragDrop operation.
	virtual void Cleanup();

	// @cmember Adds custom format ID to the custom ID list.
	virtual void EnableCustomFormat(UINT format);

	// @cmember Places Custom formats on the Clipboard using either Immediate or Delayed rendering as defined in the constructor.
	virtual void CustomFormat();

	// @cmember Places CSV format on the Clipboard using either Immediate or Delayed rendering as defined in the constructor.
	virtual void CsvFormat();

	// @cmember Places DIB format on the Clipboard using either Immediate or Delayed rendering as defined in the constructor.
	virtual void DIBFormat();

	// @cmember Places all the enabled, known format on the clipboard.
	virtual void InstantiateFormats();

	// @cmember Places RTF format on the Clipboard using either Immediate or Delayed rendering as defined in the constructor.
	virtual void RtfFormat();

	// @cmember Places the specified formats on the clipboard as a result of either a Copy or Cut operation. Please note, that operation should be done using immediate rendering.
	virtual void SetClipboard();

	// @cmember Places CF_TEXT format on the Clipboard using either Immediate or Delayed rendering as defined in the constructor.
	virtual void TextFormat();



// @Access protected operations
protected:


// @Access private operations
private:


};
///////////////////////////////////////////////////////////////////////////







///////////////////////////////////////////////////////////////////////////
// @Class		COleDataSourceView
//
// @Resp		This class is intended to be used as a mix-in for a CView 
//				derived class (or any other class that needs to respond to 
//				clipboard rendering requests).
//
// @Cat			COleDataSourceEx
class COleDataSourceView
{


// Constructor(s) & destructor
public:
	COleDataSourceView();
	virtual ~COleDataSourceView();


// Attributes
// @Access public attributes
public:


// @Access protected attributes
protected:


// @Access private attributes
private:
	// @cmember A bit-field used to specify whether 
	//			the CSV format is enabled and whether 
	//			the state can be changed at runtime.
	int              m_csvFormat;

	// @cmember A bit-field used to specify whether 
	//			the CF_TEXT format is enabled and whether 
	//			the state can be changed at runtime.
	int              m_textFormat;

	// @cmember A bit-field used to specify whether 
	//			the Rich Text format is enabled and whether 
	//			the state can be changed at runtime.
	int              m_rtfFormat;

	// @cmember A bit-field used to specify whether 
	//			the Device Independent Bitmap format 
	//			is enabled and whether 
	//			the state can be changed at runtime.
	int              m_DIBFormat;


// Operations
// @Access public operations
public:

	// @cmember Returns the bounds of the client area to be renders. By default 
	//			this value returns the entire client area. Override this method
	//			to return other values.
	virtual CRect GetBounds();

	// @cmember This method returns a pointer to the CView class associated with this instance.
	//			This method is intended to be overriden by the derived class, so
	//			will always return NULL.
	virtual CView* GetView();

	// @cmember Returns the status of the CSV format for this view. This returned value is a bit-fields. 
	//			The legal values are FORMAT_SET which means that this format is enabled, and FORMAT_EDITABLE 
	//			which means this status can be changed.
	virtual int CsvStatus();

	// @cmember Returns the status of the DIB format for this view. This returned value is a bit-fields. 
	//			The legal values are FORMAT_SET which means that this format is enabled, and FORMAT_EDITABLE 
	//			which means this status can be changed.
	virtual int DIBStatus();

	// @cmember Returns the status of the RTF format for this view. This returned value is a bit-fields. 
	//			The legal values are FORMAT_SET which means that this format is enabled, and FORMAT_EDITABLE 
	//			which means this status can be changed.
	virtual int RtfStatus();

	// @cmember Returns the status of the CF_TEXT format for this view. This returned value is a bit-fields. 
	//			The legal values are FORMAT_SET which means that this format is enabled, and FORMAT_EDITABLE 
	//			which means this status can be changed.
	virtual int TextStatus();

	// @cmember This method routes this call to the OnDraw functions associated with the view. This is an unfortunate
	//			case where the MFC team shouldn't have declared a function as protected.
	virtual void DoDraw(CDC* pDC);

		// @cmember This method routes this call to the OnPrepareDC functions associated with the view. This is an unfortunate
	//			case where the MFC team shouldn't have declared a function as protected.
	virtual void DoPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	// @cmember Sets the CSV format status value. The value is a bit-field the bit values are FORMAT_SET and FORMAT_EDITABLE.
	//			There are two short-hand forms of the, FORMAT_ENABLED and FORMAT_DISABLED. You probably want to use one of the short-hand forms.
	virtual void EnableCsvFormat(int status = FORMAT_ENABLED);

	// @cmember Sets the DIB format status value. The value is a bit-field the bit values are FORMAT_SET and FORMAT_EDITABLE.
	//			There are two short-hand forms of the, FORMAT_ENABLED and FORMAT_DISABLED. You probably want to use one of the short-hand forms.
	virtual void EnableDIBFormat(int status = FORMAT_ENABLED);

	// @cmember Sets the RTF format status value. The value is a bit-field the bit values are FORMAT_SET and FORMAT_EDITABLE.
	//			There are two short-hand forms of the, FORMAT_ENABLED and FORMAT_DISABLED. You probably want to use one of the short-hand forms.
	virtual void EnableRtfFormat(int status = FORMAT_ENABLED);

	// @cmember Sets the CF_TEXT format status value. The value is a bit-field the bit values are FORMAT_SET and FORMAT_EDITABLE.
	//			There are two short-hand forms of the, FORMAT_ENABLED and FORMAT_DISABLED. You probably want to use one of the short-hand forms.
	virtual void EnableTextFormat(int status = FORMAT_ENABLED);

	// @cmember This method receives the requests to write the CSV format. This function is intended to be overridden by the derived CView class.
	virtual BOOL WriteCsvFormat(CFile* pFile);

	// @cmember This method receives the requests to write the RTF format. This function is intended to be overridden by the derived CView class.
	virtual BOOL WriteRtfFormat(CFile* pFile);

	// @cmember This method receives the requests to write the CF_TEXT format. This function is intended to be overridden by the derived CView class.
	virtual BOOL WriteTextFormat(CFile* pFile);
	
	// @cmember This method receives the requests to write the developer defined format. This function is intended to be overridden by the derived CView class.
	virtual BOOL WriteCustomFormat(LPFORMATETC lpFormat, CFile* file);



// @Access protected operations
protected:


// @Access private operations
private:


};
///////////////////////////////////////////////////////////////////////////

#endif  // __COLEDATASOURCEEX_H_
