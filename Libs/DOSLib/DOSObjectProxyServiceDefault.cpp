/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectProxyService.cpp                                */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CEasyBuffer	CDOSObjectProxyServiceDefault::m_CompressBuffer;

CDOSObjectProxyServiceDefault::CDOSObjectProxyServiceDefault(void)
{

}

CDOSObjectProxyServiceDefault::~CDOSObjectProxyServiceDefault(void)
{
}

UINT CDOSObjectProxyServiceDefault::AddUseRef()
{
	return CNetService::AddUseRef();
}
void CDOSObjectProxyServiceDefault::Release()
{
	CNetService::Release();
}
void CDOSObjectProxyServiceDefault::Destory()
{
	CNetService::Destory();
}
BYTE CDOSObjectProxyServiceDefault::GetProxyType()
{
	return m_Config.ProxyType;
}
void CDOSObjectProxyServiceDefault::SetID(UINT ID)
{
	CNetService::SetID(ID);
}
UINT CDOSObjectProxyServiceDefault::GetID()
{
	return CNetService::GetID();
}
bool CDOSObjectProxyServiceDefault::StartService()
{
	return Start() != FALSE;
}
void CDOSObjectProxyServiceDefault::StopService()
{
	SafeTerminate();
}
bool CDOSObjectProxyServiceDefault::PushMessage(CDOSMessagePacket * pPacket)
{
	CAutoLock Lock(m_EasyCriticalSection);

	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
	pPacket->SetAllocTime(4);
	if (m_MsgQueue.PushBack(pPacket))
	{
		return true;
	}
	else
	{
		((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
	}
	return false;
}
bool CDOSObjectProxyServiceDefault::PushBroadcastMessage(CDOSMessagePacket * pPacket)
{
	LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
	while (Pos)
	{
		CDOSObjectProxyConnectionDefault * pConnection = m_ConnectionPool.GetNext(Pos);
		if (pConnection)
		{
			pConnection->PushMessage(pPacket);
		}
	}
	return true;
}
IDOSObjectProxyConnectionBase * CDOSObjectProxyServiceDefault::GetConnection(UINT ID)
{
	return m_ConnectionPool.GetObject(ID);
}

float CDOSObjectProxyServiceDefault::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}
float CDOSObjectProxyServiceDefault::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}


bool CDOSObjectProxyServiceDefault::Init(CDOSServer * pServer, CLIENT_PROXY_CONFIG& Config)
{
	SetServer(pServer);
	m_Config = Config;
	return true;
}



