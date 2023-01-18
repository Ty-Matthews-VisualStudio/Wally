#ifndef _2_PASS_SCALE_H_
#define _2_PASS_SCALE_H_

#include <math.h> 

#include "Filters.h" 

typedef struct 
{ 
   double *Weights;  // Normalized weights of neighboring pixels
   int Left,Right;   // Bounds of source pixels window
} ContributionType;  // Contirbution information for a single pixel

typedef struct 
{ 
   ContributionType *ContribRow; // Row (or column) of contribution weights 
   UINT WindowSize,              // Filter window size (of affecting source pixels) 
        LineLength;              // Length of line (no. or rows / cols) 
} LineContribType;               // Contribution information for an entire line (row or column)

typedef BOOL (*ProgressAnbAbortCallBack)(BYTE bPercentComplete);

class CDataCOLORREF {
public:
	typedef COLORREF _DataType;
	class _Accumulator {
	public:
	    _Accumulator () : r (0), g (0), b (0) 
	    {
	    };
	    void Accumulate (double dWeight, _DataType &value)
	    {
		r += (dWeight * (double)(GetRValue(value))); 
		g += (dWeight * (double)(GetGValue(value))); 
		b += (dWeight * (double)(GetBValue(value))); 
	    };
	    void Store (_DataType &value)
	    {
		value = RGB ((BYTE) r, (BYTE) g, (BYTE) b);
	    };
	    float r,g,b;
	};
};

class CDataRGB_UBYTE {
public:
	typedef unsigned char _DataType [3];
	class _Accumulator {
	public:
	    _Accumulator ()
	    {
		a [0] = a [1] = a [2] = 0;
	    };
	    void Accumulate (double dWeight, _DataType &value)
	    {
		a [0] += (dWeight * (double)(value [0])); 
		a [1] += (dWeight * (double)(value [1])); 
		a [2] += (dWeight * (double)(value [2]));
	    };
	    void Store (_DataType &value)
	    {
		value [0] = (unsigned char) a [0];
		value [1] = (unsigned char) a [1];
		value [2] = (unsigned char) a [2];
	    };
	    float a [3];
	};
};

class CDataRGBA_UBYTE {
public:
	typedef unsigned char _DataType [4];
	class _Accumulator {
	public:
	    _Accumulator ()
	    {
		a [0] = a [1] = a [2] = a [3] = 0;
	    };
	    void Accumulate (double dWeight, _DataType &value)
	    {
		a [0] += (dWeight * (double)(value [0])); 
		a [1] += (dWeight * (double)(value [1])); 
		a [2] += (dWeight * (double)(value [2])); 
		a [3] += (dWeight * (double)(value [3])); 
	    };
	    void Store (_DataType &value)
	    {
		value [0] = (unsigned char) a [0];
		value [1] = (unsigned char) a [1];
		value [2] = (unsigned char) a [2];
		value [3] = (unsigned char) a [3];
	    };
	    float a [4];
	};
};

/*
template <class FilterClass, class DataClass>
C2PassScale<FilterClass, DataClass>::_2PassDataType *
C2PassScale<FilterClass, DataClass>::
AllocAndScale ( 
    _2PassDataType  *pOrigImage,
    UINT        uOrigWidth, 
    UINT        uOrigHeight, 
    UINT        uNewWidth, 
    UINT        uNewHeight)
{ 
*/

template <class FilterClass, class DataClass = CDataCOLORREF>
class C2PassScale
{
public:
    //typedef DataClass::_DataType _DataType;
    typedef typename DataClass::_DataType _2PassDataType;

    C2PassScale (ProgressAnbAbortCallBack callback = NULL) : 
        m_Callback (callback) {}

    virtual ~C2PassScale() {}

    _2PassDataType* AllocAndScale (
        _2PassDataType  *pOrigImage,
        UINT	    uOrigWidth, 
        UINT	    uOrigHeight, 
        UINT        uNewWidth, 
        UINT        uNewHeight);

    _2PassDataType* Scale (
        _2PassDataType  *pOrigImage,
        UINT        uOrigWidth, 
        UINT        uOrigHeight, 
        _2PassDataType  *pDstImage,
        UINT        uNewWidth, 
        UINT        uNewHeight);

private:

    ProgressAnbAbortCallBack    m_Callback;
    BOOL                        m_bCanceled;

    LineContribType *AllocContributions (   UINT uLineLength, 
                                            UINT uWindowSize);

    void FreeContributions (LineContribType * p);

    LineContribType *CalcContributions (    UINT    uLineSize, 
                                            UINT    uSrcSize, 
                                            double  dScale);

    void ScaleRow ( _2PassDataType      *pSrc,
                    UINT                uSrcWidth,
                    _2PassDataType      *pRes,
                    UINT                uResWidth,
                    UINT                uRow, 
                    LineContribType    *Contrib);

