/****************************************************************************/
/*                                                                          */
/*      文件名:    NetServiceIOCP.cpp                                       */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CNetService, CBaseNetService);

CNetService::CNetService()
{
	m_WantClose = false;
	m_pServer = NULL;
	m_CurProtocol = IPPROTO_TCP;
	m_CurAddressFamily = AF_INET;
	m_IPv6Only = false;
	m_AcceptQueueSize = 0;
	m_RecvQueueSize = 0;
	m_SendQueueSize = 0;
	m_ParallelAcceptCount = 0;
	m_ParallelRecvCount = 0;
	m_IsUseListenThread = false;
	m_pIOCPEventRouter = NULL;
	m_pListenThread = NULL;
	m_AcceptQueue.SetLockMode(false, true);
	m_OverLappedObjectPool.SetTag(_T("CNetService"));
	m_AcceptQueue.SetTag(_T("CNetService"));
}

CNetService::~CNetService(void)
{
	Destory();
}

bool CNetService::OnIOCPEvent(int EventID, COverLappedObject* pOverLappedObject)
{
	if (IsWorking())
	{
		if (pOverLappedObject->GetType() == IO_ACCEPT)
		{
			if (!QueryAccept())
			{
				PrintNetLog(_T("无法发出更多的Accept请求！"));
				QueryClose();
			}

			if (EventID == IOE_PACKET)
			{
				SOCKET ListenSocket = m_Socket.GetSocket();
				if (setsockopt(pOverLappedObject->GetAcceptSocket(),
					SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
					(char*)&ListenSocket, sizeof(ListenSocket)) != SOCKET_ERROR)
				{
					if (m_AcceptQueue.PushBack(&pOverLappedObject))
					{
						return true;
					}
					else
					{
						PrintNetLog(_T("Accept队列已满！"));
					}
				}
				else
				{
					PrintNetLog(_T("更新AcceptScoket状态失败！"));
				}
			}
			else
			{
				PrintNetLog(_T("Accept返回错误！"));
			}

			closesocket(pOverLappedObject->GetAcceptSocket());
		}
		else if (pOverLappedObject->GetType() == IO_ACCEPT2)
		{
			if (EventID == IOE_PACKET)
			{
				if (m_AcceptQueue.PushBack(&pOverLappedObject))
				{
					return true;
				}
				else
					PrintNetLog(_T("Accept队列已满！"));
				closesocket(pOverLappedObject->GetAcceptSocket());
			}
			else
			{
				PrintNetLog(_T("侦听失败，关闭侦听！"));
				QueryClose();
			}
		}
		else if (pOverLappedObject->GetType() == IO_RECV)
		{
			if (!QueryUDPRecv())
			{
				PrintNetLog(_T("无法发出更多的UDPRecv请求！"));
				QueryClose();
			}

			if (EventID == IOE_PACKET)
			{
				GetServer()->AddUDPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());

				if (m_AcceptQueue.PushBack(&pOverLappedObject))
				{
					return true;
				}
				else
					PrintNetLog(_T("UDP数据接收队列已满！"));
			}
			else
			{
				PrintNetLog(_T("接收失败！"));
			}
		}
		else if (pOverLappedObject->GetType() == IO_SEND)
		{
			if (EventID == IOE_PACKET)
			{
				GetServer()->AddUDPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				ReleaseOverLappedObject(pOverLappedObject, 1);
				return true;
			}
			else
				PrintNetLog(_T("发送失败！"));
		}
		else
			PrintNetLog(_T("Service收到非法IOCP包(%u)！"), pOverLappedObject->GetType());
	}
	else
		PrintNetLog(_T("Service未启用，IOCP包被忽略！"));
	ReleaseOverLappedObject(pOverLappedObject, 2);


	return false;
}

