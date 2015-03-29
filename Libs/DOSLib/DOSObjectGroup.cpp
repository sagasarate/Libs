/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectGroup.cpp                                       */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO(CDOSObjectGroup,CEasyThread);

CDOSObjectGroup::CDOSObjectGroup(void)
{
	FUNCTION_BEGIN;
	m_pManager=NULL;
	m_Index=0;
	m_Weight=0;	
	m_StatObjectCPUCost=false;
	FUNCTION_END;
}

CDOSObjectGroup::~CDOSObjectGroup(void)
{
	FUNCTION_BEGIN;
	Destory();
	FUNCTION_END;
}

bool CDOSObjectGroup::Initialize(CDOSObjectManager * pManager,UINT Index)
{
	FUNCTION_BEGIN;
	m_pManager=pManager;
	m_Index=Index;
	UINT MaxObjectCount=m_pManager->GetServer()->GetConfig().MaxGroupObjectCount;
	if(MaxObjectCount<=0)
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::Initialize:最大对象数错误"));
		return false;
	}
	if(!m_ObjectRegisterQueue.Create(MaxObjectCount))
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::Initialize:创建%u大小的注册队列失败"));
		return false;
	}
	if(!m_ObjectUnregisterQueue.Create(MaxObjectCount))
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::Initialize:创建%u大小的注销队列失败"));
		return false;
	}
	if(!m_ObjectPool.Create(MaxObjectCount))
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::Initialize:创建%u大小的对象池失败"));
		return false;
	}

	m_StatObjectCPUCost=m_pManager->GetServer()->GetConfig().StatObjectCPUCost;

	m_ObjectCountStatMap.Create(128,32,32);
	return true;
	FUNCTION_END;
	return false;
}

void CDOSObjectGroup::Destory()
{
	FUNCTION_BEGIN;
	DOS_OBJECT_REGISTER_INFO ObjectRegisterInfo;
	while(m_ObjectRegisterQueue.PopFront(ObjectRegisterInfo))
	{
		ObjectRegisterInfo.pObject->Destory();
		SAFE_RELEASE(ObjectRegisterInfo.pObject);
	}
	m_ObjectUnregisterQueue.Clear();
	void * Pos=m_ObjectPool.GetLastObjectPos();
	while(Pos)
	{		
		DOS_OBJECT_INFO *pObjectInfo=m_ObjectPool.GetPrev(Pos);
		if(pObjectInfo)
		{
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
		}
	}
	m_ObjectPool.Clear();
	FUNCTION_END;
}


BOOL CDOSObjectGroup::RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;
	ObjectRegisterInfo.pObject->AddUseRef();
	if(m_ObjectRegisterQueue.PushBack(ObjectRegisterInfo))
	{
		m_Weight+=ObjectRegisterInfo.Weight;		
		return TRUE;
	}
	else
	{
		SAFE_RELEASE(ObjectRegisterInfo.pObject);
		return FALSE;
	}
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSObjectGroup::UnregisterObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	return m_ObjectUnregisterQueue.PushBack(ObjectID)!=NULL;
	FUNCTION_END;
	return FALSE;
}

UINT CDOSObjectGroup::GetMaxObjectMsgQueueLen()
{
	CAutoLock Lock(m_EasyCriticalSection);

	UINT MaxLen=0;
	void * Pos=m_ObjectPool.GetFirstObjectPos();
	while(Pos)
	{
		DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetNext(Pos);
		if(pObjectInfo)
		{
			UINT Len=pObjectInfo->pObject->GetMsgQueueLen();
			if(Len>MaxLen)
				MaxLen=Len;
		}
	}
	return MaxLen;
}

BOOL CDOSObjectGroup::OnStart()
{
	FUNCTION_BEGIN;
	if(!CEasyThread::OnStart())
		return FALSE;

	m_ThreadPerformanceCounter.Init(GetThreadHandle(),THREAD_CPU_COUNT_TIME);

	PrintDOSLog(0xff0000,_T("对象组[%d]线程[%u]已启动"),m_Index,GetThreadID());
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSObjectGroup::OnRun()
{
	FUNCTION_BEGIN;
	
	if(!CEasyThread::OnRun())
		return FALSE;

	int ProcessCount=0;
	

	ProcessCount+=ProcessObjectRegister();	

	void * Pos=m_ObjectPool.GetFirstObjectPos();
	while(Pos)
	{
		DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetNext(Pos);
		if(pObjectInfo)
		{
			UINT64 CPUCost=0;
			if(m_StatObjectCPUCost)
				CPUCost=CEasyTimerEx::GetTimeOrigin();

			ProcessCount+=pObjectInfo->pObject->DoCycle();	

			if(m_StatObjectCPUCost)
			{
				CPUCost=CEasyTimerEx::GetTimeOrigin()-CPUCost;
				AddObjectCPUCost(pObjectInfo->ObjectID,CPUCost);
			}
		}
	}

	ProcessCount+=ProcessObjectUnregister();

	m_ThreadPerformanceCounter.DoPerformanceCount();
	
	
	if(ProcessCount==0)
	{
		DoSleep(1);
	}
	
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDOSObjectGroup::OnTerminate()
{
	FUNCTION_BEGIN;
	Destory();
	FUNCTION_END;
}

BOOL CDOSObjectGroup::PushMessage(OBJECT_ID ObjectID,CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	if(ObjectID.ObjectIndex==BROAD_CAST_OBJECT_INDEX)
	{
		void * Pos=m_ObjectPool.GetFirstObjectPos();
		while(Pos)
		{
			DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetNext(Pos);
			if(pObjectInfo)
			{
				if(ObjectID.ObjectTypeID==BROAD_CAST_OBJECT_TYPE_ID||
					pObjectInfo->ObjectID.ObjectTypeID==ObjectID.ObjectTypeID)
				{
					pObjectInfo->pObject->PushMessage(pPacket);
				}
				
			}
		}		
		return TRUE;
	}
	else
	{
		DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetObject(ObjectID.ObjectIndex);
		if(pObjectInfo)
		{
			return pObjectInfo->pObject->PushMessage(pPacket);
		}
	}
	FUNCTION_END;
	return FALSE;
}

void CDOSObjectGroup::PrintObjectStat(UINT LogChannel)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	UINT64 TotalCost=0;
	void * Pos=m_ObjectCountStatMap.GetSortedFirstObjectPos();
	while(Pos)
	{
		OBJECT_ID ObjectID;
		OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.GetSortedNextObject(Pos,ObjectID);
		if(pInfo)
		{
			CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
				_T("组%u:对象0x%llX=%u,CPUCost=%lld"),m_Index,ObjectID,pInfo->Count,pInfo->CPUCost);
			TotalCost+=pInfo->CPUCost;
			pInfo->CPUCost=0;
		}
	}
	CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
		_T("组%u:总计对象CPUCost=%lld"),m_Index,TotalCost);
	FUNCTION_END;
}

