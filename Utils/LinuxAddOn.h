/****************************************************************************/
/*                                                                          */
/*      文件名:    LinuxAddOn.h                                             */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define _FILE_OFFSET_BITS   64

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif

#ifndef _RENENTRANT
#define	_RENENTRANT
#endif

#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <utility>
#include <assert.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
//#include <uuid/uuid.h>
//#include "atomic_ops/include/atomic_ops.h"
#include <utime.h>
#include <dlfcn.h>
#include <signal.h>
#include <sys/syscall.h>

#define __forceinline inline

#define CLOCKS_PER_MILLISEC		(CLOCKS_PER_SEC/1000)
#define INVALID_HANDLE_VALUE	(-1)



#define DEFAULT_IDLE_SLEEP_TIME		10

#define MAX_PATH        PATH_MAX

//#define TRUE			1
//#define FALSE			0
#define INFINITE		0xffffffff
#define __int64			long long

#define SIG_THREAD_SUSPEND			SIGRTMIN
#define SIG_THREAD_RESUME			(SIGRTMIN+1)
#define SIG_THREAD_GET_CALL_STACK	(SIGRTMIN+2)

typedef signed char			INT8;
typedef unsigned char		UINT8;
typedef signed short		INT16;
typedef unsigned short		UINT16;
typedef signed int			INT32;
typedef unsigned int		UINT32;


typedef long long LONG64;
typedef long long INT64;
typedef unsigned long long ULONG64;
typedef unsigned long long UINT64;
typedef unsigned long long LONGLONG;
typedef unsigned long long DWORD64;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned int WPARAM;
typedef unsigned int LPARAM;
typedef unsigned int  LRESULT;
//typedef int BOOL;
typedef float FLOAT;
typedef double DOUBLE;
typedef unsigned short WCHAR;
typedef char * LPSTR;
typedef const char * LPCSTR;
typedef WCHAR * LPWSTR;
typedef const WCHAR * LPCWSTR;
typedef void * LPVOID;
typedef const void * LPCVOID;
typedef BYTE * LPBYTE;
typedef const BYTE * LPCBYTE;
typedef void * HMODULE;
typedef void * HANDLE;
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef unsigned int DWORD_PTR, *PDWORD_PTR;




#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

#ifdef UNICODE

typedef WCHAR TCHAR;

#else

typedef char TCHAR;
#define _T(x)			x

#endif

typedef TCHAR * LPTSTR;
typedef const TCHAR * LPCTSTR;

#define _stricmp		strcasecmp
#define _strnicmp		strncasecmp
//#define _wcsicmp		wcscasecmp
//#define _wcsnicmp		wcsncasecmp
#define _atoi64			atoll
#define _strtoui64		strtoull
#define GetLastError()	errno

inline WCHAR tolower(WCHAR c)
{
	if (c >= 'A'&&c <= 'Z')
		return c - ('A' - 'a');
	return c;
}

inline int fopen_s(FILE ** _File, const char * _Filename, const char * _Mode)
{
	*_File=fopen(_Filename,_Mode);
	return 0;
}

inline size_t fread_s(void * _DstBuf, size_t _DstSize, size_t _ElementSize, size_t _Count, FILE * _File)
{
	return fread(_DstBuf,_ElementSize,_Count,_File);
}

inline void memcpy_s(void * _Dst, size_t _DstSize, const void * _Src, size_t _MaxCount)
{
	memcpy(_Dst, _Src, _MaxCount);
}

inline int strcpy_s(char * _Dst, size_t _SizeInBytes, const char * _Src)
{
	strcpy(_Dst,_Src);
	return 0;
}

inline int wcscpy_s(WCHAR * _Dst, size_t _SizeInWords, const WCHAR * _Src)
{
	if (_Dst == NULL || _SizeInWords == 0)
		return -1;
	size_t CpyCount = 0;
	while ((*_Src) && (CpyCount < _SizeInWords))
	{
		*_Dst = *_Src;
		_Dst++;
		_Src++;
		CpyCount++;
	}
	if (CpyCount < _SizeInWords)
		*_Dst = 0;
	return 0;
}

inline int strncpy_s(char * _Dst, size_t _SizeInBytes, const char * _Src, size_t _MaxCount)
{
	strncpy(_Dst,_Src,_MaxCount);
	return 0;
}

