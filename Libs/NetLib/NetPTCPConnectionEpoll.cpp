/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPTCPConnectionEpoll.cpp                               */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CNetPTCPConnection,CBaseTCPConnection);


CNetPTCPConnection::CNetPTCPConnection(void)
{
	m_pServer=NULL;
	m_pEpollEventRouter=NULL;
}

CNetPTCPConnection::~CNetPTCPConnection(void)
{
	Destory();
}

BOOL CNetPTCPConnection::OnEpollEvent(UINT EventID)
{
	if(m_Socket.IsConnected())
	{
		if(EventID&(EPOLLERR|EPOLLHUP))
		{
			PrintNetLog(0xffffffff,"CNetPTCPConnection::Epoll发生错误%d！",errno);
			Disconnect();
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

BOOL CNetPTCPConnection::Create(UINT RecvQueueSize,UINT SendQueueSize)
{


	if(GetServer()==NULL)
		return FALSE;

	Destory();


	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
		m_pEpollEventRouter->Init(this);
	}
	m_Socket.MakeSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);


	if(m_SendQueue.GetBufferSize()<SendQueueSize)
	{
		m_SendQueue.Create(SendQueueSize);
	}
	else
	{
		m_SendQueue.Clear();
	}

	return TRUE;
}

BOOL CNetPTCPConnection::Create(SOCKET Socket,UINT RecvQueueSize,UINT SendQueueSize)
{


	if(GetServer()==NULL)
		return FALSE;

	Destory();


	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
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
	return TRUE;
}

void CNetPTCPConnection::Destory()
{
	if(IsConnected())
	{
		m_Socket.Close();
		OnDisconnection();
	}
	else
	{
		m_Socket.Close();
	}



	if(m_pEpollEventRouter)
	{
		m_pEpollEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pEpollEventRouter);
		m_pEpollEventRouter=NULL;
	}

	CEpollEventObject * pEpollEventObject;
	while(m_SendQueue.PopFront(pEpollEventObject))
	{
		GetServer()->DeleteEventObject(pEpollEventObject);
	}

}

BOOL CNetPTCPConnection::Connect(const CIPAddress& Address ,DWORD TimeOut)
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
void CNetPTCPConnection::Disconnect()
{
	//PrintNetLog(0xffffffff,"(%d)Connection关闭",GetID());

	//PrintNetLog(0xffffffff,"%s连接关闭",GetName());


	m_Socket.Close();


	CEpollEventObject * pEpollEventObject;
	while(m_SendQueue.PopFront(pEpollEventObject))
	{
		GetServer()->DeleteEventObject(pEpollEventObject);
	}

	OnDisconnection();
}

BOOL CNetPTCPConnection::StartWork()
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

void CNetPTCPConnection::OnConnection(BOOL IsSucceed)
{

}

void CNetPTCPConnection::OnDisconnection()
{
}

BOOL CNetPTCPConnection::QuerySend(LPCVOID pData, UINT Size)
{
	//CAutoLock Lock(m_EasyCriticalSection);


	if(m_Socket.IsConnected())
	{
		while(Size)
		{
			int PacketSize=Size;
			if(PacketSize>NET_DATA_BLOCK_SIZE)
				PacketSize=NET_DATA_BLOCK_SIZE;
			Size-=PacketSize;

			CEpollEventObject * pEpollEventObject=GetServer()->CreateEventObject();
			if(pEpollEventObject==NULL)
			{
				PrintNetLog(0xffffffff,"Connection创建Send用OverLappedObject失败！");
				return FALSE;
			}

			pEpollEventObject->SetType(IO_SEND);
			pEpollEventObject->GetDataBuff()->SetUsedSize(0);
			pEpollEventObject->SetParentID(GetID());

			if(!pEpollEventObject->GetDataBuff()->PushBack(pData,PacketSize))
			{
				GetServer()->DeleteEventObject(pEpollEventObject);
				PrintNetLog(0xffffffff,"Connection要发送的数据包过大！");
				return FALSE;
			}
			pData=(char *)pData+PacketSize;

			if(!m_SendQueue.PushBack(pEpollEventObject))
			{
				PrintNetLog(0xffffffff,"CNetPTCPConnection::发送队列已满！");
				GetServer()->DeleteEventObject(pEpollEventObject);
				Disconnect();
				return FALSE;
			}
		}
		DoSend();
		return TRUE;
	}
	return FALSE;
}



void CNetPTCPConnection::OnRecvData(const CEasyBuffer& DataBuffer)
{
}

int CNetPTCPConnection::Update(int ProcessPacketLimit)
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

	return PacketCount;
}

void CNetPTCPConnection::DoRecv()
{
	CAutoLock Lock(m_RecvLock);

	while(true)
	{
		CEpollEventObject * pEpollEventObject=GetServer()->CreateEventObject();
		if(pEpollEventObject)
		{
			pEpollEventObject->SetType(IO_RECV);
			pEpollEventObject->SetParentID(GetID());

			int RecvSize=recv(
				m_Socket.GetSocket(),
				pEpollEventObject->GetDataBuff()->GetBuffer(),
				pEpollEventObject->GetDataBuff()->GetBufferSize(),
				0);

			if(RecvSize>0)
			{
				GetServer()->AddTCPRecvBytes(RecvSize);
				pEpollEventObject->GetDataBuff()->SetUsedSize(RecvSize);
				OnRecvData(*(pEpollEventObject->GetDataBuff()));
				GetServer()->DeleteEventObject(pEpollEventObject);
				continue;

			}
			else if(RecvSize==0)
			{
				GetServer()->DeleteEventObject(pEpollEventObject);
				PrintNetLog(0xffffffff,"CNetPTCPConnection收到连接关闭信号！");
				Disconnect();
				return;
			}
			else
			{
				GetServer()->DeleteEventObject(pEpollEventObject);

				int ErrorCode=errno;
				switch(ErrorCode)
				{
				case EAGAIN:
					return;
				default:
					PrintNetLog(0xffffffff,"CNetPTCPConnection::Recv失败(%u),Socket关闭",ErrorCode);
					Disconnect();
					return;
				}
			}
		}
		else
		{
			PrintNetLog(0xffffffff,"CNetPTCPConnection创建Recv用EpollEventObject失败！");
			Disconnect();
			return;
		}
	}
}
void CNetPTCPConnection::DoSend()
{
	CAutoLock Lock(m_SendLock);

	CEpollEventObject * pEpollEventObject=NULL;
	while(m_SendQueue.PopFront(pEpollEventObject))
	{
		int SendSize=send(
			m_Socket.GetSocket(),
			pEpollEventObject->GetDataBuff()->GetBuffer(),
			pEpollEventObject->GetDataBuff()->GetUsedSize(),
			0);
		if(SendSize>=0)
		{
			GetServer()->AddTCPSendBytes(SendSize);
			if(SendSize<(int)pEpollEventObject->GetDataBuff()->GetUsedSize())
			{
				pEpollEventObject->GetDataBuff()->PopFront(NULL,SendSize);
				m_SendQueue.PushFront(pEpollEventObject);
				return;
			}
		}
		else
		{
			int ErrorCode=errno;
			if(ErrorCode==EAGAIN)
			{
				m_SendQueue.PushFront(pEpollEventObject);
				return;
			}
			else
			{
				PrintNetLog(0xffffffff,"Send失败(%u)",ErrorCode);
				Disconnect();
				return;
			}

		}
		GetServer()->DeleteEventObject(pEpollEventObject);
	}
}


