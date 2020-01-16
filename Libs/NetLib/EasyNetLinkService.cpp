/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkService.cpp                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CEasyNetLinkService,CNetService);

CEasyNetLinkService::CEasyNetLinkService(void)
{
	m_pManager=NULL;
	m_NeedReallocConnectionID = false;
	m_RecvQueueSize = 0;
	m_SendQueueSize = 0;
	m_MaxPacketSize=0;
	m_DataCompressType = CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE;
	m_MinCompressSize = CEasyNetLinkManager::DEFAULT_MIN_COMPRESS_SIZE;
	m_IPList.SetTag(_T("CEasyNetLinkService"));
}

CEasyNetLinkService::~CEasyNetLinkService(void)
{
}

bool CEasyNetLinkService::Init(CEasyNetLinkManager* pManager, UINT ReportID, const CIPAddress& ListenAddress,
	bool NeedReallocConnectionID, bool IsUseListenThread,
	int ParallelAcceptCount, UINT AcceptQueueSize,
	UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	m_pManager = pManager;
	m_ReportID = ReportID;
	m_NeedReallocConnectionID = NeedReallocConnectionID;
	m_RecvQueueSize = RecvQueueSize;
	m_SendQueueSize = SendQueueSize;
	m_MaxPacketSize = MaxPacketSize;
	m_DataCompressType = DataCompressType;
	m_MinCompressSize = MinCompressSize;

	SetServer(m_pManager->GetServer());
	if (Create(IPPROTO_TCP, AcceptQueueSize,
		RecvQueueSize, SendQueueSize,
		ParallelAcceptCount, DEFAULT_PARALLEL_RECV, IsUseListenThread))
	{
		if (StartListen(ListenAddress))
		{
			return true;
		}
	}
	return false;
}


CBaseNetConnection * CEasyNetLinkService::CreateConnection(CIPAddress& RemoteAddress)
{
	if(m_IPList.GetCount())
	{
		bool IsMatch=false;
		for(size_t i=0;i<m_IPList.GetCount();i++)
		{
			if(m_IPList[i].IsMatch(RemoteAddress))
			{
				IsMatch=true;
				break;
			}
		}
		if(!IsMatch)
		{
			PrintNetLog( _T("连接[%s]不在IP列表中，被拒绝"), RemoteAddress.GetIPString());
			return NULL;
		}
	}
	CEasyNetLink * pLink = NULL;
	if(m_pManager)
	{
		pLink = m_pManager->CreateAcceptLink();
		if (pLink)
		{
			if (pLink->Init(m_pManager, GetReportID(), m_MaxPacketSize, m_DataCompressType, m_MinCompressSize, NeedReallocConnectionID()))
			{
				return pLink->GetConnection();
			}
			else
			{
				PrintNetLog( _T("CEasyNetLinkService::CreateConnection:初始化连接失败"));
				m_pManager->DeleteLink(pLink);
			}			
		}
	}
	return NULL;
}

bool CEasyNetLinkService::DeleteConnection(CBaseNetConnection * pConnection)
{
	if(m_pManager)
	{
		m_pManager->DeleteLink(((CENLConnection *)pConnection)->GetParent());
	}
	return true;
}

void CEasyNetLinkService::PrintInfo(UINT LogChannel)
{
	CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
		_T("ServiceID=[%s] ListenAddress=%s:%u"),
		CClassifiedID(m_ReportID).ToStr(),
		GetLocalAddress().GetIPString(),
		GetLocalAddress().GetPort());
}
