//LZRW1.cpp: compressor by Ross Williams. A very fast LZ compressor.
// addapted to MFVC++ by Neal White III

#include "stdafx.h"
#include "Wally.h"

#define FLAG_BYTES    4     /* Number of bytes used by copy flag. */
#define FLAG_COMPRESS 0     /* Signals that compression occurred. */
#define FLAG_COPY     1     /* Signals that a copyover occurred.  */

/* Fast copy routine.             */
void fast_copy( const BYTE* p_src, BYTE *p_dst, int len)
{
	while (len--) 
		*p_dst++ = *p_src++;
}

/******************************************************************************/
#define PS (*p++ != *s++)    /* Body of inner unrolled matching loop.         */
#define ITEMMAX 16           /* Maximum number of bytes in an expanded item.  */

void LzrwCompress( const BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len)
/* Input  : Specify input block using p_src_first and src_len.          */
/* Input  : Point p_dst_first to the start of the output zone (OZ).     */
/* Input  : Point p_dst_len to a DWORD to receive the output length.    */
/* Input  : Input block and output zone must not overlap.               */
/* Output : Length of output block written to *p_dst_len.               */
/* Output : Output block in Mem[p_dst_first..p_dst_first+*p_dst_len-1]. */
/* Output : May write in OZ=Mem[p_dst_first..p_dst_first+src_len+256-1].*/
/* Output : Upon completion guaranteed *p_dst_len<=src_len+FLAG_BYTES.  */
{
	const BYTE* p_src       = p_src_first;
	const BYTE* p_src_post  = p_src_first + src_len;
	const BYTE* p_src_max1  = p_src_post - ITEMMAX;
	const BYTE* p_src_max16 = p_src_post - 16*ITEMMAX;
	const BYTE* hash[4096];
	BYTE* p_dst        = p_dst_first;
	BYTE* p_dst_post   = p_dst_first + src_len;
	BYTE* p_control    = NULL;
	UINT  control      = 0;
	UINT  control_bits = 0;

	*p_dst    = FLAG_COMPRESS;
	p_dst    += FLAG_BYTES;
	p_control = p_dst;
	p_dst    += 2;

	while (TRUE)
	{
		const BYTE* p;
		const BYTE* s;
		UINT  unroll = 16;
		UINT  len;
		UINT  index;
		DWORD  offset;

		if (p_dst > p_dst_post)
			goto overrun;

		if (p_src > p_src_max16)
		{ 
			unroll = 1;
			if (p_src > p_src_max1)
			{
				if (p_src == p_src_post) 
					break;
				goto literal;
			}
		}

begin_unrolled_loop:

		index       = ((40543*((((p_src[0]<<4)^p_src[1])<<4)^p_src[2]))>>4) & 0xFFF;
		p           = hash[index];
		hash[index] = s = p_src;
		offset      = s-p;

		if ((offset > 4095) || (p < p_src_first) || (offset == 0) || PS || PS || PS)
		{
literal:
			*p_dst++ = *p_src++;
			control >>= 1;
			control_bits++;
		}
		else
		{
			PS || PS || PS || PS || PS || PS || PS ||
			PS || PS || PS || PS || PS || PS || s++;

			len = s-p_src-1;
			*p_dst++ = (BYTE)(((offset & 0x0F00) >> 4) + (len-1));
			*p_dst++ = (BYTE )(offset & 0x00FF);
			p_src   += len;
			control  = (control >> 1) | 0x8000;
			control_bits++;
		}

//end_unrolled_loop:
		if (--unroll)
			goto begin_unrolled_loop;

		if (control_bits == 16)
		{
			*p_control     = control & 0x00FF;
			*(p_control+1) = control>>8;
			p_control      = p_dst;
			p_dst         += 2;
			control        = control_bits = 0;
		}
	}
	control >>= 16-control_bits;
	*p_control++ = control & 0x00FF;
	*p_control++ = control >> 8;

	if (p_control == p_dst)
		p_dst -= 2;

	*p_dst_len = p_dst - p_dst_first;
	return;

overrun:
	fast_copy( p_src_first, p_dst_first + FLAG_BYTES, src_len);
	*p_dst_first = FLAG_COPY;
	*p_dst_len   = src_len + FLAG_BYTES;
}

