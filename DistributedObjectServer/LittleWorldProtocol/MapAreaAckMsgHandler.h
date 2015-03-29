#pragma once

class CMapAreaAckMsgHandler:public IMapAreaAck
{

public:
	CMapAreaAckMsgHandler(void);
	~CMapAreaAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgRegisterMapObjectAck(CSmartStruct& MsgPacket);
	int HandleMsgUnregisterMapObjectAck(CSmartStruct& MsgPacket);
	int HandleMsgUpdateMoveStatusAck(CSmartStruct& MsgPacket);
	int HandleMsgSendEventMsgPacketAck(CSmartStruct& MsgPacket);
	int HandleMsgNewMapObject(CSmartStruct& MsgPacket);
	int HandleMsgDeleteMapObject(CSmartStruct& MsgPacket);
	int HandleMsgMoveStatusUpdate(CSmartStruct& MsgPacket);
	int HandleMsgEventMsgPacket(CSmartStruct& MsgPacket);
	int HandleMsgHideOnMapAck(CSmartStruct& MsgPacket);
	int HandleMsgTeleportInMapAck(CSmartStruct& MsgPacket);


};

