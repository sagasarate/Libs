/****************************************************************************/
/*                                                                          */
/*      文件名:    TCharLinux.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



#ifdef UNICODE


#else

#define CP_ACP                    0           // default to ANSI code page
#define CP_UTF8                   65001       // UTF-8 translation
#define CP_UNICODE				  1200

#define _TRUNCATE		(((size_t)-1))

#define _tcscmp			strcmp
#define _tcsncmp		strncmp
#define _tcsicmp		strcasecmp
#define _tcsnicmp		strncasecmp
#define _tcslen			strlen

#define _totlower		tolower
#define _tcscpy			strcpy
#define _tcscpy_s		strcpy_s
#define _tcsncpy		strncpy
#define _tcsncpy_s		strncpy_s

#define _tcscat_s		strcat_s
#define _tcsncat_s		strncat_s

#define _tcschr			strchr

#define _ttoi			atoi
#define _tstoi			atoi
#define _tcstol			strtol
#define _tcstod			strtod
#define _tcstoul		strtoul
#define _tstoi64		atoll
#define _tcstoi64		strtoll
#define _tcstoui64		strtoull
#define _tstof			atof
#define _tcstof			strtof
#define _tcstok_s		strtok_s
#define _itot_s			_itoa_s
#define _ltot_s			_ltoa_s
#define _istalnum		isalnum
#define _tcsstr			strstr

#define _tcsftime		strftime


#define _tcprintf		printf
#define _stprintf_s		sprintf_s
#define _vstprintf_s	vsprintf_s
#define _sctprintf		printf
#define _stscanf		scanf

#define _tfopen_s		fopen_s
#define _tsplitpath_s	_splitpath_s


#endif

