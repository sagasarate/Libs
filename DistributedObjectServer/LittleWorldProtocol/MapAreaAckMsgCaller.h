#pragma once


class CMapAreaAckMsgCaller :
	public IMapAreaAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CMapAreaAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CMapAreaAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CMapAreaAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	RegisterMapObjectAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterMapObjectAck(UINT64 ObjectID ,int Result );
	


	/*************************************************
	函数名:	UnregisterMapObjectAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterMapObjectAck(UINT64 ObjectID ,int Result );
	


	/*************************************************
	函数名:	UpdateMoveStatusAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int UpdateMoveStatusAck(UINT64 ObjectID ,int Result );
	


	/*************************************************
	函数名:	SendEventMsgPacketAck
	用途:	
	参数:
    ObjectID				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int SendEventMsgPacketAck(UINT64 ObjectID ,int Result );
	


	/*************************************************
	函数名:	NewMapObject
	用途:	
	参数:
    ObjectID				
    MoveStatus				
	返回值:无特别意义
	*************************************************/
	virtual int NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	


	/*************************************************
	函数名:	DeleteMapObject
	用途:	
	参数:
    ObjectID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteMapObject(UINT64 ObjectID );
	


	/*************************************************
	函数名:	MoveStatusUpdate
	用途:	
	参数:
    ObjectID				
    IsForce				
    MoveStatus				
	返回值:无特别意义
	*************************************************/
	virtual int MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus );
	


	/*************************************************
	函数名:	EventMsgPacket
	用途:	
	参数:
    EventMsg				
	返回值:无特别意义
	*************************************************/
	virtual int EventMsgPacket(CSmartStruct& EventMsg );
	


	/*************************************************
	函数名:	HideOnMapAck
	用途:	
	参数:
    ObjectID				
    HideType				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int HideOnMapAck(UINT64 ObjectID ,BYTE HideType ,int Result );
	


	/*************************************************
	函数名:	TeleportInMapAck
	用途:	
	参数:
    ObjectID				
    TargetPosX				
    TargetPosY				
    TargetHeight				
    TargetDir				
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int TeleportInMapAck(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir ,int Result );
	

	bool PackMsgRegisterMapObjectAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result );
	bool PackMsgUnregisterMapObjectAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result );
	bool PackMsgUpdateMoveStatusAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result );
	bool PackMsgSendEventMsgPacketAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,int Result );
	bool PackMsgNewMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& MoveStatus );
	bool PackMsgDeleteMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID );
	bool PackMsgMoveStatusUpdate(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus );
	bool PackMsgEventMsgPacket(CSmartStruct& MsgPacket,CSmartStruct& EventMsg );
	bool PackMsgHideOnMapAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE HideType ,int Result );
	bool PackMsgTeleportInMapAck(CSmartStruct& MsgPacket,UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir ,int Result );

};
