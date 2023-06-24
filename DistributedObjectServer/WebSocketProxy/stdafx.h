// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#ifdef WIN32
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include <afxwin.h>         // MFC 核心和标准组件

#endif


#include "../../Libs/Utils/Utils.h"
#include "../../Libs/NetLib/NetLib.h"
#include "../../Libs/DOSLib/DOSLib.h"


// TODO: 在此处引用程序需要的其他头文件



#ifdef WIN32
#define USE_CRT_DETAIL_NEW
#endif


#define CONFIG_FILE_NAME					"WebSocketProxy.xml"


extern UINT g_LogChannel;

inline void LogWithTag(const char* szTag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(g_LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, szTag, Format, vl);
	va_end(vl);
}

inline void LogDebugWithTag(const char* szTag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(g_LogChannel, ILogPrinter::LOG_LEVEL_DEBUG, szTag, Format, vl);
	va_end(vl);
}

#ifdef WIN32
#define Log(_Format, ...)	LogWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#define LogDebug(_Format, ...)	LogDebugWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#else
#define Log(_Format, ...)	LogWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define LogDebug(_Format, ...)	LogDebugWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#endif





#include "MainConfig.h"
#include "WebSocketProxyConnection.h"
#include "WebSocketProxyService.h"





