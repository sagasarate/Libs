#include "stdafx.h"

CMapAreaMsgHandler::CMapAreaMsgHandler(void)
{
}

CMapAreaMsgHandler::~CMapAreaMsgHandler(void)
{
}

void CMapAreaMsgHandler::InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap)
{
	DOS_MSG_HANDLE_INFO MsgHandleInfo;
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgRegisterMapObject;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_REGISTER_MAP_OBJECT,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgUnregisterMapObject;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UNREGISTER_MAP_OBJECT,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgUpdateMoveStatus;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_UPDATE_MOVE_STATUS,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgSendEventMsgPacket;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_SEND_EVENT_MSG_PACKET,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgHideOnMap;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_HIDE_ON_MAP,false),MsgHandleInfo);
	MsgHandleInfo.pObject=this;
	MsgHandleInfo.pFN=(DOS_MSG_HANDLE_FN)&CMapAreaMsgHandler::HandleMsgTeleportInMap;
	MsgMap.Insert(MAKE_MSG_ID(INTERFACE_LW_MAP_AREA,METHOD_MA_TELEPORT_IN_MAP,false),MsgHandleInfo);

}

int CMapAreaMsgHandler::HandleMsgRegisterMapObject(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	OBJECT_ID ControllerID ;
	OBJECT_ID AreaEventReceiverID ;
	CSmartStruct MoveStatus ;
	UINT Flag ;


	ObjectID = MsgPacket.GetMember(REGISTERMAPOBJECT_MEMBER_OBJECTID);
	ControllerID.ID = MsgPacket.GetMember(REGISTERMAPOBJECT_MEMBER_CONTROLLERID);
	AreaEventReceiverID.ID = MsgPacket.GetMember(REGISTERMAPOBJECT_MEMBER_AREAEVENTRECEIVERID);
	MoveStatus = MsgPacket.GetMember(REGISTERMAPOBJECT_MEMBER_MOVESTATUS);
	Flag = MsgPacket.GetMember(REGISTERMAPOBJECT_MEMBER_FLAG);
	

	return RegisterMapObject( ObjectID , ControllerID , AreaEventReceiverID , MoveStatus , Flag );
}
int CMapAreaMsgHandler::HandleMsgUnregisterMapObject(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;


	ObjectID = MsgPacket.GetMember(UNREGISTERMAPOBJECT_MEMBER_OBJECTID);
	

	return UnregisterMapObject( ObjectID );
}
int CMapAreaMsgHandler::HandleMsgUpdateMoveStatus(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	CSmartStruct MoveStatus ;


	ObjectID = MsgPacket.GetMember(UPDATEMOVESTATUS_MEMBER_OBJECTID);
	MoveStatus = MsgPacket.GetMember(UPDATEMOVESTATUS_MEMBER_MOVESTATUS);
	

	return UpdateMoveStatus( ObjectID , MoveStatus );
}
int CMapAreaMsgHandler::HandleMsgSendEventMsgPacket(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	CSmartStruct EventMsgPacket ;


	ObjectID = MsgPacket.GetMember(SENDEVENTMSGPACKET_MEMBER_OBJECTID);
	EventMsgPacket = MsgPacket.GetMember(SENDEVENTMSGPACKET_MEMBER_EVENTMSGPACKET);
	

	return SendEventMsgPacket( ObjectID , EventMsgPacket );
}
int CMapAreaMsgHandler::HandleMsgHideOnMap(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	BYTE HideType ;


	ObjectID = MsgPacket.GetMember(HIDEONMAP_MEMBER_OBJECTID);
	HideType = MsgPacket.GetMember(HIDEONMAP_MEMBER_HIDETYPE);
	

	return HideOnMap( ObjectID , HideType );
}
int CMapAreaMsgHandler::HandleMsgTeleportInMap(CSmartStruct& MsgPacket)
{
	UINT64 ObjectID ;
	float TargetPosX ;
	float TargetPosY ;
	float TargetHeight ;
	float TargetDir ;


	ObjectID = MsgPacket.GetMember(TELEPORTINMAP_MEMBER_OBJECTID);
	TargetPosX = MsgPacket.GetMember(TELEPORTINMAP_MEMBER_TARGETPOSX);
	TargetPosY = MsgPacket.GetMember(TELEPORTINMAP_MEMBER_TARGETPOSY);
	TargetHeight = MsgPacket.GetMember(TELEPORTINMAP_MEMBER_TARGETHEIGHT);
	TargetDir = MsgPacket.GetMember(TELEPORTINMAP_MEMBER_TARGETDIR);
	

	return TeleportInMap( ObjectID , TargetPosX , TargetPosY , TargetHeight , TargetDir );
}
