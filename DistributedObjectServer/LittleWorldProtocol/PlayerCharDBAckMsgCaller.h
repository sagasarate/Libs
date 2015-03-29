#pragma once


class CPlayerCharDBAckMsgCaller :
	public IPlayerCharDBAck
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerCharDBAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerCharDBAckMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerCharDBAckMsgCaller(void);
	
	

	/*************************************************
	函数名:	AccountLoginAck
	用途:	
	参数:
    Result				
    AccountID				
    AccountData				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLoginAck(int Result ,UINT64 AccountID ,CSmartStruct& AccountData );
	


	/*************************************************
	函数名:	AccountLogoutAck
	用途:	
	参数:
    Result				
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLogoutAck(int Result ,UINT64 AccountID );
	


	/*************************************************
	函数名:	GetAccountCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetAccountCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	CreateCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	DeleteCharAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteCharAck(int Result ,UINT64 AccountID ,UINT64 CharID );
	


	/*************************************************
	函数名:	GetCharDataAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int GetCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	SaveCharDataAck
	用途:	
	参数:
    Result				
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int SaveCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID );
	

	bool PackMsgAccountLoginAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& AccountData );
	bool PackMsgAccountLogoutAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID );
	bool PackMsgGetAccountCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	bool PackMsgCreateCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	bool PackMsgDeleteCharAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID );
	bool PackMsgGetCharDataAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );
	bool PackMsgSaveCharDataAck(CSmartStruct& MsgPacket,int Result ,UINT64 AccountID ,UINT64 CharID );

};
