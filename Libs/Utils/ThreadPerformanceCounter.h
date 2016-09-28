#pragma once

class CThreadPerformanceCounter
{
protected:
	HANDLE			m_TreadHandle;
	CEasyTimer		m_CountIntervalTimer;
	UINT			m_CountIntervalTime;
	UINT64			m_StartTime;
	UINT			m_CycleCount;
	UINT			m_CPUCount;
	UINT64			m_StartCPUUsedTime;
	volatile float	m_CycleTime;
	volatile float	m_CPUUsedRate;
	volatile UINT64	m_CycleCPUUsedTime;
	volatile UINT	m_RecentCycleCount;
public:
	CThreadPerformanceCounter(void);
	~CThreadPerformanceCounter(void);

	void Init(HANDLE ThreadHandle,UINT CountIntervalTime);
	void DoPerformanceCount();

	UINT GetCycleCount()
	{
		return m_RecentCycleCount;
	}
	float GetCycleTime()
	{
		return m_CycleTime;
	}
	UINT64 GetCycleCPUUsedTime()
	{
		return m_CycleCPUUsedTime;
	}
	float GetCPUUsedRate()
	{
		return m_CPUUsedRate;
	}
};
