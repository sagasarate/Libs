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
	CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>	m_MsgFnMap;

	int											m_MsgCompressType;

	DECLARE_CLASS_INFO_STATIC(CDOSClient);
public:
	CDOSClient(void);
	virtual ~CDOSClient(void);	
	virtual void Destory();
	

	BOOL Start(UINT SendBufferSize,UINT AssembleBufferSize,const CIPAddress& Address,DWORD TimeOut=NO_CONNECTION_TIME_OUT);
	void Close();

	void SetMsgCompressType(int Type);

	virtual UINT GetRouterID();
	virtual OBJECT_ID GetObjectID();
	virtual int GetGroupIndex();
	virtual BOOL SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0);
	virtual BOOL SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0);

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize,UINT ReceiverCount);
	virtual BOOL ReleaseMessagePacket(CDOSMessagePacket * pPacket);
	virtual BOOL SendMessagePacket(CDOSMessagePacket * pPacket);

	virtual BOOL RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount);
	virtual BOOL UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount);
	virtual BOOL RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount);
	virtual BOOL UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount);
	virtual BOOL SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType);

	virtual BOOL AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest);
	virtual BOOL DeleteConcernedObject(OBJECT_ID ObjectID);

	virtual BOOL FindObject(UINT ObjectType);
	virtual BOOL ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize);
	virtual BOOL CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay);
	virtual BOOL RequestProxyObjectIP(OBJECT_ID ProxyObjectID);

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo);
	virtual void Release();

	virtual BOOL QueryShutDown(OBJECT_ID TargetID,int Level);
	virtual void ShutDown(UINT PluginID);
	virtual BOOL RegisterLogger(UINT LogChannel, LPCTSTR FileName);
	virtual BOOL RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader);

protected:
	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	virtual BOOL OnDOSMessage(CDOSSimpleMessage * pMessage);
	virtual BOOL OnSystemMessage(CDOSSimpleMessage * pMessage);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void SendKeepAlivePing();

};

inline void CDOSClient::SetMsgCompressType(int Type)
{
	m_MsgCompressType=Type;
}