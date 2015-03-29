#include "stdafx.h"

CPlayerManagerAckMsgHandler::CPlayerManagerAckMsgHandler(void)
{
}

CPlayerManagerAckMsgHandler::~CPlayerManagerAckMsgHandler(void)
{
}

void CPlayerManagerAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerManagerAckMsgHandler::HandleMsgPlayerLoginAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_MANAGER,METHOD_PM_PLAYER_REGISTER,true),MsgHandleInfo);

}

int CPlayerManagerAckMsgHandler::HandleMsgPlayerLoginAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 AccountID ;


	Result = MsgPacket.GetMember(PLAYERLOGINACK_MEMBER_RESULT);
	AccountID = MsgPacket.GetMember(PLAYERLOGINACK_MEMBER_ACCOUNTID);
	

	return PlayerLoginAck( Result , AccountID );
}
