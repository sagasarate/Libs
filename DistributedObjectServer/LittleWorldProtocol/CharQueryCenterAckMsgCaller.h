#pragma once


class CCharQueryCenterAckMsgCaller :
	public ICharQueryCenterAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CCharQueryCenterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CCharQueryCenterAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CCharQueryCenterAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	RegisterCharAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterCharAck(int Result );
	


	/*************************************************
	函数名:	UnregisterCharAck
	用途:	
	参数:
    Result				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterCharAck(int Result );
	

	bool PackMsgRegisterCharAck(CSmartStruct& MsgPacket,int Result );
	bool PackMsgUnregisterCharAck(CSmartStruct& MsgPacket,int Result );

};
