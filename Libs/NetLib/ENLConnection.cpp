/****************************************************************************/
/*                                                                          */
/*      文件名:    CENLConnection.cpp                                     */
/*      创建日期:  2016年04月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


#define LINK_CONNECT_TIME		(30*1000)
#define KEEP_ALIVE_TIME			(10*1000)
#define MAX_KEEP_ALIVE_COUNT	6


IMPLEMENT_CLASS_INFO_STATIC(CENLConnection, CNetConnection);

CENLConnection::CENLConnection()
{
	m_pManager = NULL;
	m_pParent = NULL;
	m_KeepAliveCount = 0;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;
}


CENLConnection::~CENLConnection()
{
}

bool CENLConnection::Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_ActiveType = ENL_ACTIVE_TYPE_PROACTIVE;
	SetServer(m_pManager->GetServer());
	SetRemoteAddress(ConnectionAddress);
	if (MaxPacketSize + NET_DATA_BLOCK_SIZE > m_AssembleBuffer.GetBufferSize())
	{
		m_AssembleBuffer.Create(MaxPacketSize + NET_DATA_BLOCK_SIZE);
	}

	if (Create(RecvQueueSize, SendQueueSize))
		return true;
	else
		return false;
}

bool CENLConnection::Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;
	SetServer(m_pManager->GetServer());
	if(MaxPacketSize + NET_DATA_BLOCK_SIZE > m_AssembleBuffer.GetBufferSize())
	{
		m_AssembleBuffer.Create(MaxPacketSize + NET_DATA_BLOCK_SIZE);
	}
	return true;
}

int CENLConnection::Update(int ProcessPacketLimit)
{
	int Process = CNetConnection::Update(ProcessPacketLimit);
	if (m_KeepAliveCount > MAX_KEEP_ALIVE_COUNT)
	{
		PrintNetLog(_T("NetLib"), _T("CENLConnection::Update 连接[%s]超时被断开"),
			CClassifiedID(GetID()).ToStr());
		Disconnect();
	}
	if (IsDisconnected())
	{
		if (m_ActiveType == ENL_ACTIVE_TYPE_PROACTIVE)
		{
			if (m_ReconnectTimer.IsTimeOut(EASY_LINK_RECONNECT_TIME))
			{
				m_ReconnectTimer.SaveTime();
				Connect(GetRemoteAddress(), LINK_CONNECT_TIME);
				PrintNetLog(_T("NetLib"), _T("[%s]开始连接[%s:%u]"),
					CClassifiedID(m_pParent->GetReportID()).ToStr(),
					GetRemoteAddress().GetIPString(),
					GetRemoteAddress().GetPort());
			}
		}
		else
		{
			if (m_pManager)
				m_pManager->DeleteLink(m_pParent);
			return Process;
		}
	}
	else if (m_pParent->GetStatus() == ENL_LINK_ACCEPTED)
	{
		if (m_KeepAliveTimer.IsTimeOut(KEEP_ALIVE_TIME))
		{
			m_KeepAliveCount++;
			if (m_ActiveType == ENL_ACTIVE_TYPE_PROACTIVE)
			{
				SendLinkMsg(EASY_NET_LINK_MSG_KEEP_ALIVE);
			}
			m_KeepAliveTimer.SaveTime();
		}
	}	
	return Process;
}


void CENLConnection::OnRecvData(const BYTE * pData, UINT DataSize)
{

	if (!m_AssembleBuffer.PushBack(pData, DataSize))
	{
		PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 拼包缓冲区溢出,连接断开"));
		Disconnect();
		return;
	}
	EASY_NET_LINK_MSG * pMsg = (EASY_NET_LINK_MSG *)m_AssembleBuffer.GetBuffer();
	while (m_AssembleBuffer.GetUsedSize() >= (int)sizeof(EASY_NET_LINK_MSG_HEAD) &&
		(UINT)m_AssembleBuffer.GetUsedSize() >= pMsg->Header.Size)
	{
		UINT MsgSize = pMsg->Header.Size;
		if (MsgSize < sizeof(EASY_NET_LINK_MSG_HEAD))
		{
			PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 收到大小非法的消息,连接断开"));
			Disconnect();
			return;
		}
		switch (pMsg->Header.MsgID)
		{
		case EASY_NET_LINK_MSG_LINK_START:
			if (MsgSize >= sizeof(EASY_NET_LINK_MSG_HEAD) + sizeof(EASY_NET_LINK_INFO))
			{
				EASY_NET_LINK_INFO * pInfo = (EASY_NET_LINK_INFO *)pMsg->Data;
				SetID(pInfo->ID);
				if (m_pManager)
				{
					if (!m_pManager->AcceptLink(m_pParent))
					{
						PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 非法连接[%s][%s:%u]被拒绝"),
							CClassifiedID(GetID()).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
						Disconnect();
					}
					else
					{
						PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 连接[%s][%s:%u]建立"),
							CClassifiedID(GetID()).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
					}
				}
				else
				{
					PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 连接[%s][%s:%u]建立时没有设置管理器，被关闭"),
						CClassifiedID(GetID()).ToStr(),
						GetRemoteAddress().GetIPString(),
						GetRemoteAddress().GetPort());
					Disconnect();
				}

			}
			break;
		case EASY_NET_LINK_MSG_KEEP_ALIVE:
			if (m_pParent->GetStatus() == ENL_LINK_ACCEPTED)
			{
				m_KeepAliveCount = 0;
				if (m_ActiveType == ENL_ACTIVE_TYPE_PASSIVE)
				{
					SendLinkMsg(EASY_NET_LINK_MSG_KEEP_ALIVE);
				}
			}
			break;
		case EASY_NET_LINK_MSG_USER_DATA:
			if (MsgSize >= sizeof(EASY_NET_LINK_MSG_HEAD))
			{
				m_pParent->OnData((BYTE *)pMsg->Data, pMsg->Header.Size - sizeof(EASY_NET_LINK_MSG_HEAD), false);
			}
			break;
		case EASY_NET_LINK_MSG_COMPRESSED_USER_DATA:
			if (MsgSize >= sizeof(EASY_NET_LINK_MSG_HEAD))
			{
				m_pParent->OnData((BYTE *)pMsg->Data, pMsg->Header.Size - sizeof(EASY_NET_LINK_MSG_HEAD), true);
			}
			break;
		default:
			PrintNetLog(_T("NetLib"), _T("CENLConnection::OnRecvData 收到不明消息ID=%u,size=%u"),
				pMsg->Header.MsgID, MsgSize);
		}
		m_AssembleBuffer.PopFront(NULL, MsgSize);
	}
}

void CENLConnection::OnConnection(BOOL IsSucceed)
{
	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);

	if (IsSucceed)
	{
		PrintNetLog(_T("NetLib"), _T("[%s]连接[%s:%u]成功"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());


		EASY_NET_LINK_INFO Info;
		Info.ID = m_pParent->GetReportID();
		SendLinkMsg(EASY_NET_LINK_MSG_LINK_START, &Info, sizeof(Info));


	}
	else
	{
		PrintNetLog(_T("NetLib"), _T("[%s]连接[%s:%u]失败"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
	}
}

void CENLConnection::OnDisconnection()
{
	PrintNetLog(_T("NetLib"), _T("[%s]连接[%s:%u]断开"),
		CClassifiedID(m_pParent->GetReportID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());

	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);

	if (GetID())
	{
		if (m_pManager)
			m_pManager->OnLinkEnd(m_pParent);
		m_pParent->OnLinkEnd();
	}
}

void CENLConnection::SendLinkMsg(DWORD MsgID, LPCVOID pData, UINT DataSize)
{
	EASY_NET_LINK_MSG_HEAD MsgHeader;
	MsgHeader.Size = sizeof(EASY_NET_LINK_MSG_HEAD) + DataSize;
	MsgHeader.MsgID = MsgID;

	Send(&MsgHeader, sizeof(MsgHeader));
	if (pData&&DataSize)
	{
		Send(pData, DataSize);
	}
}