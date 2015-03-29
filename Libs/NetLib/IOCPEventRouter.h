/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPEventRouter.h                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class COverLappedObject;
class CIOCPEventRouter:
	public CNameObject
{
protected:
	IIOCPEventHandler *			m_pIOCPEventHandler;
	UINT						m_SessionID;
	CEasyReadWriteLock			m_EasyReadWriteLock;

	DECLARE_CLASS_INFO_STATIC(CIOCPEventRouter);
public:
	CIOCPEventRouter()
	{
		m_pIOCPEventHandler=NULL;
		m_SessionID=0;
	}
	~CIOCPEventRouter()
	{
		Destory();
	}
	virtual void Destory()
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pIOCPEventHandler=NULL;
	}
	void Init(IIOCPEventHandler * pIOCPEventHander)
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pIOCPEventHandler=pIOCPEventHander;
		m_SessionID++;
	}
	void SetEventHander(IIOCPEventHandler * pIOCPEventHander)
	{
		CAutoWriteLock Lock(m_EasyReadWriteLock);
		m_pIOCPEventHandler=pIOCPEventHander;		
	}
	UINT GetSessionID()
	{
		return m_SessionID;
	}
	BOOL OnIOCPEvent(int EventID,COverLappedObject * pOverLappedObject);
};