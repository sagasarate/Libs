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
	m_pManager = NULL;
	m_Index = 0;
	m_Type = OBJECT_GROUP_TYPE_NORMAL;
	m_Status = STATUS_NONE;
	m_Weight = 0;
	m_ObjRegisterCost = 0;
	m_ObjUnregisterCost = 0;
	m_TimerCost = 0;
	m_AdditionalUpdateCost = 0;
	m_ObjectRegisterQueue.SetTag(_T("CDOSObjectGroup"));
	m_ObjectUnregisterQueue.SetTag(_T("CDOSObjectGroup"));
	m_ObjectPool.SetTag(_T("CDOSObjectGroup"));
	m_ObjectCountStatMap.SetTag(_T("CDOSObjectGroup"));
	FUNCTION_END;
}

CDOSObjectGroup::~CDOSObjectGroup(void)
{
	FUNCTION_BEGIN;
	Destory();
	FUNCTION_END;
}

bool CDOSObjectGroup::Initialize(CDOSObjectManager* pManager, UINT Index, OBJECT_GROUP_TYPE Type)
{
	FUNCTION_BEGIN;
	m_pManager = pManager;
	m_Index = Index;
	m_Type = Type;
	m_Config = m_pManager->GetServer()->GetConfig().ObjectConfig;
	if (m_Config.ObjectPoolSetting.StartSize <= 0)
	{
		PrintDOSLog(_T("对象池设置错误"));
		return false;
	}
	if (m_ObjectRegisterQueue.Create(m_Config.ObjectRegisterQueueSize))
	{
		PrintDOSDebugLog(_T("创建[%u]大小的注册队列成功"), m_Config.ObjectRegisterQueueSize);
	}
	else
	{
		PrintDOSLog(_T("创建[%u]大小的注册队列失败"), m_Config.ObjectRegisterQueueSize);
		return false;
	}
	if (m_ObjectUnregisterQueue.Create(m_Config.ObjectRegisterQueueSize))
	{
		PrintDOSDebugLog(_T("创建[%u]大小的注销队列成功"), m_Config.ObjectRegisterQueueSize);
	}
	else
	{
		PrintDOSLog(_T("创建[%u]大小的注销队列失败"), m_Config.ObjectRegisterQueueSize);
		return false;
	}
	if (m_ObjectPool.Create(m_Config.ObjectPoolSetting))
	{
		PrintDOSDebugLog(_T("创建[%u,%u,%u]大小的对象池成功"),
			m_Config.ObjectPoolSetting.StartSize, m_Config.ObjectPoolSetting.GrowSize, m_Config.ObjectPoolSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog(_T("创建[%u,%u,%u]大小的对象池失败"),
			m_Config.ObjectPoolSetting.StartSize, m_Config.ObjectPoolSetting.GrowSize, m_Config.ObjectPoolSetting.GrowLimit);
		return false;
	}
	if (m_TimerQueue.Create(m_Config.TimerQueueSetting))
	{
		PrintDOSDebugLog(_T("创建[%u,%u,%u]大小的计时器队列成功"),
			m_Config.TimerQueueSetting.StartSize, m_Config.TimerQueueSetting.GrowSize, m_Config.TimerQueueSetting.GrowLimit);
	}
	else
	{
		PrintDOSLog(_T("创建[%u,%u,%u]大小的计时器队列失败"),
			m_Config.TimerQueueSetting.StartSize, m_Config.TimerQueueSetting.GrowSize, m_Config.TimerQueueSetting.GrowLimit);
		return false;
	}

	if (m_Config.StatObjectCPUCost)
		m_ObjectCountStatMap.Create(128, 32, 32);

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


bool CDOSObjectGroup::RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;

	ObjectRegisterInfo.pObject->AddUseRef();
	if (m_ObjectRegisterQueue.PushBack(&ObjectRegisterInfo))
	{
		m_Weight += ObjectRegisterInfo.Weight;
		return true;
	}
	else
	{
		SAFE_RELEASE(ObjectRegisterInfo.pObject);
		return false;
	}
	FUNCTION_END;
	return false;
}

bool CDOSObjectGroup::UnregisterObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	return m_ObjectUnregisterQueue.PushBack(&ObjectID) != NULL;
	FUNCTION_END;
	return false;
}

UINT CDOSObjectGroup::GetMaxObjectMsgQueueLen()
{
	CAutoLock Lock(m_EasyCriticalSection);

	UINT MaxLen = 0;
	void* Pos = m_ObjectPool.GetFirstObjectPos();
	while (Pos)
	{
		DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetNextObject(Pos);
		if (pObjectInfo)
		{
			UINT Len = pObjectInfo->pObject->GetMsgQueueLen();
			if (Len > MaxLen)
				MaxLen = Len;
		}
	}
	return MaxLen;
}

