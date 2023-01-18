// ColorOpt.h

#ifndef _COLOROPT_H_
#define _COLOROPT_H_

/////////////////////////////////////////////////////////////////////////////

// Neal - both of these should now be switched on, I've hooked up auto
// detect code that will select between them as appropriate.

// Neal - OLD_METHOD is the fast method that drops the lower 3 bits 
// of each R,G,or B color component
#define OLD_METHOD     1

// Neal - BETTER_METHOD is the slow method that does not drop any bits
#define BETTER_METHOD  0

/////////////////////////////////////////////////////////////////////////////

#if OLD_METHOD
typedef struct
{
	UINT uVariance;        /* weighted variance */
	UINT uTotalWeight;     /* total weight */
	UINT tt_sum;           /* tt_sum += r*r+g*g+b*b*weight over entire box */
	UINT t_ur;             /* t_ur += r*weight over entire box */
	UINT t_ug;             /* t_ug += g*weight over entire box */
	UINT t_ub;             /* t_ub += b*weight over entire box */
	int  ir, ig, ib;       /* upper and lower bounds */
	int  jr, jg, jb;
} box;
#endif


class CColorNode;
class CBetterBox;
class CProgressBar;


class CColorOptimizer
{
// Members
private:
#if OLD_METHOD
	box** pHeap;      /* priority queue */
	UINT  uHeapSize;
	box*  pBoxes;     /* box list */
	int   iNumBoxes;
	UINT* puHist;     /* histogram */
#endif

// Methods
public:
	CColorOptimizer();
	~CColorOptimizer();

	void* SmartAlloc( UINT uSize);
	int  Optimize( COLOR_IRGB* irgbBuffer, int iSizeX, int iSizeY, 
			BYTE* pbyPalette, int iMaxColors, BOOL bShowProgrssbar);

#if OLD_METHOD
	BOOL Quantize( UINT* uHistogram, int iMaxColors, BYTE* byColorMap, int* iNumColors);

	void ShrinkBox( int ir, int ig, int ib,
					int jr, int jg, int jb,
                    int *lr, int *lg, int *lb,
                    int *hr, int *hg, int *hb);
	void down_heap(void);
	void insert_heap(box *p);
	void Sum( int ir, int ig, int ib,
		int jr, int jg, int jb,
		UINT *uTotalWeight,
		UINT *tt_sum, UINT *t_ur, UINT *t_ug, UINT *t_ub);
	void SplitBox( box* pOldBox);
	void MakeColorMap( BYTE* pbyColorMap);
	BOOL Initialize( int colors);
#endif

#if BETTER_METHOD
	CBetterBox*  m_pBetterBoxes;
	int          m_iNumBoxes;
	CColorNode*  m_pBetterHistNodeList;     /* histogram */

	BOOL        BetterQuantize( CColorNode* pNodeList, int iMaxColors, 
							BYTE* pbyPackedPalette, int* piNumColors,
							CProgressBar* pctlProgress);
	BOOL        BetterInitialize( int colors);
	void        BetterMakeColorMap( BYTE* pbyColorMap);
	CBetterBox* BetterWorstBox( void);
#endif

#if OLD_METHOD
/*----------------------------------------------------------------------------*/
/* Returns "worst" box, or NULL if no more splittable boxes remain. The worst */
/* box is the box with the largest variance.                                  */
/*----------------------------------------------------------------------------*/
	inline box *worst_box(void)
	{
		if (uHeapSize == 0)
			return NULL;
		else
			return pHeap[1];
	}
#endif
};

//////////////////////////////////////////////////////////////////////////////
// Name:	Variance
// Action:	Determines the variance of an individual point in a color box
//////////////////////////////////////////////////////////////////////////////
inline UINT Variance( UINT tw, UINT tt_sum,
				  UINT t_ur, UINT t_ug, UINT t_ub)
{
	double temp;

	// the following calculations can be performed in fixed point
	// if needed - just be sure to preserve enough precision!

	temp  = (double )t_ur * (double )t_ur;
	temp += (double )t_ug * (double )t_ug;
	temp += (double )t_ub * (double )t_ub;
	temp /= (double )tw;

	return ((UINT )((double )tt_sum - temp));
}

