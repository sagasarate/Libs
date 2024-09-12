/****************************************************************************/
/*                                                                          */
/*      文件名:    CENLConnectionNoBuff.cpp                                     */
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

CENLConnectionNoBuff::CENLConnectionNoBuff()
{
	m_pManager = NULL;
	m_pParent = NULL;
	m_Status = STATUS_ACCEPTING;
	m_KeepAliveCount = 0;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;

	m_MaxPacketSize = 0;
	m_DataCompressType = CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE;
	m_MinCompressSize = CEasyNetLinkManager::DEFAULT_MIN_COMPRESS_SIZE;
}


CENLConnectionNoBuff::~CENLConnectionNoBuff()
{
	Destory();
}

void CENLConnectionNoBuff::Release()
{
	CNetConnectionNoBuff::Release();
}

UINT CENLConnectionNoBuff::AddUseRef()
{
	return CNetConnectionNoBuff::AddUseRef();
}

bool CENLConnectionNoBuff::Init(CDOSRouterLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress,
	UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_Status = STATUS_ACCEPTING;
	m_ActiveType = ENL_ACTIVE_TYPE_PROACTIVE;
	SetPacketHeaderSize(sizeof(DWORD));
	SetServer(m_pManager->GetServer());
	m_ConnectionAddress = ConnectionAddress;	
	m_MaxPacketSize = MaxPacketSize;
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
	}
	if (Create(RecvQueueSize, SendQueueSize))
		return true;
	else
		return false;
}

bool CENLConnectionNoBuff::Init(CDOSRouterLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize,
	CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail)
{
	m_pManager = pManager;
	m_pParent = pParent;
	m_Status = STATUS_ACCEPTING;
	m_ActiveType = ENL_ACTIVE_TYPE_PASSIVE;
	SetPacketHeaderSize(sizeof(DWORD));
	SetServer(m_pManager->GetServer());	
	m_MaxPacketSize = MaxPacketSize;
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
	}
	return true;
}

int CENLConnectionNoBuff::Update(int ProcessPacketLimit)
{
	int Process = CNetConnectionNoBuff::Update(ProcessPacketLimit);
	if (m_KeepAliveCount > MAX_KEEP_ALIVE_COUNT)
	{
		PrintNetLog( _T("CENLConnectionNoBuff::Update 连接[%s]超时被断开"),
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
				SendLinkMsg(DSM_PROXY_KEEP_ALIVE_PING);
			}
			m_KeepAliveTimer.SaveTime();
		}
	}	
	return Process;
}

bool CENLConnectionNoBuff::AllocRecvBuffer(const BYTE * pPacketHeader, UINT HeaderSize, LPBYTE& pBuffer, UINT& BufferSize, LPCVOID& Param)
{
	UINT MsgSize = *((MSG_LEN_TYPE *)pPacketHeader);
	CDOSMessagePacket * pPacket = m_pManager->GetRouter()->GetServer()->NewMessagePacket(MsgSize);
	if (pPacket)
	{
		pBuffer = (BYTE *)pPacket->GetPacketData();
		BufferSize = MsgSize;
		Param = pPacket;
		return true;
	}
	return false;
}
void CENLConnectionNoBuff::ReleaseRecvBuffer(BYTE * pBuffer, LPCVOID Param)
{
	CDOSMessagePacket * pPacket = (CDOSMessagePacket *)Param;
	m_pManager->GetRouter()->GetServer()->ReleaseMessagePacket(pPacket);
}
void CENLConnectionNoBuff::ReleaseSendBuffer(LPCVOID pBuffer, LPCVOID Param)
{
	CDOSMessagePacket * pPacket = (CDOSMessagePacket *)Param;
	m_pManager->GetRouter()->GetServer()->ReleaseMessagePacket(pPacket);
}

