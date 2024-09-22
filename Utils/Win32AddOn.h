/****************************************************************************/
/*                                                                          */
/*      文件名:    Win32AddOn.h                                             */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#include <windows.h>
#include <Winsvc.h>
//#include <minwindef.h>

#include "shlobj.h"
#include "tchar.h"
#include <time.h>
#include <conio.h>
#include <Dbghelp.h>
#include <Psapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <comutil.h>

#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Dbghelp.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"shell32.lib")

#pragma warning (push)
#pragma warning (disable : 4311)
#include "atomic_ops\include\atomic_ops.h"
#pragma warning (pop)

#include <intrin.h>
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)
#ifdef _WIN64
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement64)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedAnd64)
#pragma intrinsic(_InterlockedOr64)
#endif

#define CP_UNICODE					1200
#define DEFAULT_IDLE_SLEEP_TIME		10

typedef unsigned int UINT32;
typedef double DOUBLE;

typedef unsigned __int64 ULONG64;
typedef unsigned __int64 UINTG64;

typedef __int64 LONG64;
typedef __int64 INTG64;



inline UINT GetCurProcessID()
{
	return GetCurrentProcessId();
}

inline unsigned int AtomicInc(volatile unsigned int* pVal)
{
	//return AO_int_fetch_and_add1_read(pVal)+1;
	return _InterlockedIncrement((volatile LONG*)pVal);
}

inline unsigned int AtomicDec(volatile unsigned int* pVal)
{
	//return AO_int_fetch_and_sub1_read(pVal)-1;
	return _InterlockedDecrement((volatile LONG*)pVal);
}


inline unsigned int AtomicAdd(volatile unsigned int* pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal);
	return _InterlockedExchangeAdd((volatile LONG*)pVal, AddVal) + AddVal;
}

inline unsigned int AtomicSub(volatile unsigned int* pVal, unsigned int SubVal)
{
	//return AO_int_fetch_and_add(pVal,-SubVal);
	return _InterlockedExchangeAdd((volatile LONG*)pVal, (unsigned int)(-((int)SubVal))) - SubVal;
}


inline unsigned int AtomicOr(volatile unsigned int* pVal, unsigned int AndVal)
{
	return _InterlockedOr((volatile LONG*)pVal, AndVal);
}

inline unsigned int AtomicAnd(volatile unsigned int* pVal, unsigned int AndVal)
{
	return _InterlockedAnd((volatile LONG*)pVal, AndVal);
}

inline unsigned int AtomicXor(volatile unsigned int* pVal, unsigned int AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return _InterlockedXor((volatile LONG*)pVal, AddVal);
}

inline bool AtomicCompareAndSet(volatile unsigned int* pVal, unsigned int CompValue, unsigned int NewVal)
{
	return _InterlockedCompareExchange((volatile LONG*)pVal, NewVal, CompValue) != *pVal;
}

#ifdef _WIN64

inline unsigned __int64 AtomicInc(volatile unsigned __int64* pVal)
{
	//return AO_int_fetch_and_add1_read(pVal)+1;
	return _InterlockedIncrement64((volatile LONG64*)pVal);
}
inline unsigned __int64 AtomicDec(volatile unsigned __int64* pVal)
{
	//return AO_int_fetch_and_sub1_read(pVal)-1;
	return _InterlockedDecrement64((volatile LONG64*)pVal);
}


inline unsigned __int64 AtomicAdd(volatile unsigned __int64* pVal, unsigned __int64 AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal);
	return _InterlockedExchangeAdd64((volatile LONG64*)pVal, AddVal) + AddVal;
}

inline unsigned __int64 AtomicSub(volatile unsigned __int64* pVal, unsigned __int64 SubVal)
{
	//return AO_int_fetch_and_add(pVal,-SubVal);
	return _InterlockedExchangeAdd64((volatile LONG64*)pVal, (unsigned __int64)(-((__int64)SubVal))) - SubVal;
}


inline unsigned __int64 AtomicOr(volatile unsigned __int64* pVal, unsigned __int64 AndVal)
{
	return _InterlockedOr64((volatile LONG64*)pVal, AndVal);
}

inline unsigned __int64 AtomicAnd(volatile unsigned __int64* pVal, unsigned __int64 AndVal)
{
	return _InterlockedAnd64((volatile LONG64*)pVal, AndVal);
}

inline unsigned __int64 AtomicXor(volatile unsigned __int64* pVal, unsigned __int64 AddVal)
{
	//return AO_int_fetch_and_add(pVal,AddVal)+AddVal;
	return _InterlockedXor64((volatile LONG64*)pVal, AddVal);
}

inline bool AtomicCompareAndSet(volatile unsigned __int64* pVal, unsigned __int64 CompValue, unsigned __int64 NewVal)
{
	return _InterlockedCompareExchange64((volatile LONG64*)pVal, NewVal, CompValue) != *pVal;
}
#endif


inline size_t GetEnvVar(LPCTSTR pszVarName, LPTSTR pszValue, size_t nBufferLen)
{
	return GetEnvironmentVariable(pszVarName, pszValue, (DWORD)nBufferLen);
	//	size_t ValueLen=0;	
	//#ifdef _UNICODE
	//	_wgetenv_s(&ValueLen,pszValue,nBufferLen,pszVarName);
	//#else	
	//	getenv_s(&ValueLen,pszValue,nBufferLen,pszVarName);
	//#endif
	//	return ValueLen;
}

inline bool SetEnvVar(LPCTSTR pszVarName, LPCTSTR pszValue)
{
	return SetEnvironmentVariable(pszVarName, pszValue) != FALSE;
	//#ifdef _UNICODE
	//	if(_wputenv_s(pszVarName,pszValue)==0)
	//		return true;
	//#else	
	//	if(_putenv_s(pszVarName,pszValue)==0)
	//		return true;
	//#endif
	//	return false;
}



#include "EasyCriticalSectionWin.h"
#include "AutoLock.h"
#include "EasyReadWriteLockWin.h"