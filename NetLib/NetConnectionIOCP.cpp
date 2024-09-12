/****************************************************************************/
/*                                                                          */
/*      文件名:    NetConnectionIOCP.cpp                                    */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

#undef  PrintNetLog
#define PrintNetLog(_Format, ...)	PrintConnectionLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)
#undef  PrintNetDebugLog
#define PrintNetDebugLog(_Format, ...)	PrintConnectionDebugLogWithTag(_T(__PRETTY_FUNCTION__), _Format, ##__VA_ARGS__)


IMPLEMENT_CLASS_INFO_STATIC(CNetConnection,CBaseNetConnection);

CNetConnection::CNetConnection(void)
{		
	m_pServer=NULL;
	m_WantClose=false;
	m_CurAddressFamily = AF_INET;
	m_pIOCPEventRouter=NULL;
	m_OverLappedObjectPool.SetTag(_T("CNetConnection"));
	m_RecvDataQueue.SetTag(_T("CNetConnection"));
}

CNetConnection::~CNetConnection(void)
{
	Destory();
}

bool CNetConnection::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{	
	

	if(m_Socket.IsConnected())
	{
		
		
		if(EventID==IOE_PACKET)
		{
			if(pOverLappedObject->GetType()==IO_SEND)
			{
				GetServer()->AddTCPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());				
				ReleaseOverLappedObject(pOverLappedObject);	
				return true;
			}
			else if(pOverLappedObject->GetType()==IO_RECV)
			{
				if(pOverLappedObject->GetDataBuff()->GetUsedSize()<=0)
				{						
					ReleaseOverLappedObject(pOverLappedObject);
					//PrintNetLog(_T("%s(%d)Connection收对方连接关闭信号(%d)！"),
					//	GetName(),m_Session,
					//	pOverLappedObject->GetDataBuff()->GetUsedSize());
					QueryDisconnect();
					return true;
				}
				else
				{			
					GetServer()->AddTCPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());

					bool Ret = false;
					{
						//这里加个锁，以免和Destory调用并发而导致OverLappedObject未正确释放
						CAutoLock Lock(m_OverLappedObjectPoolLock);
						//PrintNetLog(_T("Recv=%u,(%u/%u)"), pOverLappedObject->GetDataBuff()->GetUsedSize(), m_RecvDataQueue.GetUsedSize(), m_RecvDataQueue.GetBufferSize());
						Ret = m_RecvDataQueue.PushBack(&pOverLappedObject);
					}				

					if (Ret)
					{
						if(!QueryRecv())
						{
							PrintNetLog(_T("无法发出更多的Recv请求,连接关闭！"));
							QueryDisconnect();	
							return false;
						}
						else
						{
							return true;
						}
						
					}
					else
						PrintNetLog(_T("Recv数据队列已满(%u/%u)！"), m_RecvDataQueue.GetUsedSize(), m_RecvDataQueue.GetBufferSize());
				}
			}
			else
				PrintNetLog(_T("Connection收到非法IOCP包！"));
			
		}
		else
			PrintNetLog(_T("Connection收到IOCP错误！"));
		if(IsConnected())
		{
			//PrintNetLog(_T("(%d)(%d)Connection IOCP出错时连接还未断开,连接关闭！"),GetID(),m_Session);
			QueryDisconnect();
		}
		
	}
	else
	{
		PrintNetLog(_T("Connection未连接，IOCP包(%u)被忽略！"), pOverLappedObject->GetParentID());
	}
	ReleaseOverLappedObject(pOverLappedObject);
	

	return false;
}

bool CNetConnection::Create(UINT RecvQueueSize, UINT SendQueueSize)
{
	if (GetServer() == NULL)
	{
		PrintNetLog( _T("未设置Server！"));
		return false;
	}

	Close();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		if (m_pIOCPEventRouter == NULL)
		{
			PrintNetLog(_T("无法分配到IOCPEventRouter"));
			return false;
		}
		m_pIOCPEventRouter->Init(this);
	}

	if (m_OverLappedObjectPool.GetBufferSize() < RecvQueueSize + SendQueueSize)
	{
		if (m_OverLappedObjectPool.GetUsedSize())
		{
			GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		}
		m_OverLappedObjectPool.Create(RecvQueueSize + SendQueueSize);
		GetServer()->AllocOverLappedObject(m_OverLappedObjectPool, RecvQueueSize + SendQueueSize, GetID());
		if (m_OverLappedObjectPool.GetUsedSize() <= 0)
		{
			PrintNetLog(_T("无法分配到OverLappedObject"));
			return false;
		}
	}
	

	if(m_RecvDataQueue.GetUsedSize())
	{
		PrintNetLog(_T("CNetConnection::Create:数据队列不是空的！"));
	}

	
	if (m_RecvDataQueue.GetBufferSize()<RecvQueueSize)
	{
		m_RecvDataQueue.Create(RecvQueueSize);
	}
		
	m_WantClose = false;
	
	return true;
}

