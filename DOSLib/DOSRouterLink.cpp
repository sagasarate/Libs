/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSRouterLink.cpp                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSRouterLink, CEasyNetLink);

CDOSRouterLink::CDOSRouterLink()
{

}

CDOSRouterLink::~CDOSRouterLink(void)
{
}

void CDOSRouterLink::OnLinkStart()
{
	FUNCTION_BEGIN;
	PrintDOSLog(_T("收到路由(%d)的连接"),GetID());
	FUNCTION_END;
}
void CDOSRouterLink::OnLinkEnd()
{
	FUNCTION_BEGIN;
	PrintDOSLog(_T("路由(%d)的连接断开！"),GetID());
	OBJECT_ID SenderID;
	SenderID.RouterID=GetID();
	OBJECT_ID TargetID;
	TargetID.RouterID=0;
	TargetID.ObjectTypeID=BROAD_CAST_OBJECT_TYPE_ID;
	TargetID.GroupIndex=BROAD_CAST_GROUP_INDEX;
	TargetID.ObjectIndex=BROAD_CAST_OBJECT_INDEX;
	GetRouter()->RouterMessage(SenderID,TargetID,DSM_ROUTE_LINK_LOST,DOS_MESSAGE_FLAG_SYSTEM_MESSAGE,NULL,0);
	FUNCTION_END;
}

void CDOSRouterLink::OnData(const BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	CDOSMessagePacket * pNewPacket = GetRouter()->GetServer()->NewMessagePacket(DataSize);
	if (pNewPacket)
	{
		pNewPacket->CloneFrom(pData, DataSize);
		GetRouter()->RouterMessage(pNewPacket);
		GetRouter()->GetServer()->ReleaseMessagePacket(pNewPacket);
	}
	else
		PrintDOSLog(_T("分配消息内存块失败！"));
	FUNCTION_END;
}

void CDOSRouterLink::OnMsgPacket(CDOSMessagePacket * pPacket)
{
	GetRouter()->RouterMessage(pPacket);
}
bool CDOSRouterLink::SendPacket(CDOSMessagePacket * pPacket)
{
#ifndef WIN32
	if (m_pManager->UseNoBuffConnection())
	{
		return ((CENLConnectionNoBuff *)m_pConnection)->SendPacket(pPacket);
	}
	else
#endif
	{
		return SendData(pPacket->GetPacketData(), pPacket->GetPacketLength());
	}
}

CENLBaseConnection * CDOSRouterLink::NewConnection(UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
#ifndef WIN32
	if(m_pManager->UseNoBuffConnection())
	{
		CENLConnectionNoBuff * pConnection = MONITORED_NEW(_T("CDOSRouterLink"), CENLConnectionNoBuff);
		if (pConnection->Init((CDOSRouterLinkManager *)m_pManager, this, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
		{
			return pConnection;
		}
	}
	else
#endif
	{
		CENLConnection * pConnection = MONITORED_NEW(_T("CDOSRouterLink"), CENLConnection);
		if (pConnection->Init(m_pManager, this, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
		{
			return pConnection;
		}
	}
	return NULL;
}

CENLBaseConnection * CDOSRouterLink::NewConnection(const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
#ifndef WIN32
	if (m_pManager->UseNoBuffConnection())
	{
		CENLConnectionNoBuff * pConnection = MONITORED_NEW(_T("CDOSRouterLink"), CENLConnectionNoBuff);
		if (pConnection->Init((CDOSRouterLinkManager *)m_pManager, this, ConnectionAddress, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
		{
			return pConnection;
		}
	}
	else
#endif
	{
		CENLConnection * pConnection = MONITORED_NEW(_T("CDOSRouterLink"), CENLConnection);
		if (pConnection->Init(m_pManager, this, ConnectionAddress, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
		{
			return pConnection;
		}
	}
	return NULL;
}