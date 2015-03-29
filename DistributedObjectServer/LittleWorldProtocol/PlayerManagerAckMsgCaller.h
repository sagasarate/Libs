#pragma once


class CPlayerManagerAckMsgCaller :
	public IPlayerManagerAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerManagerAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerManagerAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerManagerAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	PlayerLoginAck
	用途:	
	参数:
    Result				
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int PlayerLoginAck(int Result ,UINT64 AccountID );
	

	bool PackMsgPlayerLoginAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID );

};
