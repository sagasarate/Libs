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

	CDOSRouter *										m_pRouter;
	CDOSObjectManager *									m_pManager;
	CDOSObjectGroup *									m_pGroup;
	UINT												m_MsgProcessLimit;
	CStaticMap<OBJECT_ID,CONCERNED_OBJECT_INFO>			m_ConcernedObject;	
	UINT												m_ConcernedObjectTestTime;
	UINT												m_ConcernedObjectKeepAliveCount;
	UINT												m_ConcernedObjectCheckPtr;
	UINT												m_ConcernedObjectCheckTime;
	CEasyTimer											m_ConcernedObjectCheckTimer;

	CCycleQueue<CDOSMessagePacket *>					m_MsgQueue;

	friend class CDOSObjectGroup;

	DECLARE_CLASS_INFO_STATIC(CDOSBaseObject);
public:
	CDOSBaseObject(void);
	virtual ~CDOSBaseObject(void);

	bool Init(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo);

	virtual bool Initialize();
	virtual void Destory();

	void SetRouter(CDOSRouter * pRouter);
	void SetManager(CDOSObjectManager * pManager);
	void SetGroup(CDOSObjectGroup * pGroup);

	CDOSRouter * GetRouter();
	CDOSObjectManager * GetManager();
	CDOSObjectGroup * GetGroup();
	int GetGroupIndex();
	UINT GetRouterID();

	void SetObjectID(OBJECT_ID ID);
	OBJECT_ID GetObjectID();

	//int DoCycle(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	BOOL PushMessage(CDOSMessagePacket * pPacket);
	

	UINT GetMsgQueueLen();

	BOOL SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0);
	BOOL SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0);
	BOOL BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);
	BOOL BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0);

	CDOSMessagePacket * NewMessagePacket(UINT DataSize,UINT ReceiverCount);
	BOOL ReleaseMessagePacket(CDOSMessagePacket * pPacket);
	BOOL SendMessagePacket(CDOSMessagePacket * pPacket);

	BOOL RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount);
	BOOL UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount);
	BOOL RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType);
	BOOL UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID);
	BOOL SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType);

	BOOL AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest);
	BOOL DeleteConcernedObject(OBJECT_ID ObjectID);

	BOOL FindObject(UINT ObjectType, bool OnlyLocal);
	BOOL ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize);
	BOOL CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay);
	BOOL RequestProxyObjectIP(OBJECT_ID ProxyObjectID);
	BOOL QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param);

	UINT AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat);
	BOOL DeleteTimer(UINT ID);
	

	BOOL SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	BOOL AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	BOOL RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask);
	BOOL AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID);
	BOOL RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID);
protected:
	virtual BOOL OnMessage(CDOSMessage * pMessage);
	virtual BOOL OnSystemMessage(CDOSMessage * pMessage);
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnFindObject(OBJECT_ID CallerID);
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize);
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString);
	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	virtual void OnProxyObjectDisconnect(OBJECT_ID ProxyObjectID);
	virtual void OnAliveTest(OBJECT_ID SenderID,BYTE IsEcho);
	virtual void OnRouteLinkLost(UINT RouteID);
	virtual void OnShutDown(BYTE Level, UINT Param);
	virtual void OnTimer(UINT ID, UINT64 Param, bool IsRepeat);
	virtual void OnTimerRelease(UINT ID, UINT64 Param);

	int ProcessMessage(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	int DoConcernedObjectTest();
	void DoConcernedObjectTest(CONCERNED_OBJECT_INFO& Info, UINT CurTime);

	CONCERNED_OBJECT_INFO * FindConcernedObjectInfo(OBJECT_ID ObjectID);
	
};



inline void CDOSBaseObject::SetRouter(CDOSRouter * pRouter)
{
	m_pRouter=pRouter;
}
inline void CDOSBaseObject::SetManager(CDOSObjectManager * pManager)
{
	m_pManager=pManager;
}
inline void CDOSBaseObject::SetGroup(CDOSObjectGroup * pGroup)
{
	m_pGroup=pGroup;
}

inline CDOSRouter * CDOSBaseObject::GetRouter()
{
	return m_pRouter;
}
inline CDOSObjectManager * CDOSBaseObject::GetManager()
{
	return m_pManager;
}
inline CDOSObjectGroup * CDOSBaseObject::GetGroup()
{
	return m_pGroup;
}


inline void CDOSBaseObject::SetObjectID(OBJECT_ID ID)
{
	m_ObjectID=ID;
}
inline OBJECT_ID CDOSBaseObject::GetObjectID()
{
	return m_ObjectID;
}

inline UINT CDOSBaseObject::GetMsgQueueLen()
{
	return m_MsgQueue.GetUsedSize();
}