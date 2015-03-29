#include "StdAfx.h"


CMapAreaMsgCaller::CMapAreaMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CMapAreaMsgCaller::CMapAreaMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CMapAreaMsgCaller::~CMapAreaMsgCaller(void)
{
}


int CMapAreaMsgCaller::RegisterMapObject(UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(OBJECT_ID))
		+CSmartStruct::GetFixMemberSize(sizeof(OBJECT_ID))
		+CSmartStruct::GetStructMemberSize(MoveStatus.GetDataLen())
		+CSmartStruct::GetFixMemberSize(sizeof(UINT));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_REGISTER_MAP_OBJECT,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgRegisterMapObject(MsgPacket
		, ObjectID , ControllerID , AreaEventReceiverID , MoveStatus , Flag ))
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

int CMapAreaMsgCaller::UnregisterMapObject(UINT64 ObjectID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UNREGISTER_MAP_OBJECT,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUnregisterMapObject(MsgPacket
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

int CMapAreaMsgCaller::UpdateMoveStatus(UINT64 ObjectID ,CSmartStruct& MoveStatus )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UPDATE_MOVE_STATUS,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUpdateMoveStatus(MsgPacket
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

int CMapAreaMsgCaller::SendEventMsgPacket(UINT64 ObjectID ,CSmartStruct& EventMsgPacket )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(EventMsgPacket.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_SEND_EVENT_MSG_PACKET,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgSendEventMsgPacket(MsgPacket
		, ObjectID , EventMsgPacket ))
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

int CMapAreaMsgCaller::HideOnMap(UINT64 ObjectID ,BYTE HideType )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(BYTE));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_HIDE_ON_MAP,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgHideOnMap(MsgPacket
		, ObjectID , HideType ))
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

int CMapAreaMsgCaller::TeleportInMap(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir )
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
		+CSmartStruct::GetFixMemberSize(sizeof(float));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_TELEPORT_IN_MAP,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgTeleportInMap(MsgPacket
		, ObjectID , TargetPosX , TargetPosY , TargetHeight , TargetDir ))
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


bool CMapAreaMsgCaller::PackMsgRegisterMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECT_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECT_MEMBER_CONTROLLERID,ControllerID.ID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECT_MEMBER_AREAEVENTRECEIVERID,AreaEventReceiverID.ID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECT_MEMBER_MOVESTATUS,MoveStatus),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERMAPOBJECT_MEMBER_FLAG,Flag),FailCount);


	return FailCount==0;
}

bool CMapAreaMsgCaller::PackMsgUnregisterMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UNREGISTERMAPOBJECT_MEMBER_OBJECTID,ObjectID),FailCount);


	return FailCount==0;
}

bool CMapAreaMsgCaller::PackMsgUpdateMoveStatus(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& MoveStatus )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UPDATEMOVESTATUS_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UPDATEMOVESTATUS_MEMBER_MOVESTATUS,MoveStatus),FailCount);


	return FailCount==0;
}

bool CMapAreaMsgCaller::PackMsgSendEventMsgPacket(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& EventMsgPacket )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SENDEVENTMSGPACKET_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SENDEVENTMSGPACKET_MEMBER_EVENTMSGPACKET,EventMsgPacket),FailCount);


	return FailCount==0;
}

bool CMapAreaMsgCaller::PackMsgHideOnMap(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE HideType )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(HIDEONMAP_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(HIDEONMAP_MEMBER_HIDETYPE,HideType),FailCount);


	return FailCount==0;
}

bool CMapAreaMsgCaller::PackMsgTeleportInMap(CSmartStruct& MsgPacket,UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAP_MEMBER_OBJECTID,ObjectID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAP_MEMBER_TARGETPOSX,TargetPosX),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAP_MEMBER_TARGETPOSY,TargetPosY),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAP_MEMBER_TARGETHEIGHT,TargetHeight),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(TELEPORTINMAP_MEMBER_TARGETDIR,TargetDir),FailCount);


	return FailCount==0;
}


