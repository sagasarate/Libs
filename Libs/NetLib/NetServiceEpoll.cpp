/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServiceEpoll.cpp                                      */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CNetService,CBaseNetService);

CNetService::CNetService()
{
	m_WantClose=FALSE;
	m_pServer=NULL;
	m_CurProtocol=IPPROTO_TCP;
	m_CurAddressFamily = AF_INET;
	m_IPv6Only = false;
	m_AcceptQueueSize=0;
	m_RecvQueueSize=0;
	m_SendQueueSize=0;
	m_ParallelAcceptCount=0;
	m_ParallelRecvCount=0;
	m_pEpollEventRouter=NULL;

}

CNetService::~CNetService(void)
{
	Destory();
}

BOOL CNetService::OnEpollEvent(UINT EventID)
{
	if(IsWorking())
	{
		if(EventID&(EPOLLERR|EPOLLHUP))
		{
			PrintNetLog(_T("NetLib"),"CNetService::Epoll发生错误%d！",errno);
			QueryClose();
			return TRUE;
		}
		if(EventID&EPOLLIN)
		{
			if(m_CurProtocol==IPPROTO_UDP)
				DoUDPRecv();
			else
				DoAcceptSocket();
		}
		if(EventID&EPOLLOUT)
		{
			DoUDPSend();
		}
	}
	else
		PrintNetLog(_T("NetLib"),"Service未启用，事件被忽略！");

	return FALSE;
}

BOOL CNetService::Create(int Protocol, int AcceptQueueSize, int RecvQueueSize, int SendQueueSize, int ParallelAcceptCount, int ParallelRecvCount, bool IsUseListenThread, bool IPv6Only)
{
	if(GetServer()==NULL)
		return FALSE;

	Close();

	if(m_pEpollEventRouter==NULL)
	{
		m_pEpollEventRouter=GetServer()->CreateEventRouter();
		m_pEpollEventRouter->Init(this);
	}
	m_CurProtocol = Protocol;
	m_AcceptQueueSize=AcceptQueueSize;
	m_RecvQueueSize=RecvQueueSize;
	m_SendQueueSize=SendQueueSize;
	m_ParallelAcceptCount=ParallelAcceptCount;
	m_ParallelRecvCount=ParallelRecvCount;	
	m_IPv6Only = IPv6Only;
	m_WantClose=FALSE;
	

	m_Socket.EnableBlocking(FALSE);
	return TRUE;
}

void CNetService::Destory()
{
	Close();
}


BOOL CNetService::StartListen(const CIPAddress& Address)
{
	if (Address.IsIPv4())
		m_CurAddressFamily = AF_INET;
	else if (Address.IsIPv6())
		m_CurAddressFamily = AF_INET6;

	if (m_CurProtocol == IPPROTO_UDP)
	{
		if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_DGRAM, IPPROTO_UDP))
			return FALSE;
		m_RecvQueue.Create(m_RecvQueueSize);
		m_SendQueue.Create(m_SendQueueSize);
	}
	else if (m_CurProtocol == IPPROTO_TCP)
	{
		if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP))
			return FALSE;
		m_RecvQueue.Create(m_AcceptQueueSize);
	}
	else
	{
		PrintNetLog(_T("NetLib"), _T("(%d)Service协议错误%d！"), GetID(), m_CurProtocol);
		return FALSE;
	}

	if (!m_IPv6Only)
	{
		//启用同时侦听IPv6和IPv4
		DWORD Value = 0;
		socklen_t Len = sizeof(Value);
		m_Socket.SetOption(IPPROTO_IPV6, IPV6_V6ONLY, (char *)&Value, Len);
	}
	

	if(m_Socket.GetState()==CNetSocket::SS_UNINITED)
	{
		if(!Create())
			return FALSE;
	}

	m_pEpollEventRouter->Init(this);

	if(!m_Socket.EnableBlocking(FALSE))
	{
		PrintNetLog(_T("NetLib"),"(%d)Service开始非阻塞模式失败！",GetID());
		return FALSE;
	}

	if(m_CurProtocol==IPPROTO_UDP)
	{
		if(!m_Socket.Bind(Address))
		{
			PrintNetLog(_T("NetLib"),"(%d)Service绑定失败！",GetID());
			return FALSE;
		}
		m_Socket.SetState(CNetSocket::SS_CONNECTED);
	}
	else
	{
		if(!m_Socket.Listen(Address))
		{
			PrintNetLog(_T("NetLib"),"(%d)Service侦听失败！",GetID());
			return FALSE;
		}
	}


	if(!GetServer()->BindSocket(m_Socket.GetSocket(),m_pEpollEventRouter))
	{
		PrintNetLog(_T("NetLib"),"(%d)Service绑定Socket失败！",GetID());
		return FALSE;
	}

	OnStartUp();
	return TRUE;
}
void CNetService::Close()
{
	if (m_Socket.GetState() != CNetSocket::SS_UNINITED)
		OnClose();

	m_Socket.Close();

	m_WantClose = FALSE;

	if (m_pEpollEventRouter)
	{
		m_pEpollEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pEpollEventRouter);
		m_pEpollEventRouter = NULL;
	}

	CEpollEventObject * pEpollEventObject;
	while(m_RecvQueue.PopFront(pEpollEventObject))
	{
		GetServer()->DeleteEventObject(pEpollEventObject);
	}
	while(m_SendQueue.PopFront(pEpollEventObject))
	{
		GetServer()->DeleteEventObject(pEpollEventObject);
	}

	
}
void CNetService::QueryClose()
{
	m_WantClose=TRUE;
}
void CNetService::OnStartUp()
{
}
void CNetService::OnClose()
{
}

