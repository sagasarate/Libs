/****************************************************************************/
/*                                                                          */
/*      文件名:    NetConnectionEpoll.cpp                                   */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

#undef  PrintNetLog
#define PrintNetLog(_Format, ...)	PrintConnectionLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#undef  PrintNetDebugLog
#define PrintNetDebugLog(_Format, ...)	PrintConnectionDebugLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)

IMPLEMENT_CLASS_INFO_STATIC(CNetConnectionNoBuff,CBaseNetConnection);


CNetConnectionNoBuff::CNetConnectionNoBuff(void)
{
	m_pServer=NULL;
	m_WantClose=false;
	m_IsEventProcessing = false;
	m_HaveDataToRecv = false;
	m_CurAddressFamily = AF_INET;
	m_UseSafeDisconnect=false;
	m_pEpollEventRouter=NULL;
	m_SendQueue.SetTag(_T("CNetConnectionNoBuff"));
	m_pRecvBuffer = NULL;
	m_RecvBufferParam = NULL;
	m_RecvBufferSize = 0;
}

CNetConnectionNoBuff::~CNetConnectionNoBuff(void)
{
	Destory();
}

bool CNetConnectionNoBuff::OnEpollEvent(UINT EventID)
{
	m_IsEventProcessing = true;
	if(m_Socket.IsConnected())
	{
		if(EventID&(EPOLLERR|EPOLLHUP))
		{
			int SocketErr = 0;
			socklen_t DataLen = sizeof(SocketErr);
			m_Socket.GetOption(SOL_SOCKET, SO_ERROR, (char *)&SocketErr, DataLen);
			if (SocketErr != 0 && SocketErr != EAGAIN)
			{
				PrintNetLog("(%u)Epoll发生错误%d！", GetID(), SocketErr);
				QueryDisconnect();
				m_IsEventProcessing = false;
				return true;
			}
		}
		if(EventID&EPOLLIN)
		{
			m_HaveDataToRecv = true;
		}
		if(EventID&EPOLLOUT)
		{
			DoSend();
		}
	}
	else
	{
		PrintNetLog("(%d)Connection未连接，Epoll事件被忽略！",GetID());
	}

	m_IsEventProcessing = false;
	return false;
}

bool CNetConnectionNoBuff::Create(UINT RecvQueueSize, UINT SendQueueSize)
{


	if (GetServer() == NULL)
	{
		PrintNetLog( _T("(%d)未设置Server！"), GetID());
		return false;
	}


	Close();

	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
		if (m_pEpollEventRouter == NULL)
		{
			PrintNetLog(_T("无法分配到EpollEventRouter"));
			return false;
		}
		m_pEpollEventRouter->Init(this);
	}

	if(m_SendQueue.GetBufferSize()<SendQueueSize)
	{
		m_SendQueue.Create(SendQueueSize);
	}
	else
	{
		m_SendQueue.Clear();
	}
	m_WantClose=false;

	return true;
}

bool CNetConnectionNoBuff::Create(SOCKET Socket, UINT RecvQueueSize, UINT SendQueueSize)
{


	if (GetServer() == NULL)
	{
		PrintNetLog( _T("(%d)未设置Server！"), GetID());
		return false;
	}


	Close();


	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
		if (m_pEpollEventRouter == NULL)
		{
			PrintNetLog(_T("无法分配到EpollEventRouter"));
			return false;
		}
		m_pEpollEventRouter->Init(this);
	}

	m_Socket.SetSocket(Socket);


	if(m_SendQueue.GetBufferSize()<SendQueueSize)
	{
		m_SendQueue.Create(SendQueueSize);
	}
	else
	{
		m_SendQueue.Clear();
	}
	m_WantClose=false;
	return true;
}

void CNetConnectionNoBuff::Destory()
{
	Close();
	m_SendQueue.Destory();
}

