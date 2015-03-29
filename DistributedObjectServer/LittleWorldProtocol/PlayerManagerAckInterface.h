#pragma once

//
class IPlayerManagerAck:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	PlayerLoginAck
	用途:	
	参数:
    Result				
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int PlayerLoginAck(int Result ,UINT64 AccountID ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum PLAYERLOGINACK_MEMBER_IDS
	{
		PLAYERLOGINACK_MEMBER_RESULT=1,
		PLAYERLOGINACK_MEMBER_ACCOUNTID=2,
	};


};