int CNetService::Update(int ProcessPacketLimit)
{


	//处理Accept
	int PacketCount=0;
	CEpollEventObject * pEpollEventObject;
	while(m_RecvQueue.PopFront(pEpollEventObject))
	{
		if(pEpollEventObject->GetType()==IO_ACCEPT)
		{
			if(!AcceptSocket(pEpollEventObject->GetAcceptSocket()))
			{
				PrintNetLog(_T("NetLib"),"(%d)AcceptSocket失败！",GetID());
			}
		}
		else if(pEpollEventObject->GetType()==IO_RECV)
		{
			OnRecvData(pEpollEventObject->GetAddress(),
				(BYTE *)pEpollEventObject->GetDataBuff()->GetBuffer(), pEpollEventObject->GetDataBuff()->GetUsedSize());
		}
		else
		{
			PrintNetLog(_T("NetLib"),"(%d)Servicec收到不明类型的OverLapped！",GetID());
		}
		GetServer()->DeleteEventObject(pEpollEventObject);
		PacketCount++;
		if(PacketCount>=ProcessPacketLimit)
			break;
	}

	//处理关闭
	if(m_WantClose)
	{
		Close();
	}
	return PacketCount;
}

CBaseNetConnection * CNetService::CreateConnection(CIPAddress& RemoteAddress)
{
	return NULL;
}

BOOL CNetService::DeleteConnection(CBaseNetConnection * pConnection)
{
	return FALSE;
}

BOOL CNetService::QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size)
{

	CEpollEventObject * pEpollEventObject=GetServer()->CreateEventObject();
	if(pEpollEventObject==NULL)
	{
		PrintNetLog(_T("NetLib"),"(%d)Service创建UDPSend用OverLappedObject失败！",GetID());
		return FALSE;
	}

	pEpollEventObject->SetAddress(IPAddress);

	pEpollEventObject->SetType(IO_SEND);
	pEpollEventObject->GetDataBuff()->SetUsedSize(0);
	pEpollEventObject->SetParentID(GetID());

	if(!pEpollEventObject->GetDataBuff()->PushBack(pData,Size))
	{
		GetServer()->DeleteEventObject(pEpollEventObject);
		PrintNetLog(_T("NetLib"),"(%d)Service要发送的数据包过大！",GetID());
		return FALSE;
	}

	if(m_SendQueue.PushBack(pEpollEventObject))
	{
		DoUDPSend();
		return TRUE;
	}
	PrintNetLog(_T("NetLib"),"(%d)Service的发送队列已满！",GetID());
	GetServer()->DeleteEventObject(pEpollEventObject);
	return FALSE;
}

void CNetService::OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize)
{
}




BOOL CNetService::AcceptSocket(SOCKET Socket)
{

	CIPAddress LocalAddress;
	CIPAddress RemoteAddress;

	socklen_t LocalAddressLen = sizeof(CIPAddress);
	socklen_t RemoteAddressLen = sizeof(CIPAddress);

	getsockname(Socket,
		LocalAddress.GetSockAddrPtr(),
		&LocalAddressLen);

	getpeername(Socket,
		RemoteAddress.GetSockAddrPtr(),
		&RemoteAddressLen);

	CBaseNetConnection * pConnection=dynamic_cast<CBaseNetConnection *>(CreateConnection(RemoteAddress));
	if(pConnection)
	{
		if(pConnection->Create(Socket,m_RecvQueueSize,m_SendQueueSize))
		{
			pConnection->SetRemoteAddress(RemoteAddress);
			pConnection->SetLocalAddress(LocalAddress);

			if(pConnection->StartWork())
			{
				return TRUE;
			}
			else
				PrintNetLog(_T("NetLib"),"(%d)Service启动Connection失败！",GetID());
		}
		else
		{
			PrintNetLog(_T("NetLib"),"(%d)Service初始化Connection失败！",GetID());
			closesocket(Socket);
		}
		DeleteConnection(pConnection);
	}
	else
	{
		PrintNetLog(_T("NetLib"),"(%d)Servicec创建Connection失败！",GetID());
		closesocket(Socket);
	}
	return FALSE;
}

