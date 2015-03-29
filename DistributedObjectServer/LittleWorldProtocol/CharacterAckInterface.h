#pragma once

//
class ICharacterAck:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	QueryCharInfoAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfoAck(int Result ,CSmartStruct& CharData ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryMoveAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryMoveAck(int Result ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	AliveTestAck
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int AliveTestAck() {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryFlyAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryFlyAck(int Result ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	MapChange
	用途:	
	参数:
    MapID				
    PosX				
    PosY				
    Direction				
	返回值:无特别意义
	*************************************************/
	virtual int MapChange(UINT MapID ,float PosX ,float PosY ,float Direction ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryJumpAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryJumpAck(UINT64 ObjectID ,int Result ) {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum QUERYCHARINFOACK_MEMBER_IDS
	{
		QUERYCHARINFOACK_MEMBER_RESULT=1,
		QUERYCHARINFOACK_MEMBER_CHARDATA=2,
	};

	enum QUERYMOVEACK_MEMBER_IDS
	{
		QUERYMOVEACK_MEMBER_RESULT=1,
	};

	enum ALIVETESTACK_MEMBER_IDS
	{
	};

	enum QUERYFLYACK_MEMBER_IDS
	{
		QUERYFLYACK_MEMBER_RESULT=1,
	};

	enum MAPCHANGE_MEMBER_IDS
	{
		MAPCHANGE_MEMBER_MAPID=1,
		MAPCHANGE_MEMBER_POSX=2,
		MAPCHANGE_MEMBER_POSY=3,
		MAPCHANGE_MEMBER_DIRECTION=4,
	};

	enum QUERYJUMPACK_MEMBER_IDS
	{
		QUERYJUMPACK_MEMBER_OBJECTID=1,
		QUERYJUMPACK_MEMBER_RESULT=2,
	};


};
