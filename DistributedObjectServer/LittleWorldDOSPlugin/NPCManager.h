#pragma once

class CNPCManager :
	public CNameObject,
	public IDistributedObject,
	public CMapAreaAckMsgHandler
{
protected:	
	IDistributedObjectOperator *			m_pOperator;

	CThreadSafeIDStorage<CBaseNPC>			m_NPCPool;
	OBJECT_ID								m_CurMapID;
	OBJECT_ID								m_CharQueryCenterID;
	CEasyTimer								m_FindTimer;

	DECLARE_DOS_MSG_MAP(CNPCManager)
public:
	CNPCManager(void);
	virtual ~CNPCManager(void);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();

	BOOL CreateNPC(UINT64 CharID,LPCTSTR CharName,float XPos,float YPos,float Height,float Direction);
	BOOL DeleteNPC(UINT PoolID);

protected:
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	virtual void OnObjectReport(OBJECT_ID ObjectID,const CSmartStruct& ObjectInfo);
	virtual int Update(int ProcessPacketLimit);

	
	
	
};
