/****************************************************************************/
/*                                                                          */
/*      文件名:    NetPUDPServiceIOCP.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CNetServer;

class CNetPUDPService :	
	public CBaseNetService,public IIOCPEventHandler
{
protected:
	volatile BOOL							m_WantClose;
	CNetServer *							m_pServer;
	int										m_ParallelRecvCount;	
	CIOCPEventRouter *						m_pIOCPEventRouter;
	

	DECLARE_CLASS_INFO_STATIC(CNetPUDPService);
public:
	CNetPUDPService();
	virtual ~CNetPUDPService(void);
	

	void SetServer(CNetServer * pServer);

	CNetServer * GetServer();

	virtual BOOL OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);
	

	virtual BOOL Create(int ParallelRecvCount=DEFAULT_PARALLEL_RECV);
	virtual void Destory();
	
	BOOL StartListen(const CIPAddress& Address);
	void Close();	

	virtual void OnStartUp();
	virtual void OnClose();

	BOOL QueryUDPSend(const CIPAddress& IPAddress,LPCVOID pData,int Size);

	
protected:	
	BOOL QueryUDPRecv();
	
};


inline CNetServer * CNetPUDPService::GetServer()
{
	return m_pServer;
}

