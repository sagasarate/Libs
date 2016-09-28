#pragma once

class CDOSObjectProxyServiceDefault;

class CDOSObjectProxyConnectionGroup :
	public CEasyThread
{
protected:
	CDOSObjectProxyServiceDefault *								m_pService;
	CLIENT_PROXY_CONFIG											m_Config;
	UINT														m_Index;
	CIDStorage<CDOSObjectProxyConnectionDefault *>				m_ConnectionPool;
	CEasyCriticalSection										m_EasyCriticalSection;
	CThreadPerformanceCounter									m_ThreadPerformanceCounter;
	CEasyBuffer													m_CompressBuffer;

public:
	CDOSObjectProxyConnectionGroup();
	~CDOSObjectProxyConnectionGroup();

	bool Init(CDOSObjectProxyServiceDefault * pService, CLIENT_PROXY_CONFIG& Config, UINT Index);
	bool AddConnection(CDOSObjectProxyConnectionDefault * pConnection);
	UINT GetConnectionCount()
	{
		return m_ConnectionPool.GetObjectCount();
	}
	float GetCPUUsedRate()
	{
		return m_ThreadPerformanceCounter.GetCPUUsedRate();
	}
	float GetCycleTime()
	{
		return m_ThreadPerformanceCounter.GetCycleTime();
	}

	BOOL OnStart();
	virtual BOOL OnRun();
};

