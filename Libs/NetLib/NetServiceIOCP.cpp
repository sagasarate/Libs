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

IMPLEMENT_CLASS_INFO_STATIC(CNetService,CBaseService);

CNetService::CNetService()
{
	m_WantClose=FALSE;
	m_pServer=NULL;
	m_CurProtocol=IPPROTO_TCP;
	m_AcceptQueueSize=0;
	m_RecvQueueSize=0;
	m_SendQueueSize=0;
	m_ParallelAcceptCount=0;
	m_ParallelRecvCount=0;
	m_IsUseListenThread=false;
	m_pIOCPEventRouter=NULL;	
	m_pListenThread=NULL;
	
}

CNetService::~CNetService(void)
{
	Destory();
}

BOOL CNetService::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{		
	if(IsWorking())
	{
		if(pOverLappedObject->GetType()==IO_ACCEPT)
		{			
			if(!QueryAccept())
			{
				PrintNetLog(0xffffffff,_T("无法发出更多的Accept请求！"));
				QueryClose();
			}

			if(EventID==IOE_PACKET)
			{	
				SOCKET ListenSocket=m_Socket.GetSocket();
				if(setsockopt( pOverLappedObject->GetAcceptSocket(),
					SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
					(char *)&ListenSocket, sizeof(ListenSocket))!=SOCKET_ERROR)
				{
					if(m_AcceptQueue.PushBack(pOverLappedObject))
					{							
						return TRUE;
					}
					else
					{
						PrintNetLog(0xffffffff,_T("Accept队列已满！"));
					}
				}				
				else
				{
					PrintNetLog(0xffffffff,_T("更新AcceptScoket状态失败！"));
				}
			}
			else
			{
				PrintNetLog(0xffffffff,_T("Accept返回错误！"));
			}

			closesocket(pOverLappedObject->GetAcceptSocket());
		}
		else if(pOverLappedObject->GetType()==IO_ACCEPT2)
		{
			if(EventID==IOE_PACKET)
			{
				if(m_AcceptQueue.PushBack(pOverLappedObject))
				{							
					return TRUE;
				}
				else
					PrintNetLog(0xffffffff,_T("Accept队列已满！"));
				closesocket(pOverLappedObject->GetAcceptSocket());
			}			
			else
			{
				PrintNetLog(0xffffffff,_T("侦听失败，关闭侦听！"));
				QueryClose();
			}
		}
		else if(pOverLappedObject->GetType()==IO_RECV)
		{			
			if(!QueryUDPRecv())
			{
				PrintNetLog(0xffffffff,_T("无法发出更多的UDPRecv请求！"));
				QueryClose();
			}

			if(EventID==IOE_PACKET)
			{
				GetServer()->AddUDPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());

				if(m_AcceptQueue.PushBack(pOverLappedObject))
				{						
					return TRUE;
				}
				else
					PrintNetLog(0xffffffff,_T("Accept队列已满！"));		
			}
			else
			{
				PrintNetLog(0xffffffff,_T("接收失败！"));
			}				
		}
		else if(pOverLappedObject->GetType()==IO_SEND)
		{
			if(EventID==IOE_PACKET)
			{
				GetServer()->AddUDPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
				return TRUE;
			}
			else
				PrintNetLog(0xffffffff,_T("发送失败！"));
		}			
		else
			PrintNetLog(0xffffffff,_T("Service收到非法IOCP包！"));
	}
	else
		PrintNetLog(0xffffffff,_T("Service未启用，IOCP包被忽略！"));
	GetServer()->DeleteOverLappedObject(pOverLappedObject);


	return FALSE;
}

