#pragma once

class CMapAreaMsgHandler:public IMapArea
{

public:
	CMapAreaMsgHandler(void);
	~CMapAreaMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgRegisterMapObject(CSmartStruct& MsgPacket);
	int HandleMsgUnregisterMapObject(CSmartStruct& MsgPacket);
	int HandleMsgUpdateMoveStatus(CSmartStruct& MsgPacket);
	int HandleMsgSendEventMsgPacket(CSmartStruct& MsgPacket);
	int HandleMsgHideOnMap(CSmartStruct& MsgPacket);
	int HandleMsgTeleportInMap(CSmartStruct& MsgPacket);


};

