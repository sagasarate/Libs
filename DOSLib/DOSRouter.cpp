/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSRouter.cpp                                            */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSRouter, CEasyNetLinkManager);

CDOSRouter::CDOSRouter(void)
{
	FUNCTION_BEGIN;
	m_pServer=NULL;
	m_MsgQueue.SetTag(_T("CDOSRouter"));
	m_MsgStateInfos.SetTag(_T("CDOSRouter"));
	ResetStatData();
	FUNCTION_END;
}

CDOSRouter::~CDOSRouter(void)
{
	SafeTerminate();
}

BOOL CDOSRouter::Init(CDOSServer * pServer)
{
	FUNCTION_BEGIN;
	m_pServer=pServer;
	m_Config = m_pServer->GetConfig().RouterConfig;

	if (m_Config.RouterID == 0)
	{
		PrintDOSLog(_T("路由ID不能为0！"));
		return FALSE;
	}

	return Start();
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSRouter::OnStart()
{
	FUNCTION_BEGIN;
	

	m_MsgProcessLimit = m_Config.RouterMsgProcessLimit;

	if(m_MsgProcessLimit==0)
	{
		PrintDOSLog(_T("路由消息处理限制不能为0！"));
		return FALSE;
	}

	if (!m_MsgQueue.Create(m_Config.MaxRouterSendMsgQueue))
	{
		PrintDOSLog(_T("创建%d大小的路由消息队列失败！"), m_Config.MaxRouterSendMsgQueue);
		return FALSE;
	}

	if (m_Config.StatMsgTransfer)
	{
		m_MsgStateInfos.Create(256, 256, 256);
	}

	ResetStatData();

	m_ThreadPerformanceCounter.Init(GetThreadHandle(),THREAD_CPU_COUNT_TIME);

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.SetTargetThread(this);
		m_GuardThread.SetKeepAliveTime(m_Config.GuardThreadKeepAliveTime, m_Config.GuardThreadKeepAliveCount);
		m_GuardThread.Start();
	}

	PrintDOSLog(_T("路由线程[%u]已启动"),GetThreadID());
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSRouter::OnRun()
{
	FUNCTION_BEGIN;
	EXCEPTION_CATCH_START

	int ProcessCount = 0;
	//处理消息发送
	ProcessCount+=DoMessageRoute(m_MsgProcessLimit);
	if(ProcessCount==0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}

	m_ThreadPerformanceCounter.DoPerformanceCount();

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.MakeKeepAlive();
	}

	EXCEPTION_CATCH_END
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDOSRouter::OnTerminate()
{
	if (m_Config.EnableGuardThread)
		m_GuardThread.SafeTerminate();

	if (GetServer())
	{
		CDOSMessagePacket * pPacket;
		while (m_MsgQueue.PopFront(&pPacket))
		{
			((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
		}
		m_MsgQueue.Destory();
	}
}




BOOL CDOSRouter::RouterMessage(OBJECT_ID SenderID,OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	UINT PacketSize=CDOSMessagePacket::CaculatePacketLength(DataSize,1);
	CDOSMessagePacket * pPacket=((CDOSServer *)GetServer())->NewMessagePacket(PacketSize);
	if(pPacket==NULL)
		return FALSE;
	pPacket->GetMessage().SetMsgID(MsgID);
	pPacket->GetMessage().SetSenderID(SenderID);
	pPacket->GetMessage().SetDataLength(DataSize);
	pPacket->GetMessage().SetMsgFlag(MsgFlag);
	if (pData)
		pPacket->GetMessage().PutMsgData(0, pData, DataSize);
	pPacket->SetTargetIDs(1,&ReceiverID);
	pPacket->MakePacketLength();

	BOOL Ret=RouterMessage(pPacket);
	((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSRouter::RouterMessage(OBJECT_ID SenderID, OBJECT_ID * pReceiverID, UINT ReceiverCount, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	UINT PacketSize = CDOSMessagePacket::CaculatePacketLength(DataSize, ReceiverCount);
	CDOSMessagePacket * pPacket = ((CDOSServer *)GetServer())->NewMessagePacket(PacketSize);
	if (pPacket == NULL)
		return FALSE;
	pPacket->GetMessage().SetMsgID(MsgID);
	pPacket->GetMessage().SetSenderID(SenderID);
	pPacket->GetMessage().SetDataLength(DataSize);
	pPacket->GetMessage().SetMsgFlag(MsgFlag);
	if (pData)
		pPacket->GetMessage().PutMsgData(0, pData, DataSize);		
	pPacket->SetTargetIDs(ReceiverCount, pReceiverID);
	pPacket->MakePacketLength();

	BOOL Ret = RouterMessage(pPacket);
	((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
	return Ret;
	FUNCTION_END;
	return FALSE;
}
BOOL CDOSRouter::RouterMessage(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	if(pPacket->GetTargetIDCount()<=0)
	{
		PrintDOSLog(_T("消息[0x%X]没有发送目标，不发送！"),pPacket->GetMessage().GetMsgID());
		return FALSE;
	}
	pPacket->MakePacketLength();
	if(!pPacket->CheckPacket())
	{
		PrintDOSLog(_T("消息[0x%X]有效性检查失败！"),pPacket->GetMessage().GetMsgID());
		return FALSE;
	}
	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
	pPacket->SetAllocTime(0x11);
#endif
	CAutoLock Lock(m_EasyCriticalSection);
	if(!m_MsgQueue.PushBack(&pPacket))
	{
		((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
		PrintDOSLog(_T("将消息压入路由发送队列失败！"));
		return FALSE;
	}
	if (m_Config.StatMsgTransfer)
	{
		MSG_STATE_INFO * pStateInfo = NULL;
		m_MsgStateInfos.New(pPacket->GetMessage().GetMsgID(), &pStateInfo);
		if (pStateInfo)
		{
			pStateInfo->CurCount++;
			pStateInfo->CurFlow += pPacket->GetPacketLength();
		}
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

UINT CDOSRouter::GetRouterID()
{
	FUNCTION_BEGIN;
	return m_Config.RouterID;
	FUNCTION_END;
	return 0;
}

void CDOSRouter::PrintMsgStat(UINT LogChannel)
{
	CAutoLock Lock(m_EasyCriticalSection);
	void * Pos = m_MsgStateInfos.GetFirstObjectPos();
	while (Pos)
	{
		UINT MsgID;
		MSG_STATE_INFO * pStateInfo = m_MsgStateInfos.GetNextObject(Pos, MsgID);
		CLogManager::GetInstance()->PrintLog(LogChannel,
			ILogPrinter::LOG_LEVEL_NORMAL, NULL, _T("[0x%lX]:C=%u,F=%u"), MsgID, pStateInfo->CurCount, pStateInfo->CurFlow);
		pStateInfo->CurCount = 0;
		pStateInfo->CurFlow = 0;
	}
	m_MsgStateInfos.Clear();
}


int CDOSRouter::DoMessageRoute(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pPacket;

	int ProcessCount = 0;
	while (m_MsgQueue.PopFront(&pPacket))
	{
		//PrintDOSDebugLog(_T("路由了消息[%u]"),pPacket->GetMessage().GetCmdID());
#ifdef _DEBUG
		pPacket->SetAllocTime(0x21);
#endif

		AtomicInc(&m_RouteInMsgCount);
		AtomicAdd(&m_RouteInMsgFlow, pPacket->GetPacketLength());

		assert(pPacket->VerfyPacketLen());

		ID_LIST_COUNT_TYPE ReceiverIDCount = pPacket->GetTargetIDCount();
		OBJECT_ID * pReceiverIDs = pPacket->GetTargetIDPtr();
		if (ReceiverIDCount == 0)
		{
			PrintDOSLog(_T("错误的消息格式！"));
			continue;
		}
		if ((ReceiverIDCount == 1 || IsSameRouter(pReceiverIDs, ReceiverIDCount))
			&& pReceiverIDs[0].RouterID != BROAD_CAST_ROUTER_ID)
		{
			//只有一个接收对象，或者接受对象在同一个路由,路由广播除外
			if (pReceiverIDs->RouterID == 0 || pReceiverIDs->RouterID == GetRouterID())
			{
				//发给自己路由的消息，路由ID=0等同自己
				DispatchMessage(pPacket, pReceiverIDs, ReceiverIDCount);
			}
			else
			{
				m_pServer->GetRouterLinkManager()->SendMessage(pReceiverIDs->RouterID, pPacket);
			}
		}
		else
		{
			//接收对象分布于多个路由


			ID_LIST_COUNT_TYPE GroupCount = 0;
			OBJECT_ID * pReceiverIDGroup = pReceiverIDs;

			while (ReceiverIDCount)
			{
				GroupCount = GetGroupCount(pReceiverIDGroup, ReceiverIDCount);
				if (pReceiverIDGroup->RouterID == 0 || pReceiverIDGroup->RouterID == GetRouterID())
				{
					//发给自己路由的消息，路由ID=0等同自己
					DispatchMessage(pPacket, pReceiverIDGroup, GroupCount);
				}
				else if (pReceiverIDGroup->RouterID == BROAD_CAST_ROUTER_ID)
				{
					//处理广播
					pPacket->SetTargetIDs(GroupCount, NULL);
					if (pReceiverIDs != pReceiverIDGroup)
						memmove(pReceiverIDs, pReceiverIDGroup, GroupCount * sizeof(OBJECT_ID));
					for (UINT i = 0; i < GroupCount; i++)
					{
						//路由ID设置为0避免被再次广播
						pReceiverIDs[i].RouterID = 0;
					}
					pPacket->MakePacketLength();
					UINT SendCount = m_pServer->GetRouterLinkManager()->BroadcastMessage(pPacket);
					AtomicAdd(&m_RouteOutMsgCount, SendCount);
					AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetPacketLength()*SendCount);
					//在本路由广播
					DispatchMessage(pPacket, pReceiverIDs, GroupCount);
				}
				else
				{
					pPacket->SetTargetIDs(GroupCount, NULL);
					if (pReceiverIDs != pReceiverIDGroup)
						memmove(pReceiverIDs, pReceiverIDGroup, GroupCount * sizeof(OBJECT_ID));
					pPacket->MakePacketLength();

					AtomicInc(&m_RouteOutMsgCount);
					AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetPacketLength());

					m_pServer->GetRouterLinkManager()->SendMessage(pReceiverIDs->RouterID, pPacket);
				}
				pReceiverIDGroup += GroupCount;
				ReceiverIDCount -= GroupCount;
			}

		}

		if (!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("释放消息内存块失败！"));
		}
		ProcessPacketLimit--;
		ProcessCount++;
		if (ProcessPacketLimit <= 0)
			break;

	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}


BOOL CDOSRouter::IsSameRouter(const OBJECT_ID * pReceiverIDs, int Count)
{
	FUNCTION_BEGIN;
	ROUTE_ID_TYPE RouterID = pReceiverIDs->RouterID;
	for (int i = 1; i < Count; i++)
	{
		if (pReceiverIDs[i].RouterID != RouterID)
			return FALSE;
	}
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

int CDOSRouter::GetGroupCount(const OBJECT_ID * pReceiverIDs, int Count)
{
	FUNCTION_BEGIN;
	ROUTE_ID_TYPE RouterID = pReceiverIDs->RouterID;
	for (int i = 1; i < Count; i++)
	{
		if (pReceiverIDs[i].RouterID != RouterID)
			return i;
	}
	return Count;
	FUNCTION_END;
	return 0;
}


BOOL CDOSRouter::DispatchMessage(CDOSMessagePacket * pPacket, const OBJECT_ID * pReceiverIDs, int Count)
{
	FUNCTION_BEGIN;
	UINT RouterID = GetRouterID();
	for (int i = 0; i < Count; i++)
	{
		OBJECT_ID ReceiverID = pReceiverIDs[i];
		ReceiverID.RouterID = RouterID;

		if (ReceiverID.ObjectTypeID == DOT_PROXY_OBJECT || ReceiverID.ObjectTypeID == BROAD_CAST_OBJECT_TYPE_ID)
		{
			if (((CDOSServer *)GetServer())->GetProxyManager()->PushMessage(ReceiverID, pPacket))
			{
				AtomicInc(&m_RouteOutMsgCount);
				AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
			}
			//else
			//{
			//	PrintDOSDebugLog(_T("将[0x%llX]发出的消息[%X]递送到代理对象[%llX]失败"),
			//		pPacket->GetMessage().GetSenderID(),
			//		pPacket->GetMessage().GetMsgID(),
			//		pReceiverIDs[i]);
			//}
		}
		if (ReceiverID.ObjectTypeID != DOT_PROXY_OBJECT)
		{

			if (((CDOSServer *)GetServer())->GetObjectManager()->PushMessage(ReceiverID, pPacket))
			{
				AtomicInc(&m_RouteOutMsgCount);
				AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
			}
			//else
			//{
			//	PrintDOSDebugLog(_T("将[0x%llX]发出的消息[%X]递送到对象[%llX]失败"),
			//		pPacket->GetMessage().GetSenderID(),
			//		pPacket->GetMessage().GetMsgID(),
			//		pReceiverIDs[i]);
			//}
		}
	}
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

