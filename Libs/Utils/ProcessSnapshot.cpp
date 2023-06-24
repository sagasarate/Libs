#include "stdafx.h"

#define PROCESS_BLOCK_SIZE	128

CProcessSnapshot::CProcessSnapshot()
{
	m_RecentSystemTime = 0;
	m_RecentSystemIdleTime = 0;
	m_CPUCount = 1;
	m_CPUUsedRate = NAN;
	m_SnapshotCount = 0;
	m_ProcessList.SetTag(_T("CProcessSnapshot"));

#ifdef WIN32
	SYSTEM_INFO SysInfo;
	GetNativeSystemInfo(&SysInfo);
	m_CPUCount = SysInfo.dwNumberOfProcessors;
#else
	m_CPUFreq = sysconf(_SC_CLK_TCK);
	m_PageSize = sysconf(_SC_PAGE_SIZE);
	m_SystemInfoFile = NULL;
	m_ProcessInfoFileList.SetTag(_T("CProcessSnapshot"));
#endif
}


CProcessSnapshot::~CProcessSnapshot()
{
#ifndef WIN32
	if (m_SystemInfoFile)
	{
		fclose(m_SystemInfoFile);
		m_SystemInfoFile=NULL;
	}
	for (UINT i = 0; i < m_ProcessInfoFileList.GetCount(); i++)
	{
		fclose(m_ProcessInfoFileList[i].InfoFile);
	}
	m_ProcessInfoFileList.Clear();
#endif
}