inline int wcsncpy_s(WCHAR * _Dst, size_t _SizeInWords, const WCHAR * _Src, size_t _MaxCount)
{
	if (_Dst == NULL || _SizeInWords == 0)
		return -1;
	size_t CpyCount = 0;
	while ((*_Src) && (CpyCount < _SizeInWords) && (CpyCount < _MaxCount))
	{
		*_Dst = *_Src;
		_Dst++;
		_Src++;
		CpyCount++;
	}
	if (CpyCount < _SizeInWords)
		*_Dst = 0;
	return 0;
}

inline int strcat_s(char * _Dst, size_t _SizeInBytes, const char * _Src)
{
	strcat(_Dst,_Src);
	return 0;
}

inline int wcscat_s(WCHAR * _Dst, size_t _SizeInWords, const WCHAR * _Src)
{
	if (_Dst == NULL || _SizeInWords == 0)
		return -1;
	size_t CpyCount = 0;
	while ((*_Dst) && (CpyCount < _SizeInWords))
	{
		_Dst++;
		CpyCount++;
	}
	while ((*_Src) && (CpyCount < _SizeInWords))
	{
		*_Dst = *_Src;
		_Dst++;
		_Src++;
		CpyCount++;
	}
	if (CpyCount < _SizeInWords)
		*_Dst = 0;
	return 0;
}

inline char *  strtok_s(char * _Str, const char * _Delim, char ** _Context)
{
	if(_Str==NULL)
		_Str=*_Context;
	char * Ret=strtok(_Str,_Delim);
	*_Context=Ret+1;
	return Ret;
}

inline int _itoa_s(int _Value, char * _DstBuf, size_t _Size, int _Radix)
{
	if(_Radix==16)
	{
		sprintf(_DstBuf,_T("%X"),_Value);
	}
	else
	{
		sprintf(_DstBuf,_T("%d"),_Value);
	}
	return 0;
}

inline int wcscmp(const WCHAR *string1, const WCHAR *string2)
{
	while ((*string1) && (*string2))
	{
		if ((*string1) > (*string2))
			return 1;
		else if ((*string1) < (*string2))
			return -1;
		string1++;
		string2++;
	}
	if ((*string1) > (*string2))
		return 1;
	else if ((*string1) < (*string2))
		return -1;
	else
		return 0;
}

inline int wcsncmp(const WCHAR *string1, const WCHAR *string2, size_t count)
{
	while ((*string1) && (*string2) && (count))
	{
		if ((*string1) > (*string2))
			return 1;
		else if ((*string1) < (*string2))
			return -1;
		string1++;
		string2++;
		count--;
	}
	return 0;
}

inline int _wcsicmp(const WCHAR *string1, const WCHAR *string2)
{
	WCHAR c1, c2;
	while ((*string1) && (*string2))
	{
		c1 = tolower(*string1);
		c2 = tolower(*string2);
		if (c1 > c2)
			return 1;
		else if (c1 < c2)
			return -1;
		string1++;
		string2++;
	}
	c1 = tolower(*string1);
	c2 = tolower(*string2);
	if (c1 > c2)
		return 1;
	else if (c1 < c2)
		return -1;
	else
		return 0;
}

inline int _wcsnicmp(const WCHAR *string1, const WCHAR *string2, size_t count)
{
	WCHAR c1, c2;
	while ((*string1) && (*string2) && (count))
	{
		c1 = tolower(*string1);
		c2 = tolower(*string2);
		if (c1 > c2)
			return 1;
		else if (c1 < c2)
			return -1;
		string1++;
		string2++;
		count++;
	}
	c1 = tolower(*string1);
	c2 = tolower(*string2);
	if (c1 > c2)
		return 1;
	else if (c1 < c2)
		return -1;
	else
		return 0;
}

inline size_t wcslen(const WCHAR *string)
{
	size_t len = 0;
	if (string)
	{
		while (*string)
		{
			string++;
		}
	}
	return len;
}

inline WCHAR * wcsstr(const WCHAR *string, const WCHAR *strCharSet)
{
	if (string == NULL || strCharSet == NULL)
		return NULL;

	const WCHAR *s = string;

	const WCHAR *t = strCharSet;

	for (; *t != '\0'; ++string)
	{
		for (s = string, t = strCharSet; *t != '\0' && *s == *t; ++s, ++t)
			NULL;

		if (*t == '\0')
			return (WCHAR *)string;
	}

	return NULL;
}

