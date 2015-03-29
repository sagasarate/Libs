#pragma once


class CMapAreaMsgCaller :
	public IMapArea
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CMapAreaMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CMapAreaMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CMapAreaMsgCaller(void);
	
	

	/*************************************************
	函数名:	RegisterMapObject
	用途:	
	参数:
    ObjectID				
    ControllerID				
    AreaEventReceiverID				
    MoveStatus				
    Flag				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterMapObject(UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag );
	


	/*************************************************
	函数名:	UnregisterMapObject
	用途:	
	参数:
    ObjectID				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterMapObject(UINT64 ObjectID );
	


	/*************************************************
	函数名:	UpdateMoveStatus
	用途:	
	参数:
    ObjectID				
    MoveStatus				
	返回值:无特别意义
	*************************************************/
	virtual int UpdateMoveStatus(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	


	/*************************************************
	函数名:	SendEventMsgPacket
	用途:	
	参数:
    ObjectID				
    EventMsgPacket				
	返回值:无特别意义
	*************************************************/
	virtual int SendEventMsgPacket(UINT64 ObjectID ,CSmartStruct& EventMsgPacket );
	


	/*************************************************
	函数名:	HideOnMap
	用途:	
	参数:
    ObjectID				
    HideType				
	返回值:无特别意义
	*************************************************/
	virtual int HideOnMap(UINT64 ObjectID ,BYTE HideType );
	


	/*************************************************
	函数名:	TeleportInMap
	用途:	
	参数:
    ObjectID				
    TargetPosX				
    TargetPosY				
    TargetHeight				
    TargetDir				
	返回值:无特别意义
	*************************************************/
	virtual int TeleportInMap(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir );
	

	bool PackMsgRegisterMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag );
	bool PackMsgUnregisterMapObject(CSmartStruct& MsgPacket,UINT64 ObjectID );
	bool PackMsgUpdateMoveStatus(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& MoveStatus );
	bool PackMsgSendEventMsgPacket(CSmartStruct& MsgPacket,UINT64 ObjectID ,CSmartStruct& EventMsgPacket );
	bool PackMsgHideOnMap(CSmartStruct& MsgPacket,UINT64 ObjectID ,BYTE HideType );
	bool PackMsgTeleportInMap(CSmartStruct& MsgPacket,UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir );

};