int CDOSObjectGroup::ProcessObjectRegister(int ProcessLimit)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	int ProcessCount=0;
	DOS_OBJECT_REGISTER_INFO ObjectRegisterInfo;
	while(m_ObjectRegisterQueue.PopFront(ObjectRegisterInfo))
	{
		UINT ID;
		DOS_OBJECT_INFO * pObjectInfo=NULL;
		ID=m_ObjectPool.NewObject(&pObjectInfo);
		if(pObjectInfo)
		{			
			pObjectInfo->ObjectID=ObjectRegisterInfo.ObjectID;
			pObjectInfo->ObjectID.GroupIndex=m_Index;
			pObjectInfo->ObjectID.ObjectIndex=ID;
			OnObjectRegister(pObjectInfo->ObjectID);
			pObjectInfo->Weight=ObjectRegisterInfo.Weight;
			pObjectInfo->Param=ObjectRegisterInfo.Param;
			pObjectInfo->pObject=ObjectRegisterInfo.pObject;
			pObjectInfo->pObject->SetObjectID(pObjectInfo->ObjectID);
			pObjectInfo->pObject->SetGroup(this);
			if(!pObjectInfo->pObject->Init(ObjectRegisterInfo))
			{
				UnregisterObject(pObjectInfo->ObjectID);
			}
		}
		else
		{
			PrintDOSLog(0,_T("CDOSObjectGroup::ProcessObjectRegister:对象池已耗尽%u/%u"),m_ObjectPool.GetObjectCount(),m_ObjectPool.GetBufferSize());
			m_Weight-=ObjectRegisterInfo.Weight;
			ObjectRegisterInfo.pObject->Destory();
			SAFE_RELEASE(ObjectRegisterInfo.pObject);
		}		
		ProcessLimit--;
		ProcessCount++;
		if(ProcessLimit<=0)
			break;
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}
int CDOSObjectGroup::ProcessObjectUnregister(int ProcessLimit)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	int ProcessCount=0;
	OBJECT_ID UnregisterObjectID;	
	while(m_ObjectUnregisterQueue.PopFront(UnregisterObjectID))
	{
		DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetObject(UnregisterObjectID.ObjectIndex);
		if(pObjectInfo)
		{
			OnObjectUnregister(UnregisterObjectID);
			m_Weight-=pObjectInfo->Weight;
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
			m_ObjectPool.DeleteObject(UnregisterObjectID.ObjectIndex);
		}
		else
		{
			PrintDOSLog(0,_T("注销对象时，对象[%llX]无法找到"),UnregisterObjectID.ID);
		}
		ProcessLimit--;
		ProcessCount++;
		if(ProcessLimit<=0)
			break;
	}
	FUNCTION_END;
	return 0;
}

void CDOSObjectGroup::OnObjectRegister(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	ObjectID.ObjectIndex=0;
	OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.Find(ObjectID);
	if(pInfo)
	{
		pInfo->Count++;
	}
	else
	{
		m_ObjectCountStatMap.New(ObjectID,&pInfo);
		if(pInfo)
		{
			pInfo->Count=1;
			pInfo->CPUCost=0;
		}
	}
	FUNCTION_END;
}
void CDOSObjectGroup::OnObjectUnregister(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	ObjectID.ObjectIndex=0;
	OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.Find(ObjectID);
	if(pInfo)
	{
		pInfo->Count--;
		if(pInfo->Count<0)
			pInfo->Count=0;
	}
	else
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::OnObjectUnregister:对象注销异常"));
	}
	FUNCTION_END;
}

void CDOSObjectGroup::AddObjectCPUCost(OBJECT_ID ObjectID,UINT64 CPUCost)
{
	FUNCTION_BEGIN;
	ObjectID.ObjectIndex=0;
	OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.Find(ObjectID);
	if(pInfo)
	{
		pInfo->CPUCost+=CPUCost;		
	}
	else
	{
		PrintDOSLog(0,_T("CDOSObjectGroup::AddObjectCPUCost:无法找到对象[0x%llX]的信息"),ObjectID);
	}
	FUNCTION_END;
}