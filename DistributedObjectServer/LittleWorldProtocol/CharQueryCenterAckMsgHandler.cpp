#include "stdafx.h"

CCharQueryCenterAckMsgHandler::CCharQueryCenterAckMsgHandler(void)
{
}

CCharQueryCenterAckMsgHandler::~CCharQueryCenterAckMsgHandler(void)
{
}

void CCharQueryCenterAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharQueryCenterAckMsgHandler::HandleMsgRegisterCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_REGISTER_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharQueryCenterAckMsgHandler::HandleMsgUnregisterCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_UNREGISTER_CHAR,true),MsgHandleInfo);

}

int CCharQueryCenterAckMsgHandler::HandleMsgRegisterCharAck(CSmartStruct& MsgPacket)
{
	int Result ;


	Result = MsgPacket.GetMember(REGISTERCHARACK_MEMBER_RESULT);
	

	return RegisterCharAck( Result );
}
int CCharQueryCenterAckMsgHandler::HandleMsgUnregisterCharAck(CSmartStruct& MsgPacket)
{
	int Result ;


	Result = MsgPacket.GetMember(UNREGISTERCHARACK_MEMBER_RESULT);
	

	return UnregisterCharAck( Result );
}