bool CDOSObjectGroup::OnStart()
{
	FUNCTION_BEGIN;
	if (!CEasyThread::OnStart())
		return false;

	m_ThreadPerformanceCounter.Init(GetThreadHandle(), THREAD_CPU_COUNT_TIME);

	for (DOS_GROUP_INIT_FN InitFN : m_Config.DOSGroupInitFNList)
	{
		InitFN((int)m_Index);
	}

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.SetTargetThread(this);
		m_GuardThread.SetKeepAliveTime(m_Config.GuardThreadKeepAliveTime, m_Config.GuardThreadKeepAliveCount);
		m_GuardThread.Start();
		PrintDOSLog(_T("对象组[%d]守护线程已启动，KeepAliveTime=%u,KeepAliveCount=%u"),
			m_Index, m_Config.GuardThreadKeepAliveTime, m_Config.GuardThreadKeepAliveCount);
	}

	m_Status = STATUS_WORKING;

	PrintDOSLog(_T("对象组[%d]线程[%u]已启动"), m_Index, GetThreadID());
	return true;
	FUNCTION_END;
	return false;
}

bool CDOSObjectGroup::OnRun()
{
	FUNCTION_BEGIN;

	//if(!CEasyThread::OnRun())
	//	return false;

	if (m_Config.EnableGuardThread)
	{
		m_GuardThread.MakeKeepAlive();
	}

	int ProcessLimit = m_Config.ObjectGroupProcessLimit;
	int ProcessCount = 0;

	switch (m_Status)
	{
	case STATUS_WORKING:
		if (m_Config.StatObjectCPUCost)
		{
			UINT64 RegisterCost = CEasyTimerEx::GetTime();
			ProcessCount += ProcessObjectRegister(ProcessLimit);
			RegisterCost = CEasyTimerEx::GetTime() - RegisterCost;
			AtomicAdd(&m_ObjRegisterCost, RegisterCost);

			void* Pos = m_ObjectPool.GetFirstObjectPos();
			while (Pos)
			{
				DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetNextObject(Pos);
				if (pObjectInfo)
				{
					CDOSBaseObject* pObject = pObjectInfo->pObject;

					UINT64 CPUCost = 0;
					UINT64 MsgProcCost = 0;
					UINT64 COTestCost = 0;
					UINT64 UpdateCost = 0;

					CPUCost = CEasyTimerEx::GetTime();

					MsgProcCost = CEasyTimerEx::GetTime();
					ProcessCount += pObject->ProcessMessage();
					MsgProcCost = CEasyTimerEx::GetTime() - MsgProcCost;

					COTestCost = CEasyTimerEx::GetTime();
					ProcessCount += pObject->DoConcernedObjectTest();
					COTestCost = CEasyTimerEx::GetTime() - COTestCost;

					UpdateCost = CEasyTimerEx::GetTime();
					if (pObjectInfo->ProcessLimit)
						ProcessCount += pObject->Update((int)pObjectInfo->ProcessLimit);
					else
						ProcessCount += pObject->Update(ProcessLimit);
					UpdateCost = CEasyTimerEx::GetTime() - UpdateCost;

					CPUCost = CEasyTimerEx::GetTime() - CPUCost;
					AddObjectCPUCost(pObjectInfo->ObjectID, CPUCost, MsgProcCost, COTestCost, UpdateCost);
				}
			}
			UINT64 TimerCost = CEasyTimerEx::GetTime();
			ProcessCount += ProcessTimer(ProcessLimit);
			TimerCost = CEasyTimerEx::GetTime() - TimerCost;
			AtomicAdd(&m_TimerCost, TimerCost);

			UINT64 UnregisterCost = CEasyTimerEx::GetTime();
			ProcessCount += ProcessObjectUnregister(ProcessLimit);
			UnregisterCost = CEasyTimerEx::GetTime() - UnregisterCost;
			AtomicAdd(&m_ObjUnregisterCost, UnregisterCost);

			UINT64 AdditionalUpdateCost = CEasyTimerEx::GetTime();
			for (DOS_GROUP_UPDATE_FN UpdateFN : m_Config.DOSGroupUpdateFNList)
			{
				UpdateFN((int)m_Index, ProcessLimit);
			}
			AdditionalUpdateCost = CEasyTimerEx::GetTime() - AdditionalUpdateCost;
			AtomicAdd(&m_AdditionalUpdateCost, AdditionalUpdateCost);
		}
		else
		{
			ProcessCount += ProcessObjectRegister(ProcessLimit);

			void* Pos = m_ObjectPool.GetFirstObjectPos();
			while (Pos)
			{
				DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetNextObject(Pos);
				if (pObjectInfo)
				{
					CDOSBaseObject* pObject = pObjectInfo->pObject;
					ProcessCount += pObject->ProcessMessage();
					ProcessCount += pObject->DoConcernedObjectTest();
					if (pObjectInfo->ProcessLimit)
						ProcessCount += pObject->Update((int)pObjectInfo->ProcessLimit);
					else
						ProcessCount += pObject->Update(ProcessLimit);
				}
			}
			ProcessCount += ProcessTimer(ProcessLimit);
			ProcessCount += ProcessObjectUnregister(ProcessLimit);
			for (DOS_GROUP_UPDATE_FN UpdateFN : m_Config.DOSGroupUpdateFNList)
			{
				UpdateFN((int)m_Index, ProcessLimit);
			}
		}
		break;
	case STATUS_SUSPENDING:
		m_Status = STATUS_SUSPENDED;
		break;
	}
	if (m_Config.StatObjectCPUCost)
	{
		if (m_Config.UseRealGroupLoadWeight)
		{
			if (m_GroupWeightUpdateTimer.IsTimeOut(GROUP_WEIGHT_UPDATE_TIME))
			{
				m_GroupWeightUpdateTimer.SaveTime();
				AdjustObjectWeights();
			}
		}

		m_ThreadPerformanceCounter.DoPerformanceCount();
	}

	if (ProcessCount == 0)
	{
		DoSleep(DEFAULT_IDLE_SLEEP_TIME);
	}

	return true;
	FUNCTION_END;
	return false;
}

