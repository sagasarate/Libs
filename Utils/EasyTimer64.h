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

//#define MAXTIME	(UINT64(0xffffffffffffffff))
//
//inline UINT64	GetTimeToTime(UINT64 t1, UINT64 t2)
//{
//	return (t1 <= t2 ? (t2 - t1) : (MAXTIME - t1 + t2));
//}


class CEasyTimer64
{
protected:
	UINT64	m_SavedTime;
	UINT64	m_TimeoutTime;
public:
	CEasyTimer64():m_SavedTime(0),m_TimeoutTime(0)
	{

	}
	CEasyTimer64(const CEasyTimer64& Timer)
	{
		m_SavedTime=Timer.m_SavedTime;
		m_TimeoutTime=Timer.m_TimeoutTime;
	}
	CEasyTimer64& operator=(const CEasyTimer64& Timer)
	{
		m_SavedTime=Timer.m_SavedTime;
		m_TimeoutTime=Timer.m_TimeoutTime;
		return *this;
	}
#ifdef WIN32
	static inline UINT64	GetTime()
	{
		return GetTickCount64();
	}
#else
	static inline UINT64	GetTime()
	{
		struct timeval CurTime;
		gettimeofday(&CurTime,NULL);
		return CurTime.tv_sec*1000+CurTime.tv_usec/1000;
	}	
#endif
	void	SaveTime(UINT64 CurTime)
	{
		m_SavedTime = CurTime;
	}
	void	SaveTime()
	{
		SaveTime(CEasyTimer64::GetTime());
	}
	static bool	IsTimeOut(UINT64 StartTime, UINT64 Timeout )
	{
		UINT64	CurTime = CEasyTimer64::GetTime();
		if (CurTime >= StartTime + Timeout)
		{
			return true;
		}
		return false;
	}
	bool IsTimeOut(UINT64 TimeOut)
	{
		UINT64	CurTime = CEasyTimer64::GetTime();
		if (CurTime >= m_SavedTime + TimeOut)
		{
			return true;
		}
		return false;
	}
	void SetTimeOut( UINT64	dwTimeOut)
	{
		m_SavedTime = CEasyTimer64::GetTime();
		m_TimeoutTime = dwTimeOut;
	}
	bool IsTimeOut()
	{
		UINT64	CurTime = CEasyTimer64::GetTime();
		if (CurTime >= m_SavedTime + m_TimeoutTime)
			return true;
		return false;
	}
	void Reset(UINT64 dwCurTime)
	{
		m_SavedTime=dwCurTime;
	}
	void Reset()
	{
		Reset(CEasyTimer64::GetTime());
	}
	UINT64	GetLeftTime(UINT64 CurTime)
	{
		UINT64 PastTime = CurTime - m_SavedTime;
		if(PastTime >= m_TimeoutTime )
			return 0;
		return (m_TimeoutTime - PastTime);
	}
	UINT64	GetLeftTime()
	{
		return GetLeftTime(CEasyTimer64::GetTime());
	}
	UINT64 GetPastTime(UINT64 CurTime)
	{		
		return CurTime - m_SavedTime;
	}
	UINT64 GetPastTime()
	{
		return GetPastTime(CEasyTimer64::GetTime());
	}
	UINT64	GetTimeOut()
	{ 
		return m_TimeoutTime;
	}
	UINT64	GetSavedTime()
	{ 
		return m_SavedTime;
	}

};
