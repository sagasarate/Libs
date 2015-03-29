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


IMPLEMENT_CLASS_INFO_STATIC(CNetPUDPService,CBaseService);

CNetPUDPService::CNetPUDPService()
{
	m_WantClose=FALSE;
	m_pServer=NULL;	
	m_ParallelRecvCount=0;	
	m_pIOCPEventRouter=NULL;	
	
	
}

CNetPUDPService::~CNetPUDPService(void)
{
	Destory();
}

BOOL CNetPUDPService::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{		
	if(IsWorking())
	{
		if(pOverLappedObject->GetType()==IO_RECV)
		{			
			if(!QueryUDPRecv())
			{
				PrintNetLog(0xffffffff,_T("PUDPService无法发出更多的UDPRecv请求！"));
				Close();
			}

			if(EventID==IOE_PACKET)
			{
				GetServer()->AddUDPRecvBytes(pOverLappedObject->GetDataBuff()->GetUsedSize());
				OnRecvData(pOverLappedObject->GetAddress(),*pOverLappedObject->GetDataBuff());
				GetServer()->DeleteOverLappedObject(pOverLappedObject);
				return TRUE;
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
			PrintNetLog(0xffffffff,_T("PUDPService收到非法IOCP包！"));
	}
	else
		PrintNetLog(0xffffffff,_T("PUDPService未启用，IOCP包被忽略！"));
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	return FALSE;
}

BOOL CNetPUDPService::Create(int ParallelRecvCount)
{	
	if(GetServer()==NULL)
		return FALSE;

	if(m_pIOCPEventRouter==NULL)
	{
		m_pIOCPEventRouter=GetServer()->CreateEventRouter();
		m_pIOCPEventRouter->Init(this);
	}

	m_ParallelRecvCount=ParallelRecvCount;	
	m_Socket.MakeSocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
	
	return TRUE;
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
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);
}


BOOL CNetPUDPService::StartListen(const CIPAddress& Address)
{
	

	if(m_Socket.GetState()==CNetSocket::SS_UNINITED)
	{
		if(!Create())
			return FALSE;
	}
	if(!m_Socket.Listen(Address))
	{
		PrintNetLog(0xffffffff,_T("(%d)PUDPService侦听失败！"),GetID());
		return FALSE;
	}


	
	
	if(!GetServer()->BindSocket(m_Socket.GetSocket()))
	{
		PrintNetLog(0xffffffff,_T("(%d)PUDPService绑定IOCP失败！"),GetID());
		Close();
		return FALSE;
	}	
	
	
			
	for(int i=0;i<m_ParallelRecvCount;i++)
	{
		if(!QueryUDPRecv())
		{
			PrintNetLog(0xffffffff,_T("(%d)Service发出UDPRecv请求失败！"),GetID());
			Close();
			return FALSE;
		}
	}
	
	OnStartUp();
	return TRUE;
}
void CNetPUDPService::Close()
{	

	m_Socket.Close();
	
	InterlockedExchange((LPLONG)&(m_WantClose),FALSE);	

	OnClose();	
}


void CNetPUDPService::OnStartUp()
{
}

void CNetPUDPService::OnClose()
{
}


BOOL CNetPUDPService::QueryUDPRecv()
{	

	COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(0xffffffff,_T("(%d)PUDPService创建热UDPRecv用OverLappedObject失败！"),GetID());
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
	PrintNetLog(0xffffffff,_T("(%d)PUDPService发出UDPRecv请求失败！"),GetID());	
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	return FALSE;
}

BOOL CNetPUDPService::QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size)
{		
	COverLappedObject * pOverLappedObject=GetServer()->CreateOverLappedObject();
	if(pOverLappedObject==NULL)
	{
		PrintNetLog(0xffffffff,_T("(%d)PUDPService创建UDPSend用OverLappedObject失败！"),GetID());
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
		PrintNetLog(0xffffffff,_T("(%d)PUDPService要发送的数据包过大！"),GetID());
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
	PrintNetLog(0xffffffff,_T("(%d)PUDPService发出UDPSend请求失败！"),GetID());	
	GetServer()->DeleteOverLappedObject(pOverLappedObject);
	return FALSE;
}

void CNetPUDPService::OnRecvData(const CIPAddress& IPAddress,const CEasyBuffer& DataBuffer)
{
}

