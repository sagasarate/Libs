/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPListenThread.h                                       */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CNetService;

class CIOCPListenThread :
	public CEasyThread
{
protected:
	SOCKET			m_ListenSocket;
	CNetService*	m_pServer;
public:
	CIOCPListenThread(void);
	virtual ~CIOCPListenThread(void);

	bool Init(CNetService * pService,SOCKET ListenSocket);
protected:	
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();

};


