#pragma once

class CMapCollisionNode :
	public CNameObject
{
protected:
	struct NODE_LD_INFO
	{
		int					LinkType;
		CMapCollisionNode *	pLinkNode;
	};
	CMapArea *								m_pMapArea;
	//CStaticMap<UINT64,CMapObject *>			m_MapObjectList;
	NODE_LD_INFO							m_Links[LD_MAX];
	int										m_GridType;
	int										m_GridX;
	int										m_GridY;

public:
	CMapCollisionNode(void);
	~CMapCollisionNode(void);

	bool Create(CMapArea * pMapArea,UINT GridX,UINT GridY);

	void SetLink(UINT Dir,int Type,CMapCollisionNode * pLinkNode);
	int GetLinkType(UINT Dir);
	CMapCollisionNode * GetLinkNode(UINT Dir);

	UINT GetGridX();
	UINT GetGridY();
	int GetGridType();

	CMapCollisionNode * MoveTo(CMapObject * pMapObject,UINT Dir,UINT MoveType);
	bool Enter(CMapObject * pMapObject);
	bool Leave(CMapObject * pMapObject);

	bool CanEnter();
};

inline int CMapCollisionNode::GetLinkType(UINT Dir)
{
	if(Dir<LD_MAX)
	{
		return m_Links[Dir].LinkType;
	}
	return LT_NONE;
}

inline CMapCollisionNode * CMapCollisionNode::GetLinkNode(UINT Dir)
{
	if(Dir<LD_MAX)
	{
		return m_Links[Dir].pLinkNode;
	}
	return NULL;
}

inline UINT CMapCollisionNode::GetGridX()
{
	return m_GridX;
}
inline UINT CMapCollisionNode::GetGridY()
{
	return m_GridY;
}
inline int CMapCollisionNode::GetGridType()
{
	return m_GridType;
}