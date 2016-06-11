#pragma once

class CThreadPerformanceCounter
{
protected:
	HANDLE			m_TreadHandle;
	CEasyTimer		m_CountIntervalTimer;
	UINT			m_CountIntervalTime;
	UINT64			m_StartPerformanceCount;
	UINT			m_CycleCount;
	UINT			m_CPUCount;
	UINT64			m_StartCPUUsedTime;
	volatile float	m_CycleTime;
	volatile float	m_CPUUsedRate;
public:
	CThreadPerformanceCounter(void);
	~CThreadPerformanceCounter(void);

	void Init(HANDLE ThreadHandle,UINT CountIntervalTime);
	void DoPerformanceCount();

	float GetCycleTime()
	{
		return m_CycleTime;
	}
	float GetCPUUsedRate()
	{
		return m_CPUUsedRate;
	}
};