bool CNetService::Create(int Protocol, UINT AcceptQueueSize, UINT RecvQueueSize, UINT SendQueueSize, UINT ParallelAcceptCount, UINT ParallelRecvCount, bool IsUseListenThread, bool IPv6Only)
{


	if (GetServer() == NULL)
		return false;

	Close();

	if (m_pIOCPEventRouter == NULL)
	{
		m_pIOCPEventRouter = GetServer()->CreateEventRouter();
		if (m_pIOCPEventRouter == NULL)
		{
			PrintNetLog(_T("无法分配到IOCPEventRouter"));
			return false;
		}
		m_pIOCPEventRouter->Init(this);
	}



	m_CurProtocol = Protocol;
	m_AcceptQueueSize = AcceptQueueSize;
	m_RecvQueueSize = RecvQueueSize;
	m_SendQueueSize = SendQueueSize;
	m_ParallelAcceptCount = ParallelAcceptCount;
	m_ParallelRecvCount = ParallelRecvCount;
	m_IsUseListenThread = IsUseListenThread;
	m_IPv6Only = IPv6Only;
	m_WantClose = false;
	return true;
}

void CNetService::Destory()
{
	Close();
	CAutoLock Lock(m_OverLappedObjectPoolLock);
	if (GetServer() && m_OverLappedObjectPool.GetBufferSize())
	{
		GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		m_OverLappedObjectPool.Destory();
	}
}

bool CNetService::StartListen(const CIPAddress& Address)
{
	if (m_pIOCPEventRouter == NULL)
		Create();

	if (Address.IsIPv4())
		m_CurAddressFamily = AF_INET;
	else if (Address.IsIPv6())
		m_CurAddressFamily = AF_INET6;

	if (m_CurProtocol == IPPROTO_UDP)
	{
		UINT PoolSize = (UINT)(m_RecvQueueSize + m_SendQueueSize);
		if (m_OverLappedObjectPool.GetBufferSize() < PoolSize)
		{
			if (m_OverLappedObjectPool.GetUsedSize())
			{
				GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
			}

			m_OverLappedObjectPool.Create(PoolSize);
			GetServer()->AllocOverLappedObject(m_OverLappedObjectPool, PoolSize, GetID());
			if (m_OverLappedObjectPool.GetUsedSize() <= 0)
			{
				PrintNetLog(_T("无法分配到OverLappedObject"));
				return false;
			}
		}

		if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_DGRAM, IPPROTO_UDP))
			return false;
		m_AcceptQueue.Create(m_RecvQueueSize);
	}
	else if (m_CurProtocol == IPPROTO_TCP)
	{
		if (m_AcceptQueueSize < m_ParallelAcceptCount)
		{
			m_AcceptQueueSize = m_ParallelAcceptCount * 2;
			PrintNetLog(_T("Accept队列大小小于并发Accept数，已修改为并发Accept数的2倍"));
		}
		if (m_OverLappedObjectPool.GetBufferSize() < m_AcceptQueueSize)
		{
			if (m_OverLappedObjectPool.GetUsedSize())
			{
				GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
			}

			m_OverLappedObjectPool.Create(m_AcceptQueueSize);
			GetServer()->AllocOverLappedObject(m_OverLappedObjectPool, m_AcceptQueueSize, GetID());
			if (m_OverLappedObjectPool.GetUsedSize() <= 0)
			{
				PrintNetLog(_T("无法分配到OverLappedObject"));
				return false;
			}
		}

		if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP))
			return false;
		m_AcceptQueue.Create(m_AcceptQueueSize);
	}
	else
	{
		PrintNetLog(_T("(%d)Service协议错误%d！"), GetID(), m_CurProtocol);
		return false;
	}

	if (!m_IPv6Only)
	{
		//启用同时侦听IPv6和IPv4
		DWORD Value = 0;
		socklen_t Len = sizeof(Value);
		m_Socket.SetOption(IPPROTO_IPV6, IPV6_V6ONLY, (char*)&Value, Len);
	}



	m_pIOCPEventRouter->Init(this);


	if (!m_IsUseListenThread || m_CurProtocol == IPPROTO_UDP)
	{

		if (!GetServer()->BindSocket(m_Socket.GetSocket()))
		{
			PrintNetLog(_T("(%d)Service绑定IOCP失败！"), GetID());
			Close();
			return false;
		}
	}


	if (m_CurProtocol == IPPROTO_UDP)
	{
		if (!m_Socket.Bind(Address))
		{
			PrintNetLog(_T("(%d)Service绑定失败！"), GetID());
			return false;
		}

		m_Socket.SetState(CNetSocket::SS_CONNECTED);

		for (UINT i = 0; i < m_ParallelRecvCount; i++)
		{
			if (!QueryUDPRecv())
			{
				PrintNetLog(_T("(%d)Service发出UDPRecv请求失败！"), GetID());
				Close();
				return false;
			}
		}
	}
	else
	{
		if (!m_Socket.Listen(Address))
		{
			PrintNetLog(_T("(%d)Service侦听失败！"), GetID());
			return false;
		}

		if (m_IsUseListenThread)
		{
			PrintNetLog(_T("(%d)Service启用线程侦听模式！"), GetID());
			if (m_pListenThread == NULL)
				m_pListenThread = MONITORED_NEW(_T("CNetService"), CIOCPListenThread);
			m_pListenThread->Init(this, m_Socket.GetSocket());
			m_pListenThread->Start();
		}
		else
		{
			PrintNetLog(_T("(%d)Service启用IOCP侦听模式！"), GetID());
			for (UINT i = 0; i < m_ParallelAcceptCount; i++)
			{
				if (!QueryAccept())
				{
					PrintNetLog(_T("(%d)Service发出Accpet请求失败！"), GetID());
					Close();
					return false;
				}
			}
		}
	}
	OnStartUp();
	return true;
}



