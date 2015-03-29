#include "stdafx.h"

UINT64	CEasyTimerEx::m_PerformanceFrequency=0;

#ifdef WIN32

CQueryPerformanceFrequency g_QueryPerformanceFrequency;


#endif
