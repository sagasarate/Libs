#include "stdafx.h"

CPlayerCharOperatorAckMsgHandler::CPlayerCharOperatorAckMsgHandler(void)
{
}

CPlayerCharOperatorAckMsgHandler::~CPlayerCharOperatorAckMsgHandler(void)
{
}

void CPlayerCharOperatorAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorAckMsgHandler::HandleMsgGetCharListAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_GET_CHAR_LIST,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorAckMsgHandler::HandleMsgCreateCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_CREATE_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorAckMsgHandler::HandleMsgDeleteCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_DELETE_CHAR,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorAckMsgHandler::HandleMsgSelectCharAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_SELECT_CHAR,true),MsgHandleInfo);

}

int CPlayerCharOperatorAckMsgHandler::HandleMsgGetCharListAck(CSmartStruct& MsgPacket)
{
	int Result ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(GETCHARLISTACK_MEMBER_RESULT);
	CharData = MsgPacket.GetMember(GETCHARLISTACK_MEMBER_CHARDATA);
	

	return GetCharListAck( Result , CharData );
}
int CPlayerCharOperatorAckMsgHandler::HandleMsgCreateCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(CREATECHARACK_MEMBER_RESULT);
	CharData = MsgPacket.GetMember(CREATECHARACK_MEMBER_CHARDATA);
	

	return CreateCharAck( Result , CharData );
}
int CPlayerCharOperatorAckMsgHandler::HandleMsgDeleteCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 CharID ;


	Result = MsgPacket.GetMember(DELETECHARACK_MEMBER_RESULT);
	CharID = MsgPacket.GetMember(DELETECHARACK_MEMBER_CHARID);
	

	return DeleteCharAck( Result , CharID );
}
int CPlayerCharOperatorAckMsgHandler::HandleMsgSelectCharAck(CSmartStruct& MsgPacket)
{
	int Result ;
	UINT64 CharID ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(SELECTCHARACK_MEMBER_RESULT);
	CharID = MsgPacket.GetMember(SELECTCHARACK_MEMBER_CHARID);
	CharData = MsgPacket.GetMember(SELECTCHARACK_MEMBER_CHARDATA);
	

	return SelectCharAck( Result , CharID , CharData );
}
