#pragma once

class CDOSObjectProxyServiceDefault;

class CDOSObjectProxyConnectionGroup :
	public CEasyThread
{
protected:
	CDOSObjectProxyServiceDefault *								m_pService;
	CLIENT_PROXY_CONFIG											m_Config;
	UINT														m_Index;
	CStaticMap<UINT, CDOSObjectProxyConnectionDefault*>			m_ConnectionPool;
	CEasyCriticalSection										m_EasyCriticalSection;
	CThreadPerformanceCounter									m_ThreadPerformanceCounter;
	CEasyBuffer													m_CompressBuffer;
	CEasyBuffer													m_EncyptBuffer;
	char														m_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

	CCycleQueue<DISPATCHED_MSG>									m_MsgQueue;

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
	UINT GetMsgQueueLen()
	{
		return m_MsgQueue.GetUsedSize();
	}
	virtual bool OnStart() override;
	virtual bool OnRun() override;
	virtual void OnTerminate() override;

	bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket* pPacket);
protected:
	void ProcessMsg(DISPATCHED_MSG& Msg);
	void OnMsg(CDOSMessage* pMessage);
	void OnSystemMsg(CDOSMessagePacket* pPacket);
};

