#pragma once

class CPlayerManagerMsgHandler:public IPlayerManager
{

public:
	CPlayerManagerMsgHandler(void);
	~CPlayerManagerMsgHandler(void);

	void InitMsgMap(CEasyMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>& MsgMap);	

	int HandleMsgPlayerLogin(CSmartStruct& MsgPacket);


};

