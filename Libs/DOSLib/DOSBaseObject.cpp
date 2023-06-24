/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSBaseObject.cpp                                        */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

#define CONCERNED_OBJECT_TEST_SIMPLE		1000
#define CONCERNED_OBJECT_PAGE_SIZE			512
#define CONCERNED_OBJECT_ONCE_TEST_COUNT	512
#define CONCERNED_OBJECT_TEST_FACTOR		512

IMPLEMENT_CLASS_INFO_STATIC(CDOSBaseObject, CNameObject);



CDOSBaseObject::CDOSBaseObject(void)
{
	FUNCTION_BEGIN;
	m_pRouter=NULL;
	m_pManager=NULL;
	m_pGroup=NULL;
	m_MsgProcessLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT;
	m_ConcernedObjectCheckTime=1000;
	m_ConcernedObjectKeepAliveCount=5;
	m_ConcernedObjectCheckPtr=1;
	m_ConcernedObject.SetTag(_T("CDOSBaseObject"));
	m_MsgQueue.SetTag(_T("CDOSBaseObject"));
	FUNCTION_END;
}

CDOSBaseObject::~CDOSBaseObject(void)
{
	FUNCTION_BEGIN;
	Destory();
	FUNCTION_END;
}

bool CDOSBaseObject::Init(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;
	UINT MsgQueueSize=ObjectRegisterInfo.MsgQueueSize;
	if(MsgQueueSize==0)
		MsgQueueSize = GetManager()->GetServer()->GetConfig().ObjectConfig.MaxObjectMsgQueue;
	if(ObjectRegisterInfo.MsgProcessLimit)
		m_MsgProcessLimit=ObjectRegisterInfo.MsgProcessLimit;

	if (m_MsgQueue.GetBufferSize()<MsgQueueSize)
	{
		if(!m_MsgQueue.Create(MsgQueueSize))
		{
			PrintDOSLog(_T("对象[%llX]创建%u大小的消息队列失败"),
				GetObjectID().ID,MsgQueueSize);
			return false;
		}
	}
	else
	{
		m_MsgQueue.Clear();
	}
	if(m_ConcernedObject.GetBufferSize()<CONCERNED_OBJECT_PAGE_SIZE)
	{
		if(!m_ConcernedObject.Create(CONCERNED_OBJECT_PAGE_SIZE,CONCERNED_OBJECT_PAGE_SIZE))
		{
			PrintDOSLog(_T("对象[%llX]创建关注对象池失败"),
				GetObjectID().ID);
			return false;
		}
	}
	else
	{
		m_ConcernedObject.Clear();
	}

	m_ConcernedObjectTestTime = GetManager()->GetServer()->GetConfig().ObjectConfig.ObjectAliveTestTime;
	m_ConcernedObjectCheckTime = GetManager()->GetServer()->GetConfig().ObjectConfig.ObjectAliveCheckTime;
	m_ConcernedObjectKeepAliveCount = GetManager()->GetServer()->GetConfig().ObjectConfig.ObjectKeepAliveCount;
	m_ConcernedObjectCheckTimer.SaveTime();
	m_ConcernedObjectCheckPtr=1;
	return Initialize();
	FUNCTION_END;
	return false;
}

bool CDOSBaseObject::Initialize()
{
	FUNCTION_BEGIN;
	return TRUE;
	FUNCTION_END;
	return false;
}

void CDOSBaseObject::Destory()
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pPacket;
	while(m_MsgQueue.PopFront(&pPacket))
	{

		if(!ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}

	}
	//m_ConcernedObject.Clear();
	//m_MsgQueue.Destory();
	//m_ConcernedObject.Destory();
	m_pRouter=NULL;
	m_pManager=NULL;
	m_pGroup=NULL;
	FUNCTION_END;
}


UINT CDOSBaseObject::GetRouterID()
{
	FUNCTION_BEGIN;
	return m_pRouter->GetRouterID();
	FUNCTION_END;
	return 0;
}

int CDOSBaseObject::GetGroupIndex()
{
	if(m_pGroup)
		return (int)m_pGroup->GetIndex();
	return -1;
}

