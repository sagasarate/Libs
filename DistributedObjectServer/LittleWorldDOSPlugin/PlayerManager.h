#pragma once


class CPlayerManager :
	public CNameObject,
	public IDistributedObject,
	public CPlayerManagerMsgHandler,
	public CMapAreaAckMsgHandler,
	public CPlayerCharDBAckMsgHandler
{
protected:
	IDistributedObjectOperator *		m_pOperator;
	OBJECT_ID							m_CurMapID;
	OBJECT_ID							m_CharQueryCenterID;
	OBJECT_ID							m_PlayerCharDBObjectID;
	CEasyTimer							m_FindTimer;

	CThreadSafeIDStorage<CPlayer>		m_PlayerPool;

	DECLARE_DOS_MSG_MAP(CPlayerManager)
public:
	CPlayerManager(void);
	virtual ~CPlayerManager(void);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();

	BOOL CreatePlayer(OBJECT_ID ClientProxyID,LPCTSTR szAccountName,LPCTSTR szPassword);
	BOOL DeletePlayer(UINT PoolID);
	
protected:
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnObjectReport(OBJECT_ID ObjectID,const CSmartStruct& ObjectInfo);
	virtual int Update(int ProcessPacketLimit);

	virtual int PlayerLogin(LPCTSTR szAccountName ,LPCTSTR szPassword );

	
};

