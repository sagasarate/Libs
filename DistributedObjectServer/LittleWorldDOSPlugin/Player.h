#pragma once

class CPlayerManager;
class CPlayer :
	public CCharacter,
	public CPlayerCharDBAckMsgHandler,
	public CPlayerCharOperatorMsgHandler
{
protected:
	enum STATUS
	{
		STATUS_LOGIN,
		STATUS_CHAR_OPERATE,
		STATUS_GAMING,
	};

	CPlayerManager *	m_pManager;
	OBJECT_ID			m_ManagerID;
	UINT				m_PoolID;
	OBJECT_ID			m_ClientProxyID;
	OBJECT_ID			m_CharDBID;
	CEasyString			m_AccountName;
	CEasyString			m_Password;
	UINT64				m_AccountID;

	int					m_Status;

	DECLARE_DOS_MSG_MAP(CPlayer)
public:
	CPlayer(void);
	~CPlayer(void);

	BOOL Init(CPlayerManager * pManager,OBJECT_ID ManagerID,UINT PoolID,OBJECT_ID MapID,OBJECT_ID CharQueryCenterID,OBJECT_ID CharDBID,OBJECT_ID ClientProxyID,LPCTSTR szAccountName,LPCTSTR szPassword);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();

	void ForceRelease();
protected:

	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual int Update(int ProcessPacketLimit);

	virtual int RegisterMapObjectAck(UINT64 ObjectID ,int Result );
	virtual int MapChangeConfirm(int Result );

	void OnClientDisconnect();
	void DisconnectClient();
	void SavePlayerData();

protected:
	//CPlayerCharDBAckMsgHandler
	virtual int AccountLoginAck(int Result ,UINT64 AccountID ,CSmartStruct& AccountData );
	virtual int AccountLogoutAck(int Result ,UINT64 AccountID );
	virtual int GetAccountCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	virtual int CreateCharAck(int Result ,UINT64 AccountID ,CSmartStruct& CharData );
	virtual int DeleteCharAck(int Result ,UINT64 AccountID ,UINT64 CharID );
	virtual int GetCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );
	virtual int SaveCharDataAck(int Result ,UINT64 AccountID ,UINT64 CharID );

protected:
	//CPlayerCharOperatorMsgHandler
	virtual int GetCharList();
	virtual int CreateChar(CSmartStruct& CharCreateData );
	virtual int DeleteChar(UINT64 CharID );
	virtual int SelectChar(UINT64 CharID );

protected:	

	virtual int NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	virtual int DeleteMapObject(UINT64 ObjectID );
	virtual int MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& MoveStatus );
	virtual int EventMsgPacket(CSmartStruct& EventMsg );




};