void CDOSObjectGroup::OnTerminate()
{
	FUNCTION_BEGIN;

	if (m_Config.EnableGuardThread)
		m_GuardThread.SafeTerminate();

	CAutoLock Lock(m_EasyCriticalSection);

	DOS_OBJECT_REGISTER_INFO ObjectRegisterInfo;
	while (m_ObjectRegisterQueue.PopFront(&ObjectRegisterInfo))
	{
		ObjectRegisterInfo.pObject->Destory();
		SAFE_RELEASE(ObjectRegisterInfo.pObject);
	}
	m_ObjectUnregisterQueue.Clear();

	void* Pos = m_ObjectPool.GetLastObjectPos();
	while (Pos)
	{
		DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetPrevObject(Pos);
		if (pObjectInfo)
		{
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
		}
	}
	m_ObjectPool.Clear();
	m_Weight = 0;

	for (DOS_GROUP_DESTORY_FN DestoryFN : m_Config.DOSGroupDestoryFNList)
	{
		DestoryFN((int)m_Index);
	}

	m_Status = STATUS_NONE;
	FUNCTION_END;
}

bool CDOSObjectGroup::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket* pPacket)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	if (ObjectID.ObjectIndex == BROAD_CAST_OBJECT_INDEX)
	{
		void* Pos = m_ObjectPool.GetFirstObjectPos();
		while (Pos)
		{
			DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetNextObject(Pos);
			if (pObjectInfo)
			{
				if (ObjectID.ObjectTypeID == BROAD_CAST_OBJECT_TYPE_ID ||
					pObjectInfo->ObjectID.ObjectTypeID == ObjectID.ObjectTypeID)
				{
					pObjectInfo->pObject->PushMessage(pPacket);
				}

			}
		}
		return true;
	}
	else
	{
		DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetObject(ObjectID.ObjectIndex);
		if (pObjectInfo)
		{
			return pObjectInfo->pObject->PushMessage(pPacket);
		}
	}
	FUNCTION_END;
	return false;
}

