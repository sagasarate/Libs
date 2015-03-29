#pragma once



class CMainConfig :
	public CStaticObject<CMainConfig>
{
public:
	struct POOL_CONFIG
	{
		UINT	StartSize;
		UINT	GrowSize;
		UINT	GrowLimit;
		POOL_CONFIG()
		{
			StartSize=16;
			GrowSize=1;
			GrowLimit=0;
		}
	};

	struct NET_CONFIG
	{
		UINT			NetWorkThreadPerCPU;
		POOL_CONFIG		EventObjectPool;
		POOL_CONFIG		EventRouterPool;
		NET_CONFIG()
		{
			NetWorkThreadPerCPU=2;
		}
	};
protected:
	CEasyString				m_ServiceName;
	CEasyString				m_ServiceDesc;
	NET_CONFIG				m_NetConfig;
	DOS_CONFIG				m_DOSConfig;
	UINT					m_MaxPluginObject;
	CEasyArray<CEasyString>	m_PluginFileNameList;
public:
	CMainConfig(void);
	~CMainConfig(void);

	bool LoadConfig(LPCTSTR FileName);

	LPCTSTR GetServiceName()
	{
		return m_ServiceName;
	}
	LPCTSTR GetServiceDesc()
	{
		return m_ServiceDesc;
	}
	bool HaveServiceName()
	{
		return !(m_ServiceName.IsEmpty()||m_ServiceDesc.IsEmpty());
	}

	NET_CONFIG& GetNetConfig()
	{
		return m_NetConfig;
	}

	DOS_CONFIG& GetDOSConfig()
	{
		return m_DOSConfig;
	}	

	UINT GetPluginCount()
	{
		return (UINT)m_PluginFileNameList.GetCount();
	}
	UINT GetMaxPluginObject()
	{
		return m_MaxPluginObject;
	}
	LPCTSTR GetPluginFileName(UINT Index)
	{
		if(Index<m_PluginFileNameList.GetCount())
		{
			return m_PluginFileNameList[Index];
		}
		return NULL;
	}
protected:
	bool ReadPoolConfig(xml_node& XMLContent,POOL_CONFIG& Config);
	bool ReadProxyConfig(xml_node& XMLContent, CLIENT_PROXY_CONFIG& Config);
};
