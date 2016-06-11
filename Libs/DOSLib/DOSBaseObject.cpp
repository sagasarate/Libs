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
		MsgQueueSize=GetManager()->GetServer()->GetConfig().MaxObjectMsgQueue;
	if(ObjectRegisterInfo.MsgProcessLimit)
		m_MsgProcessLimit=ObjectRegisterInfo.MsgProcessLimit;

	if(m_MsgQueue.GetBufferSize()<MsgQueueSize)
	{
		if(!m_MsgQueue.Create(MsgQueueSize))
		{
			PrintDOSLog(0,_T("对象[%llX]创建%u大小的消息队列失败"),
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
			PrintDOSLog(0,_T("对象[%llX]创建关注对象池失败"),
				GetObjectID().ID);
			return false;
		}
	}
	else
	{
		m_ConcernedObject.Clear();
	}

	m_ConcernedObjectTestTime=GetManager()->GetServer()->GetConfig().ObjectAliveTestTime;
	m_ConcernedObjectCheckTime=GetManager()->GetServer()->GetConfig().ObjectAliveCheckTime;
	m_ConcernedObjectKeepAliveCount=GetManager()->GetServer()->GetConfig().ObjectKeepAliveCount;
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
	while(m_MsgQueue.PopFront(pPacket))
	{

		if(!ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSBaseObject::Destory:释放消息内存块失败！"));
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

int CDOSBaseObject::DoCycle(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pPacket;
	int ProcessCount=0;
	int Limit=m_MsgProcessLimit;
	while(m_MsgQueue.PopFront(pPacket))
	{
		if(pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
		{
			OnSystemMessage(&(pPacket->GetMessage()));
		}
		else
		{
			OnMessage(&(pPacket->GetMessage()));
		}
		if(!ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("CDOSBaseObject::DoCycle:释放消息内存块失败！"));
		}
		Limit--;
		ProcessCount++;
		if(Limit<=0)
			break;
	}
	ProcessCount+=DoConcernedObjectTest();
	ProcessCount+=Update(ProcessPacketLimit);
	return ProcessCount;
	FUNCTION_END;
	return 0;
}

BOOL CDOSBaseObject::PushMessage(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	((CDOSServer *)m_pRouter->GetServer())->AddRefMessagePacket(pPacket);
	pPacket->SetAllocTime(2);
	if(m_MsgQueue.PushBack(pPacket))
	{
		return TRUE;
	}
	else
	{
		ReleaseMessagePacket(pPacket);
		PrintDOSLog(_T("DOSLib"),_T("CDOSBaseObject::PushMessage:消息压栈失败！"));
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
		PrintDOSLog(_T("DOSLib"),_T("分配消息内存块失败！"));
		return FALSE;
	}
	pNewPacket->GetMessage().SetMsgID(MsgID);
	pNewPacket->GetMessage().SetSenderID(GetObjectID());
	pNewPacket->GetMessage().SetDataLength(DataSize);
	pNewPacket->GetMessage().SetMsgFlag(MsgFlag);
	if(pData)
		memcpy(pNewPacket->GetMessage().GetDataBuffer(),pData,DataSize);
	pNewPacket->SetTargetIDs(ReceiverCount,pReceiverIDList);
	if(!IsSorted)
	{
		qsort(pNewPacket->GetTargetIDs(),ReceiverCount,sizeof(OBJECT_ID),OBJECT_ID::Compare);
	}

	pNewPacket->MakePacketLength();

	BOOL Ret=m_pRouter->RouterMessage(pNewPacket);
	ReleaseMessagePacket(pNewPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}

CDOSMessagePacket * CDOSBaseObject::NewMessagePacket(UINT DataSize,UINT ReceiverCount)
{
	FUNCTION_BEGIN;
	MSG_LEN_TYPE PacketSize=CDOSMessagePacket::CaculatePacketLength(DataSize,ReceiverCount);
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


BOOL CDOSBaseObject::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID=ProxyRouterID;
	ProxyID.ObjectTypeID=DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType, BROAD_CAST_PROXY_ID);
	ProxyID.ObjectIndex = 0;
	return SendMessage(ProxyID,DSM_PROXY_REGISTER_GLOBAL_MSG_MAP,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,pMsgIDList,sizeof(MSG_ID_TYPE)*CmdCount);
	FUNCTION_END;
	return FALSE;
}


BOOL CDOSBaseObject::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID=ProxyRouterID;
	ProxyID.ObjectTypeID=DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType, BROAD_CAST_PROXY_ID);
	ProxyID.ObjectIndex = 0;
	return SendMessage(ProxyID,DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,pMsgIDList,sizeof(MSG_ID_TYPE)*CmdCount);
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSBaseObject::SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType)
{
	FUNCTION_BEGIN;
	OBJECT_ID ProxyID;
	ProxyID.RouterID = ProxyRouterID;
	ProxyID.ObjectTypeID = DOT_PROXY_OBJECT;
	ProxyID.GroupIndex = MAKE_PROXY_GROUP_INDEX(ProxyType, BROAD_CAST_PROXY_ID);
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

BOOL CDOSBaseObject::FindObject(UINT ObjectType)
{
	FUNCTION_BEGIN;
	OBJECT_ID ObjectID;
	ObjectID.RouterID=BROAD_CAST_ROUTER_ID;
	ObjectID.ObjectTypeID=ObjectType;
	ObjectID.GroupIndex=BROAD_CAST_GROUP_INDEX;
	ObjectID.ObjectIndex=BROAD_CAST_OBJECT_INDEX;
	return SendMessage(ObjectID,DSM_OBJECT_FIND,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
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

BOOL CDOSBaseObject::QueryShutDown(OBJECT_ID TargetID,int Level)
{
	FUNCTION_BEGIN;
	return SendMessage(TargetID,DSM_SYSTEM_SHUTDOWN,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&Level,sizeof(int));
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
			WORD Port = *((WORD *)pMessage->GetDataBuffer());
			char * pIPStr = (char *)pMessage->GetDataBuffer() + sizeof(WORD);
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
			OnAliveTest(pMessage->GetSenderID(),*((BYTE *)pMessage->GetDataBuffer()));
		}
		return TRUE;
	case DSM_OBJECT_FIND:
		OnFindObject(pMessage->GetSenderID());
		return TRUE;
	case DSM_OBJECT_REPORT:
		OnObjectReport(pMessage->GetSenderID(), pMessage->GetDataBuffer(), pMessage->GetDataLength());
		return TRUE;
	case DSM_SYSTEM_SHUTDOWN:
		if(pMessage->GetDataLength()>=sizeof(int))
		{
			int * pLevel=((int *)pMessage->GetDataBuffer());
			OnShutDown(*pLevel);
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
		PrintDOSDebugLog(0,_T("[0x%llX]被关注的代理对象[0x%llX]已断线"),
				GetObjectID(),ProxyObjectID.ID);
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
				PrintDOSLog(0,_T("被关注的对象[0x%llX]对应的路由连接已断开"),pInfo->ObjectID.ID);
				DeleteConcernedObject(Key);
				OnConcernedObjectLost(Key);
			}
		}
	}
	FUNCTION_END;
}

void CDOSBaseObject::OnShutDown(int Level)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

int CDOSBaseObject::DoConcernedObjectTest()
{
	FUNCTION_BEGIN;
	int ProcessCount=0;

	if(m_ConcernedObjectCheckTimer.IsTimeOut(m_ConcernedObjectCheckTime))
	{
		m_ConcernedObjectCheckTimer.SaveTime();
		UINT CurTime=CEasyTimer::GetTime();

		int Factor=CONCERNED_OBJECT_ONCE_TEST_COUNT*CONCERNED_OBJECT_TEST_FACTOR;
		while(Factor>0)
		{
			CONCERNED_OBJECT_INFO * pInfo=m_ConcernedObject.GetObject(m_ConcernedObjectCheckPtr);
			if(pInfo)
			{
				if(pInfo->NeedTest)
				{
					if(GetTimeToTime(pInfo->RecentTestTime,CurTime)>=m_ConcernedObjectTestTime)
					{
						pInfo->RecentTestTime=CurTime;

						if(pInfo->AliveFailedCount>m_ConcernedObjectKeepAliveCount)
						{
							PrintDOSLog(0,_T("被关注的对象[0x%llX]活动测试失败"),
								pInfo->ObjectID.ID);
							OBJECT_ID ObjectID=pInfo->ObjectID;
							DeleteConcernedObject(ObjectID);
							OnConcernedObjectLost(ObjectID);
						}
						else
						{
							pInfo->AliveFailedCount++;
							BYTE IsEcho=0;
							SendMessage(pInfo->ObjectID,DSM_OBJECT_ALIVE_TEST,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,&IsEcho,sizeof(IsEcho));
						}
					}
					ProcessCount++;
					m_ConcernedObjectCheckPtr++;
					Factor-=CONCERNED_OBJECT_TEST_FACTOR;
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
			if(m_ConcernedObjectCheckPtr>=m_ConcernedObject.GetBufferSize())
			{
				m_ConcernedObjectCheckPtr=1;
				break;
			}
		}
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}

CDOSBaseObject::CONCERNED_OBJECT_INFO * CDOSBaseObject::FindConcernedObjectInfo(OBJECT_ID ObjectID)
{
	return m_ConcernedObject.Find(ObjectID);
}