BOOL CDOSObjectProxyServiceDefault::OnStart()
{
	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	if (!m_MsgQueue.Create(m_Config.ConnectionMsgQueueSize))
	{
		PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]创建%u大小的消息队列失败！"),
			GetID(),
			m_Config.ConnectionMsgQueueSize);
		return FALSE;
	}

	if (!m_MessageMap.Create(m_Config.GlobalMsgMapSize))
	{
		PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]创建%u大小的消息映射表失败！"),
			GetID(),
			m_Config.GlobalMsgMapSize);
		return FALSE;
	}

	if (m_Config.MinMsgCompressSize)
	{
		switch (m_Config.MsgCompressType)
		{
		case MSG_COMPRESS_LZO:
		{
			if (lzo_init() != LZO_E_OK)
			{
				PrintDOSLog(_T("DOSLib"), _T("代理服务开启消息压缩失败"));
				m_Config.MsgCompressType = MSG_COMPRESS_NONE;
				m_Config.MinMsgCompressSize = 0;
			}
			PrintDOSLog(_T("DOSLib"), _T("代理服务开启消息压缩"));
		}
		break;
		default:
			PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]不支持消息压缩模式%u！"),
				GetID(), m_Config.MsgCompressType);
			m_Config.MsgCompressType = MSG_COMPRESS_NONE;
			m_Config.MinMsgCompressSize = 0;
			break;
		}
	}
	else
	{
		m_Config.MsgCompressType = MSG_COMPRESS_NONE;
	}

	if (!Create(IPPROTO_TCP,
		m_Config.AcceptQueueSize,
		m_Config.RecvBufferSize,
		m_Config.SendBufferSize,
		m_Config.ParallelAcceptCount,
		DEFAULT_PARALLEL_RECV,
		false))
	{
		PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]创建失败！"), GetID());
		return FALSE;
	}

	if (!StartListen(m_Config.ListenAddress))
	{
		PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]侦听于(%s:%u)失败！"),
			GetID(),
			m_Config.ListenAddress.GetIPString(),
			m_Config.ListenAddress.GetPort());
		return FALSE;
	}
	PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]侦听于(%s:%u)！"),
		GetID(),
		m_Config.ListenAddress.GetIPString(),
		m_Config.ListenAddress.GetPort());

	PrintDOSLog(_T("DOSLib"), _T("对象代理[%u]线程[%u]已启动"), GetID(), GetThreadID());

	if (m_Config.MsgCompressType == MSG_COMPRESS_LZO)
	{
		if (m_CompressBuffer.GetBufferSize() < m_Config.MaxMsgSize)
		{
			if (!m_CompressBuffer.Create(m_Config.MaxMsgSize))
			{
				PrintDOSLog(_T("DOSLib"), _T("创建%u大小的压缩缓冲失败！"),
					m_Config.MaxMsgSize);
				return FALSE;
			}
		}
	}

	if (!m_ConnectionPool.Create(m_Config.MaxConnection))
	{
		PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]创建%u大小的连接池失败！"),
			GetID(),
			m_Config.MaxConnection);
		return FALSE;
	}
	return TRUE;
}

BOOL CDOSObjectProxyServiceDefault::OnRun()
{
	m_ThreadPerformanceCounter.DoPerformanceCount();
	int ProcessCount = Update();
	if (ProcessCount == 0)
	{
		DoSleep(1);
	}
	return TRUE;
}

void CDOSObjectProxyServiceDefault::OnTerminate()
{
	CNetService::Close();
	PrintDOSLog(_T("DOSLib"), _T("代理服务[%u]关闭"), GetID());
}

void CDOSObjectProxyServiceDefault::OnClose()
{
	CAutoLock Lock(m_EasyCriticalSection);

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		if (!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("CDOSObjectProxyServiceBase::OnClose:释放消息内存块失败！"));
		}
	}	

	LPVOID Pos=m_ConnectionPool.GetFirstObjectPos();
	while(Pos)
	{
		CDOSObjectProxyConnectionDefault * pConnection=m_ConnectionPool.GetNext(Pos);

		pConnection->Destory();
		 
	}
	m_ConnectionPool.Clear();
	m_MessageMap.Clear();
}

int CDOSObjectProxyServiceDefault::Update(int ProcessPacketLimit)
{
	int ProcessCount = CNetService::Update(ProcessPacketLimit);

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(pPacket))
	{
		//PrintDOSDebugLog(0,_T("发送了消息[%u]"),pPacket->GetMessage().GetMsgID());
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMsg(&(pPacket->GetMessage()));
		else
			OnMsg(&(pPacket->GetMessage()));

		if (!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("释放消息内存块失败！"));
		}

		ProcessCount++;
		if (ProcessCount >= ProcessPacketLimit)
			break;
	}


	LPVOID Pos = m_ConnectionPool.GetFirstObjectPos();
	while (Pos)
	{
		CDOSObjectProxyConnectionDefault * pConnection = m_ConnectionPool.GetNext(Pos);
		if (pConnection->IsConnected())
			ProcessCount += pConnection->Update();
		else
			DeleteConnection(pConnection);
	}
	return ProcessCount;
}


