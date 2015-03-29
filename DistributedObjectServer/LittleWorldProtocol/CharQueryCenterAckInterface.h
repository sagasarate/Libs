#pragma once

//
class ICharQueryCenterAck:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	RegisterCharAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterCharAck(int Result ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	UnregisterCharAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterCharAck(int Result ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum REGISTERCHARACK_MEMBER_IDS
	{
		REGISTERCHARACK_MEMBER_RESULT=1,
	};

	enum UNREGISTERCHARACK_MEMBER_IDS
	{
		UNREGISTERCHARACK_MEMBER_RESULT=1,
	};


};
