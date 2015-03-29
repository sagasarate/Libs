#pragma once

class CPlayerCharDBMsgHandler:public IPlayerCharDB
{

public:
	CPlayerCharDBMsgHandler(void);
	~CPlayerCharDBMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgAccountLogin(CSmartStruct& MsgPacket);
	int HandleMsgAccountLogout(CSmartStruct& MsgPacket);
	int HandleMsgGetAccountChar(CSmartStruct& MsgPacket);
	int HandleMsgCreateChar(CSmartStruct& MsgPacket);
	int HandleMsgDeleteChar(CSmartStruct& MsgPacket);
	int HandleMsgGetCharData(CSmartStruct& MsgPacket);
	int HandleMsgSaveCharData(CSmartStruct& MsgPacket);


};

