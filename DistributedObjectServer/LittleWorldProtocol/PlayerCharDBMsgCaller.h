#pragma once


class CPlayerCharDBMsgCaller :
	public IPlayerCharDB
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerCharDBMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerCharDBMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerCharDBMsgCaller(void);
	
	

	/*************************************************
	函数名:	AccountLogin
	用途:	
	参数:
    szAccountName				
    szPassword				
    szClientIP				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLogin(LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP );
	


	/*************************************************
	函数名:	AccountLogout
	用途:	
	参数:
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int AccountLogout(UINT64 AccountID );
	


	/*************************************************
	函数名:	GetAccountChar
	用途:	
	参数:
    AccountID				
	返回值:无特别意义
	*************************************************/
	virtual int GetAccountChar(UINT64 AccountID );
	


	/*************************************************
	函数名:	CreateChar
	用途:	
	参数:
    AccountID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int CreateChar(UINT64 AccountID ,CSmartStruct& CharData );
	


	/*************************************************
	函数名:	DeleteChar
	用途:	
	参数:
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int DeleteChar(UINT64 AccountID ,UINT64 CharID );
	


	/*************************************************
	函数名:	GetCharData
	用途:	
	参数:
    AccountID				
    CharID				
	返回值:无特别意义
	*************************************************/
	virtual int GetCharData(UINT64 AccountID ,UINT64 CharID );
	


	/*************************************************
	函数名:	SaveCharData
	用途:	
	参数:
    AccountID				
    CharID				
    CharData				
	返回值:无特别意义
	*************************************************/
	virtual int SaveCharData(UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );
	

	bool PackMsgAccountLogin(CSmartStruct& MsgPacket,LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP );
	bool PackMsgAccountLogout(CSmartStruct& MsgPacket,UINT64 AccountID );
	bool PackMsgGetAccountChar(CSmartStruct& MsgPacket,UINT64 AccountID );
	bool PackMsgCreateChar(CSmartStruct& MsgPacket,UINT64 AccountID ,CSmartStruct& CharData );
	bool PackMsgDeleteChar(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID );
	bool PackMsgGetCharData(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID );
	bool PackMsgSaveCharData(CSmartStruct& MsgPacket,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );

};
