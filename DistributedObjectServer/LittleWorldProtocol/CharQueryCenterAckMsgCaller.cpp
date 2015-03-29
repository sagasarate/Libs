#include "StdAfx.h"


CCharQueryCenterAckMsgCaller::CCharQueryCenterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CCharQueryCenterAckMsgCaller::CCharQueryCenterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CCharQueryCenterAckMsgCaller::~CCharQueryCenterAckMsgCaller(void)
{
}


int CCharQueryCenterAckMsgCaller::RegisterCharAck(int Result )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_REGISTER_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgRegisterCharAck(MsgPacket
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

int CCharQueryCenterAckMsgCaller::UnregisterCharAck(int Result )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_UNREGISTER_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUnregisterCharAck(MsgPacket
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


bool CCharQueryCenterAckMsgCaller::PackMsgRegisterCharAck(CSmartStruct& MsgPacket,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERCHARACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}

bool CCharQueryCenterAckMsgCaller::PackMsgUnregisterCharAck(CSmartStruct& MsgPacket,int Result )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UNREGISTERCHARACK_MEMBER_RESULT,Result),FailCount);


	return FailCount==0;
}