/******************************************************************************/

void LzrwDecompress( const BYTE* p_src_first, DWORD src_len, BYTE* p_dst_first, DWORD* p_dst_len)
/* Input  : Specify input block using p_src_first and src_len.          */
/* Input  : Point p_dst_first to the start of the output zone.          */
/* Input  : Point p_dst_len to a DWORD to receive the output length.    */
/* Input  : Input block and output zone must not overlap. User knows    */
/* Input  : upperbound on output block length from earlier compression. */
/* Input  : In any case, maximum expansion possible is eight times.     */
/* Output : Length of output block written to *p_dst_len.               */
/* Output : Output block in Mem[p_dst_first..p_dst_first+*p_dst_len-1]. */
/* Output : Writes only  in Mem[p_dst_first..p_dst_first+*p_dst_len-1]. */
{
	UINT  controlbits = 0;
	UINT  control;
	const BYTE* p_src      = p_src_first + FLAG_BYTES;
	BYTE* p_dst            = p_dst_first;
	const BYTE* p_src_post = p_src_first + src_len;
	DWORD dwSize           = *p_dst_len;

	if (*p_src_first == FLAG_COPY)
	{
		fast_copy( p_src_first+FLAG_BYTES, p_dst_first, src_len-FLAG_BYTES);

		dwSize = src_len - FLAG_BYTES;

		ASSERT( *p_dst_len == dwSize);
		*p_dst_len = dwSize;
		return;
	}

	ASSERT(*p_src_first == FLAG_COMPRESS);

	//while (p_src != p_src_post)
	//while ((p_src != p_src_post) && 
	//			((DWORD )(p_dst - p_dst_first) <= dwSize))	// neal - prevent over-runs
	while (p_src < p_src_post)
	{
		if (controlbits == 0)
		{
			control     = *p_src++;
			control    |= (*p_src++) << 8;
			controlbits = 16;
		}
		if (control & 1)
		{
			UINT offset,len;
			BYTE *p;

			offset = (*p_src & 0x00F0) << 4;
			len    = 1 + (*p_src++ & 0x000F);

			offset += *p_src++ & 0x00FF;
			p       = p_dst - offset;

			while (len--)
			//while ((len--) &&
			//			((DWORD )(p_dst - p_dst_first) <= dwSize))	// neal - prevent over-runs
			{
				*p_dst++ = *p++;
			}
		}
		else
			*p_dst++ = *p_src++;

	    control >>= 1;
		controlbits--;
	}
	dwSize = p_dst - p_dst_first;

	ASSERT( *p_dst_len == dwSize);
	*p_dst_len = dwSize;
}

HGLOBAL CompressMemory( HGLOBAL* phSrc, DWORD dwOriginalSize, DWORD* pdwSize, BOOL bFreeSrc)
{
	BYTE* lpSrc               = (BYTE* )GlobalLock( *phSrc);
	DWORD dwSize              = dwOriginalSize;		// GlobalSize() may be slightly too large
	DWORD dwUncompressedSize  = dwSize;

#define FUDGE 100		// fudge factor - compress can overrun buffer a little bit
						// (this should be plenty)

//#ifdef _DEBUG
//	HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE| GMEM_ZEROINIT, 
//			dwSize + FLAG_BYTES + sizeof( DWORD));
//#else
	HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE, 
			dwSize + FLAG_BYTES + 2*sizeof( DWORD) + FUDGE);
