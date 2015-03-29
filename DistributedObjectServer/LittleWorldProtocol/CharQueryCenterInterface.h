#pragma once

//
class ICharQueryCenter:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	RegisterChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterChar(UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	UnregisterChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterChar(UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryCharInfo
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfo(UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum REGISTERCHAR_MEMBER_IDS
	{
		REGISTERCHAR_MEMBER_CHARID=1,
	};

	enum UNREGISTERCHAR_MEMBER_IDS
	{
		UNREGISTERCHAR_MEMBER_CHARID=1,
	};

	enum QUERYCHARINFO_MEMBER_IDS
	{
		QUERYCHARINFO_MEMBER_CHARID=1,
	};


};
