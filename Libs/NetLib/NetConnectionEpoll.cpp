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

IMPLEMENT_CLASS_INFO_STATIC(CNetConnection,CBaseTCPConnection);


CNetConnection::CNetConnection(void)
{
	m_pServer=NULL;
	m_WantClose=FALSE;
	m_UseSafeDisconnect=false;
	m_pEpollEventRouter=NULL;
	m_EnableSendLock = false;
}

CNetConnection::~CNetConnection(void)
{
	Destory();
}

BOOL CNetConnection::OnEpollEvent(UINT EventID)
{
	if(m_Socket.IsConnected())
	{
		if(EventID&(EPOLLERR|EPOLLHUP))
		{
			PrintNetLog(0xffffffff,"CNetConnection::Epoll发生错误%d！",errno);
			QueryDisconnect();
			return TRUE;
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
		PrintNetLog(0xffffffff,"(%d)Connection未连接，Epoll事件被忽略！",GetID());
	}


	return FALSE;
}

BOOL CNetConnection::Create(UINT RecvQueueSize,UINT SendQueueSize)
{


	if(GetServer()==NULL)
		return FALSE;

	Destory();

	RecvQueueSize *= EPOLL_DATA_BLOCK_SIZE;
	SendQueueSize *= EPOLL_DATA_BLOCK_SIZE;

	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
		m_pEpollEventRouter->Init(this);
	}
	m_Socket.MakeSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

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
	m_WantClose=FALSE;

	return TRUE;
}

BOOL CNetConnection::Create(SOCKET Socket,UINT RecvQueueSize,UINT SendQueueSize)
{


	if(GetServer()==NULL)
		return FALSE;

	Destory();

	RecvQueueSize *= EPOLL_DATA_BLOCK_SIZE;
	SendQueueSize *= EPOLL_DATA_BLOCK_SIZE;


	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
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
	m_WantClose=FALSE;
	return TRUE;
}

void CNetConnection::Destory()
{
	if(IsConnected())
	{
		OnDisconnection();
	}

	m_Socket.Close();

	if(m_pEpollEventRouter)
	{
		m_pEpollEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pEpollEventRouter);
		m_pEpollEventRouter=NULL;
	}
	m_RecvQueue.Clear();
	m_SendQueue.Clear();

	m_WantClose=FALSE;
}

