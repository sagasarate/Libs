#pragma once

class CPlayerCharDBAckMsgHandler:public IPlayerCharDBAck
{

public:
	CPlayerCharDBAckMsgHandler(void);
	~CPlayerCharDBAckMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgAccountLoginAck(CSmartStruct& MsgPacket);
	int HandleMsgAccountLogoutAck(CSmartStruct& MsgPacket);
	int HandleMsgGetAccountCharAck(CSmartStruct& MsgPacket);
	int HandleMsgCreateCharAck(CSmartStruct& MsgPacket);
	int HandleMsgDeleteCharAck(CSmartStruct& MsgPacket);
	int HandleMsgGetCharDataAck(CSmartStruct& MsgPacket);
	int HandleMsgSaveCharDataAck(CSmartStruct& MsgPacket);


};