void CENLConnectionNoBuff::OnRecvData(const BYTE * pData, UINT DataSize, LPCVOID Param)
{
	if (DataSize < sizeof(CDOSMessagePacket::CaculatePacketLength(0, 0)))
	{
		PrintNetLog(_T("收到大小非法的消息,连接断开"));
		Disconnect();
		return;
	}

	CDOSMessagePacket * pPacket = (CDOSMessagePacket *)Param;
		
	switch (pPacket->GetMessage().GetMsgID())
	{
	case DSM_ROUTE_LINK_START:
		if (pPacket->GetMessage().GetDataLength() >= sizeof(EASY_NET_LINK_INFO))
		{
			EASY_NET_LINK_INFO * pInfo = (EASY_NET_LINK_INFO *)pPacket->GetMessage().GetMsgData();
			if (m_pManager)
			{
				if (!m_pManager->AcceptLink(pInfo->ID, m_pParent))
				{
					PrintNetLog(_T("非法连接[%s][%s:%u]被拒绝"),
						CClassifiedID(pInfo->ID).ToStr(),
						GetRemoteAddress().GetIPString(),
						GetRemoteAddress().GetPort());
					Disconnect();
				}
				else
				{
					m_Status = STATUS_ACCEPTED;
					PrintNetLog(_T("连接[%s][%s:%u]建立"),
						CClassifiedID(pInfo->ID).ToStr(),
						GetRemoteAddress().GetIPString(),
						GetRemoteAddress().GetPort());
				}
			}
			else
			{
				PrintNetLog(_T("连接[%s][%s:%u]建立时没有设置管理器，被关闭"),
					CClassifiedID(pInfo->ID).ToStr(),
					GetRemoteAddress().GetIPString(),
					GetRemoteAddress().GetPort());
				Disconnect();
			}

		}
		break;
	case DSM_PROXY_KEEP_ALIVE_PING:
		if (m_pParent->GetStatus() == ENL_LINK_ACCEPTED)
		{
			m_KeepAliveCount = 0;
			if (m_ActiveType == ENL_ACTIVE_TYPE_PASSIVE)
			{
				SendLinkMsg(DSM_PROXY_KEEP_ALIVE_PING);
			}
		}
		break;	
	default:
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_COMPRESSED)
		{
			BYTE * pData = (BYTE *)pPacket->GetMessage().GetMsgData();
			UINT DataLen = pPacket->GetPacketLength() - sizeof(MSG_LEN_TYPE) - CDOSMessage::GetMsgHeaderLength();
			if (DataLen >= sizeof(UINT))
			{
				UINT OrginSize = *((UINT *)pData);
				pData += sizeof(UINT);
				DataLen -= sizeof(UINT);
				CDOSMessagePacket * pNewPacket = m_pManager->GetRouter()->GetServer()->NewMessagePacket(OrginSize + sizeof(MSG_LEN_TYPE) + CDOSMessage::GetMsgHeaderLength());
				if (pNewPacket)
				{
					pNewPacket->GetMessage().GetMsgHeader() = pPacket->GetMessage().GetMsgHeader();
					lzo_uint OutLen = OrginSize - sizeof(MSG_LEN_TYPE) - CDOSMessage::GetMsgHeaderLength();
					int Result = lzo1x_decompress_safe(pData, DataSize, (BYTE *)pNewPacket->GetMessage().GetMsgData(), &OutLen, s_LZOCompressWorkMemory);
					if (Result == LZO_E_OK)
					{
						((CDOSRouterLink *)m_pParent)->OnMsgPacket(pNewPacket);
						m_pManager->GetRouter()->GetServer()->ReleaseMessagePacket(pNewPacket);
					}
					else
					{
						PrintNetLog(_T("lzo解压数据失败(%d)"),
							Result);
						Disconnect();
					}
				}
				else
				{
					PrintNetLog(_T("无法创建消息包"));
					Disconnect();
				}
			}
			else
			{
				PrintNetLog(_T("收到大小非法的压缩消息,连接断开"));
				Disconnect();
			}
		}
		else
		{
			((CDOSRouterLink *)m_pParent)->OnMsgPacket(pPacket);
		}
	}
}

