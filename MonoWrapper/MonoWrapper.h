#pragma once

//#include <mono/jit/jit.h>
//#include <mono/metadata/environment.h>
//#include <mono/metadata/assembly.h>
//#include <mono/metadata/mono-config.h>
////#include <mono/metadata/mono-config-dirs.h>
//#include <mono/metadata/mono-debug.h>
//#include <mono/mini/debugger-agent.h>
//#include <mono/metadata/threads.h>
//#include <mono/metadata/mono-gc.h>
//#include <mono/utils/mono-dl-fallback.h>
//#include <mono/utils/mono-logger.h>

#include "../../Libs/Utils/Utils.h"

#define LOG_MONO_CHANNEL				11401

inline void LogMonoWithTag(LPCTSTR Tag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_MONO_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, 0, Format, vl);
	va_end(vl);
}

inline void LogMonoDebugWithTag(LPCTSTR Tag, const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_MONO_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, 0, Format, vl);
	va_end(vl);
}

#define LogMono(_Format, ...)	LogMonoWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#define LogMonoDebug(_Format, ...)	LogMonoDebugWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)

#include "mono_api_mini.h"

#define SAFE_MONO_FREE(p) if(p) mono_free(p)