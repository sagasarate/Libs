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
	CDOSServer *							m_pServer;
	CEasyArray<CDOSObjectProxyService *>	m_ProxyServiceList;

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
		return m_ProxyServiceList.GetCount();
	}
	
	CDOSObjectProxyService * GetProxyService(OBJECT_ID ObjectID);
	CDOSObjectProxyService * GetProxyServiceByID(BYTE ProxyID);
	CDOSObjectProxyService * GetProxyServiceByIndex(UINT Index);
	CDOSObjectProxyService * GetProxyServiceByType(BYTE ProxyType);
	CDOSProxyConnection * GetProxyConnect(OBJECT_ID ObjectID);
};


inline CDOSObjectProxyService * CDOSProxyManager::GetProxyService(OBJECT_ID ObjectID)
{
	UINT ProxyID = GET_PROXY_ID_FROM_PROXY_GROUP_INDEX(ObjectID.GroupIndex) - 1;
	if (ProxyID < m_ProxyServiceList.GetCount())
		return m_ProxyServiceList[ProxyID];
	return NULL;
}
inline CDOSObjectProxyService * CDOSProxyManager::GetProxyServiceByID(BYTE ProxyID)
{
	ProxyID--;
	if (ProxyID < m_ProxyServiceList.GetCount())
		return m_ProxyServiceList[ProxyID];
	return NULL;
}
inline CDOSObjectProxyService * CDOSProxyManager::GetProxyServiceByIndex(UINT Index)
{
	if (Index < m_ProxyServiceList.GetCount())
		return m_ProxyServiceList[Index];
	return NULL;
}
inline CDOSObjectProxyService * CDOSProxyManager::GetProxyServiceByType(BYTE ProxyType)
{
	for (UINT i = 0; i < m_ProxyServiceList.GetCount(); i++)
	{
		if (m_ProxyServiceList[i]->GetProxyType() == ProxyType)
			return m_ProxyServiceList[i];
	}
	return NULL;
}
inline CDOSProxyConnection * CDOSProxyManager::GetProxyConnect(OBJECT_ID ObjectID)
{
	UINT ProxyID = GET_PROXY_ID_FROM_PROXY_GROUP_INDEX(ObjectID.GroupIndex) - 1;
	if (ProxyID < m_ProxyServiceList.GetCount())
		return m_ProxyServiceList[ProxyID]->GetConnection(ObjectID.ObjectIndex);
	return NULL;
}


