#pragma once

//
class ICharacter:public CBaseMsgHandler
{
public:
	

	/*************************************************
	函数名:	QueryCharInfo
	用途:	
	参数:
    RequesterID				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfo(OBJECT_ID RequesterID ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryMove
	用途:	
	参数:
    MoveStatus				
    StartPosX				
    StartPosY				
    StartHeight				
    TargetPosX				
    TargetPosY				
    TargetHeight				
    RotateStatus				
    Direction				
	返回值:无特别意义
	*************************************************/
	virtual int QueryMove(BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	AliveTest
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int AliveTest() {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryFly
	用途:	
	参数:
    MapID				
    PosX				
    PosY				
    Height				
    Direction				
	返回值:无特别意义
	*************************************************/
	virtual int QueryFly(UINT MapID ,float PosX ,float PosY ,float Height ,float Direction ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	MapChangeConfirm
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int MapChangeConfirm(int Result ) {return COMMON_RESULT_FAILED;}
	


	/*************************************************
	函数名:	QueryJump
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int QueryJump() {return COMMON_RESULT_FAILED;}
	

	
protected:	
	enum QUERYCHARINFO_MEMBER_IDS
	{
		QUERYCHARINFO_MEMBER_REQUESTERID=2,
	};

	enum QUERYMOVE_MEMBER_IDS
	{
		QUERYMOVE_MEMBER_MOVESTATUS=1,
		QUERYMOVE_MEMBER_STARTPOSX=2,
		QUERYMOVE_MEMBER_STARTPOSY=3,
		QUERYMOVE_MEMBER_STARTHEIGHT=8,
		QUERYMOVE_MEMBER_TARGETPOSX=4,
		QUERYMOVE_MEMBER_TARGETPOSY=5,
		QUERYMOVE_MEMBER_TARGETHEIGHT=9,
		QUERYMOVE_MEMBER_ROTATESTATUS=6,
		QUERYMOVE_MEMBER_DIRECTION=7,
	};

	enum ALIVETEST_MEMBER_IDS
	{
	};

	enum QUERYFLY_MEMBER_IDS
	{
		QUERYFLY_MEMBER_MAPID=1,
		QUERYFLY_MEMBER_POSX=2,
		QUERYFLY_MEMBER_POSY=3,
		QUERYFLY_MEMBER_HEIGHT=5,
		QUERYFLY_MEMBER_DIRECTION=4,
	};

	enum MAPCHANGECONFIRM_MEMBER_IDS
	{
		MAPCHANGECONFIRM_MEMBER_RESULT=1,
	};

	enum QUERYJUMP_MEMBER_IDS
	{
	};


};
