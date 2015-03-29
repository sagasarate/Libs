#pragma once

//
class IPlayerCharOperator:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	GetCharList
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int GetCharList() {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	CreateChar
	用途:	
	参数:
    CharCreateData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateChar(CSmartStruct& CharCreateData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	DeleteChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteChar(UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	SelectChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int SelectChar(UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum GETCHARLIST_MEMBER_IDS
	{
	};

	enum CREATECHAR_MEMBER_IDS
	{
		CREATECHAR_MEMBER_CHARCREATEDATA=1,
	};

	enum DELETECHAR_MEMBER_IDS
	{
		DELETECHAR_MEMBER_CHARID=1,
	};

	enum SELECTCHAR_MEMBER_IDS
	{
		SELECTCHAR_MEMBER_CHARID=1,
	};


};
