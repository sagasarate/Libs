#include "StdAfx.h"


CCharacterMsgCaller::CCharacterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CCharacterMsgCaller::CCharacterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CCharacterMsgCaller::~CCharacterMsgCaller(void)
{
}


int CCharacterMsgCaller::QueryCharInfo(OBJECT_ID RequesterID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(OBJECT_ID));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_CHAR_INFO,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgQueryCharInfo(MsgPacket
		, RequesterID ))
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

int CCharacterMsgCaller::QueryMove(BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(BYTE))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(BYTE))
		+CSmartStruct::GetFixMemberSize(sizeof(float));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_MOVE,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgQueryMove(MsgPacket
		, MoveStatus , StartPosX , StartPosY , StartHeight , TargetPosX , TargetPosY , TargetHeight , RotateStatus , Direction ))
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

int CCharacterMsgCaller::AliveTest()
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize();
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_ALIVE_TEST,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgAliveTest(MsgPacket))
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

int CCharacterMsgCaller::QueryFly(UINT MapID ,float PosX ,float PosY ,float Height ,float Direction )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float))
		+CSmartStruct::GetFixMemberSize(sizeof(float));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_FLY,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgQueryFly(MsgPacket
		, MapID , PosX , PosY , Height , Direction ))
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

int CCharacterMsgCaller::MapChangeConfirm(int Result )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_MAP_CHANGE,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgMapChangeConfirm(MsgPacket
		, Result ))
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

int CCharacterMsgCaller::QueryJump()
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize();
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_JUMP,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgQueryJump(MsgPacket))
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


bool CCharacterMsgCaller::PackMsgQueryCharInfo(CSmartStruct& MsgPacket,OBJECT_ID RequesterID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYCHARINFO_MEMBER_REQUESTERID,RequesterID.ID),FailCount);


	return FailCount==0;
}

bool CCharacterMsgCaller::PackMsgQueryMove(CSmartStruct& MsgPacket,BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_MOVESTATUS,MoveStatus),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_STARTPOSX,StartPosX),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_STARTPOSY,StartPosY),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_STARTHEIGHT,StartHeight),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_TARGETPOSX,TargetPosX),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_TARGETPOSY,TargetPosY),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_TARGETHEIGHT,TargetHeight),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_ROTATESTATUS,RotateStatus),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYMOVE_MEMBER_DIRECTION,Direction),FailCount);


	return FailCount==0;
}

bool CCharacterMsgCaller::PackMsgAliveTest(CSmartStruct& MsgPacket)
{
	UINT FailCount=0;



	return FailCount==0;
}

bool CCharacterMsgCaller::PackMsgQueryFly(CSmartStruct& MsgPacket,UINT MapID ,float PosX ,float PosY ,float Height ,float Direction )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYFLY_MEMBER_MAPID,MapID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYFLY_MEMBER_POSX,PosX),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYFLY_MEMBER_POSY,PosY),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYFLY_MEMBER_HEIGHT,Height),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYFLY_MEMBER_DIRECTION,Direction),FailCount);


	return FailCount==0;
}

bool CCharacterMsgCaller::PackMsgMapChangeConfirm(CSmartStruct& MsgPacket,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(MAPCHANGECONFIRM_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CCharacterMsgCaller::PackMsgQueryJump(CSmartStruct& MsgPacket)
{
	UINT FailCount=0;



	return FailCount==0;
}


