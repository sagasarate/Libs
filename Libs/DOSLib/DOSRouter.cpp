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
	ResetStatData();
	FUNCTION_END;
}

CDOSRouter::~CDOSRouter(void)
{
}

BOOL CDOSRouter::Init(CDOSServer * pServer)
{
	FUNCTION_BEGIN;
	m_pServer=pServer;
	return Start();
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSRouter::OnStart()
{
	FUNCTION_BEGIN;
	DOS_CONFIG& Config=m_pServer->GetConfig();

	m_MsgProcessLimit=Config.RouterMsgProcessLimit;

	if(m_MsgProcessLimit==0)
	{
		PrintDOSLog(_T("DOSLib"),_T("路由消息处理限制不能为0！"));
		return FALSE;
	}

	if(!m_MsgQueue.Create(Config.MaxRouterSendMsgQueue))
	{
		PrintDOSLog(_T("DOSLib"),_T("创建%d大小的路由消息队列失败！"),Config.MaxRouterSendMsgQueue);
		return FALSE;
	}

	ResetStatData();

	m_ThreadPerformanceCounter.Init(GetThreadHandle(),THREAD_CPU_COUNT_TIME);

	//将连接ID都设置为RouterID
	Config.RouterLinkConfig.ServerID = GetRouterID();
	for (UINT i = 0; i < Config.RouterLinkConfig.ServiceConfig.ServiceList.GetCount(); i++)
	{
		Config.RouterLinkConfig.ServiceConfig.ServiceList[i].ReportID = GetRouterID();
	}
	for (UINT i = 0; i < Config.RouterLinkConfig.ConnectionConfig.ConnectionList.GetCount(); i++)
	{
		Config.RouterLinkConfig.ConnectionConfig.ConnectionList[i].ReportID = GetRouterID();
	}

	if (!CEasyNetLinkManager::Init(m_pServer, Config.RouterLinkConfig))
	{
		PrintDOSLog(_T("DOSLib"),_T("连接管理器初始化失败！"));
		return FALSE;
	}

	PrintDOSLog(_T("DOSLib"),_T("路由线程[%u]已启动"),GetThreadID());
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDOSRouter::OnClose()
{
	FUNCTION_BEGIN;
	CEasyNetLinkManager::Destory();
	m_MsgQueue.Destory();
	FUNCTION_END;
}

BOOL CDOSRouter::OnRun()
{
	FUNCTION_BEGIN;
	EXCEPTION_CATCH_START

	int ProcessCount=CEasyNetLinkManager::Update();
	//处理消息发送
	ProcessCount+=DoMessageRoute(m_MsgProcessLimit);
	if(ProcessCount==0)
	{
		DoSleep(1);
	}

	m_ThreadPerformanceCounter.DoPerformanceCount();

	EXCEPTION_CATCH_END
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

CEasyNetLink * CDOSRouter::OnCreateLink(UINT ID)
{
	FUNCTION_BEGIN;
	return new CDOSRouterLink();
	FUNCTION_END;
	return NULL;
}

void CDOSRouter::OnLinkStart(CEasyNetLink * pConnection)
{

}

void CDOSRouter::OnLinkEnd(CEasyNetLink * pConnection)
{

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
	if(pData)
		memcpy(pPacket->GetMessage().GetDataBuffer(),pData,DataSize);
	pPacket->SetTargetIDs(1,&ReceiverID);
	pPacket->MakePacketLength();

	BOOL Ret=RouterMessage(pPacket);
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
		PrintDOSLog(_T("DOSLib"),_T("消息[0x%X]没有发送目标，不发送！"),pPacket->GetMessage().GetMsgID());
		return FALSE;
	}
	pPacket->MakePacketLength();
	if(!pPacket->CheckPacket())
	{
		PrintDOSLog(_T("DOSLib"),_T("消息[0x%X]有效性检查失败！"),pPacket->GetMessage().GetMsgID());
		return FALSE;
	}
	((CDOSServer *)GetServer())->AddRefMessagePacket(pPacket);
	pPacket->SetAllocTime(1);
	if(!m_MsgQueue.PushBack(pPacket))
	{
		((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket);
		PrintDOSLog(_T("DOSLib"),_T("将消息压入路由发送队列失败！"));
		return FALSE;
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

UINT CDOSRouter::GetRouterID()
{
	FUNCTION_BEGIN;
	return ((CDOSServer *)GetServer())->GetConfig().RouterID;
	FUNCTION_END;
	return 0;
}

int CDOSRouter::DoMessageRoute(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pPacket;

	int ProcessCount=0;
	while(m_MsgQueue.PopFront(pPacket))
	{
		//PrintDOSDebugLog(0,_T("路由了消息[%u]"),pPacket->GetMessage().GetCmdID());

		AtomicInc(&m_RouteInMsgCount);
		AtomicAdd(&m_RouteInMsgFlow,pPacket->GetPacketLength());

		ID_LIST_COUNT_TYPE ReceiverIDCount=pPacket->GetTargetIDCount();
		OBJECT_ID * pReceiverIDs=pPacket->GetTargetIDs();
		if(ReceiverIDCount==0)
		{
			PrintDOSLog(_T("DOSLib"),_T("错误的消息格式！"));
			continue;
		}
		if((ReceiverIDCount==1||IsSameRouter(pReceiverIDs,ReceiverIDCount))
			&&pReceiverIDs[0].RouterID!=BROAD_CAST_ROUTER_ID)
		{
			//只有一个接收对象，或者接受对象在同一个路由,路由广播除外
			if(pReceiverIDs->RouterID==0||pReceiverIDs->RouterID==GetRouterID())
			{
				//发给自己路由的消息，路由ID=0等同自己
				DispatchMessage(pPacket,pReceiverIDs,ReceiverIDCount);
			}
			else
			{
				CDOSRouterLink * pRouterLink=dynamic_cast<CDOSRouterLink *>(GetLink(pReceiverIDs->RouterID));
				if(pRouterLink)
				{
					AtomicInc(&m_RouteOutMsgCount);
					AtomicAdd(&m_RouteOutMsgFlow,pPacket->GetPacketLength());
					pRouterLink->SendData(pPacket->GetPacketBuffer(),pPacket->GetPacketLength());
				}
				else
				{
					PrintDOSLog(_T("DOSLib"),_T("无法找到路由%u！"),pReceiverIDs->RouterID);
				}
			}
		}
		else
		{
			//接收对象分布于多个路由


			ID_LIST_COUNT_TYPE GroupCount=0;
			OBJECT_ID * pReceiverIDGroup=pReceiverIDs;

			while(ReceiverIDCount)
			{
				GroupCount=GetGroupCount(pReceiverIDGroup,ReceiverIDCount);
				if(pReceiverIDGroup->RouterID==0||pReceiverIDGroup->RouterID==GetRouterID())
				{
					//发给自己路由的消息，路由ID=0等同自己
					DispatchMessage(pPacket,pReceiverIDGroup,GroupCount);
				}
				else if(pReceiverIDGroup->RouterID==BROAD_CAST_ROUTER_ID)
				{
					//处理广播
					pPacket->SetTargetIDs(GroupCount,NULL);
					if(pReceiverIDs!=pReceiverIDGroup)
						memmove(pReceiverIDs,pReceiverIDGroup,GroupCount);
					for(UINT i=0;i<GroupCount;i++)
					{
						//路由ID设置为0避免被再次广播
						pReceiverIDs[i].RouterID=0;
					}
					pPacket->MakePacketLength();
					for(UINT i=0;i<GetLinkCount();i++)
					{
						CDOSRouterLink * pRouterLink=dynamic_cast<CDOSRouterLink *>(GetLinkByIndex(i));
						if(pRouterLink)
						{
							AtomicInc(&m_RouteOutMsgCount);
							AtomicAdd(&m_RouteOutMsgFlow,pPacket->GetPacketLength());
							pRouterLink->SendData(pPacket->GetPacketBuffer(),pPacket->GetPacketLength());
						}
					}
					//在本路由广播
					DispatchMessage(pPacket,pReceiverIDs,GroupCount);
				}
				else
				{
					CDOSRouterLink * pRouterLink=dynamic_cast<CDOSRouterLink *>(GetLink(pReceiverIDGroup->RouterID));
					if(pRouterLink)
					{


						pPacket->SetTargetIDs(GroupCount,NULL);
						if(pReceiverIDs!=pReceiverIDGroup)
							memmove(pReceiverIDs,pReceiverIDGroup,GroupCount);
						pPacket->MakePacketLength();

						AtomicInc(&m_RouteOutMsgCount);
						AtomicAdd(&m_RouteOutMsgFlow,pPacket->GetPacketLength());

						pRouterLink->SendData(pPacket->GetPacketBuffer(),pPacket->GetPacketLength());
					}
					else
					{
						PrintDOSLog(_T("DOSLib"),_T("无法找到路由%u！"),pReceiverIDGroup->RouterID);
					}
				}
				pReceiverIDGroup+=GroupCount;
				ReceiverIDCount-=GroupCount;
			}

		}

		if(!((CDOSServer *)GetServer())->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"),_T("释放消息内存块失败！"));
		}
		ProcessPacketLimit--;
		ProcessCount++;
		if(ProcessPacketLimit<=0)
			break;

	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}


BOOL CDOSRouter::IsSameRouter(OBJECT_ID * pReceiverIDs,int Count)
{
	FUNCTION_BEGIN;
	ROUTE_ID_TYPE RouterID=pReceiverIDs->RouterID;
	for(int i=1;i<Count;i++)
	{
		if(pReceiverIDs[i].RouterID!=RouterID)
			return FALSE;
	}
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

int CDOSRouter::GetGroupCount(OBJECT_ID * pReceiverIDs,int Count)
{
	FUNCTION_BEGIN;
	ROUTE_ID_TYPE RouterID=pReceiverIDs->RouterID;
	for(int i=1;i<Count;i++)
	{
		if(pReceiverIDs[i].RouterID!=RouterID)
			return i;
	}
	return Count;
	FUNCTION_END;
	return 0;
}


BOOL CDOSRouter::DispatchMessage(CDOSMessagePacket * pPacket,OBJECT_ID * pReceiverIDs,int Count)
{
	FUNCTION_BEGIN;
	UINT RouterID=GetRouterID();
	for(int i=0;i<Count;i++)
	{
		if(pReceiverIDs[i].ObjectTypeID==DOT_PROXY_OBJECT)
		{
			CDOSProxyManager * pProxyManager = ((CDOSServer *)GetServer())->GetProxyManager();


			if(pReceiverIDs[i].ObjectIndex==0)
			{
				//发送到Service的消息
				BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pReceiverIDs[i].GroupIndex);
				BYTE ProxyID = GET_PROXY_ID_FROM_PROXY_GROUP_INDEX(pReceiverIDs[i].GroupIndex);
				if (ProxyType == BROAD_CAST_PROXY_TYPE)
				{
					for (UINT j = 0; j < pProxyManager->GetProxyServiceCount(); j++)
					{
						AtomicInc(&m_RouteOutMsgCount);
						AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
						pProxyManager->GetProxyServiceByIndex(j)->PushMessage(pPacket);
					}
				}
				else
				{
					IDOSObjectProxyServiceBase * pProxyService = pProxyManager->GetProxyServiceByID(ProxyID);
					if (pProxyService == NULL)
						pProxyService = ((CDOSServer *)GetServer())->GetProxyManager()->GetProxyServiceByType(ProxyType);
					if (pProxyService)
					{
						AtomicInc(&m_RouteOutMsgCount);
						AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
						pProxyService->PushMessage(pPacket);
					}
					else
					{
						PrintDOSDebugLog(0, _T("CDOSRouter::DispatchMessage:将[0x%llX]发出的消息[%X]递送到代理服务[%llX]时代理服务不存在"),
							pPacket->GetMessage().GetSenderID(),
							pPacket->GetMessage().GetMsgID(),
							pReceiverIDs[i]);
					}
				}
			}
			else if(pReceiverIDs[i].ObjectIndex==BROAD_CAST_OBJECT_INDEX)
			{
				//群发消息
				BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(pReceiverIDs[i].GroupIndex);
				BYTE ProxyID = GET_PROXY_ID_FROM_PROXY_GROUP_INDEX(pReceiverIDs[i].GroupIndex);
				if (ProxyType == BROAD_CAST_PROXY_TYPE)
				{
					for (UINT j = 0; j < pProxyManager->GetProxyServiceCount(); i++)
					{
						AtomicInc(&m_RouteOutMsgCount);
						AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
						pProxyManager->GetProxyServiceByIndex(j)->PushBroadcastMessage(pPacket);
					}
				}
				else
				{
					IDOSObjectProxyServiceBase * pProxyService = pProxyManager->GetProxyServiceByID(ProxyID);
					if (pProxyService == NULL)
						pProxyService = ((CDOSServer *)GetServer())->GetProxyManager()->GetProxyServiceByType(ProxyType);
					if (pProxyService)
					{
						AtomicInc(&m_RouteOutMsgCount);
						AtomicAdd(&m_RouteOutMsgFlow, pPacket->GetMessage().GetMsgLength());
						pProxyService->PushBroadcastMessage(pPacket);
					}
					else
					{
						PrintDOSDebugLog(0, _T("CDOSRouter::DispatchMessage:将[0x%llX]发出的消息[%X]递送到代理服务[%llX]时代理服务不存在"),
							pPacket->GetMessage().GetSenderID(),
							pPacket->GetMessage().GetMsgID(),
							pReceiverIDs[i]);
					}
				}
			}
			else
			{
				//单个消息
				IDOSObjectProxyConnectionBase * pProxyConnection = pProxyManager->GetProxyConnect(pReceiverIDs[i]);
				if (pProxyConnection)
				{
					AtomicInc(&m_RouteOutMsgCount);
					AtomicAdd(&m_RouteOutMsgFlow,pPacket->GetPacketLength());
					pProxyConnection->PushMessage(pPacket);
				}
				else
				{
					PrintDOSDebugLog(0,_T("CDOSRouter::DispatchMessage:将[0x%llX]发出的消息[%X]递送到代理对象[%llX]时代理对象不存在"),
						pPacket->GetMessage().GetSenderID(),
						pPacket->GetMessage().GetMsgID(),
						pReceiverIDs[i]);
				}
			}
		}
		else
		{
			pReceiverIDs[i].RouterID=RouterID;
			if(((CDOSServer *)GetServer())->GetObjectManager()->PushMessage(pReceiverIDs[i],pPacket))
			{
				AtomicInc(&m_RouteOutMsgCount);
				AtomicAdd(&m_RouteOutMsgFlow,pPacket->GetMessage().GetMsgLength());
			}
			else
			{
				PrintDOSDebugLog(0,_T("CDOSRouter::DispatchMessage:将[0x%llX]发出的消息[%X]递送到对象[%llX]失败"),
					pPacket->GetMessage().GetSenderID(),
					pPacket->GetMessage().GetMsgID(),
					pReceiverIDs[i]);
			}
		}
	}
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

