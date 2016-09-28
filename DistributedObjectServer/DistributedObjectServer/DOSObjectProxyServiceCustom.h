#pragma once

class CDOSObjectProxyServiceCustom :
	public IDOSObjectProxyServiceBase,
	public IDOSObjectProxyServiceOperator,	
	public CEasyThread
{
protected:
	CLIENT_PROXY_CONFIG										m_Config;
	OBJECT_ID												m_ObjectID;
	CCycleQueue<CDOSMessagePacket *>						m_MsgQueue;

	CThreadPerformanceCounter								m_ThreadPerformanceCounter;


	CIDStorage<CDOSObjectProxyConnectionCustom>				m_ConnectionPool;
	CLIENT_PROXY_PLUGIN_INFO								m_PluginInfo;
	CDOSServer *											m_pServer;
	IDOSObjectProxyService *								m_pProxyService;
public:
	CDOSObjectProxyServiceCustom();
	~CDOSObjectProxyServiceCustom();

	//IDOSObjectProxyServiceBase
	virtual UINT AddUseRef();
	virtual void Release();
	virtual void Destory();
	BYTE GetProxyType();
	virtual void SetID(UINT ID);
	virtual UINT GetID();
	virtual bool StartService();
	virtual void StopService();
	virtual bool PushMessage(CDOSMessagePacket * pPacket);
	virtual bool PushBroadcastMessage(CDOSMessagePacket * pPacket);
	virtual IDOSObjectProxyConnectionBase * GetConnection(UINT ID);
	virtual float GetCPUUsedRate();
	virtual float GetCycleTime();

	//IDOSObjectProxyServiceOperator
	virtual OBJECT_ID GetObjectID();
	virtual const CLIENT_PROXY_CONFIG& GetConfig();
	virtual CNetServer * GetNetServer();
	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	virtual IDOSObjectProxyConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual bool DeleteConnection(UINT ID);


	BOOL Init(CDOSServer * pServer, CLIENT_PROXY_PLUGIN_INFO& PluginInfo);	

	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	CLIENT_PROXY_PLUGIN_INFO& GetPluginInfo()
	{
		return m_PluginInfo;
	}
	CDOSServer * GetDOSServer()
	{
		return m_pServer;
	}
	UINT GetRouterID()
	{
		return m_pServer->GetRouter()->GetRouterID();
	}

protected:
	void OnMsg(CDOSMessage * pMessage);
	void OnSystemMsg(CDOSMessage * pMessage);
};

