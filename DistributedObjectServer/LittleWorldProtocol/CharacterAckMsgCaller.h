#pragma once


class CCharacterAckMsgCaller :
	public ICharacterAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CCharacterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CCharacterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CCharacterAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	QueryCharInfoAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfoAck(int Result ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	QueryMoveAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryMoveAck(int Result );
	


	/*************************************************
	函数名:	AliveTestAck
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int AliveTestAck();
	


	/*************************************************
	函数名:	QueryFlyAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryFlyAck(int Result );
	


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
	virtual int MapChange(UINT MapID ,float PosX ,float PosY ,float Direction );
	


	/*************************************************
	函数名:	QueryJumpAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int QueryJumpAck(UINT64 ObjectID ,int Result );
	

	bool PackMsgQueryCharInfoAck(CSmartStruct& MsgPacket,int Result ,CSmartStruct& CharData );
	bool PackMsgQueryMoveAck(CSmartStruct& MsgPacket,int Result );
	bool PackMsgAliveTestAck(CSmartStruct& MsgPacket);
	bool PackMsgQueryFlyAck(CSmartStruct& MsgPacket,int Result );
	bool PackMsgMapChange(CSmartStruct& MsgPacket,UINT MapID ,float PosX ,float PosY ,float Direction );
	bool PackMsgQueryJumpAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result );

};
