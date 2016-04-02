/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectProxyService.h                                  */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CDOSObjectProxyService :
	public CNetService,public CEasyThread
{
protected:
	CLIENT_PROXY_CONFIG							m_Config;
	CIDStorage<CDOSProxyConnection>				m_ConnectionPool;
	CThreadSafeIDStorage<CDOSMessagePacket *>	m_MsgQueue;
	CStaticMap<MSG_ID_TYPE,OBJECT_ID>			m_MessageMap;

	CEasyCriticalSection						m_EasyCriticalSection;

	CThreadPerformanceCounter					m_ThreadPerformanceCounter;
	

	DECLARE_CLASS_INFO_STATIC(CDOSObjectProxyService);
public:
	CDOSObjectProxyService(void);
	virtual ~CDOSObjectProxyService(void);

	BOOL Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config);

	virtual BOOL OnStart();
	virtual void OnClose();	
	virtual BOOL OnRun();

	virtual CBaseTCPConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual BOOL DeleteConnection(CBaseTCPConnection * pConnection);
	

	CDOSProxyConnection * GetConnection(UINT ID);
	float GetCPUUsedRate();
	float GetCycleTime();

	BOOL PushMessage(CDOSMessagePacket * pPacket);
	BOOL PushBroadcastMessage(CDOSMessagePacket * pPacket);

	OBJECT_ID GetGlobalMsgMapObjectID(MSG_ID_TYPE MsgID);

	CLIENT_PROXY_CONFIG& GetConfig()
	{
		return m_Config;
	}
	BYTE GetProxyType()
	{
		return m_Config.ProxyType;
	}

	UINT GetID()
	{
		return CNetService::GetID();
	}
	void SetID(UINT ID)
	{
		CNetService::SetID(ID);
	}
protected:
	int DoMessageProcess(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void OnMsg(CDOSMessage * pMessage);

	BOOL RegisterGlobalMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID);
	BOOL UnregisterGlobalMsgMap(MSG_ID_TYPE MsgID,OBJECT_ID ObjectID);

	void ClearMsgMapByRouterID(UINT RouterID);
};

inline CDOSProxyConnection * CDOSObjectProxyService::GetConnection(UINT ID)
{
	return m_ConnectionPool.GetObject(ID);
}
inline float CDOSObjectProxyService::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}

inline float CDOSObjectProxyService::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}