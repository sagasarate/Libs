#pragma once

class CPlayerCharOperatorAckMsgHandler:public IPlayerCharOperatorAck
{

public:
	CPlayerCharOperatorAckMsgHandler(void);
	~CPlayerCharOperatorAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgGetCharListAck(CSmartStruct& MsgPacket);
	int HandleMsgCreateCharAck(CSmartStruct& MsgPacket);
	int HandleMsgDeleteCharAck(CSmartStruct& MsgPacket);
	int HandleMsgSelectCharAck(CSmartStruct& MsgPacket);


};

