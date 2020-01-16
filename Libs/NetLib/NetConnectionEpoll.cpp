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

IMPLEMENT_CLASS_INFO_STATIC(CNetConnection,CBaseNetConnection);


CNetConnection::CNetConnection(void)
{
	m_pServer=NULL;
	m_WantClose=false;
	m_IsEventProcessing = false;
	m_CurAddressFamily = AF_INET;
	m_UseSafeDisconnect=false;
	m_pEpollEventRouter=NULL;
	m_RecvQueue.SetTag(_T("CNetConnection"));
	m_SendQueue.SetTag(_T("CNetConnection"));
}

CNetConnection::~CNetConnection(void)
{
	Destory();
}

bool CNetConnection::OnEpollEvent(UINT EventID)
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
			DoRecv();
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

bool CNetConnection::Create(UINT RecvQueueSize, UINT SendQueueSize)
{


	if (GetServer() == NULL)
	{
		PrintNetLog( _T("(%d)CNetConnection::Create:未设置Server！"), GetID());
		return false;
	}


	Close();

	RecvQueueSize *= NET_DATA_BLOCK_SIZE;
	SendQueueSize *= NET_DATA_BLOCK_SIZE;

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

	if(m_RecvQueue.GetBufferSize()<RecvQueueSize)
	{
		m_RecvQueue.Create(RecvQueueSize);
	}
	else
	{
		m_RecvQueue.Clear();
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

bool CNetConnection::Create(SOCKET Socket, UINT RecvQueueSize, UINT SendQueueSize)
{


	if (GetServer() == NULL)
	{
		PrintNetLog( _T("(%d)CNetConnection::Create:未设置Server！"), GetID());
		return false;
	}


	Close();

	RecvQueueSize *= NET_DATA_BLOCK_SIZE;
	SendQueueSize *= NET_DATA_BLOCK_SIZE;


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

	if(m_RecvQueue.GetBufferSize()<RecvQueueSize)
	{
		m_RecvQueue.Create(RecvQueueSize);
	}
	else
	{
		m_RecvQueue.Clear();
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

void CNetConnection::Destory()
{
	Close();
	m_RecvQueue.Destory();
	m_SendQueue.Destory();
}

void CNetConnection::Close()
{
	Disconnect();
	//等待现有的事件处理完毕
	while (m_IsEventProcessing)
	{
		DoSleep(1);
	}
	if (m_pEpollEventRouter)
	{
		m_pEpollEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pEpollEventRouter);
		m_pEpollEventRouter = NULL;
	}
}

bool CNetConnection::Connect(const CIPAddress& Address, DWORD TimeOut)
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
void CNetConnection::Disconnect()
{
	//PrintNetLog("(%d)Connection关闭",GetID());

	//PrintNetLog("%s连接关闭",GetName());
	if (IsConnected())
	{
		OnDisconnection();
	}

	//解除epoll事件注册
	if (GetServer() && m_pEpollEventRouter)
	{
		if(m_pEpollEventRouter->GetEpollThread())
		{
			if (GetServer()->UnbindSocket(m_Socket.GetSocket(), m_pEpollEventRouter))
			{
				PrintNetDebugLog("(%d)Connection已解除Epoll绑定", GetID());
			}
		}
	}
		

	m_Socket.Close();

	m_WantClose=false;

	m_RecvQueue.Clear();
	m_SendQueue.Clear();

}
void CNetConnection::QueryDisconnect()
{
	if(!m_WantClose)
	{
		if (GetServer() && m_pEpollEventRouter)
		{
			if (m_pEpollEventRouter->GetEpollThread())
			{
				if (GetServer()->UnbindSocket(m_Socket.GetSocket(), m_pEpollEventRouter))
				{
					PrintNetDebugLog("(%d)Connection已解除Epoll绑定", GetID());
				}
			}
		}
	}		
	m_WantClose=true;	
}



bool CNetConnection::StartWork()
{
	//PrintNetLog("(%d)Connection开始工作",GetID());

	m_pEpollEventRouter->Init(this);

	m_Socket.SetState(CNetSocket::SS_CONNECTED);

	if(!m_Socket.EnableBlocking(false))
	{
		OnConnection(false);
		PrintNetLog("(%d)Connection开始非阻塞模式失败！",GetID());
		m_Socket.Close();
		return false;
	}

	if(!GetServer()->BindSocket(m_Socket.GetSocket(),m_pEpollEventRouter))
	{
		OnConnection(false);
		PrintNetLog("(%d)Connection绑定Epoll失败！",GetID());
		m_Socket.Close();
		return false;
	}

	//CEasyString ConnectionName;
	//ConnectionName.Format("[%s:%d]",GetRemoteAddress().GetIPString(),GetRemoteAddress().GetPort());
	//SetName(ConnectionName);;


	//PrintNetLog("%s连接建立[%u]",GetName(),(UINT)m_Socket.GetSocket());

	OnConnection(true);
	return true;
}




bool CNetConnection::SendMulti(LPCVOID * pDataBuffers, const UINT * pDataSizes, UINT BufferCount)
{
	CAutoLockEx Lock(m_SendLock);

	if(m_Socket.IsConnected())
	{
		for (UINT i = 0; i < BufferCount; i++)
		{
			LPCVOID pData = pDataBuffers[i];
			UINT Size = pDataSizes[i];
			if (m_SendQueue.GetUsedSize()>0)
			{
				//有数据未发完，直接加入缓冲区
				if (!m_SendQueue.PushBack(pData, Size))
				{
					PrintNetLog("发送缓冲溢出！");
					QueryDisconnect();
					return false;
				}
			}
			else
			{
				UINT SendSize = TrySend(pData, Size);
				if (SendSize < Size)
				{
					//发送不完，剩余数据加入缓冲区
					if (!m_SendQueue.PushBack((BYTE *)pData + SendSize, Size - SendSize))
					{
						PrintNetLog("发送缓冲溢出！");
						QueryDisconnect();
						return false;
					}
				}
			}
		}
		return true;
	}
	return false;
}



int CNetConnection::Update(int ProcessPacketLimit)
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
	else
	{
		UINT DataSize = m_RecvQueue.GetSmoothUsedSize();
		while (DataSize)
		{
			OnRecvData((BYTE*)m_RecvQueue.GetUsedBuffer(), DataSize);
			m_RecvQueue.PopFront((LPVOID)NULL, DataSize);
			DataSize = m_RecvQueue.GetSmoothUsedSize();
			PacketCount++;
			if (PacketCount >= ProcessPacketLimit)
				break;
		}
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


//bool CNetConnection::StealFrom(CNameObject * pObject,UINT Param)
//{
//	PrintNetLog("(%d)执行连接替换(%d)！",GetID(),pObject->GetID());
//	if(pObject->IsKindOf(GET_CLASS_INFO(CNetConnection)))
//	{
//		Close();
//
//		if(!CNameObject::StealFrom(pObject,Param))
//			return false;
//		CNetConnection * pConnection=(CNetConnection *)pObject;
//		if(!m_Socket.StealFrom(&(pConnection->m_Socket),Param))
//			return false;
//
//		m_pServer=pConnection->m_pServer;
//		m_WantClose=pConnection->m_WantClose;
//		m_pEpollEventRouter=pConnection->m_pEpollEventRouter;
//		pConnection->m_pEpollEventRouter=NULL;
//		if(m_pEpollEventRouter)
//			m_pEpollEventRouter->SetEventHander(this);
//
//		m_RecvQueue.CloneFrom(pConnection->m_RecvQueue);
//		m_SendQueue.CloneFrom(pConnection->m_SendQueue);
//		return true;
//
//	}
//	return false;
//}

void CNetConnection::DoRecv()
{
	while (true)
	{
		UINT BufferSize = m_RecvQueue.GetSmoothFreeSize();
		if (BufferSize <= 0)
		{
			PrintNetLog( "CNetConnection接收缓冲溢出！");
			QueryDisconnect();
			return;
		}
		LPVOID pBuffer = m_RecvQueue.GetFreeBuffer();
		int RecvSize=recv(
			m_Socket.GetSocket(),
			pBuffer,
			BufferSize,
			0);

		if(RecvSize>0)
		{
			GetServer()->AddTCPRecvBytes(RecvSize);
			m_RecvQueue.PushBack((LPVOID)NULL, RecvSize);
		}
		else if(RecvSize==0)
		{
			PrintNetLog("CNetConnection收到连接关闭信号！");
			QueryDisconnect();
			return;
		}
		else
		{
			int ErrorCode=errno;
			switch(ErrorCode)
			{
			case EAGAIN:
				return;
			default:
				PrintNetLog("CNetConnection::Recv失败(%u),Socket关闭",ErrorCode);
				QueryDisconnect();
				return;
			}
		}
	}
}
void CNetConnection::DoSend()
{
	CAutoLockEx Lock(m_SendLock);

	UINT DataSize = m_SendQueue.GetSmoothUsedSize();
	while (DataSize)
	{
		LPVOID pBuffer = m_SendQueue.GetUsedBuffer();
		UINT SendSize = TrySend(pBuffer, DataSize);
		if (SendSize > 0)
			m_SendQueue.PopFront((LPVOID)NULL, SendSize);
		if (SendSize < DataSize)
		{
			//数据发送不完，退出等下一次发送事件
			break;
		}			
		DataSize = m_SendQueue.GetSmoothUsedSize();
	}
}

UINT CNetConnection::TrySend(LPCVOID pData, UINT Size)
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