BOOL CNetService::Create(int Protocol,int AcceptQueueSize,int RecvQueueSize,int SendQueueSize,int ParallelAcceptCount,int ParallelRecvCount,bool IsUseListenThread)
{	
	

	if(GetServer()==NULL)
		return FALSE;

	Destory();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		m_pIOCPEventRouter->Init(this);
	}
	m_AcceptQueueSize=AcceptQueueSize;
	m_RecvQueueSize=RecvQueueSize;
	m_SendQueueSize=SendQueueSize;
	m_ParallelAcceptCount=ParallelAcceptCount;
	m_ParallelRecvCount=ParallelRecvCount;
	m_IsUseListenThread=IsUseListenThread;
	if(Protocol==IPPROTO_UDP)
	{
		m_Socket.MakeSocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);	
		m_AcceptQueue.Create(m_RecvQueueSize);
	}
	else
	{
		m_Socket.MakeSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		m_AcceptQueue.Create(m_AcceptQueueSize);
	}
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
	m_CurProtocol=m_Socket.GetProtocol();
	return TRUE;
}
void CNetService::Destory()
{
	

	if(m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter=NULL;
	}


	m_Socket.Close();	

	SAFE_RELEASE(m_pListenThread);

	COverLappedObject * pOverLappedObject;
	while(m_AcceptQueue.PopFront(pOverLappedObject))
	{	
		GetServer()->DeleteOverLappedObject(pOverLappedObject);		
	}

	
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
}