//#endif

	if (hDest)
	{
		BYTE* lpDest = (BYTE* )GlobalLock( hDest);

		*(DWORD* )(lpDest) = dwOriginalSize;	// save original size in block

		LzrwCompress( lpSrc, dwSize, lpDest + 2*sizeof( DWORD), &dwSize);

		*(DWORD* )(lpDest+sizeof( DWORD)) = dwSize;		// save compressed size in block

		dwSize += 2*sizeof( DWORD);  // return size of memory block (not just size of compressed data)

		if (pdwSize)
			*pdwSize = dwSize;

		GlobalReAlloc( hDest, dwSize, GMEM_MOVEABLE);
		GlobalUnlock( hDest);

#ifdef _DEBUG

		// double-check - did data compress PERFECTLY?

		HGLOBAL hComp = hDest;		// neal - just for clarity
		HGLOBAL hDeco = DecompressMemory( &hComp, FALSE);

		if (hDeco)
		{
			BYTE* lpDeco = (BYTE* )GlobalLock( hDeco);

			for (UINT j = 0; j < dwUncompressedSize; j++)
			{
				if (lpDeco[j] != lpSrc[j])
				{
					ASSERT( FALSE);
					break;
				}
			}
		}
		GlobalFree( hDeco);

#endif

		if (bFreeSrc)
		{
			GlobalUnlock( *phSrc);
			GlobalFree(   *phSrc);

			*phSrc = NULL;
		}
	}
	return hDest;
}

HGLOBAL DecompressMemory( HGLOBAL* phCompressedSrc, BOOL bFreeSrc)
{
	BYTE* lpSrc               = (BYTE* )GlobalLock( *phCompressedSrc);
	DWORD dwSize              = *(DWORD* )(lpSrc);
	DWORD dwCompressedSize    = *(DWORD* )(lpSrc + sizeof( DWORD));
	DWORD dwUncompressedSize  = dwSize;

//#ifdef _DEBUG
//	HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE| GMEM_ZEROINIT, dwSize);
//#else

	// neal - +1024 because decompress often over-flows by a few bytes
	// (I've seen at least by 7 bytes)

	//HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE, dwUncompressedSize+1024);
	// The bug is now FIXED!

	HGLOBAL hDest = GlobalAlloc( GMEM_MOVEABLE, dwUncompressedSize);
//#endif

	if (hDest)
	{
		BYTE* lpDest = (BYTE* )GlobalLock( hDest);

		LzrwDecompress( lpSrc + 2*sizeof( DWORD), dwCompressedSize, lpDest, &dwSize);

		if (bFreeSrc)
		{
			GlobalUnlock( *phCompressedSrc);
			GlobalFree(   *phCompressedSrc);

			*phCompressedSrc = NULL;
		}

		GlobalUnlock( hDest);
		//GlobalReAlloc( hDest, dwUncompressedSize, GMEM_MOVEABLE);
	}
	return hDest;
}

CString CompressString( const CString& strSrc)
{
	CString strDest;
	DWORD   dwCharSize = strSrc.GetLength();
	DWORD   dwSize     = dwCharSize * sizeof( TCHAR);	// size in bytes

	ASSERT( dwSize <= 32768);

	TCHAR*  szDest = strDest.GetBufferSetLength( (dwSize + FLAG_BYTES + 
				sizeof( DWORD)) / sizeof( TCHAR));

	*(DWORD* )(szDest) = dwSize;	// save original size in block (in bytes)

	LzrwCompress( (BYTE *)(LPCTSTR )strSrc, dwSize, 
			((BYTE *)szDest) + sizeof( DWORD), &dwSize);

	strDest.ReleaseBuffer( (dwSize + sizeof( DWORD)) / sizeof( TCHAR));
	return strDest;
}

extern CString DecompressString( const CString& strCompressedSrc)
{
	CString strDest;
	LPCTSTR szSrc  = strCompressedSrc;
	DWORD   dwSize = *(DWORD* )(szSrc);   // size in bytes

	ASSERT( dwSize <= 32768);

	DWORD  dwCharSize = dwSize / sizeof( TCHAR);		// nunber of characters
	TCHAR* szDest     = strDest.GetBufferSetLength( dwCharSize);

	dwSize = (strCompressedSrc.GetLength() * sizeof( TCHAR)) - sizeof( DWORD);

	LzrwDecompress( (const BYTE *)szSrc + sizeof( DWORD), dwSize,
			(BYTE *)szDest, &dwSize);

	strDest.ReleaseBuffer( dwCharSize);
	return strDest;
}

