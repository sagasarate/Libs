﻿/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPListenThread.cpp                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

CIOCPListenThread::CIOCPListenThread(void)
{
	m_pServer=NULL;
	m_ListenSocket=INVALID_SOCKET;
}

CIOCPListenThread::~CIOCPListenThread(void)
{
}

bool CIOCPListenThread::Init(CNetService * pService,SOCKET ListenSocket)
{
	m_pServer=pService;
	m_ListenSocket=ListenSocket;
	return true;
}

bool CIOCPListenThread::OnStart()
{
	PrintNetLog(_T("ListenThread启动"));
	return true;
}

bool CIOCPListenThread::OnRun()
{
	CIPAddress Address;
	int AddressLen = sizeof(CIPAddress);
	SOCKET AcceptSocket = WSAAccept(m_ListenSocket, Address.GetSockAddrPtr(), &AddressLen, NULL, 0);
	if(AcceptSocket==INVALID_SOCKET)
	{
		int ErrorCode=GetLastError();
		PrintNetLog(_T("Accept失败(%u)"),ErrorCode);
		if(ErrorCode!=WSAECONNRESET&&ErrorCode!=WSAEINTR&&
			ErrorCode!=WSAEINPROGRESS&&ErrorCode!=WSAEMFILE&&
			ErrorCode!=WSAENOBUFS&&ErrorCode!=WSAEWOULDBLOCK&&
			ErrorCode!=WSAECONNREFUSED&&ErrorCode!=WSAEACCES&&
			ErrorCode!=WSATRY_AGAIN)
		{
			COverLappedObject * pOverLappedObject = m_pServer->AllocOverLappedObject(IO_ACCEPT2);
			if(pOverLappedObject)
			{
				m_pServer->OnIOCPEvent(IOE_ERROR,pOverLappedObject);
				return false;
			}
			else
			{
				PrintNetLog(_T("CIOCPListenThread创建Accept用OverLappedObject失败！"));			
			}
		}
	}
	else
	{
		//if(setsockopt(AcceptSocket,
		//	SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
		//	(char *)&m_ListenSocket, sizeof(m_ListenSocket))!=SOCKET_ERROR)
		//{
		COverLappedObject * pOverLappedObject = m_pServer->AllocOverLappedObject(IO_ACCEPT2);
			if(pOverLappedObject)
			{
				pOverLappedObject->SetAcceptSocket(AcceptSocket);
				m_pServer->OnIOCPEvent(IOE_PACKET,pOverLappedObject);
				return true;
			}
			else
			{
				PrintNetLog(_T("CIOCPListenThread创建Accept用OverLappedObject失败！"));			
			}
		//}				
		//else
		//	PrintNetLog(_T("CIOCPListenThread更新AcceptScoket状态失败！"),GetID());

		closesocket(AcceptSocket);
	}
	return true;
}

void CIOCPListenThread::OnTerminate()
{
	PrintNetLog(_T("ListenThread关闭"));
}