void CNetService::DoAcceptSocket()
{
	CIPAddress Address;

	while(true)
	{
		socklen_t AddressLen = sizeof(CIPAddress);
		SOCKET AcceptSocket = accept(m_Socket.GetSocket(), Address.GetSockAddrPtr(), &AddressLen);
		if(AcceptSocket==INVALID_SOCKET)
		{
			int ErrorCode=errno;
			if(ErrorCode==EAGAIN)
			{
				return;
			}
			else
			{
				PrintNetLog(_T("NetLib"),"Accept失败(%u)",ErrorCode);
				QueryClose();
				return;
			}
		}
		else
		{
			CEpollEventObject * pEpollEventObject=GetServer()->CreateEventObject();
			if(pEpollEventObject)
			{
				pEpollEventObject->SetType(IO_ACCEPT);
				pEpollEventObject->SetAcceptSocket(AcceptSocket);
				pEpollEventObject->SetParentID(GetID());

				if(m_RecvQueue.PushBack(pEpollEventObject))
				{
					continue;
				}
				else
				{
					GetServer()->DeleteEventObject(pEpollEventObject);
					PrintNetLog(_T("NetLib"),"CNetService的Accept队列已满！");
				}

			}
			else
			{
				PrintNetLog(_T("NetLib"),"CNetService创建Accept用EpollEventObject失败！");
			}

			closesocket(AcceptSocket);
		}
	}
}

void CNetService::DoUDPRecv()
{
	while(true)
	{
		CEpollEventObject * pEpollEventObject=GetServer()->CreateEventObject();
		int RecvSize=0;
		if(pEpollEventObject)
		{
			pEpollEventObject->SetType(IO_RECV);
			pEpollEventObject->SetParentID(GetID());

			 RecvSize=recvfrom(
				m_Socket.GetSocket(),
				pEpollEventObject->GetDataBuff()->GetBuffer(),
				pEpollEventObject->GetDataBuff()->GetBufferSize(),
				0,
				pEpollEventObject->GetAddress().GetSockAddrPtr(),
				&(pEpollEventObject->GetAddressLen()));
		}
		else
		{
			PrintNetLog(_T("NetLib"),"CNetService创建Recv用EpollEventObject失败,数据将被丢弃！");

			static char RecvBuffer[NET_DATA_BLOCK_SIZE];
			static CIPAddress FromAddr;
			static socklen_t FromAddrLen;

			FromAddrLen=sizeof(FromAddr);
			RecvSize=recvfrom(
				m_Socket.GetSocket(),
				RecvBuffer,
				NET_DATA_BLOCK_SIZE,
				0,
				FromAddr.GetSockAddrPtr(),
				&FromAddrLen);
		}

		if(RecvSize>=0)
		{
			GetServer()->AddUDPRecvBytes(RecvSize);
			if(pEpollEventObject)
			{
				pEpollEventObject->GetDataBuff()->SetUsedSize(RecvSize);
				if(m_RecvQueue.PushBack(pEpollEventObject))
				{
					continue;
				}
				else
				{
					GetServer()->DeleteEventObject(pEpollEventObject);
					PrintNetLog(_T("NetLib"),"CNetService的Recv队列已满！");
				}
			}

		}
		else
		{
			if(pEpollEventObject)
				GetServer()->DeleteEventObject(pEpollEventObject);

			int ErrorCode=errno;
			switch(ErrorCode)
			{
			case EAGAIN:
				return;
			case EINTR:
				PrintNetLog(_T("NetLib"),"Recv被系统中断");
				break;
			default:
				PrintNetLog(_T("NetLib"),"Recv失败(%u),Socket关闭",ErrorCode);
				QueryClose();
				return;
			}
		}


	}
}

void CNetService::DoUDPSend()
{
	CEpollEventObject * pEpollEventObject=NULL;
	while(m_SendQueue.PopFront(pEpollEventObject))
	{
		int SendSize=sendto(
			m_Socket.GetSocket(),
			pEpollEventObject->GetDataBuff()->GetBuffer(),
			pEpollEventObject->GetDataBuff()->GetUsedSize(),
			0,
			pEpollEventObject->GetAddress().GetSockAddrPtr(),
			sizeof(CIPAddress));



		if(SendSize>=0)
		{
			GetServer()->AddUDPSendBytes(SendSize);
			if(SendSize<(int)pEpollEventObject->GetDataBuff()->GetUsedSize())
			{
				PrintNetLog(_T("NetLib"),"Send没有完成，数据被部分丢弃");
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
				PrintNetLog(_T("NetLib"),"Send失败(%u)",ErrorCode);
			}

		}
		GetServer()->DeleteEventObject(pEpollEventObject);
	}
}
