#include "stdafx.h"

CMainConfig::CMainConfig(void)
{
	FUNCTION_BEGIN;

	m_MaxPluginObject=1024;

	FUNCTION_END;
}

CMainConfig::~CMainConfig(void)
{
}

bool CMainConfig::LoadConfig(LPCTSTR FileName)
{
	FUNCTION_BEGIN;
	xml_parser Parser;

	if(Parser.parse_file(FileName,pug::parse_trim_attribute))
	{
		m_DOSConfig.RouterLinkConfigFileName=FileName;

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
				if(Net.has_attribute("NetWorkThreadPerCPU"))
					m_NetConfig.NetWorkThreadPerCPU=Net.attribute("NetWorkThreadPerCPU");
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

			xml_node ClientProxys=Config;
			if(ClientProxys.moveto_child("ClientProxys"))
			{
				for (UINT i = 0; i < ClientProxys.children(); i++)
				{
					xml_node ClientProxy = ClientProxys.child(i);
					if (_stricmp(ClientProxys.name(), "ClientProxy"))
					{
						CLIENT_PROXY_CONFIG ProxyConfig;
						if (ReadProxyConfig(ClientProxy, ProxyConfig))
						{
							m_DOSConfig.ClientProxyConfigs.Add(ProxyConfig);
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
				if(Object.has_attribute("MaxGroupObjectCount"))
					m_DOSConfig.MaxGroupObjectCount=(UINT)Object.attribute("MaxGroupObjectCount");
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
			}
			xml_node Plugins=Config;
			if(Plugins.moveto_child("Plugins"))
			{
				if(Plugins.has_attribute("MaxPluginObject"))
					m_MaxPluginObject=(UINT)Plugins.attribute("MaxPluginObject");

				for(UINT i=0;i<Plugins.children();i++)
				{
					xml_node Plugin=Plugins.child(i);
					if(_stricmp(Plugin.name(),"Plugin")==0)
					{
						if(Plugin.has_attribute("FileName"))
						{
							m_PluginFileNameList.Add(Plugin.attribute("FileName").getvalue());
						}
					}
				}
			}
		}
		else
		{
			PrintImportantLog(0,"配置文件[%s]不合法",FileName);
			return false;
		}

	}
	else
	{
		PrintImportantLog(0,"无法打开配置文件[%s]",FileName);
		return false;
	}
	return true;
	FUNCTION_END;
	return false;
}


bool CMainConfig::ReadPoolConfig(xml_node& XMLContent,POOL_CONFIG& Config)
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

bool CMainConfig::ReadProxyConfig(xml_node& XMLContent, CLIENT_PROXY_CONFIG& Config)
{
	FUNCTION_BEGIN;

	if (XMLContent.has_attribute("ProxyType"))
		Config.ProxyType = XMLContent.attribute("ProxyType");



	if (XMLContent.has_attribute("ListenIP"))
		Config.ListenAddress.SetIP(XMLContent.attribute("ListenIP").getvalue());

	if (XMLContent.has_attribute("ListenPort"))
		Config.ListenAddress.SetPort((WORD)XMLContent.attribute("ListenPort"));

	if (XMLContent.has_attribute("MaxConnection"))
		Config.MaxConnection = (UINT)XMLContent.attribute("MaxConnection");

	if (XMLContent.has_attribute("ConnectionMsgQueueSize"))
		Config.ConnectionMsgQueueSize = (UINT)XMLContent.attribute("ConnectionMsgQueueSize");

	if (XMLContent.has_attribute("SendBufferSize"))
		Config.SendBufferSize = (UINT)XMLContent.attribute("SendBufferSize");

	if (XMLContent.has_attribute("SendDelay"))
		Config.SendDelay = (UINT)XMLContent.attribute("SendDelay");

	if (XMLContent.has_attribute("SendQueryLimit"))
		Config.SendQueryLimit = (UINT)XMLContent.attribute("SendQueryLimit");

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


	return true;
	FUNCTION_END;
	return false;
}