/******************************************************************************/
/*                          End of LZRW1.C                                    */
/******************************************************************************/
/*
void CompressTest( void)
{
	const BYTE* str1 = (BYTE* )"aaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccd\n\r";
	BYTE str2[100];
	BYTE str3[100];
	DWORD len1 = strlen( (const char *)str1)+1;
	DWORD len2 = 0;
	DWORD len3 = 0;

	LzrwCompress(   str1, len1, str2, &len2);
	LzrwDecompress( str2, len2, str3, &len3);

	ASSERT( len1 == len3);

//			"aaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccdaaaabbbccd\n\r";
	TRACE0( "************************************************************\n");
	TRACE0( str1);
	TRACE0( str3);
	TRACE2( "Uncompressed Size: %d,  Compressed Size: %d\n", len1, len2);
	TRACE0( "************************************************************\n");

	// COMPRESSION TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#if 0
	int i, j;
	srand( 111);

	// neal - this test takes many minutes to run !!!

	for (i = 0; i < 100000; i++)
//	for (i = 0; i < 10; i++)
	{
		if ((i & 511) == 0)
			TRACE1( "Compression Test: %d\n", i);

		int iSize = 1 + (rand() & ((64*1024)-1));

		HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE, iSize);
		DWORD dwError = GetLastError();

		if (hMem)
		{
			BYTE* lpMem = (BYTE *)GlobalLock( hMem);

			BYTE byRand = (BYTE )(rand() & 255);

			for (j = 0; j < iSize; j++)
			{
				if ((rand() & 32) == 0)
					byRand = (BYTE )(rand() & 255);

				lpMem[j] = byRand;
			}

			HGLOBAL hComp = CompressMemory( &hMem, FALSE);
			HGLOBAL hDeco = DecompressMemory( &hComp, FALSE);

			if (hDeco)
			{
				BYTE* lpDeco = (BYTE* )GlobalLock( hDeco);

				for (j = 0; j < iSize; j++)
				{
					if (lpDeco[j] != lpMem[j])
					{
						ASSERT( FALSE);
						break;
					}
				}
			}

			GlobalFree( hMem);
			GlobalFree( hComp);
			GlobalFree( hDeco);
		}
	}
	TRACE1( "Compression Test: %d\n", i);
#endif
	// COMPRESSION TEST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

	// STRING COMPRESSION TEST <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#if 0
	int i, j;
	srand( 123);

	// neal - this test takes many minutes to run !!!

	for (i = 0; i < 100000; i++)
//	for (i = 0; i < 10; i++)
	{
		if ((i & 511) == 0)
			TRACE1( "String Compression Test: %d\n", i);

		int iSize = 1 + (rand() & ((31*1024)-1));

		CString strSrc;

		BYTE byRand = (BYTE )(rand() & 255);

		LPTSTR szBuffer = strSrc.GetBufferSetLength( iSize);

		for (j = 0; j < iSize; j++)
		{
			if ((rand() & 32) == 0)
				byRand = (BYTE )(rand() & 255);

			//strSrc += byRand;
			szBuffer[j] = byRand;
		}
		szBuffer[iSize-1] = TCHAR( '\0');
		strSrc.ReleaseBuffer( iSize);

		CString strComp = CompressString( strSrc);
		CString strDeco = DecompressString( strComp);

		if (strDeco)
		{
			if (strDeco != strSrc)
			{
				ASSERT( FALSE);
			}
		}
	}
	TRACE1( "String Compression Test: %d\n", i);
#endif
	// STRING COMPRESSION TEST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
}
*/
