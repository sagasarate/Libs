#include <stdlib.h>
#include <string.h>


/* Our own notion of wide character, as UCS-4, according to ISO-10646-1. */
typedef unsigned int ucs4_t;

/* State used by a conversion. 0 denotes the initial state. */
typedef unsigned int state_t;

typedef void* iconv_t;

/* iconv_t is an opaque type. This is the real iconv_t type. */
typedef struct conv_struct * conv_t;

/* Hook performed after every successful conversion of a Unicode character. */
typedef void(*iconv_unicode_char_hook) (unsigned int uc, void* data);
/* Hook performed after every successful conversion of a wide character. */
typedef void(*iconv_wide_char_hook) (wchar_t wc, void* data);
/* Set of hooks. */
struct iconv_hooks {
	iconv_unicode_char_hook uc_hook;
	iconv_wide_char_hook wc_hook;
	void* data;
};

/*
* Data type for conversion multibyte -> unicode
*/
struct mbtowc_funcs {
	int(*xxx_mbtowc) (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n);
	/*
	* int xxx_mbtowc (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n)
	* converts the byte sequence starting at s to a wide character. Up to n bytes
	* are available at s. n is >= 1.
	* Result is number of bytes consumed (if a wide character was read),
	* or -1 if invalid, or -2 if n too small, or -2-(number of bytes consumed)
	* if only a shift sequence was read.
	*/
	int(*xxx_flushwc) (conv_t conv, ucs4_t *pwc);
	/*
	* int xxx_flushwc (conv_t conv, ucs4_t *pwc)
	* returns to the initial state and stores the pending wide character, if any.
	* Result is 1 (if a wide character was read) or 0 if none was pending.
	*/
};

/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ      -1
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)  (-2-(n))

/*
* Data type for conversion unicode -> multibyte
*/
struct wctomb_funcs {
	int(*xxx_wctomb) (conv_t conv, unsigned char *r, ucs4_t wc, int n);
	/*
	* int xxx_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
	* converts the wide character wc to the character set xxx, and stores the
	* result beginning at r. Up to n bytes may be written at r. n is >= 1.
	* Result is number of bytes written, or -1 if invalid, or -2 if n too small.
	*/
	int(*xxx_reset) (conv_t conv, unsigned char *r, int n);
	/*
	* int xxx_reset (conv_t conv, unsigned char *r, int n)
	* stores a shift sequences returning to the initial state beginning at r.
	* Up to n bytes may be written at r. n is >= 0.
	* Result is number of bytes written, or -2 if n too small.
	*/
};

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2

/*
* Data type for general conversion loop.
*/
struct loop_funcs {
	size_t(*loop_convert) (iconv_t icd,
		const char* * inbuf, size_t *inbytesleft,
		char* * outbuf, size_t *outbytesleft);
	size_t(*loop_reset) (iconv_t icd,
		char* * outbuf, size_t *outbytesleft);
};

/*
* Contents of a conversion descriptor.
*/
struct conv_struct {
	struct loop_funcs lfuncs;
	/* Input (conversion multibyte -> unicode) */
	int iindex;
	struct mbtowc_funcs ifuncs;
	state_t istate;
	/* Output (conversion unicode -> multibyte) */
	int oindex;
	struct wctomb_funcs ofuncs;
	int oflags;
	state_t ostate;
	/* Operation flags */
	int transliterate;
	int discard_ilseq;
#ifndef LIBICONV_PLUG
	struct iconv_hooks hooks;
#endif
};



typedef struct {
	unsigned short indx; /* index into big table */
	unsigned short used; /* bitmask of used entries */
} Summary16;

#include "gb2312.h"
#include "gbk.h"
#include "utf8.h"
#include "ucs2internal.h"

typedef int (*INPUT_FN)(conv_t conv, ucs4_t *pwc, const unsigned char *s, int n);
typedef int (*OUTPUT_FN)(conv_t conv, unsigned char *r, ucs4_t wc, int n);

size_t CodePageConv(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen, INPUT_FN InputFN, OUTPUT_FN OutputFN)
{
	if (szSrv == NULL)
		return 0;
	if (SrcLen <= 0)
		SrcLen = strlen(szSrv);
	size_t SrcPtr = 0;
	int SrcCharLen = 1;
	size_t DestPtr = 0;
	conv_t conv = NULL;
	ucs4_t MidChar;
	char ConvBuff[6];
	int i;

	while (SrcPtr < SrcLen)
	{

		int Ret = InputFN(conv, &MidChar, szSrv + SrcPtr, SrcCharLen);
		if (Ret == RET_ILSEQ)
		{
			//非编码字符，直接输出
			if (szDest != NULL&&DestPtr + SrcCharLen <= DestLen)
			{
				for (i = 0; i < SrcCharLen; i++)
					szDest[DestPtr + i] = szSrv[SrcPtr + i];
			}
			SrcPtr += SrcCharLen;
			DestPtr += SrcCharLen;
			SrcCharLen = 1;
		}
		else if (Ret <= RET_TOOSMALL)
		{
			//还需要获取更多字节
			SrcCharLen += (-1 - Ret);
		}
		else
		{
			//转换成功
			int Ret = OutputFN(conv, ConvBuff, MidChar, 6);
			if (Ret > 0)
			{
				if (szDest != NULL&&DestPtr + Ret <= DestLen)
				{
					for (i = 0; i < Ret; i++)
						szDest[DestPtr + i] = ConvBuff[i];
				}
				SrcPtr += SrcCharLen;
				DestPtr += Ret;
			}
			else
			{
				//转换失败，直接输出
				if (szDest != NULL&&DestPtr + SrcCharLen <= DestLen)
				{
					for (i = 0; i < SrcCharLen; i++)
						szDest[DestPtr + i] = szSrv[SrcPtr + i];
				}
				SrcPtr += SrcCharLen;
				DestPtr += SrcCharLen;
			}
			SrcCharLen = 1;
		}
	}
	return DestPtr;
}

size_t iconv_gbk2utf8(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, gbk_mbtowc, utf8_wctomb);	
}

size_t iconv_utf82gbk(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, utf8_mbtowc, gbk_wctomb);
}


size_t iconv_gbk2ucs2(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, gbk_mbtowc, ucs2internal_wctomb);
}
size_t iconv_ucs22gbk(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, ucs2internal_mbtowc, gbk_wctomb);
}

size_t iconv_ucs22utf8(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, ucs2internal_mbtowc, utf8_wctomb);
}

size_t iconv_utf82ucs2(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen)
{
	return CodePageConv(szSrv, SrcLen, szDest, DestLen, utf8_mbtowc, ucs2internal_wctomb);
}