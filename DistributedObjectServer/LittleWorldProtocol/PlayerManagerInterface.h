#pragma once

//
class IPlayerManager:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	PlayerLogin
	用途:	
	参数:
    szAccountName				
    szPassword				
	返回值:无特别意义
	*************************************************/
	virtual int PlayerLogin(LPCTSTR szAccountName ,LPCTSTR szPassword ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum PLAYERLOGIN_MEMBER_IDS
	{
		PLAYERLOGIN_MEMBER_SZACCOUNTNAME=1,
		PLAYERLOGIN_MEMBER_SZPASSWORD=5,
	};


};