void CNetConnectionNoBuff::Close()
{
	if (IsConnected())
	{
		OnDisconnection();
	}

	//等待现有的事件处理完毕
	while (m_IsEventProcessing)
	{
		DoSleep(1);
	}

	//解除epoll事件注册
	if (GetServer() && m_pEpollEventRouter)
	{
		if (m_pEpollEventRouter->GetEpollThread())
		{
			if (GetServer()->UnbindSocket(m_Socket.GetSocket(), m_pEpollEventRouter))
			{
				PrintNetDebugLog("(%d)已解除Epoll绑定", GetID());
			}
		}

		m_pEpollEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pEpollEventRouter);
		m_pEpollEventRouter = NULL;
	}

	m_Socket.Close();

	m_WantClose = false;
	m_HaveDataToRecv = false;
	m_PacketHeaderBuffer.SetUsedSize(0);
	if (m_pRecvBuffer)
	{
		ReleaseRecvBuffer(m_pRecvBuffer, m_RecvBufferParam);
		m_pRecvBuffer = NULL;
	}
	SEND_BUFFER SendBuffer;
	while (m_SendQueue.PopFront(&SendBuffer))
	{
		ReleaseSendBuffer(SendBuffer.pData, SendBuffer.Param);
	}
}

bool CNetConnectionNoBuff::Connect(const CIPAddress& Address, DWORD TimeOut)
{
	if(GetServer()==NULL)
		return false;

	if (Address.IsIPv4())
		m_CurAddressFamily = AF_INET;
	else if (Address.IsIPv6())
		m_CurAddressFamily = AF_INET6;

	if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP))
		return false;

	m_pEpollEventRouter->Init(this);

	if(TimeOut==NO_CONNECTION_TIME_OUT)
	{
		if(m_Socket.Connect(Address))
		{
			StartWork();
			return true;
		}
	}
	else
	{
		if(m_Socket.ConnectNoBlocking(Address,TimeOut))
		{
			return true;
		}
	}
	return false;
}
void CNetConnectionNoBuff::Disconnect()
{
	Close();
}
void CNetConnectionNoBuff::QueryDisconnect()
{
	if(!m_WantClose)
	{
		if (GetServer() && m_pEpollEventRouter)
		{
			if (m_pEpollEventRouter->GetEpollThread())
			{
				if (GetServer()->UnbindSocket(m_Socket.GetSocket(), m_pEpollEventRouter))
				{
					PrintNetDebugLog("(%d)已解除Epoll绑定", GetID());
				}
			}
		}
	}
	m_WantClose=true;
}



bool CNetConnectionNoBuff::StartWork()
{
	//PrintNetLog("(%d)Connection开始工作",GetID());

	m_pEpollEventRouter->Init(this);

	m_Socket.SetState(CNetSocket::SS_CONNECTED);

	if(!m_Socket.EnableBlocking(false))
	{
		OnConnection(false);
		PrintNetLog("(%d)开始非阻塞模式失败！",GetID());
		m_Socket.Close();
		return false;
	}

	if (!GetServer()->BindSocket(m_Socket.GetSocket(), m_pEpollEventRouter, true))
	{
		OnConnection(false);
		PrintNetLog("(%d)绑定Epoll失败！", GetID());
		m_Socket.Close();
		return false;
	}

	OnConnection(true);
	return true;
}




bool CNetConnectionNoBuff::SendMulti(LPCVOID * pDataBuffers, const UINT * pDataSizes, LPCVOID * pParams, UINT BufferCount)
{
	CAutoLockEx Lock(m_SendLock);

	if(m_Socket.IsConnected())
	{
		SEND_BUFFER SendBuffer;
		SendBuffer.SendedSize = 0;
		for (UINT i = 0; i < BufferCount; i++)
		{
			SendBuffer.pData = (BYTE *)pDataBuffers[i];
			SendBuffer.DataSize = pDataSizes[i];
			SendBuffer.Param = pParams[i];
			if (!m_SendQueue.PushBack(&SendBuffer))
			{
				PrintNetLog("(%d)发送队列溢出！", GetID());
				QueryDisconnect();
				return false;
			}
		}
		DoSend();
		return true;
	}
	return false;
}

bool CNetConnectionNoBuff::AllocRecvBuffer(const BYTE * pPacketHeader, UINT HeaderSize, LPBYTE& pBuffer, UINT& BufferSize, LPCVOID& Param)
{

}

void CNetConnectionNoBuff::ReleaseRecvBuffer(BYTE * pBuffer, LPCVOID Param)
{

}

void CNetConnectionNoBuff::ReleaseSendBuffer(LPCVOID pBuffer, LPCVOID Param)
{

}

