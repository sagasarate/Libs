#pragma once

class CMoveStatus
{
protected:
	UINT64		m_ModifyFlag;

	float		m_XPos;
	float		m_YPos;
	float		m_Height;
	float		m_Direction;
	BYTE		m_MoveStatus;	
	float		m_MoveSpeed;
	BYTE		m_RotateStatus;	
	float		m_RotateSpeed;
	float		m_StartXPos;
	float		m_StartYPos;
	float		m_StartHeight;
	float		m_TargetXPos;
	float		m_TargetYPos;
	float		m_TargetHeight;
	CEasyTimer	m_MoveStatusTimer;
	CEasyTimer	m_RotateStatusTimer;
	float		m_TotalMoveTime;		
public:
	CMoveStatus(void);
	~CMoveStatus(void);

	virtual void Clear();

	float GetPosX();
	float GetPosY();
	float GetHeight();
	float GetStartPosX();
	float GetStartPosY();
	float GetStartHeight();
	float GetTargetPosX();
	float GetTargetPosY();
	float GetTargetHeight();
	float GetDirection();
	BYTE GetMoveStatus();
	float GetMoveSpeed();
	BYTE GetRotateStatus();
	float GetRotateSpeed();
	UINT GetMovePastTime(UINT CurTime);
	float GetTotalMoveTime();
	
	void SetMoveStatus(UINT CurTime,CSmartStruct& Packet);
	void SetMoveStatus(UINT CurTime,BYTE MoveStatus ,float MoveSpeed ,float StartXPos ,float StartYPos ,float StartHeight,float TargetXPos ,float TargetYPos ,float TargetHeight,float Direction ,BYTE RotateStatus ,float RotateSpeed);
	void SetRotateStatus(UINT CurTime,float Direction ,BYTE RotateStatus ,float RotateSpeed);
	void StopMove(UINT CurTime);
	void StopMove(UINT CurTime,float PosX ,float PosY,float Height);
	void EnbaleMoveStatusChange(UINT CurTime);

	virtual void Update(UINT CurTime);

	virtual void GetModifyFlag(OBJECT_MEMBER_FLAGS& ModifyFlags,int GetType,const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual bool IsModified(const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual void ClearModifyFlag(const OBJECT_MEMBER_FLAGS& MemberFlags);

	virtual bool MakePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);
	virtual bool MakeUpdatePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);

	virtual bool ParsePacket(CSmartStruct& Packet,const OBJECT_MEMBER_FLAGS& MemberFlags);

protected:
	float IdentityRadian(float Radian);
};


inline float CMoveStatus::GetPosX()
{
	return m_XPos;
}
inline float CMoveStatus::GetPosY()
{
	return m_YPos;
}
inline float CMoveStatus::GetHeight()
{
	return m_Height;
}
inline float CMoveStatus::GetStartPosX()
{
	return m_StartXPos;
}
inline float CMoveStatus::GetStartPosY()
{
	return m_StartYPos;
}
inline float CMoveStatus::GetStartHeight()
{
	return m_StartHeight;
}
inline float CMoveStatus::GetTargetPosX()
{
	return m_TargetXPos;
}
inline float CMoveStatus::GetTargetPosY()
{
	return m_TargetYPos;
}
inline float CMoveStatus::GetTargetHeight()
{
	return m_TargetHeight;
}
inline float CMoveStatus::GetDirection()
{
	return m_Direction;
}
inline BYTE CMoveStatus::GetMoveStatus()
{
	return m_MoveStatus;
}
inline float CMoveStatus::GetMoveSpeed()
{
	return m_MoveSpeed;
}
inline BYTE CMoveStatus::GetRotateStatus()
{
	return m_RotateStatus;
}
inline float CMoveStatus::GetRotateSpeed()
{
	return m_RotateSpeed;
}
inline UINT CMoveStatus::GetMovePastTime(UINT CurTime)
{
	return m_MoveStatusTimer.GetPastTime(CurTime);
}
inline float CMoveStatus::GetTotalMoveTime()
{
	return m_TotalMoveTime;
}