#ifdef WIN32
bool CProcessSnapshot::Snapshot()
{
	FILETIME IdleTime,KernelTime,UserTime;
	UINT64 TimeSpan = 0;
	if (GetSystemTimes(&IdleTime, &KernelTime, &UserTime))
	{
		//windows的空闲时间包含在核心时间内
		UINT64_CONVERTER Converter;
		UINT64 SystemUserTime = 0;
		UINT64 SystemKernelTime = 0;
		UINT64 SystemIdleTime = 0;
		Converter.LowPart = IdleTime.dwLowDateTime;
		Converter.HighPart = IdleTime.dwHighDateTime;
		SystemIdleTime += Converter.QuadPart * 100;
		Converter.LowPart = KernelTime.dwLowDateTime;
		Converter.HighPart = KernelTime.dwHighDateTime;
		SystemKernelTime += Converter.QuadPart * 100;
		Converter.LowPart = UserTime.dwLowDateTime;
		Converter.HighPart = UserTime.dwHighDateTime;
		SystemUserTime += Converter.QuadPart * 100;

		UINT64 SystemTime = SystemUserTime + SystemKernelTime;
		if (m_RecentSystemTime)
		{
			TimeSpan = SystemTime - m_RecentSystemTime;
			UINT64 IdleSpan = SystemIdleTime - m_RecentSystemIdleTime;
			if (IdleSpan&&TimeSpan)
			{
				m_CPUUsedRate = (float)(TimeSpan - IdleSpan) / (float)TimeSpan;
			}
		}

		m_RecentSystemTime = SystemTime;
		m_RecentSystemIdleTime = SystemIdleTime;
	}
	else
	{
		return false;
	}

	CEasyArray<DWORD> ProcessIDList(_T("CProcessSnapshot"));

	UINT BlockCount = 1;
	DWORD ReturnSize;
	UINT ProcessCount = 0;
	do
	{
		ProcessIDList.Resize(PROCESS_BLOCK_SIZE*BlockCount);
		if (EnumProcesses(ProcessIDList.GetBuffer(), sizeof(DWORD)*(DWORD)ProcessIDList.GetBufferLength(), &ReturnSize))
		{
			ProcessCount = ReturnSize / sizeof(DWORD);
			BlockCount++;
		}
		else
		{
			return false;
		}
	} while (ProcessCount >= ProcessIDList.GetCount());

	CEasyArray<PROCESS_INFO> ProcessList(_T("CProcessSnapshot"));
	ProcessList.Reserve(ProcessCount);
	for (UINT i = 0; i < ProcessCount; i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessIDList[i]);
		if (hProcess != NULL)
		{
			PROCESS_INFO * pProcessInfo = ProcessList.AddEmpty();
			pProcessInfo->ProcessID = ProcessIDList[i];

			

			TCHAR ImageFilePath[1024];
			DWORD BufferLen = sizeof(ImageFilePath) - 1;
			if (QueryFullProcessImageName(hProcess, 0, ImageFilePath, &BufferLen))
			{
				ImageFilePath[BufferLen] = 0;
				pProcessInfo->ImageFile = CFileTools::MakeFullPath(ImageFilePath);
			}
			//if (EnumProcessModulesEx(hProcess, &hModule, sizeof(hModule), &ReturnSize, LIST_MODULES_ALL))
			//{
			//	
			//	if (GetModuleFileNameEx(hProcess, hModule, ImageFilePath, MAX_PATH) != 0)
			//	{
			//		UINT Len = GetLongPathName(ImageFilePath, NULL, 0);
			//		pProcessInfo->ImageFile.Resize(Len);
			//		GetLongPathName(ImageFilePath, (TCHAR *)pProcessInfo->ImageFile.GetBuffer(), MAX_PATH);
			//		pProcessInfo->ImageFile.TrimBuffer();
			//		pProcessInfo->ImageFile = CFileTools::MakeFullPath(pProcessInfo->ImageFile);
			//	}
			//}

			int err = GetLastError();

			FILETIME CreateTime, ExitTime, KernelTime, UserTime;
			if (GetProcessTimes(hProcess, &CreateTime, &ExitTime, &KernelTime, &UserTime))
			{
				UINT64_CONVERTER Converter;
				Converter.LowPart = KernelTime.dwLowDateTime;
				Converter.HighPart = KernelTime.dwHighDateTime;
				UINT64 UseTime = Converter.QuadPart * 100;
				Converter.LowPart = UserTime.dwLowDateTime;
				Converter.HighPart = UserTime.dwHighDateTime;
				UseTime += Converter.QuadPart * 100;
				pProcessInfo->CPUUsedTime = UseTime;

				const PROCESS_INFO * pOldInfo = GetProcessInfo(pProcessInfo->ProcessID);
				if (pOldInfo&&TimeSpan)
				{
					pProcessInfo->CPUUsedRate = (float)(UseTime - pOldInfo->CPUUsedTime) / (float)TimeSpan;
				}
				else
				{
					pProcessInfo->CPUUsedRate = NAN;
				}

			}

			PROCESS_MEMORY_COUNTERS MemoryInfo;
			if (GetProcessMemoryInfo(hProcess, &MemoryInfo, sizeof(MemoryInfo)))
			{
				pProcessInfo->MemoryUsed = MemoryInfo.WorkingSetSize;
				pProcessInfo->VirtualMemoryUsed = MemoryInfo.PagefileUsage;
			}
			CloseHandle(hProcess);
		}
	}
	m_ProcessList = ProcessList;
	m_SnapshotCount++;
	return true;
}
#else
bool CProcessSnapshot::Snapshot()
{
	UINT64 UsrTime = 0;
	UINT64 LowUsrTime = 0;
	UINT64 SysTime = 0;
	UINT64 IdleTime = 0;
	UINT64 IOWaitTime = 0;
	UINT64 IRQTime = 0;
	UINT64 SoftIRQTime = 0;
	UINT64 StealTime = 0;
	UINT64 QuestTime = 0;

	if (m_SystemInfoFile == NULL)
	{
		m_SystemInfoFile = fopen("/proc/stat", "r");
		if (m_SystemInfoFile == NULL)
			return false;
	}

	fseek(m_SystemInfoFile, 0, SEEK_SET);
	fflush(m_SystemInfoFile);

	int FetchCount = fscanf(m_SystemInfoFile, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
		&UsrTime, &LowUsrTime, &SysTime, &IdleTime, &IOWaitTime,
		&IRQTime, &SoftIRQTime, &StealTime, &QuestTime);

	if (FetchCount < 4)
		return false;




	UINT64 SystemTime = UsrTime + LowUsrTime + SysTime + IdleTime + IOWaitTime + IRQTime + SoftIRQTime + StealTime + QuestTime;

	SystemTime = (UINT64)(((double)SystemTime / (double)m_CPUFreq)*CEasyTimerEx::TIME_UNIT_PER_SECOND);
	UINT64 SystemIdleTime = (UINT64)(((double)IdleTime / (double)m_CPUFreq)*CEasyTimerEx::TIME_UNIT_PER_SECOND);
	UINT64 TimeSpan = 0;
	if (m_RecentSystemTime)
	{
		TimeSpan = SystemTime - m_RecentSystemTime;
		UINT64 IdleSpan = SystemIdleTime - m_RecentSystemIdleTime;
		if (IdleSpan&&TimeSpan)
		{
			m_CPUUsedRate = (float)(TimeSpan - IdleSpan) / (float)TimeSpan;
		}
	}
	m_RecentSystemTime = SystemTime;
	m_RecentSystemIdleTime = SystemIdleTime;

	CFileSearcher FileSearcher;

	CEasyArray<PROCESS_INFO> ProcessList(_T("CProcessSnapshot"));
	ProcessList.Create(32,32);
	FileSearcher.FindFirst("/proc/*");
	while (FileSearcher.FindNext())
	{
		if (FileSearcher.IsDirectory() && (!FileSearcher.IsDots()))
		{
			UINT ProcessID = atoi(FileSearcher.GetFileName());
			PROCESS_INFO_FILE * pProcessFile = GetProcessInfoFile(ProcessID, FileSearcher.GetFilePath());
			if (pProcessFile)
			{
				PROCESS_INFO * pProcessInfo = ProcessList.AddEmpty();
				pProcessInfo->ProcessID = ProcessID;
				pProcessInfo->ImageFile = pProcessFile->ImageFile;


				int PID = 0;
				char ImageFileName[MAX_PATH];
				ImageFileName[0] = 0;
				char Status = 0;
				int PPID = 0;
				int PGRP = 0;
				int Session = 0;
				int TTY_NR = 0;
				int TPGID = 0;
				int Flags = 0;
				int MinorFLT = 0;
				int CMinorFLT = 0;
				int MajorFLT = 0;
				int CMajorFLT = 0;
				UINT64 UserTime = 0;
				UINT64 SysTime = 0;
				UINT64 ChildUserTime = 0;
				UINT64 ChildSysTime = 0;
				int Priority = 0;
				int Nice = 0;
				int ThreadCount = 0;
				UINT64 ItrealValue = 0;
				UINT64 StartTime = 0;
				UINT64 VSize = 0;
				UINT64 RSS = 0;
				UINT64 RSSLimit = 0;

				fseek(pProcessFile->InfoFile, 0, SEEK_SET);
				fflush(pProcessFile->InfoFile);
				int FetchCount = fscanf(pProcessFile->InfoFile, "%d %s %c %d %d %d %d %d %d %d %d %d %d %llu %llu %llu %llu %d %d %d %llu %llu %llu %llu %llu",
					&PID, ImageFileName, &Status, &PPID, &PGRP, &Session, &TTY_NR, &TPGID, &Flags, &MinorFLT, &CMinorFLT, &MajorFLT,
					&CMajorFLT, &UserTime, &SysTime, &ChildUserTime, &ChildSysTime, &Priority, &Nice, &ThreadCount, &ItrealValue, &StartTime, &VSize, &RSS, &RSSLimit);


				if (FetchCount >= 25)
				{
					switch (Status)
					{
					case 'R':
						pProcessInfo->Status = PROCESS_STATUS_RUNNING;
						break;
					case 'S':
						pProcessInfo->Status = PROCESS_STATUS_SLEEPING;
						break;
					case 'D':
						pProcessInfo->Status = PROCESS_STATUS_DISK_SLEEPING;
						break;
					case 'Z':
						pProcessInfo->Status = PROCESS_STATUS_ZOMBIE;
						break;
					case 'T':
						pProcessInfo->Status = PROCESS_STATUS_SUSPENDED;
						break;
					case 'W':
						pProcessInfo->Status = PROCESS_STATUS_PAGING;
						break;
					default:
						pProcessInfo->Status = PROCESS_STATUS_UNKNOW;
						break;
					}
				}
				pProcessInfo->Priority = Priority;
				UINT64 CPUUsedTime = UserTime + SysTime;
				pProcessInfo->CPUUsedTime = (UINT64)(((double)CPUUsedTime / (double)m_CPUFreq)*CEasyTimerEx::TIME_UNIT_PER_SECOND);

				const PROCESS_INFO * pOldInfo = GetProcessInfo(pProcessInfo->ProcessID);
				if (pOldInfo&&TimeSpan)
				{
					pProcessInfo->CPUUsedRate = (float)(pProcessInfo->CPUUsedTime - pOldInfo->CPUUsedTime) / (float)TimeSpan;
				}
				else
				{
					pProcessInfo->CPUUsedRate = NAN;
				}
				pProcessInfo->MemoryUsed = RSS * m_PageSize;
				pProcessInfo->VirtualMemoryUsed = VSize;

			}
		}
	}
	CheckProcessInfoFile();
	m_ProcessList = ProcessList;
	m_SnapshotCount++;
	return true;
}
#endif


