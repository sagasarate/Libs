#include "stdafx.h"


CThreadPerformanceCounter::CThreadPerformanceCounter(void)
{
	m_TreadHandle=NULL;
	m_CountIntervalTime=0;
	m_StartTime=0;
	m_CycleCount=0;
	m_CPUCount=1;
	m_StartCPUUsedTime=0;
	m_CycleTime=0;
	m_CPUUsedRate=0;
	m_CycleCPUUsedTime = 0;
}

CThreadPerformanceCounter::~CThreadPerformanceCounter(void)
{
}

void CThreadPerformanceCounter::Init(HANDLE ThreadHandle,UINT CountIntervalTime)
{
	m_TreadHandle=ThreadHandle;
	m_CountIntervalTime=CountIntervalTime;
	m_CountIntervalTimer.SaveTime();
	m_StartTime=CEasyTimerEx::GetTime();
	m_CycleCount=0;

#ifdef WIN32
	SYSTEM_INFO	si;
	GetSystemInfo( &si );
	m_CPUCount=si.dwNumberOfProcessors;

	FILETIME CreationTime,ExitTime,KernelTime,UserTime;
	if(!GetThreadTimes(m_TreadHandle,
		&CreationTime,&ExitTime,
		&KernelTime,&UserTime))
	{
		PrintSystemLog(_T("获取线程CPU时间失败%d"),GetLastError());
	}
	m_StartCPUUsedTime=((UINT64)KernelTime.dwLowDateTime)|(((UINT64)KernelTime.dwHighDateTime)<<32);
	m_StartCPUUsedTime+=((UINT64)UserTime.dwLowDateTime)|(((UINT64)UserTime.dwHighDateTime)<<32);
	m_StartCPUUsedTime *= 100;
#else
	m_CPUCount=sysconf(_SC_NPROCESSORS_CONF);

	timespec OrginTime;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID,&OrginTime);
	m_StartCPUUsedTime=(UINT64)OrginTime.tv_sec*1000000000+OrginTime.tv_nsec;
#endif
}

void CThreadPerformanceCounter::DoPerformanceCount()
{
	if(m_TreadHandle)
	{
		m_CycleCount++;
		if(m_CountIntervalTimer.IsTimeOut(m_CountIntervalTime))
		{
			m_CountIntervalTimer.SaveTime();

			UINT64 CurTime = CEasyTimerEx::GetTime();
			UINT64 CPUTime = CurTime - m_StartTime;
			m_StartTime = CurTime;

			m_CycleTime = CPUTime*1000.0f / m_CycleCount / CEasyTimerEx::TIME_UNIT_PER_SECOND;
			

			UINT64 CurCPUUsedTime;
#ifdef WIN32			
			FILETIME CreationTime, ExitTime, KernelTime, UserTime;
			if (!GetThreadTimes(m_TreadHandle,
				&CreationTime, &ExitTime,
				&KernelTime, &UserTime))
			{
				PrintSystemLog(_T("获取线程CPU时间失败%d"), GetLastError());
			}
			CurCPUUsedTime = ((UINT64)KernelTime.dwLowDateTime) | (((UINT64)KernelTime.dwHighDateTime) << 32);
			CurCPUUsedTime += ((UINT64)UserTime.dwLowDateTime) | (((UINT64)UserTime.dwHighDateTime) << 32);
			CurCPUUsedTime *= 100;
#else
			timespec OrginTime;
			clock_gettime(CLOCK_THREAD_CPUTIME_ID,&OrginTime);
			CurCPUUsedTime=(UINT64)OrginTime.tv_sec*1000000000+OrginTime.tv_nsec;
#endif
			m_CycleCPUUsedTime = CurCPUUsedTime - m_StartCPUUsedTime;
			m_CPUUsedRate = (float)m_CycleCPUUsedTime / ((float)CPUTime*m_CPUCount);
			m_CycleCPUUsedTime = m_CycleCPUUsedTime / m_CycleCount;

			m_StartCPUUsedTime=CurCPUUsedTime;
			m_RecentCycleCount = m_CycleCount;
			m_CycleCount = 0;
		}
	}
}
