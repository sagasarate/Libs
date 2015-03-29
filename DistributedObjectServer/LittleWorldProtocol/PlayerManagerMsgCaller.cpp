#include "StdAfx.h"


CPlayerManagerMsgCaller::CPlayerManagerMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CPlayerManagerMsgCaller::CPlayerManagerMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CPlayerManagerMsgCaller::~CPlayerManagerMsgCaller(void)
{
}


int CPlayerManagerMsgCaller::PlayerLogin(LPCTSTR szAccountName ,LPCTSTR szPassword )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+(szAccountName!=NULL?CSmartStruct::GetStringMemberSize(strlen(szAccountName)):0)
		+(szPassword!=NULL?CSmartStruct::GetStringMemberSize(strlen(szPassword)):0);
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_MANAGER,METHOD_PM_PLAYER_REGISTER,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgPlayerLogin(MsgPacket
		, szAccountName , szPassword ))
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


bool CPlayerManagerMsgCaller::PackMsgPlayerLogin(CSmartStruct& MsgPacket,LPCTSTR szAccountName ,LPCTSTR szPassword )
{
	UINT FailCount=0;

	if( szAccountName != NULL)
	{
		CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(PLAYERLOGIN_MEMBER_SZACCOUNTNAME,szAccountName),FailCount);
	}

	if( szPassword != NULL)
	{
		CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(PLAYERLOGIN_MEMBER_SZPASSWORD,szPassword),FailCount);
	}



	return FailCount==0;
}