void CNetService::Close()
{
	if (m_Socket.GetState() != CNetSocket::SS_UNINITED)
		OnClose();

	m_Socket.Close();

	SAFE_RELEASE(m_pListenThread);

	m_WantClose = false;

	if (m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter = NULL;
	}

	COverLappedObject* pOverLappedObject;
	while (m_AcceptQueue.PopFront(&pOverLappedObject))
	{
		ReleaseOverLappedObject(pOverLappedObject, 3);
	}


}
void CNetService::QueryClose()
{
	m_WantClose = true;
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
	int PacketCount = 0;
	COverLappedObject* pOverLappedObject;
	while (m_AcceptQueue.PopFront(&pOverLappedObject))
	{
		if (pOverLappedObject->GetType() == IO_ACCEPT)
		{
			if (!AcceptSocketEx(pOverLappedObject->GetAcceptSocket(), pOverLappedObject->GetDataBuff()))
			{
				PrintNetLog(_T("(%d)AcceptSocketEx失败！"), GetID());
			}
		}
		else if (pOverLappedObject->GetType() == IO_ACCEPT2)
		{
			if (!AcceptSocket(pOverLappedObject->GetAcceptSocket()))
			{
				PrintNetLog(_T("(%d)AcceptSocket失败！"), GetID());
			}
		}
		else if (pOverLappedObject->GetType() == IO_RECV)
		{
			OnRecvData(pOverLappedObject->GetAddress(),
				(const BYTE*)pOverLappedObject->GetDataBuff()->GetBuffer(), (UINT)pOverLappedObject->GetDataBuff()->GetUsedSize());
		}
		else
		{
			PrintNetLog(_T("(%d)Servicec收到不明类型(%d)的OverLapped！"), GetID(), pOverLappedObject->GetType());
		}
		ReleaseOverLappedObject(pOverLappedObject, 4);
		PacketCount++;
		if (PacketCount >= ProcessPacketLimit)
			break;
	}

	//处理关闭
	if (m_WantClose)
	{
		Close();
	}
	return PacketCount;
}

CBaseNetConnection* CNetService::CreateConnection(CIPAddress& RemoteAddress)
{
	return NULL;
}

bool CNetService::DeleteConnection(CBaseNetConnection* pConnection)
{
	return false;
}


