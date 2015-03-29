#include "StdAfx.h"


CMapAreaAckMsgCaller::CMapAreaAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CMapAreaAckMsgCaller::CMapAreaAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CMapAreaAckMsgCaller::~CMapAreaAckMsgCaller(void)
{
}


int CMapAreaAckMsgCaller::RegisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_REGISTER_MAP_OBJECT,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgRegisterMapObjectAck(MsgPacket
		, ObjectID , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::UnregisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UNREGISTER_MAP_OBJECT,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUnregisterMapObjectAck(MsgPacket
		, ObjectID , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::UpdateMoveStatusAck(UINT64 ObjectID ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UPDATE_MOVE_STATUS,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUpdateMoveStatusAck(MsgPacket
		, ObjectID , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::SendEventMsgPacketAck(UINT64 ObjectID ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_SEND_EVENT_MSG_PACKET,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgSendEventMsgPacketAck(MsgPacket
		, ObjectID , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(MoveStatus.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_NEW_MAP_OBJECT,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgNewMapObject(MsgPacket
		, ObjectID , MoveStatus ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::DeleteMapObject(UINT64 ObjectID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_DEL_MAP_OBJECT,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgDeleteMapObject(MsgPacket
		, ObjectID ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(BYTE))
		+CSmartStruct::GetStructMemberSize(MoveStatus.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_MOVE_STATUS_UPDATE,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgMoveStatusUpdate(MsgPacket
		, ObjectID , IsForce , MoveStatus ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::EventMsgPacket(CSmartStruct& EventMsg )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetStructMemberSize(EventMsg.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_EVENT_MSG_PACKET,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgEventMsgPacket(MsgPacket
		, EventMsg ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::HideOnMapAck(UINT64 ObjectID ,BYTE HideType ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(BYTE))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_HIDE_ON_MAP,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgHideOnMapAck(MsgPacket
		, ObjectID , HideType , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}

int CMapAreaAckMsgCaller::TeleportInMapAck(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir ,int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_TELEPORT_IN_MAP,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgTeleportInMapAck(MsgPacket
		, ObjectID , TargetPosX , TargetPosY , TargetHeight , TargetDir , Result ))
	{	
		pMsgPacket->SetTargetIDs(m_TargetIDs.GetCount(),m_TargetIDs.GetBuffer());
		if(m_pOperator->SendMessagePacket(pMsgPacket))
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_SUCCEED;
		}
		else
		{
			m_pOperator->ReleaseMessagePacket(pMsgPacket);
			return COMMON_RESULT_MSG_SEND_ERROR;
		}
	}
	else
	{
		m_pOperator->ReleaseMessagePacket(pMsgPacket);
		return COMMON_RESULT_MSG_PACK_ERROR;
	}
	
}


bool CMapAreaAckMsgCaller::PackMsgRegisterMapObjectAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECTACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECTACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgUnregisterMapObjectAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UNREGISTERMAPOBJECTACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UNREGISTERMAPOBJECTACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgUpdateMoveStatusAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UPDATEMOVESTATUSACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UPDATEMOVESTATUSACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgSendEventMsgPacketAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SENDEVENTMSGPACKETACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SENDEVENTMSGPACKETACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgNewMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(NEWMAPOBJECT_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(NEWMAPOBJECT_MEMBER_MOVESTATUS,MoveStatus),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgDeleteMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETEMAPOBJECT_MEMBER_OBJECTID,ObjectID),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgMoveStatusUpdate(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(MOVESTATUSUPDATE_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(MOVESTATUSUPDATE_MEMBER_ISFORCE,IsForce),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(MOVESTATUSUPDATE_MEMBER_MOVESTATUS,MoveStatus),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgEventMsgPacket(CSmartStruct& MsgPacket,CSmartStruct& EventMsg )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(EVENTMSGPACKET_MEMBER_EVENTMSG,EventMsg),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgHideOnMapAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE HideType ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(HIDEONMAPACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(HIDEONMAPACK_MEMBER_HIDETYPE,HideType),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(HIDEONMAPACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CMapAreaAckMsgCaller::PackMsgTeleportInMapAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir ,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_TARGETPOSX,TargetPosX),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_TARGETPOSY,TargetPosY),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_TARGETHEIGHT,TargetHeight),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_TARGETDIR,TargetDir),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAPACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}


