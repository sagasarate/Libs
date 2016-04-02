#pragma once


class CCharacter :
	public CNameObject,
	public IDistributedObject,
	public CCharacterMsgHandler,
	public CMapAreaAckMsgHandler
{
protected:	
	struct FLY_PREPARE_INFO
	{
		bool	IsInFly;
		UINT	MapID;
		float	PosX;
		float	PosY;
		float	Height;
		float	Direction;
	};
	IDistributedObjectOperator *	m_pOperator;
	OBJECT_ID						m_MapID;
	OBJECT_ID						m_CharQueryCenterID;
	OBJECT_ID						m_AreaEventReceiverID;
	CMoveStatus						m_MoveStatus;
	CBaseCharInfo					m_BaseCharInfo;

	FLY_PREPARE_INFO				m_FlyPrepareInfo;

	DECLARE_DOS_MSG_MAP(CCharacter)
public:
	CCharacter(void);
	virtual ~CCharacter(void);

	BOOL Init();

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();
	virtual UINT AddUseRef();
	virtual void Release();

	void ForceRelease();

	BOOL AddCharToMap(OBJECT_ID MapID,OBJECT_ID CharQueryCenterID,OBJECT_ID AreaEventReceiverID,UINT MapObjectFlag,UINT64 CharID,LPCTSTR CharName,UINT CharType,float XPos,float YPos,float Height,float Direction);

protected:
	virtual int Update(int ProcessPacketLimit);
protected:

	virtual int QueryCharInfo(OBJECT_ID RequesterID );
	virtual int QueryMove(BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction );
	virtual int AliveTest();
	virtual int QueryFly(UINT MapID ,float PosX ,float PosY ,float Height ,float Direction );
	virtual int MapChangeConfirm(int Result );
	virtual int QueryJump();

	virtual int RegisterMapObjectAck(UINT64 ObjectID ,int Result );
	virtual int UnregisterMapObjectAck(UINT64 ObjectID ,int Result );
	virtual int UpdateMoveStatusAck(UINT64 ObjectID ,int Result );
	virtual int SendEventMsgPacketAck(UINT64 ObjectID ,int Result );
	virtual int MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& CharData );
	virtual int EventMsgPacket(CSmartStruct& EventMsg );
	virtual int HideOnMapAck(UINT64 ObjectID ,BYTE HideType ,int Result );
	virtual int TeleportInMapAck(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetDir ,int Result );
	
	
};


