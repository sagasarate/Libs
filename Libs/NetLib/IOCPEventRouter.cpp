/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPEventRouter.cpp                                      */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CIOCPEventRouter,CNameObject);

BOOL CIOCPEventRouter::OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject)
{
	CAutoReadLock Lock(m_EasyReadWriteLock);

	if(pOverLappedObject->GetRouterSession()==m_SessionID)
	{
		if(m_pIOCPEventHandler!=NULL)
		{
			
			return m_pIOCPEventHandler->OnIOCPEvent(EventID,pOverLappedObject);			
			
		}
		else
		{
			PrintNetLog(0xffffffff,_T("EventRouter没有EventHandler,Overlapped(%u)被忽略"),
				pOverLappedObject->GetParentID());
		}
	}
	else
	{
		PrintNetLog(0xffffffff,_T("EventRouter收到不是本Session的Overlapped(%u)，忽略"),
			pOverLappedObject->GetParentID());
	}
	pOverLappedObject->Release();
	return FALSE;
}