void CDOSObjectGroup::PrintObjectStat(UINT LogChannel)
{
	FUNCTION_BEGIN;
	CAutoLock Lock(m_EasyCriticalSection);

	UINT64 TotalCost = 0;
	void* Pos = m_ObjectCountStatMap.GetSortedFirstObjectPos();
	while (Pos)
	{
		UINT ObjectType;
		OBJECT_STAT_INFO* pInfo = m_ObjectCountStatMap.GetSortedNextObject(Pos, ObjectType);
		if (pInfo)
		{
			CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, NULL,
				_T("组%u:对象0x%02X(%s)=%u,CPUCost=%lldns,MsgProcCost=%lldns,COTestCost=%lldns,UpdateCost=%lldns,ExecCount=%u,UnitCost=%0.2fns"),
				m_Index, ObjectType, (LPCSTR)pInfo->ObjectTypeName, pInfo->Count,
				pInfo->CPUCost, pInfo->MsgProcCost, pInfo->COTestCost, pInfo->UpdateCost,
				pInfo->ExecCount,
				pInfo->ExecCount ? (float)pInfo->CPUCost / (float)pInfo->ExecCount : 0.0f);
			TotalCost += pInfo->CPUCost;
			pInfo->CPUCost = 0;
			pInfo->MsgProcCost = 0;
			pInfo->COTestCost = 0;
			pInfo->UpdateCost = 0;
			pInfo->ExecCount = 0;
			if (pInfo->Count == 0)
			{
				//如果该对象的数量为0，删除统计数据，下一次日志就不输出了
				m_ObjectCountStatMap.Delete(ObjectType);
			}
		}
	}
	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0,
		_T("组%u:总计对象CPUCost=%lldns,对象注册消耗=%lldns,对象注销消耗=%lldns,定时器消耗=%lldns,额外更新消耗=%lldns"),
		m_Index, TotalCost, m_ObjRegisterCost, m_ObjUnregisterCost, m_TimerCost, m_AdditionalUpdateCost);
	m_ObjRegisterCost = 0;
	m_ObjUnregisterCost = 0;
	m_TimerCost = 0;
	m_AdditionalUpdateCost = 0;
	FUNCTION_END;
}

