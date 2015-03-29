#pragma once

class CMapArea;
class CMapEventNode;
class CMapCollisionNode;

class CMapObject :
	public CNameObject
{
protected:
	CMapArea *					m_pMapArea;
	UINT						m_Flag;
	UINT64						m_ObjectID;
	OBJECT_ID					m_ControllerID;
	OBJECT_ID					m_AreaEventReceiverID;		
	CMoveStatus					m_MoveStatus;
	
	CMapEventNode *				m_pMapEventNode;
	CMapCollisionNode * 		m_pMapCollisionNode;

	CSmartStruct				m_MsgBuffer;

	CEasyArray<MAP_GRID_POS>	m_MoveGridBuffer;

	UINT						m_MoveUpdateTimer;
	UINT						m_MsgUpdateTimer;
	
public:
	CMapObject(void);
	~CMapObject(void);

	bool Init(CMapArea * pMapArea,UINT64 ObjectID,OBJECT_ID ControllerID,OBJECT_ID AreaEventReceiverID,CSmartStruct& MoveStatusPacket,UINT Flag);
	void Destory();

	bool UpdateMoveStatus(CSmartStruct& MoveStatusPacket);
	

	int Update(UINT CurTime);

	bool PushMsg(MSG_ID_TYPE MsgID,CSmartStruct& DataPacket,int Level);
	bool PushMsgPacket(CSmartStruct& MsgPacket,int Level);

	UINT64 GetObjectID();
	OBJECT_ID GetControllerID();
	OBJECT_ID GetAreaEventReceiverID();

	UINT GetFlag();
	void SetFlag(UINT Flag);
	void ModifyFlag(UINT AddFlag,UINT RemoveFlag);
	bool CheckFlag(UINT Flag);

	CMapEventNode * GetMapEventNode();
	CMapCollisionNode * GetMapCollisionNode();

	bool PacketObjectInfo(CSmartStruct& Packet);
	bool PacketMoveStatus(CSmartStruct& Packet);

	bool HideOnMap(BYTE HideType);
	bool Teleport(float PosX,float PosY,float Height,float Direction);


protected:
	bool MoveTo(float PosX,float PosY);
	void StopMove();
	void StopMove(float PosX,float PosY,float Height);
	bool GetGridPath(UINT SrcGridX,UINT SrcGridY,UINT DestGridX,UINT DestGridY,MAP_GRID_POS * pGridBuffer,UINT& GridCount);
	void ClearMsgBuffer();
	bool DoEnterMap();
	bool DoLeaveMap();


};

inline UINT64 CMapObject::GetObjectID()
{
	return m_ObjectID;
}
inline OBJECT_ID CMapObject::GetControllerID()
{
	return m_ControllerID;
}
inline OBJECT_ID CMapObject::GetAreaEventReceiverID()
{
	return m_AreaEventReceiverID;
}
inline UINT CMapObject::GetFlag()
{
	return m_Flag;
}
inline void CMapObject::SetFlag(UINT Flag)
{
	m_Flag=Flag;
}
inline void CMapObject::ModifyFlag(UINT AddFlag,UINT RemoveFlag)
{
	m_Flag&=~RemoveFlag;
	m_Flag|=AddFlag;
}
inline bool CMapObject::CheckFlag(UINT Flag)
{
	return (m_Flag&Flag)!=0;
}
inline CMapEventNode * CMapObject::GetMapEventNode()
{
	return m_pMapEventNode;
}
inline CMapCollisionNode * CMapObject::GetMapCollisionNode()
{
	return m_pMapCollisionNode;
}