/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSBaseObject.h                                          */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CDOSRouter;
class CDOSObjectManager;
class CDOSObjectGroup;



class CDOSBaseObject :
	public CNameObject
{
protected:
	struct CONCERNED_OBJECT_INFO
	{
		OBJECT_ID		ObjectID;
		UINT			AliveFailedCount;
		bool			NeedTest;
		UINT			RecentTestTime;
	};

	OBJECT_ID											m_ObjectID;

	CDOSRouter*											m_pRouter;
	CDOSObjectManager*									m_pManager;
	CDOSObjectGroup*									m_pGroup;
	UINT												m_MsgProcessLimit;
	CStaticMap<OBJECT_ID, CONCERNED_OBJECT_INFO>		m_ConcernedObject;
	CEasyArray<OBJECT_ID>								m_ConcernorList;
	UINT												m_ConcernedObjectTestTime;
	UINT												m_ConcernedObjectKeepAliveCount;
	UINT												m_ConcernedObjectCheckPtr;
	UINT												m_ConcernedObjectCheckTime;
	CEasyTimer											m_ConcernedObjectCheckTimer;
	bool												m_SendDestoryNotify;
	bool												m_BroadcastDestoryNotify;

	CCycleQueue<CDOSMessagePacket*>						m_MsgQueue;

	friend class CDOSObjectGroup;

	DECLARE_CLASS_INFO_STATIC(CDOSBaseObject);
public:
	CDOSBaseObject(void);
	virtual ~CDOSBaseObject(void);

	bool Init(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo);

	virtual bool Initialize();
	virtual void Destory();

	void SetRouter(CDOSRouter* pRouter);
	void SetManager(CDOSObjectManager* pManager);
	void SetGroup(CDOSObjectGroup* pGroup);

	CDOSRouter* GetRouter();
	CDOSObjectManager* GetManager();
	CDOSObjectGroup* GetGroup();
	int GetGroupIndex();
	UINT GetRouterID();

	void SetObjectID(OBJECT_ID ID);
	OBJECT_ID GetObjectID();

	//int DoCycle(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	bool PushMessage(CDOSMessagePacket* pPacket);


	UINT GetMsgQueueLen();

	bool SendMessage(OBJECT_ID ReceiverID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	bool SendMessageMulti(OBJECT_ID* pReceiverIDList, UINT ReceiverCount, bool IsSorted, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	bool BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	bool BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);

	CDOSMessagePacket* NewMessagePacket(UINT DataSize, UINT ReceiverCount);
	bool ReleaseMessagePacket(CDOSMessagePacket* pPacket);
	bool SendMessagePacket(CDOSMessagePacket* pPacket);

	bool RegisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE* pMsgIDList, int CmdCount);
	bool UnregisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE* pMsgIDList, int CmdCount);
	bool RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType);
	bool UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID);
	bool SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType);

	bool AddConcernedObject(OBJECT_ID ObjectID, bool NeedTest);
	bool DeleteConcernedObject(OBJECT_ID ObjectID);

	bool FindObject(UINT ObjectType, bool OnlyLocal);
	bool ReportObject(OBJECT_ID TargetID, const void* pObjectInfoData, UINT DataSize);
	bool CloseProxyObject(OBJECT_ID ProxyObjectID, UINT Delay);
	bool RequestProxyObjectIP(OBJECT_ID ProxyObjectID);
	bool QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param);

	UINT AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat);
	bool DeleteTimer(UINT ID);


	bool SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	bool AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	bool RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	bool AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID);
	bool RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID);
protected:
	virtual bool OnMessage(CDOSMessage* pMessage);
	virtual bool OnSystemMessage(CDOSMessage* pMessage);
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnFindObject(OBJECT_ID CallerID);
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void* pObjectInfoData, UINT DataSize);
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString);
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual void OnObjectDestoryed(OBJECT_ID ObjectID);
	virtual void OnAliveTest(OBJECT_ID SenderID, BYTE IsEcho);
	virtual void OnRouteLinkLost(UINT RouteID);
	virtual void OnShutDown(BYTE Level, UINT Param);
	virtual void OnTimer(UINT ID, UINT64 Param, bool IsRepeat);
	virtual void OnTimerRelease(UINT ID, UINT64 Param);

	int ProcessMessage(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	int DoConcernedObjectTest();
	void DoConcernedObjectTest(CONCERNED_OBJECT_INFO& Info, UINT CurTime);

	CONCERNED_OBJECT_INFO* FindConcernedObjectInfo(OBJECT_ID ObjectID);

};



inline void CDOSBaseObject::SetRouter(CDOSRouter* pRouter)
{
	m_pRouter = pRouter;
}
inline void CDOSBaseObject::SetManager(CDOSObjectManager* pManager)
{
	m_pManager = pManager;
}
inline void CDOSBaseObject::SetGroup(CDOSObjectGroup* pGroup)
{
	m_pGroup = pGroup;
}

inline CDOSRouter* CDOSBaseObject::GetRouter()
{
	return m_pRouter;
}
inline CDOSObjectManager* CDOSBaseObject::GetManager()
{
	return m_pManager;
}
inline CDOSObjectGroup* CDOSBaseObject::GetGroup()
{
	return m_pGroup;
}


inline void CDOSBaseObject::SetObjectID(OBJECT_ID ID)
{
	m_ObjectID = ID;
}
inline OBJECT_ID CDOSBaseObject::GetObjectID()
{
	return m_ObjectID;
}

inline UINT CDOSBaseObject::GetMsgQueueLen()
{
	return m_MsgQueue.GetUsedSize();
}