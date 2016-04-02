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

IMPLEMENT_CLASS_INFO_STATIC(CDOSRouterLink, CEasyNetLinkConnection);

CDOSRouterLink::CDOSRouterLink()
{

}

CDOSRouterLink::~CDOSRouterLink(void)
{
}

void CDOSRouterLink::OnLinkStart()
{
	FUNCTION_BEGIN;
	PrintDOSLog(0xff0000,_T("收到路由(%d)的连接"),GetID());
	FUNCTION_END;
}
void CDOSRouterLink::OnLinkEnd()
{
	FUNCTION_BEGIN;
	PrintDOSLog(0xff0000,_T("路由(%d)的连接断开！"),GetID());
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

void CDOSRouterLink::OnData(const CEasyBuffer& DataBuffer)
{
	FUNCTION_BEGIN;
	MSG_LEN_TYPE PacketLen=*((MSG_LEN_TYPE *)DataBuffer.GetBuffer());
	CDOSMessagePacket * pNewPacket=GetRouter()->GetServer()->NewMessagePacket(PacketLen);
	if(pNewPacket)
	{
		memcpy(pNewPacket->GetPacketBuffer(),DataBuffer.GetBuffer(),PacketLen);
		GetRouter()->RouterMessage(pNewPacket);
		GetRouter()->GetServer()->ReleaseMessagePacket(pNewPacket);
	}
	else
		PrintDOSLog(0xff0000,_T("分配消息内存块失败！"));
	FUNCTION_END;
}