CBaseNetConnection * CDOSObjectProxyServiceDefault::CreateConnection(CIPAddress& RemoteAddress)
{
	CDOSObjectProxyConnectionDefault * pConnection=NULL;
	UINT ID=m_ConnectionPool.NewObject(&pConnection);
	if(pConnection)
	{
		if(!pConnection->Init(this,ID))
		{
			m_ConnectionPool.DeleteObject(ID);
		}
		return pConnection;
	}
	return NULL;
}

BOOL CDOSObjectProxyServiceDefault::DeleteConnection(CBaseNetConnection * pConnection)
{
	pConnection->Destory();
	if(m_ConnectionPool.DeleteObject(pConnection->GetID()))
	{
		return TRUE;
	}
	return FALSE;
}

OBJECT_ID CDOSObjectProxyServiceDefault::GetGlobalMsgMapObjectID(MSG_ID_TYPE MsgID)
{
	FUNCTION_BEGIN;
	OBJECT_ID * pObjectID = m_MessageMap.Find(MsgID);
	if (pObjectID)
	{
		return *pObjectID;
	}
	FUNCTION_END;
	return 0;
}

OBJECT_ID CDOSObjectProxyServiceDefault::GetUnhandleMsgReceiverID()
{
	return m_UnhandleMsgReceiverID;
}


void CDOSObjectProxyServiceDefault::OnMsg(CDOSMessage * pMessage)
{
	PrintDOSLog(_T("DOSLib"), _T("ProxyService收到非系统消息0x%llX"), pMessage->GetMsgID());
}

void CDOSObjectProxyServiceDefault::OnSystemMsg(CDOSMessage * pMessage)
{
	switch (pMessage->GetMsgID())
	{
	case DSM_PROXY_REGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pMessage->GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pMessage->GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoRegisterGlobalMsgMap(pMsgIDs[i], pMessage->GetSenderID());
			}
		}
		break;
	case DSM_PROXY_UNREGISTER_GLOBAL_MSG_MAP:
		if (pMessage->GetDataLength() >= sizeof(MSG_ID_TYPE))
		{
			int Count = (pMessage->GetDataLength()) / sizeof(MSG_ID_TYPE);
			MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pMessage->GetDataBuffer());
			for (int i = 0; i < Count; i++)
			{
				DoUnregisterGlobalMsgMap(pMsgIDs[i], pMessage->GetSenderID());
			}
		}
		break;
	case DSM_PROXY_SET_UNHANDLE_MSG_RECEIVER:
		m_UnhandleMsgReceiverID = pMessage->GetSenderID();
		break;
	case DSM_ROUTE_LINK_LOST:
		ClearMsgMapByRouterID(pMessage->GetSenderID().RouterID);
		break;
	default:
		PrintDOSLog(_T("DOSLib"), _T("ProxyService收到未知系统消息0x%llX"), pMessage->GetMsgID());
	}
}

bool CDOSObjectProxyServiceDefault::DoRegisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	PrintDOSLog(_T("DOSLib"), _T("0x%llX注册了全局代理消息映射[0x%X]！"), ObjectID.ID, MsgID);
	return m_MessageMap.Insert(MsgID, ObjectID) != 0;
}
bool CDOSObjectProxyServiceDefault::DoUnregisterGlobalMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID)
{
	PrintDOSLog(_T("DOSLib"), _T("0x%llX注销了全局代理消息映射[0x%X]！"), ObjectID.ID, MsgID);
	return m_MessageMap.Delete(MsgID) != FALSE;
}

void CDOSObjectProxyServiceDefault::ClearMsgMapByRouterID(UINT RouterID)
{
	void * Pos = m_MessageMap.GetFirstObjectPos();
	while (Pos)
	{
		MSG_ID_TYPE MsgID;
		OBJECT_ID * pObjectID = m_MessageMap.GetNextObject(Pos, MsgID);
		if (pObjectID->RouterID == RouterID)
		{
			m_MessageMap.Delete(MsgID);
		}
	}
}


