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

static char s_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

CENLConnection::CENLConnection()
{
	m_pManager = NULL;
	m_pParent = NULL;
	m_Status = STATUS_ACCEPTING;
	m_KeepAliveCount = 0;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;
	m_AssembleBuffer.SetTag(_T("CENLConnection"));

	m_MaxPacketSize = 0;
	m_DataCompressType = CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE;
	m_MinCompressSize = CEasyNetLinkManager::DEFAULT_MIN_COMPRESS_SIZE;
	m_CompressBuffer.SetTag(_T("CENLConnection"));
}


CENLConnection::~CENLConnection()
{
	Destory();
}

void CENLConnection::Release()
{
	CNetConnection::Release();
}

UINT CENLConnection::AddUseRef()
{
	return CNetConnection::AddUseRef();
}

bool CENLConnection::Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress, 
	UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_Status = STATUS_ACCEPTING;
	m_ActiveType = ENL_ACTIVE_TYPE_PROACTIVE;
	SetServer(m_pManager->GetServer());
	m_ConnectionAddress = ConnectionAddress;

	m_MaxPacketSize = MaxPacketSize;
	if (m_MaxPacketSize + NET_DATA_BLOCK_SIZE > m_AssembleBuffer.GetBufferSize())
	{
		m_AssembleBuffer.Create(m_MaxPacketSize + NET_DATA_BLOCK_SIZE);
	}
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
		if (m_DataCompressType != CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE&&m_CompressBuffer.GetBufferSize() < m_MaxPacketSize)
		{
			m_CompressBuffer.Create(m_MaxPacketSize);
		}
	}

	if (Create(RecvQueueSize, SendQueueSize))
		return true;
	else
		return false;
}

bool CENLConnection::Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize, 
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_Status = STATUS_ACCEPTING;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;
	SetServer(m_pManager->GetServer());
	m_MaxPacketSize = MaxPacketSize;
	if(m_MaxPacketSize + NET_DATA_BLOCK_SIZE > m_AssembleBuffer.GetBufferSize())
	{
		m_AssembleBuffer.Create(m_MaxPacketSize + NET_DATA_BLOCK_SIZE);
	}	
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
		if (m_DataCompressType != CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE&&m_CompressBuffer.GetBufferSize() < m_MaxPacketSize)
		{
			m_CompressBuffer.Create(m_MaxPacketSize);
		}
	}
	return true;
}