//int CDOSBaseObject::DoCycle(int ProcessPacketLimit)
//{
//	FUNCTION_BEGIN;
//	CDOSMessagePacket * pPacket;
//	int ProcessCount=0;
//	int Limit=m_MsgProcessLimit;
//	while(m_MsgQueue.PopFront(&pPacket))
//	{
//		if(pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
//		{
//			OnSystemMessage(&(pPacket->GetMessage()));
//		}
//		else
//		{
//			OnMessage(&(pPacket->GetMessage()));
//		}
//		if(!ReleaseMessagePacket(pPacket))
//		{
//			PrintDOSLog(_T("释放消息内存块失败！"));
//		}
//		Limit--;
//		ProcessCount++;
//		if(Limit<=0)
//			break;
//	}
//	ProcessCount+=DoConcernedObjectTest();
//	ProcessCount+=Update(ProcessPacketLimit);
//	return ProcessCount;
//	FUNCTION_END;
//	return 0;
//}

BOOL CDOSBaseObject::PushMessage(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	((CDOSServer *)m_pRouter->GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
	pPacket->SetAllocTime(0x12);
#endif
	if(m_MsgQueue.PushBack(&pPacket))
	{
		return TRUE;
	}
	else
	{
		ReleaseMessagePacket(pPacket);
		PrintDOSLog(_T("消息压栈失败！"));
	}
	FUNCTION_END;
	return FALSE;
}



BOOL CDOSBaseObject::SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	if(m_pRouter)
		return m_pRouter->RouterMessage(GetObjectID(),ReceiverID,MsgID,MsgFlag,pData,DataSize);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	if(pReceiverIDList==NULL||ReceiverCount==0)
	{
		return FALSE;
	}
	int PacketSize=CDOSMessagePacket::CaculatePacketLength(DataSize,ReceiverCount);

	CDOSMessagePacket * pNewPacket=m_pRouter->GetServer()->NewMessagePacket(PacketSize);
	if(pNewPacket==NULL)
	{
		PrintDOSLog(_T("分配消息内存块失败！"));
		return FALSE;
	}
	pNewPacket->GetMessage().SetMsgID(MsgID);
	pNewPacket->GetMessage().SetSenderID(GetObjectID());
	pNewPacket->GetMessage().SetDataLength(DataSize);
	pNewPacket->GetMessage().SetMsgFlag(MsgFlag);
	if (pData)
		pNewPacket->GetMessage().PutMsgData(0, pData, DataSize);
	pNewPacket->SetTargetIDs(ReceiverCount,pReceiverIDList);
	if(!IsSorted)
	{
		pNewPacket->SortTargetIDs();
	}

	pNewPacket->MakePacketLength();

	BOOL Ret=m_pRouter->RouterMessage(pNewPacket);
	ReleaseMessagePacket(pNewPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (Mask == 0)
		return FALSE;
	int PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize + sizeof(MASK_BROADCAST_INFO), 1);

	CDOSMessagePacket * pNewPacket = m_pRouter->GetServer()->NewMessagePacket(PacketSize);
	if (pNewPacket == NULL)
	{
		PrintDOSLog(_T("分配消息内存块失败！"));
		return FALSE;
	}
	pNewPacket->GetMessage().SetMsgID(DSM_PROXY_BROADCAST_BY_MASK);
	pNewPacket->GetMessage().SetSenderID(GetObjectID());
	pNewPacket->GetMessage().SetDataLength(sizeof(MASK_BROADCAST_INFO) + DataSize);
	pNewPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	MASK_BROADCAST_INFO * pInfo = (MASK_BROADCAST_INFO *)pNewPacket->GetMessage().GetMsgData();
	pInfo->Mask = Mask;
	pInfo->MsgID = MsgID;
	pInfo->MsgFlag = MsgFlag;
	if (pData)
		pNewPacket->GetMessage().PutMsgData(sizeof(MASK_BROADCAST_INFO), pData, DataSize);
	OBJECT_ID TargetID;
	TargetID.RouterID = RouterID;
	TargetID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType);
	TargetID.ObjectTypeID = DOT_PROXY_OBJECT;
	TargetID.ObjectIndex = 0;
	pNewPacket->SetTargetIDCount(0);
	pNewPacket->AddTargetID(TargetID);
	pNewPacket->MakePacketLength();

	BOOL Ret = m_pRouter->RouterMessage(pNewPacket);
	ReleaseMessagePacket(pNewPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (GroupID == 0)
		return FALSE;
	int PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize + sizeof(GROUP_BROADCAST_INFO), 1);

	CDOSMessagePacket* pNewPacket = m_pRouter->GetServer()->NewMessagePacket(PacketSize);
	if (pNewPacket == NULL)
	{
		PrintDOSLog(_T("分配消息内存块失败！"));
		return FALSE;
	}
	pNewPacket->GetMessage().SetMsgID(DSM_PROXY_BROADCAST_BY_GROUP);
	pNewPacket->GetMessage().SetSenderID(GetObjectID());
	pNewPacket->GetMessage().SetDataLength(sizeof(GROUP_BROADCAST_INFO) + DataSize);
	pNewPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	GROUP_BROADCAST_INFO* pInfo = (GROUP_BROADCAST_INFO*)pNewPacket->GetMessage().GetMsgData();
	pInfo->GroupID = GroupID;
	pInfo->MsgID = MsgID;
	pInfo->MsgFlag = MsgFlag;
	if (pData)
		pNewPacket->GetMessage().PutMsgData(sizeof(GROUP_BROADCAST_INFO), pData, DataSize);
	OBJECT_ID TargetID;
	TargetID.RouterID = RouterID;
	TargetID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType);
	TargetID.ObjectTypeID = DOT_PROXY_OBJECT;
	TargetID.ObjectIndex = 0;
	pNewPacket->SetTargetIDCount(0);
	pNewPacket->AddTargetID(TargetID);
	pNewPacket->MakePacketLength();

	BOOL Ret = m_pRouter->RouterMessage(pNewPacket);
	ReleaseMessagePacket(pNewPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}