inline  WCHAR * wcschr(const WCHAR *string, WCHAR c)
{
	if (string)
	{
		while (*string)
		{
			if (*string == c)
				return (WCHAR *)string;
			string++;
		}
	}
	return NULL;
}

inline WCHAR * wcsrchr(const WCHAR * string, WCHAR c)
{
	if (string)
	{
		WCHAR * finded = NULL;
		while (*string)
		{
			if (*string == c)
				finded = (WCHAR *)string;
			string++;
		}
		return finded;
	}
	return NULL;
}
//inline int _ltoa_s(long _Val, char * _DstBuf, size_t _Size, int _Radix)
//{
//	if(_Radix==16)
//	{
//		sprintf(_DstBuf,_T("%lX"),_Val);
//	}
//	else
//	{
//		sprintf(_DstBuf,_T("%ld"),_Val);
//	}
//	return 0;
//}

#define sprintf_s(_DstBuf, _SizeInBytes, _Format, ...)	sprintf(_DstBuf,_Format,##__VA_ARGS__)
#define _vscprintf(_Format, _ArgList)		vsnprintf(NULL,0,_Format,_ArgList)
//#define _vscwprintf(_Format, _ArgList)		vswprintf(NULL,0,_Format,_ArgList)
#define vsprintf_s(_DstBuf,_SizeInBytes,_Format,_ArgList)	vsnprintf(_DstBuf,_SizeInBytes,_Format,_ArgList)
//#define vswprintf_s(_DstBuf,_SizeInBytes,_Format,_ArgList)	vswprintf(_DstBuf,_SizeInBytes,_Format,_ArgList)
#define _snprintf_s(_DstBuf, _SizeInBytes, _MaxCount, _Format, ...)	sprintf(_DstBuf,_Format,##__VA_ARGS__)

inline int _vscwprintf(const WCHAR *format, va_list argptr)
{
	return 0;
}

inline int vswprintf_s(WCHAR *buffer, size_t numberOfElements, const WCHAR *format, va_list argptr)
{
	return 0;
}

//inline int vsprintf_s(char * _DstBuf, size_t _SizeInBytes, const char * _Format, va_list _ArgList)
//{
//	return vsprintf(_DstBuf,_Format,_ArgList);
//}

//inline int vswprintf_s(WCHAR * _DstBuf, size_t _SizeInBytes, const WCHAR * _Format, va_list _ArgList)
//{
//	return vswprintf(_DstBuf,_SizeInBytes,_Format,_ArgList);
//}

inline int _strupr_s(char * _Str, size_t _Size)
{
	while(_Size&&(*_Str))
	{
		*_Str=toupper(*_Str);
		_Str++;
		_Size--;
	}
	return 0;
}

inline int _wcsupr_s(WCHAR * _Str, size_t _SizeInWords)
{
	while(_SizeInWords&&(*_Str))
	{
		*_Str=towupper(*_Str);
		_Str++;
		_SizeInWords--;
	}
	return 0;
}

inline int _strlwr_s(char * _Str, size_t _Size)
{
	while(_Size&&(*_Str))
	{
		*_Str=tolower(*_Str);
		_Str++;
		_Size--;
	}
	return 0;
}

inline int _wcslwr_s(WCHAR * _Str,  size_t _SizeInWords)
{
	while(_SizeInWords&&(*_Str))
	{
		*_Str=towlower(*_Str);
		_Str++;
		_SizeInWords--;
	}
	return 0;
}

inline int strncat_s(char * _Dst, size_t _SizeInBytes, const char * _Src, size_t _MaxCount)
{
	strncat(_Dst,_Src,_MaxCount);
	return 0;
}

inline int localtime_s(struct tm * _Tm, const time_t * _Time)
{
	localtime_r(_Time,_Tm);
	return 0;
}

inline int gmtime_s(struct tm* _tm,const time_t* time)
{
	gmtime_r(time,_tm);
	return 0;
}

#define _mkgmtime timegm



