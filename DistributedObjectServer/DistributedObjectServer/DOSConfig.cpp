#include "stdafx.h"

CDOSConfig::CDOSConfig(void)
{
	FUNCTION_BEGIN;
	m_ExistWhenNoPlugin = false;
	FUNCTION_END;
}

CDOSConfig::~CDOSConfig(void)
{
}

bool CDOSConfig::LoadConfig(LPCTSTR FileName)
{
	FUNCTION_BEGIN;
	xml_parser Parser;

	if(Parser.parse_file(FileName,pug::parse_trim_attribute))
	{
		xml_node Config=Parser.document();
		if(Config.moveto_child("Main"))
		{
			if(Config.has_attribute("ServiceName"))
				m_ServiceName=Config.attribute("ServiceName").getvalue();
			if(Config.has_attribute("ServiceDesc"))
				m_ServiceDesc=Config.attribute("ServiceDesc").getvalue();

			xml_node Net=Config;
			if(Net.moveto_child("Net"))
			{
				if(Net.has_attribute("NetWorkThreadCount"))
					m_NetConfig.NetWorkThreadCount = Net.attribute("NetWorkThreadCount");
				xml_node EventObjectPool=Net;
				if(EventObjectPool.moveto_child("EventObjectPool"))
				{
					ReadPoolConfig(EventObjectPool,m_NetConfig.EventObjectPool);
				}
				xml_node EventRouterPool=Net;
				if(EventRouterPool.moveto_child("EventRouterPool"))
				{
					ReadPoolConfig(EventRouterPool,m_NetConfig.EventRouterPool);
				}
			}

			xml_node Router=Config;
			if(Router.moveto_child("Router"))
			{
				if(Router.has_attribute("RouterID"))
					m_DOSConfig.RouterID=(UINT)Router.attribute("RouterID");
				if(Router.has_attribute("MsgQueueSize"))
					m_DOSConfig.MaxRouterSendMsgQueue=(UINT)Router.attribute("MsgQueueSize");
				if(Router.has_attribute("MsgProcessLimit"))
					m_DOSConfig.RouterMsgProcessLimit=(UINT)Router.attribute("MsgProcessLimit");

			}

			xml_node RouterLink = Config;
			if (RouterLink.moveto_child("RouterLink"))
			{
				CEasyNetLinkManager::LoadConfig(RouterLink, m_DOSConfig.RouterLinkConfig);
			}

			xml_node ClientProxys=Config;
			if(ClientProxys.moveto_child("ClientProxys"))
			{
				for (UINT i = 0; i < ClientProxys.children(); i++)
				{
					xml_node ClientProxy = ClientProxys.child(i);
					if (_stricmp(ClientProxys.name(), "ClientProxy"))
					{
						CLIENT_PROXY_PLUGIN_INFO ProxyConfig;
						if (ReadProxyConfig(ClientProxy, ProxyConfig))
						{
							if (ProxyConfig.ProxyMode == CLIENT_PROXY_MODE_DEFAULT)
								m_DOSConfig.ClientProxyConfigs.Add(ProxyConfig);
							else
								m_ProxyPluginList.Add(ProxyConfig);
						}
					}
				}
			}

			xml_node MemoryPool=Config;
			if(MemoryPool.moveto_child("MemoryPool"))
			{
				if(MemoryPool.has_attribute("BlockSize"))
					m_DOSConfig.MemoryPoolBlockSize=(UINT)MemoryPool.attribute("BlockSize");
				if(MemoryPool.has_attribute("LevelSize"))
					m_DOSConfig.MemoryPoolLeveSize=(UINT)MemoryPool.attribute("LevelSize");
				if(MemoryPool.has_attribute("LevelCount"))
					m_DOSConfig.MemoryPoolLevelCount=(UINT)MemoryPool.attribute("LevelCount");
			}
			xml_node Object=Config;
			if(Object.moveto_child("Object"))
			{
				if(Object.has_attribute("GroupCount"))
					m_DOSConfig.ObjectGroupCount=(UINT)Object.attribute("GroupCount");
				if(Object.has_attribute("ObjectPoolStartSize"))
					m_DOSConfig.ObjectPoolSetting.StartSize=(UINT)Object.attribute("ObjectPoolStartSize");
				if (Object.has_attribute("ObjectPoolGrowSize"))
					m_DOSConfig.ObjectPoolSetting.GrowSize = (UINT)Object.attribute("ObjectPoolGrowSize");
				if (Object.has_attribute("ObjectPoolGrowLimit"))
					m_DOSConfig.ObjectPoolSetting.GrowLimit = (UINT)Object.attribute("ObjectPoolGrowLimit");
				if(Object.has_attribute("MsgQueueSize"))
					m_DOSConfig.MaxObjectMsgQueue=(UINT)Object.attribute("MsgQueueSize");
				if(Object.has_attribute("AliveTestTime"))
					m_DOSConfig.ObjectAliveTestTime=(UINT)Object.attribute("AliveTestTime");
				if(Object.has_attribute("AliveCheckTime"))
					m_DOSConfig.ObjectAliveCheckTime=(UINT)Object.attribute("AliveCheckTime");
				if(Object.has_attribute("KeepAliveCount"))
					m_DOSConfig.ObjectKeepAliveCount=(UINT)Object.attribute("KeepAliveCount");
				if(Object.has_attribute("StatObjectCPUCost"))
					m_DOSConfig.StatObjectCPUCost=Object.attribute("StatObjectCPUCost");
				if (Object.has_attribute("UseRealGroupLoadWeight"))
					m_DOSConfig.UseRealGroupLoadWeight = Object.attribute("UseRealGroupLoadWeight");

				m_DOSConfig.pDOSGroupInitFN = CDOSMainThread::DosGroupInitFn;
				m_DOSConfig.pDOSGroupDestoryFN = CDOSMainThread::DosGroupDestoryFn;
			}

			xml_node Mono = Config;
			if (Mono.moveto_child("Mono"))
			{
				if (Mono.has_attribute("Enable"))
				{
					m_MonoConfig.EnableMono = Mono.attribute("Enable");
//#ifdef _WIN64
//					Log("win64不支持mono");
//					m_MonoConfig.EnableMono = false;
//#endif
				}
				if (Mono.has_attribute("LibraryDir"))
				{
					m_MonoConfig.LibraryDir = Mono.attribute("LibraryDir").getvalue();
				}
				if (Mono.has_attribute("RunDir"))
				{
					m_MonoConfig.RunDir = Mono.attribute("RunDir").getvalue();
				}
				if (Mono.has_attribute("CompilerPath"))
				{
					m_MonoConfig.CompilerPath = Mono.attribute("CompilerPath").getvalue();
				}
				if (Mono.has_attribute("CreateProj"))
				{
					m_MonoConfig.CreateProj = Mono.attribute("CreateProj");
				}
				if (Mono.has_attribute("ConfigFilePath"))
				{
					m_MonoConfig.ConfigFilePath = Mono.attribute("ConfigFilePath").getvalue();
				}
				if (Mono.has_attribute("EnableDebug"))
				{
					m_MonoConfig.EnableDebug = Mono.attribute("EnableDebug");
				}
				if (Mono.has_attribute("DebugAgentListenPort"))
				{
					m_MonoConfig.DebugAgentListenAddress.SetPort(Mono.attribute("DebugAgentListenPort"));
				}
				if (Mono.has_attribute("DebugAgentListenIP"))
				{
					m_MonoConfig.DebugAgentListenAddress.SetIP(Mono.attribute("DebugAgentListenIP").getvalue());
				}
				if (Mono.has_attribute("IsDebugSuspend"))
				{
					m_MonoConfig.IsDebugSuspend = Mono.attribute("IsDebugSuspend");
				}
				if (Mono.has_attribute("BaseGCInterval"))
				{
					UINT Value = Mono.attribute("BaseGCInterval");
					if (Value > 0)
						m_MonoConfig.BaseGCInterval = Value;
				}
				if (Mono.has_attribute("AdvanceGCMul"))
				{
					UINT Value = Mono.attribute("AdvanceGCMul");
					if (Value > 1)
						m_MonoConfig.AdvanceGCMul = Value;
				}
				if (Mono.has_attribute("FullGCInterval"))
				{
					UINT Value = Mono.attribute("FullGCInterval");
					if (Value > 0)
						m_MonoConfig.FullGCInterval = Value;
				}
			}
			xml_node Libs = Config;
			if (Libs.moveto_child("Libs"))
			{
				LoadLibInfo(Libs);
			}

			xml_node Plugins=Config;
			if(Plugins.moveto_child("Plugins"))
			{
				LoadPluginInfo(Plugins);
			}
		}
		else
		{
			PrintImportantLog("配置文件[%s]不合法",FileName);
			return false;
		}

	}
	else
	{
		PrintImportantLog("无法打开配置文件[%s]",FileName);
		return false;
	}
	return true;
	FUNCTION_END;
	return false;
}


