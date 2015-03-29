#pragma once

//
class IPlayerCharDBAck:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	AccountLoginAck
	用途:	
	参数:
    Result				
    AccountID				
    AccountData				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLoginAck(int Result ,UINT64 AccountID ,CSmartStruct& AccountData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	AccountLogoutAck
	用途:	
	参数:
    Result				
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLogoutAck(int Result ,UINT64 AccountID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	GetAccountCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetAccountCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	CreateCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	DeleteCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteCharAck(int Result ,UINT64 AccountID ,UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	GetCharDataAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	SaveCharDataAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int SaveCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum ACCOUNTLOGINACK_MEMBER_IDS
	{
		ACCOUNTLOGINACK_MEMBER_RESULT=1,
		ACCOUNTLOGINACK_MEMBER_ACCOUNTID=2,
		ACCOUNTLOGINACK_MEMBER_ACCOUNTDATA=3,
	};

	enum ACCOUNTLOGOUTACK_MEMBER_IDS
	{
		ACCOUNTLOGOUTACK_MEMBER_RESULT=1,
		ACCOUNTLOGOUTACK_MEMBER_ACCOUNTID=2,
	};

	enum GETACCOUNTCHARACK_MEMBER_IDS
	{
		GETACCOUNTCHARACK_MEMBER_RESULT=1,
		GETACCOUNTCHARACK_MEMBER_ACCOUNTID=2,
		GETACCOUNTCHARACK_MEMBER_CHARDATA=3,
	};

	enum CREATECHARACK_MEMBER_IDS
	{
		CREATECHARACK_MEMBER_RESULT=1,
		CREATECHARACK_MEMBER_ACCOUNTID=2,
		CREATECHARACK_MEMBER_CHARDATA=3,
	};

	enum DELETECHARACK_MEMBER_IDS
	{
		DELETECHARACK_MEMBER_RESULT=1,
		DELETECHARACK_MEMBER_ACCOUNTID=2,
		DELETECHARACK_MEMBER_CHARID=3,
	};

	enum GETCHARDATAACK_MEMBER_IDS
	{
		GETCHARDATAACK_MEMBER_RESULT=1,
		GETCHARDATAACK_MEMBER_ACCOUNTID=2,
		GETCHARDATAACK_MEMBER_CHARID=3,
		GETCHARDATAACK_MEMBER_CHARDATA=4,
	};

	enum SAVECHARDATAACK_MEMBER_IDS
	{
		SAVECHARDATAACK_MEMBER_RESULT=1,
		SAVECHARDATAACK_MEMBER_ACCOUNTID=2,
		SAVECHARDATAACK_MEMBER_CHARID=3,
	};


};