bool CNetConnection::Create(SOCKET Socket, UINT RecvQueueSize, UINT SendQueueSize)
{
	

	if (GetServer() == NULL)
	{
		PrintNetLog( _T("CNetConnection::Create:未设置Server！"));
		return false;
	}
		
		
	Close();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		if (m_pIOCPEventRouter == NULL)
		{
			PrintNetLog(_T("无法分配到IOCPEventRouter"));
			return false;
		}
		m_pIOCPEventRouter->Init(this);
	}

	if (m_OverLappedObjectPool.GetBufferSize() < RecvQueueSize + SendQueueSize)
	{
		if (m_OverLappedObjectPool.GetUsedSize())
		{
			GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		}
		m_OverLappedObjectPool.Create(RecvQueueSize + SendQueueSize);
		GetServer()->AllocOverLappedObject(m_OverLappedObjectPool, RecvQueueSize + SendQueueSize, GetID());
		if (m_OverLappedObjectPool.GetUsedSize() <= 0)
		{
			PrintNetLog(_T("无法分配到OverLappedObject"));
			return false;
		}
	}
	
	m_Socket.SetSocket(Socket);

	if(m_RecvDataQueue.GetUsedSize())
	{
		PrintNetLog(_T("CNetConnection::Create:数据队列不是空的！"));
	}

	if(m_RecvDataQueue.GetBufferSize()<RecvQueueSize)
	{
		m_RecvDataQueue.Create(RecvQueueSize);
	}
	
	m_WantClose = false;
	
	return true;
}

void CNetConnection::Destory()
{	
	if (m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter = NULL;
	}

	m_Socket.Close();

	m_WantClose = false;
	
	if (GetServer())
	{
		CAutoLock Lock(m_OverLappedObjectPoolLock);

		//GetServer()->ReleaseOverLappedObject(m_RecvDataQueue);
		m_RecvDataQueue.Destory();
		GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		m_OverLappedObjectPool.Destory();
	}
}

void CNetConnection::Close()
{
	Disconnect();
	if (m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter = NULL;
	}
}

bool CNetConnection::Connect(const CIPAddress& Address, DWORD TimeOut)
{

	if(GetServer()==NULL)
		return false;

	if (m_pIOCPEventRouter == NULL)
	{
		if (!Create())
			return false;
	}

	if (Address.IsIPv4())
		m_CurAddressFamily = AF_INET;
	else if (Address.IsIPv6())
		m_CurAddressFamily = AF_INET6;

	if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP))
		return false;

	m_pIOCPEventRouter->Init(this);


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
	if (IsConnected())
		OnDisconnection();
	
	m_Socket.Close();
	
	m_WantClose = false;
	
	

	COverLappedObject * pOverLappedObject;
	while(m_RecvDataQueue.PopFront(&pOverLappedObject))
	{			
		ReleaseOverLappedObject(pOverLappedObject);		
	}	
}
void CNetConnection::QueryDisconnect()
{
	m_WantClose = true;	
}



bool CNetConnection::StartWork()
{	
	m_Socket.SetState(CNetSocket::SS_CONNECTED);	
	m_pIOCPEventRouter->Init(this);
	

	if(!m_Socket.EnableBlocking(false))
	{
		OnConnection(false);
		PrintNetLog(_T("Connection开始非阻塞模式失败！"));
		m_Socket.Close();		
		return false;
	}

	if(!GetServer()->BindSocket(m_Socket.GetSocket()))
	{
		OnConnection(false);
		PrintNetLog(_T("Connection绑定IOCP失败！"));
		m_Socket.Close();		
		return false;
	}
	
	if(!QueryRecv())
	{
		OnConnection(false);
		PrintNetLog(_T("Connection初始化Recv失败！"));
		m_Socket.Close();
		return false;
	}

	OnConnection(true);
	return true;
}