bool CNetService::QueryAccept()
{

	//PrintNetLog(_T("(%d)发出AcceptEx请求！"),GetID());

	SOCKET	AcceptSocket = 0;
	AcceptSocket = socket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP);
	if (AcceptSocket == INVALID_SOCKET)
	{
		PrintNetLog(_T("(%d)创建AcceptSocket失败(%u)！"), GetID(), GetLastError());
		return false;
	}

	COverLappedObject* pOverLappedObject = AllocOverLappedObject(IO_ACCEPT);
	if (pOverLappedObject == NULL)
	{
		PrintNetLog(_T("(%d)Service创建OverLappedObject失败！"), GetID());
		return false;
	}

	//pOverLappedObject->SetType(IO_ACCEPT);
	pOverLappedObject->SetAcceptSocket(AcceptSocket);
	//pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
	//pOverLappedObject->SetParentID(GetID());

	static DWORD NumberOfBytes;

	if (m_Socket.AcceptOverlapped(pOverLappedObject->GetAcceptSocket(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		0,
		NumberOfBytes, pOverLappedObject->GetOverlapped()))
	{
		return true;
	}
	PrintNetLog(_T("发出Accept请求失败！"));
	closesocket(AcceptSocket);
	ReleaseOverLappedObject(pOverLappedObject, 5);

	return false;

}
bool CNetService::QueryUDPRecv()
{

	COverLappedObject* pOverLappedObject = AllocOverLappedObject(IO_RECV);
	if (pOverLappedObject == NULL)
	{
		PrintNetLog(_T("(%d)Service创建热UDPRecv用OverLappedObject失败！"), GetID());
		return false;
	}

	//pOverLappedObject->SetType(IO_RECV);	
	//pOverLappedObject->GetDataBuff()->SetUsedSize(0);
	//pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);	
	//pOverLappedObject->SetParentID(GetID());

	static DWORD NumberOfBytes;
	static DWORD Flag;

	NumberOfBytes = 0;
	Flag = 0;

	if (m_Socket.RecvFromOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetAddressLen(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		(int)pOverLappedObject->GetDataBuff()->GetBufferSize(),
		NumberOfBytes, Flag,
		pOverLappedObject->GetOverlapped()))
	{
		return true;
	}
	PrintNetLog(_T("Service发出UDPRecv请求失败！"));
	ReleaseOverLappedObject(pOverLappedObject, 6);
	return false;
}

bool CNetService::QueryUDPSend(const CIPAddress& IPAddress, LPCVOID pData, int Size)
{

	COverLappedObject* pOverLappedObject = AllocOverLappedObject(IO_SEND);
	if (pOverLappedObject == NULL)
	{
		PrintNetLog(_T("创建UDPSend用OverLappedObject失败！"));
		return false;
	}

	pOverLappedObject->SetAddress(IPAddress);

	//pOverLappedObject->SetType(IO_SEND);	
	//pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
	//pOverLappedObject->GetDataBuff()->SetUsedSize(0);
	//pOverLappedObject->SetParentID(GetID());

	if (!pOverLappedObject->GetDataBuff()->PushBack(pData, Size))
	{
		ReleaseOverLappedObject(pOverLappedObject, 7);
		PrintNetLog(_T("要发送的数据包过大！"));
		return false;
	}


	static DWORD NumberOfBytes;
	static DWORD Flag;

	NumberOfBytes = 0;
	Flag = 0;

	if (m_Socket.SendToOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		(int)pOverLappedObject->GetDataBuff()->GetUsedSize(),
		NumberOfBytes, Flag,
		pOverLappedObject->GetOverlapped()))
	{
		return true;
	}
	PrintNetLog(_T("Service发出UDPSend请求失败！"));
	ReleaseOverLappedObject(pOverLappedObject, 8);
	return false;
}

void CNetService::OnRecvData(const CIPAddress& IPAddress, const BYTE* pData, UINT DataSize)
{
}

bool CNetService::AcceptSocket(SOCKET Socket)
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

	CBaseNetConnection* pConnection = dynamic_cast<CBaseNetConnection*>(CreateConnection(RemoteAddress));
	if (pConnection)
	{
		if (pConnection->Create(Socket, m_RecvQueueSize, m_SendQueueSize))
		{
			pConnection->SetRemoteAddress(RemoteAddress);
			pConnection->SetLocalAddress(LocalAddress);

			if (pConnection->StartWork())
			{
				return true;
			}
			else
				PrintNetLog(_T("启动Connection失败！"));
		}
		else
		{
			PrintNetLog(_T("初始化Connection失败！"));
			closesocket(Socket);
		}
		DeleteConnection(pConnection);
	}
	else
	{
		PrintNetLog(_T("创建Connection失败！"));
		closesocket(Socket);
	}
	return false;
}

