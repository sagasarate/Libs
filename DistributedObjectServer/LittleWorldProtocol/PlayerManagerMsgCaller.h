#pragma once


class CPlayerManagerMsgCaller :
	public IPlayerManager
{
protected:
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_SenderID;
	CEasyArray<OBJECT_ID>			m_TargetIDs;
public:
	CPlayerManagerMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID TargetID);
	CPlayerManagerMsgCaller(IDistributedObjectOperator * pOperator,OBJECT_ID SenderID,OBJECT_ID * pTargetIDs,ID_LIST_COUNT_TYPE TargetIDCount);
	~CPlayerManagerMsgCaller(void);
	
	

	/*************************************************
	函数名:	PlayerLogin
	用途:	
	参数:
    szAccountName				
    szPassword				
	返回值:无特别意义
	*************************************************/
	virtual int PlayerLogin(LPCTSTR szAccountName ,LPCTSTR szPassword );
	

	bool PackMsgPlayerLogin(CSmartStruct& MsgPacket,LPCTSTR szAccountName ,LPCTSTR szPassword );

};
