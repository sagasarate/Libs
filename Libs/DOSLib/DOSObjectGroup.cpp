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
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CDOSObjectGroup, CEasyThread);

CDOSObjectGroup::CDOSObjectGroup(void)
{
	FUNCTION_BEGIN;
	m_pManager=NULL;
	m_Index=0;
	m_Status = STATUS_NONE;
	m_Weight=0;
	m_StatObjectCPUCost=false;
	m_TotalGroupCost = 0;
	m_UseRealGroupLoadWeight = false;
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
	const STORAGE_POOL_SETTING& PoolSetting = m_pManager->GetServer()->GetConfig().ObjectPoolSetting;
	if (PoolSetting.StartSize <= 0)
	{
		PrintDOSLog(_T("对象池设置错误"));
		return false;
	}
	if (m_ObjectRegisterQueue.Create(PoolSetting))
	{
		PrintDOSDebugLog( _T("创建[%u,%u,%u]大小的注册队列成功"),
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog( _T("创建[%u,%u,%u]大小的注册队列失败"), 
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
		return false;
	}
	if (m_ObjectUnregisterQueue.Create(PoolSetting))
	{
		PrintDOSDebugLog(_T("创建[%u,%u,%u]大小的注销队列成功"),
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog( _T("创建[%u,%u,%u]大小的注销队列失败"), 
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
		return false;
	}
	if (m_ObjectPool.Create(PoolSetting))
	{
		PrintDOSDebugLog(_T("创建[%u,%u,%u]大小的对象池成功"),
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog( _T("创建[%u,%u,%u]大小的对象池失败"), 
			PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
		return false;
	}

	m_StatObjectCPUCost=m_pManager->GetServer()->GetConfig().StatObjectCPUCost;
	m_UseRealGroupLoadWeight = m_pManager->GetServer()->GetConfig().UseRealGroupLoadWeight;

	m_ObjectCountStatMap.Create(128,32,32);

	m_GroupWeightUpdateTimer.SaveTime();

	return true;
	FUNCTION_END;
	return false;
}

void CDOSObjectGroup::Destory()
{
	FUNCTION_BEGIN;
	
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
		DOS_OBJECT_INFO * pObjectInfo=m_ObjectPool.GetNextObject(Pos);
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

	DOS_GROUP_INIT_FN pDOSGroupInitFN = GetManager()->GetServer()->GetConfig().pDOSGroupInitFN;
	if (pDOSGroupInitFN)
	{
		pDOSGroupInitFN(m_Index);
	}
	m_Status = STATUS_WORKING;

	PrintDOSLog(_T("对象组[%d]线程[%u]已启动"),m_Index,GetThreadID());
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CDOSObjectGroup::OnRun()
{
	FUNCTION_BEGIN;

	//if(!CEasyThread::OnRun())
	//	return FALSE;

	int ProcessCount=0;	

	switch (m_Status)
	{
	case STATUS_WORKING:
		{
			ProcessCount += ProcessObjectRegister();

			void * Pos = m_ObjectPool.GetFirstObjectPos();
			while (Pos)
			{
				DOS_OBJECT_INFO * pObjectInfo = m_ObjectPool.GetNextObject(Pos);
				if (pObjectInfo)
				{
					UINT64 CPUCost = 0;
					if (m_StatObjectCPUCost)
						CPUCost = CEasyTimerEx::GetTime();

					ProcessCount += pObjectInfo->pObject->DoCycle();

					if (m_StatObjectCPUCost)
					{
						CPUCost = CEasyTimerEx::GetTime() - CPUCost;
						AddObjectCPUCost(pObjectInfo->ObjectID, CPUCost);
					}
				}
			}

			ProcessCount += ProcessObjectUnregister();
		}
		break;
	case STATUS_SUSPENDING:
		{			
			m_Status = STATUS_SUSPENDED;
		}
		break;
	}
	if (m_UseRealGroupLoadWeight)
	{
		if (m_GroupWeightUpdateTimer.IsTimeOut(GROUP_WEIGHT_UPDATE_TIME))
		{
			m_GroupWeightUpdateTimer.SaveTime();
			AdjustObjectWeights();
		}
		
	}

	m_ThreadPerformanceCounter.DoPerformanceCount();


	if(ProcessCount==0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}

	return TRUE;
	FUNCTION_END;
	return FALSE;
}

void CDOSObjectGroup::OnTerminate()
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	DOS_OBJECT_REGISTER_INFO ObjectRegisterInfo;
	while (m_ObjectRegisterQueue.PopFront(ObjectRegisterInfo))
	{
		ObjectRegisterInfo.pObject->Destory();
		SAFE_RELEASE(ObjectRegisterInfo.pObject);
	}
	m_ObjectUnregisterQueue.Clear();

	void * Pos = m_ObjectPool.GetLastObjectPos();
	while (Pos)
	{
		DOS_OBJECT_INFO *pObjectInfo = m_ObjectPool.GetPrevObject(Pos);
		if (pObjectInfo)
		{
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
		}
	}
	m_ObjectPool.Clear();
	m_Weight = 0;

	DOS_GROUP_DESTORY_FN pDOSGroupDestoryFN = GetManager()->GetServer()->GetConfig().pDOSGroupDestoryFN;
	if (pDOSGroupDestoryFN)
	{
		pDOSGroupDestoryFN(m_Index);
	}

	m_Status = STATUS_NONE;
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
			DOS_OBJECT_INFO * pObjectInfo = m_ObjectPool.GetNextObject(Pos);
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
			CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, NULL,
				_T("组%u:对象0x%llX(%s)=%u,CPUCost=%lldns,ExecCount=%u,UnitCost=%0.2fns"),
				m_Index, ObjectID, (LPCSTR)pInfo->ObjectTypeName, pInfo->Count, pInfo->CPUCost, pInfo->ExecCount,
				pInfo->ExecCount ? (float)pInfo->CPUCost / (float)pInfo->ExecCount : 0.0f);
			TotalCost+=pInfo->CPUCost;			
			pInfo->CPUCost=0;
			pInfo->ExecCount = 0;
			if (pInfo->Count == 0)
			{
				//如果该对象的数量为0，删除统计数据，下一次日志就不输出了
				m_ObjectCountStatMap.Delete(ObjectID);
			}
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
			pObjectInfo->Weight=ObjectRegisterInfo.Weight;
			//pObjectInfo->Param=ObjectRegisterInfo.Param;
			pObjectInfo->pObject=ObjectRegisterInfo.pObject;
			pObjectInfo->pObject->SetObjectID(pObjectInfo->ObjectID);
			pObjectInfo->pObject->SetGroup(this);
			if (pObjectInfo->pObject->Init(ObjectRegisterInfo))
			{
				OnObjectRegister(pObjectInfo->ObjectID, ObjectRegisterInfo.szObjectTypeName, ObjectRegisterInfo.Weight);
			}
			else
			{
				m_Weight -= pObjectInfo->Weight;
				pObjectInfo->pObject->Destory();
				SAFE_RELEASE(pObjectInfo->pObject);
				m_ObjectPool.DeleteObject(ID);
			}
		}
		else
		{
			PrintDOSLog(_T("对象池已耗尽%u/%u"),m_ObjectPool.GetObjectCount(),m_ObjectPool.GetBufferSize());
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
			OnObjectUnregister(UnregisterObjectID, pObjectInfo->Weight);
			m_Weight-=pObjectInfo->Weight;
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
			m_ObjectPool.DeleteObject(UnregisterObjectID.ObjectIndex);
		}
		else
		{
			PrintDOSLog(_T("注销对象时，对象[%llX]无法找到"),UnregisterObjectID.ID);
		}
		ProcessLimit--;
		ProcessCount++;
		if(ProcessLimit<=0)
			break;
	}
	FUNCTION_END;
	return 0;
}


void CDOSObjectGroup::OnObjectRegister(OBJECT_ID ObjectID, LPCSTR szObjectTypeName, int Weight)
{
	FUNCTION_BEGIN;
	ObjectID.ObjectIndex=0;
	OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.Find(ObjectID);
	if(pInfo)
	{
		pInfo->Count++;
		pInfo->Weight += Weight;
	}
	else
	{
		m_ObjectCountStatMap.New(ObjectID,&pInfo);
		if(pInfo)
		{
			pInfo->ObjectTypeName = szObjectTypeName;
			pInfo->Count=1;
			pInfo->CPUCost=0;
			pInfo->ExecCount = 0;
			pInfo->Weight = Weight;
		}
	}
	FUNCTION_END;
}
void CDOSObjectGroup::OnObjectUnregister(OBJECT_ID ObjectID, int Weight)
{
	FUNCTION_BEGIN;
	ObjectID.ObjectIndex=0;
	OBJECT_STAT_INFO * pInfo=m_ObjectCountStatMap.Find(ObjectID);
	if(pInfo)
	{
		pInfo->Count--;
		if (pInfo->Count < 0)
			pInfo->Count = 0;
		pInfo->Weight -= Weight;
		if (pInfo->Weight < 0)
			pInfo->Weight = 0;
	}
	else
	{
		PrintDOSLog(_T("对象注销异常"));
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
		pInfo->ExecCount++;
		pInfo->CPUCost+=CPUCost;
	}
	else
	{
		PrintDOSLog(_T("无法找到对象[0x%llX]的信息"),ObjectID);
	}
	FUNCTION_END;
}

void CDOSObjectGroup::AdjustObjectWeights()
{
	m_Weight = 0;
	void * Pos = m_ObjectCountStatMap.GetSortedFirstObjectPos();
	while (Pos)
	{
		OBJECT_ID ObjectID;
		OBJECT_STAT_INFO * pInfo = m_ObjectCountStatMap.GetSortedNextObject(Pos, ObjectID);
		if (pInfo)
		{
			pInfo->Weight = (int)(pInfo->CPUCost / 100);
			m_Weight += pInfo->Weight;
		}
	}
}