BOOL CNetService::StartListen(const CIPAddress& Address)
{
	

	if(m_Socket.GetState()==CNetSocket::SS_UNINITED)
	{
		if(!Create())
			return FALSE;
	}
	
	m_pIOCPEventRouter->Init(this);


	if(!m_IsUseListenThread||m_CurProtocol==IPPROTO_UDP)
	{
	
		if(!GetServer()->BindSocket(m_Socket.GetSocket()))
		{
			PrintNetLog(0xffffffff,_T("(%d)Service绑定IOCP失败！"),GetID());
			Close();
			return FALSE;
		}	
	}
	
	
	if(m_CurProtocol==IPPROTO_UDP)
	{		
		if(!m_Socket.Bind(Address))
		{
			PrintNetLog(0xffffffff,_T("(%d)Service绑定失败！"),GetID());
			return FALSE;
		}

		m_Socket.SetState(CNetSocket::SS_CONNECTED);

		for(int i=0;i<m_ParallelRecvCount;i++)
		{
			if(!QueryUDPRecv())
			{
				PrintNetLog(0xffffffff,_T("(%d)Service发出UDPRecv请求失败！"),GetID());
				Close();
				return FALSE;
			}
		}
	}
	else
	{
		if(!m_Socket.Listen(Address))
		{
			PrintNetLog(0xffffffff,_T("(%d)Service侦听失败！"),GetID());
			return FALSE;
		}

		if(m_IsUseListenThread)
		{
			PrintNetLog(0xffffffff,_T("(%d)Service启用线程侦听模式！"),GetID());
			if(m_pListenThread==NULL)
				m_pListenThread=new CIOCPListenThread();
			m_pListenThread->Init(this,m_Socket.GetSocket());
			m_pListenThread->Start();
		}
		else
		{		
			PrintNetLog(0xffffffff,_T("(%d)Service启用IOCP侦听模式！"),GetID());
			for(int i=0;i<m_ParallelAcceptCount;i++)
			{
				if(!QueryAccept())
				{
					PrintNetLog(0xffffffff,_T("(%d)Service发出Accpet请求失败！"),GetID());
					Close();
					return FALSE;
				}
			}
		}
	}	
	OnStartUp();
	return TRUE;
}
void CNetService::Close()
{
	

	m_Socket.Close();
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);

	COverLappedObject * pOverLappedObject;
	while(m_AcceptQueue.PopFront(pOverLappedObject))
	{	
		GetServer()->DeleteOverLappedObject(pOverLappedObject);		
	}	

	OnClose();	
}
void CNetService::QueryClose()
{
	InterlockedExchange((LPLONG)&(m_WantClose),TRUE);
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
	COverLappedObject * pOverLappedObject;
	while(m_AcceptQueue.PopFront(pOverLappedObject))
	{
		if(pOverLappedObject->GetType()==IO_ACCEPT)
		{
			if(!AcceptSocketEx(pOverLappedObject->GetAcceptSocket(),pOverLappedObject->GetDataBuff()))
			{
				PrintNetLog(0xffffffff,_T("(%d)AcceptSocketEx失败！"),GetID());
			}
		}
		else if(pOverLappedObject->GetType()==IO_ACCEPT2)
		{					
			if(!AcceptSocket(pOverLappedObject->GetAcceptSocket()))
			{
				PrintNetLog(0xffffffff,_T("(%d)AcceptSocket失败！"),GetID());
			}
		}
		else if(pOverLappedObject->GetType()==IO_RECV)
		{				
			OnRecvData(pOverLappedObject->GetAddress(),
				(const BYTE *)pOverLappedObject->GetDataBuff()->GetBuffer(), pOverLappedObject->GetDataBuff()->GetUsedSize());
		}
		else
		{
			PrintNetLog(0xffffffff,_T("(%d)Servicec收到不明类型的OverLapped！"),GetID());			
		}
		GetServer()->DeleteOverLappedObject(pOverLappedObject);
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

CBaseTCPConnection * CNetService::CreateConnection(CIPAddress& RemoteAddress)
{
	return NULL;
}

BOOL CNetService::DeleteConnection(CBaseTCPConnection * pConnection)
{
	return FALSE;
}


BOOL CNetService::QueryAccept()
{		

	//PrintNetLog(0xffffffff,_T("(%d)发出AcceptEx请求！"),GetID());

	SOCKET	AcceptSocket=0;
	AcceptSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if(AcceptSocket==INVALID_SOCKET)
	{
		PrintNetLog(0xffffffff,_T("(%d)创建AcceptSocket失败(%u)！"),GetID(),GetLastError());
		return FALSE;
	}

	COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(0xffffffff,_T("(%d)Service创建OverLappedObject失败！"),GetID());
		return FALSE;
	}

	pOverLappedObject->SetType(IO_ACCEPT);
	pOverLappedObject->SetAcceptSocket(AcceptSocket);
	pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
	pOverLappedObject->SetParentID(GetID());
	
	static DWORD NumberOfBytes;	

	NumberOfBytes=0;
	if(m_Socket.AcceptOverlapped(pOverLappedObject->GetAcceptSocket(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		0,
		NumberOfBytes,pOverLappedObject->GetOverlapped()))
	{
		return TRUE;
	}
	PrintNetLog(0xffffffff,_T("(%d)发出Accept请求失败！"),GetID());
	closesocket(AcceptSocket);
	GetServer()->DeleteOverLappedObject(pOverLappedObject);

	return FALSE;
	
}
BOOL CNetService::QueryUDPRecv()
{	

	COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(0xffffffff,_T("(%d)Service创建热UDPRecv用OverLappedObject失败！"),GetID());
		return FALSE;
	}

	pOverLappedObject->SetType(IO_RECV);	
	pOverLappedObject->GetDataBuff()->SetUsedSize(0);
	pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);	
	pOverLappedObject->SetParentID(GetID());

	static DWORD NumberOfBytes;
	static DWORD Flag;	

	NumberOfBytes=0;
	Flag=0;

	if(m_Socket.RecvFromOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetAddressLen(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		pOverLappedObject->GetDataBuff()->GetBufferSize(),
		NumberOfBytes,Flag,
		pOverLappedObject->GetOverlapped()))
	{
		return TRUE;
	}
	PrintNetLog(0xffffffff,_T("(%d)Service发出UDPRecv请求失败！"),GetID());	
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	return FALSE;
}

