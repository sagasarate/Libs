#pragma once

extern "C"
{
	size_t iconv_gbk2utf8(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);
	size_t iconv_utf82gbk(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);

	size_t iconv_gbk2ucs2(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);
	size_t iconv_ucs22gbk(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);

	size_t iconv_ucs22utf8(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);
	size_t iconv_utf82ucs2(const char * szSrv, size_t SrcLen, char * szDest, size_t DestLen);
}
