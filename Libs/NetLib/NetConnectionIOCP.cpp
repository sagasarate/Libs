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

IMPLEMENT_CLASS_INFO_STATIC(CNetConnection,CBaseNetConnection);

CNetConnection::CNetConnection(void)
{		
	m_pServer=NULL;
	m_WantClose=FALSE;
	m_CurAddressFamily = AF_INET;
	m_pIOCPEventRouter=NULL;
	m_SendQueryCount=0;
	m_UseSendBuffer=0;
	m_IsRecvPaused=false;
	m_SendDelay=0;
	m_SendQueryLimit=0;
}

CNetConnection::~CNetConnection(void)
{
	Destory();
}

BOOL CNetConnection::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{	
	
	if(m_Socket.IsConnected())
	{
		
		
		if(EventID==IOE_PACKET)
		{
			if(pOverLappedObject->GetType()==IO_SEND)
			{
				GetServer()->AddTCPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				GetServer()->DeleteOverLappedObject(pOverLappedObject);	
				
				AtomicDec(&m_SendQueryCount);
				
				return TRUE;
			}
			else if(pOverLappedObject->GetType()==IO_RECV)
			{
				if(pOverLappedObject->GetDataBuff()->GetUsedSize()<=0)
				{						
					GetServer()->DeleteOverLappedObject(pOverLappedObject);
					//PrintNetLog(_T("NetLib"),_T("%s(%d)Connection收对方连接关闭信号(%d)！"),
					//	GetName(),m_Session,
					//	pOverLappedObject->GetDataBuff()->GetUsedSize());
					QueryDisconnect();
					return TRUE;
				}
				else
				{			
					GetServer()->AddTCPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
					
					if(m_DataQueue.PushBack(pOverLappedObject))
					{
						if(!QueryRecv())
						{
							PrintNetLog(_T("NetLib"),_T("无法发出更多的Recv请求,连接关闭！"));
							QueryDisconnect();	
							return FALSE;
						}
						else
						{
							return TRUE;
						}
						
					}
					else
						PrintNetLog(_T("NetLib"),_T("Recv数据队列已满！"));
				}
			}
			else
				PrintNetLog(_T("NetLib"),_T("Connection收到非法IOCP包！"));
			
		}
		else
			PrintNetLog(_T("NetLib"),_T("(%d)Connection收到IOCP错误！"),GetID());
		if(IsConnected())
		{
			//PrintNetLog(_T("NetLib"),_T("(%d)(%d)Connection IOCP出错时连接还未断开,连接关闭！"),GetID(),m_Session);
			QueryDisconnect();
		}
		
	}
	else
	{
		PrintNetLog(_T("NetLib"),_T("(%d)Connection未连接，IOCP包(%u)被忽略！"),GetID(),pOverLappedObject->GetParentID());
	}
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	

	return FALSE;
}

BOOL CNetConnection::Create(UINT RecvQueueSize,UINT SendQueueSize)
{
	

	if (GetServer() == NULL)
	{
		PrintNetLog(_T("NetLib"), _T("(%d)CNetConnection::Create:未设置Server！"), GetID());
		return FALSE;
	}
		
	

	Close();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();	
		m_pIOCPEventRouter->Init(this);
	}
	

	if(m_DataQueue.GetObjectCount())
	{
		PrintNetLog(_T("NetLib"),_T("(%d)CNetConnection::Create:数据队列不是空的！"),GetID());
	}

	
	if(m_DataQueue.GetBufferSize()<RecvQueueSize)
	{
		m_DataQueue.Create(RecvQueueSize);
	}

	if(SendQueueSize)
	{
		PrintNetLog(0,_T("启用发送队列，大小为%u"),SendQueueSize);
		m_UseSendBuffer=true;
		if(m_SendBuffer.GetBufferSize()<SendQueueSize)
		{
			m_SendBuffer.Create(SendQueueSize);
		}
	}
	else
	{
		m_UseSendBuffer=false;
	}
		
		
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
	
	return TRUE;
}

BOOL CNetConnection::Create(SOCKET Socket,UINT RecvQueueSize,UINT SendQueueSize)
{
	

	if (GetServer() == NULL)
	{
		PrintNetLog(_T("NetLib"), _T("(%d)CNetConnection::Create:未设置Server！"), GetID());
		return FALSE;
	}
		
		
	Close();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		m_pIOCPEventRouter->Init(this);
	}
	
	m_Socket.SetSocket(Socket);

	if(m_DataQueue.GetObjectCount())
	{
		PrintNetLog(_T("NetLib"),_T("(%d)CNetConnection::Create:数据队列不是空的！"),GetID());
	}

	if(m_DataQueue.GetBufferSize()<RecvQueueSize)
	{
		m_DataQueue.Create(RecvQueueSize);
	}

	if(SendQueueSize)
	{
		PrintNetLog(0,_T("启用发送队列，大小为%u"),SendQueueSize);
		m_UseSendBuffer=true;
		if(m_SendBuffer.GetBufferSize()<SendQueueSize)
		{
			m_SendBuffer.Create(SendQueueSize);
		}
	}
	else
	{
		m_UseSendBuffer=false;
	}
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);	
	return TRUE;
}

