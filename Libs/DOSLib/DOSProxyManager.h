/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSProxyManager.h                                       */
/*      创建日期:  2015年1月6日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#pragma once

class CDOSProxyManager :
	public CNameObject
{
protected:
	CDOSServer *								m_pServer;
	CEasyArray<CBaseDOSObjectProxyService *>	m_ProxyServiceList;
	CBaseDOSObjectProxyService *				m_ProxyServiceMap[256];

	DECLARE_CLASS_INFO_STATIC(CDOSProxyManager);
public:
	CDOSProxyManager();
	~CDOSProxyManager();

	virtual bool Initialize(CDOSServer * pServer);
	virtual void Destory();

	CDOSServer * GetServer()
	{
		return m_pServer;
	}
	UINT GetProxyServiceCount()
	{
		return (UINT)m_ProxyServiceList.GetCount();
	}
	UINT GetConnectionCount();

	bool RegisterProxyService(CBaseDOSObjectProxyService * pService);
	bool UnregisterProxyService(BYTE ProxyID);

	bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket);
	
	CBaseDOSObjectProxyService * GetProxyService(OBJECT_ID ObjectID);
	CBaseDOSObjectProxyService * GetProxyServiceByType(BYTE ProxyType);
	CBaseDOSObjectProxyService * GetProxyServiceByIndex(UINT Index);
};



inline CBaseDOSObjectProxyService * CDOSProxyManager::GetProxyService(OBJECT_ID ObjectID)
{
	UINT ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(ObjectID.GroupIndex);
	return GetProxyServiceByType(ProxyType);
}
inline CBaseDOSObjectProxyService * CDOSProxyManager::GetProxyServiceByType(BYTE ProxyType)
{
	return m_ProxyServiceMap[ProxyType];
}
inline CBaseDOSObjectProxyService * CDOSProxyManager::GetProxyServiceByIndex(UINT Index)
{
	if (Index < m_ProxyServiceList.GetCount())
		return m_ProxyServiceList[Index];
	return NULL;
}



