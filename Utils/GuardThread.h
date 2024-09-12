/****************************************************************************/
/*                                                                          */
/*      文件名:    GuardThread.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class IFileAccessor;

class CGuardThread :
	public CEasyThread
{
protected:
	UINT					m_RecentKeepAliveTime;
	UINT					m_KeepAliveTime;
	UINT					m_LostAliveCount;
	UINT					m_MaxLostAliveCount;
	UINT					m_TargetThreadID;
	HANDLE					m_TargetThreadHandle;
	CEasyCriticalSection	m_EasyCriticalSection;

	static volatile bool	m_Enable;

	DECLARE_CLASS_INFO_STATIC(CGuardThread)
public:
	CGuardThread(void);
	~CGuardThread(void);

	void SetKeepAliveTime(UINT Time,UINT MaxLostAliveCount)
	{
		m_KeepAliveTime=Time;
		m_MaxLostAliveCount=MaxLostAliveCount;
	}
	void SetTargetThread(CEasyThread * pThread)
	{
		m_TargetThreadID = pThread->GetThreadID();
		m_TargetThreadHandle = pThread->GetThreadHandle();
	}

	void MakeKeepAlive()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		m_RecentKeepAliveTime=CEasyTimer::GetTime();
		m_LostAliveCount=0;
	}

	static void Enable(bool Enable);

protected:
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();
	void PrintDumpLog(IFileAccessor * pLogFile, LPCTSTR Format, ...);
	void DumpThreadCallStack();
};
