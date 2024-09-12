/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollEventRouter.h                                       */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CEpollEventObject;
class CEpollThread;
class CEpollEventRouter:
	public CNameObject
{
protected:
	IEpollEventHandler *				m_pEpollEventHandler;
	CEpollThread *						m_pEpollThread;
	CEasyReadWriteLock					m_EasyReadWriteLock;
	volatile UINT						m_SessionID;

	DECLARE_CLASS_INFO_STATIC(CEpollEventRouter);
public:
	CEpollEventRouter()
	{
		m_pEpollEventHandler = NULL;
		m_pEpollThread = NULL;
		m_SessionID=0;
	}
	~CEpollEventRouter()
	{
		Destory();
	}
	virtual void Destory()
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pEpollEventHandler=NULL;
		m_SessionID=0;
	}
	void Init(IEpollEventHandler * pEpollEventHander)
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pEpollEventHandler=pEpollEventHander;
		m_pEpollThread = NULL;
		AtomicInc(&m_SessionID);
	}
	void SetEventHander(IEpollEventHandler * pEpollEventHander)
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pEpollEventHandler=pEpollEventHander;
	}
	void SetEpollThread(CEpollThread * pEpollThread)
	{
		m_pEpollThread = pEpollThread;
	}
	CEpollThread * GetEpollThread()
	{
		return m_pEpollThread;
	}
	UINT GetSessionID()
	{
		return m_SessionID;
	}
	bool OnEpollEvent(UINT EventID, UINT SessionID);
};
