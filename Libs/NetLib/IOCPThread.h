/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPThread.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CIOCPThread :
	public CEasyThread
{
protected:
	HANDLE			m_hIOCP;	
	
	DECLARE_CLASS_INFO_STATIC(CIOCPThread)
public:
	CIOCPThread(void);
	virtual ~CIOCPThread(void);

	void SetIOCPHandle(HANDLE hIOCP);
protected:	
	virtual BOOL OnStart();
	virtual BOOL OnRun();
	virtual void OnTerminate();
};

inline void CIOCPThread::SetIOCPHandle(HANDLE hIOCP)
{
	m_hIOCP=hIOCP;
}