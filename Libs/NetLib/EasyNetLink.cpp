/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLink.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"




LPCTSTR g_szLinkStatus[ENL_LINK_MAX]={_T("None"),_T("Accepting"),_T("Accepted")};

IMPLEMENT_CLASS_INFO_STATIC(CEasyNetLink, CNameObject);

CEasyNetLink::CEasyNetLink(void)
{
	m_pManager=NULL;
	m_pConnection = NULL;
	
	m_Status=ENL_LINK_NONE;
	m_ReportID=0;
	m_NeedReallocConnectionID = false;
	
}

CEasyNetLink::~CEasyNetLink(void)
{
	if (m_pConnection)
	{
		PrintNetLog( _T("[%s]连接[%s:%u]释放"),
			CClassifiedID(GetReportID()).ToStr(),
			m_pConnection->GetBaseConnection()->GetRemoteAddress().GetIPString(),
			m_pConnection->GetBaseConnection()->GetRemoteAddress().GetPort());
	}
	SAFE_RELEASE(m_pConnection);
}

bool CEasyNetLink::Init(CEasyNetLinkManager * pManager, const CIPAddress& ConnectionAddress, UINT ReportID, 
	UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pManager;
	m_ReportID = ReportID;
	
	m_pConnection = NewConnection(ConnectionAddress, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail);
	if (m_pConnection)
		return true;
	else
		return false;	
}

bool CEasyNetLink::Init(CEasyNetLinkManager * pManager, UINT ReportID, UINT MaxPacketSize, 
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail, bool NeedReallocConnectionID)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pManager;
	m_ReportID = ReportID;	
	m_NeedReallocConnectionID = NeedReallocConnectionID;
	
	m_pConnection = NewConnection(MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail);
	if (m_pConnection)
		return true;
	else
		return false;
}

bool CEasyNetLink::Init(CEasyNetLink * pLink)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pLink->m_pManager;
	m_ReportID = pLink->m_ReportID;	
	m_NeedReallocConnectionID = pLink->m_NeedReallocConnectionID;
	m_pConnection = pLink->m_pConnection;
	m_pConnection->AddUseRef();
	m_pConnection->SetParent(this);
	return true;
	
}



void CEasyNetLink::OnLinkStart()
{
}

void CEasyNetLink::OnLinkEnd()
{
}

bool CEasyNetLink::SendData(LPCVOID pData, UINT DataSize)
{
	if (m_pConnection)
	{
		return m_pConnection->SendData(pData, DataSize);
	}
	else
	{
		PrintNetLog(_T("CEasyNetLinkConnection::SendData 连接未初始化"));
	}
	return false;
}

int CEasyNetLink::Update(int ProcessPacketLimit)
{
	int Process = 0;
	if (m_pConnection)
		Process = m_pConnection->Update(ProcessPacketLimit);	
	return Process;
}







void CEasyNetLink::PrintInfo(UINT LogChannel)
{
	if (m_pConnection)
	{
		CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0,
			_T("LinkID=[%s] RemoteAddress=%s:%u %s %s"),
			CClassifiedID(GetID()).ToStr(),
			m_pConnection->GetBaseConnection()->GetRemoteAddress().GetIPString(),
			m_pConnection->GetBaseConnection()->GetRemoteAddress().GetPort(),
			m_pConnection->IsConnected() ? _T("Connected") : _T("Disconnected"),
			g_szLinkStatus[m_Status]);
	}
}

CENLBaseConnection * CEasyNetLink::NewConnection(UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
	CENLConnection * pConnection = MONITORED_NEW(_T("CEasyNetLink"), CENLConnection);
	if (pConnection->Init(m_pManager, this, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
	{
		return pConnection;
	}
	return NULL;
}

CENLBaseConnection * CEasyNetLink::NewConnection(const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
	CENLConnection * pConnection = MONITORED_NEW(_T("CEasyNetLink"), CENLConnection);
	if (pConnection->Init(m_pManager, this, ConnectionAddress, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize, DisconnectOnTransferFail))
	{
		return pConnection;
	}
	return NULL;
}