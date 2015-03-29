#pragma once

class CCharacterAckMsgHandler:public ICharacterAck
{

public:
	CCharacterAckMsgHandler(void);
	~CCharacterAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgQueryCharInfoAck(CSmartStruct& MsgPacket);
	int HandleMsgQueryMoveAck(CSmartStruct& MsgPacket);
	int HandleMsgAliveTestAck(CSmartStruct& MsgPacket);
	int HandleMsgQueryFlyAck(CSmartStruct& MsgPacket);
	int HandleMsgMapChange(CSmartStruct& MsgPacket);
	int HandleMsgQueryJumpAck(CSmartStruct& MsgPacket);


};

