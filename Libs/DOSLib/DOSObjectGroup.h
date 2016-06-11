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
		int					Count;
		volatile UINT64		CPUCost;
	};
	CDOSObjectManager *								m_pManager;
	UINT											m_Index;
	STATUS											m_Status;
	volatile int									m_Weight;
	bool											m_StatObjectCPUCost;

	CThreadPerformanceCounter						m_ThreadPerformanceCounter;


	CThreadSafeIDStorage<DOS_OBJECT_REGISTER_INFO>	m_ObjectRegisterQueue;
	CThreadSafeIDStorage<OBJECT_ID>					m_ObjectUnregisterQueue;

	CIDStorage<DOS_OBJECT_INFO>						m_ObjectPool;

	CStaticMap<OBJECT_ID,OBJECT_STAT_INFO>			m_ObjectCountStatMap;

	CEasyCriticalSection							m_EasyCriticalSection;
	DECLARE_CLASS_INFO_STATIC(CDOSObjectGroup);
public:
	CDOSObjectGroup(void);
	virtual ~CDOSObjectGroup(void);

	virtual bool Initialize(CDOSObjectManager * pManager,UINT Index);
	virtual void Destory();

	
	CDOSObjectManager * GetManager();

	BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO& ObjectRegisterInfo);
	BOOL UnregisterObject(OBJECT_ID ObjectID);
	UINT GetObjectCount();

	int GetWeight();
	float GetCPUUsedRate();
	float GetCycleTime();
	UINT GetMaxObjectMsgQueueLen();
	UINT GetIndex();

	bool Suspend();
	bool Resume();
	bool IsWorking();


	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

	BOOL PushMessage(OBJECT_ID ObjectID,CDOSMessagePacket * pPacket);

	void PrintObjectStat(UINT LogChannel);
protected:
	int ProcessObjectRegister(int ProcessLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	int ProcessObjectUnregister(int ProcessLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	void OnObjectRegister(OBJECT_ID ObjectID);
	void OnObjectUnregister(OBJECT_ID ObjectID);
	void AddObjectCPUCost(OBJECT_ID ObjectID,UINT64 CPUCost);
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

inline UINT CDOSObjectGroup::GetIndex()
{
	return m_Index;
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