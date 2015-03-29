#pragma once


class CMapArea :
	public IDistributedObject,public CMapAreaMsgHandler
{
public:
	

protected:	
	IDistributedObjectOperator *	m_pOperator;
	CStaticMap<UINT64,CMapObject>	m_ObjectPool;
	MOVE_NOTIFY_GRIDS				m_MoveGrids[LD_MAX];
	MOVE_NOTIFY_GRIDS				m_FullNotifyGrids;

	UINT							m_EventBroadcastSize;
	float							m_EventGridSize;
	float							m_CollisionGridSize;

	float							m_MapStartPosX;
	float							m_MapStartPosY;
	float							m_MapWidth;
	float							m_MapHeight;

	CEasyArray<CMapEventNode>		m_pMapEventGrids;
	UINT							m_MapEventGridWidth;
	UINT							m_MapEventGridHeight;

	CEasyArray<CMapCollisionNode>	m_pMapCollisionGrids;
	UINT							m_MapCollisionGridWidth;
	UINT							m_MapCollisionGridHeight;

	CEasyTimer						m_MapNodeUpdateTimer;

	DECLARE_DOS_MSG_MAP(CMapArea)
public:
	CMapArea(void);
	virtual ~CMapArea(void);

	virtual BOOL Initialize(IDistributedObjectOperator * pOperator);
	virtual void Destory();

protected:
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID);
	
	virtual int Update(int ProcessPacketLimit);

	void OnPlayerEnter(OBJECT_ID PlayerID);
	void OnPlayerLeave(OBJECT_ID PlayerID);


	CMapObject * CreateMapObject(UINT64 ObjectID,OBJECT_ID ControllerID,OBJECT_ID AreaEventReceiverID,CSmartStruct& MoveStatusPacket,UINT Flag);
	CMapObject * GetMapObject(UINT64 ObjectID);	
	BOOL DeleteMapObject(CMapObject * pMapObject);
	
	virtual int RegisterMapObject(UINT64 ObjectID ,OBJECT_ID ControllerID ,OBJECT_ID AreaEventReceiverID ,CSmartStruct& MoveStatus ,UINT Flag );
	virtual int UnregisterMapObject(UINT64 ObjectID );
	virtual int UpdateMoveStatus(UINT64 ObjectID ,CSmartStruct& MoveStatus );
	virtual int SendEventMsgPacket(UINT64 ObjectID ,CSmartStruct& EventMsgPacket );
	virtual int HideOnMap(UINT64 ObjectID ,BYTE HideType );
	virtual int TeleportInMap(UINT64 ObjectID ,float TargetPosX ,float TargetPosY ,float TargetHeight ,float TargetDir );
		
public:	

	BOOL SendMsg(OBJECT_ID TargetID,MSG_ID_TYPE MsgID,LPCVOID pData,UINT DataSize);

	IDistributedObjectOperator * GetOperator();
	CMapEventNode * GetMapEventNode(UINT GridX,UINT GridY);
	CMapCollisionNode * GetMapCollisionNode(UINT GridX,UINT GridY);

	UINT GetEventBroadcastSize();
	float GetEventGridSize();
	float GetCollisionGridSize();

	UINT PosXToCollisionGridX(float PosX);
	UINT PosYToCollisionGridY(float PosY);
	UINT PosXToEventGridX(float PosX);
	UINT PosYToEventGridY(float PosY);

	float CollisionGridXToPosX(UINT GridX);
	float CollisionGridYToPosY(UINT GridY);
	float EventGridXToPosX(UINT GridX);
	float EventGridYToPosY(UINT GridY);
	bool CanMoveEventGrid(float CurPosX,float CurPosY,UINT CurGridX,UINT CurGridY);

	bool IsPosInMap(float PosX,float PosY);

	MOVE_NOTIFY_GRIDS * GetMoveGrids(UINT Dir);
	MOVE_NOTIFY_GRIDS * GetFullNotifyGrids();
	
	

protected:
	
	void CreateMapGrid();
	void InitNotifyGrids();

};

inline IDistributedObjectOperator * CMapArea::GetOperator()
{
	return m_pOperator;
}


inline CMapEventNode * CMapArea::GetMapEventNode(UINT GridX,UINT GridY)
{
	if(GridX<m_MapEventGridWidth&&GridY<m_MapEventGridHeight)
	{
		return m_pMapEventGrids.GetObject(GridY*m_MapEventGridWidth+GridX);
	}
	else
	{
		//Log("CMapArea::GetMapEventNode:非法的事件格坐标[%u,%u],合法范围[%u,%u]",
		//	GridX,GridY,m_MapEventGridWidth,m_MapEventGridHeight);
	}
	return NULL;
}

inline CMapCollisionNode * CMapArea::GetMapCollisionNode(UINT GridX,UINT GridY)
{
	if(GridX<m_MapCollisionGridWidth&&GridY<m_MapCollisionGridHeight)
	{
		return m_pMapCollisionGrids.GetObject(GridY*m_MapCollisionGridWidth+GridX);
	}
	else
	{
		//Log("CMapArea::GetMapCollisionNode:非法的事件格坐标[%u,%u],合法范围[%u,%u]",
		//	GridX,GridY,m_MapCollisionGridWidth,m_MapCollisionGridHeight);
	}
	return NULL;
}


inline UINT CMapArea::GetEventBroadcastSize()
{
	return m_EventBroadcastSize;
}
inline float CMapArea::GetEventGridSize()
{
	return m_EventGridSize;
}
inline float CMapArea::GetCollisionGridSize()
{
	return m_CollisionGridSize;
}
inline UINT CMapArea::PosXToCollisionGridX(float PosX)
{
	return (UINT)((PosX-m_MapStartPosX)/m_CollisionGridSize);
}
inline UINT CMapArea::PosYToCollisionGridY(float PosY)
{
	return (UINT)((PosY-m_MapStartPosY)/m_CollisionGridSize);
}
inline UINT CMapArea::PosXToEventGridX(float PosX)
{
	return (UINT)((PosX-m_MapStartPosX)/m_EventGridSize);
}
inline UINT CMapArea::PosYToEventGridY(float PosY)
{
	return (UINT)((PosY-m_MapStartPosY)/m_EventGridSize);
}

inline float CMapArea::CollisionGridXToPosX(UINT GridX)
{
	return m_MapStartPosX+GridX*m_CollisionGridSize;
}
inline float CMapArea::CollisionGridYToPosY(UINT GridY)
{
	return m_MapStartPosY+GridY*m_CollisionGridSize;
}
inline float CMapArea::EventGridXToPosX(UINT GridX)
{
	return m_MapStartPosX+GridX*m_EventGridSize;
}
inline float CMapArea::EventGridYToPosY(UINT GridY)
{
	return m_MapStartPosY+GridY*m_EventGridSize;
}

inline bool CMapArea::CanMoveEventGrid(float CurPosX,float CurPosY,UINT CurGridX,UINT CurGridY)
{
	float DX=CurPosX-EventGridXToPosX(CurGridX);
	float DY=CurPosY-EventGridYToPosY(CurGridY);
	if(DX<-2.0f||DX>m_EventGridSize+2.0f||
		DY<-2.0f||DY>m_EventGridSize+2.0f)
	{
		return true;
	}
	return false;
}

inline bool CMapArea::IsPosInMap(float PosX,float PosY)
{
	if(PosX>=m_MapStartPosX&&PosX<=m_MapStartPosX+m_MapWidth&&
		PosY>=m_MapStartPosY&&PosY<=m_MapStartPosY+m_MapHeight)
	{
		return true;
	}
	return false;
}