#pragma once

class CMapArea;

class CMapEventNode :
	public CNameObject
{
public:
	static CEasyArray<OBJECT_ID>			m_BroadcastIDBuffer;
	static CEasyArray<CMapObject *>			m_BroadcastPtrBuffer;
	
protected:	
	CMapArea *								m_pMapArea;	
	int										m_GridType;
	int										m_GridX;
	int										m_GridY;
	CStaticMap<UINT64,CMapObject *>			m_MapObjectList;
		
public:
	CMapEventNode(void);
	~CMapEventNode(void);

	bool Create(CMapArea * pMapArea,const CMainConfig::POOL_CONFIG& ObjectPoolConfig,UINT GridX,UINT GridY);
	

	UINT GetGridX();
	UINT GetGridY();


	bool Enter(CMapObject * pMapObject,UINT Dir);
	bool Leave(CMapObject * pMapObject,UINT Dir);

	CMapEventNode * MoveTo(CMapObject * pMapObject,UINT Dir,UINT MoveType);

	int Update(int ProcessLimit);
	
	//int PushMsgToCache(MSG_ID_TYPE MsgID,CSmartStruct& DataPacket,int Level);
	//int PushMsgPacketToCache(CSmartStruct& MsgPacket,int Level);
	
	void DoMoveStatusNotify(CMapObject * pMapObject,bool IsForce);
	//bool SendEventMsgPacketRaw(CSmartStruct& Packet);
	bool SendEventMsgPacket(CSmartStruct& Packet);
	

	UINT GetMapObjectCount();	
	void FetchObjects(CEasyArray<OBJECT_ID>& NoMsgCachedIDList,CEasyArray<CMapObject *>& MsgCachedObjectList);

	LPVOID GetFirstMapObjectPos();
	CMapObject * GetNextMapObject(LPVOID& Pos);
protected:
	bool IsObjectExist(CMapObject * pMapObject);
	void DoObjectEnterNotify(CMapObject * pMapObject,UINT Dir);
	void DoObjectLeaveNotify(CMapObject * pMapObject,UINT Dir);

	void NotifyAppearSingleToAround(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids);
	void NotifyDisappearSingleToAround(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids);

	void NotifyAppearAroundToSingle(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids);
	void NotifyDisappearAroundToSingle(CMapObject * pMapObject,MOVE_NOTIFY_GRIDS * pNotifyGrids);
	
};


inline UINT CMapEventNode::GetGridX()
{
	return m_GridX;
}
inline UINT CMapEventNode::GetGridY()
{
	return m_GridY;
}