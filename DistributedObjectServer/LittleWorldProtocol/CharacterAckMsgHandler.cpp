#include "stdafx.h"

CCharacterAckMsgHandler::CCharacterAckMsgHandler(void)
{
}

CCharacterAckMsgHandler::~CCharacterAckMsgHandler(void)
{
}

void CCharacterAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgQueryCharInfoAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_CHAR_INFO,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgQueryMoveAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_MOVE,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgAliveTestAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_ALIVE_TEST,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgQueryFlyAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_FLY,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgMapChange;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_MAP_CHANGE,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterAckMsgHandler::HandleMsgQueryJumpAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_JUMP,true),MsgHandleInfo);

}

int CCharacterAckMsgHandler::HandleMsgQueryCharInfoAck(CSmartStruct& MsgPacket)
{
	int Result ;
	CSmartStruct CharData ;


	Result = MsgPacket.GetMember(QUERYCHARINFOACK_MEMBER_RESULT);
	CharData = MsgPacket.GetMember(QUERYCHARINFOACK_MEMBER_CHARDATA);
	

	return QueryCharInfoAck( Result , CharData );
}
int CCharacterAckMsgHandler::HandleMsgQueryMoveAck(CSmartStruct& MsgPacket)
{
	int Result ;


	Result = MsgPacket.GetMember(QUERYMOVEACK_MEMBER_RESULT);
	

	return QueryMoveAck( Result );
}
int CCharacterAckMsgHandler::HandleMsgAliveTestAck(CSmartStruct& MsgPacket)
{


	

	return AliveTestAck();
}
int CCharacterAckMsgHandler::HandleMsgQueryFlyAck(CSmartStruct& MsgPacket)
{
	int Result ;


	Result = MsgPacket.GetMember(QUERYFLYACK_MEMBER_RESULT);
	

	return QueryFlyAck( Result );
}
int CCharacterAckMsgHandler::HandleMsgMapChange(CSmartStruct& MsgPacket)
{
	UINT MapID ;
	float PosX ;
	float PosY ;
	float Direction ;


	MapID = MsgPacket.GetMember(MAPCHANGE_MEMBER_MAPID);
	PosX = MsgPacket.GetMember(MAPCHANGE_MEMBER_POSX);
	PosY = MsgPacket.GetMember(MAPCHANGE_MEMBER_POSY);
	Direction = MsgPacket.GetMember(MAPCHANGE_MEMBER_DIRECTION);
	

	return MapChange( MapID , PosX , PosY , Direction );
}
int CCharacterAckMsgHandler::HandleMsgQueryJumpAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	int Result ;


	ObjectID = MsgPacket.GetMember(QUERYJUMPACK_MEMBER_OBJECTID);
	Result = MsgPacket.GetMember(QUERYJUMPACK_MEMBER_RESULT);
	

	return QueryJumpAck( ObjectID , Result );
}
