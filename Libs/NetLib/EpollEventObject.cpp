/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollEventObject.cpp                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CEpollEventObject::CEpollEventObject(void)
{
	m_ID=0;
	m_DataBuff.SetTag(_T("CEpollEventObject"));
	m_DataBuff.Create(NET_DATA_BLOCK_SIZE);
	Create(NULL);
}

CEpollEventObject::~CEpollEventObject(void)
{
	Destory();
}

BOOL CEpollEventObject::Create(CNetServer * pCreator)
{
	m_pCreator=pCreator;
	m_DataBuff.SetUsedSize(0);
	m_TypeID=IO_NOTSET;
	m_AcceptSocket=INVALID_SOCKET;
	m_Address.SetIP((DWORD)0);
	m_Address.SetPort(0);
	m_AddressLen = sizeof(CIPAddress);
	m_ParentID=0;
	m_ErrorCode=0;
	m_EventID=0;
	return TRUE;
}
void CEpollEventObject::Destory()
{
	m_DataBuff.SetUsedSize(0);
	m_TypeID=IO_NOTSET;
	m_AcceptSocket=INVALID_SOCKET;
	m_Address.SetIP((DWORD)0);
	m_Address.SetPort(0);
	m_AddressLen = sizeof(CIPAddress);
	m_ParentID=0;
	m_ErrorCode=0;
	m_EventID=0;
}

