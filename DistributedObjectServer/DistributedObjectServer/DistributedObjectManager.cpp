#include "StdAfx.h"


CDistributedObjectManager::CDistributedObjectManager(void)
{
	FUNCTION_BEGIN;
	m_pDOSObjectManager=NULL;
	FUNCTION_END;
}

CDistributedObjectManager::~CDistributedObjectManager(void)
{
}

BOOL CDistributedObjectManager::Init(CDOSObjectManager * pDOSObjectManager,UINT MaxObjectCount)
{
	FUNCTION_BEGIN;
	m_pDOSObjectManager=pDOSObjectManager;
	if(!m_ObjectPool.Create(MaxObjectCount))
	{
		Log("无法创建%u大小的插件对象池",MaxObjectCount);
		return FALSE;
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDistributedObjectManager::Destory()
{
	FUNCTION_BEGIN;
	m_pDOSObjectManager=NULL;
	m_ObjectPool.Destory();
	FUNCTION_END;
}

BOOL CDistributedObjectManager::RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;
	if(m_pDOSObjectManager&&ObjectRegisterInfo.pObject)
	{
		CDistributedObjectOperator * pObjectOperator=CreateObjectOperator(ObjectRegisterInfo.pObject,ObjectRegisterInfo.ObjectID);
		if(pObjectOperator)
		{
			DOS_OBJECT_REGISTER_INFO RegisterInfo;

			RegisterInfo.ObjectID=ObjectRegisterInfo.ObjectID;
			RegisterInfo.pObject=pObjectOperator;
			RegisterInfo.Weight=ObjectRegisterInfo.Weight;
			RegisterInfo.ObjectGroupIndex=ObjectRegisterInfo.ObjectGroupIndex;
			RegisterInfo.MsgQueueSize=ObjectRegisterInfo.MsgQueueSize;
			RegisterInfo.MsgProcessLimit=ObjectRegisterInfo.MsgProcessLimit;
			RegisterInfo.Param=ObjectRegisterInfo.Param;
			
			if(m_pDOSObjectManager->RegisterObject(RegisterInfo))
			{
				//LogDebug("CDistributedObjectManager::RegisterObject:成功注册对象0x%llX",
				//	pObjectOperator->GetObjectID().ID);
				return TRUE;
			}
			else
			{
				Log("CDistributedObjectManager::RegisterObject:注册对象0x%llX失败",
					ObjectRegisterInfo.ObjectID.ID);
			}
		}
		
	}
	
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectManager::UnregisterObject(CDistributedObjectOperator * pObjectOperator)
{
	FUNCTION_BEGIN;
	if(m_pDOSObjectManager&&pObjectOperator)
	{
		
		if(m_pDOSObjectManager->UnregisterObject(pObjectOperator->GetObjectID()))
		{
			//LogDebug("CDistributedObjectManager::UnregisterObject:成功注销对象0x%llX",
			//	pObjectOperator->GetObjectID().ID);
			return TRUE;
		}
		else
		{
			Log("CDistributedObjectManager::UnregisterObject:注销对象0x%llX失败",
				pObjectOperator->GetObjectID().ID);
		}
	}
	FUNCTION_END;
	return FALSE;
}

CDistributedObjectOperator * CDistributedObjectManager::CreateObjectOperator(IDistributedObject * pObject,OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	CDistributedObjectOperator * pObjectOperator=NULL;
	UINT ID=m_ObjectPool.NewObject(&pObjectOperator);
	if(pObjectOperator)
	{
		if(pObjectOperator->Init(this,ID,pObject))
		{
			//LogDebug("CDistributedObjectManager::CreateObjectOperator:成功新建对象%u[%llX],现有对象%u",ID,ObjectID.ID,m_ObjectPool.GetObjectCount());
			return pObjectOperator;
		}
		else
		{
			m_ObjectPool.DeleteObject(ID);
		}
	}
	LogDebug("CDistributedObjectManager::CreateObjectOperator:新建对象失败,现有对象%u",m_ObjectPool.GetObjectCount());
	FUNCTION_END;	
	return NULL;

}

BOOL CDistributedObjectManager::DeleteObjectOperator(CDistributedObjectOperator * pObjectOperator)
{
	FUNCTION_BEGIN;	
	UINT ID=pObjectOperator->GetPoolID();
	
	if(m_ObjectPool.DeleteObject(ID))
	{
		//LogDebug("CDistributedObjectManager::CreateObjectOperator:删除对象%u,现有对象%u",ID,m_ObjectPool.GetObjectCount());
		return TRUE;
	}
	else
	{
		LogDebug("CDistributedObjectManager::CreateObjectOperator:删除对象%u失败,现有对象%u",ID,m_ObjectPool.GetObjectCount());
	}
	FUNCTION_END;
	return FALSE;
}

void CDistributedObjectManager::PrintObjectCount()
{
	Log("一共有%u个插件对象",
		m_ObjectPool.GetObjectCount());
}