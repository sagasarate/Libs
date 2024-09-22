#pragma once


#include "jsvm_interface.h"

#define LOG_JSVM_CHANNEL				11501

inline void LogJSVMWithTag(const char* szTag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_JSVM_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, szTag, Format, vl);
	va_end(vl);
}

inline void LogJSVMDebugWithTag(const char* szTag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_JSVM_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, szTag, Format, vl);
	va_end(vl);
}

#ifdef WIN32
#define LogJSVM(_Format, ...)	LogJSVMWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#define LogJSVMDebug(_Format, ...)	LogJSVMDebugWithTag(__FUNCTION__, _Format, ##__VA_ARGS__)
#else
#define LogJSVM(_Format, ...)	LogJSVMWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#define LogJSVMDebug(_Format, ...)	LogJSVMDebugWithTag(__PRETTY_FUNCTION__, _Format, ##__VA_ARGS__)
#endif

#include "TypeWrap.h"
#include "CallWrap.h"
