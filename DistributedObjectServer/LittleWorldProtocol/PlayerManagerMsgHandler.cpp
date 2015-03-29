#include "stdafx.h"

CPlayerManagerMsgHandler::CPlayerManagerMsgHandler(void)
{
}

CPlayerManagerMsgHandler::~CPlayerManagerMsgHandler(void)
{
}

void CPlayerManagerMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerManagerMsgHandler::HandleMsgPlayerLogin;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_MANAGER,METHOD_PM_PLAYER_REGISTER,false),MsgHandleInfo);

}

int CPlayerManagerMsgHandler::HandleMsgPlayerLogin(CSmartStruct& MsgPacket)
{
	LPCTSTR szAccountName ;
	LPCTSTR szPassword ;


	szAccountName = MsgPacket.GetMember(PLAYERLOGIN_MEMBER_SZACCOUNTNAME);
	szPassword = MsgPacket.GetMember(PLAYERLOGIN_MEMBER_SZPASSWORD);
	

	return PlayerLogin( szAccountName , szPassword );
}
