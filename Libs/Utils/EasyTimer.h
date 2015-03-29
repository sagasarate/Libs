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


#define MAXTIME	(DWORD(0xffffffff))

inline DWORD	GetTimeToTime(DWORD t1,DWORD t2 )
{
	return (t1<=t2?(t2-t1):(MAXTIME-t1+t2));
}

class CEasyTimer
{
protected:
	DWORD	m_dwSavedTime;
	DWORD	m_dwTimeoutTime;
public:
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
	static inline DWORD	GetTime()
	{
		return GetTickCount();
	}
#else
	static inline DWORD	GetTime()
	{
		struct timeval CurTime;
		gettimeofday(&CurTime,NULL);
		return CurTime.tv_sec*1000+CurTime.tv_usec/1000;
	}	
#endif
	void	SaveTime(DWORD dwCurTime)
	{
		m_dwSavedTime = dwCurTime;
	}
	void	SaveTime()
	{
		SaveTime(CEasyTimer::GetTime());
	}
	static BOOL	IsTimeOut( DWORD starttime, DWORD timeout )
	{
		DWORD	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(starttime, dwTime) >= timeout )
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL	IsTimeOut( DWORD dwTimeOut )
	{
		DWORD	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(m_dwSavedTime, dwTime)>= dwTimeOut )
		{
			return TRUE;
		}
		return FALSE;
	}
	void	SetTimeOut( DWORD	dwTimeOut)
	{
		m_dwSavedTime = CEasyTimer::GetTime();
		m_dwTimeoutTime = dwTimeOut;
	}
	BOOL	IsTimeOut()
	{
		DWORD	dwTime = CEasyTimer::GetTime();
		if( GetTimeToTime(m_dwSavedTime, dwTime)>= m_dwTimeoutTime )
			return TRUE;
		return FALSE;
	}
	void Reset(DWORD dwCurTime)
	{
		m_dwSavedTime=dwCurTime;
	}
	void Reset()
	{
		Reset(CEasyTimer::GetTime());
	}
	DWORD	GetLeftTime(DWORD dwCurTime)
	{
		DWORD dwTime = GetTimeToTime( m_dwSavedTime, dwCurTime );
		if( dwTime >= m_dwTimeoutTime )return 0;
		return (m_dwTimeoutTime - dwTime);
	}
	DWORD	GetLeftTime()
	{
		return GetLeftTime(CEasyTimer::GetTime());
	}
	DWORD GetPastTime(DWORD dwCurTime)
	{		
		return GetTimeToTime( m_dwSavedTime, dwCurTime );
	}
	DWORD GetPastTime()
	{
		return GetPastTime(CEasyTimer::GetTime());
	}
	DWORD	GetTimeOut(){ return m_dwTimeoutTime;}
	DWORD	GetSavedTime(){ return m_dwSavedTime;}

};
