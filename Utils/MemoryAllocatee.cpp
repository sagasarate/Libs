/****************************************************************************/
/*                                                                          */
/*      文件名:    MemoryAllocatee.cpp                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

TCHAR MEMORY_ALLOCATEE[] = _T("MemoryAllocateeInstance");

volatile bool CMemoryAllocatee::m_Enable = true;

CMemoryAllocatee::CMemoryAllocatee(void)
{
	if(m_Enable)
	{
		for (UINT i = 0; i < PTR_HASH_SIZE; i++)
		{
			CAutoLock Lock(m_AllocInfoLock[i]);
			m_AllocInfos[i].Create(4096, 4096, 4096);
		}
		{
			CAutoLock Lock(m_AllocModuleLock);
			m_AllocModules.Create(1024, 1024, 1024);
			m_TotalAllocSize = 0;
			m_AllocCount = 0;
			m_LogDir[0] = 0;
		}
	}
}

CMemoryAllocatee::~CMemoryAllocatee(void)
{
	m_Enable = false;
	for (UINT i = 0; i < PTR_HASH_SIZE; i++)
	{
		CAutoLock Lock(m_AllocInfoLock[i]);
		m_AllocInfos[i].Destory();
	}
	CAutoLock Lock(m_AllocModuleLock);	
	m_AllocModules.Destory();
	
}

void CMemoryAllocatee::AddAllocRecord(void * pMemory, size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName)
{
	if(m_Enable)
	{
		//if (_tcscmp(ModuleName, "CGameHall") == 0)
		//	int err = 0;

		UINT Index = ((UINT64)pMemory) % PTR_HASH_SIZE;

		{
			CAutoLock Lock(m_AllocInfoLock[Index]);
			ALLOC_INFO * pAllocInfo = NULL;
			m_AllocInfos[Index].New(pMemory, &pAllocInfo);
			if (pAllocInfo)
			{
				pAllocInfo->AllocSize = Size;
				pAllocInfo->FileName = FileName;
				pAllocInfo->Line = Line;
				pAllocInfo->ModuleName = ModuleName;
				pAllocInfo->AllocCount = m_AllocCount;
			}
		}

		{
			CAutoLock Lock(m_AllocModuleLock);

			if (ModuleName)
			{
				MODULE_ALLOC_INFO * pModuleAllocInfo = m_AllocModules.Find(ModuleName);
				if (pModuleAllocInfo == NULL)
				{
					m_AllocModules.New(ModuleName, &pModuleAllocInfo);
					if (pModuleAllocInfo)
					{
						pModuleAllocInfo->ModuleName = ModuleName;
						pModuleAllocInfo->AllocSize = 0;
						pModuleAllocInfo->AllocCount = 0;
					}
				}
				if (pModuleAllocInfo)
				{
					pModuleAllocInfo->AllocSize += Size;
					pModuleAllocInfo->AllocCount++;
				}
			}
			m_AllocCount++;
			m_TotalAllocSize += Size;
		}		
	}
}

void CMemoryAllocatee::AddFreeRecord(void * pMemory)
{
	if (m_Enable)
	{
		UINT Index = ((UINT64)pMemory) % PTR_HASH_SIZE;
		LPCTSTR ModuleName = NULL;
		size_t AllocSize = 0;
		{
			CAutoLock Lock(m_AllocInfoLock[Index]);
			LPVOID Pos = m_AllocInfos[Index].FindPos(pMemory);
			if (Pos)
			{
				ALLOC_INFO * pAllocInfo = m_AllocInfos[Index].GetObject(Pos);
				ModuleName = pAllocInfo->ModuleName;
				AllocSize = pAllocInfo->AllocSize;
				m_AllocInfos[Index].DeleteByPos(Pos);
			}
		}
		{
			CAutoLock Lock(m_AllocModuleLock);

			
			if (ModuleName)
			{
				LPVOID Pos = m_AllocModules.FindPos(ModuleName);
				if (Pos)
				{
					MODULE_ALLOC_INFO * pModuleAllocInfo = m_AllocModules.GetObject(Pos);
					pModuleAllocInfo->AllocSize -= AllocSize;
					pModuleAllocInfo->AllocCount--;
					if (pModuleAllocInfo->AllocSize == 0)
					{
						m_AllocModules.DeleteByPos(Pos);
					}
				}
			}
			m_TotalAllocSize -= AllocSize;
		}
	}
}

static void PrintLog(IFileAccessor * pFile, LPCTSTR szFormat, ...)
{
	va_list	vl;
	va_start(vl, szFormat);
	TCHAR szBuff[4096];
	_vstprintf_s(szBuff, 4096, szFormat, vl);
	_tcscat_s(szBuff, 4096, _T("\r\n"));
	pFile->Write(szBuff, _tcslen(szBuff));
	va_end(vl);
}

void CMemoryAllocatee::ReportLeakStat()
{
	UINT LeakCount = 0;
	for (UINT i = 0; i < PTR_HASH_SIZE; i++)
	{
		CAutoLock Lock(m_AllocInfoLock[i]);
		LeakCount += m_AllocInfos[i].GetObjectCount();
	}

	if (LeakCount == 0)
		return;

	IFileAccessor * pFile = NULL;
#ifdef WIN32
	pFile = new CWinFileAccessor();
#else
	pFile = new CLinuxFileAccessor();
#endif


	if (m_LogDir[0] == 0)
	{
		SetLogDir(CFileTools::GetModulePath(NULL));
	}

	TCHAR FileName[MAX_PATH];
	CEasyTime CurTime;
	CurTime.FetchLocalTime();
	_stprintf_s(FileName, MAX_PATH, _T("%s/MemLeakStat.%u-%02u-%02u %02u-%02u-%02u.Log"),
		m_LogDir, CurTime.Year(), CurTime.Month(), CurTime.Day(), CurTime.Hour(), CurTime.Minute(), CurTime.Second());

	if (pFile->Open(FileName, IFileAccessor::modeCreateAlways | IFileAccessor::modeWrite | IFileAccessor::shareShareRead | IFileAccessor::osWriteThrough))
	{
		if (m_TotalAllocSize)
		{
			PrintLog(pFile, _T("Total Memory Leak Size=%u"),
				m_TotalAllocSize);
		}
		for (UINT i = 0; i < PTR_HASH_SIZE; i++)
		{
			CAutoLock Lock(m_AllocInfoLock[i]);
			void * Pos = m_AllocInfos[i].GetFirstObjectPos();
			while (Pos)
			{
				void * Key;
				ALLOC_INFO * pInfo = m_AllocInfos[i].GetNextObject(Pos, Key);

				if (pInfo->FileName)
				{
					PrintLog(pFile, _T("[%p](%u) Leak Size=%u,Position=[%s]%s:%d"),
						Key, pInfo->AllocCount, pInfo->AllocSize, pInfo->ModuleName, pInfo->FileName, pInfo->Line);
				}
				else
				{
					PrintLog(pFile, _T("[%p](%u) Leak Size=%u"),
						Key, pInfo->AllocCount, pInfo->AllocSize);
				}
			}
		}	
	}
	delete pFile;
}

void CMemoryAllocatee::ReportAllocStat(UINT LogChannel)
{
	CAutoLock Lock(m_AllocModuleLock);

	MODULE_ALLOC_INFO * InfoList[1024];

	if (m_AllocModules.GetObjectCount() < 1024)
	{
		UINT Count = 0;
		void * Pos = m_AllocModules.GetFirstObjectPos();
		while (Pos)
		{
			InfoList[Count++] = m_AllocModules.GetNextObject(Pos);
		}
		qsort(InfoList, Count, sizeof(MODULE_ALLOC_INFO *), MODULE_ALLOC_INFO::CompareWithAllocSize);
		for (UINT i = 0; i < Count; i++)
		{
			CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, _T(""),
				_T("Module[%s]=%s(%d)"),
				InfoList[i]->ModuleName, (LPCTSTR)FormatNumberWords(InfoList[i]->AllocSize, true), InfoList[i]->AllocCount);
		}
	}
	else
	{
		void * Pos = m_AllocModules.GetFirstObjectPos();
		while (Pos)
		{
			MODULE_ALLOC_INFO * pAllocInfo = m_AllocModules.GetNextObject(Pos);
			if (pAllocInfo)
			{
				CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, _T(""),
					_T("Module[%s]=%s(%d)"),
					pAllocInfo->ModuleName, (LPCTSTR)FormatNumberWords(pAllocInfo->AllocSize, true), pAllocInfo->AllocCount);
			}
		}
	}
	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, _T(""),
		_T("Total=%s"),
		(LPCTSTR)FormatNumberWords(m_TotalAllocSize, true));
}