int CENLConnection::Update(int ProcessPacketLimit)
{
	int Process = CNetConnection::Update(ProcessPacketLimit);
	if (m_KeepAliveCount > MAX_KEEP_ALIVE_COUNT)
	{
		PrintNetLog( _T("CENLConnection::Update 连接[%s]超时被断开"),
			CClassifiedID(m_pParent->GetID()).ToStr());
		Disconnect();
	}
	if (IsDisconnected())
	{
		if (m_ActiveType == ENL_ACTIVE_TYPE_PROACTIVE)
		{
			if (m_ReconnectTimer.IsTimeOut(EASY_LINK_RECONNECT_TIME))
			{
				m_ReconnectTimer.SaveTime();
				Connect(m_ConnectionAddress, LINK_CONNECT_TIME);
				PrintNetLog( _T("[%s]开始连接[%s:%u]"),
					CClassifiedID(m_pParent->GetReportID()).ToStr(),
					m_ConnectionAddress.GetIPString(),
					m_ConnectionAddress.GetPort());
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
		PrintNetLog( _T("拼包缓冲区溢出,连接断开"));
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
			PrintNetLog( _T("收到大小非法的消息,连接断开"));
			Disconnect();
			return;
		}
		switch (pMsg->Header.MsgID)
		{
		case EASY_NET_LINK_MSG_LINK_START:
			if (MsgSize >= sizeof(EASY_NET_LINK_MSG_HEAD) + sizeof(EASY_NET_LINK_INFO))
			{
				EASY_NET_LINK_INFO * pInfo = (EASY_NET_LINK_INFO *)pMsg->Data;
				if (m_pManager)
				{
					if (!m_pManager->AcceptLink(pInfo->ID, m_pParent))
					{
						PrintNetLog( _T("非法连接[%s][%s:%u]被拒绝"),
							CClassifiedID(pInfo->ID).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
						Disconnect();
					}
					else
					{
						m_Status = STATUS_ACCEPTED;
						PrintNetLog( _T("连接[%s][%s:%u]建立"),
							CClassifiedID(pInfo->ID).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
					}
				}
				else
				{
					PrintNetLog( _T("连接[%s][%s:%u]建立时没有设置管理器，被关闭"),
						CClassifiedID(pInfo->ID).ToStr(),
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
				m_pParent->OnData((BYTE *)pMsg->Data, pMsg->Header.Size - sizeof(EASY_NET_LINK_MSG_HEAD));
			}
			break;
		case EASY_NET_LINK_MSG_COMPRESSED_USER_DATA:
			if (MsgSize >= sizeof(EASY_NET_LINK_MSG_HEAD))
			{
				lzo_uint OutLen = m_CompressBuffer.GetBufferSize();
				int Result = lzo1x_decompress_safe(pData, DataSize, (BYTE *)m_CompressBuffer.GetBuffer(), &OutLen, s_LZOCompressWorkMemory);
				if (Result == LZO_E_OK)
				{
					m_pParent->OnData((BYTE *)m_CompressBuffer.GetBuffer(), (UINT)OutLen);
				}
				else
				{
					PrintNetLog(_T("lzo解压数据失败(%d)"),
						Result);
					Disconnect();
				}
			}
			break;
		default:
			PrintNetLog( _T("CENLConnection::OnRecvData 收到不明消息ID=%u,size=%u"),
				pMsg->Header.MsgID, MsgSize);
		}
		m_AssembleBuffer.PopFront(NULL, MsgSize);
	}
}

void CENLConnection::OnConnection(bool IsSucceed)
{
	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);

	m_Status = STATUS_ACCEPTING;

	if (IsSucceed)
	{
		PrintNetLog( _T("[%s]连接[%s:%u]成功"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());


		EASY_NET_LINK_INFO Info;
		Info.ID = m_pParent->GetReportID();
		SendLinkMsg(EASY_NET_LINK_MSG_LINK_START, &Info, sizeof(Info));


	}
	else
	{
		PrintNetLog( _T("[%s]连接[%s:%u]失败"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
	}
}

void CENLConnection::OnDisconnection()
{
	PrintNetLog( _T("[%s]连接[%s:%u]断开"),
		CClassifiedID(m_pParent->GetReportID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());

	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);

	

	if (m_Status == STATUS_ACCEPTED)
	{
		if (m_pManager)
			m_pManager->OnLinkEnd(m_pParent);
		m_pParent->OnLinkEnd();
	}

	m_Status = STATUS_ACCEPTING;
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

bool CENLConnection::SendData(LPCVOID pData, UINT DataSize)
{
	if (pData&&DataSize > 0)
	{
		EASY_NET_LINK_MSG_HEAD MsgHeader;
		MsgHeader.Size = sizeof(EASY_NET_LINK_MSG_HEAD) + DataSize;
		MsgHeader.MsgID = EASY_NET_LINK_MSG_USER_DATA;
		if (m_DataCompressType == CEasyNetLinkManager::DATA_COMPRESS_TYPE_LZO && DataSize >= m_MinCompressSize)
		{
			lzo_uint OutLen = m_CompressBuffer.GetBufferSize();
			int Result = lzo1x_1_compress((BYTE *)pData, DataSize,
				(BYTE *)m_CompressBuffer.GetBuffer(), &OutLen,
				s_LZOCompressWorkMemory);

			if (Result == LZO_E_OK)
			{
				MsgHeader.MsgID = EASY_NET_LINK_MSG_COMPRESSED_USER_DATA;
				pData = m_CompressBuffer.GetBuffer();
				DataSize = OutLen;
			}
			else
			{
				PrintNetLog(_T("lzo压缩数据失败(%d)，将直接发送"), Result);
			}
		}
			
		if (Send(&MsgHeader, sizeof(MsgHeader)))
		{
			if (pData&&DataSize)
			{
				return Send(pData, DataSize);
			}
			return true;
		}
	}
	else
	{
		PrintNetLog(_T("CEasyNetLinkConnection::SendData 发送的数据大小不合法"));
	}
	return false;
}

void CENLConnection::Disconnect()
{
	CNetConnection::Disconnect();
}
bool CENLConnection::IsDisconnected()
{
	return CNetConnection::IsDisconnected();
}
bool CENLConnection::IsConnected()
{
	return CNetConnection::IsDisconnected();
}
CBaseNetConnection * CENLConnection::GetBaseConnection()
{
	return dynamic_cast<CBaseNetConnection *>(this);
}