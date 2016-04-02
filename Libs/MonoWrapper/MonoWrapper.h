#pragma once

#ifdef WIN32

#include "include_win/jit.h"
#include "include_win/environment.h"
#include "include_win/assembly.h"
#include "include_win/mono-config.h"
#include "include_win/mono-config-dirs.h"
#include "include_win/class-internals.h"
#include "include_win/mono-debug.h"
#include "include_win/debugger-agent.h"
#include "include_win/mono-logger.h"
#include "include_win/threads.h"
#include "include_win/mono-gc.h"

#else

#include "include_win/jit.h"
#include "include_win/environment.h"
#include "include_win/assembly.h"
#include "include_win/mono-config.h"
#include "include_win/mono-config-dirs.h"
#include "include_win/class-internals.h"
#include "include_win/mono-debug.h"
#include "include_win/debugger-agent.h"
#include "include_win/mono-logger.h"
#include "include_win/threads.h"
#include "include_win/mono-gc.h"

#endif




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