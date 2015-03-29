#include "StdAfx.h"


CPlayerCharOperatorAckMsgCaller::CPlayerCharOperatorAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CPlayerCharOperatorAckMsgCaller::CPlayerCharOperatorAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CPlayerCharOperatorAckMsgCaller::~CPlayerCharOperatorAckMsgCaller(void)
{
}


int CPlayerCharOperatorAckMsgCaller::GetCharListAck(int Result ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_GET_CHAR_LIST,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgGetCharListAck(MsgPacket
		, Result , CharData ))
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

int CPlayerCharOperatorAckMsgCaller::CreateCharAck(int Result ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_CREATE_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgCreateCharAck(MsgPacket
		, Result , CharData ))
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

int CPlayerCharOperatorAckMsgCaller::DeleteCharAck(int Result ,UINT64 CharID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_DELETE_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgDeleteCharAck(MsgPacket
		, Result , CharID ))
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

int CPlayerCharOperatorAckMsgCaller::SelectCharAck(int Result ,UINT64 CharID ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_SELECT_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgSelectCharAck(MsgPacket
		, Result , CharID , CharData ))
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


bool CPlayerCharOperatorAckMsgCaller::PackMsgGetCharListAck(CSmartStruct& MsgPacket,int Result ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARLISTACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARLISTACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharOperatorAckMsgCaller::PackMsgCreateCharAck(CSmartStruct& MsgPacket,int Result ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHARACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharOperatorAckMsgCaller::PackMsgDeleteCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHARACK_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CPlayerCharOperatorAckMsgCaller::PackMsgSelectCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 CharID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SELECTCHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SELECTCHARACK_MEMBER_CHARID,CharID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SELECTCHARACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}


