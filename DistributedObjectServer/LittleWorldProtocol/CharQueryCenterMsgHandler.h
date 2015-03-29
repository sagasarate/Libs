#pragma once

class CCharQueryCenterMsgHandler:public ICharQueryCenter
{

public:
	CCharQueryCenterMsgHandler(void);
	~CCharQueryCenterMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgRegisterChar(CSmartStruct& MsgPacket);
	int HandleMsgUnregisterChar(CSmartStruct& MsgPacket);
	int HandleMsgQueryCharInfo(CSmartStruct& MsgPacket);


};

