#pragma once

class CDOSObjectProxyServiceCustom :
	public CBaseDOSObjectProxyService,
	public IDOSObjectProxyServiceOperator,
	public CNameObject
{
protected:
	CLIENT_PROXY_CONFIG										m_Config;

	CLIENT_PROXY_PLUGIN_INFO								m_PluginInfo;
	CDOSServer *											m_pServer;
	IDOSObjectProxyService *								m_pProxyService;
public:
	CDOSObjectProxyServiceCustom();
	~CDOSObjectProxyServiceCustom();


	virtual void Release() override;
	virtual void Destory() override;
	virtual UINT AddUseRef() override;	
	virtual BYTE GetProxyType() override;
	virtual void SetID(UINT ID) override;
	virtual UINT GetID() override;
	virtual bool StartService() override;
	virtual bool StartService(IDOSObjectProxyServiceOperator* pOperator) override;
	virtual void StopService() override;
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket) override;


	virtual UINT GetConnectionCount() override;
	virtual float GetCPUUsedRate() override;
	virtual float GetCycleTime() override;
	virtual UINT GetMsgQueueLen() override;
	virtual UINT GetGroupCount() override;
	virtual float GetGroupCPUUsedRate(UINT Index) override;
	virtual float GetGroupCycleTime(UINT Index) override;
	virtual UINT GetGroupMsgQueueLen(UINT Index) override;

	//IDOSObjectProxyServiceOperator	
	virtual const CLIENT_PROXY_CONFIG& GetConfig() override;
	virtual CNetServer * GetNetServer() override;
	virtual UINT GetRouterID() override;
	virtual bool SendMessage(OBJECT_ID SenderID, OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual bool SendMessageMulti(OBJECT_ID SenderID, OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount) override;
	virtual bool ReleaseMessagePacket(CDOSMessagePacket * pPacket) override;
	virtual bool SendMessagePacket(CDOSMessagePacket * pPacket) override;


	BOOL Init(CDOSServer * pServer, CLIENT_PROXY_PLUGIN_INFO& PluginInfo);



	CLIENT_PROXY_PLUGIN_INFO& GetPluginInfo()
	{
		return m_PluginInfo;
	}
	CDOSServer * GetDOSServer()
	{
		return m_pServer;
	}

protected:
};

