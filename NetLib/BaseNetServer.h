/****************************************************************************/
/*                                                                          */
/*      文件名:    BaseServer.h                                             */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CBaseNetServer :
	protected CEasyThread
{
protected:
	volatile UINT64		m_TCPRecvBytes;
	volatile UINT64		m_TCPSendBytes;
	volatile UINT64		m_UDPRecvBytes;
	volatile UINT64		m_UDPSendBytes;

	volatile UINT64		m_TCPRecvCount;
	volatile UINT64		m_TCPSendCount;
	volatile UINT64		m_UDPRecvCount;
	volatile UINT64		m_UDPSendCount;

	DECLARE_CLASS_INFO_STATIC(CBaseNetServer);
public:
	CBaseNetServer(void);
	~CBaseNetServer(void);

	void AddTCPRecvBytes(UINT64 Count)
	{
		AtomicAdd(&m_TCPRecvBytes,Count);
		AtomicInc(&m_TCPRecvCount);
	}
	void AddTCPSendBytes(UINT64 Count)
	{
		AtomicAdd(&m_TCPSendBytes,Count);
		AtomicInc(&m_TCPSendCount);
	}
	void AddUDPRecvBytes(UINT64 Count)
	{
		AtomicAdd(&m_UDPRecvBytes,Count);
		AtomicInc(&m_UDPRecvCount);
	}
	void AddUDPSendBytes(UINT64 Count)
	{
		AtomicAdd(&m_UDPSendBytes,Count);
		AtomicInc(&m_UDPSendCount);
	}
	void ResetFluxStat()
	{
		m_TCPRecvBytes=0;
		m_TCPSendBytes=0;
		m_UDPRecvBytes=0;
		m_UDPSendBytes=0;

		m_TCPRecvCount=0;
		m_TCPSendCount=0;
		m_UDPRecvCount=0;
		m_UDPSendCount=0;
	}

	virtual bool StartUp(int EventObjectPoolSize = DEFAULT_EVENT_OBJECT_COUNT,
		int WorkThreadCount = DEFAULT_WORK_THREAD_COUNT,
		int EventRouterPoolSize=DEFAULT_EVENT_ROUTER_COUNT,
		int EventObjectPoolGrowSize=DEFAULT_EVENT_OBJECT_POOL_GROW_SIZE,
		int EventObjectPoolGrowLimit=DEFAULT_EVENT_OBJECT_POOL_GROW_LIMIT,
		int EventRouterPoolGrowSize=DEFAULT_EVENT_ROUTER_POOL_GROW_SIZE,
		int EventRouterPoolGrowlimit=DEFAULT_EVENT_ROUTER_POOL_GROW_LIMIT)=0;
	bool StartUp(const STORAGE_POOL_SETTING& EventObjectPoolSetting, const STORAGE_POOL_SETTING& EventRouterPoolSetting, int WorkThreadCount)
	{
		return StartUp(EventObjectPoolSetting.StartSize, WorkThreadCount, EventRouterPoolSetting.StartSize,
			EventObjectPoolSetting.GrowSize, EventObjectPoolSetting.GrowLimit,
			EventRouterPoolSetting.GrowSize, EventRouterPoolSetting.GrowLimit);
	}
	virtual void ShutDown(DWORD Milliseconds=DEFAULT_THREAD_TERMINATE_TIME)=0;

	UINT GetThreadID()
	{
		return CEasyThread::GetThreadID();
	}

protected:
	virtual bool OnStartUp() = 0;
	virtual void OnShutDown()=0;
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT)=0;

};