bool CNetService::AcceptSocketEx(SOCKET Socket, CEasyBuffer* pAcceptData)
{

	sockaddr* pLocalAddress = NULL;
	sockaddr* pRemoteAddress = NULL;
	int LocalAddressLen, RemoteAddressLen;

	GetAcceptExSockaddrs(pAcceptData->GetBuffer(), (DWORD)pAcceptData->GetUsedSize(),
		sizeof(CIPAddress) + 16,
		sizeof(CIPAddress) + 16,
		(sockaddr**)&pLocalAddress, &LocalAddressLen,
		(sockaddr**)&pRemoteAddress, &RemoteAddressLen);


	CIPAddress LocalAddress;
	CIPAddress RemoteAddress;

	if (pLocalAddress)
	{
		LocalAddress.SetAddress(pLocalAddress, LocalAddressLen);
	}

	if (pRemoteAddress)
	{
		RemoteAddress.SetAddress(pRemoteAddress, RemoteAddressLen);
	}

	CBaseNetConnection* pConnection = dynamic_cast<CBaseNetConnection*>(CreateConnection(RemoteAddress));
	if (pConnection)
	{
		if (pConnection->Create(Socket, m_RecvQueueSize, m_SendQueueSize))
		{
			pConnection->SetRemoteAddress(RemoteAddress);
			pConnection->SetLocalAddress(LocalAddress);

			if (pConnection->StartWork())
			{
				return true;
			}
			else
				PrintNetLog(_T("启动Connection失败！"));
		}
		else
		{
			PrintNetLog(_T("初始化Connection失败！"));
			closesocket(Socket);
		}
		DeleteConnection(pConnection);
	}
	else
	{
		PrintNetLog(_T("创建Connection失败！"));
		closesocket(Socket);
	}
	return false;
}

COverLappedObject* CNetService::AllocOverLappedObject(int Type)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	COverLappedObject* pObject = NULL;
	if (m_OverLappedObjectPool.PopFront(&pObject))
	{
		if (pObject->GetStatus() != OVERLAPPED_OBJECT_STATUS_IDLE)
		{
			PrintNetLog(_T("OverLappedObject(%u,%u)状态%u错误"), pObject->GetID(), pObject->GetType(), pObject->GetStatus());
		}

		pObject->SetType(Type);
		pObject->SetStatus(OVERLAPPED_OBJECT_STATUS_USING);
		pObject->GetDataBuff()->SetUsedSize(0);
		pObject->SetIOCPEventRouter(m_pIOCPEventRouter);
		//PrintNetLog(_T("OverLappedObject(%u,%u)已分配"), pObject->GetID(), pObject->GetType());
		return pObject;
	}
	else
	{
		PrintNetLog(_T("已无OverLappedObject可分配(%u/%u)"),
			m_OverLappedObjectPool.GetUsedSize(), m_OverLappedObjectPool.GetBufferSize());
	}
	return NULL;
}
bool CNetService::ReleaseOverLappedObject(COverLappedObject* pObject, int Where)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	if (pObject->GetStatus() != OVERLAPPED_OBJECT_STATUS_USING)
	{
		PrintNetLog(_T("(%d)OverLappedObject(%u,%u)状态%u错误,缓冲池状态(%u/%u)"),
			Where, pObject->GetID(), pObject->GetType(), pObject->GetStatus(),
			m_OverLappedObjectPool.GetUsedSize(), m_OverLappedObjectPool.GetBufferSize());
		return false;
	}

	if (m_OverLappedObjectPool.PushBack(&pObject))
	{
		pObject->SetStatus(OVERLAPPED_OBJECT_STATUS_IDLE);
		//PrintNetLog(_T("(%d)OverLappedObject(%u,%u)已释放"), Where, pObject->GetID(), pObject->GetType());
		return true;
	}
	else
	{
		PrintNetLog(_T("(%d)OverLappedObject缓冲已满(%u/%u)，直接释放"),
			m_OverLappedObjectPool.GetUsedSize(), Where, m_OverLappedObjectPool.GetBufferSize());
		pObject->Release();
		return false;
	}
}