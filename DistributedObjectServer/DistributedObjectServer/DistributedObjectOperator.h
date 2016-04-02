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

	PLUGIN_TYPE						m_PluginType;
	MONO_DOMAIN_INFO				m_MonoDomainInfo;
	UINT							m_hCSOperatorObject;
	UINT							m_hCSObject;
	
	MONO_CLASS_INFO_DO				m_MonoClassInfo_DO;
public:
	CDistributedObjectOperator();
	~CDistributedObjectOperator();

	BOOL Init(CDistributedObjectManager * pManager,UINT PoolID,IDistributedObject * pDistributedObject);
	BOOL Init(CDistributedObjectManager * pManager, UINT PoolID, MONO_DOMAIN_INFO& MonoDomainInfo, MonoObject * pDistributedObject);
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
	BOOL OnPreTranslateMessage(CDOSMessage * pMessage);
	virtual BOOL OnMessage(CDOSMessage * pMessage);
	virtual BOOL OnSystemMessage(CDOSMessage * pMessage);
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnFindObject(OBJECT_ID CallerID);
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize);
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID,UINT IP,UINT Port,LPCSTR szIPString);
	virtual void OnShutDown(int Level);
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	bool CallCSInitialize();
	void CallCSDestory();
	BOOL CallCSOnPreTranslateMessage(CDOSMessage * pMessage);
	BOOL CallCSOnMessage(CDOSMessage * pMessage);
	BOOL CallCSOnSystemMessage(CDOSMessage * pMessage);
	void CallCSOnConcernedObjectLost(OBJECT_ID ObjectID);
	BOOL CallCSOnFindObject(OBJECT_ID CallerID);
	void CallCSOnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize);
	void CallCSOnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT IP, UINT Port, LPCSTR szIPString);
	void CallCSOnShutDown(int Level);
	int CallCSUpdate(int ProcessPacketLimit);

	static bool DoRegisterLogger(UINT LogChannel, LPCTSTR FileName);
	static bool DoRegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader);

public:
	static UINT InternalCallGetRouterID();
	static UINT64 InternalCallGetObjectID(CDistributedObjectOperator * pOperator);
	static int InternalCallGetGroupIndex(CDistributedObjectOperator * pOperator);
	static bool InternalCallSendMessage(CDistributedObjectOperator * pOperator, MonoObject * ReceiverID, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallSendMessageMulti(CDistributedObjectOperator * pOperator, MonoArray * ReceiverIDList, bool IsSorted, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallRegisterMsgMap(CDistributedObjectOperator * pOperator, MonoObject * ProxyObjectID, MonoArray * MsgIDList);
	static bool InternalCallUnregisterMsgMap(CDistributedObjectOperator * pOperator, MonoObject * ProxyObjectID, MonoArray * MsgIDList);
	static bool InternalCallRegisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, MonoArray * MsgIDList);
	static bool InternalCallUnregisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, MonoArray * MsgIDList);
	static bool InternalCallAddConcernedObject(CDistributedObjectOperator * pOperator, MonoObject * ObjectID, bool NeedTest);
	static bool InternalCallDeleteConcernedObject(CDistributedObjectOperator * pOperator, MonoObject * ObjectID);
	static bool InternalCallFindObject(CDistributedObjectOperator * pOperator, UINT ObjectType);
	static bool InternalCallReportObject(CDistributedObjectOperator * pOperator, MonoObject * TargetID, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallCloseProxyObject(CDistributedObjectOperator * pOperator, MonoObject * ProxyObjectID, UINT Delay);
	static bool InternalCallRequestProxyObjectIP(CDistributedObjectOperator * pOperator, MonoObject * ProxyObjectID);
	static bool InternalCallRegisterObjectStatic(UINT PluginID, MonoObject * ObjectRegisterInfo);
	static bool InternalCallRegisterObject(CDistributedObjectOperator * pOperator, MonoObject * ObjectRegisterInfo);
	static void InternalCallRelease(CDistributedObjectOperator * pOperator);
	static bool InternalCallQueryShutDown(CDistributedObjectOperator * pOperator, MonoObject * TargetID, int Level);
	static void InternalCallShutDown(CDistributedObjectOperator * pOperator, UINT PluginID);
	static bool InternalCallRegisterLogger(UINT LogChannel, MonoString * FileName);
	static bool InternalCallRegisterCSVLogger(UINT LogChannel, MonoString * FileName, MonoString * CSVLogHeader);
};

inline UINT CDistributedObjectOperator::GetPoolID()
{
	return m_PoolID;
}