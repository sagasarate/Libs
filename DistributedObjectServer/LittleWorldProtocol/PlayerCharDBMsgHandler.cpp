#include "stdafx.h"

CPlayerCharDBMsgHandler::CPlayerCharDBMsgHandler(void)
{
}

CPlayerCharDBMsgHandler::~CPlayerCharDBMsgHandler(void)
{
}

void CPlayerCharDBMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgAccountLogin;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGIN,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgAccountLogout;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_ACCOUNT_LOGOUT,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgGetAccountChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_ACCOUNT_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgCreateChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_CREATE_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgDeleteChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_DELETE_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgGetCharData;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_GET_CHAR_DATA,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharDBMsgHandler::HandleMsgSaveCharData;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_DB,METHOD_PCD_SAVE_CHAR_DATA,false),MsgHandleInfo);

}

int CPlayerCharDBMsgHandler::HandleMsgAccountLogin(CSmartStruct& MsgPacket)
{
	LPCTSTR szAccountName ;
	LPCTSTR szPassword ;
	LPCTSTR szClientIP ;


	szAccountName = MsgPacket.GetMember(ACCOUNTLOGIN_MEMBER_SZACCOUNTNAME);
	szPassword = MsgPacket.GetMember(ACCOUNTLOGIN_MEMBER_SZPASSWORD);
	szClientIP = MsgPacket.GetMember(ACCOUNTLOGIN_MEMBER_SZCLIENTIP);
	

	return AccountLogin( szAccountName , szPassword , szClientIP );
}
int CPlayerCharDBMsgHandler::HandleMsgAccountLogout(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;


	AccountID = MsgPacket.GetMember(ACCOUNTLOGOUT_MEMBER_ACCOUNTID);
	

	return AccountLogout( AccountID );
}
int CPlayerCharDBMsgHandler::HandleMsgGetAccountChar(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;


	AccountID = MsgPacket.GetMember(GETACCOUNTCHAR_MEMBER_ACCOUNTID);
	

	return GetAccountChar( AccountID );
}
int CPlayerCharDBMsgHandler::HandleMsgCreateChar(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;
	CSmartStruct CharData ;


	AccountID = MsgPacket.GetMember(CREATECHAR_MEMBER_ACCOUNTID);
	CharData = MsgPacket.GetMember(CREATECHAR_MEMBER_CHARDATA);
	

	return CreateChar( AccountID , CharData );
}
int CPlayerCharDBMsgHandler::HandleMsgDeleteChar(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;
	UINT64 CharID ;


	AccountID = MsgPacket.GetMember(DELETECHAR_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(DELETECHAR_MEMBER_CHARID);
	

	return DeleteChar( AccountID , CharID );
}
int CPlayerCharDBMsgHandler::HandleMsgGetCharData(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;
	UINT64 CharID ;


	AccountID = MsgPacket.GetMember(GETCHARDATA_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(GETCHARDATA_MEMBER_CHARID);
	

	return GetCharData( AccountID , CharID );
}
int CPlayerCharDBMsgHandler::HandleMsgSaveCharData(CSmartStruct& MsgPacket)
{
	UINT64 AccountID ;
	UINT64 CharID ;
	CSmartStruct CharData ;


	AccountID = MsgPacket.GetMember(SAVECHARDATA_MEMBER_ACCOUNTID);
	CharID = MsgPacket.GetMember(SAVECHARDATA_MEMBER_CHARID);
	CharData = MsgPacket.GetMember(SAVECHARDATA_MEMBER_CHARDATA);
	

	return SaveCharData( AccountID , CharID , CharData );
}
