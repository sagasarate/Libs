#pragma once

class CCharQueryCenterAckMsgHandler:public ICharQueryCenterAck
{

public:
	CCharQueryCenterAckMsgHandler(void);
	~CCharQueryCenterAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgRegisterCharAck(CSmartStruct& MsgPacket);
	int HandleMsgUnregisterCharAck(CSmartStruct& MsgPacket);


};