    void HorizScale (   _2PassDataType      *pSrc,
                        UINT                uSrcWidth,
                        UINT                uSrcHeight,
                        _2PassDataType      *pDst,
                        UINT                uResWidth,
                        UINT                uResHeight);

    void ScaleCol ( _2PassDataType      *pSrc,
                    UINT                uSrcWidth,
                    _2PassDataType      *pRes,
                    UINT                uResWidth,
                    UINT                uResHeight,
                    UINT                uCol, 
                    LineContribType    *Contrib);

    void VertScale (    _2PassDataType      *pSrc,
                        UINT                uSrcWidth,
                        UINT                uSrcHeight,
                        _2PassDataType      *pDst,
                        UINT                uResWidth,
                        UINT                uResHeight);
};

template <class FilterClass, class DataClass>
LineContribType *
C2PassScale<FilterClass, DataClass>::
AllocContributions (UINT uLineLength, UINT uWindowSize)
{
    LineContribType *res = new LineContribType; 
        // Init structure header 
    res->WindowSize = uWindowSize; 
    res->LineLength = uLineLength; 
        // Allocate list of contributions 
    res->ContribRow = new ContributionType[uLineLength];
    for (UINT u = 0 ; u < uLineLength ; u++) 
    {
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new double[uWindowSize];
    }
    return res; 
} 
 
template <class FilterClass, class DataClass>
void
C2PassScale<FilterClass, DataClass>::
FreeContributions (LineContribType * p)
{ 
    for (UINT u = 0; u < p->LineLength; u++) 
    {
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header
} 
 
template <class FilterClass, class DataClass>
LineContribType *
C2PassScale<FilterClass, DataClass>::
CalcContributions (UINT uLineSize, UINT uSrcSize, double dScale)
{ 
    FilterClass CurFilter;

    double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = CurFilter.GetWidth();

    if (dScale < 1.0) 
    {    // Minification
        dWidth = dFilterWidth / dScale; 
        dFScale = dScale; 
    } 
    else
    {    // Magnification
        dWidth= dFilterWidth; 
    }
 
    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1; 
 
    // Allocate a new line contributions strucutre
    LineContribType *res = AllocContributions (uLineSize, iWindowSize); 
 
    for (UINT u = 0; u < uLineSize; u++) 
    {   // Scan through line of contributions
        double dCenter = (double)u / dScale;   // Reverse mapping
        // Find the significant edge points that affect the pixel
        int iLeft = max (0, (int)floor (dCenter - dWidth)); 
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 
 
        // Cut edge points to fit in filter window in case of spill-off
        if (iRight - iLeft + 1 > iWindowSize) 
        {
            if (iLeft < (int(uSrcSize) - 1 / 2)) 
            {
                iLeft++; 
            }
            else 
            {
                iRight--; 
            }
        }
	    //ets+++ adjusted ileft and iright values not stored in contrib array
        res->ContribRow[u].Left = iLeft;
        res->ContribRow[u].Right = iRight;
	    //ets
	    double dTotalWeight = 0.0;  // Zero sum of weights
        int iSrc = 0;
        for (iSrc = iLeft; iSrc <= iRight; iSrc++)
        {   // Calculate weights
            dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  
                                dFScale * CurFilter.Filter (dFScale * (dCenter - (double)iSrc))); 
        }
        ASSERT (dTotalWeight >= 0.0);   // An error in the filter function can cause this 
        if (dTotalWeight > 0.0)
        {   // Normalize weight of neighbouring points
            for (iSrc = iLeft; iSrc <= iRight; iSrc++)
            {   // Normalize point
                res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight; 
            }
        }
   } 
   return res; 
} 
 
 
template <class FilterClass, class DataClass>
void 
C2PassScale<FilterClass, DataClass>::
ScaleRow (  _2PassDataType      *pSrc,
            UINT                uSrcWidth,
            _2PassDataType      *pRes,
            UINT                uResWidth,
            UINT                uRow, 
            LineContribType    *Contrib)
{
    _2PassDataType *pSrcRow = &(pSrc[uRow * uSrcWidth]);
    _2PassDataType *pDstRow = &(pRes[uRow * uResWidth]);
    for (UINT x = 0; x < uResWidth; x++) 
    {   // Loop through row
	typename DataClass::_Accumulator a;
        int iLeft = Contrib->ContribRow[x].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[x].Right;  // Retrieve right boundries
        for (int i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
	    a .Accumulate (Contrib->ContribRow[x].Weights[i-iLeft], pSrcRow[i]);
        } 
	a .Store (pDstRow [x]);
    } 
} 