int CDOSObjectGroup::ProcessObjectRegister(int ProcessLimit)
{
	FUNCTION_BEGIN;


	int ProcessCount = 0;
	DOS_OBJECT_REGISTER_INFO ObjectRegisterInfo;
	while (m_ObjectRegisterQueue.PopFront(&ObjectRegisterInfo))
	{
		UINT ID;
		DOS_OBJECT_INFO* pObjectInfo = NULL;
		{
			CAutoLock Lock(m_EasyCriticalSection);
			ID = m_ObjectPool.NewObject(&pObjectInfo);
		}
		if (pObjectInfo)
		{
			pObjectInfo->ObjectID = ObjectRegisterInfo.ObjectID;
			pObjectInfo->ObjectID.GroupIndex = m_Index;
			pObjectInfo->ObjectID.ObjectIndex = ID;
			pObjectInfo->Weight = ObjectRegisterInfo.Weight;
			pObjectInfo->ProcessLimit = ObjectRegisterInfo.MsgProcessLimit;
			//pObjectInfo->Param=ObjectRegisterInfo.Param;
			pObjectInfo->pObject = ObjectRegisterInfo.pObject;
			pObjectInfo->pObject->SetObjectID(pObjectInfo->ObjectID);
			pObjectInfo->pObject->SetGroup(this);
			if (pObjectInfo->pObject->Init(ObjectRegisterInfo))
			{
				OnObjectRegister(pObjectInfo->ObjectID, ObjectRegisterInfo.ObjectTypeName, ObjectRegisterInfo.Weight);
			}
			else
			{
				m_Weight -= pObjectInfo->Weight;
				pObjectInfo->pObject->Destory();
				SAFE_RELEASE(pObjectInfo->pObject);
				{
					CAutoLock Lock(m_EasyCriticalSection);
					m_ObjectPool.DeleteObject(ID);
				}
			}
		}
		else
		{
			PrintDOSLog(_T("对象池已耗尽%u/%u"), m_ObjectPool.GetObjectCount(), m_ObjectPool.GetBufferSize());
			m_Weight -= ObjectRegisterInfo.Weight;
			ObjectRegisterInfo.pObject->Destory();
			SAFE_RELEASE(ObjectRegisterInfo.pObject);
		}
		ProcessLimit--;
		ProcessCount++;
		if (ProcessLimit <= 0)
			break;
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}
int CDOSObjectGroup::ProcessObjectUnregister(int ProcessLimit)
{
	FUNCTION_BEGIN;

	int ProcessCount = 0;
	OBJECT_ID UnregisterObjectID;
	while (m_ObjectUnregisterQueue.PopFront(&UnregisterObjectID))
	{
		DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetObject(UnregisterObjectID.ObjectIndex);
		if (pObjectInfo)
		{
			OnObjectUnregister(UnregisterObjectID, pObjectInfo->Weight);
			m_Weight -= pObjectInfo->Weight;
			pObjectInfo->pObject->Destory();
			SAFE_RELEASE(pObjectInfo->pObject);
			//移除该对象注册的定时器
			void* Pos = m_TimerQueue.GetFirstTimerPos();
			while (Pos)
			{
				TIMER_DATA TimerData;
				UINT ID = m_TimerQueue.GetNextTimer(Pos, &TimerData);
				if (TimerData.ObjectID == pObjectInfo->ObjectID)
				{
					m_TimerQueue.DeleteTimer(ID, NULL);
					pObjectInfo->pObject->OnTimerRelease(ID, TimerData.Param);
					break;
				}
			}
			{
				CAutoLock Lock(m_EasyCriticalSection);
				m_ObjectPool.DeleteObject(UnregisterObjectID.ObjectIndex);
			}
		}
		else
		{
			PrintDOSLog(_T("注销对象时，对象[%llX]无法找到"), UnregisterObjectID.ID);
		}
		ProcessLimit--;
		ProcessCount++;
		if (ProcessLimit <= 0)
			break;
	}
	FUNCTION_END;
	return 0;
}

int CDOSObjectGroup::ProcessTimer(int ProcessLimit)
{
	FUNCTION_BEGIN;
	int ProcessCount = 0;
	UINT TimerID;
	TIMER_DATA TimerData;
	bool IsRepeat = false;
	while (TimerID = m_TimerQueue.UpdateTimer(&TimerData, &IsRepeat))
	{
		DOS_OBJECT_INFO* pObjectInfo = m_ObjectPool.GetObject(TimerData.ObjectID.ObjectIndex);
		if (pObjectInfo)
		{
			pObjectInfo->pObject->OnTimer(TimerID, TimerData.Param, IsRepeat);
			if (!IsRepeat)
			{
				pObjectInfo->pObject->OnTimerRelease(TimerID, TimerData.Param);
			}
		}
		else
		{
			m_TimerQueue.DeleteTimer(TimerID, NULL);
			PrintDOSLog(_T("对象[0x%llX]无法找到"), TimerData.ObjectID);
		}
		ProcessCount++;
		ProcessLimit--;
		if (ProcessLimit <= 0)
			break;
	}
	return ProcessCount;
	FUNCTION_END;
	return 0;
}


void CDOSObjectGroup::OnObjectRegister(OBJECT_ID ObjectID, LPCSTR szObjectTypeName, int Weight)
{
	FUNCTION_BEGIN;
	OBJECT_STAT_INFO* pInfo = m_ObjectCountStatMap.Find(ObjectID.ObjectTypeID);
	if (pInfo)
	{
		pInfo->Count++;
		pInfo->Weight += Weight;
	}
	else
	{
		m_ObjectCountStatMap.New(ObjectID.ObjectTypeID, &pInfo);
		if (pInfo)
		{
			pInfo->ObjectTypeName = szObjectTypeName;
			pInfo->Count = 1;
			pInfo->CPUCost = 0;
			pInfo->MsgProcCost = 0;
			pInfo->COTestCost = 0;
			pInfo->UpdateCost = 0;
			pInfo->ExecCount = 0;
			pInfo->Weight = Weight;
		}
	}
	FUNCTION_END;
}
void CDOSObjectGroup::OnObjectUnregister(OBJECT_ID ObjectID, int Weight)
{
	FUNCTION_BEGIN;
	OBJECT_STAT_INFO* pInfo = m_ObjectCountStatMap.Find(ObjectID.ObjectTypeID);
	if (pInfo)
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

void CDOSObjectGroup::AddObjectCPUCost(OBJECT_ID ObjectID, UINT64 CPUCost, UINT64 MsgProcCost, UINT64 COTestCost, UINT64 UpdateCost)
{
	FUNCTION_BEGIN;
	OBJECT_STAT_INFO* pInfo = m_ObjectCountStatMap.Find(ObjectID.ObjectTypeID);
	if (pInfo)
	{
		pInfo->ExecCount++;
		pInfo->CPUCost += CPUCost;
		pInfo->MsgProcCost += MsgProcCost;
		pInfo->COTestCost += COTestCost;
		pInfo->UpdateCost += UpdateCost;
	}
	else
	{
		PrintDOSLog(_T("无法找到对象[0x%llX]的信息"), ObjectID);
	}
	FUNCTION_END;
}

void CDOSObjectGroup::AdjustObjectWeights()
{
	m_Weight = 0;
	void* Pos = m_ObjectCountStatMap.GetSortedFirstObjectPos();
	while (Pos)
	{
		OBJECT_STAT_INFO* pInfo = m_ObjectCountStatMap.GetSortedNextObject(Pos);
		if (pInfo)
		{
			pInfo->Weight = (int)(pInfo->CPUCost / 100);
			m_Weight += pInfo->Weight;
		}
	}
}
