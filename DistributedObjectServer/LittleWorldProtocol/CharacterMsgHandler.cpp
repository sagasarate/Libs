#include "stdafx.h"

CCharacterMsgHandler::CCharacterMsgHandler(void)
{
}

CCharacterMsgHandler::~CCharacterMsgHandler(void)
{
}

void CCharacterMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgQueryCharInfo;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_CHAR_INFO,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgQueryMove;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_MOVE,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgAliveTest;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_ALIVE_TEST,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgQueryFly;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_FLY,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgMapChangeConfirm;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_MAP_CHANGE,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharacterMsgHandler::HandleMsgQueryJump;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_JUMP,false),MsgHandleInfo);

}

int CCharacterMsgHandler::HandleMsgQueryCharInfo(CSmartStruct& MsgPacket)
{
	OBJECT_ID RequesterID ;


	RequesterID.ID = MsgPacket.GetMember(QUERYCHARINFO_MEMBER_REQUESTERID);
	

	return QueryCharInfo( RequesterID );
}
int CCharacterMsgHandler::HandleMsgQueryMove(CSmartStruct& MsgPacket)
{
	BYTE MoveStatus ;
	float StartPosX ;
	float StartPosY ;
	float StartHeight ;
	float TargetPosX ;
	float TargetPosY ;
	float TargetHeight ;
	BYTE RotateStatus ;
	float Direction ;


	MoveStatus = MsgPacket.GetMember(QUERYMOVE_MEMBER_MOVESTATUS);
	StartPosX = MsgPacket.GetMember(QUERYMOVE_MEMBER_STARTPOSX);
	StartPosY = MsgPacket.GetMember(QUERYMOVE_MEMBER_STARTPOSY);
	StartHeight = MsgPacket.GetMember(QUERYMOVE_MEMBER_STARTHEIGHT);
	TargetPosX = MsgPacket.GetMember(QUERYMOVE_MEMBER_TARGETPOSX);
	TargetPosY = MsgPacket.GetMember(QUERYMOVE_MEMBER_TARGETPOSY);
	TargetHeight = MsgPacket.GetMember(QUERYMOVE_MEMBER_TARGETHEIGHT);
	RotateStatus = MsgPacket.GetMember(QUERYMOVE_MEMBER_ROTATESTATUS);
	Direction = MsgPacket.GetMember(QUERYMOVE_MEMBER_DIRECTION);
	

	return QueryMove( MoveStatus , StartPosX , StartPosY , StartHeight , TargetPosX , TargetPosY , TargetHeight , RotateStatus , Direction );
}
int CCharacterMsgHandler::HandleMsgAliveTest(CSmartStruct& MsgPacket)
{


	

	return AliveTest();
}
int CCharacterMsgHandler::HandleMsgQueryFly(CSmartStruct& MsgPacket)
{
	UINT MapID ;
	float PosX ;
	float PosY ;
	float Height ;
	float Direction ;


	MapID = MsgPacket.GetMember(QUERYFLY_MEMBER_MAPID);
	PosX = MsgPacket.GetMember(QUERYFLY_MEMBER_POSX);
	PosY = MsgPacket.GetMember(QUERYFLY_MEMBER_POSY);
	Height = MsgPacket.GetMember(QUERYFLY_MEMBER_HEIGHT);
	Direction = MsgPacket.GetMember(QUERYFLY_MEMBER_DIRECTION);
	

	return QueryFly( MapID , PosX , PosY , Height , Direction );
}
int CCharacterMsgHandler::HandleMsgMapChangeConfirm(CSmartStruct& MsgPacket)
{
	int Result ;


	Result = MsgPacket.GetMember(MAPCHANGECONFIRM_MEMBER_RESULT);
	

	return MapChangeConfirm( Result );
}
int CCharacterMsgHandler::HandleMsgQueryJump(CSmartStruct& MsgPacket)
{


	

	return QueryJump();
}
