/****************************************************************************/
/*                                                                          */
/*      文件名:    OverLappedObject.cpp                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


COverLappedObject::COverLappedObject(void)		
{
	m_ID=0;	
	m_DataBuff.Create(MAX_DATA_PACKET_SIZE);
	Create(NULL);
}

COverLappedObject::~COverLappedObject(void)
{	
	Destory();
}

BOOL COverLappedObject::Create(CNetServer * pCreator)
{
	m_pCreator=pCreator;	
	ZeroMemory(&m_OverLapped,sizeof(OVERLAPPED_EX));
	m_OverLapped.pOverLappedObject=this;
	m_DataBuff.SetUsedSize(0);
	m_TypeID=IO_NOTSET;
	m_AcceptSocket=INVALID_SOCKET;
	m_pIOCPEventRouter=NULL;
	m_RouterSession=0;
	m_Address.SetIP((ULONG)0);
	m_Address.SetPort(0);	
	m_AddressLen=sizeof(sockaddr_in);
	m_ParentID=0;	
	m_ErrorCode=0;
	m_EventID=0;
	return TRUE;
}
void COverLappedObject::Destory()
{		
	m_DataBuff.SetUsedSize(0);
	m_TypeID=IO_NOTSET;
	m_AcceptSocket=INVALID_SOCKET;
	m_pIOCPEventRouter=NULL;
	m_Address.SetIP((ULONG)0);
	m_Address.SetPort(0);	
	m_AddressLen=sizeof(sockaddr_in);
	m_RouterSession=0;
	m_ParentID=0;	
	m_ErrorCode=0;
	m_EventID=0;
}

void COverLappedObject::Release()
{
	m_pCreator->DeleteOverLappedObject(this);
}
