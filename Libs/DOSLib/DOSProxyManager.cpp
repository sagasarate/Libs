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
			if (ProxyConfigs[i].ProxyType == BROAD_CAST_PROXY_TYPE)
			{
				PrintDOSLog(0, _T("代理类型码不能为%d,侦听地址为(%s:%d)的代理将被忽略"), BROAD_CAST_PROXY_TYPE,
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
						PrintDOSLog(0, _T("代理类型码重复,侦听地址为(%s:%d)的代理将被忽略"), ProxyConfigs[i].ListenAddress.GetIPString(), ProxyConfigs[i].ListenAddress.GetPort());
						ProxyConfigs.Delete(i);
						break;
					}
					else if (ProxyConfigs[i].ListenAddress == ProxyConfigs[j].ListenAddress)
					{
						PrintDOSLog(0, _T("侦听地址重复,侦听地址为(%s:%d)的代理将被忽略"), ProxyConfigs[i].ListenAddress.GetIPString(), ProxyConfigs[i].ListenAddress.GetPort());
						ProxyConfigs.Delete(i);
						break;
					}
				}
			}
		}
		else
		{
			PrintDOSLog(0, _T("模式为%d的代理将被忽略"), ProxyConfigs[i].ProxyMode);
			ProxyConfigs.Delete(i);
		}
	}
	if (ProxyConfigs.GetCount())
	{
		for (UINT i = 0; i < ProxyConfigs.GetCount(); i++)
		{
			CDOSObjectProxyServiceDefault * pProxyServiceList = new CDOSObjectProxyServiceDefault();
			pProxyServiceList->SetID(i + 1);
			if (pProxyServiceList->Init(m_pServer, ProxyConfigs[i]))
			{
				if (pProxyServiceList->StartService())
				{
					m_ProxyServiceList.Add(pProxyServiceList);
				}
				else
				{
					PrintDOSLog(0, _T("代理%d无法启动"), pProxyServiceList->GetProxyType());
					SAFE_RELEASE(pProxyServiceList);
				}
				
			}
			else
			{
				SAFE_RELEASE(pProxyServiceList);
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
		SAFE_RELEASE(m_ProxyServiceList[i]);
	}
	m_ProxyServiceList.Clear();
}

bool CDOSProxyManager::RegisterProxyService(IDOSObjectProxyServiceBase * pService)
{
	pService->SetID((UINT)m_ProxyServiceList.GetCount() + 1);
	if (pService->StartService())
	{
		pService->AddUseRef();
		m_ProxyServiceList.Add(pService);
		PrintDOSLog(0, _T("类型为%d的代理已被注册"), pService->GetProxyType());
		return true;
	}
	else
	{
		PrintDOSLog(0, _T("类型为%d的代理无法启动"), pService->GetProxyType());
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