BOOL CNetConnection::Connect(const CIPAddress& Address ,DWORD TimeOut)
{
	if(GetServer()==NULL)
		return FALSE;

	if(m_Socket.GetState()==CNetSocket::SS_UNINITED)
	{
		if(!Create())
			return FALSE;
	}

	m_pEpollEventRouter->Init(this);

	if(TimeOut==NO_CONNECTION_TIME_OUT)
	{
		if(m_Socket.Connect(Address))
		{
			StartWork();
			return TRUE;
		}
	}
	else
	{
		if(m_Socket.ConnectNoBlocking(Address,TimeOut))
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CNetConnection::Disconnect()
{
	//PrintNetLog(0xffffffff,"(%d)Connection关闭",GetID());

	//PrintNetLog(0xffffffff,"%s连接关闭",GetName());


	m_Socket.Close();

	m_WantClose=FALSE;

	m_RecvQueue.Clear();
	m_SendQueue.Clear();

	OnDisconnection();
}
void CNetConnection::QueryDisconnect()
{
	m_WantClose=TRUE;
}



BOOL CNetConnection::StartWork()
{
	//PrintNetLog(0xffffffff,"(%d)Connection开始工作",GetID());

	m_pEpollEventRouter->Init(this);

	m_Socket.SetState(CNetSocket::SS_CONNECTED);

	if(!m_Socket.EnableBlocking(FALSE))
	{
		OnConnection(FALSE);
		PrintNetLog(0xffffffff,"(%d)Connection开始非阻塞模式失败！",GetID());
		m_Socket.Close();
		return FALSE;
	}

	if(!GetServer()->BindSocket(m_Socket.GetSocket(),m_pEpollEventRouter))
	{
		OnConnection(FALSE);
		PrintNetLog(0xffffffff,"(%d)Connection绑定Epoll失败！",GetID());
		m_Socket.Close();
		return FALSE;
	}

	//CEasyString ConnectionName;
	//ConnectionName.Format("[%s:%d]",GetRemoteAddress().GetIPString(),GetRemoteAddress().GetPort());
	//SetName(ConnectionName);;


	//PrintNetLog(0xffffffff,"%s连接建立[%u]",GetName(),(UINT)m_Socket.GetSocket());

	OnConnection(TRUE);
	return TRUE;
}

void CNetConnection::OnConnection(BOOL IsSucceed)
{

}

void CNetConnection::OnDisconnection()
{
}

BOOL CNetConnection::Send(LPCVOID pData,UINT Size)
{
	CAutoLockEx Lock;
	if (m_EnableSendLock)
	{
		Lock.Lock(m_SendLock);
	}


	if(m_Socket.IsConnected())
	{
		if (m_SendQueue.GetUsedSize()>0)
		{
			//有数据未发完，直接加入缓冲区，并注册发送事件，保证可靠
			m_SendQueue.PushBack(pData, Size);
			if (!GetServer()->ModifySendEvent(m_Socket.GetSocket(), m_pEpollEventRouter, true))
			{
				PrintNetLog(0xffffffff, "(%d)注册Epoll发送事件失败！", GetID());
				QueryDisconnect();
				return FALSE;
			}
		}
		else
		{
			UINT SendSize = TrySend(pData, Size);
			if (SendSize < Size)
			{
				//发送不完，剩余数据加入缓冲区，并注册发送事件
				m_SendQueue.PushBack(pData + SendSize, Size - SendSize);
				if (!GetServer()->ModifySendEvent(m_Socket.GetSocket(), m_pEpollEventRouter, true))
				{
					PrintNetLog(0xffffffff, "(%d)注册Epoll发送事件失败！", GetID());
					QueryDisconnect();
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
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
			OnConnection(FALSE);
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


bool CNetConnection::StealFrom(CNameObject * pObject,UINT Param)
{
	PrintNetLog(0xffffffff,"(%d)执行连接替换(%d)！",GetID(),pObject->GetID());
	if(pObject->IsKindOf(GET_CLASS_INFO(CNetConnection)))
	{
		Destory();

		if(!CNameObject::StealFrom(pObject,Param))
			return false;
		CNetConnection * pConnection=(CNetConnection *)pObject;
		if(!m_Socket.StealFrom(&(pConnection->m_Socket),Param))
			return false;

		m_pServer=pConnection->m_pServer;
		m_WantClose=pConnection->m_WantClose;
		m_pEpollEventRouter=pConnection->m_pEpollEventRouter;
		pConnection->m_pEpollEventRouter=NULL;
		if(m_pEpollEventRouter)
			m_pEpollEventRouter->SetEventHander(this);

		m_RecvQueue.CloneFrom(pConnection->m_RecvQueue);
		m_SendQueue.CloneFrom(pConnection->m_SendQueue);
		return true;

	}
	return false;
}

void CNetConnection::DoRecv()
{
	while (true)
	{
		UINT BufferSize = m_RecvQueue.GetSmoothFreeSize();
		if (BufferSize <= 0)
		{
			PrintNetLog(0xffffffff, "CNetConnection接收缓冲溢出！");
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
			PrintNetLog(0xffffffff,"CNetConnection收到连接关闭信号！");
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
				PrintNetLog(0xffffffff,"CNetConnection::Recv失败(%u),Socket关闭",ErrorCode);
				QueryDisconnect();
				return;
			}
		}
	}
}
void CNetConnection::DoSend()
{
	UINT DataSize = m_SendQueue.GetSmoothUsedSize();
	while (DataSize)
	{
		LPVOID pBuffer = m_SendQueue.GetUsedBuffer();
		UINT SendSize = TrySend(pBuffer, DataSize);
		if (SendSize < DataSize)
		{
			//数据发送不完，退出等下一次发送事件
			break;
		}
		else
		{
			if (SendSize >= m_SendQueue.GetUsedSize())
			{
				//所有数据已发送完毕，注销发送事件
				if (!GetServer()->ModifySendEvent(m_Socket.GetSocket(), m_pEpollEventRouter, false))
				{
					PrintNetLog(0xffffffff, "(%d)注销Epoll发送事件失败！", GetID());
					QueryDisconnect();
				}
			}
		}
		if (SendSize>0)
			m_SendQueue.PopFront((LPVOID)NULL, SendSize);
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
			PrintNetLog(0xffffffff, "Send失败(%u)", ErrorCode);
			QueryDisconnect();
			return 0;
		}

	}
}

