#include "StdAfx.h"


CPlayerCharDBMsgCaller::CPlayerCharDBMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Add(TargetID);
}

CPlayerCharDBMsgCaller::CPlayerCharDBMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount)
{
	m_pOperator=pOperator;
	m_SenderID=SenderID;
	m_TargetIDs.Resize(TargetIDCount);
	for(UINT i=0;i<TargetIDCount;i++)
	{
		m_TargetIDs[i]=pTargetIDs[i];
	}	
}

CPlayerCharDBMsgCaller::~CPlayerCharDBMsgCaller(void)
{
}


int CPlayerCharDBMsgCaller::AccountLogin(LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+(szAccountName!=NULL?CSmartStruct::GetStringMemberSize(strlen(szAccountName)):0)
		+(szPassword!=NULL?CSmartStruct::GetStringMemberSize(strlen(szPassword)):0)
		+(szClientIP!=NULL?CSmartStruct::GetStringMemberSize(strlen(szClientIP)):0);
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGIN,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgAccountLogin(MsgPacket
		, szAccountName , szPassword , szClientIP ))
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

int CPlayerCharDBMsgCaller::AccountLogout(UINT64 AccountID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGOUT,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgAccountLogout(MsgPacket
		, AccountID ))
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

int CPlayerCharDBMsgCaller::GetAccountChar(UINT64 AccountID )
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

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_ACCOUNT_CHAR,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgGetAccountChar(MsgPacket
		, AccountID ))
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

int CPlayerCharDBMsgCaller::CreateChar(UINT64 AccountID ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_CREATE_CHAR,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgCreateChar(MsgPacket
		, AccountID , CharData ))
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

int CPlayerCharDBMsgCaller::DeleteChar(UINT64 AccountID ,UINT64 CharID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_DELETE_CHAR,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgDeleteChar(MsgPacket
		, AccountID , CharID ))
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

int CPlayerCharDBMsgCaller::GetCharData(UINT64 AccountID ,UINT64 CharID )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64));
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_CHAR_DATA,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgGetCharData(MsgPacket
		, AccountID , CharID ))
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

int CPlayerCharDBMsgCaller::SaveCharData(UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	if(m_pOperator==NULL)
	{
		return COMMON_RESULT_FAILED;
	}

	UINT MsgDataSize=CSmartStruct::GetEmptyStructSize()
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetFixMemberSize(sizeof(UINT64))
		+CSmartStruct::GetStructMemberSize(CharData.GetDataLen());
		
	CDOSMessagePacket * pMsgPacket=m_pOperator->NewMessagePacket(MsgDataSize,m_TargetIDs.GetCount());
	if(pMsgPacket==NULL)
	{
		return COMMON_RESULT_MSG_ALLOC_ERROR;
	}

	pMsgPacket->GetMessage().SetMsgID(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_SAVE_CHAR_DATA,false));
	pMsgPacket->GetMessage().SetSenderID(m_SenderID);
	pMsgPacket->GetMessage().SetDataLength(MsgDataSize);

	UINT FailCount=0;
	
	CSmartStruct MsgPacket=pMsgPacket->GetMessage().GetEmptyDataPacket();
	
	if(PackMsgSaveCharData(MsgPacket
		, AccountID , CharID , CharData ))
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


bool CPlayerCharDBMsgCaller::PackMsgAccountLogin(CSmartStruct& MsgPacket,LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP )
{
	UINT FailCount=0;

	if( szAccountName != NULL)
	{
		CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGIN_MEMBER_SZACCOUNTNAME,szAccountName),FailCount);
	}

	if( szPassword != NULL)
	{
		CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGIN_MEMBER_SZPASSWORD,szPassword),FailCount);
	}

	if( szClientIP != NULL)
	{
		CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGIN_MEMBER_SZCLIENTIP,szClientIP),FailCount);
	}



	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgAccountLogout(CSmartStruct& MsgPacket,UINT64 AccountID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(ACCOUNTLOGOUT_MEMBER_ACCOUNTID,AccountID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgGetAccountChar(CSmartStruct& MsgPacket,UINT64 AccountID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETACCOUNTCHAR_MEMBER_ACCOUNTID,AccountID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgCreateChar(CSmartStruct& MsgPacket,UINT64 AccountID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHAR_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(CREATECHAR_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgDeleteChar(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHAR_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(DELETECHAR_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgGetCharData(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATA_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(GETCHARDATA_MEMBER_CHARID,CharID),FailCount);


	return FailCount==0;
}

bool CPlayerCharDBMsgCaller::PackMsgSaveCharData(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData )
{
	UINT FailCount=0;

	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATA_MEMBER_ACCOUNTID,AccountID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATA_MEMBER_CHARID,CharID),FailCount);
	CHECK_SMART_STRUCT_ADD(MsgPacket.AddMember(SAVECHARDATA_MEMBER_CHARDATA,CharData),FailCount);


	return FailCount==0;
}


