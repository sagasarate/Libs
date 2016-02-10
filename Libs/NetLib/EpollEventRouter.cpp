/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollEventRouter.cpp                                     */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CEpollEventRouter,CNameObject);

BOOL CEpollEventRouter::OnEpollEvent(UINT EventID,UINT SessionID)
{
	CAutoReadLock Lock(m_EasyReadWriteLock);
	if(m_SessionID==SessionID)
	{
		if(m_pEpollEventHandler!=NULL)
		{

			return m_pEpollEventHandler->OnEpollEvent(EventID);

		}
		else
		{
			PrintNetLog(0xffffffff,"EventRouter没有EventHandler,Event(%u)被忽略",EventID);
		}
	}
	else
	{
		PrintNetLog(0xffffffff,"不是本次Session的Event,Event(%u)被忽略",EventID);
	}

	return FALSE;
}
