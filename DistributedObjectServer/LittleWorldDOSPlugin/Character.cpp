#include "StdAfx.h"

START_DOS_MSG_MAP(CCharacter)
DOS_MSG_MAP(CCharacterMsgHandler)
DOS_MSG_MAP(CMapAreaAckMsgHandler)
END_DOS_MSG_MAP(CCharacter)

CCharacter::CCharacter(void)
{
	FUNCTION_BEGIN;

	INIT_DOS_MSG_MAP;

	m_pOperator=NULL;
	m_MapID=0;	
	m_AreaEventReceiverID=0;
	m_FlyPrepareInfo.IsInFly=false;
	FUNCTION_END;
}

CCharacter::~CCharacter(void)
{
	FUNCTION_BEGIN;
	
	FUNCTION_END;
}

BOOL CCharacter::Init()
{
	FUNCTION_BEGIN;	
	m_MapID=0;	
	m_AreaEventReceiverID=0;
	m_FlyPrepareInfo.IsInFly=false;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CCharacter::Initialize(IDistributedObjectOperator * pOperator)
{
	FUNCTION_BEGIN;
	m_pOperator=pOperator;	
	return TRUE;

	FUNCTION_END;
	return FALSE;
}
void CCharacter::Destory()
{
	FUNCTION_BEGIN;

	//向角色查询中心注销
	{
		CCharQueryCenterMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharQueryCenterID);

		MsgCaller.UnregisterChar(m_BaseCharInfo.GetCharID());
	}

	//注销地图对象
	CMapAreaMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);

	MsgCaller.UnregisterMapObject(m_BaseCharInfo.GetCharID());	

	m_BaseCharInfo.Clear();	

	m_pOperator=NULL;
	FUNCTION_END;
}

void CCharacter::ForceRelease()
{
	FUNCTION_BEGIN;
	if(m_pOperator)
		m_pOperator->Release();
	FUNCTION_END;
}

BOOL CCharacter::AddCharToMap(OBJECT_ID MapID,OBJECT_ID CharQueryCenterID,OBJECT_ID AreaEventReceiverID,UINT MapObjectFlag,UINT64 CharID,LPCTSTR CharName,UINT CharType,float XPos,float YPos,float Height,float Direction)
{
	FUNCTION_BEGIN;

	m_MapID=MapID;	
	m_CharQueryCenterID=CharQueryCenterID;
	m_AreaEventReceiverID=AreaEventReceiverID;
	m_FlyPrepareInfo.IsInFly=false;

	m_BaseCharInfo.SetCharID(CharID);
	m_BaseCharInfo.SetCharType(CharType);
	m_BaseCharInfo.SetCharName(CharName);

	UINT CurTime=CEasyTimer::GetTime();
	m_MoveStatus.SetMoveStatus(CurTime,MS_STOP,0,XPos,YPos,Height,XPos,YPos,Height,
		Direction,RS_STOP,0);

	//向角色查询中心注册
	{
		CCharQueryCenterMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_CharQueryCenterID);

		MsgCaller.RegisterChar(m_BaseCharInfo.GetCharID());
	}
	

	//注册地图对象
	CMapAreaMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);
	char Buffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct MoveStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);

	m_MoveStatus.MakePacket(MoveStatus,MF_MOVE_STATUS);

	return MsgCaller.RegisterMapObject(m_BaseCharInfo.GetCharID(),m_pOperator->GetObjectID(),m_AreaEventReceiverID,MoveStatus,MapObjectFlag)==COMMON_RESULT_SUCCEED;

	FUNCTION_END;
	return FALSE;
}


int CCharacter::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	UINT CurTime=CEasyTimer::GetTime();
	m_MoveStatus.Update(CurTime);
	FUNCTION_END;
	return 0;
}

