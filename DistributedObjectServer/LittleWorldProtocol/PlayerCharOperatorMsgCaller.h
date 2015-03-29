#pragma once


class CPlayerCharOperatorMsgCaller :
	public IPlayerCharOperator
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerCharOperatorMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerCharOperatorMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerCharOperatorMsgCaller(void);
	
	

	/*************************************************
	函数名:	GetCharList
	用途:	
	参数:

	返回值:无特别意义
	*************************************************/
	virtual int GetCharList();
	


	/*************************************************
	函数名:	CreateChar
	用途:	
	参数:
    CharCreateData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateChar(CSmartStruct& CharCreateData );
	


	/*************************************************
	函数名:	DeleteChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteChar(UINT64 CharID );
	


	/*************************************************
	函数名:	SelectChar
	用途:	
	参数:
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int SelectChar(UINT64 CharID );
	

	bool PackMsgGetCharList(CSmartStruct& MsgPacket);
	bool PackMsgCreateChar(CSmartStruct& MsgPacket,CSmartStruct& CharCreateData );
	bool PackMsgDeleteChar(CSmartStruct& MsgPacket,UINT64 CharID );
	bool PackMsgSelectChar(CSmartStruct& MsgPacket,UINT64 CharID );

};
