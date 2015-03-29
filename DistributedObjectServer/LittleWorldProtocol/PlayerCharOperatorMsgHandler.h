#pragma once

class CPlayerCharOperatorMsgHandler:public IPlayerCharOperator
{

public:
	CPlayerCharOperatorMsgHandler(void);
	~CPlayerCharOperatorMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgGetCharList(CSmartStruct& MsgPacket);
	int HandleMsgCreateChar(CSmartStruct& MsgPacket);
	int HandleMsgDeleteChar(CSmartStruct& MsgPacket);
	int HandleMsgSelectChar(CSmartStruct& MsgPacket);


};