inline size_t GetEnvVar(LPCTSTR pszVarName,LPTSTR pszValue,size_t nBufferLen)
{
	size_t ValueLen=0;
#ifdef _UNICODE
	//_wgetenv_s(&ValueLen,pszValue,nBufferLen,pszVarName);
#else
	char * szValue=getenv(pszVarName);
	if(szValue)
	{
		ValueLen=strlen(szValue);
		if(pszValue&&nBufferLen>=ValueLen)
		{
			strcpy(pszValue,szValue);
		}
	}
#endif
	return ValueLen;
}

inline bool SetEnvVar(LPCTSTR pszVarName,LPCTSTR pszValue)
{
#ifdef _UNICODE
	//if(_wputenv_s(pszVarName,pszValue)==0)
	//	return true;
#else
	if(setenv(pszVarName,pszValue,1)==0)
		return true;
#endif
	return false;
}

inline UINT GetCurProcessID()
{
	return getpid();
}

inline unsigned int AtomicInc(volatile unsigned int * pVal)
{
	//return AO_int_fetch_and_add1_read(pVal)+1;
	return __sync_add_and_fetch(pVal, 1);
}

inline unsigned int AtomicDec(volatile unsigned int * pVal)
{
	//return AO_int_fetch_and_sub1_read(pVal)-1;
	return __sync_sub_and_fetch(pVal, 1);
}


inline unsigned int AtomicAdd(volatile unsigned int * pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_add_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicSub(volatile unsigned int * pVal, unsigned int SubVal)
{
	//return AO_int_fetch_and_add(pVal,-SubVal)-SubVal;
	return __sync_sub_and_fetch(pVal, SubVal);
}

inline unsigned int AtomicOr(volatile unsigned int * pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_or_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicAnd(volatile unsigned int * pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_and_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicXor(volatile unsigned int * pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_xor_and_fetch(pVal, AddVal);
}
inline bool AtomicCompareAndSet(volatile unsigned int * pVal, unsigned int CompValue, unsigned int NewVal)
{
	return __sync_bool_compare_and_swap(pVal, CompValue, NewVal);
}
//inline unsigned int AtomicNand(volatile unsigned int * pVal, int AddVal)
//{
//	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
//	return __sync_nand_and_fetch(pVal, AddVal);
//}

#ifdef __x86_64__
inline unsigned int AtomicInc(volatile unsigned long long * pVal)
{
	//return AO_int_fetch_and_add1_read(pVal)+1;
	return __sync_add_and_fetch(pVal, 1);
}

inline unsigned int AtomicDec(volatile unsigned long long * pVal)
{
	//return AO_int_fetch_and_sub1_read(pVal)-1;
	return __sync_sub_and_fetch(pVal, 1);
}


inline unsigned int AtomicAdd(volatile unsigned long long * pVal, unsigned long long AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_add_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicSub(volatile unsigned long long * pVal, unsigned long long SubVal)
{
	//return AO_int_fetch_and_add(pVal,-SubVal)-SubVal;
	return __sync_sub_and_fetch(pVal, SubVal);
}

inline unsigned int AtomicOr(volatile unsigned long long * pVal, unsigned long long AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_or_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicAnd(volatile unsigned long long * pVal, unsigned long long AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_and_and_fetch(pVal, AddVal);
}

inline unsigned int AtomicXor(volatile unsigned long long * pVal, unsigned long long AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return __sync_xor_and_fetch(pVal, AddVal);
}

inline bool AtomicCompareAndSet(volatile unsigned __int64 * pVal, unsigned __int64 CompValue, unsigned __int64 NewVal)
{
	return __sync_bool_compare_and_swap(pVal, CompValue, NewVal);
}
//inline unsigned int AtomicNand(volatile unsigned long long * pVal, long long AddVal)
//{
//	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
//	return __sync_nand_and_fetch(pVal, AddVal);
//}
#endif



#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))



#include "TCharLinux.h"

#include "EasyCriticalSectionLinux.h"
#include "AutoLock.h"
#include "EasyReadWriteLockLinux.h"

typedef struct tagRECT
{
	int    left;
	int    top;
	int    right;
	int    bottom;
} RECT, *PRECT,  *LPRECT;

typedef const tagRECT * LPCRECT;

typedef struct tagPOINT
{
	int  x;
	int  y;
} POINT, *PPOINT,  *LPPOINT;

typedef const tagPOINT * LPCPOINT;

typedef struct tagSIZE
{
	int        cx;
	int        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef const tagSIZE * LPCSIZE;
