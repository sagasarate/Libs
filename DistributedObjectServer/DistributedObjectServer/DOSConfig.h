#pragma once



class CDOSConfig :
	public CStaticObject4<CDOSConfig>
{
public:
	

	struct NET_CONFIG
	{
		UINT					NetWorkThreadCount;
		STORAGE_POOL_SETTING	EventObjectPool;
		STORAGE_POOL_SETTING	EventRouterPool;
		NET_CONFIG()
		{
			NetWorkThreadCount = 2;
		}
	};

	struct MONO_CONFIG
	{
		bool					EnableMono;
		CEasyString				LibraryDir;
		CEasyString				RunDir;
		CEasyString				CompilerPath;
		bool					CreateProj;
		CEasyString				ConfigFilePath;
		bool					EnableDebug;
		CIPAddress				DebugAgentListenAddress;
		bool					IsDebugSuspend;
		UINT					BaseGCInterval;
		UINT					AdvanceGCMul;
		UINT					FullGCInterval;

		MONO_CONFIG()
		{
			EnableMono = false;
			CreateProj = false;
			EnableDebug = false;
			DebugAgentListenAddress.SetPort(56600);
			IsDebugSuspend = false;
			BaseGCInterval = 1000;
			AdvanceGCMul = 5;
			FullGCInterval = 5 * 60 * 1000;
		}
	};

	
protected:
	CEasyString								m_ServiceName;
	CEasyString								m_ServiceDesc;
	NET_CONFIG								m_NetConfig;
	DOS_CONFIG								m_DOSConfig;
	MONO_CONFIG								m_MonoConfig;
	STORAGE_POOL_SETTING					m_PluginObjectPoolConfig;
	CEasyArray<PLUGIN_INFO>					m_PluginList;
	CEasyArray<LIB_INFO>					m_LibList;
	CEasyArray<CLIENT_PROXY_PLUGIN_INFO>	m_ProxyPluginList;
	bool									m_ExistWhenNoPlugin;
public:
	CDOSConfig(void);
	~CDOSConfig(void);

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

	const NET_CONFIG& GetNetConfig()
	{
		return m_NetConfig;
	}

	const DOS_CONFIG& GetDOSConfig()
	{
		return m_DOSConfig;
	}	
	const MONO_CONFIG& GetMonoConfig()
	{
		return m_MonoConfig;
	}
	const STORAGE_POOL_SETTING& GetPluginObjectPoolConfig()
	{
		return m_PluginObjectPoolConfig;
	}
	const CEasyArray<PLUGIN_INFO>& GetPluginList()
	{
		return m_PluginList;
	}
	const CEasyArray<LIB_INFO>& GetLibList()
	{
		return m_LibList;
	}
	const CEasyArray<CLIENT_PROXY_PLUGIN_INFO>& GetProxyPluginList()
	{
		return m_ProxyPluginList;
	}
	bool ExistWhenNoPlugin()
	{
		return m_ExistWhenNoPlugin;
	}
protected:
	bool ReadPoolConfig(xml_node& XMLContent, STORAGE_POOL_SETTING& Config);
	bool ReadProxyConfig(xml_node& XMLContent, CLIENT_PROXY_PLUGIN_INFO& Config);
	bool LoadLibInfo(xml_node& XMLContent);
	bool LoadPluginInfo(xml_node& XMLContent);
};
