/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkConnection.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


#define LINK_CONNECT_TIME		(30*1000)
#define KEEP_ALIVE_TIME			(10*1000)
#define MAX_KEEP_ALIVE_COUNT	6

LPCTSTR g_szLinkStatus[ENL_LINK_MAX]={_T("None"),_T("Accepting"),_T("Accepted")};

IMPLEMENT_CLASS_INFO_STATIC(CEasyNetLinkConnection,CNetConnection);

CEasyNetLinkConnection::CEasyNetLinkConnection(void)
{
	m_pManager=NULL;
	m_Status=ENL_LINK_NONE;
	m_ReportID=0;
	m_IsKeepConnect=false;
	m_AssembleBuffer.Create(MAX_EASY_NET_LINK_MSG_SIZE+MAX_DATA_PACKET_SIZE);
	m_SendBuffer.Create(MAX_EASY_NET_LINK_MSG_SIZE);
	m_KeepAliveCount=0;
	m_NeedReallocConnectionID=FALSE;
	m_pStealer=NULL;
}

CEasyNetLinkConnection::~CEasyNetLinkConnection(void)
{
	PrintNetLog(0xffffffff,_T("[%s]连接[%s:%u]释放"),
		CClassifiedID(GetReportID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());
}


void CEasyNetLinkConnection::SetMaxPacketSize(UINT MaxPacketSize)
{
	if (MaxPacketSize>m_SendBuffer.GetBufferSize())
	{
		m_AssembleBuffer.Create(MaxPacketSize+MAX_DATA_PACKET_SIZE);
		m_SendBuffer.Create(MaxPacketSize);
	}
}


void CEasyNetLinkConnection::OnLinkStart()
{
}

void CEasyNetLinkConnection::OnLinkEnd()
{
}

void CEasyNetLinkConnection::OnData(const CEasyBuffer& DataBuffer)
{
}

void CEasyNetLinkConnection::SendData(LPCVOID pData,int DataSize)
{
	if(pData&&DataSize>0&&(UINT)DataSize<=m_SendBuffer.GetBufferSize()-sizeof(EASY_NET_LINK_MSG_HEAD))
	{
		SendLinkMsg(EASY_NET_LINK_MSG_USER_DATA,pData,DataSize);
	}
	else
	{
		PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::SendData 发送的数据大小不合法"));
	}
}

int CEasyNetLinkConnection::Update(int ProcessPacketLimit)
{
	int Process=CNetConnection::Update(ProcessPacketLimit);
	if(m_KeepAliveCount>MAX_KEEP_ALIVE_COUNT)
	{
		PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::Update 连接[%s]超时被断开"),
			CClassifiedID(GetID()).ToStr());
		Disconnect();
	}
	if(IsDisconnected())
	{
		if(IsKeepConnect())
		{
			if(m_ReconnectTimer.IsTimeOut(EASY_LINK_RECONNECT_TIME))
			{
				m_ReconnectTimer.SaveTime();
				Connect(GetRemoteAddress(),LINK_CONNECT_TIME);
				PrintNetLog(0xffffffff,_T("[%s]开始连接[%s:%u]"),
					CClassifiedID(GetReportID()).ToStr(),
					GetRemoteAddress().GetIPString(),
					GetRemoteAddress().GetPort());
			}
		}
		else
		{
			if(m_pManager)
				m_pManager->DeleteConnection(this);
			Release();
			return Process;
		}
	}
	else if(GetStatus()==ENL_LINK_ACCEPTED)
	{
		if(m_KeepAliveTimer.IsTimeOut(KEEP_ALIVE_TIME))
		{
			m_KeepAliveCount++;
			if(IsKeepConnect())
			{
				SendLinkMsg(EASY_NET_LINK_MSG_KEEP_ALIVE);
			}
			m_KeepAliveTimer.SaveTime();
		}
	}
	if(m_pStealer)
	{
		//执行替换
		if(m_pStealer->StealFrom(this))
		{
			if(m_pManager)
			{
				m_pManager->OnLinkStart(m_pStealer);
				m_pStealer->OnLinkStart();
			}
			else
			{
				m_pStealer->Disconnect();
				PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::Update 连接[%s][%s:%u]执行替换时没有设置管理器，被关闭"),
					CClassifiedID(GetID()).ToStr(),
					GetRemoteAddress().GetIPString(),
					GetRemoteAddress().GetPort());
			}

		}
		else
		{
			m_pStealer->Disconnect();
		}
		m_pStealer=NULL;

		if(m_pManager)
			m_pManager->DeleteConnection(this);
		Release();
	}
	return Process;
}


