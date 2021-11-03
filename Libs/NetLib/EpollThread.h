/****************************************************************************/
/*                                                                          */
/*      文件名:    EpollThread.h                                            */
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
class CEpollThread :
	public CEasyThread
{
protected:
	CNetServer *	m_pServer;
	int				m_hEpoll;	
	volatile UINT	m_BindSocketCount;
	
	DECLARE_CLASS_INFO_STATIC(CEpollThread)
public:
	CEpollThread();
	CEpollThread(CNetServer * pServer);
	virtual ~CEpollThread(void);

	bool Init(CNetServer * pServer);

	bool BindSocket(SOCKET Socket, CEpollEventRouter * pEpollEventRouter, bool UseLTMode);
	//bool ModifySendEvent(SOCKET Socket, CEpollEventRouter * pEpollEventRouter, bool IsSet);
	bool UnbindSocket(SOCKET Socket);

	UINT GetBindCount()
	{
		return m_BindSocketCount;
	}

protected:	
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();
};