int CCharacter::QueryCharInfo(OBJECT_ID RequesterID )
{
	char Buffer[MAX_PLAYER_CHAR_INFO_SIZE];
	CSmartStruct CharInfo(Buffer,MAX_PLAYER_CHAR_INFO_SIZE,true);


	m_BaseCharInfo.MakePacket(CharInfo,MF_BASE_CHAR_INFO);

	CCharacterAckMsgCaller Caller(m_pOperator,m_pOperator->GetObjectID(),RequesterID);

	Caller.QueryCharInfoAck(COMMON_RESULT_SUCCEED,CharInfo);


	return COMMON_RESULT_SUCCEED;
}
int CCharacter::QueryMove(BYTE MoveStatus ,float StartPosX ,float StartPosY ,float StartHeight ,float TargetPosX ,float TargetPosY ,float TargetHeight ,BYTE RotateStatus ,float Direction )
{
	CMapAreaMsgCaller Caller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);
	float MoveSpeed=RUN_SPEED;
	if(MoveStatus==MS_BACKWORD)
		MoveSpeed=WALK_SPEED;

	char Buffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct QueryStatus(Buffer,MAX_MOVE_STATUS_SIZE,true);
	
	QueryStatus.AddMember(SST_MS_XPOS,StartPosX);
	QueryStatus.AddMember(SST_MS_YPOS,StartPosY);
	QueryStatus.AddMember(SST_MS_HEIGHT,StartHeight);
	QueryStatus.AddMember(SST_MS_TARGET_XPOS,TargetPosX);
	QueryStatus.AddMember(SST_MS_TARGET_YPOS,TargetPosY);
	QueryStatus.AddMember(SST_MS_TARGET_HEIGHT,TargetHeight);
	QueryStatus.AddMember(SST_MS_MOVE_STATUS,MoveStatus);
	QueryStatus.AddMember(SST_MS_MOVE_SPEED,MoveSpeed);

	QueryStatus.AddMember(SST_MS_DIRECTION,Direction);		
	QueryStatus.AddMember(SST_MS_ROTATE_STATUS,RotateStatus);
	QueryStatus.AddMember(SST_MS_ROTATE_SPEED,ROTATE_SPEED);
	
	Caller.UpdateMoveStatus(m_BaseCharInfo.GetCharID(),QueryStatus);
	return COMMON_RESULT_SUCCEED;
}

int CCharacter::AliveTest()
{
	CCharacterAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),GetCurMsgSenderID());
	MsgCaller.AliveTestAck();
	return COMMON_RESULT_SUCCEED;
}

int CCharacter::QueryFly(UINT MapID ,float PosX ,float PosY ,float Height ,float Direction )
{
	CMapAreaMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);

	LogDebug("[%llu]请求传送到(%g,%g,%g)",
		m_BaseCharInfo.GetCharID(),PosX,PosY,Direction);

	m_FlyPrepareInfo.IsInFly=true;
	m_FlyPrepareInfo.MapID=MapID;
	m_FlyPrepareInfo.PosX=PosX;
	m_FlyPrepareInfo.PosY=PosY;
	m_FlyPrepareInfo.Height=Height;
	m_FlyPrepareInfo.Direction=Direction;
	MsgCaller.HideOnMap(m_BaseCharInfo.GetCharID(),MOHT_LEAVE_MAP);
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::MapChangeConfirm(int Result )
{
	LogDebug("[%llu]收到传送确认(%d)",
		m_BaseCharInfo.GetCharID(),Result);
	CMapAreaMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_MapID);
	if(Result==CMRT_OK)
	{
		MsgCaller.TeleportInMap(m_BaseCharInfo.GetCharID(),m_FlyPrepareInfo.PosX,m_FlyPrepareInfo.PosY,m_FlyPrepareInfo.Height,m_FlyPrepareInfo.Direction);
	}
	else
	{
		m_FlyPrepareInfo.IsInFly=false;
		MsgCaller.HideOnMap(m_BaseCharInfo.GetCharID(),MOHT_SHOW);
	}
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::QueryJump()
{
	CCharacterAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),0);

	char MsgDataBuffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct MsgPacket(MsgDataBuffer,MAX_MOVE_STATUS_SIZE,true);	
	MsgCaller.PackMsgQueryJumpAck(MsgPacket,m_BaseCharInfo.GetCharID(),CMRT_OK);

	char EventBuffer[MAX_MOVE_STATUS_SIZE];
	CSmartStruct EventPacket(EventBuffer,MAX_MOVE_STATUS_SIZE,true);	

	PackMsgToMsgPacket(EventPacket,
		MAKE_MSG_ID(INTERFACE_LW_CHARACTER,METHOD_CHAR_QUERY_JUMP,true),
		MsgPacket);

	CMapAreaMsgCaller MsgCaller2(m_pOperator,m_pOperator->GetObjectID(),m_MapID);

	MsgCaller2.SendEventMsgPacket(m_BaseCharInfo.GetCharID(),EventPacket);


	return COMMON_RESULT_SUCCEED;
}
int CCharacter::RegisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::UnregisterMapObjectAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::UpdateMoveStatusAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::SendEventMsgPacketAck(UINT64 ObjectID ,int Result )
{
	return COMMON_RESULT_SUCCEED;
}

