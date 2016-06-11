/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyTimer.h                                              */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define MAXTIME64	(UINT64(0xffffffffffffffffLL))


inline UINT64	GetTimeToTimeEx(UINT64 t1,UINT64 t2 )
{
	return (t1<=t2?(t2-t1):(MAXTIME64-t1+t2));
}

class CEasyTimerEx
{
protected:
	UINT64	m_dwSavedTime;
	UINT64	m_dwTimeoutTime;
	static UINT64	m_PerformanceFrequency;

	friend class CQueryPerformanceFrequency;
public:
	static const UINT64 TIME_UNIT_PER_SECOND=1000000000;

	CEasyTimerEx():m_dwSavedTime(0),m_dwTimeoutTime(0)
	{

	}
	CEasyTimerEx(const CEasyTimerEx& Timer)
	{
		m_dwSavedTime=Timer.m_dwSavedTime;
		m_dwTimeoutTime=Timer.m_dwTimeoutTime;
	}
	CEasyTimerEx& operator=(const CEasyTimerEx& Timer)
	{
		m_dwSavedTime=Timer.m_dwSavedTime;
		m_dwTimeoutTime=Timer.m_dwTimeoutTime;
		return *this;
	}

	static inline UINT64	GetTime()
	{
		UINT64 Time;
#ifdef WIN32
		QueryPerformanceCounter((LARGE_INTEGER*)&Time);
		Time=(UINT64)((float)Time/(float)m_PerformanceFrequency)*TIME_UNIT_PER_SECOND;
#else
		timespec OrginTime;		
		clock_gettime(CLOCK_MONOTONIC,&OrginTime);
		Time=(UINT64)OrginTime.tv_sec*1000000000+OrginTime.tv_nsec;
#endif
		return Time;
	}

	static inline UINT64	GetTimeOrigin()
	{
		UINT64 Time;
#ifdef WIN32
		QueryPerformanceCounter((LARGE_INTEGER*)&Time);
#else
		timespec OrginTime;		
		clock_gettime(CLOCK_MONOTONIC,&OrginTime);
		Time=(UINT64)OrginTime.tv_sec*1000000000+OrginTime.tv_nsec;
#endif
		return Time;
	}

	void SaveTime(UINT64 dwCurTime)
	{
		m_dwSavedTime = dwCurTime;
	}
	void SaveTime()
	{
		SaveTime(CEasyTimerEx::GetTime());
	}
	static BOOL	IsTimeOut( UINT64 starttime, UINT64 timeout )
	{
		UINT64	dwTime = CEasyTimerEx::GetTime();
		if( GetTimeToTimeEx(starttime, dwTime) >= timeout )
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL	IsTimeOut( UINT64 dwTimeOut )
	{
		UINT64	dwTime = CEasyTimerEx::GetTime();
		if( GetTimeToTimeEx(m_dwSavedTime, dwTime)>= dwTimeOut )
		{
			return TRUE;
		}
		return FALSE;
	}
	void	SetTimeOut( UINT64	dwTimeOut)
	{
		m_dwSavedTime = CEasyTimerEx::GetTime();
		m_dwTimeoutTime = dwTimeOut;
	}
	BOOL	IsTimeOut()
	{
		UINT64	dwTime = CEasyTimerEx::GetTime();
		if( GetTimeToTimeEx(m_dwSavedTime, dwTime)>= m_dwTimeoutTime )
			return TRUE;
		return FALSE;
	}
	void Reset(UINT64 dwCurTime)
	{
		m_dwSavedTime=dwCurTime;
	}
	void Reset()
	{
		Reset(CEasyTimerEx::GetTime());
	}
	UINT64	GetLeftTime(UINT64 dwCurTime)
	{
		UINT64 dwTime = GetTimeToTimeEx( m_dwSavedTime, dwCurTime );
		if( dwTime >= m_dwTimeoutTime )return 0;
		return (m_dwTimeoutTime - dwTime);
	}
	UINT64	GetLeftTime()
	{
		return GetLeftTime(CEasyTimerEx::GetTime());
	}
	UINT64 GetPastTime(UINT64 dwCurTime)
	{		
		return GetTimeToTimeEx( m_dwSavedTime, dwCurTime );
	}
	UINT64 GetPastTime()
	{
		return GetPastTime(CEasyTimerEx::GetTime());
	}
	UINT64	GetTimeOut()
	{ 
		return m_dwTimeoutTime;
	}
	UINT64	GetSavedTime()
	{ 
		return m_dwSavedTime;
	}

};

#ifdef WIN32
class CQueryPerformanceFrequency
{
public:
	CQueryPerformanceFrequency()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&CEasyTimerEx::m_PerformanceFrequency);		
	}
};
#endif