#ifndef WIN32
CProcessSnapshot::PROCESS_INFO_FILE * CProcessSnapshot::GetProcessInfoFile(UINT ProcessID, LPCTSTR ProcDir)
{
	if (ProcessID == 0)
		return NULL;

	for (UINT i = 0; i < m_ProcessInfoFileList.GetCount(); i++)
	{
		if (m_ProcessInfoFileList[i].ProcessID == ProcessID)
		{
			m_ProcessInfoFileList[i].AccessCount = m_SnapshotCount;
			return m_ProcessInfoFileList.GetObject(i);
		}
	}

	TCHAR FileName[MAX_PATH];

	sprintf_s(FileName, MAX_PATH,"%s/stat", ProcDir);

	FILE * pFile = fopen(FileName, "r");
	if (pFile)
	{
		PROCESS_INFO_FILE * pInfo = m_ProcessInfoFileList.AddEmpty();
		pInfo->InfoFile = pFile;
		pInfo->ProcessID = ProcessID;
		pInfo->ImageFile.Resize(MAX_PATH + 1);
		sprintf_s(FileName, MAX_PATH,"%s/exe", ProcDir);
		int Len = readlink(FileName, (char *)pInfo->ImageFile.GetBuffer(), MAX_PATH);
		if (Len > 0)
		{
			pInfo->ImageFile[Len] = 0;
			pInfo->ImageFile.TrimBuffer();
		}
		else
		{
			pInfo->ImageFile.Clear();
		}
		//PrintImportantLog("Process Snapshot PID:%d File:%s",pInfo->ProcessID,(LPCTSTR)pInfo->ImageFile);
		pInfo->AccessCount = m_SnapshotCount;
		return pInfo;
	}
	return NULL;
}

void CProcessSnapshot::CheckProcessInfoFile()
{
	for (int i = (int)m_ProcessInfoFileList.GetCount() - 1; i >= 0; i--)
	{
		if (m_ProcessInfoFileList[i].AccessCount != m_SnapshotCount)
		{
			fclose(m_ProcessInfoFileList[i].InfoFile);
			m_ProcessInfoFileList.Delete(i);
		}
	}
}
#endif