void CEasyNetLinkConnection::OnRecvData(const CEasyBuffer& DataBuffer)
{

	if(!m_AssembleBuffer.PushBack(DataBuffer.GetBuffer(),DataBuffer.GetUsedSize()))
	{
		PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 接受缓冲区,连接断开"));
		Disconnect();
		return;
	}
	EASY_NET_LINK_MSG * pMsg=(EASY_NET_LINK_MSG *)m_AssembleBuffer.GetBuffer();
	while(m_AssembleBuffer.GetUsedSize()>=(int)sizeof(EASY_NET_LINK_MSG_HEAD)&&
		(UINT)m_AssembleBuffer.GetUsedSize()>=pMsg->Header.Size&&
		m_pStealer==NULL)
	{
		UINT MsgSize=pMsg->Header.Size;
		if(MsgSize<sizeof(EASY_NET_LINK_MSG_HEAD))
		{
			PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 收到大小非法的消息,连接断开"));
			Disconnect();
			return;
		}
		switch(pMsg->Header.MsgID)
		{
		case EASY_NET_LINK_MSG_LINK_START:
			if(MsgSize>=sizeof(EASY_NET_LINK_MSG_HEAD)+sizeof(EASY_NET_LINK_INFO))
			{
				EASY_NET_LINK_INFO * pInfo=(EASY_NET_LINK_INFO *)pMsg->Data;
				SetID(pInfo->ID);
				if(m_pManager)
				{
					if(!m_pManager->AcceptConnection(this))
					{
						PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 非法连接[%s][%s:%u]被拒绝"),
							CClassifiedID(GetID()).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
						Disconnect();
					}
					else
					{
						PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 连接[%s][%s:%u]建立"),
							CClassifiedID(GetID()).ToStr(),
							GetRemoteAddress().GetIPString(),
							GetRemoteAddress().GetPort());
					}
				}
				else
				{
					PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 连接[%s][%s:%u]建立时没有设置管理器，被关闭"),
						CClassifiedID(GetID()).ToStr(),
						GetRemoteAddress().GetIPString(),
						GetRemoteAddress().GetPort());
					Disconnect();
				}

			}
			break;
		case EASY_NET_LINK_MSG_KEEP_ALIVE:
			if(GetStatus()==ENL_LINK_ACCEPTED)
			{
				m_KeepAliveCount=0;
				if(!IsKeepConnect())
				{
					SendLinkMsg(EASY_NET_LINK_MSG_KEEP_ALIVE);
				}
			}
			break;
		case EASY_NET_LINK_MSG_USER_DATA:
			if(MsgSize>=sizeof(EASY_NET_LINK_MSG_HEAD))
			{
				CEasyBuffer Data(pMsg->Data,pMsg->Header.Size-sizeof(EASY_NET_LINK_MSG_HEAD));
				Data.SetUsedSize(Data.GetBufferSize());


				OnData(Data);
			}
			break;
		default:
			PrintNetLog(0xffffffff,_T("CEasyNetLinkConnection::OnRecvData 收到不明消息ID=%u,size=%u"),
				pMsg->Header.MsgID,MsgSize);
		}
		m_AssembleBuffer.PopFront(NULL,MsgSize);
	}
}

void CEasyNetLinkConnection::OnConnection(BOOL IsSucceed)
{
	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount=0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);
	m_SendBuffer.SetUsedSize(0);

	if(IsSucceed)
	{
		PrintNetLog(0xffffffff,_T("[%s]连接[%s:%u]成功"),
			CClassifiedID(GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());


		EASY_NET_LINK_INFO Info;
		Info.ID=GetReportID();
		SendLinkMsg(EASY_NET_LINK_MSG_LINK_START,&Info,sizeof(Info));


	}
	else
	{
		PrintNetLog(0xffffffff,_T("[%s]连接[%s:%u]失败"),
			CClassifiedID(GetReportID()).ToStr(),
			GetRemoteAddress().GetIPString(),
			GetRemoteAddress().GetPort());
	}
}

void CEasyNetLinkConnection::OnDisconnection()
{
	PrintNetLog(0xffffffff,_T("[%s]连接[%s:%u]断开"),
		CClassifiedID(GetReportID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());

	m_ReconnectTimer.SaveTime();
	m_KeepAliveCount=0;
	m_KeepAliveTimer.SaveTime();
	m_AssembleBuffer.SetUsedSize(0);
	m_SendBuffer.SetUsedSize(0);

	if(GetID())
	{
		if(m_pManager)
			m_pManager->OnLinkEnd(this);
		OnLinkEnd();
	}
}

void CEasyNetLinkConnection::SendLinkMsg(DWORD MsgID,LPCVOID pData,UINT DataSize)
{
	EASY_NET_LINK_MSG_HEAD MsgHeader;
	MsgHeader.Size=sizeof(EASY_NET_LINK_MSG_HEAD)+DataSize;
	MsgHeader.MsgID=MsgID;

	m_SendBuffer.SetUsedSize(0);
	m_SendBuffer.PushBack(&MsgHeader,sizeof(MsgHeader));
	if(DataSize)
	{
		m_SendBuffer.PushBack(pData,DataSize);
	}
	Send(m_SendBuffer.GetBuffer(),m_SendBuffer.GetUsedSize());
}


bool CEasyNetLinkConnection::StealFrom(CNameObject * pObject,UINT Param)
{
	if(pObject->IsKindOf(GET_CLASS_INFO(CEasyNetLinkConnection)))
	{
		if(!CNetConnection::StealFrom(pObject,Param))
			return false;

		CEasyNetLinkConnection * pLink=(CEasyNetLinkConnection *)pObject;
		m_pManager=pLink->m_pManager;
		m_ReportID=pLink->m_ReportID;
		m_IsKeepConnect=pLink->m_IsKeepConnect;
		pLink->m_IsKeepConnect=false;
		m_AssembleBuffer.Create(pLink->m_AssembleBuffer.GetBufferSize());
		m_AssembleBuffer.PushBack(pLink->m_AssembleBuffer.GetBuffer(),
			pLink->m_AssembleBuffer.GetUsedSize());
		m_SendBuffer.Create(pLink->m_SendBuffer.GetBufferSize());
		m_KeepAliveTimer=pLink->m_KeepAliveTimer;
		m_KeepAliveCount=pLink->m_KeepAliveCount;
		m_NeedReallocConnectionID=pLink->m_NeedReallocConnectionID;

		return true;
	}



	return false;
}

void CEasyNetLinkConnection::PrintInfo(UINT LogChannel)
{
	CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
		_T("LinkID=[%s] RemoteAddress=%s:%u %s %s"),
		CClassifiedID(GetID()).ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort(),
		IsConnected()?_T("Connected"):_T("Disconnected"),
		g_szLinkStatus[m_Status]);
}