int CNetConnectionNoBuff::Update(int ProcessPacketLimit)
{

	int PacketCount=0;
	//处理Connect
	if(m_Socket.GetState()==CNetSocket::SS_CONNECTING)
	{
		m_Socket.Connected();
		if(m_Socket.GetState()==CNetSocket::SS_CONNECTED)
		{
			StartWork();
		}
		if(m_Socket.GetState()==CNetSocket::SS_UNUSED)
		{
			OnConnection(false);
		}
	}
	else if(m_HaveDataToRecv)
	{
		DoRecv();
		m_HaveDataToRecv = false;
	}

	//处理关闭
	if(m_WantClose)
	{
		if(m_UseSafeDisconnect)
		{
			if(m_SendQueue.GetUsedSize()<=0)
			{
				Disconnect();
			}
		}
		else
		{
			Disconnect();
		}
	}
	return PacketCount;
}


void CNetConnectionNoBuff::DoRecv()
{
	if (m_pRecvBuffer)
	{
		int RecvSize = recv(m_Socket.GetSocket(),
			m_pRecvBuffer + m_RecvedDataSize,
			m_RecvBufferSize - m_RecvedDataSize, 0);
		m_RecvedDataSize += RecvSize;
		if (m_RecvedDataSize >= m_RecvBufferSize)
		{
			OnRecvData(m_pRecvBuffer, m_RecvBufferSize, m_RecvBufferParam);
			ReleaseRecvBuffer(m_pRecvBuffer, m_RecvBufferParam);
			m_pRecvBuffer = NULL;
		}
	}
	else
	{
		int RecvSize = recv(m_Socket.GetSocket(),
			((BYTE *)m_PacketHeaderBuffer.GetBuffer()) + m_PacketHeaderBuffer.GetUsedSize(),
			m_PacketHeaderBuffer.GetBufferSize() - m_PacketHeaderBuffer.GetUsedSize(), 0);
		m_PacketHeaderBuffer.PushBack(NULL, RecvSize);
		if (m_PacketHeaderBuffer.GetUsedSize() >= m_PacketHeaderBuffer.GetBufferSize())
		{
			if (AllocRecvBuffer((BYTE *)m_PacketHeaderBuffer.GetBuffer(), m_PacketHeaderBuffer.GetUsedSize(), m_pRecvBuffer, m_RecvBufferSize, m_RecvBufferParam))
			{
				memcpy(m_pRecvBuffer, m_PacketHeaderBuffer.GetBuffer(), m_PacketHeaderBuffer.GetUsedSize());
				m_RecvedDataSize = m_PacketHeaderBuffer.GetUsedSize();
				m_PacketHeaderBuffer.SetUsedSize(0);
			}
			else
			{
				PrintNetLog("分配接收缓冲失败！");
				Disconnect();
				return;
			}
		}
	}
}
void CNetConnectionNoBuff::DoSend()
{
	CAutoLockEx Lock(m_SendLock);

	SEND_BUFFER * pSendBuffer = m_SendQueue.GetUsedTop();
	while (pSendBuffer)
	{
		UINT SendSize = TrySend(pSendBuffer->pData + pSendBuffer->SendedSize, pSendBuffer->DataSize - pSendBuffer->SendedSize);
		pSendBuffer->SendedSize += SendSize;
		if (pSendBuffer->SendedSize >= pSendBuffer->DataSize)
		{
			ReleaseSendBuffer(pSendBuffer->pData, pSendBuffer->Param);
			m_SendQueue.PopFront(NULL);
		}
		else
		{
			//数据发送不完，退出等下一次发送事件
			break;
		}
		pSendBuffer = m_SendQueue.GetUsedTop();
	}
}

UINT CNetConnectionNoBuff::TrySend(LPCVOID pData, UINT Size)
{
	int SendSize = send(
		m_Socket.GetSocket(),
		pData,
		Size,
		0);
	if (SendSize >= 0)
	{
		GetServer()->AddTCPSendBytes(SendSize);
		return (UINT)SendSize;
	}
	else
	{
		int ErrorCode = errno;
		if (ErrorCode == EAGAIN)
		{
			return 0;
		}
		else
		{
			PrintNetLog( "Send失败(%u)", ErrorCode);
			QueryDisconnect();
			return 0;
		}

	}
}

