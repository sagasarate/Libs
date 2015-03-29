#pragma once

class CCharacterMsgHandler:public ICharacter
{

public:
	CCharacterMsgHandler(void);
	~CCharacterMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgQueryCharInfo(CSmartStruct& MsgPacket);
	int HandleMsgQueryMove(CSmartStruct& MsgPacket);
	int HandleMsgAliveTest(CSmartStruct& MsgPacket);
	int HandleMsgQueryFly(CSmartStruct& MsgPacket);
	int HandleMsgMapChangeConfirm(CSmartStruct& MsgPacket);
	int HandleMsgQueryJump(CSmartStruct& MsgPacket);


};