bool CNetConnection::SendMulti(LPCVOID * pDataBuffers, const UINT * pDataSizes, UINT BufferCount)
{
	if (m_Socket.IsConnected())
	{
		for (UINT i = 0; i < BufferCount; i++)
		{
			LPCVOID pData = pDataBuffers[i];
			UINT Size = pDataSizes[i];
			while (Size)
			{
				int PacketSize = Size;
				if (PacketSize > NET_DATA_BLOCK_SIZE)
					PacketSize = NET_DATA_BLOCK_SIZE;
				Size -= PacketSize;


				COverLappedObject * pOverLappedObject = AllocOverLappedObject();
				if (pOverLappedObject == NULL)
				{
					PrintNetLog(_T("创建Send用OverLappedObject失败！"));
					Disconnect();
					return false;
				}

				pOverLappedObject->SetType(IO_SEND);
				pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
				pOverLappedObject->GetDataBuff()->SetUsedSize(0);
				pOverLappedObject->SetParentID(GetID());

				if (!pOverLappedObject->GetDataBuff()->PushBack(pData, PacketSize))
				{
					ReleaseOverLappedObject(pOverLappedObject);
					PrintNetLog(_T("要发送的数据包过大！"));
					Disconnect();
					return false;
				}
				pData = (char *)pData + PacketSize;

				static DWORD NumberOfBytes;
				static DWORD Flag;

				NumberOfBytes = 0;
				Flag = 0;

				if (m_Socket.SendOverlapped(
					pOverLappedObject->GetDataBuff()->GetBuffer(),
					pOverLappedObject->GetDataBuff()->GetUsedSize(),
					NumberOfBytes, Flag,
					pOverLappedObject->GetOverlapped()))
				{

					continue;
				}

				PrintNetLog(_T("发出Send请求失败！"));
				ReleaseOverLappedObject(pOverLappedObject);
				Disconnect();
				return false;
			}
		}
		return true;
	}
	return false;
}



bool CNetConnection::QueryRecv()
{
	//CAutoLock Lock(m_EasyCriticalSection);
	
	if(m_Socket.IsConnected())
	{
		//if (m_RecvDataQueue.GetUsedSize() >= m_RecvDataQueue.GetBufferSize())
		//{
		//	PrintNetDebugLog(_T("接收缓冲已满"));			
		//	return false;
		//}
		COverLappedObject * pOverLappedObject = AllocOverLappedObject();
		if(pOverLappedObject==NULL)
		{
			PrintNetLog(_T("创建Recv用OverLappedObject失败！"));
			return false;
		}

		pOverLappedObject->SetType(IO_RECV);	
		pOverLappedObject->GetDataBuff()->SetUsedSize(0);
		pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
		pOverLappedObject->SetParentID(GetID());

		static DWORD NumberOfBytes;
		static DWORD Flag;

		NumberOfBytes=0;
		Flag=0;

		if(m_Socket.RecvOverlapped(
			pOverLappedObject->GetDataBuff()->GetBuffer(),
			pOverLappedObject->GetDataBuff()->GetBufferSize(),
			NumberOfBytes,Flag,
			pOverLappedObject->GetOverlapped()))
		{
			return true;
		}
		PrintNetLog(_T("发出Recv请求失败！"));	
		ReleaseOverLappedObject(pOverLappedObject);
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
		COverLappedObject * pOverLappedObject;
		while(m_RecvDataQueue.PopFront(&pOverLappedObject))
		{			
			OnRecvData((const BYTE *)pOverLappedObject->GetDataBuff()->GetBuffer(), pOverLappedObject->GetDataBuff()->GetUsedSize());
			ReleaseOverLappedObject(pOverLappedObject);
			PacketCount++;
			if(PacketCount>=ProcessPacketLimit)
				break;
		}			
	}

	//处理关闭
	if(m_WantClose)
	{		
		Disconnect();		
	}
	return PacketCount;
}

COverLappedObject * CNetConnection::AllocOverLappedObject()
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	COverLappedObject * pObject = NULL;
	if (m_OverLappedObjectPool.PopFront(&pObject))
	{
		if (pObject->GetStatus() != OVERLAPPED_OBJECT_STATUS_IDLE)
		{
			PrintNetLog(_T("OverLappedObject(%u,%u)状态%u错误"), pObject->GetID(), pObject->GetType(), pObject->GetStatus());
		}

		pObject->SetStatus(OVERLAPPED_OBJECT_STATUS_USING);
		return pObject;
	}
	else
	{
		PrintNetLog(_T("已无OverLappedObject可分配(%u/%u)"),
			m_OverLappedObjectPool.GetUsedSize(), m_OverLappedObjectPool.GetBufferSize());
	}
	return NULL;
}
bool CNetConnection::ReleaseOverLappedObject(COverLappedObject * pObject)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	if (pObject->GetStatus() != OVERLAPPED_OBJECT_STATUS_USING)
	{
		PrintNetLog(_T("OverLappedObject(%u,%u)状态%u错误"), pObject->GetID(), pObject->GetType(), pObject->GetStatus());
		return false;
	}
	
	if (m_OverLappedObjectPool.PushBack(&pObject))
	{
		pObject->SetStatus(OVERLAPPED_OBJECT_STATUS_IDLE);
		return true;
	}
	else
	{
		pObject->Release();
		return false;
	}
}

