/****************************************************************************/
/*                                                                          */
/*      文件名:    IEpollEventHandler.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

enum EPOLL_EVENT_ID
{
	EPE_PACKET,
	EPE_ERROR,
};
class CEpollEventObject;

class IEpollEventHandler
{
public:	
	virtual bool OnEpollEvent(UINT EventID)=0;
};