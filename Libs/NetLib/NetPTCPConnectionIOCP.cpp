/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPTCPConnectionIOCP.cpp                                */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CNetPTCPConnection,CBaseTCPConnection);

CNetPTCPConnection::CNetPTCPConnection(void)
{		
	m_pServer=NULL;
	m_pIOCPEventRouter=NULL;
	m_SendQueryCount=0;
}

CNetPTCPConnection::~CNetPTCPConnection(void)
{
	Destory();
}

BOOL CNetPTCPConnection::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{	
	if(m_Socket.IsConnected())
	{
		
		if(EventID==IOE_PACKET)
		{
			if(pOverLappedObject->GetType()==IO_SEND)
			{
				AtomicDec(&m_SendQueryCount);
				GetServer()->AddTCPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				GetServer()->DeleteOverLappedObject(pOverLappedObject);	
				return TRUE;
			}
			else if(pOverLappedObject->GetType()==IO_RECV)
			{
				if(pOverLappedObject->GetDataBuff()->GetUsedSize()<=0)
				{						
					GetServer()->DeleteOverLappedObject(pOverLappedObject);
					//PrintNetLog(0xffffffff,_T("%s(%d)Connection收对方连接关闭信号(%d)！"),
					//	GetName(),m_Session,
					//	pOverLappedObject->GetDataBuff()->GetUsedSize());
					Disconnect();
					return TRUE;
				}
				else
				{			
					GetServer()->AddTCPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
					OnRecvData(*(pOverLappedObject->GetDataBuff()));
					GetServer()->DeleteOverLappedObject(pOverLappedObject);
					if(!QueryRecv())
					{
						PrintNetLog(0xffffffff,_T("无法发出更多的Recv请求,连接关闭！"));
						Disconnect();	
						return FALSE;
					}
					else
					{								
						return TRUE;							
					}
				}
			}
			else
				PrintNetLog(0xffffffff,_T("Connection收到非法IOCP包！"));
			
		}

		//else
		//	PrintNetLog(0xffffffff,_T("(%d)(%d)Connection收到IOCP错误！"),GetID(),m_Session);
		if(IsConnected())
		{
			//PrintNetLog(0xffffffff,_T("(%d)(%d)Connection IOCP出错时连接还未断开,连接关闭！"),GetID(),m_Session);
			Disconnect();
		}		
	}
	else
	{
		//PrintNetLog(0xffffffff,_T("(%d)(%d)Connection未连接，IOCP包(%u)被忽略！"),GetID(),m_Session,pOverLappedObject->GetParentID());
	}
	GetServer()->DeleteOverLappedObject(pOverLappedObject);


	return FALSE;
}

BOOL CNetPTCPConnection::Create(UINT RecvQueueSize,UINT SendQueueSize)
{

	if(GetServer()==NULL)
		return FALSE;

	Destory();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();	
		m_pIOCPEventRouter->Init(this);
	}
	m_Socket.MakeSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);		
	
	return TRUE;
}

BOOL CNetPTCPConnection::Create(SOCKET Socket,UINT RecvQueueSize,UINT SendQueueSize)
{

	if(GetServer()==NULL)
		return FALSE;

	Destory();

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		m_pIOCPEventRouter->Init(this);
	}
	
	m_Socket.SetSocket(Socket);
	
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

	if(m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter=NULL;
	}	

	m_SendQueryCount=0;
	
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
void CNetPTCPConnection::Disconnect()
{

	OnDisconnection();

	m_Socket.Close();
	
	
}


BOOL CNetPTCPConnection::IsConnected()
{
	return m_Socket.IsConnected();
}

BOOL CNetPTCPConnection::IsDisconnected()
{
	return m_Socket.IsDisconnected();
}

BOOL CNetPTCPConnection::StartWork()
{	
	//PrintNetLog(0xffffffff,_T("(%d)Connection开始工作"),GetID());	

	m_Socket.SetState(CNetSocket::SS_CONNECTED);	
	m_pIOCPEventRouter->Init(this);

	m_SendQueryCount=0;

	if(!m_Socket.EnableBlocking(FALSE))
	{
		OnConnection(FALSE);
		PrintNetLog(0xffffffff,_T("(%d)Connection开始非阻塞模式失败！"),GetID());
		m_Socket.Close();		
		return FALSE;
	}

	if(!GetServer()->BindSocket(m_Socket.GetSocket()))
	{
		OnConnection(FALSE);
		PrintNetLog(0xffffffff,_T("(%d)Connection绑定IOCP失败！"),GetID());
		m_Socket.Close();		
		return FALSE;
	}
	
	if(!QueryRecv())
	{
		OnConnection(FALSE);
		PrintNetLog(0xffffffff,_T("(%d)Connection初始化Recv失败！"),GetID());
		m_Socket.Close();
		return FALSE;
	}
		
	
	//PrintNetLog(0xffffffff,_T("%s连接建立[%u]"),GetName(),(UINT)m_Socket.GetSocket());

	OnConnection(TRUE);
	return TRUE;
}

void CNetPTCPConnection::OnConnection(BOOL IsSucceed)
{
	
}

void CNetPTCPConnection::OnDisconnection()
{
}

BOOL CNetPTCPConnection::QuerySend(LPCVOID pData,int Size)
{		

	if(m_Socket.IsConnected())
	{
		while(Size)
		{
			int PacketSize=Size;
			if(PacketSize>MAX_DATA_PACKET_SIZE)
				PacketSize=MAX_DATA_PACKET_SIZE;
			Size-=PacketSize;
		

			COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
			if(pOverLappedObject==NULL)
			{
				PrintNetLog(0xffffffff,_T("Connection创建Send用OverLappedObject失败！"));
				return FALSE;
			}

			pOverLappedObject->SetType(IO_SEND);	
			pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
			pOverLappedObject->GetDataBuff()->SetUsedSize(0);
			pOverLappedObject->SetParentID(GetID());

			if(!pOverLappedObject->GetDataBuff()->PushBack(pData,PacketSize))
			{
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
				PrintNetLog(0xffffffff,_T("Connection要发送的数据包过大！"));
				return FALSE;
			}
			pData=(char *)pData+PacketSize;

			static DWORD NumberOfBytes;
			static DWORD Flag;

			NumberOfBytes=0;
			Flag=0;

			if(m_Socket.SendOverlapped(
				pOverLappedObject->GetDataBuff()->GetBuffer(),
				pOverLappedObject->GetDataBuff()->GetUsedSize(),
				NumberOfBytes,Flag,
				pOverLappedObject->GetOverlapped()))
			{			
				AtomicInc(&m_SendQueryCount);
				continue;
			}
			PrintNetLog(0xffffffff,_T("发出Send请求失败！"));	
			GetServer()->DeleteOverLappedObject(pOverLappedObject);
			return FALSE;
		}
		return TRUE;
	}	
	return FALSE;	
}

BOOL CNetPTCPConnection::QueryRecv()
{
	if(m_Socket.IsConnected())
	{
		COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
		if(pOverLappedObject==NULL)
		{
			PrintNetLog(0xffffffff,_T("(%d)Connection创建Recv用OverLappedObject失败！"),GetID());
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
		PrintNetLog(0xffffffff,_T("(%d)发出Recv请求失败！"),GetID());	
		GetServer()->DeleteOverLappedObject(pOverLappedObject);
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
		PacketCount++;
	}
		
	return PacketCount;
}


UINT CNetPTCPConnection::GetCurSendQueryCount()
{
	return m_SendQueryCount;
}
