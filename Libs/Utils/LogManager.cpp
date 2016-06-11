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

CLogManager::CLogManager(void):CNameObject()
{
	m_LogChannels.Insert(LOG_SYSTEM_CHANNEL,new CVSOutputLogPrinter());

	CEasyString ImportLogFileName=GetModuleFilePath(NULL);

	m_LogChannels.Insert(LOG_IMPORTANT_CHANNEL,new CFileLogPrinter(ImportLogFileName,FILE_LOG_APPEND|FILE_LOG_SAFE_WRITE));
}

CLogManager::~CLogManager(void)
{
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
}

ILogPrinter * CLogManager::GetChannel(UINT ChannelID)
{
	ILogPrinter ** ppLogPrinter=m_LogChannels.Find(ChannelID);
	if(ppLogPrinter)
	{
		return *ppLogPrinter;
	}
	return NULL;
}

BOOL CLogManager::DelChannel(UINT ChannelID)
{
	ILogPrinter ** ppLogPrinter=m_LogChannels.Find(ChannelID);
	if(ppLogPrinter)
	{
		SAFE_RELEASE(*ppLogPrinter);
		m_LogChannels.Delete(ChannelID);
		return TRUE;
	}
	return FALSE;
}

BOOL CLogManager::PrintLogDirect(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Msg)
{
	ILogPrinter * pLogPrinter = GetChannel(ChannelID);
	if (pLogPrinter)
	{
		pLogPrinter->PrintLogDirect(Level, Tag, Msg);
		return TRUE;
	}
	return FALSE;
}

BOOL CLogManager::PrintLog(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, ...)
{
	ILogPrinter * pLogPrinter=GetChannel(ChannelID);
	if(pLogPrinter)
	{
		va_list	vl;
		va_start(vl,Format);
		pLogPrinter->PrintLogVL(Level, Tag, Format, vl);
		va_end( vl);
		return TRUE;
	}
	return FALSE;
}
BOOL CLogManager::PrintLogVL(UINT ChannelID, int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
{
	ILogPrinter * pLogPrinter=GetChannel(ChannelID);
	if(pLogPrinter)
	{
		pLogPrinter->PrintLogVL(Level, Tag, Format, vl);
		return TRUE;
	}
	return FALSE;
}