int CCharacter::MoveStatusUpdate(UINT64 ObjectID ,BYTE IsForce ,CSmartStruct& CharData )
{
	if(m_BaseCharInfo.GetCharID()==ObjectID)		
	{
		UINT CurTime=CEasyTimer::GetTime();
		m_MoveStatus.SetMoveStatus(CurTime,CharData);
	}

	return COMMON_RESULT_SUCCEED;
}

int CCharacter::EventMsgPacket(CSmartStruct& EventMsg )
{
	void * Pos=EventMsg.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartStruct Value=EventMsg.GetNextMember(Pos,MemberID);
		if(MemberID==SST_GDMP_MSG)
		{
			UINT MsgID=Value.GetMember(SST_GDM_MSG_ID);
			CSmartStruct MsgPacket=Value.GetMember(SST_GDM_MSG_DATA);

			HandleMsg(MsgID,MsgPacket);
		}
	}
	return COMMON_RESULT_SUCCEED;
}

int CCharacter::HideOnMapAck(UINT64 ObjectID ,BYTE HideType ,int Result )
{
	LogDebug("[%llu]收到场景隐身请求返回%d",
		m_BaseCharInfo.GetCharID(),Result);
	if(m_FlyPrepareInfo.IsInFly)
	{
		CCharacterAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_AreaEventReceiverID);
		if(Result==HOMRT_OK)
		{
			LogDebug("[%llu]收到场景隐身成功通知客户端开始传送(%g,%g,%g)",
				m_BaseCharInfo.GetCharID(),m_FlyPrepareInfo.PosX,m_FlyPrepareInfo.PosY,m_FlyPrepareInfo.Direction);
			MsgCaller.MapChange(m_FlyPrepareInfo.MapID,m_FlyPrepareInfo.PosX,m_FlyPrepareInfo.PosY,m_FlyPrepareInfo.Direction);
		}
		else
		{
			LogDebug("[%llu]收到场景隐身请求失败",
				m_BaseCharInfo.GetCharID());
			m_FlyPrepareInfo.IsInFly=false;
			MsgCaller.QueryFlyAck(CMRT_FAILED);
		}
	}
	return COMMON_RESULT_SUCCEED;
}
int CCharacter::TeleportInMapAck(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetDir ,int Result )
{
	LogDebug("[%llu]收到传送请求返回%d",
		m_BaseCharInfo.GetCharID(),Result);
	CCharacterAckMsgCaller MsgCaller(m_pOperator,m_pOperator->GetObjectID(),m_AreaEventReceiverID);
	if(m_FlyPrepareInfo.IsInFly)
	{
		if(Result==TIMRT_OK)
		{
			LogDebug("[%llu]收到传送请求成功，通知客户端",
				m_BaseCharInfo.GetCharID());
			MsgCaller.QueryFlyAck(CMRT_OK);
		}
		else
		{
			LogDebug("[%llu]收到传送请求失败，在原地现身",
				m_BaseCharInfo.GetCharID());
			MsgCaller.QueryFlyAck(CMRT_FAILED);
			CMapAreaMsgCaller MsgCaller2(m_pOperator,m_pOperator->GetObjectID(),m_MapID);
			MsgCaller2.HideOnMap(m_BaseCharInfo.GetCharID(),MOHT_SHOW);
		}
		m_FlyPrepareInfo.IsInFly=false;
	}
	else
	{
		LogDebug("[%llu]收到传送请求成功，但未在传送状态",
			m_BaseCharInfo.GetCharID());
		if(Result==TIMRT_OK)
			MsgCaller.MapChange(0,TargetPosX,TargetPosY,TargetDir);
	}
	return COMMON_RESULT_SUCCEED;
}