void CENLConnectionNoBuff::OnConnection(bool IsSucceed)
{
	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();

	m_Status = STATUS_ACCEPTING;

	if (IsSucceed)
	{
		PrintNetLog( _T("[%s]连接[%s:%u]成功"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());


		EASY_NET_LINK_INFO Info;
		Info.ID = m_pParent->GetReportID();
		SendLinkMsg(DSM_ROUTE_LINK_START, &Info, sizeof(Info));


	}
	else
	{
		PrintNetLog( _T("[%s]连接[%s:%u]失败"),
			CClassifiedID(m_pParent->GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
	}
}

void CENLConnectionNoBuff::OnDisconnection()
{
	PrintNetLog( _T("[%s]连接[%s:%u]断开"),
		CClassifiedID(m_pParent->GetReportID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());

	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount = 0;
	m_KeepAliveTimer.SaveTime();

	

	if (m_Status == STATUS_ACCEPTED)
	{
		if (m_pManager)
			m_pManager->OnLinkEnd(m_pParent);
		m_pParent->OnLinkEnd();
	}

	m_Status = STATUS_ACCEPTING;
}

void CENLConnectionNoBuff::SendLinkMsg(DWORD MsgID, LPCVOID pData, UINT DataSize)
{
	CDOSMessagePacket * pPacket = m_pManager->GetRouter()->GetServer()->NewMessagePacket(CDOSMessagePacket::CaculatePacketLength(DataSize,0));
	if (pPacket)
	{
		pPacket->GetMessage().SetMsgID(MsgID);
		pPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
		pPacket->GetMessage().SetData(pData, DataSize);
		Send(pPacket->GetPacketData(), pPacket->GetPacketLength(), pPacket);
	}
}

bool CENLConnectionNoBuff::SendPacket(CDOSMessagePacket * pPacket)
{
	UINT DataSize = pPacket->GetPacketLength() - sizeof(MSG_LEN_TYPE) - CDOSMessage::GetMsgHeaderLength();
	BYTE * pData = (BYTE *)pPacket->GetMessage().GetMsgData();
	pPacket->IncRefCount();
	if (m_DataCompressType == CEasyNetLinkManager::DATA_COMPRESS_TYPE_LZO && DataSize >= m_MinCompressSize)
	{
		lzo_uint OutLen = DataSize + 32;
		CDOSMessagePacket * pNewPacket = m_pManager->GetRouter()->GetServer()->NewMessagePacket(CDOSMessagePacket::CaculatePacketLength(OutLen, 0));
		if (pNewPacket)
		{
			int Result = lzo1x_1_compress((BYTE *)pData, DataSize,
				(BYTE *)pNewPacket->GetMessage().GetMsgData(), &OutLen,
				s_LZOCompressWorkMemory);

			if (Result == LZO_E_OK)
			{
				pNewPacket->GetMessage().GetMsgHeader() = pPacket->GetMessage().GetMsgHeader();
				pNewPacket->GetMessage().SetMsgFlag(pPacket->GetMessage().GetMsgFlag() | DOS_MESSAGE_FLAG_COMPRESSED);
				pNewPacket->SetPacketLength(sizeof(MSG_LEN_TYPE) + CDOSMessage::GetMsgHeaderLength() + OutLen);
				pPacket->DecRefCount();
				pPacket = pNewPacket;
			}
			else
			{
				PrintNetLog(_T("lzo压缩数据失败(%d)，将直接发送"), Result);
			}
		}
		else
		{
			PrintNetLog(_T("无法创建消息包，将直接发送"));
		}
	}

	return Send(pPacket->GetPacketData(), pPacket->GetPacketLength(), pPacket);
}

void CENLConnectionNoBuff::Disconnect()
{
	CNetConnectionNoBuff::Disconnect();
}
bool CENLConnectionNoBuff::IsDisconnected()
{
	return CNetConnectionNoBuff::IsDisconnected();
}
bool CENLConnectionNoBuff::IsConnected()
{
	return CNetConnectionNoBuff::IsDisconnected();
}
CBaseNetConnection * CENLConnectionNoBuff::GetBaseConnection()
{
	return dynamic_cast<CBaseNetConnection *>(this);
}