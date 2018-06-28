#pragma once

class CProcessSnapshot
{
public:
	enum PROCESS_STATUS
	{
		PROCESS_STATUS_UNKNOW,
		PROCESS_STATUS_RUNNING,
		PROCESS_STATUS_SUSPENDED,
		PROCESS_STATUS_SLEEPING,
		PROCESS_STATUS_DISK_SLEEPING,
		PROCESS_STATUS_PAGING,
		PROCESS_STATUS_ZOMBIE,
	};
	struct PROCESS_INFO
	{
		UINT			ProcessID;
		CEasyString		ImageFile;
		PROCESS_STATUS	Status;
		int				Priority;		
		UINT64			CPUUsedTime;
		float			CPUUsedRate;
		UINT64			MemoryUsed;
		UINT64			VirtualMemoryUsed;
	};
	struct PROCESS_INFO_FILE
	{
		UINT			ProcessID;
		CEasyString		ImageFile;
		FILE *			InfoFile;
		UINT			AccessCount;
		PROCESS_INFO_FILE()
		{
			ProcessID = 0;
			InfoFile = NULL;
			AccessCount = 0;
		}
	};
protected:
	UINT64							m_RecentSystemTime;
	UINT64							m_RecentSystemIdleTime;
	UINT							m_CPUCount;
	float							m_CPUUsedRate;
	CEasyArray<PROCESS_INFO>		m_ProcessList;	
	UINT							m_SnapshotCount;
#ifndef WIN32
	UINT64							m_CPUFreq;
	UINT							m_PageSize;
	FILE *							m_SystemInfoFile;
	CEasyArray<PROCESS_INFO_FILE>	m_ProcessInfoFileList;
#endif
public:
	CProcessSnapshot();
	~CProcessSnapshot();

	bool Snapshot();
	float GetCPURate()
	{
		return m_CPUUsedRate;
	}
	const CEasyArray<PROCESS_INFO>& GetProcessList()
	{
		return m_ProcessList;
	}
	const PROCESS_INFO * GetProcessInfo(UINT ProcessID)
	{
		for (UINT i = 0; i < m_ProcessList.GetCount(); i++)
		{
			if (m_ProcessList[i].ProcessID == ProcessID)
				return m_ProcessList.GetObject(i);
		}
		return NULL;
	}
protected:
#ifndef WIN32
	PROCESS_INFO_FILE * GetProcessInfoFile(UINT ProcessID, LPCTSTR ProcDir);
	void CheckProcessInfoFile();
#endif
};

