#pragma once

class CPlayerCharDataDBAdapter :
	public IDistributedObject,
	public CPlayerCharDBMsgHandler
{
protected:
	IDistributedObjectOperator *			m_pOperator;
	IDatabase *								m_pDatabase;
	CDBTransationManager *					m_pDBTransManager;

	UINT64									m_CharIDSeed;
	CEasyTimer								m_CharIDSeedFetchTimer;

	DECLARE_DOS_MSG_MAP(CPlayerCharDataDBAdapter)
public:
	CPlayerCharDataDBAdapter(void);
	virtual ~CPlayerCharDataDBAdapter(void);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();

protected:
	virtual int Update(int ProcessPacketLimit);

protected:
	bool UnpackCharCreateInfo(const CSmartStruct& Packet,CHAR_CREATE_INFO& CreateInfo);
	bool PackPlayerCharInfo(CSmartStruct& Packet,const BASE_CHAR_INFO& BaseCharInfo);
	bool PackPlayerCharInfo(CSmartStruct& Packet,const PLAYER_CHAR_INFO& PlayerCharInfo);
	bool UnpackPlayerCharInfo(const CSmartStruct& Packet,PLAYER_CHAR_INFO& PlayerCharInfo);
	bool PackPlayerCharInfoList(CSmartStruct& Packet,const CEasyArray<BASE_CHAR_INFO>& CharList);

protected:
	virtual int AccountLogin(LPCTSTR szAccountName ,LPCTSTR szPassword ,LPCTSTR szClientIP );
	virtual int AccountLogout(UINT64 AccountID );
	virtual int GetAccountChar(UINT64 AccountID );
	virtual int CreateChar(UINT64 AccountID ,CSmartStruct& CharData );
	virtual int DeleteChar(UINT64 AccountID ,UINT64 CharID );
	virtual int GetCharData(UINT64 AccountID ,UINT64 CharID );
	virtual int SaveCharData(UINT64 AccountID ,UINT64 CharID ,CSmartStruct& CharData );

public:
	void OnDBRetGetCharIDSeed(UINT64 CharIDSeed);
	void OnDBRetAccountLogin(OBJECT_ID CallerID,LPCTSTR szAccountName,UINT64 AccountID);
	void OnDBRetCreateChar(OBJECT_ID CallerID,int Result,PLAYER_CHAR_INFO& PlayerCharInfo);
	void OnDBRetGetAccountChar(OBJECT_ID CallerID,int Result,UINT64 AccountID,const CEasyArray<BASE_CHAR_INFO> CharList);
	void OnDBRetGetCharData(OBJECT_ID CallerID,int Result,const PLAYER_CHAR_INFO& CharInfo);
	void OnDBRetSaveCharData(OBJECT_ID CallerID,int Result,const PLAYER_CHAR_INFO& CharInfo);
};
