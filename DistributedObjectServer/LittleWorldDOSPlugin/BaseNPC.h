#pragma once

class CNPCManager;

class CBaseNPC :
	public CCharacter
{
protected:
	enum AI_STATUS
	{
		AI_STATUS_IDLE,
		AI_STATUS_MOVE,
		AI_STATUS_JUMP,
	};	
	CNPCManager *						m_pManager;
	UINT								m_PoolID;
	BYTE								m_SendedMoveStatus;
	UINT64								m_TargetCharID;
	CMoveStatus							m_TargetMoveStatus;
	CEasyTimer							m_StatusUpdateTimer;

	int									m_AIStatus;
	CEasyTimer							m_AIStatusTimer;

	float								m_BornPosX;
	float								m_BornPosY;

public:
	CBaseNPC(void);
	virtual ~CBaseNPC(void);

	BOOL Init(CNPCManager * pManager,UINT PoolID,OBJECT_ID MapID,UINT64 CharID,LPCTSTR CharName,float XPos,float YPos,float Height,float Direction);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();

	void ForceRelease();
protected:
	
	virtual int Update(int ProcessPacketLimit);

	virtual int RegisterMapObjectAck(UINT64 ObjectID ,int Result );
	virtual int UnregisterMapObjectAck(UINT64 ObjectID ,int Result );
	virtual int UpdateMoveStatusAck(UINT64 ObjectID ,int Result );
	virtual int NewMapObject(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	virtual int DeleteMapObject(UINT64 ObjectID );
	virtual int MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& CharData );

	
protected:
	float IdentityRadian(float Radian);
	
	float TraceTarget(UINT CurTime);

	
	void SendMoveStatus(bool SendMove,bool SendRotation);

	void SelectMoveTarget(UINT CurTime);
	void DoJump();

};


