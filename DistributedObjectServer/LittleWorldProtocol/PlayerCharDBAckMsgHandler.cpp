#include "stdafx.h"

CPlayerCharDBAckMsgHandler::CPlayerCharDBAckMsgHandler(void)
{
}

CPlayerCharDBAckMsgHandler::~CPlayerCharDBAckMsgHandler(void)
{
}

void CPlayerCharDBAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgAccountLoginAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGIN,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgAccountLogoutAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGOUT,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgGetAccountCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_ACCOUNT_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgCreateCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_CREATE_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgDeleteCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_DELETE_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgGetCharDataAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_CHAR_DATA,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBAckMsgHandler::HandleMsgSaveCharDataAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_SAVE_CHAR_DATA,true),MsgHandleInfo);

}

int CPlayerCharDBAckMsgHandler::HandleMsgAccountLoginAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	CSmartStruct AccountData ;


	Result = MsgPacket.GetMember(ACCOUNTLOGINACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(ACCOUNTLOGINACK_MEMBER_ACCOUNTID);
	AccountData = MsgPacket.GetMember(ACCOUNTLOGINACK_MEMBER_ACCOUNTDATA);
	

	return AccountLoginAck( Result , AccountID , AccountData );
}
int CPlayerCharDBAckMsgHandler::HandleMsgAccountLogoutAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;


	Result = MsgPacket.GetMember(ACCOUNTLOGOUTACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(ACCOUNTLOGOUTACK_MEMBER_ACCOUNTID);
	

	return AccountLogoutAck( Result , AccountID );
}
int CPlayerCharDBAckMsgHandler::HandleMsgGetAccountCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(GETACCOUNTCHARACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(GETACCOUNTCHARACK_MEMBER_ACCOUNTID);
	CharData = MsgPacket.GetMember(GETACCOUNTCHARACK_MEMBER_CHARDATA);
	

	return GetAccountCharAck( Result , AccountID , CharData );
}
int CPlayerCharDBAckMsgHandler::HandleMsgCreateCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(CREATECHARACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(CREATECHARACK_MEMBER_ACCOUNTID);
	CharData = MsgPacket.GetMember(CREATECHARACK_MEMBER_CHARDATA);
	

	return CreateCharAck( Result , AccountID , CharData );
}
int CPlayerCharDBAckMsgHandler::HandleMsgDeleteCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	UINT64 CharID ;


	Result = MsgPacket.GetMember(DELETECHARACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(DELETECHARACK_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(DELETECHARACK_MEMBER_CHARID);
	

	return DeleteCharAck( Result , AccountID , CharID );
}
int CPlayerCharDBAckMsgHandler::HandleMsgGetCharDataAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	UINT64 CharID ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(GETCHARDATAACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(GETCHARDATAACK_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(GETCHARDATAACK_MEMBER_CHARID);
	CharData = MsgPacket.GetMember(GETCHARDATAACK_MEMBER_CHARDATA);
	

	return GetCharDataAck( Result , AccountID , CharID , CharData );
}
int CPlayerCharDBAckMsgHandler::HandleMsgSaveCharDataAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;
	UINT64 CharID ;


	Result = MsgPacket.GetMember(SAVECHARDATAACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(SAVECHARDATAACK_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(SAVECHARDATAACK_MEMBER_CHARID);
	

	return SaveCharDataAck( Result , AccountID , CharID );
}
