#pragma once

class CPlayerManagerAckMsgHandler:public IPlayerManagerAck
{

public:
	CPlayerManagerAckMsgHandler(void);
	~CPlayerManagerAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgPlayerLoginAck(CSmartStruct& MsgPacket);


};

