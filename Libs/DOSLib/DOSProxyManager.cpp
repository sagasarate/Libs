/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSProxyManager.cpp                                     */
/*      创建日期:  2015年1月6日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CDOSProxyManager, CNameObject);
CDOSProxyManager::CDOSProxyManager()
{
	m_pServer = NULL;
	ZeroMemory(m_ProxyServiceMap, sizeof(m_ProxyServiceMap));
	m_ProxyServiceList.SetTag(_T("CDOSProxyManager"));
}


CDOSProxyManager::~CDOSProxyManager()
{
	Destory();
}


bool CDOSProxyManager::Initialize(CDOSServer * pServer)
{
	m_pServer = pServer;
	CEasyArray<CLIENT_PROXY_CONFIG>& ProxyConfigs = m_pServer->GetConfig().ClientProxyConfigs;
	//检查设置有效性
	for (int i = (int)ProxyConfigs.GetCount() - 1; i >= 0; i--)
	{
		if (ProxyConfigs[i].ProxyMode == CLIENT_PROXY_MODE_DEFAULT)
		{
			if (ProxyConfigs[i].ProxyType >= BROAD_CAST_PROXY_TYPE)
			{
				PrintDOSLog( _T("代理类型码不能为%d,侦听地址为(%s:%d)的代理将被忽略"), BROAD_CAST_PROXY_TYPE,
					ProxyConfigs[i].ListenAddress.GetIPString(), ProxyConfigs[i].ListenAddress.GetPort());
				ProxyConfigs.Delete(i);
			}
			else
			{
				bool IsListenAddressExist = false;
				bool IsProxyTypeExist = false;
				for (int j = i - 1; j >= 0; j--)
				{
					if (ProxyConfigs[i].ProxyType == ProxyConfigs[j].ProxyType)
					{
						PrintDOSLog( _T("代理类型码重复,侦听地址为(%s:%d)的代理将被忽略"), ProxyConfigs[i].ListenAddress.GetIPString(), ProxyConfigs[i].ListenAddress.GetPort());
						ProxyConfigs.Delete(i);
						break;
					}
					else if (ProxyConfigs[i].ListenAddress == ProxyConfigs[j].ListenAddress)
					{
						PrintDOSLog( _T("侦听地址重复,侦听地址为(%s:%d)的代理将被忽略"), ProxyConfigs[i].ListenAddress.GetIPString(), ProxyConfigs[i].ListenAddress.GetPort());
						ProxyConfigs.Delete(i);
						break;
					}
				}
			}
		}
		else
		{
			PrintDOSLog( _T("模式为%d的代理将被忽略"), ProxyConfigs[i].ProxyMode);
			ProxyConfigs.Delete(i);
		}
	}
	if (ProxyConfigs.GetCount())
	{
		for (UINT i = 0; i < ProxyConfigs.GetCount(); i++)
		{
			CDOSObjectProxyServiceDefault * pProxyServiceList = MONITORED_NEW(_T("CDOSProxyManager"), CDOSObjectProxyServiceDefault);
			pProxyServiceList->SetID(i + 1);
			if (pProxyServiceList->Init(m_pServer, ProxyConfigs[i]))
			{
				if (pProxyServiceList->StartService())
				{
					m_ProxyServiceList.Add(pProxyServiceList);
					m_ProxyServiceMap[pProxyServiceList->GetProxyType()] = pProxyServiceList;
				}
				else
				{
					PrintDOSLog( _T("代理%d无法启动"), pProxyServiceList->GetProxyType());
					SAFE_DELETE(pProxyServiceList);
				}
				
			}
			else
			{
				SAFE_DELETE(pProxyServiceList);
			}
		}
	}
	return true;
}
void CDOSProxyManager::Destory()
{
	for (UINT i = 0; i < m_ProxyServiceList.GetCount(); i++)
	{
		if (m_ProxyServiceList[i])
			m_ProxyServiceList[i]->StopService();
		SAFE_DELETE(m_ProxyServiceList[i]);
	}
	m_ProxyServiceList.Clear();
}

UINT CDOSProxyManager::GetConnectionCount()
{
	UINT Count = 0;
	for (UINT i = 0; i < m_ProxyServiceList.GetCount(); i++)
	{
		if (m_ProxyServiceList[i])
			Count+=m_ProxyServiceList[i]->GetConnectionCount();
	}
	return Count;
}

bool CDOSProxyManager::RegisterProxyService(CBaseDOSObjectProxyService * pService)
{
	pService->SetID((UINT)m_ProxyServiceList.GetCount() + 1);
	if (pService->StartService())
	{
		m_ProxyServiceList.Add(pService);
		PrintDOSLog( _T("类型为%d的代理已被注册"), pService->GetProxyType());
		return true;
	}
	else
	{
		PrintDOSLog( _T("类型为%d的代理无法启动"), pService->GetProxyType());
		return false;
	}	
}

bool CDOSProxyManager::UnregisterProxyService(BYTE ProxyID)
{
	ProxyID--;
	if (ProxyID < m_ProxyServiceList.GetCount())
	{
		SAFE_RELEASE(m_ProxyServiceList[ProxyID]);
		m_ProxyServiceList.Delete(ProxyID);
		return true;
	}
	return false;
}

bool CDOSProxyManager::PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket)
{
	BYTE ProxyType = GET_PROXY_TYPE_FROM_PROXY_GROUP_INDEX(ObjectID.GroupIndex);
	if (ProxyType == BROAD_CAST_PROXY_TYPE)
	{
		//群发到各Serivce
		for (UINT j = 0; j < m_ProxyServiceList.GetCount(); j++)
		{
			m_ProxyServiceList[j]->PushMessage(ObjectID, pPacket);
		}
		return true;
	}
	else
	{
		CBaseDOSObjectProxyService * pProxyService = m_ProxyServiceMap[ProxyType];
		if (pProxyService)
		{
			return pProxyService->PushMessage(ObjectID, pPacket);
		}
		else
		{
			PrintDOSDebugLog(_T("将[0x%llX]发出的消息[%X]递送到代理服务[%llX]时代理服务不存在"),
				pPacket->GetMessage().GetSenderID(),
				pPacket->GetMessage().GetMsgID(),
				ObjectID);
			return false;
		}
	}
}