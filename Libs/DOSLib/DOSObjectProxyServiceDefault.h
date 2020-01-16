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
	public CNameObject
{
public:
	virtual BYTE GetProxyType() = 0;
	virtual void SetID(UINT ID)
	{
		CNameObject::SetID(ID);
	}
	virtual UINT GetID()
	{
		return CNameObject::GetID();
	}

	virtual bool StartService() = 0;
	virtual void StopService() = 0;
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket) = 0;

	virtual UINT GetConnectionCount(){ return 0; }
	virtual float GetCPUUsedRate(){ return 0; }
	virtual float GetCycleTime(){ return 0; }
	virtual UINT GetGroupCount(){ return 0; }
	virtual float GetGroupCPUUsedRate(UINT Index){ return 0; }
	virtual float GetGroupCycleTime(UINT Index){ return 0; }
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

	CLIENT_PROXY_CONFIG												m_Config;
	CCycleQueue<CDOSMessagePacket *>								m_MsgQueue;

	CStaticMap<MSG_ID_TYPE, MSG_MAP_INFO>							m_MessageMap;
	OBJECT_ID														m_UnhandleMsgReceiverID;
	CEasyCriticalSection											m_EasyCriticalSection;

	CThreadPerformanceCounter										m_ThreadPerformanceCounter;
	CGuardThread													m_GuardThread;


	CIDStorage<CDOSObjectProxyConnectionDefault, EDSM_NEW_ONCE>		m_ConnectionPool;
	UINT															m_FreeObjectCheckPtr;
	CThreadSafeIDStorage<CDOSObjectProxyConnectionDefault *>		m_DestoryConnectionList;
	CEasyArray<CDOSObjectProxyConnectionGroup>						m_ConnectionGroups;
	CEasyBuffer														m_CompressBuffer;
	char															m_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

	CHashMap<CIPAddress, IP_INFO>									m_IPBlackList;
	CHashMap<CIPAddress, IP_INFO>									m_RecvProtectedIPList;
	CEasyArray<IP_INFO>												m_PrepareIPBlackList;
	CEasyCriticalSection											m_BlackListCriticalSection;
	CEasyTimer														m_BlackListUpdateTimer;

public:
	CDOSObjectProxyServiceDefault(void);
	virtual ~CDOSObjectProxyServiceDefault(void);
	
	virtual void Release();
	virtual void Destory();
	virtual BYTE GetProxyType();
	virtual void SetID(UINT ID);
	virtual UINT GetID();
	virtual bool StartService();
	virtual void StopService();
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket);
	
	virtual UINT GetConnectionCount();
	virtual float GetCPUUsedRate();
	virtual float GetCycleTime();
	virtual UINT GetGroupCount();
	virtual float GetGroupCPUUsedRate(UINT Index);
	virtual float GetGroupCycleTime(UINT Index);



	bool Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config);
	


	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	virtual void OnClose();
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	virtual CBaseNetConnection * CreateConnection(CIPAddress& RemoteAddress);
	virtual bool DeleteConnection(CBaseNetConnection * pConnection);	
	CDOSObjectProxyConnectionDefault * GetConnection(UINT ID);

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
	void OnMsg(CDOSMessage * pMessage);
	void OnSystemMsg(CDOSMessage * pMessage);
	bool DoRegisterGlobalMsgMap(MSG_ID_TYPE MsgID, int MapType, OBJECT_ID ObjectID);
	bool DoUnregisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID);
	void ClearMsgMapByRouterID(UINT RouterID);
	bool CheckEncryptConfig();
	int CheckFreeObject();
};



inline CDOSObjectProxyConnectionDefault * CDOSObjectProxyServiceDefault::GetConnection(UINT ID)
{
	return m_ConnectionPool.GetObject(ID);
}