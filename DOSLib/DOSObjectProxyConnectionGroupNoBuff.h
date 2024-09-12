#pragma once

class CDOSObjectProxyServiceNoBuff;

class CDOSObjectProxyConnectionGroupNoBuff :
	public CEasyThread
{
protected:
	CDOSObjectProxyServiceNoBuff *								m_pService;
	CLIENT_PROXY_CONFIG											m_Config;
	UINT														m_Index;
	CStaticMap<UINT, CDOSObjectProxyConnectionNoBuff*>			m_ConnectionPool;
	CEasyCriticalSection										m_EasyCriticalSection;
	CThreadPerformanceCounter									m_ThreadPerformanceCounter;
	CEasyBuffer													m_CompressBuffer;
	CEasyBuffer													m_EncyptBuffer;
	char														m_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

	CCycleQueue<DISPATCHED_MSG>									m_MsgQueue;

public:
	CDOSObjectProxyConnectionGroupNoBuff();
	~CDOSObjectProxyConnectionGroupNoBuff();

	bool Init(CDOSObjectProxyServiceNoBuff * pService, CLIENT_PROXY_CONFIG& Config, UINT Index);
	bool AddConnection(CDOSObjectProxyConnectionNoBuff * pConnection);
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
	virtual BOOL OnStart() override;
	virtual BOOL OnRun() override;
	virtual void OnTerminate() override;

	bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket* pPacket);
protected:
	void ProcessMsg(DISPATCHED_MSG& Msg);
	void OnMsg(CDOSMessage* pMessage);
	void OnSystemMsg(CDOSMessagePacket* pPacket);
};