bool CDOSConfig::ReadPoolConfig(xml_node& XMLContent, STORAGE_POOL_SETTING& Config)
{
	FUNCTION_BEGIN;
	if(XMLContent.has_attribute("StartSize"))
		Config.StartSize=XMLContent.attribute("StartSize");
	if(XMLContent.has_attribute("GrowSize"))
		Config.GrowSize=XMLContent.attribute("GrowSize");
	if(XMLContent.has_attribute("GrowLimit"))
		Config.GrowLimit=XMLContent.attribute("GrowLimit");

	return true;
	FUNCTION_END;
	return false;
}

bool CDOSConfig::ReadProxyConfig(xml_node& XMLContent, CLIENT_PROXY_PLUGIN_INFO& Config)
{
	FUNCTION_BEGIN;

	if (XMLContent.has_attribute("ProxyType"))
		Config.ProxyType = XMLContent.attribute("ProxyType");

	if (XMLContent.has_attribute("ProxyMode"))
		Config.ProxyMode = (CLIENT_PROXY_MODE)((int)XMLContent.attribute("ProxyMode"));

	if (XMLContent.has_attribute("ListenIP"))
		Config.ListenAddress.SetIP(XMLContent.attribute("ListenIP").getvalue());

	if (XMLContent.has_attribute("ListenPort"))
		Config.ListenAddress.SetPort((WORD)XMLContent.attribute("ListenPort"));

	if (XMLContent.has_attribute("ConnectionGroupCount"))
		Config.ConnectionGroupCount = (UINT)XMLContent.attribute("ConnectionGroupCount");

	if (XMLContent.has_attribute("ConnectionPoolStartSize"))
		Config.ConnectionPoolSetting.StartSize = (UINT)XMLContent.attribute("ConnectionPoolStartSize");

	if (XMLContent.has_attribute("ConnectionPoolGrowSize"))
		Config.ConnectionPoolSetting.GrowSize = (UINT)XMLContent.attribute("ConnectionPoolGrowSize");

	if (XMLContent.has_attribute("ConnectionPoolGrowLimit"))
		Config.ConnectionPoolSetting.GrowLimit = (UINT)XMLContent.attribute("ConnectionPoolGrowLimit");

	if (XMLContent.has_attribute("ConnectionMsgQueueSize"))
		Config.ConnectionMsgQueueSize = (UINT)XMLContent.attribute("ConnectionMsgQueueSize");

	if (XMLContent.has_attribute("AcceptQueueSize"))
		Config.AcceptQueueSize = (UINT)XMLContent.attribute("AcceptQueueSize");

	if (XMLContent.has_attribute("ParallelAcceptCount"))
		Config.ParallelAcceptCount = (UINT)XMLContent.attribute("ParallelAcceptCount");

	if (XMLContent.has_attribute("RecvBufferSize"))
		Config.RecvBufferSize = (UINT)XMLContent.attribute("RecvBufferSize");

	if (XMLContent.has_attribute("SendBufferSize"))
		Config.SendBufferSize = (UINT)XMLContent.attribute("SendBufferSize");

	if (XMLContent.has_attribute("UnacceptConnectionKeepTime"))
		Config.UnacceptConnectionKeepTime = XMLContent.attribute("UnacceptConnectionKeepTime");

	if (XMLContent.has_attribute("UseServerInitiativeKeepAlive"))
		Config.UseServerInitiativeKeepAlive = XMLContent.attribute("UseServerInitiativeKeepAlive");

	if (XMLContent.has_attribute("KeepAliveCount"))
		Config.KeepAliveCount = (UINT)XMLContent.attribute("KeepAliveCount");

	if (XMLContent.has_attribute("KeepAliveTime"))
		Config.KeepAliveTime = (UINT)XMLContent.attribute("KeepAliveTime");

	if (XMLContent.has_attribute("MsgCompressType"))
	{
		CEasyString TypeStr = XMLContent.attribute("MsgCompressType").getvalue();
		if (TypeStr.CompareNoCase("lzo") == 0)
			Config.MsgCompressType = MSG_COMPRESS_LZO;
		else if (TypeStr.CompareNoCase("zip_fast") == 0)
			Config.MsgCompressType = MSG_COMPRESS_ZIP_FAST;
		else if (TypeStr.CompareNoCase("zip") == 0)
			Config.MsgCompressType = MSG_COMPRESS_ZIP_NORMAL;
		else if (TypeStr.CompareNoCase("zip_slow") == 0)
			Config.MsgCompressType = MSG_COMPRESS_ZIP_SLOW;
		else
			Config.MsgCompressType = MSG_COMPRESS_NONE;
	}

	if (XMLContent.has_attribute("MinMsgCompressSize"))
		Config.MinMsgCompressSize = XMLContent.attribute("MinMsgCompressSize");

	if (XMLContent.has_attribute("MsgQueueSize"))
		Config.MsgQueueSize = (UINT)XMLContent.attribute("MsgQueueSize");

	if (XMLContent.has_attribute("GlobalMsgMapSize"))
		Config.GlobalMsgMapSize = (UINT)XMLContent.attribute("GlobalMsgMapSize");

	if (XMLContent.has_attribute("MsgMapSize"))
		Config.MsgMapSize = (UINT)XMLContent.attribute("MsgMapSize");

	if (XMLContent.has_attribute("MaxMsgSize"))
		Config.MaxMsgSize = (UINT)XMLContent.attribute("MaxMsgSize");

	if (XMLContent.has_attribute("PluginName"))
	{
		Config.PluginName = XMLContent.attribute("PluginName").getvalue();
	}
	if (XMLContent.has_attribute("PluginModuleFileName"))
	{
		Config.ModuleFileName = XMLContent.attribute("PluginModuleFileName").getvalue();
	}
	if (XMLContent.has_attribute("ConfigDir"))
	{
		Config.ConfigDir = XMLContent.attribute("ConfigDir").getvalue();
	}
	if (XMLContent.has_attribute("LogDir"))
	{
		Config.LogDir = XMLContent.attribute("LogDir").getvalue();
	}

	return true;
	FUNCTION_END;
	return false;
}
bool CDOSConfig::LoadLibInfo(xml_node& XMLContent)
{
	for (UINT i = 0; i < XMLContent.children(); i++)
	{
		xml_node Lib = XMLContent.child(i);
		if (_stricmp(Lib.name(), "Lib") == 0)
		{
			if (Lib.has_attribute("Name"))
			{
				LIB_INFO LibInfo;

				LibInfo.LibName = Lib.attribute("Name").getvalue();
				LibInfo.LibName.Trim();
				if (!LibInfo.LibName.IsEmpty())
				{
					if (Lib.has_attribute("NeedCompile"))
						LibInfo.NeedCompile = Lib.attribute("NeedCompile");

					if (Lib.has_attribute("OutDir"))
						LibInfo.OutDir = Lib.attribute("OutDir").getvalue();

					if (Lib.has_attribute("PrjDir"))
						LibInfo.PrjDir = Lib.attribute("PrjDir").getvalue();

					for (UINT j = 0; j < Lib.children(); j++)
					{
						xml_node SourceDir = Lib.child(j);
						if (_stricmp(SourceDir.name(), "SourceDir") == 0)
						{
							LibInfo.SourceDirs.Add(SourceDir.attribute("Name").getvalue());
						}
					}
					if (LibInfo.SourceDirs.GetCount())
						m_LibList.Add(LibInfo);
				}
			}
		}
	}
	return true;
}
bool CDOSConfig::LoadPluginInfo(xml_node& XMLContent)
{
	if (XMLContent.has_attribute("ExistWhenNoPlugin"))
		m_ExistWhenNoPlugin = XMLContent.attribute("ExistWhenNoPlugin");

	for (UINT i = 0; i < XMLContent.children(); i++)
	{
		xml_node Plugin = XMLContent.child(i);
		if (_stricmp(Plugin.name(), "PluginObjectPool") == 0)
		{
			ReadPoolConfig(Plugin, m_PluginObjectPoolConfig);
		}
		else if (_stricmp(Plugin.name(), "Plugin") == 0)
		{
			PLUGIN_INFO PluginInfo;
			if (Plugin.has_attribute("Type"))
			{
				CEasyString TypeName = Plugin.attribute("Type").getvalue();
				if (TypeName.CompareNoCase("CSharp") == 0)
					PluginInfo.PluginType = PLUGIN_TYPE_CSHARP;
			}				
			if (Plugin.has_attribute("Name"))
			{
				PluginInfo.PluginName = Plugin.attribute("Name").getvalue();
			}
			if (Plugin.has_attribute("ModuleFileName"))
			{
				PluginInfo.ModuleFileName = Plugin.attribute("ModuleFileName").getvalue();
			}
			if (Plugin.has_attribute("ConfigDir"))
			{
				PluginInfo.ConfigDir = Plugin.attribute("ConfigDir").getvalue();
			}
			if (Plugin.has_attribute("LogDir"))
			{
				PluginInfo.LogDir = Plugin.attribute("LogDir").getvalue();
			}

			if (Plugin.has_attribute("LoadType"))
			{
				CEasyString TypeName = Plugin.attribute("LoadType").getvalue();
				if (TypeName.CompareNoCase("SourceCode") == 0)
					PluginInfo.LoadType = PLUGIN_LOAD_TYPE_SOURCE_CODE;
			}			
			if (Plugin.has_attribute("MainClassNameSpace"))
			{
				PluginInfo.MainClassNameSpace = Plugin.attribute("MainClassNameSpace").getvalue();
			}
			if (Plugin.has_attribute("MainClass"))
			{
				PluginInfo.MainClass = Plugin.attribute("MainClass").getvalue();
			}

			for (UINT j = 0; j< Plugin.children(); j++)
			{
				xml_node SourceDir = Plugin.child(j);
				if (_stricmp(SourceDir.name(), "SourceDir") == 0)
				{
					PluginInfo.SourceDirs.Add(SourceDir.attribute("Name").getvalue());
				}
			}


			m_PluginList.Add(PluginInfo);
		}
	}

	return true;
}