#pragma once


class CCharQueryCenterMsgCaller :
	public ICharQueryCenter
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CCharQueryCenterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CCharQueryCenterMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CCharQueryCenterMsgCaller(void);
	
	

	/*************************************************
	函数名:	RegisterChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int RegisterChar(UINT64 CharID );
	


	/*************************************************
	函数名:	UnregisterChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int UnregisterChar(UINT64 CharID );
	


	/*************************************************
	函数名:	QueryCharInfo
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int QueryCharInfo(UINT64 CharID );
	

	bool PackMsgRegisterChar(CSmartStruct& MsgPacket,UINT64 CharID );
	bool PackMsgUnregisterChar(CSmartStruct& MsgPacket,UINT64 CharID );
	bool PackMsgQueryCharInfo(CSmartStruct& MsgPacket,UINT64 CharID );

};
