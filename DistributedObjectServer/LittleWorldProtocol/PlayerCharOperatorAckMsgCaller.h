#pragma once


class CPlayerCharOperatorAckMsgCaller :
	public IPlayerCharOperatorAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerCharOperatorAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerCharOperatorAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerCharOperatorAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	GetCharListAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetCharListAck(int Result ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	CreateCharAck
	用途:	
	参数:
    Result				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateCharAck(int Result ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	DeleteCharAck
	用途:	
	参数:
    Result				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteCharAck(int Result ,UINT64 CharID );
	


	/*************************************************
	函数名:	SelectCharAck
	用途:	
	参数:
    Result				
    CharID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int SelectCharAck(int Result ,UINT64 CharID ,CSmartStruct& CharData );
	

	bool PackMsgGetCharListAck(CSmartStruct& MsgPacket,int Result ,CSmartStruct& CharData );
	bool PackMsgCreateCharAck(CSmartStruct& MsgPacket,int Result ,CSmartStruct& CharData );
	bool PackMsgDeleteCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 CharID );
	bool PackMsgSelectCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 CharID ,CSmartStruct& CharData );

};
