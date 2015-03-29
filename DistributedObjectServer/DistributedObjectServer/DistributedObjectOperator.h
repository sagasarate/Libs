#pragma once

class CDistributedObjectManager;

class CDistributedObjectOperator :
	public IDistributedObjectOperator,
	public CDOSBaseObject
{
protected:
	CDistributedObjectManager		*m_pManager;
	IDistributedObject				*m_pDistributedObject;
	UINT							m_PoolID;
public:
	CDistributedObjectOperator();
	~CDistributedObjectOperator();

	BOOL Init(CDistributedObjectManager * pManager,UINT PoolID,IDistributedObject * pDistributedObject);
	virtual bool Initialize();
	virtual void Destory();



	UINT GetPoolID();

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

	virtual BOOL AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest);
	virtual BOOL DeleteConcernedObject(OBJECT_ID ObjectID);

	virtual BOOL FindObject(UINT ObjectType);
	virtual BOOL ReportObject(OBJECT_ID TargetID,const CSmartStruct& ObjectInfo);
	virtual BOOL CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay);
	virtual BOOL RequestProxyObjectIP(OBJECT_ID ProxyObjectID);

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo);
	virtual void Release();

	virtual BOOL QueryShutDown(OBJECT_ID TargetID,int Level);
	virtual void ShutDown(UINT PluginID);

protected:
	virtual BOOL OnMessage(CDOSMessage * pMessage);
	virtual BOOL OnSystemMessage(CDOSMessage * pMessage);
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnFindObject(OBJECT_ID CallerID);
	virtual void OnObjectReport(OBJECT_ID ObjectID,const CSmartStruct& ObjectInfo);
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID,UINT IP,UINT Port,LPCSTR szIPString);
	virtual void OnShutDown(int Level);
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
};

inline UINT CDistributedObjectOperator::GetPoolID()
{
	return m_PoolID;
}