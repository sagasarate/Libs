#pragma once

class CDOSRouter;
class CDOSRouterLinkManager :
	public CEasyNetLinkManager, public CEasyThread
{
protected:
	struct MSG_SEND_INFO
	{
		OBJECT_ID			ObjectID;
		CDOSMessagePacket * pPacket;
	};
	CDOSServer *								m_pServer;
	CDOSRouter *								m_pRouter;
	CEasyNetLinkManager::ENL_CONFIG				m_RouterLinkConfig;
	CEasyCriticalSection						m_EasyCriticalSection;
	CThreadPerformanceCounter					m_ThreadPerformanceCounter;
public:
	CDOSRouterLinkManager();
	~CDOSRouterLinkManager();

	bool Init(CDOSServer * pServer);

	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	virtual CEasyNetLink * OnCreateLink(UINT ID);
	virtual void OnLinkStart(CEasyNetLink * pConnection);
	virtual void OnLinkEnd(CEasyNetLink * pConnection);

	bool SendMessage(UINT RouterID, CDOSMessagePacket * pPacket);
	UINT BroadcastMessage(CDOSMessagePacket * pPacket);

	CDOSRouter * GetRouter()
	{
		return m_pRouter;
	}

	float GetCPUUsedRate()
	{
		return m_ThreadPerformanceCounter.GetCPUUsedRate();
	}
	float GetCycleTime()
	{
		return m_ThreadPerformanceCounter.GetCycleTime();
	}
};

