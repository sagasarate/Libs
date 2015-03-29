#include "StdAfx.h"


CMapCollisionNode::CMapCollisionNode(void)
{
	m_pMapArea=NULL;
	ZeroMemory(&m_Links,sizeof(m_Links));
	m_GridType=LT_MOVE;
	m_GridX=0;
	m_GridY=0;
}

CMapCollisionNode::~CMapCollisionNode(void)
{
}

bool CMapCollisionNode::Create(CMapArea * pMapArea,UINT GridX,UINT GridY)
{
	//if(!m_MapObjectList.Create(MaxMapObject))
	//{
	//	return false;
	//}
	m_pMapArea=pMapArea;	
	m_GridX=GridX;
	m_GridY=GridY;	
	return true;
}

void CMapCollisionNode::SetLink(UINT Dir,int Type,CMapCollisionNode * pLinkNode)
{
	if(Dir<LD_MAX)
	{
		m_Links[Dir].LinkType=Type;
		m_Links[Dir].pLinkNode=pLinkNode;
	}
}

CMapCollisionNode * CMapCollisionNode::MoveTo(CMapObject * pMapObject,UINT Dir,UINT MoveType)
{	
	if(Dir<LD_MAX)
	{
		if(GetLinkType(Dir)==LT_MOVE||GetLinkType(Dir)==LT_FALL)
		{
			UINT OppositeDir=GetOppositeDir(Dir);
			CMapCollisionNode * pNewNode=GetLinkNode(Dir);
			if(pNewNode->GetGridType()!=LT_NONE)
			{
				Leave(pMapObject);
				Enter(pMapObject);
				return pNewNode;
			}			
		}
	}
	return NULL;
}

bool CMapCollisionNode::Enter(CMapObject * pMapObject)
{
	if(pMapObject==NULL)
		return false;
	//PrintSystemLog(0,"对象%llu进入碰撞格[%u,%u]",
	//	pMapObject->GetObjectID(),m_GridX,m_GridY);

	if(GetGridType()!=LT_NONE)
	{
		return true;

		//if(m_MapObjectList.Find(pMapObject->GetObjectID()))
		//{
		//	Log("CMapCollisionNode::Enter:对象%llu已经在碰撞格中",pMapObject->GetObjectID());
		//	return false;
		//}
		//if(m_MapObjectList.Insert(pMapObject->GetObjectID(),pMapObject))
		//{			
		//	return true;
		//}
		//else
		//{
		//	Log("CMapCollisionNode::Enter:碰撞格对象列表已满",pMapObject->GetObjectID());
		//}
	}	
	return false;
}

bool CMapCollisionNode::Leave(CMapObject * pMapObject)
{
	if(pMapObject==NULL)
		return false;
	//PrintSystemLog(0,"对象%llu离开碰撞格[%u,%u]",
	//	pMapObject->GetObjectID(),m_GridX,m_GridY);	
	return true;
	//if(m_MapObjectList.Delete(pMapObject->GetObjectID()))
	//{		
	//	return true;			
	//}
	//else
	//{
	//	Log("CMapCollisionNode::Leave:对象%llu未在事件格中",pMapObject->GetObjectID());
	//}
	//return false;
}


bool CMapCollisionNode::CanEnter()
{
	if(GetGridType()!=LT_NONE)
	{
		return true;		
	}	
	return false;
}