CDOSMessagePacket * CDOSBaseObject::NewMessagePacket(UINT DataSize,UINT ReceiverCount)
{
	FUNCTION_BEGIN;
	//多分配16字节，方便后续加密填充
	MSG_LEN_TYPE PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize + 16, ReceiverCount);
	return m_pRouter->GetServer()->NewMessagePacket(PacketSize);
	FUNCTION_END;
	return NULL;
}

BOOL CDOSBaseObject::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	return m_pRouter->GetServer()->ReleaseMessagePacket(pPacket);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	return m_pRouter->RouterMessage(pPacket);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	FUNCTION_BEGIN;
	return SendMessage(ProxyObjectID,DSM_PROXY_REGISTER_MSG_MAP,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,pMsgIDList,sizeof(MSG_ID_TYPE)*CmdCount);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	FUNCTION_BEGIN;
	return SendMessage(ProxyObjectID,DSM_PROXY_UNREGISTER_MSG_MAP,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,pMsgIDList,sizeof(MSG_ID_TYPE)*CmdCount);
	FUNCTION_END;
	return FALSE;
}


BOOL CDOSBaseObject::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID=ProxyRouterID;
	ProxyID.ObjectTypeID=DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType);
	ProxyID.ObjectIndex = 0;
	UINT Data[2];
	Data[0] = MsgID;
	Data[1] = MapType;
	return SendMessage(ProxyID, DSM_PROXY_REGISTER_GLOBAL_MSG_MAP, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, Data, sizeof(Data));
	FUNCTION_END;
	return FALSE;
}


