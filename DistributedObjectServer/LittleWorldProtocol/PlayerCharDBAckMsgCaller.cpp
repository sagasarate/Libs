#include "StdAfx.h"


CPlayerCharDBAckMsgCaller::CPlayerCharDBAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CPlayerCharDBAckMsgCaller::CPlayerCharDBAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CPlayerCharDBAckMsgCaller::~CPlayerCharDBAckMsgCaller(void)
{
}


int CPlayerCharDBAckMsgCaller::AccountLoginAck(int Result ,UINT64 AccountID ,CSmartStruct& AccountData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(AccountData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGIN,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgAccountLoginAck(MsgPacket
		, Result , AccountID , AccountData ))
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

int CPlayerCharDBAckMsgCaller::AccountLogoutAck(int Result ,UINT64 AccountID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGOUT,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgAccountLogoutAck(MsgPacket
		, Result , AccountID ))
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

int CPlayerCharDBAckMsgCaller::GetAccountCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_ACCOUNT_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgGetAccountCharAck(MsgPacket
		, Result , AccountID , CharData ))
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

int CPlayerCharDBAckMsgCaller::CreateCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_CREATE_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgCreateCharAck(MsgPacket
		, Result , AccountID , CharData ))
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

int CPlayerCharDBAckMsgCaller::DeleteCharAck(int Result ,UINT64 AccountID ,UINT64 CharID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_DELETE_CHAR,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgDeleteCharAck(MsgPacket
		, Result , AccountID , CharID ))
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

int CPlayerCharDBAckMsgCaller::GetCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_CHAR_DATA,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgGetCharDataAck(MsgPacket
		, Result , AccountID , CharID , CharData ))
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

int CPlayerCharDBAckMsgCaller::SaveCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(int))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_SAVE_CHAR_DATA,true));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgSaveCharDataAck(MsgPacket
		, Result , AccountID , CharID ))
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


bool CPlayerCharDBAckMsgCaller::PackMsgAccountLoginAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& AccountData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGINACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGINACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGINACK_MEMBER_ACCOUNTDATA,AccountData),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgAccountLogoutAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGOUTACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGOUTACK_MEMBER_ACCOUNTID,AccountID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgGetAccountCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETACCOUNTCHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETACCOUNTCHARACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETACCOUNTCHARACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgCreateCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHARACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHARACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgDeleteCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHARACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHARACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHARACK_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgGetCharDataAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATAACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATAACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATAACK_MEMBER_CHARID,CharID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATAACK_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBAckMsgCaller::PackMsgSaveCharDataAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATAACK_MEMBER_RESULT,Result),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATAACK_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATAACK_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}


