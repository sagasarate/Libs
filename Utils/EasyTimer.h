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

class CEasyTimer
{
protected:
	UINT32	m_dwSavedTime;
	UINT32	m_dwTimeoutTime;
public:
	const static UINT32 MAXTIME = UINT32(0xffffffff);

	static inline UINT32 GetTimeToTime(UINT32 t1, UINT32 t2)
	{
		return (t1 <= t2 ? (t2 - t1) : (MAXTIME - t1 + t2));
	}

	CEasyTimer():m_dwSavedTime(0),m_dwTimeoutTime(0)
	{

	}
	CEasyTimer(const CEasyTimer& Timer)
	{
		m_dwSavedTime=Timer.m_dwSavedTime;
		m_dwTimeoutTime=Timer.m_dwTimeoutTime;
	}
	CEasyTimer& operator=(const CEasyTimer& Timer)
	{
		m_dwSavedTime=Timer.m_dwSavedTime;
		m_dwTimeoutTime=Timer.m_dwTimeoutTime;
		return *this;
	}
#ifdef WIN32
	static inline UINT32	GetTime()
	{
		return GetTickCount();
	}
#else
	static inline UINT32	GetTime()
	{
		struct timeval CurTime;
		gettimeofday(&CurTime,NULL);
		return CurTime.tv_sec*1000+CurTime.tv_usec/1000;
	}	
#endif
	void	SaveTime(UINT32 dwCurTime)
	{
		m_dwSavedTime = dwCurTime;
	}
	void	SaveTime()
	{
		SaveTime(CEasyTimer::GetTime());
	}
	static bool	IsTimeOut( UINT32 starttime, UINT32 timeout )
	{
		UINT32	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(starttime, dwTime) >= timeout )
		{
			return true;
		}
		return false;
	}
	bool	IsTimeOut( UINT32 dwTimeOut )
	{
		UINT32	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(m_dwSavedTime, dwTime)>= dwTimeOut )
		{
			return true;
		}
		return false;
	}
	void	SetTimeOut( UINT32	dwTimeOut)
	{
		m_dwSavedTime = CEasyTimer::GetTime();
		m_dwTimeoutTime = dwTimeOut;
	}
	bool	IsTimeOut()
	{
		UINT32	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(m_dwSavedTime, dwTime)>= m_dwTimeoutTime )
			return true;
		return false;
	}
	void Reset(UINT32 dwCurTime)
	{
		m_dwSavedTime=dwCurTime;
	}
	void Reset()
	{
		Reset(CEasyTimer::GetTime());
	}
	UINT32	GetLeftTime(UINT32 dwCurTime)
	{
		UINT32 dwTime = GetTimeToTime( m_dwSavedTime, dwCurTime );
		if( dwTime >= m_dwTimeoutTime )return 0;
		return (m_dwTimeoutTime - dwTime);
	}
	UINT32	GetLeftTime()
	{
		return GetLeftTime(CEasyTimer::GetTime());
	}
	UINT32 GetPastTime(UINT32 dwCurTime)
	{		
		return GetTimeToTime( m_dwSavedTime, dwCurTime );
	}
	UINT32 GetPastTime()
	{
		return GetPastTime(CEasyTimer::GetTime());
	}
	UINT32	GetTimeOut(){ return m_dwTimeoutTime;}
	UINT32	GetSavedTime(){ return m_dwSavedTime;}

};
