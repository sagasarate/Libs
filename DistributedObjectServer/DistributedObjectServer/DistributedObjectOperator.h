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
	bool							m_IsCommandReceiver;

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

	virtual UINT GetRouterID() override;
	virtual OBJECT_ID GetObjectID() override;
	virtual int GetGroupIndex() override;
	virtual BOOL SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual BOOL SendMessageMulti(OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) override;
	virtual BOOL BroadcastMessageToProxyObjectByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize) override;

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount) override;
	virtual BOOL ReleaseMessagePacket(CDOSMessagePacket * pPacket) override;
	virtual BOOL SendMessagePacket(CDOSMessagePacket * pPacket) override;

	virtual BOOL RegisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE * pMsgIDList, int CmdCount) override;
	virtual BOOL UnregisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE * pMsgIDList, int CmdCount) override;
	virtual BOOL RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType) override;
	virtual BOOL UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID) override;
	virtual BOOL SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType) override;

	virtual BOOL AddConcernedObject(OBJECT_ID ObjectID, bool NeedTest) override;
	virtual BOOL DeleteConcernedObject(OBJECT_ID ObjectID) override;

	virtual BOOL FindObject(UINT ObjectType) override;
	virtual BOOL ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize) override;
	virtual BOOL CloseProxyObject(OBJECT_ID ProxyObjectID, UINT Delay) override;
	virtual BOOL RequestProxyObjectIP(OBJECT_ID ProxyObjectID) override;

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo) override;
	virtual void Release() override;

	virtual BOOL QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param) override;
	virtual void ShutDown(UINT PluginID) override;
	virtual BOOL RegisterCommandReceiver() override;
	virtual BOOL UnregisterCommandReceiver() override;
	BOOL OnConsoleCommand(LPCTSTR szCommand);

	virtual BOOL RegisterLogger(UINT LogChannel, LPCTSTR FileName) override;
	virtual BOOL RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader) override;

	virtual void SetServerWorkStatus(BYTE WorkStatus) override;

	virtual UINT AddTimer(UINT TimeOut, UINT64 Param, bool IsRepeat) override;
	virtual BOOL DeleteTimer(UINT ID) override;

	virtual BOOL SetBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID) override;

protected:
	BOOL OnPreTranslateMessage(CDOSMessage * pMessage);
	virtual BOOL OnMessage(CDOSMessage * pMessage) override;
	virtual BOOL OnSystemMessage(CDOSMessage * pMessage) override;
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID) override;
	virtual void OnFindObject(OBJECT_ID CallerID) override;
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize) override;
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString) override;
	virtual void OnShutDown(BYTE Level, UINT Param) override;
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;
	virtual void OnTimer(UINT ID, UINT64 Param) override;
	virtual void OnTimerRelease(UINT ID, UINT64 Param) override;

	bool CallCSInitialize();
	void CallCSDestory();
	BOOL CallCSOnPreTranslateMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, const BYTE * pData, UINT DataSize);
	BOOL CallCSOnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, const BYTE * pData, UINT DataSize);
	BOOL CallCSOnSystemMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, const BYTE * pData, UINT DataSize);
	void CallCSOnConcernedObjectLost(OBJECT_ID ObjectID);
	BOOL CallCSOnFindObject(OBJECT_ID CallerID);
	void CallCSOnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize);
	void CallCSOnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString);
	void CallCSOnShutDown(BYTE Level, UINT Param);
	int CallCSUpdate(int ProcessPacketLimit);
	void CallCSOnException(MonoObject * pPostException);
	bool CallOnConsoleCommand(LPCTSTR szCommand);
	void CallCSOnTimer(UINT ID, UINT64 Param);
	void CallCSOnTimerRelease(UINT ID, UINT64 Param);

	static bool DoRegisterLogger(UINT LogChannel, LPCTSTR FileName);
	static bool DoRegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader);

public:
	static UINT InternalCallGetRouterID();
	static UINT64 InternalCallGetObjectID(CDistributedObjectOperator * pOperator);
	static int InternalCallGetGroupIndex(CDistributedObjectOperator * pOperator);
	static bool InternalCallSendMessage(CDistributedObjectOperator * pOperator, OBJECT_ID ReceiverID, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallSendMessageMulti(CDistributedObjectOperator * pOperator, MonoArray * ReceiverIDList, bool IsSorted, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallBroadcastMessageToProxyObjectByGroup(CDistributedObjectOperator * pOperator, WORD RouterID, BYTE ProxyType, UINT64 GroupID, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallRegisterMsgMap(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, MonoArray * MsgIDList);
	static bool InternalCallUnregisterMsgMap(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, MonoArray * MsgIDList);
	static bool InternalCallRegisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, UINT MsgID, int MapType);
	static bool InternalCallUnregisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, UINT MsgID);
	static bool InternalCallSetUnhanleMsgReceiver(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType);
	static bool InternalCallAddConcernedObject(CDistributedObjectOperator * pOperator, OBJECT_ID ObjectID, bool NeedTest);
	static bool InternalCallDeleteConcernedObject(CDistributedObjectOperator * pOperator, OBJECT_ID ObjectID);
	static bool InternalCallFindObject(CDistributedObjectOperator * pOperator, UINT ObjectType);
	static bool InternalCallReportObject(CDistributedObjectOperator * pOperator, OBJECT_ID TargetID, MonoArray * Data, int StartIndex, int DataLen);
	static bool InternalCallCloseProxyObject(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, UINT Delay);
	static bool InternalCallRequestProxyObjectIP(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID);
	static bool InternalCallRegisterObjectStatic(UINT PluginID, MonoObject * ObjectRegisterInfo);
	static bool InternalCallRegisterObject(CDistributedObjectOperator * pOperator, MonoObject * ObjectRegisterInfo);
	static void InternalCallRelease(CDistributedObjectOperator * pOperator);
	static bool InternalCallQueryShutDown(CDistributedObjectOperator * pOperator, OBJECT_ID TargetID, BYTE Level, UINT Param);
	static void InternalCallShutDown(CDistributedObjectOperator * pOperator, UINT PluginID);
	static bool InternalCallRegisterLogger(UINT LogChannel, MonoString * FileName);
	static bool InternalCallRegisterCSVLogger(UINT LogChannel, MonoString * FileName, MonoString * CSVLogHeader);
	static bool InternalCallRegisterCommandReceiver(CDistributedObjectOperator * pOperator);
	static bool InternalCallUnregisterCommandReceiver(CDistributedObjectOperator * pOperator);
	static void InternalCallSetServerWorkStatus(CDistributedObjectOperator * pOperator, BYTE WorkStatus);
	static UINT InternalCallAddTimer(CDistributedObjectOperator * pOperator, UINT TimeOut, MonoObject * pParam, bool IsRepeat);
	static bool InternalCallDeleteTimer(CDistributedObjectOperator * pOperator, UINT ID);
	static bool InternalCallSetBroadcastGroup(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, UINT64 GroupID);
};

inline UINT CDistributedObjectOperator::GetPoolID()
{
	return m_PoolID;
}