/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSObjectGroup.h                                         */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CDOSObjectGroup :
	public CEasyThread
{
protected:
	enum STATUS
	{
		STATUS_NONE,
		STATUS_WORKING,
		STATUS_SUSPENDING,
		STATUS_SUSPENDED,
	};
	struct DOS_OBJECT_INFO
	{
		OBJECT_ID			ObjectID;
		CDOSBaseObject *	pObject;
		int					Weight;
		UINT				Param;
	};
	struct OBJECT_STAT_INFO
	{
		CEasyStringA		ObjectTypeName;
		int					Count;
		int					Weight;
		UINT				ExecCount;
		volatile UINT64		CPUCost;
		volatile UINT64		MsgProcCost;
		volatile UINT64		COTestCost;
		volatile UINT64		UpdateCost;
	};
	struct TIMER_DATA
	{
		OBJECT_ID	ObjectID;
		UINT64		Param;
	};
	CDOSObjectManager *								m_pManager;
	UINT											m_Index;
	OBJECT_GROUP_TYPE								m_Type;
	volatile STATUS									m_Status;
	volatile int									m_Weight;

	DOS_OBJECT_CONFIG								m_Config;
	
	UINT64											m_TotalGroupCost;
	CEasyTimer										m_GroupWeightUpdateTimer;

	CThreadPerformanceCounter						m_ThreadPerformanceCounter;


	CThreadSafeIDStorage<DOS_OBJECT_REGISTER_INFO>	m_ObjectRegisterQueue;
	CThreadSafeIDStorage<OBJECT_ID>					m_ObjectUnregisterQueue;

	CIDStorage<DOS_OBJECT_INFO>						m_ObjectPool;

	CStaticMap<OBJECT_ID,OBJECT_STAT_INFO>			m_ObjectCountStatMap;

	CEasyCriticalSection							m_EasyCriticalSection;

	CGuardThread									m_GuardThread;
	CTimerQueue<TIMER_DATA>							m_TimerQueue;

	DECLARE_CLASS_INFO_STATIC(CDOSObjectGroup);
public:
	CDOSObjectGroup(void);
	virtual ~CDOSObjectGroup(void);

	virtual bool Initialize(CDOSObjectManager * pManager, UINT Index, OBJECT_GROUP_TYPE Type);
	virtual void Destory();

	
	CDOSObjectManager * GetManager();

	BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo);
	BOOL UnregisterObject(OBJECT_ID ObjectID);
	UINT GetObjectCount();

	int GetWeight();
	float GetCPUUsedRate();
	float GetCycleTime();
	UINT64 GetCPUUsedTime();
	UINT GetCycleCount();
	UINT GetMaxObjectMsgQueueLen();
	UINT GetIndex();
	OBJECT_GROUP_TYPE GetType();

	bool Suspend();
	bool Resume();
	bool IsWorking();


	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	BOOL PushMessage(OBJECT_ID ObjectID,CDOSMessagePacket * pPacket);

	void PrintObjectStat(UINT LogChannel);

	UINT AddTimer(OBJECT_ID ObjectID, UINT64 TimeOut, UINT64 Param, bool IsRepeat);
	BOOL DeleteTimer(UINT ID);
protected:
	int ProcessObjectRegister(int ProcessLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	int ProcessObjectUnregister(int ProcessLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	int ProcessTimer(int ProcessLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void OnObjectRegister(OBJECT_ID ObjectID, LPCSTR szObjectTypeName, int Weight);
	void OnObjectUnregister(OBJECT_ID ObjectID, int Weight);
	void AddObjectCPUCost(OBJECT_ID ObjectID, UINT64 CPUCost, UINT64 MsgProcCost, UINT64 COTestCost, UINT64 UpdateCost);
	void AdjustObjectWeights();
};

inline CDOSObjectManager * CDOSObjectGroup::GetManager()
{
	return m_pManager;
}

inline UINT CDOSObjectGroup::GetObjectCount()
{
	return m_ObjectPool.GetObjectCount();
}

inline int CDOSObjectGroup::GetWeight()
{
	return m_Weight;
}
inline float CDOSObjectGroup::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}

inline float CDOSObjectGroup::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}

inline UINT64 CDOSObjectGroup::GetCPUUsedTime()
{
	return m_ThreadPerformanceCounter.GetCycleCPUUsedTime();
}
inline UINT CDOSObjectGroup::GetCycleCount()
{
	return m_ThreadPerformanceCounter.GetCycleCount();
}
inline UINT CDOSObjectGroup::GetIndex()
{
	return m_Index;
}
inline OBJECT_GROUP_TYPE CDOSObjectGroup::GetType()
{
	return m_Type;
}

inline bool CDOSObjectGroup::Suspend()
{
	if (m_Status == STATUS_WORKING)
	{
		m_Status = STATUS_SUSPENDING;
		return true;
	}
	return false;
}

inline bool CDOSObjectGroup::Resume()
{
	if (m_Status == STATUS_SUSPENDING || m_Status == STATUS_SUSPENDED)
	{
		m_Status = STATUS_WORKING;
		return true;
	}
	return false;
}

inline bool CDOSObjectGroup::IsWorking()
{
	return m_Status == STATUS_WORKING;
}

inline UINT CDOSObjectGroup::AddTimer(OBJECT_ID ObjectID, UINT64 TimeOut, UINT64 Param, bool IsRepeat)
{
	TIMER_DATA TimerData;
	TimerData.ObjectID = ObjectID;
	TimerData.Param = Param;
	return m_TimerQueue.AddTimer(TimeOut, TimerData, IsRepeat);
}
inline BOOL CDOSObjectGroup::DeleteTimer(UINT ID)
{
	TIMER_DATA TimerData;
	if (m_TimerQueue.DeleteTimer(ID, &TimerData))
	{
		DOS_OBJECT_INFO * pObjectInfo = m_ObjectPool.GetObject(TimerData.ObjectID.ObjectIndex);
		if (pObjectInfo)
		{
			pObjectInfo->pObject->OnTimerRelease(ID, TimerData.Param);
		}
		else
		{
			PrintDOSLog(_T("对象[0x%llX]无法找到"), TimerData.ObjectID);
		}
		return TRUE;
	}
	return FALSE;
}