#if BETTER_METHOD

	#define SPLIT_RED	0
	#define SPLIT_GREEN	1
	#define SPLIT_BLUE	2
	#define NIL_NODE	0x00FFFFFF

	class CColorNode
	{
	protected:				// neal - data is packed - we alloc these by the thousands
		DWORD m_dwColor;	// bottom 24 bits is RGB, top 8 are LOWER count byte
		DWORD m_dwNext;		// bottom 24 bits is NEXT INDEX, top 8 are UPPER count byte

	public:
		CColorNode() { m_dwColor = 0; m_dwNext = NIL_NODE; };

		COLORREF GetRGB() { return (m_dwColor & 0xFFFFFF); };
		void     SetRGB( COLORREF rgbColor)
				{
					m_dwColor = (m_dwColor & 0xFF000000) + (rgbColor & 0x00FFFFFF);
				};
		UINT     GetR()   { return GetRValue( m_dwColor); };
		UINT     GetG()   { return GetGValue( m_dwColor); };
		UINT     GetB()   { return GetBValue( m_dwColor); };

		UINT     GetCount() { return ((m_dwNext >> 16) & 0xFF00) | (m_dwColor >> 24); };
		void     SetCount( UINT uCount)
				{ 
					uCount = min( uCount, 0xFFFF);
					m_dwNext  = (m_dwNext & 0x00FFFFFF)  | ((uCount >> 8) << 24);
					m_dwColor = (m_dwColor & 0x00FFFFFF) | (uCount        << 24);
				};
		void     IncCount() { SetCount( GetCount() + 1); };

		UINT     GetNextIndex() { return (m_dwNext & 0xFFFFFF); };
		void     SetNextIndex( UINT uNext)
				{
					ASSERT( uNext <= NIL_NODE); 
					m_dwNext = (m_dwNext & 0xFF000000) + uNext;
				};
	};

	class CBetterBox
	{
	public:
		UINT m_uVariance;						// weighted variance
		UINT m_uTotalWeight;					// total weight
		UINT m_uSum;							// tt_sum += r*r+g*g+b*b*weight over entire box
		UINT m_uR;								// t_ur += r*weight over entire box
		UINT m_uG;								// t_ug += g*weight over entire box
		UINT m_uB;								// t_ub += b*weight over entire box
		UINT m_uUpperR, m_uUpperG, m_uUpperB;	// upper and lower bounds
		UINT m_uLowerR, m_uLowerG, m_uLowerB;
		UINT m_uNodeIndex;
		CColorOptimizer* m_pColorOpt;

		CBetterBox();
		void Init( void);

		void BetterShrinkBox( void);
		void BetterCalcSum( void);
		UINT FindBestSplit( int iSplit, UINT* uVariance) const;
		void BetterSplitBox( void);
		void AddNode( UINT uNodeIndex);

		const CBetterBox& operator=( const CBetterBox& Box)
		{
			m_uVariance    = Box.m_uVariance;
			m_uTotalWeight = Box.m_uTotalWeight;
			m_uSum         = Box.m_uSum;
			m_uR           = Box.m_uR;
			m_uG           = Box.m_uG;
			m_uB           = Box.m_uB;
			m_uUpperR      = Box.m_uUpperR;
			m_uUpperG      = Box.m_uUpperG;
			m_uUpperB      = Box.m_uUpperB;
			m_uLowerR      = Box.m_uLowerR;
			m_uLowerG      = Box.m_uLowerG;
			m_uLowerB      = Box.m_uLowerB;
			//m_pNodeList    = Box.m_pNodeList;
			m_uNodeIndex   = Box.m_uNodeIndex;
			m_pColorOpt    = Box.m_pColorOpt;

			return *this;
		}

		const CBetterBox& operator+( const CBetterBox& Box)
		{
			m_uTotalWeight += Box.m_uTotalWeight;
			m_uSum         += Box.m_uSum;
			m_uR           += Box.m_uR;
			m_uG           += Box.m_uG;
			m_uB           += Box.m_uB;

			return *this;
		}

		const CBetterBox& operator-( const CBetterBox& Box)
		{
			m_uTotalWeight -= Box.m_uTotalWeight;
			m_uSum         -= Box.m_uSum;
			m_uR           -= Box.m_uR;
			m_uG           -= Box.m_uG;
			m_uB           -= Box.m_uB;

			return *this;
		}

		//CColorNode* GetHead( void)
		//	{
		//		ASSERT( m_pNodeList);
		//		return m_pNodeList;
		//	};
		UINT GetHead( void)
			{
				ASSERT( m_uNodeIndex != NIL_NODE);
				return m_uNodeIndex;
			};
		//CColorNode* GetNext( CColorNode* pNode)
		//	{
		//		ASSERT( pNode && m_pColorOpt);
		//		if (pNode)
		//		{
		//			UINT uIndex = pNode->GetNextIndex();
		//			if (uIndex != NIL_NODE) 
		//				return &(m_pColorOpt->m_pBetterHistNodeList[uIndex]);
		//		}
		//		return NULL;
		//	};
		UINT GetNextIndex( UINT uNodeIndex)
			{
				ASSERT( (uNodeIndex != NIL_NODE) && m_pColorOpt);

				if (uNodeIndex != NIL_NODE)
				{
					UINT uIndex = m_pColorOpt->m_pBetterHistNodeList[uNodeIndex].GetNextIndex();
					if (uIndex != NIL_NODE) 
						return uIndex;
				}
				return NIL_NODE;
			};
		CColorNode* GetNode( UINT uNodeIndex)
			{
				if (uNodeIndex != NIL_NODE)
				{
					return &(m_pColorOpt->m_pBetterHistNodeList[uNodeIndex]);
				}
				return NULL;
			};
//		CColorNode* GetNextNode( UINT uNodeIndex)
//			{
//				if (uNodeIndex != NIL_NODE)
//				{
//					uNodeIndex = m_pColorOpt->m_pBetterHistNodeList[uNodeIndex].GetNextIndex();
//					return GetNode( uNodeIndex);
//				}
//				return NULL;
//			};
	};
#endif


#endif // _COLOROPT_H_