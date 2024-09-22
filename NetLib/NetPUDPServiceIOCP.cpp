/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPUDPServiceIOCP.cpp                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


IMPLEMENT_CLASS_INFO_STATIC(CNetPUDPService,CBaseNetService);

CNetPUDPService::CNetPUDPService()
{
	m_WantClose=false;
	m_pServer=NULL;	
	m_ParallelRecvCount=0;	
	m_pIOCPEventRouter=NULL;	
	m_OverLappedObjectPool.SetTag(_T("CNetPUDPService"));
	
}

CNetPUDPService::~CNetPUDPService(void)
{
	Destory();
}

bool CNetPUDPService::OnIOCPEvent(int EventID, COverLappedObject * pOverLappedObject)
{		
	if(IsWorking())
	{
		if(pOverLappedObject->GetType()==IO_RECV)
		{			
			if(!QueryUDPRecv())
			{
				PrintNetLog(_T("PUDPService无法发出更多的UDPRecv请求！"));
				Close();
			}

			if(EventID==IOE_PACKET)
			{
				GetServer()->AddUDPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				OnRecvData(pOverLappedObject->GetAddress(),
					(const BYTE *)pOverLappedObject->GetDataBuff()->GetBuffer(), (UINT)pOverLappedObject->GetDataBuff()->GetUsedSize());
				ReleaseOverLappedObject(pOverLappedObject);				
				return true;
			}
			else
			{
				PrintNetLog(_T("接收失败！"));
			}				
		}
		else if(pOverLappedObject->GetType()==IO_SEND)
		{
			if(EventID==IOE_PACKET)
			{
				GetServer()->AddUDPSendBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				ReleaseOverLappedObject(pOverLappedObject);
				return true;
			}
			else
				PrintNetLog(_T("发送失败！"));
		}			
		else
			PrintNetLog(_T("PUDPService收到非法IOCP包！"));
	}
	else
		PrintNetLog(_T("PUDPService未启用，IOCP包被忽略！"));
	ReleaseOverLappedObject(pOverLappedObject);
	return false;
}

bool CNetPUDPService::Create(UINT ParallelRecvCount)
{	
	if(GetServer()==NULL)
		return false;

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

	m_ParallelRecvCount=ParallelRecvCount;		
	m_WantClose = false;	

	if (m_OverLappedObjectPool.GetBufferSize() < m_ParallelRecvCount)
	{
		if (m_OverLappedObjectPool.GetUsedSize())
		{
			GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		}

		m_OverLappedObjectPool.Create(m_ParallelRecvCount);
		GetServer()->AllocOverLappedObject(m_OverLappedObjectPool, m_ParallelRecvCount, GetID());
		if (m_OverLappedObjectPool.GetUsedSize() <= 0)
		{
			PrintNetLog(_T("无法分配到OverLappedObject"));
			return false;
		}
	}
	
	return true;
}
void CNetPUDPService::Destory()
{
	

	if(m_pIOCPEventRouter)
	{
		m_pIOCPEventRouter->SetEventHander(NULL);
		GetServer()->DeleteEventRouter(m_pIOCPEventRouter);
		m_pIOCPEventRouter=NULL;
	}

	m_Socket.Close();	
	
	m_WantClose = false;

	if (GetServer())
	{
		GetServer()->ReleaseOverLappedObject(m_OverLappedObjectPool);
		m_OverLappedObjectPool.Destory();
	}
}


bool CNetPUDPService::StartListen(const CIPAddress& Address)
{
	if(m_Socket.GetState()==CNetSocket::SS_UNINITED)
	{
		if(!Create())
			return false;
	}

	int af = AF_INET;

	if (Address.IsIPv4())
		af = AF_INET;
	else if (Address.IsIPv6())
		af = AF_INET6;
	else
	{
		PrintNetLog( _T("(%d)PUDPService协议错误！"), GetID());
		return false;
	}

	if (!m_Socket.MakeSocket(af, SOCK_DGRAM, IPPROTO_UDP))
		return false;

	if(!m_Socket.Listen(Address))
	{
		PrintNetLog(_T("(%d)PUDPService侦听失败！"),GetID());
		return false;
	}


	
	
	if(!GetServer()->BindSocket(m_Socket.GetSocket()))
	{
		PrintNetLog(_T("(%d)PUDPService绑定IOCP失败！"),GetID());
		Close();
		return false;
	}	
	
	
			
	for (UINT i = 0; i<m_ParallelRecvCount; i++)
	{
		if(!QueryUDPRecv())
		{
			PrintNetLog(_T("(%d)Service发出UDPRecv请求失败！"),GetID());
			Close();
			return false;
		}
	}
	
	OnStartUp();
	return true;
}
void CNetPUDPService::Close()
{	

	m_Socket.Close();
	
	m_WantClose = false;

	OnClose();	
}


void CNetPUDPService::OnStartUp()
{
}

void CNetPUDPService::OnClose()
{
}


bool CNetPUDPService::QueryUDPRecv()
{	

	COverLappedObject * pOverLappedObject = AllocOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(_T("(%d)PUDPService创建热UDPRecv用OverLappedObject失败！"),GetID());
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

	if(m_Socket.RecvFromOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetAddressLen(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		(int)pOverLappedObject->GetDataBuff()->GetBufferSize(),
		NumberOfBytes,Flag,
		pOverLappedObject->GetOverlapped()))
	{
		return true;
	}
	PrintNetLog(_T("(%d)PUDPService发出UDPRecv请求失败！"),GetID());	
	ReleaseOverLappedObject(pOverLappedObject);
	return false;
}

bool CNetPUDPService::QueryUDPSend(const CIPAddress& IPAddress, LPCVOID pData, int Size)
{		
	COverLappedObject * pOverLappedObject = AllocOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(_T("(%d)PUDPService创建UDPSend用OverLappedObject失败！"),GetID());
		return false;
	}

	pOverLappedObject->SetAddress(IPAddress);

	pOverLappedObject->SetType(IO_SEND);	
	pOverLappedObject->SetIOCPEventRouter(m_pIOCPEventRouter);
	pOverLappedObject->GetDataBuff()->SetUsedSize(0);
	pOverLappedObject->SetParentID(GetID());

	if(!pOverLappedObject->GetDataBuff()->PushBack(pData,Size))
	{
		ReleaseOverLappedObject(pOverLappedObject);
		PrintNetLog(_T("(%d)PUDPService要发送的数据包过大！"),GetID());
		return false;
	}
	

	static DWORD NumberOfBytes;
	static DWORD Flag;

	NumberOfBytes=0;
	Flag=0;

	if(m_Socket.SendToOverlapped(pOverLappedObject->GetAddress(),
		pOverLappedObject->GetDataBuff()->GetBuffer(),
		(int)pOverLappedObject->GetDataBuff()->GetUsedSize(),
		NumberOfBytes,Flag,
		pOverLappedObject->GetOverlapped()))
	{				
		return true;
	}
	PrintNetLog(_T("(%d)PUDPService发出UDPSend请求失败！"),GetID());	
	ReleaseOverLappedObject(pOverLappedObject);
	return false;
}


COverLappedObject * CNetPUDPService::AllocOverLappedObject()
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	COverLappedObject * pObject = NULL;
	if (m_OverLappedObjectPool.PopFront(&pObject))
	{
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
bool CNetPUDPService::ReleaseOverLappedObject(COverLappedObject * pObject)
{
	CAutoLock Lock(m_OverLappedObjectPoolLock);

	
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