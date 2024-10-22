﻿/****************************************************************************/
/*                                                                          */
/*      文件名:    Utils.h                                                  */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#pragma once



#define UTILS_VERSION	"2.0.0.0"

//#define LOG_POOL_CREATE

#ifdef WIN32
#define __PRETTY_FUNCTION__	__FUNCTION__
#endif

#if defined _WIN32 || defined __CYGWIN__
	#define UTILS_DLL_IMPORT __declspec(dllimport)
	#define UTILS_DLL_EXPORT __declspec(dllexport)
	#define UTILS_DLL_LOCAL
#else
	#if __GNUC__ >= 4
		#define UTILS_DLL_IMPORT __attribute__ ((visibility("default")))
		#define UTILS_DLL_EXPORT __attribute__ ((visibility("default")))
		#define UTILS_DLL_LOCAL  __attribute__ ((visibility("hidden")))
	#else
		#define UTILS_DLL_IMPORT
		#define UTILS_DLL_EXPORT
		#define UTILS_DLL_LOCAL
	#endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <math.h>

#ifdef WIN32

#include "Win32AddOn.h"

#else

#include "LinuxAddOn.h"

#endif
#include "stacktrace/stacktrace.h"


#if defined(USE_CRT_DETAIL_NEW) && defined(WIN32) && defined(_DEBUG)
#include "CrtDebugNew.h"
#endif

#define DEFAULT_PROCESS_LIMIT	32



#define ASSERT_AND_THROW(Express)		if(!(Express)) {assert(Express);throw;}



const float PI				= float(3.14159265358979323846);
const float TWO_PI			= float(2.0 * 3.14159265358979323846);
const float HALF_PI			= float(3.14159265358979323846/ 2.0);



union UINT64_CONVERTER
{
	UINT64 QuadPart;
	struct
	{
		UINT LowPart;
		UINT HighPart;
	};
	struct
	{
		WORD Words[4];
	};
	struct
	{
		BYTE Bytes[8];
	};
};

#include "iconv/miniconv.h"
#include "ExceptionHandler.h"


enum EASY_DATA_STORAGE_MODE
{
	EDSM_STATIC,
	EDSM_NEW_ONCE,
	EDSM_NEW_EVERY_TIME,
};

struct STORAGE_POOL_SETTING
{
	UINT	StartSize;
	UINT	GrowSize;
	UINT	GrowLimit;
	STORAGE_POOL_SETTING()
	{
		StartSize = 128;
		GrowSize = 0;
		GrowLimit = 0;
	}
	STORAGE_POOL_SETTING(UINT Start, UINT Grow, UINT Limit)
	{
		StartSize = Start;
		GrowSize = Grow;
		GrowLimit = Limit;
	}
	UINT MaxSize()
	{
		return StartSize + GrowSize*GrowLimit;
	}
	void LimitSize(UINT Size)
	{
		if (StartSize > Size)
		{
			StartSize = Size;
			GrowSize = 0;
			GrowLimit = 0;
		}
		else if (GrowSize)
		{
			GrowLimit = (Size - StartSize) / GrowSize;
		}
	}
};

#include "StaticObject.h"
#include "StaticMapLite.h"
#include "MemoryLeakDetective.h"
#include "MemoryAllocatee.h"


#include "SmartPtr.h"

#include "EasyArray.h"
#include "EasyBuffer.h"

#include "EasyString.h"


#include "EasyTime.h"
#include "EasyTimer.h"
#include "EasyTimer64.h"
#include "EasyTimerEx.h"

//#include "IndexSet.h"


#include "StringBuilder.h"

#include "EasyMap.h"
#include "EasyList.h"
#include "ListIterator.h"
#include "IDStorage.h"
#include "StaticMap.h"
#include "HashMap.h"
#include "CycleQueue.h"

#include "VariedValue.h"


#include "StringSplitter.h"

#include "WordFilter.h"


#include "SmartValue.h"
#include "SmartArray.h"
#include "SmartStruct.h"