template <class FilterClass, class DataClass>
void
C2PassScale<FilterClass, DataClass>::
HorizScale (    _2PassDataType      *pSrc,
                UINT                uSrcWidth,
                UINT                uSrcHeight,
                _2PassDataType      *pDst,
                UINT                uResWidth,
                UINT                uResHeight)
{ 
    TRACE ("Performing horizontal scaling...\n"); 
    if (uResWidth == uSrcWidth)
    {   // No scaling required, just copy
        memcpy (pDst, pSrc, sizeof (_2PassDataType) * uSrcHeight * uSrcWidth);
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResWidth, uSrcWidth, double(uResWidth) / double(uSrcWidth)); 
    for (UINT u = 0; u < uResHeight; u++)
    {   // Step through rows
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (BYTE(double(u) / double (uResHeight) * 50.0)))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = TRUE;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
                
        ScaleRow (  pSrc, 
                    uSrcWidth,
                    pDst,
                    uResWidth,
                    u,
                    Contrib);    // Scale each row 
    }
    FreeContributions (Contrib);  // Free contributions structure
} 
 
template <class FilterClass, class DataClass>
void
C2PassScale<FilterClass, DataClass>::
ScaleCol (  _2PassDataType      *pSrc,
            UINT                uSrcWidth,
            _2PassDataType      *pRes,
            UINT                uResWidth,
            UINT                uResHeight,
            UINT                uCol, 
            LineContribType    *Contrib)
{ 
    for (UINT y = 0; y < uResHeight; y++) 
    {    // Loop through column
	    typename DataClass::_Accumulator a;
        int iLeft = Contrib->ContribRow[y].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[y].Right;  // Retrieve right boundries
        for (int i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
  	    a .Accumulate (Contrib->ContribRow[y].Weights[i-iLeft], pSrc[i * uSrcWidth + uCol]);
        }
	a .Store (pRes[y * uResWidth + uCol]);
    }
} 
 

template <class FilterClass, class DataClass>
void
C2PassScale<FilterClass, DataClass>::
VertScale (_2PassDataType *pSrc,
            UINT                uSrcWidth,
            UINT                uSrcHeight,
            _2PassDataType *pDst,
            UINT                uResWidth,
            UINT                uResHeight)
{ 
    TRACE ("Performing vertical scaling..."); 

    if (uSrcHeight == uResHeight)
    {   // No scaling required, just copy
        memcpy (pDst, pSrc, sizeof (_2PassDataType) * uSrcHeight * uSrcWidth);
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResHeight, uSrcHeight, double(uResHeight) / double(uSrcHeight)); 
    for (UINT u = 0; u < uResWidth; u++)
    {   // Step through columns
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (BYTE(double(u) / double (uResWidth) * 50.0) + 50))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = TRUE;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
        ScaleCol (  pSrc,
                    uSrcWidth,
                    pDst,
                    uResWidth,
                    uResHeight,
                    u,
                    Contrib);   // Scale each column
    }
    FreeContributions (Contrib);     // Free contributions structure
} 

template <class FilterClass, class DataClass>
typename C2PassScale<FilterClass, DataClass>::_2PassDataType *
C2PassScale<FilterClass, DataClass>::
AllocAndScale ( 
    _2PassDataType *pOrigImage,
    UINT        uOrigWidth, 
    UINT        uOrigHeight, 
    UINT        uNewWidth, 
    UINT        uNewHeight)
{ 
    // Scale source image horizontally into temporary image
    m_bCanceled = FALSE;
    _2PassDataType *pTemp = new _2PassDataType[uNewWidth * uOrigHeight];
    HorizScale (pOrigImage, 
                uOrigWidth,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uOrigHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }
    // Scale temporary image vertically into result image    
    _2PassDataType *pRes = new _2PassDataType[uNewWidth * uNewHeight];
    VertScale ( pTemp,
                uNewWidth,
                uOrigHeight,
                pRes,
                uNewWidth,
                uNewHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        delete [] pRes;
        return NULL;
    }
    delete [] pTemp;
    return pRes;
} 

template <class FilterClass, class DataClass>
typename C2PassScale<FilterClass, DataClass>::_2PassDataType *
C2PassScale<FilterClass, DataClass>::
Scale ( 
    _2PassDataType *pOrigImage,
    UINT        uOrigWidth, 
    UINT        uOrigHeight, 
    _2PassDataType *pDstImage,
    UINT        uNewWidth, 
    UINT        uNewHeight)
{ 
    // Scale source image horizontally into temporary image
    m_bCanceled = FALSE;
    _2PassDataType *pTemp = new _2PassDataType[uNewWidth * uOrigHeight];
    HorizScale (pOrigImage, 
                uOrigWidth,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uOrigHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }

    // Scale temporary image vertically into result image    
    VertScale ( pTemp,
                uNewWidth,
                uOrigHeight,
                pDstImage,
                uNewWidth,
                uNewHeight);
    delete [] pTemp;
    if (m_bCanceled)
    {
        return NULL;
    }
    return pDstImage;
} 


#endif //   _2_PASS_SCALE_H_
