#pragma once


class CCharacterMsgCaller :
	public ICharacter
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CCharacterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CCharacterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CCharacterMsgCaller(void);
	
	

	/*************************************************
	函数名:	QueryCharInfo
	用途:	
	参数:
    RequesterID				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfo(OBJECT_ID RequesterID );
	


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
	virtual int QueryMove(BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction );
	


	/*************************************************
	函数名:	AliveTest
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int AliveTest();
	


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
	virtual int QueryFly(UINT MapID ,float PosX ,float PosY ,float Height ,float Direction );
	


	/*************************************************
	函数名:	MapChangeConfirm
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int MapChangeConfirm(int Result );
	


	/*************************************************
	函数名:	QueryJump
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int QueryJump();
	

	bool PackMsgQueryCharInfo(CSmartStruct& MsgPacket,OBJECT_ID RequesterID );
	bool PackMsgQueryMove(CSmartStruct& MsgPacket,BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction );
	bool PackMsgAliveTest(CSmartStruct& MsgPacket);
	bool PackMsgQueryFly(CSmartStruct& MsgPacket,UINT MapID ,float PosX ,float PosY ,float Height ,float Direction );
	bool PackMsgMapChangeConfirm(CSmartStruct& MsgPacket,int Result );
	bool PackMsgQueryJump(CSmartStruct& MsgPacket);

};