#include "AutoReadLock.h"
#include "AutoWriteLock.h"


#include "NameObject.h"

#include "ClassifiedID.h"

#ifdef WIN32
#include "EasyThreadWin.h"
#else
#include "EasyThreadLinux.h"
#endif
#include "GuardThread.h"






#include "LZMA/LzmaLib.h"
#include "LZO/minilzo.h"








#ifdef WIN32

#include "ExceptionParser.h"

#endif



#ifdef WIN32
#include "ToolsWin.h"
#else
#include "ToolsLinux.h"
#endif

#include "ToolsAll.h"




#include "GrowBuffer.h"
#include "CycleBuffer.h"
#include "CycleBufferEx.h"
#include "ThreadSafeList.h"
#include "ThreadSafeIDStorage.h"
#include "ThreadSafeCycleBuffer.h"
#include "ThreadSafeCycleBufferEx.h"
#include "ThreadSafeCycleQueue.h"
#include "ThreadSafeStaticMap.h"


#include "PerformanceStatistician.h"
#include "AutoPerformanceCounter.h"
#include "ThreadPerformanceCounter.h"

#include "FileInfo.h"

#include "IFileAccessor.h"
#include "IFileObjectCreator.h"
#include "FileSystemManager.h"

#include "StandardFileAccessor.h"
#include "StandardFileObjectCreator.h"

#ifdef WIN32
#include "WinFileAccessor.h"
#include "WinFileAccessorObjectCreator.h"
#include "ResourceFileAccessor.h"
#include "ResourceFileObjectCreator.h"
#else
#include "LinuxFileAccessor.h"
#include "LinuxFileAccessorObjectCreator.h"
#endif

#include "FilePathManager.h"

#include "ILogPrinter.h"
#include "AsyncLogWorkThread.h"

#include "VSOutputLogPrinter.h"
#include "FileLogPrinter.h"
#include "AsyncFileLogPrinter.h"
#include "CSVFileLogPrinter.h"
#include "LogManager.h"


#include "FileTools.h"

#include "SettingFile.h"
#include "StringFile.h"
#include "CSVReader.h"

#ifdef WIN32
#include "FileSearcherWin.h"
#else
#include "FileSearcherLinux.h"
#endif


#ifdef WIN32
#pragma warning (push)
#pragma warning (disable : 4267)	// type conversion
#pragma warning (disable : 4311)	// pointer truncation
#pragma warning (disable : 4244)	// type conversion
#pragma warning (disable : 6011)
#pragma warning (disable : 28182)
#pragma warning (disable : 6387)
#pragma warning (disable : 6308)

#include "pugxml.h"
using namespace pug;

#pragma warning (pop)
#else
#include <iterator>
#include "pugxml.h"
using namespace pug;
#endif








#include "FloatRect.h"

#ifdef WIN32
#include "WinTypeWrap.h"
#include "TreeObject.h"
#include "USOObjectCreateFilter.h"
#include "USOResourceManager.h"
#endif




#include "FastMemoryPool.h"

#include "EasyScriptTypes.h"
#include "ESVariableList.h"
#include "ESFunctionList.h"
#include "ESBolanStack.h"
#include "ESThread.h"
#include "ESFunctionLib.h"
#include "EasyScriptExecutor.h"

#include "LZWPack.h"
#ifdef WIN32
#include "Gif.h"
#endif



#include "HashMD5.h"
#include "Base64.h"
#include "Base32.h"
#include "SHA1.h"
#include "HMAC_SHA1.h"
#include "SHA256.h"
#include "URLEncode.h"
#include "TEA.h"
#include "Crypto/aes.h"
#include "Crypto/des.h"
#include "CCRCMaker.h"
#include "CryptTools.h"

#include "ProcessSnapshot.h"
#ifdef WIN32
#include "WinServiceController.h"
#endif

#include "TimerQueue.h"

#ifdef WIN32
#include "NamedPipeWin.h"
#else
#include "NamedPipeLinux.h"
#endif