/****************************************************************************/
/*                                                                          */
/*      文件名:    LogManager.cpp                                           */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CLogManager,CNameObject);

TCHAR LOG_MANAGER_INSTANCE[]=_T("LogManagerInstance");

CLogManager::CLogManager(void)
	:CNameObject()
{
	m_LogChannels.SetTag(_T("CLogManager"));
	m_WorkThreadList.SetTag(_T("CLogManager"));

	m_LogChannels.Create(128, 128, 128);
	m_LogChannels.Insert(LOG_SYSTEM_CHANNEL, MONITORED_NEW(_T("CLogManager"), CVSOutputLogPrinter));

	CEasyString ImportLogFileName = CFileTools::GetModuleFilePath(NULL);
	
	m_LogChannels.Insert(LOG_IMPORTANT_CHANNEL, MONITORED_NEW(_T("CLogManager"), CFileLogPrinter, ImportLogFileName, FILE_LOG_APPEND | FILE_LOG_SAFE_WRITE));

	m_WorkThreadList.Resize(DEFAULT_ASYNC_LOG_WORK_THREAD_COUNT);
	for (UINT i = 0; i < m_WorkThreadList.GetCount(); i++)
	{
		m_WorkThreadList[i].Start();
	}
}

CLogManager::~CLogManager(void)
{
	for (UINT i = 0; i < m_WorkThreadList.GetCount(); i++)
	{
		m_WorkThreadList[i].SafeTerminate(5);
	}

	void * Pos=m_LogChannels.GetFirstObjectPos();
	while(Pos)
	{
		UINT Key;
		ILogPrinter ** ppLogPrinter=m_LogChannels.GetNextObject(Pos,Key);
		if(ppLogPrinter)
		{
			SAFE_RELEASE(*ppLogPrinter)
		}
	}
	m_LogChannels.Clear();
}

UINT CLogManager::AddWorkThreadCount(UINT Count)
{
	if (Count)
	{
		UINT OldCount = (UINT)m_WorkThreadList.GetCount();
		m_WorkThreadList.Resize(m_WorkThreadList.GetCount() + Count);
		for (UINT i = OldCount; i < m_WorkThreadList.GetCount(); i++)
		{
			m_WorkThreadList[i].Start();
		}
	}
	return (UINT)m_WorkThreadList.GetCount();
}

UINT CLogManager::GetChannelCount()
{
	return m_LogChannels.GetObjectCount();
}

void CLogManager::AddChannel(UINT ChannelID,ILogPrinter * pLogPrinter)
{
	if(pLogPrinter==NULL)
		return;
	if(m_LogChannels.Find(ChannelID)!=NULL)
	{
		DelChannel(ChannelID);
	}
	m_LogChannels.Insert(ChannelID,pLogPrinter);
	pLogPrinter->AddUseRef();
	if (pLogPrinter->NeedAsyncUpdate())
	{
		CAsyncLogWorkThread * pWorkThread = NULL;
		UINT MinCount = 0;
		for (UINT i = 0; i < m_WorkThreadList.GetCount(); i++)
		{
			if (pWorkThread == NULL || MinCount>m_WorkThreadList[i].GetLogPrinterCount())
			{
				pWorkThread = m_WorkThreadList.GetObject(i);
				MinCount = pWorkThread->GetLogPrinterCount();
			}
		}
		if (pWorkThread)
		{
			pWorkThread->AddLogPrinter(pLogPrinter);
		}
		else
		{
			PrintImportantLog(_T("无法给%u通道的日志输器分配工作线程"), ChannelID);
		}
	}
	PrintImportantLog(_T("已在%u通道添加日志输出器"), ChannelID);
}

ILogPrinter * CLogManager::GetChannel(UINT ChannelID)
{
	ILogPrinter ** ppLogPrinter=m_LogChannels.Find(ChannelID);
	if(ppLogPrinter)
	{
		return *ppLogPrinter;
	}
	//else
	//{
	//	PrintSystemLog(_T("无法找到Log通道%u"), ChannelID);
	//}
	return NULL;
}

bool CLogManager::DelChannel(UINT ChannelID)
{
	ILogPrinter ** ppLogPrinter=m_LogChannels.Find(ChannelID);
	if(ppLogPrinter)
	{
		if ((*ppLogPrinter)->NeedAsyncUpdate())
		{
			bool HaveRemoved = false;
			for (UINT i = 0; i < m_WorkThreadList.GetCount(); i++)
			{
				if (m_WorkThreadList[i].RemoveLogPrinter((*ppLogPrinter)))
					HaveRemoved = true;
			}
			if (!HaveRemoved)
			{
				PrintSystemLog(_T("无法将通道%u的日志输出其从工作线程移除"), ChannelID);
				return false;
			}
		}
		SAFE_RELEASE(*ppLogPrinter);
		m_LogChannels.Delete(ChannelID);
		return true;
	}
	//else
	//{
	//	PrintSystemLog(_T("无法找到Log通道%u"), ChannelID);
	//}
	return false;
}

bool CLogManager::PrintLogDirect(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Msg)
{
	ILogPrinter * pLogPrinter = GetChannel(ChannelID);
	if (pLogPrinter)
	{
		pLogPrinter->PrintLogDirect(Level, Tag, Msg);
		return true;
	}
	//else
	//{
	//	PrintSystemLog(_T("无法找到Log通道%u"), ChannelID);
	//}
	return false;
}

bool CLogManager::PrintLog(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, ...)
{
	ILogPrinter * pLogPrinter=GetChannel(ChannelID);
	if(pLogPrinter)
	{
		va_list	vl;
		va_start(vl,Format);
		pLogPrinter->PrintLogVL(Level, Tag, Format, vl);
		va_end( vl);
		return true;
	}
	//else
	//{
	//	PrintSystemLog(_T("无法找到Log通道%u"), ChannelID);
	//}
	return false;
}
bool CLogManager::PrintLogVL(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
{
	ILogPrinter * pLogPrinter=GetChannel(ChannelID);
	if(pLogPrinter)
	{
		pLogPrinter->PrintLogVL(Level, Tag, Format, vl);
		return true;
	}
	//else
	//{
	//	PrintSystemLog(_T("无法找到Log通道%u"), ChannelID);
	//}
	return false;
}


