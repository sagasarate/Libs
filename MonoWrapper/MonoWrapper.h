#pragma once

//#ifdef WIN32
//
#include "include/jit.h"
#include "include/environment.h"
#include "include/assembly.h"
#include "include/mono-config.h"
#include "include/mono-config-dirs.h"
//#include "include/class-internals.h"
#include "include/mono-debug.h"
#include "include/debugger-agent.h"
#include "include/mono-logger.h"
#include "include/threads.h"
#include "include/mono-gc.h"
#include "include/mono-dl-fallback.h"
//
//#else
//
//#include "include_win/jit.h"
//#include "include_win/environment.h"
//#include "include_win/assembly.h"
//#include "include_win/mono-config.h"
//#include "include_win/mono-config-dirs.h"
//#include "include_win/class-internals.h"
//#include "include_win/mono-debug.h"
//#include "include_win/debugger-agent.h"
//#include "include_win/mono-logger.h"
//#include "include_win/threads.h"
//#include "include_win/mono-gc.h"
//
//#endif






inline void LogMono(const char* Format, ...)
{


	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_MONO_CHANNEL, ILogPrinter::LOG_LEVEL_NORMAL, 0, Format, vl);
	va_end(vl);



}

inline void LogMonoDebug(const char* Format, ...)
{
	va_list vl;

	va_start(vl, Format);
	CLogManager::GetInstance()->PrintLogVL(LOG_MONO_CHANNEL, ILogPrinter::LOG_LEVEL_DEBUG, 0, Format, vl);
	va_end(vl);
}