BOOL CNetService::QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size)
{	
	
	COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(0xffffffff,_T("(%d)Service创建UDPSend用OverLappedObject失败！"),GetID());
		return FALSE;
	}

	pOverLappedObject->SetAddress(IPAddress.GetSockAddr());

	pOverLappedObject->SetType(IO_SEND);	
	pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
	pOverLappedObject->GetDataBuff()->SetUsedSize(0);
	pOverLappedObject->SetParentID(GetID());

	if(!pOverLappedObject->GetDataBuff()->PushBack(pData,Size))
	{
		GetServer()->DeleteOverLappedObject(pOverLappedObject);
		PrintNetLog(0xffffffff,_T("(%d)Service要发送的数据包过大！"),GetID());
		return FALSE;
	}
	

	static DWORD NumberOfBytes;
	static DWORD Flag;

	NumberOfBytes=0;
	Flag=0;

	if(m_Socket.SendToOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		pOverLappedObject->GetDataBuff()->GetUsedSize(),
		NumberOfBytes,Flag,
		pOverLappedObject->GetOverlapped()))
	{		
		return TRUE;
	}
	PrintNetLog(0xffffffff,_T("(%d)Service发出UDPSend请求失败！"),GetID());	
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	return FALSE;
}

void CNetService::OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize)
{
}

BOOL CNetService::AcceptSocket(SOCKET Socket)
{

	CIPAddress LocalAddress;
	CIPAddress RemoteAddress;			

	socklen_t LocalAddressLen=sizeof(sockaddr_in);
	socklen_t RemoteAddressLen=sizeof(sockaddr_in);

	getsockname(Socket,
		(sockaddr *)&(LocalAddress.GetSockAddr()),
		&LocalAddressLen);

	getpeername(Socket,
		(sockaddr *)&(RemoteAddress.GetSockAddr()),
		&RemoteAddressLen);

	CBaseTCPConnection * pConnection=dynamic_cast<CBaseTCPConnection *>(CreateConnection(RemoteAddress));
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
				PrintNetLog(0xffffffff,_T("(%d)Service启动Connection失败！"),GetID());
		}
		else
		{
			PrintNetLog(0xffffffff,_T("(%d)Service初始化Connection失败！"),GetID());
			closesocket(Socket);
		}
		DeleteConnection(pConnection);
	}
	else
	{
		PrintNetLog(0xffffffff,_T("(%d)Servicec创建Connection失败！"),GetID());	
		closesocket(Socket);
	}
	return FALSE;
}

BOOL CNetService::AcceptSocketEx(SOCKET Socket,CEasyBuffer * pAcceptData)
{

	sockaddr_in * pLocalAddress=NULL;
	sockaddr_in * pRemoteAddress=NULL;
	int LocalAddressLen,RemoteAddressLen;

	GetAcceptExSockaddrs(pAcceptData->GetBuffer(),pAcceptData->GetUsedSize(),
		sizeof( SOCKADDR_IN) + 16,
		sizeof( SOCKADDR_IN) + 16,
		(sockaddr**)&pLocalAddress,&LocalAddressLen,
		(sockaddr**)&pRemoteAddress,&RemoteAddressLen);


	CIPAddress LocalAddress;
	CIPAddress RemoteAddress;			

	if(pLocalAddress)
	{
		LocalAddress=*pLocalAddress;
	}

	if(pRemoteAddress)
	{
		RemoteAddress=*pRemoteAddress;
	}

	CBaseTCPConnection * pConnection=dynamic_cast<CBaseTCPConnection *>(CreateConnection(RemoteAddress));
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
				PrintNetLog(0xffffffff,_T("(%d)Service启动Connection失败！"),GetID());
		}
		else
		{
			PrintNetLog(0xffffffff,_T("(%d)Service初始化Connection失败！"),GetID());
			closesocket(Socket);
		}
		DeleteConnection(pConnection);
	}
	else
	{
		PrintNetLog(0xffffffff,_T("(%d)Servicec创建Connection失败！"),GetID());	
		closesocket(Socket);
	}
	return FALSE;
}