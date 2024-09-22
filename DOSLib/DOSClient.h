/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSClient.h                                              */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CDOSClient :	
	public CNetConnection,
	public IDistributedObjectOperator
{
protected:
	CEasyBuffer									m_AssembleBuffer;	
	CEasyBuffer									m_SendBuffer;
	UINT										m_KeepAliveCount;
	UINT										m_MaxKeepAliveCount;
	UINT										m_KeepAliveTime;
	CEasyTimer									m_KeepAliveTimer;
	UINT										m_RecentPingDelay;

	CDOSSimpleMessage *							m_pCurHandleMsg;
	CHashMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>	m_MsgFnMap;

	int											m_MsgCompressType;
	int											m_MsgEnCryptType;
	CEasyStringA								m_SecretKey;
	CEasyBuffer									m_EncyptBuffer;
	UINT										m_TotalMsgSendCount;

	bool										m_DumpMsg;

	DECLARE_CLASS_INFO_STATIC(CDOSClient);
public:
	CDOSClient(void);
	virtual ~CDOSClient(void);	
	virtual void Destory();
	

	bool Start(UINT SendQueueLen, UINT RecvQueueLen, const CIPAddress& Address, DWORD TimeOut = NO_CONNECTION_TIME_OUT);
	void Close();

	void SetMsgCompressType(int Type);
	void SetMsgEncyptType(int Type, LPCTSTR SecretKey);
	void EnableMsgDump(bool Enable);

	virtual UINT GetRouterID() override;
	virtual OBJECT_ID GetObjectID() override;
	virtual int GetGroupIndex() override;
	virtual bool SendMessage(OBJECT_ID ReceiverID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual bool SendMessageMulti(OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual bool BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize) override;
	virtual bool BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize) override;

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount) override;
	virtual bool ReleaseMessagePacket(CDOSMessagePacket * pPacket) override;
	virtual bool SendMessagePacket(CDOSMessagePacket * pPacket) override;

	virtual bool RegisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE * pMsgIDList, int CmdCount) override;
	virtual bool UnregisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE * pMsgIDList, int CmdCount) override;
	virtual bool RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType) override;
	virtual bool UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID) override;
	virtual bool SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType) override;

	virtual bool AddConcernedObject(OBJECT_ID ObjectID, bool NeedTest) override;
	virtual bool DeleteConcernedObject(OBJECT_ID ObjectID) override;

	virtual bool FindObject(UINT ObjectType, bool OnlyLocal) override;
	virtual bool ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize) override;
	virtual bool CloseProxyObject(OBJECT_ID ProxyObjectID, UINT Delay) override;
	virtual bool RequestProxyObjectIP(OBJECT_ID ProxyObjectID) override;

	virtual bool RegisterObject(DOS_OBJ_REGISTER_INFO& ObjectRegisterInfo) override;
	virtual void Release() override;

	virtual bool QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param) override;
	virtual void ShutDown(UINT PluginID) override;
	virtual bool RegisterCommandReceiver() override;
	virtual bool UnregisterCommandReceiver() override;
	virtual bool RegisterLogger(UINT LogChannel, LPCTSTR FileName) override;
	virtual bool RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader) override;

	virtual void SetServerWorkStatus(BYTE WorkStatus) override;
	virtual UINT AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat) override;
	virtual bool DeleteTimer(UINT ID) override;

	virtual bool SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) override;
	virtual bool AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) override;
	virtual bool RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) override;
	virtual bool AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID) override;
	virtual bool RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID) override;
protected:
	virtual void OnRecvData(const BYTE * pData, UINT DataSize) override;
	virtual bool OnDOSMessage(CDOSSimpleMessage * pMessage);
	virtual bool OnSystemMessage(CDOSSimpleMessage * pMessage);

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;

	void SendKeepAlivePing();
	const void * EncyptMsg(const void * pData, MSG_LEN_TYPE& DataLen, WORD& CRC, UINT MsgSequence);
	void * DecyptMsg(void * pData, MSG_LEN_TYPE& DataLen, WORD CRC, UINT MsgSequence);
	WORD MakeCRC(const void * pData, UINT DataLen, const void * pKey, UINT KeyLen, UINT MsgSequence);
};

inline void CDOSClient::SetMsgCompressType(int Type)
{
	m_MsgCompressType=Type;
}

inline void CDOSClient::SetMsgEncyptType(int Type, LPCTSTR SecretKey)
{
	m_MsgEnCryptType = Type;
	m_SecretKey = SecretKey;
}
inline void CDOSClient::EnableMsgDump(bool Enable)
{
	m_DumpMsg = Enable;
}