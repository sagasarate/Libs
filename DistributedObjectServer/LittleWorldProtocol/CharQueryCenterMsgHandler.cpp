#include "stdafx.h"

CCharQueryCenterMsgHandler::CCharQueryCenterMsgHandler(void)
{
}

CCharQueryCenterMsgHandler::~CCharQueryCenterMsgHandler(void)
{
}

void CCharQueryCenterMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharQueryCenterMsgHandler::HandleMsgRegisterChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_REGISTER_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharQueryCenterMsgHandler::HandleMsgUnregisterChar;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_UNREGISTER_CHAR,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CCharQueryCenterMsgHandler::HandleMsgQueryCharInfo;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_CHAR_QUERY_CENTER,METHOD_CQC_QUERY_CHAR_INFO,false),MsgHandleInfo);

}

int CCharQueryCenterMsgHandler::HandleMsgRegisterChar(CSmartStruct& MsgPacket)
{
	UINT64 CharID ;


	CharID = MsgPacket.GetMember(REGISTERCHAR_MEMBER_CHARID);
	

	return RegisterChar( CharID );
}
int CCharQueryCenterMsgHandler::HandleMsgUnregisterChar(CSmartStruct& MsgPacket)
{
	UINT64 CharID ;


	CharID = MsgPacket.GetMember(UNREGISTERCHAR_MEMBER_CHARID);
	

	return UnregisterChar( CharID );
}
int CCharQueryCenterMsgHandler::HandleMsgQueryCharInfo(CSmartStruct& MsgPacket)
{
	UINT64 CharID ;


	CharID = MsgPacket.GetMember(QUERYCHARINFO_MEMBER_CHARID);
	

	return QueryCharInfo( CharID );
}
