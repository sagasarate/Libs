#include "stdafx.h"

CPlayerCharOperatorMsgHandler::CPlayerCharOperatorMsgHandler(void)
{
}

CPlayerCharOperatorMsgHandler::~CPlayerCharOperatorMsgHandler(void)
{
}

void CPlayerCharOperatorMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorMsgHandler::HandleMsgGetCharList;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_GET_CHAR_LIST,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorMsgHandler::HandleMsgCreateChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_CREATE_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorMsgHandler::HandleMsgDeleteChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_DELETE_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CPlayerCharOperatorMsgHandler::HandleMsgSelectChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_PLAYER_CHAR_OPERATOR,METHOD_PCO_SELECT_CHAR,false),MsgHandleInfo);

}

int CPlayerCharOperatorMsgHandler::HandleMsgGetCharList(CSmartStruct& MsgPacket)
{


	

	return GetCharList();
}
int CPlayerCharOperatorMsgHandler::HandleMsgCreateChar(CSmartStruct& MsgPacket)
{
	CSmartStruct CharCreateData ;


	CharCreateData = MsgPacket.GetMember(CREATECHAR_MEMBER_CHARCREATEDATA);
	

	return CreateChar( CharCreateData );
}
int CPlayerCharOperatorMsgHandler::HandleMsgDeleteChar(CSmartStruct& MsgPacket)
{
	UINT64 CharID ;


	CharID = MsgPacket.GetMember(DELETECHAR_MEMBER_CHARID);
	

	return DeleteChar( CharID );
}
int CPlayerCharOperatorMsgHandler::HandleMsgSelectChar(CSmartStruct& MsgPacket)
{
	UINT64 CharID ;


	CharID = MsgPacket.GetMember(SELECTCHAR_MEMBER_CHARID);
	

	return SelectChar( CharID );
}
