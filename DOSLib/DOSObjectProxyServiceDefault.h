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

class CBaseDOSObjectProxyService:
	public IDOSObjectProxyService
{
public:
	virtual ~CBaseDOSObjectProxyService()
	{

	}
	virtual BYTE GetProxyType() = 0;
	virtual void SetID(UINT ID) = 0;
	virtual UINT GetID() = 0;
	virtual bool StartService() = 0;
};


class CDOSObjectProxyServiceDefault :
	public CBaseDOSObjectProxyService,
	public CNetService, 
	public CEasyThread
{
protected:
	struct MSG_MAP_INFO
	{
		int						MsgMapType;
		CEasyArray<OBJECT_ID>	ObjectIDList;
		MSG_MAP_INFO()
			:ObjectIDList(_T("MSG_MAP_INFO"))
		{
			ObjectIDList.SetTag(_T("CDOSObjectProxyServiceDefault"));
			ObjectIDList.Create(16, 16);
			MsgMapType = GLOBAL_MSG_MAP_TYPE_OVERLAP;
		}
	};

	struct IP_INFO
	{
		UINT		ExpireTime;
		CIPAddress	IP;
	};

	CLIENT_PROXY_CONFIG													m_Config;
	CCycleQueue<DISPATCHED_MSG>											m_MsgQueue;

	CStaticMap<MSG_ID_TYPE, MSG_MAP_INFO>								m_MessageMap;
	OBJECT_ID															m_UnhandleMsgReceiverID;
	CEasyCriticalSection												m_EasyCriticalSection;

	CThreadPerformanceCounter											m_ThreadPerformanceCounter;
	CGuardThread														m_GuardThread;


	CIDStorage<CDOSObjectProxyConnectionDefault, EDSM_NEW_ONCE>			m_ConnectionPool;
	UINT																m_FreeObjectCheckPtr;
	CThreadSafeIDStorage<CDOSObjectProxyConnectionDefault *>			m_DestoryConnectionList;
	CEasyArray<CDOSObjectProxyConnectionGroup>							m_ConnectionGroups;
	CEasyBuffer															m_CompressBuffer;
	CEasyBuffer															m_EncyptBuffer;
	char																m_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

	CHashMap<CIPAddress, IP_INFO>										m_IPBlackList;
	CHashMap<CIPAddress, IP_INFO>										m_RecvProtectedIPList;
	CEasyArray<IP_INFO>													m_PrepareIPBlackList;
	CEasyCriticalSection												m_BlackListCriticalSection;
	CEasyTimer															m_BlackListUpdateTimer;

public:
	CDOSObjectProxyServiceDefault(void);
	virtual ~CDOSObjectProxyServiceDefault(void);
	
	virtual void Release() override;
	virtual void Destory() override;
	virtual UINT AddUseRef() override;
	virtual UINT GetUseRef();
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


	bool Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config);
	


	virtual BOOL OnStart() override;
	virtual BOOL OnRun() override;
	virtual void OnTerminate() override;

	virtual void OnClose() override;
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;
	virtual CBaseNetConnection * CreateConnection(CIPAddress& RemoteAddress) override;
	virtual bool DeleteConnection(CBaseNetConnection * pConnection) override;
	virtual CDOSObjectProxyConnectionDefault* GetConnection(UINT ID);
	virtual LPVOID GetFirstConnectionPos();
	virtual CDOSObjectProxyConnectionDefault* GetNextConnection(LPVOID& Pos);

	void AcceptConnection(CDOSObjectProxyConnectionDefault * pConnection);
	void QueryDestoryConnection(CDOSObjectProxyConnectionDefault * pConnection);


	bool HaveGlobalMsgMap(MSG_ID_TYPE MsgID);
	bool SendGlobalMapMessage(OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize);

	CLIENT_PROXY_CONFIG& GetConfig()
	{
		return m_Config;
	}

	CEasyBuffer& GetCompressBuffer()
	{
		return m_CompressBuffer;
	}
	CDOSServer * GetServer();

	bool AddBlackList(CIPAddress IP, UINT Duration);
	bool OnRecvProtected(CIPAddress IP);
protected:
	void ProcessMsg(DISPATCHED_MSG& Msg);
	void OnMsg(CDOSMessage * pMessage);
	void OnSystemMsg(CDOSMessagePacket * pPacket);
	bool DoRegisterGlobalMsgMap(MSG_ID_TYPE MsgID, int MapType, OBJECT_ID ObjectID);
	bool DoUnregisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	void ClearMsgMapByRouterID(UINT RouterID);
	bool CheckEncryptConfig();
	virtual int CheckFreeObject();
};