BOOL CDOSBaseObject::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID=ProxyRouterID;
	ProxyID.ObjectTypeID=DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType);
	ProxyID.ObjectIndex = 0;
	return SendMessage(ProxyID, DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &MsgID, sizeof(MSG_ID_TYPE));
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID = ProxyRouterID;
	ProxyID.ObjectTypeID = DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType);
	ProxyID.ObjectIndex = 0;
	return SendMessage(ProxyID, DSM_PROXY_SET_UNHANDLE_MSG_RECEIVER, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, NULL, 0);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest)
{
	FUNCTION_BEGIN;
	if(FindConcernedObjectInfo(ObjectID))
		return FALSE;

	CONCERNED_OBJECT_INFO Info;
	Info.ObjectID=ObjectID;
	Info.AliveFailedCount=0;
	Info.NeedTest=NeedTest;
	Info.RecentTestTime=CEasyTimer::GetTime();
	m_ConcernedObject.Insert(ObjectID,Info);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::DeleteConcernedObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	return m_ConcernedObject.Delete(ObjectID);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::FindObject(UINT ObjectType, bool OnlyLocal)
{
	FUNCTION_BEGIN;
	OBJECT_ID ObjectID;
	if (OnlyLocal)
		ObjectID.RouterID = 0;
	else
		ObjectID.RouterID = BROAD_CAST_ROUTER_ID;
	ObjectID.ObjectTypeID = ObjectType;
	ObjectID.GroupIndex = BROAD_CAST_GROUP_INDEX;
	ObjectID.ObjectIndex = BROAD_CAST_OBJECT_INDEX;
	return SendMessage(ObjectID, DSM_OBJECT_FIND, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSBaseObject::ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize)
{
	FUNCTION_BEGIN;
	return SendMessage(TargetID, DSM_OBJECT_REPORT, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, pObjectInfoData, DataSize);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay)
{
	FUNCTION_BEGIN;
	return SendMessage(ProxyObjectID,DSM_PROXY_DISCONNECT,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&Delay,sizeof(Delay));
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::RequestProxyObjectIP(OBJECT_ID ProxyObjectID)
{
	FUNCTION_BEGIN;
	return SendMessage(ProxyObjectID,DSM_PROXY_GET_IP,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	FUNCTION_END;
	return FALSE;
}

UINT CDOSBaseObject::AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat)
{
	FUNCTION_BEGIN;
	return m_pGroup->AddTimer(m_ObjectID, TimeOut, Param, IsRepeat);
	FUNCTION_END;
	return 0;
}
BOOL CDOSBaseObject::DeleteTimer(UINT ID)
{
	FUNCTION_BEGIN;
	return m_pGroup->DeleteTimer(ID);
	FUNCTION_END;
	return false;
}

BOOL CDOSBaseObject::QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param)
{
	FUNCTION_BEGIN;
	SHUTDOWN_INFO Info;
	Info.Level = Level;
	Info.Param = Param;
	return SendMessage(TargetID, DSM_SYSTEM_SHUTDOWN, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask)
{
	FUNCTION_BEGIN;
	BROADCAST_MASK_SET_INFO Info;
	Info.ProxyObjectID = ProxyObjectID;
	Info.Mask = Mask;
	ProxyObjectID.ObjectIndex = 0;
	return SendMessage(ProxyObjectID, DSM_PROXY_SET_BROADCAST_MASK, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSBaseObject::AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask)
{
	FUNCTION_BEGIN;
	BROADCAST_MASK_SET_INFO Info;
	Info.ProxyObjectID = ProxyObjectID;
	Info.Mask = Mask;
	ProxyObjectID.ObjectIndex = 0;
	return SendMessage(ProxyObjectID, DSM_PROXY_ADD_BROADCAST_MASK, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSBaseObject::RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask)
{
	FUNCTION_BEGIN;
	BROADCAST_MASK_SET_INFO Info;
	Info.ProxyObjectID = ProxyObjectID;
	Info.Mask = Mask;
	ProxyObjectID.ObjectIndex = 0;
	return SendMessage(ProxyObjectID, DSM_PROXY_REMOVE_BROADCAST_MASK, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSBaseObject::AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID)
{
	FUNCTION_BEGIN;
	BROADCAST_GROUP_SET_INFO Info;
	Info.ProxyObjectID = ProxyObjectID;
	Info.GroupID = GroupID;
	ProxyObjectID.ObjectIndex = 0;
	return SendMessage(ProxyObjectID, DSM_PROXY_ADD_BROADCAST_GROUP, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSBaseObject::RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID)
{
	FUNCTION_BEGIN;
	BROADCAST_GROUP_SET_INFO Info;
	Info.ProxyObjectID = ProxyObjectID;
	Info.GroupID = GroupID;
	ProxyObjectID.ObjectIndex = 0;
	return SendMessage(ProxyObjectID, DSM_PROXY_REMOVE_BROADCAST_GROUP, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &Info, sizeof(Info));
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::OnMessage(CDOSMessage * pMessage)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::OnSystemMessage(CDOSMessage * pMessage)
{
	FUNCTION_BEGIN;
	switch(pMessage->GetMsgID())
	{
	case DSM_PROXY_REGISTER_MSG_MAP_RESULT:
	case DSM_PROXY_UNREGISTER_MSG_MAP_RESULT:
	case DSM_PROXY_REGISTER_GLOBAL_MSG_MAP_RESULT:
	case DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP_RESULT:
		return FALSE;
	case DSM_PROXY_DISCONNECT:
		OnProxyObjectDisconnect(pMessage->GetSenderID());
		return TRUE;
	case DSM_PROXY_IP_REPORT:
		if (pMessage->GetDataLength() >= sizeof(WORD) + 1)
		{
			WORD Port = *((const WORD *)pMessage->GetMsgData());
			char * pIPStr = (char *)pMessage->GetMsgData() + sizeof(WORD);
			UINT StrLen = pMessage->GetDataLength() - sizeof(WORD) - 1;
			pIPStr[StrLen] = 0;			
			OnProxyObjectIPReport(pMessage->GetSenderID(), Port, pIPStr);
		}
		return TRUE;
	case DSM_ROUTE_LINK_LOST:
		OnRouteLinkLost(pMessage->GetSenderID().RouterID);
		return TRUE;
	case DSM_OBJECT_ALIVE_TEST:
		if(pMessage->GetDataLength()>=sizeof(BYTE))
		{
			OnAliveTest(pMessage->GetSenderID(),*((const BYTE *)pMessage->GetMsgData()));
		}
		return TRUE;
	case DSM_OBJECT_FIND:
		OnFindObject(pMessage->GetSenderID());
		return TRUE;
	case DSM_OBJECT_REPORT:
		OnObjectReport(pMessage->GetSenderID(), pMessage->GetMsgData(), pMessage->GetDataLength());
		return TRUE;
	case DSM_SYSTEM_SHUTDOWN:
		if (pMessage->GetDataLength() >= sizeof(SHUTDOWN_INFO))
		{
			const SHUTDOWN_INFO * pInfo = ((const SHUTDOWN_INFO *)pMessage->GetMsgData());
			OnShutDown(pInfo->Level, pInfo->Param);
		}
		return TRUE;
	}
	FUNCTION_END;
	return FALSE;
}

void CDOSBaseObject::OnConcernedObjectLost(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CDOSBaseObject::OnFindObject(OBJECT_ID CallerID)
{
	FUNCTION_BEGIN;	
	ReportObject(CallerID, NULL, 0);
	FUNCTION_END;
}

void CDOSBaseObject::OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CDOSBaseObject::OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}


int CDOSBaseObject::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
	return 0;
}

void CDOSBaseObject::OnProxyObjectDisconnect(OBJECT_ID ProxyObjectID)
{
	FUNCTION_BEGIN;
	if(DeleteConcernedObject(ProxyObjectID))
	{
		//PrintDOSDebugLog(_T("[0x%llX]被关注的代理对象[0x%llX]已断线"),
		//		GetObjectID(),ProxyObjectID.ID);
		OnConcernedObjectLost(ProxyObjectID);
	}
	FUNCTION_END;
}
void CDOSBaseObject::OnAliveTest(OBJECT_ID SenderID,BYTE IsEcho)
{
	FUNCTION_BEGIN;
	if(IsEcho)
	{
		CONCERNED_OBJECT_INFO * pInfo=FindConcernedObjectInfo(SenderID);
		if(pInfo)
		{
			pInfo->AliveFailedCount=0;
		}
	}
	else
	{
		IsEcho=1;
		SendMessage(SenderID,DSM_OBJECT_ALIVE_TEST,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&IsEcho,sizeof(IsEcho));
	}
	FUNCTION_END;
}
void CDOSBaseObject::OnRouteLinkLost(UINT RouteID)
{
	FUNCTION_BEGIN;
	void * Pos=m_ConcernedObject.GetFirstObjectPos();
	while(Pos)
	{
		OBJECT_ID Key;
		CONCERNED_OBJECT_INFO * pInfo=m_ConcernedObject.GetNextObject(Pos,Key);
		if(pInfo)
		{
			if(pInfo->ObjectID.RouterID==RouteID)
			{
				PrintDOSLog(_T("被关注的对象[0x%llX]对应的路由连接已断开"),pInfo->ObjectID.ID);
				DeleteConcernedObject(Key);
				OnConcernedObjectLost(Key);
			}
		}
	}
	FUNCTION_END;
}

void CDOSBaseObject::OnShutDown(BYTE Level, UINT Param)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

void CDOSBaseObject::OnTimer(UINT ID, UINT64 Param, bool IsRepeat)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}
void CDOSBaseObject::OnTimerRelease(UINT ID, UINT64 Param)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

int CDOSBaseObject::ProcessMessage(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pPacket;
	int ProcessCount = 0;
	int Limit = m_MsgProcessLimit;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		{
			OnSystemMessage(&(pPacket->GetMessage()));
		}
		else
		{
			OnMessage(&(pPacket->GetMessage()));
		}
		if (!ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
		Limit--;
		ProcessCount++;
		if (Limit <= 0)
			break;
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}

int CDOSBaseObject::DoConcernedObjectTest()
{
	FUNCTION_BEGIN;
	int ProcessCount=0;

	if(m_ConcernedObjectCheckTimer.IsTimeOut(m_ConcernedObjectCheckTime))
	{
		m_ConcernedObjectCheckTimer.SaveTime();
		UINT CurTime=CEasyTimer::GetTime();

		if (m_ConcernedObject.GetObjectCount() < CONCERNED_OBJECT_TEST_SIMPLE)
		{
			for (CONCERNED_OBJECT_INFO& Info : m_ConcernedObject)
			{
				if (Info.NeedTest)
				{
					DoConcernedObjectTest(Info, CurTime);
					ProcessCount++;
				}					
			}
		}
		else
		{
			int Factor = CONCERNED_OBJECT_ONCE_TEST_COUNT * CONCERNED_OBJECT_TEST_FACTOR;
			while (Factor > 0)
			{
				CONCERNED_OBJECT_INFO * pInfo = m_ConcernedObject.GetObject(m_ConcernedObjectCheckPtr);
				if (pInfo)
				{
					if (pInfo->NeedTest)
					{
						DoConcernedObjectTest(*pInfo, CurTime);
						ProcessCount++;
						m_ConcernedObjectCheckPtr++;
						Factor -= CONCERNED_OBJECT_TEST_FACTOR;
					}
					else
					{
						m_ConcernedObjectCheckPtr++;
						Factor--;
					}
				}
				else
				{
					m_ConcernedObjectCheckPtr++;
					Factor--;
				}
				if (m_ConcernedObjectCheckPtr >= m_ConcernedObject.GetBufferSize())
				{
					m_ConcernedObjectCheckPtr = 1;
					break;
				}
			}
		}
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}

void CDOSBaseObject::DoConcernedObjectTest(CONCERNED_OBJECT_INFO& Info, UINT CurTime)
{
	if (CEasyTimer::GetTimeToTime(Info.RecentTestTime, CurTime) >= m_ConcernedObjectTestTime)
	{
		Info.RecentTestTime = CurTime;

		if (Info.AliveFailedCount > m_ConcernedObjectKeepAliveCount)
		{
			PrintDOSLog(_T("被关注的对象[0x%llX]活动测试失败"),
				Info.ObjectID.ID);
			OBJECT_ID ObjectID = Info.ObjectID;
			DeleteConcernedObject(ObjectID);
			OnConcernedObjectLost(ObjectID);
		}
		else
		{
			Info.AliveFailedCount++;
			BYTE IsEcho = 0;
			SendMessage(Info.ObjectID, DSM_OBJECT_ALIVE_TEST, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &IsEcho, sizeof(IsEcho));
		}
	}
}

CDOSBaseObject::CONCERNED_OBJECT_INFO * CDOSBaseObject::FindConcernedObjectInfo(OBJECT_ID ObjectID)
{
	return m_ConcernedObject.Find(ObjectID);
}
