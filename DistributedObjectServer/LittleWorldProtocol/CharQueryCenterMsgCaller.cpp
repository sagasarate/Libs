#include "StdAfx.h"


CCharQueryCenterMsgCaller::CCharQueryCenterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CCharQueryCenterMsgCaller::CCharQueryCenterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CCharQueryCenterMsgCaller::~CCharQueryCenterMsgCaller(void)
{
}


int CCharQueryCenterMsgCaller::RegisterChar(UINT64 CharID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_REGISTER_CHAR,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgRegisterChar(MsgPacket
		, CharID ))
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

int CCharQueryCenterMsgCaller::UnregisterChar(UINT64 CharID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_UNREGISTER_CHAR,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgUnregisterChar(MsgPacket
		, CharID ))
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

int CCharQueryCenterMsgCaller::QueryCharInfo(UINT64 CharID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_QUERY_CHAR_INFO,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgQueryCharInfo(MsgPacket
		, CharID ))
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


bool CCharQueryCenterMsgCaller::PackMsgRegisterChar(CSmartStruct& MsgPacket,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(REGISTERCHAR_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CCharQueryCenterMsgCaller::PackMsgUnregisterChar(CSmartStruct& MsgPacket,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(UNREGISTERCHAR_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CCharQueryCenterMsgCaller::PackMsgQueryCharInfo(CSmartStruct& MsgPacket,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(QUERYCHARINFO_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}


