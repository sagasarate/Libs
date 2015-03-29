#pragma once

//
class IPlayerCharOperatorAck:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	GetCharListAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetCharListAck(int Result ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	CreateCharAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateCharAck(int Result ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	DeleteCharAck
	用途:	
	参数:
    Result				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteCharAck(int Result ,UINT64 CharID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	SelectCharAck
	用途:	
	参数:
    Result				
    CharID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int SelectCharAck(int Result ,UINT64 CharID ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum GETCHARLISTACK_MEMBER_IDS
	{
		GETCHARLISTACK_MEMBER_RESULT=1,
		GETCHARLISTACK_MEMBER_CHARDATA=2,
	};

	enum CREATECHARACK_MEMBER_IDS
	{
		CREATECHARACK_MEMBER_RESULT=1,
		CREATECHARACK_MEMBER_CHARDATA=2,
	};

	enum DELETECHARACK_MEMBER_IDS
	{
		DELETECHARACK_MEMBER_RESULT=1,
		DELETECHARACK_MEMBER_CHARID=2,
	};

	enum SELECTCHARACK_MEMBER_IDS
	{
		SELECTCHARACK_MEMBER_RESULT=1,
		SELECTCHARACK_MEMBER_CHARID=2,
		SELECTCHARACK_MEMBER_CHARDATA=3,
	};


};
