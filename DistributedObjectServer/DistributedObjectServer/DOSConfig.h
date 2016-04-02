#pragma once



class CDOSConfig :
	public CStaticObject<CDOSConfig>
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
	CEasyString					m_ServiceName;
	CEasyString					m_ServiceDesc;
	NET_CONFIG					m_NetConfig;
	DOS_CONFIG					m_DOSConfig;
	MONO_CONFIG					m_MonoConfig;
	UINT						m_MaxPluginObject;
	CEasyArray<PLUGIN_INFO>		m_PluginList;
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
	UINT GetMaxPluginObject()
	{
		return m_MaxPluginObject;
	}
	const CEasyArray<PLUGIN_INFO>& GetPluginList()
	{
		return m_PluginList;
	}
	
protected:
	bool ReadPoolConfig(xml_node& XMLContent,POOL_CONFIG& Config);
	bool ReadProxyConfig(xml_node& XMLContent, CLIENT_PROXY_CONFIG& Config);
	bool LoadPluginInfo(xml_node& XMLContent);
};
