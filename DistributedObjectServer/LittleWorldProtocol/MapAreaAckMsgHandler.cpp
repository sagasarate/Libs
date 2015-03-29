#include "stdafx.h"

CMapAreaAckMsgHandler::CMapAreaAckMsgHandler(void)
{
}

CMapAreaAckMsgHandler::~CMapAreaAckMsgHandler(void)
{
}

void CMapAreaAckMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgRegisterMapObjectAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_REGISTER_MAP_OBJECT,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgUnregisterMapObjectAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UNREGISTER_MAP_OBJECT,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgUpdateMoveStatusAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UPDATE_MOVE_STATUS,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgSendEventMsgPacketAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_SEND_EVENT_MSG_PACKET,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgNewMapObject;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_NEW_MAP_OBJECT,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgDeleteMapObject;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_DEL_MAP_OBJECT,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgMoveStatusUpdate;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_MOVE_STATUS_UPDATE,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgEventMsgPacket;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_EVENT_MSG_PACKET,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgHideOnMapAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_HIDE_ON_MAP,true),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaAckMsgHandler::HandleMsgTeleportInMapAck;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_TELEPORT_IN_MAP,true),MsgHandleInfo);

}

int CMapAreaAckMsgHandler::HandleMsgRegisterMapObjectAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	int Result ;


	ObjectID = MsgPacket.GetMember(REGISTERMAPOBJECTACK_MEMBER_OBJECTID);
	Result = MsgPacket.GetMember(REGISTERMAPOBJECTACK_MEMBER_RESULT);
	

	return RegisterMapObjectAck( ObjectID , Result );
}
int CMapAreaAckMsgHandler::HandleMsgUnregisterMapObjectAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	int Result ;


	ObjectID = MsgPacket.GetMember(UNREGISTERMAPOBJECTACK_MEMBER_OBJECTID);
	Result = MsgPacket.GetMember(UNREGISTERMAPOBJECTACK_MEMBER_RESULT);
	

	return UnregisterMapObjectAck( ObjectID , Result );
}
int CMapAreaAckMsgHandler::HandleMsgUpdateMoveStatusAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	int Result ;


	ObjectID = MsgPacket.GetMember(UPDATEMOVESTATUSACK_MEMBER_OBJECTID);
	Result = MsgPacket.GetMember(UPDATEMOVESTATUSACK_MEMBER_RESULT);
	

	return UpdateMoveStatusAck( ObjectID , Result );
}
int CMapAreaAckMsgHandler::HandleMsgSendEventMsgPacketAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	int Result ;


	ObjectID = MsgPacket.GetMember(SENDEVENTMSGPACKETACK_MEMBER_OBJECTID);
	Result = MsgPacket.GetMember(SENDEVENTMSGPACKETACK_MEMBER_RESULT);
	

	return SendEventMsgPacketAck( ObjectID , Result );
}
int CMapAreaAckMsgHandler::HandleMsgNewMapObject(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	CSmartStruct MoveStatus ;


	ObjectID = MsgPacket.GetMember(NEWMAPOBJECT_MEMBER_OBJECTID);
	MoveStatus = MsgPacket.GetMember(NEWMAPOBJECT_MEMBER_MOVESTATUS);
	

	return NewMapObject( ObjectID , MoveStatus );
}
int CMapAreaAckMsgHandler::HandleMsgDeleteMapObject(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;


	ObjectID = MsgPacket.GetMember(DELETEMAPOBJECT_MEMBER_OBJECTID);
	

	return DeleteMapObject( ObjectID );
}
int CMapAreaAckMsgHandler::HandleMsgMoveStatusUpdate(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	BYTE IsForce ;
	CSmartStruct MoveStatus ;


	ObjectID = MsgPacket.GetMember(MOVESTATUSUPDATE_MEMBER_OBJECTID);
	IsForce = MsgPacket.GetMember(MOVESTATUSUPDATE_MEMBER_ISFORCE);
	MoveStatus = MsgPacket.GetMember(MOVESTATUSUPDATE_MEMBER_MOVESTATUS);
	

	return MoveStatusUpdate( ObjectID , IsForce , MoveStatus );
}
int CMapAreaAckMsgHandler::HandleMsgEventMsgPacket(CSmartStruct& MsgPacket)
{
	CSmartStruct EventMsg ;


	EventMsg = MsgPacket.GetMember(EVENTMSGPACKET_MEMBER_EVENTMSG);
	

	return EventMsgPacket( EventMsg );
}
int CMapAreaAckMsgHandler::HandleMsgHideOnMapAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	BYTE HideType ;
	int Result ;


	ObjectID = MsgPacket.GetMember(HIDEONMAPACK_MEMBER_OBJECTID);
	HideType = MsgPacket.GetMember(HIDEONMAPACK_MEMBER_HIDETYPE);
	Result = MsgPacket.GetMember(HIDEONMAPACK_MEMBER_RESULT);
	

	return HideOnMapAck( ObjectID , HideType , Result );
}
int CMapAreaAckMsgHandler::HandleMsgTeleportInMapAck(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	float TargetPosX ;
	float TargetPosY ;
	float TargetHeight ;
	float TargetDir ;
	int Result ;


	ObjectID = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_OBJECTID);
	TargetPosX = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_TARGETPOSX);
	TargetPosY = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_TARGETPOSY);
	TargetHeight = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_TARGETHEIGHT);
	TargetDir = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_TARGETDIR);
	Result = MsgPacket.GetMember(TELEPORTINMAPACK_MEMBER_RESULT);
	

	return TeleportInMapAck( ObjectID , TargetPosX , TargetPosY , TargetHeight , TargetDir , Result );
}