void CNetConnection::Destory()
{	
	Close();
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

BOOL CNetConnection::Connect(const CIPAddress& Address ,DWORD TimeOut)
{

	if(GetServer()==NULL)
		return FALSE;

	if (m_pIOCPEventRouter == NULL)
	{
		if (!Create())
			return FALSE;
	}

	if (Address.IsIPv4())
		m_CurAddressFamily = AF_INET;
	else if (Address.IsIPv6())
		m_CurAddressFamily = AF_INET6;

	if (!m_Socket.MakeSocket(m_CurAddressFamily, SOCK_STREAM, IPPROTO_TCP))
		return FALSE;

	m_pIOCPEventRouter->Init(this);


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
	//PrintNetLog(_T("NetLib"),_T("(%d)Connection关闭"),GetID());

	//PrintNetLog(_T("NetLib"),_T("%s连接关闭"),GetName());

	if (IsConnected())
		OnDisconnection();
	
	m_Socket.Close();
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
	m_SendQueryCount=0;

	COverLappedObject * pOverLappedObject;
	while(m_DataQueue.PopFront(pOverLappedObject))
	{			
		GetServer()->DeleteOverLappedObject(pOverLappedObject);		
	}	
	
	while(m_SendBuffer.PopFront(pOverLappedObject))
	{			
		GetServer()->DeleteOverLappedObject(pOverLappedObject);		
	}	

	
}
void CNetConnection::QueryDisconnect()
{
	InterlockedExchange((LPLONG)&(m_WantClose),TRUE);
}



BOOL CNetConnection::StartWork()
{	
	//PrintNetLog(_T("NetLib"),_T("(%d)Connection开始工作"),GetID());

	m_Socket.SetState(CNetSocket::SS_CONNECTED);	
	m_pIOCPEventRouter->Init(this);
	m_SendQueryCount=0;
	

	if(!m_Socket.EnableBlocking(FALSE))
	{
		OnConnection(FALSE);
		PrintNetLog(_T("NetLib"),_T("(%d)Connection开始非阻塞模式失败！"),GetID());
		m_Socket.Close();		
		return FALSE;
	}

	if(!GetServer()->BindSocket(m_Socket.GetSocket()))
	{
		OnConnection(FALSE);
		PrintNetLog(_T("NetLib"),_T("(%d)Connection绑定IOCP失败！"),GetID());
		m_Socket.Close();		
		return FALSE;
	}
	
	if(!QueryRecv())
	{
		OnConnection(FALSE);
		PrintNetLog(_T("NetLib"),_T("(%d)Connection初始化Recv失败！"),GetID());
		m_Socket.Close();
		return FALSE;
	}
		
	//CEasyString ConnectionName;
	//ConnectionName.Format(_T("[%s:%d]"),GetRemoteAddress().GetIPString(),GetRemoteAddress().GetPort());
	//SetName(ConnectionName);;


	//PrintNetLog(_T("NetLib"),_T("%s连接建立[%u]"),GetName(),(UINT)m_Socket.GetSocket());

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
	if(m_UseSendBuffer)
	{
		UINT FreeSize=(m_SendBuffer.GetBufferSize()-m_SendBuffer.GetObjectCount())*NET_DATA_BLOCK_SIZE;
		if(FreeSize<Size)
		{
			int FlushCount=(Size/NET_DATA_BLOCK_SIZE)*2;
			DoBufferSend(FlushCount);			
		}
		void * Pos=m_SendBuffer.GetLastObjectPos();
		if(Pos)
		{
			//先将最后一个包填满
			COverLappedObject ** ppOverLappedObject=m_SendBuffer.GetObject(Pos);
			if(ppOverLappedObject)
			{
				if((*ppOverLappedObject)->GetDataBuff()->GetFreeSize())
				{
					UINT PushSize=(*ppOverLappedObject)->GetDataBuff()->GetFreeSize();
					if(PushSize>Size)
						PushSize=Size;
					(*ppOverLappedObject)->GetDataBuff()->PushBack(pData,PushSize);
					pData=(char *)pData+PushSize;
					Size-=PushSize;
				}
			}
		}		
		while(Size)
		{
			int PacketSize=Size;
			if(PacketSize>NET_DATA_BLOCK_SIZE)
				PacketSize=NET_DATA_BLOCK_SIZE;

			COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
			if(pOverLappedObject)
			{
				pOverLappedObject->SetType(IO_SEND);
				pOverLappedObject->GetDataBuff()->SetUsedSize(0);

				if(pOverLappedObject->GetDataBuff()->PushBack(pData,PacketSize))
				{
					
					pData=(char *)pData+PacketSize;
					Size-=PacketSize;
					if(m_SendBuffer.PushBack(pOverLappedObject))
					{
						continue;
					}
					else
					{
						PrintNetLog(0,_T("异常，发送缓冲已满"));
					}
					
				}
				else
				{
					PrintNetLog(_T("NetLib"),_T("Connection要发送的数据包过大！"));
				}
				
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
			}
			else
			{
				PrintNetLog(_T("NetLib"),_T("Connection创建Send用OverLappedObject失败！"));
			}
			Disconnect();
			return FALSE;
			
		}
		return TRUE;
	}
	else
	{
		return QuerySend(pData,Size);
	}
}

BOOL CNetConnection::SendDirect(LPCVOID pData,UINT Size)
{
	return QuerySend(pData,Size);;
}

BOOL CNetConnection::QuerySend(LPCVOID pData,UINT Size)
{		
	//CAutoLock Lock(m_EasyCriticalSection);
	
	FUNCTION_BEGIN;
	if(m_Socket.IsConnected())
	{
		
		while(Size)
		{
			int PacketSize=Size;
			if(PacketSize>NET_DATA_BLOCK_SIZE)
				PacketSize=NET_DATA_BLOCK_SIZE;
			Size-=PacketSize;

			
			COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
			if(pOverLappedObject==NULL)
			{
				PrintNetLog(_T("NetLib"),_T("Connection创建Send用OverLappedObject失败！"));
				Disconnect();
				return FALSE;
			}

			pOverLappedObject->SetType(IO_SEND);	
			pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
			pOverLappedObject->GetDataBuff()->SetUsedSize(0);
			pOverLappedObject->SetParentID(GetID());

			if(!pOverLappedObject->GetDataBuff()->PushBack(pData,PacketSize))
			{
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
				PrintNetLog(_T("NetLib"),_T("Connection要发送的数据包过大！"));
				Disconnect();
				return FALSE;
			}
			pData=(char *)pData+PacketSize;

			static DWORD NumberOfBytes;
			static DWORD Flag;

			NumberOfBytes=0;
			Flag=0;
			
			AtomicInc(&m_SendQueryCount);
			if(m_Socket.SendOverlapped(
				pOverLappedObject->GetDataBuff()->GetBuffer(),
				pOverLappedObject->GetDataBuff()->GetUsedSize(),
				NumberOfBytes,Flag,
				pOverLappedObject->GetOverlapped()))
			{
				
				continue;
			}
			AtomicDec(&m_SendQueryCount);
			PrintNetLog(_T("NetLib"),_T("发出Send请求失败！"));	
			GetServer()->DeleteOverLappedObject(pOverLappedObject);
			Disconnect();
			return FALSE;
		}
		return TRUE;
	}	

	FUNCTION_END;
	return FALSE;	
}

BOOL CNetConnection::QueryRecv()
{
	//CAutoLock Lock(m_EasyCriticalSection);
	if(m_IsRecvPaused)
	{
		return TRUE;
	}

	if(m_Socket.IsConnected())
	{
		if(m_DataQueue.GetObjectCount()>=m_DataQueue.GetBufferSize())
		{
			PrintNetDebugLog(0,_T("接收缓冲已满，暂停接收"));
			m_IsRecvPaused=true;
			return TRUE;
		}
		COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
		if(pOverLappedObject==NULL)
		{
			PrintNetLog(_T("NetLib"),_T("(%d)Connection创建Recv用OverLappedObject失败！"),GetID());
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

		if(m_Socket.RecvOverlapped(
			pOverLappedObject->GetDataBuff()->GetBuffer(),
			pOverLappedObject->GetDataBuff()->GetBufferSize(),
			NumberOfBytes,Flag,
			pOverLappedObject->GetOverlapped()))
		{
			return TRUE;
		}
		PrintNetLog(_T("NetLib"),_T("(%d)发出Recv请求失败！"),GetID());	
		GetServer()->DeleteOverLappedObject(pOverLappedObject);
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
		COverLappedObject * pOverLappedObject;
		while(m_DataQueue.PopFront(pOverLappedObject))
		{			
			OnRecvData((const BYTE *)pOverLappedObject->GetDataBuff()->GetBuffer(), pOverLappedObject->GetDataBuff()->GetUsedSize());
			GetServer()->DeleteOverLappedObject(pOverLappedObject);
			PacketCount++;
			if(PacketCount>=ProcessPacketLimit)
				break;
		}			
	}

	m_IsRecvPaused=false;

	//处理缓冲的发送
	if(m_UseSendBuffer)
	{
		if(m_SendDelay)
		{
			if(m_SendDelayTimer.IsTimeOut(m_SendDelay))
			{
				m_SendDelayTimer.SaveTime();
				PacketCount+=DoBufferSend(ProcessPacketLimit);
			}
		}
		else
		{
			PacketCount+=DoBufferSend(ProcessPacketLimit);
		}
		
	}

	//处理关闭
	if(m_WantClose)
	{		
		Disconnect();		
	}
	return PacketCount;
}

UINT CNetConnection::GetCurSendQueryCount()
{
	return m_SendQueryCount;
}


//bool CNetConnection::StealFrom(CNameObject * pObject,UINT Param)
//{
//	PrintNetLog(_T("NetLib"),_T("(%d)执行连接替换(%d)！"),GetID(),pObject->GetID());
//	if(pObject->IsKindOf(GET_CLASS_INFO(CNetConnection)))
//	{
//		Close();
//
//		COverLappedObject * pOverLappedObject;
//
//		if(!CNameObject::StealFrom(pObject,Param))
//			return false;
//		CNetConnection * pConnection=(CNetConnection *)pObject;
//		if(!m_Socket.StealFrom(&(pConnection->m_Socket),Param))
//			return false;
//		
//		m_pServer=pConnection->m_pServer;
//		m_WantClose=pConnection->m_WantClose;
//		m_pIOCPEventRouter=pConnection->m_pIOCPEventRouter;
//		pConnection->m_pIOCPEventRouter=NULL;
//		if(m_pIOCPEventRouter)
//			m_pIOCPEventRouter->SetEventHander(this);
//
//		m_IsRecvPaused=pConnection->m_IsRecvPaused;
//		m_SendDelay=pConnection->m_SendDelay;
//
//		m_SendQueryCount=pConnection->m_SendQueryCount;
//		m_UseSendBuffer=pConnection->m_UseSendBuffer;
//		if(m_UseSendBuffer)
//		{
//			m_SendBuffer.Create(pConnection->m_SendBuffer.GetBufferSize());
//			while(pConnection->m_SendBuffer.PopFront(pOverLappedObject))
//			{
//				m_SendBuffer.PushBack(pOverLappedObject);
//			}		
//		}
//
//		m_DataQueue.Create(pConnection->m_DataQueue.GetBufferSize());
//		
//		while(pConnection->m_DataQueue.PopFront(pOverLappedObject))
//		{
//			m_DataQueue.PushBack(pOverLappedObject);
//		}		
//
//		return true;
//		
//	}
//	return false;
//}

int CNetConnection::DoBufferSend(int ProcessPacketLimit)
{
	int ProcessCount=0;

	if(m_Socket.IsConnected())
	{
		if(m_SendQueryLimit)
		{
			if(GetCurSendQueryCount()>m_SendQueryLimit)
				return ProcessCount;
		}
		COverLappedObject * pOverLappedObject;
		while(m_SendBuffer.PopFront(pOverLappedObject))
		{			
			pOverLappedObject->SetType(IO_SEND);	
			pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
			pOverLappedObject->SetParentID(GetID());

			static DWORD NumberOfBytes;
			static DWORD Flag;

			NumberOfBytes=0;
			Flag=0;

			AtomicInc(&m_SendQueryCount);
			if(!m_Socket.SendOverlapped(
				pOverLappedObject->GetDataBuff()->GetBuffer(),
				pOverLappedObject->GetDataBuff()->GetUsedSize(),
				NumberOfBytes,Flag,
				pOverLappedObject->GetOverlapped()))
			{

				AtomicDec(&m_SendQueryCount);
				PrintNetLog(_T("NetLib"),_T("发出Send请求失败！"));	
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
				Disconnect();
				break;
			}

			if(m_SendQueryLimit)
			{
				if(GetCurSendQueryCount()>m_SendQueryLimit)
					return ProcessCount;
			}

			ProcessCount++;
			if(ProcessCount>=ProcessPacketLimit)
				break;
		}
	}

	return ProcessCount;

}