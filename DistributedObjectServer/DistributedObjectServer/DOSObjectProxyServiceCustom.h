#pragma once

class CDOSObjectProxyServiceCustom :
	public CBaseDOSObjectProxyService,
	public IDOSObjectProxyServiceOperator
{
protected:
	CLIENT_PROXY_CONFIG										m_Config;
	OBJECT_ID												m_ObjectID;

	CLIENT_PROXY_PLUGIN_INFO								m_PluginInfo;
	CDOSServer *											m_pServer;
	IDOSObjectProxyService *								m_pProxyService;
public:
	CDOSObjectProxyServiceCustom();
	~CDOSObjectProxyServiceCustom();


	virtual void Destory();
	virtual BYTE GetProxyType();
	virtual bool StartService();
	virtual void StopService();
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket);


	virtual UINT GetConnectionCount();
	virtual float GetCPUUsedRate();
	virtual float GetCycleTime();
	virtual UINT GetGroupCount();
	virtual float GetGroupCPUUsedRate(UINT Index);
	virtual float GetGroupCycleTime(UINT Index);

	//IDOSObjectProxyServiceOperator
	virtual OBJECT_ID GetObjectID();
	virtual const CLIENT_PROXY_CONFIG& GetConfig();
	virtual CNetServer * GetNetServer();
	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	virtual bool SendMessageMulti(OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount);
	virtual bool ReleaseMessagePacket(CDOSMessagePacket * pPacket);
	virtual bool SendMessagePacket(CDOSMessagePacket * pPacket);


	BOOL Init(CDOSServer * pServer, CLIENT_PROXY_PLUGIN_INFO& PluginInfo);



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

