/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLinkManager.cpp                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"





IMPLEMENT_CLASS_INFO_STATIC(CEasyNetLinkManager,CNameObject);

CEasyNetLinkManager::CEasyNetLinkManager(void)
{
	m_pServer=NULL;
}

CEasyNetLinkManager::~CEasyNetLinkManager(void)
{
	Destory();
}


bool CEasyNetLinkManager::Init(CNetServer * pServer, LPCTSTR ConfigFileName)
{
	ENL_CONFIG ConfigData;
	if (!LoadConfig(ConfigFileName, ConfigData))
		return false;

	return Init(pServer, ConfigData);
}

bool CEasyNetLinkManager::Init(CNetServer * pServer, xml_node& Config)
{
	ENL_CONFIG ConfigData;
	if (!LoadConfig(Config, ConfigData))
		return false;

	return Init(pServer, ConfigData);
}

bool CEasyNetLinkManager::Init(CNetServer * pServer, const ENL_CONFIG& Config)
{
	m_pServer = pServer;
	if (m_pServer == NULL)
		return false;

	if (Config.ServerID == 0)
	{
		return false;
	}
	

	if (!m_LinkIDPool.Create(Config.ReallocIDRange))
	{
		PrintNetLog( _T("CEasyNetLinkManager::Init 创建[%u]大小的LinkIDPool失败"),
			Config.ReallocIDRange);
		return false;
	}
	

	for (UINT i = 0; i < Config.ServiceConfig.ServiceList.GetCount(); i++)
	{
		const ENL_SERVICE& ServiceConfig = Config.ServiceConfig.ServiceList[i];

		CEasyNetLinkService * pService = AddService(ServiceConfig.ServiceID, ServiceConfig.ReportID, ServiceConfig.ListenAddress, 
			ServiceConfig.NeedReallocConnectionID, ServiceConfig.MaxPacketSize, ServiceConfig.IsUseListenThread,
			ServiceConfig.ParallelAcceptCount, ServiceConfig.AcceptQueueSize, ServiceConfig.RecvQueueSize, ServiceConfig.SendQueueSize, 
			ServiceConfig.DataCompressType, ServiceConfig.MinCompressSize);
		if (pService)
		{
			pService->SetIPList(ServiceConfig.IPPatternList);
		}
		else
		{
			PrintNetLog( _T("CEasyNetLinkManager::Init 创建Service[%s][%s:%u]失败"),
				CClassifiedID::IDToStr(ServiceConfig.ServiceID), ServiceConfig.ListenAddress.GetIPString(), ServiceConfig.ListenAddress.GetPort());
			return false;
		}
	}

	for (UINT i = 0; i < Config.ConnectionConfig.ConnectionList.GetCount(); i++)
	{
		const ENL_CONNECTION& ConnectionConfig = Config.ConnectionConfig.ConnectionList[i];
		if (!AddLink(ConnectionConfig.ReportID, ConnectionConfig.Address, ConnectionConfig.MaxPacketSize, ConnectionConfig.RecvQueueSize, 
			ConnectionConfig.SendQueueSize, ConnectionConfig.DataCompressType, ConnectionConfig.MinCompressSize))
		{
			PrintNetLog( _T("CEasyNetLinkManager::Init 创建Connection[%s][%s:%u]失败"),
				CClassifiedID::IDToStr(ConnectionConfig.ReportID), ConnectionConfig.Address.GetIPString(), ConnectionConfig.Address.GetPort());
			return false;
		}
	}
		
		
	
	return true;
}

bool CEasyNetLinkManager::Init(CNetServer * pServer)
{
	m_pServer=pServer;
	if(m_pServer==NULL)
		return false;
	return true;
}
bool CEasyNetLinkManager::LoadConfig(LPCTSTR ConfigFileName, ENL_CONFIG& Config)
{
	xml_parser Parser;

	if (!Parser.parse_file(ConfigFileName, pug::parse_trim_attribute))
	{
		return false;
	}
	xml_node Root = Parser.document();
	if (!Root.moveto_child(_T("EasyLink")))
		return false;

	return LoadConfig(Root, Config);
}
bool CEasyNetLinkManager::LoadConfig(xml_node& XmlRoot, ENL_CONFIG& Config)
{
	if (!XmlRoot.has_attribute(_T("ServerID")))
	{
		return false;
	}

	
	Config.ServerID = CClassifiedID::StrToID(XmlRoot.attribute(_T("ServerID")).getvalue());

	if (XmlRoot.has_attribute(_T("ReallocIDRange")))
	{
		Config.ReallocIDRange = XmlRoot.attribute(_T("ReallocIDRange"));
	}
	

	for (UINT i = 0; i < XmlRoot.children(); i++)
	{
		if (_tcsnicmp(XmlRoot.child(i).name(), _T("Services"), 8) == 0)
		{
			xml_node Services = XmlRoot.child(i);
			

			for (UINT j = 0; j < Services.children(); j++)
			{
				if (_tcsnicmp(Services.child(j).name(), _T("Service"), 7) == 0)
				{
					xml_node Service = Services.child(j);
					if (Service.has_attribute(_T("ID")) &&
						Service.has_attribute(_T("Port")))
					{
						ENL_SERVICE ServiceConfig;
						ServiceConfig.ServiceID = CClassifiedID::StrToID(Service.attribute(_T("ID")).getvalue());

						if (Services.has_attribute(_T("ReportID")))
						{
							ServiceConfig.ReportID = CClassifiedID::StrToID(Services.attribute(_T("ReportID")).getvalue());
						}
						else
						{
							ServiceConfig.ReportID = Config.ServerID;
						}

						if (Services.has_attribute(_T("ReallocConnectionID")))
							ServiceConfig.NeedReallocConnectionID = Services.attribute(_T("ReallocConnectionID"));

						
						if (Service.has_attribute(_T("IP")))
						{
							CEasyString IPStr = Service.attribute(_T("IP")).getvalue();
							ServiceConfig.ListenAddress.SetIP(IPStr);
						}
						ServiceConfig.ListenAddress.SetPort(Service.attribute(_T("Port")));

						if (Service.has_attribute(_T("MaxPacketSize")))
							ServiceConfig.MaxPacketSize = Service.attribute(_T("MaxPacketSize"));
						if (Service.has_attribute(_T("IsUseListenThread")))
							ServiceConfig.IsUseListenThread = (bool)Service.attribute(_T("IsUseListenThread"));
						if (Service.has_attribute(_T("ParallelAcceptCount")))
							ServiceConfig.ParallelAcceptCount = Service.attribute(_T("ParallelAcceptCount"));
						if (Service.has_attribute(_T("AcceptQueueSize")))
							ServiceConfig.AcceptQueueSize = Service.attribute(_T("AcceptQueueSize"));
						if (Service.has_attribute(_T("RecvQueueSize")))
							ServiceConfig.RecvQueueSize = Service.attribute(_T("RecvQueueSize"));
						if (Service.has_attribute(_T("SendQueueSize")))
							ServiceConfig.SendQueueSize = Service.attribute(_T("SendQueueSize"));
						
						if (Service.has_attribute(_T("DataCompressType")))
						{
							if (Service.attribute(_T("DataCompressType")).getvalue().CompareNoCase(_T("lzo")) == 0)
								ServiceConfig.DataCompressType = DATA_COMPRESS_TYPE_LZO;
						}
						if (Service.has_attribute(_T("MinCompressSize")))
						{
							ServiceConfig.MinCompressSize = Service.attribute(_T("MinCompressSize"));
						}
						
						for (UINT k = 0; k < Service.children(); k++)
						{
							if (_tcsnicmp(Service.child(k).name(), _T("IPList"), 6) == 0)
							{
								xml_node IPList = Service.child(k);

								for (UINT l = 0; l < IPList.children(); l++)
								{
									if (_tcsnicmp(IPList.child(l).name(), _T("IP"), 2) == 0)
									{
										xml_node IP = IPList.child(l);
										if (IP.has_attribute(_T("IP")))
										{
											CEasyString IPStr = Service.attribute(_T("IP")).getvalue();
											ServiceConfig.IPPatternList.Add(CIPPattern(IPStr));
										}
									}
								}
							}
						}

						Config.ServiceConfig.ServiceList.Add(ServiceConfig);
					}
				}
			}
		}
		if (_tcsnicmp(XmlRoot.child(i).name(), _T("Connections"), 11) == 0)
		{
			xml_node Connections = XmlRoot.child(i);

			for (UINT j = 0; j < Connections.children(); j++)
			{
				if (_tcsnicmp(Connections.child(j).name(), _T("Connection"), 10) == 0)
				{
					xml_node Connection = Connections.child(j);
					if (Connection.has_attribute(_T("IP")) &&
						Connection.has_attribute(_T("Port")))
					{
						ENL_CONNECTION ConnectionConfig;

						if (Connections.has_attribute(_T("ReportID")))
						{
							ConnectionConfig.ReportID = CClassifiedID::StrToID(Connections.attribute(_T("ReportID")).getvalue());
						}
						else
						{
							ConnectionConfig.ReportID = Config.ServerID;
						}


						
						CEasyString IPStr = Connection.attribute(_T("IP")).getvalue();
						ConnectionConfig.Address.SetIP(IPStr);
						ConnectionConfig.Address.SetPort(Connection.attribute(_T("Port")));
						if (Connection.has_attribute(_T("MaxPacketSize")))
							ConnectionConfig.MaxPacketSize = Connection.attribute(_T("MaxPacketSize"));
						if (Connection.has_attribute(_T("RecvQueueSize")))
							ConnectionConfig.RecvQueueSize = Connection.attribute(_T("RecvQueueSize"));
						if (Connection.has_attribute(_T("SendQueueSize")))
							ConnectionConfig.SendQueueSize = Connection.attribute(_T("SendQueueSize"));
						if (Connection.has_attribute(_T("DataCompressType")))
						{
							if (Connection.attribute(_T("DataCompressType")).getvalue().CompareNoCase(_T("lzo")) == 0)
								ConnectionConfig.DataCompressType = DATA_COMPRESS_TYPE_LZO;
						}
						if (Connection.has_attribute(_T("MinCompressSize")))
						{
							ConnectionConfig.MinCompressSize = Connection.attribute(_T("MinCompressSize"));
						}
						Config.ConnectionConfig.ConnectionList.Add(ConnectionConfig);
					}
				}
			}
		}
	}
	return true;
}

void CEasyNetLinkManager::Destory()
{
	for(int i=(int)m_LinkList.GetCount()-1;i>=0;i--)
	{
		SAFE_RELEASE(m_LinkList[i]);
	}

	for(int i=(int)m_ServiceList.GetCount()-1;i>=0;i--)
	{
		SAFE_RELEASE(m_ServiceList[i]);
	}
	m_LinkMap.Clear();
	m_LinkList.Clear();
	m_ServiceMap.Clear();
	m_ServiceList.Clear();
}

CEasyNetLinkService * CEasyNetLinkManager::AddService(UINT ID,UINT ReportID,const CIPAddress& ListenAddress,
	bool NeedReallocConnectionID, UINT MaxPacketSize, bool IsUseListenThread,
	int ParallelAcceptCount, UINT AcceptQueueSize,
	UINT RecvQueueSize, UINT SendQueueSize,
	DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	CEasyNetLinkService * pService=CreateLinkService(ID);
	if(pService)
	{
		if (pService->Init(this, ReportID, ListenAddress, NeedReallocConnectionID, IsUseListenThread, ParallelAcceptCount, AcceptQueueSize,
			RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize))
		{
			PrintNetLog( _T("CEasyNetLinkManager::AddService 创建Service[%s][%s:%u],%s,并发Accept=%d,Accept队列长度=%u,接收队列长度=%u,发送队列长度=%u,最大数据包大小=%u,压缩类型=%d,最小压缩大小=%u"),
				CClassifiedID(ID).ToStr(),
				ListenAddress.GetIPString(),
				ListenAddress.GetPort(),
				IsUseListenThread ? _T("使用侦听线程") : _T("使用IOCP侦听"),
				ParallelAcceptCount, AcceptQueueSize, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize);
			return pService;
		}
		DeleteLinkService(pService);
		SAFE_DELETE(pService);
	}
	return NULL;
}

bool CEasyNetLinkManager::AddLink(UINT ReportID, const CIPAddress& ConnectionAddress, UINT MaxPacketSize,
										UINT RecvQueueSize,UINT SendQueueSize,
										DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	CEasyNetLink * pLink=CreateAcceptLink();
	if (pLink)
	{
		if (pLink->Init(this, ConnectionAddress, ReportID, RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize))
		{
			PrintNetLog( _T("CEasyNetLinkManager::AddLink 创建Connection[%s][%s:%u],接收队列长度=%u,发送队列长度=%u,最大数据包大小=%u,压缩类型=%d,最小压缩大小=%u"),
				CClassifiedID(ReportID).ToStr(),
				ConnectionAddress.GetIPString(),
				ConnectionAddress.GetPort(),
				RecvQueueSize, SendQueueSize, MaxPacketSize, DataCompressType, MinCompressSize);
			return true;
		}
		else
		{
			DeleteLink(pLink);
			PrintNetLog( _T("CEasyNetLinkManager::AddLink 创建Connection失败"));			
		}
	}
	return false;
}

int CEasyNetLinkManager::Update(int ProcessPacketLimit)
{
	int Process=0;
	for(int i=(int)m_LinkList.GetCount()-1;i>=0;i--)
	{
		Process+=m_LinkList[i]->Update(ProcessPacketLimit);
	}
	for(size_t i=0;i<m_ServiceList.GetCount();i++)
	{
		Process+=m_ServiceList[i]->Update(ProcessPacketLimit);
	}
	return Process;
}

CEasyNetLinkService * CEasyNetLinkManager::OnCreateService(UINT ID)
{
	return new CEasyNetLinkService();
}

CEasyNetLink * CEasyNetLinkManager::GetLink(UINT ID)
{
	CEasyNetLink ** ppValue=m_LinkMap.Find(ID);
	if(ppValue)
	{
		return *ppValue;
	}
	return NULL;
}
LPVOID CEasyNetLinkManager::GetFirstLinkPos()
{
	return m_LinkMap.GetFirstObjectPos();
}
CEasyNetLink *  CEasyNetLinkManager::GetNextLink(LPVOID& Pos)
{
	UINT Key;
	CEasyNetLink ** ppValue=m_LinkMap.GetNextObject(Pos,Key);
	if(ppValue)
	{
		return *ppValue;
	}
	return NULL;
}

CEasyNetLinkService * CEasyNetLinkManager::GetService(UINT ID)
{
	CEasyNetLinkService ** ppValue=m_ServiceMap.Find(ID);
	if(ppValue)
	{
		return *ppValue;
	}
	return NULL;
}

UINT CEasyNetLinkManager::GetLinkCount()
{
	return (UINT)m_LinkList.GetCount();
}

CEasyNetLink * CEasyNetLinkManager::GetLinkByIndex(UINT Index)
{
	if(Index<m_LinkList.GetCount())
	{
		return m_LinkList[Index];
	}
	return NULL;
}

CEasyNetLink * CEasyNetLinkManager::CreateAcceptLink()
{
	CEasyNetTempLink * pLink = new CEasyNetTempLink();
	if (pLink)
	{
		pLink->SetStatus(ENL_LINK_ACCEPTING);
		m_LinkList.Add(pLink);
	}
	return pLink;
}

CEasyNetLink * CEasyNetLinkManager::CreateLink(UINT ID)
{
	CEasyNetLink * pLink = OnCreateLink(ID);
	if (pLink)
	{
		pLink->SetID(ID);
		pLink->SetStatus(ENL_LINK_ACCEPTED);
		m_LinkList.Add(pLink);
		m_LinkMap[ID] = pLink;
	}
	return pLink;
}



bool CEasyNetLinkManager::AcceptLink(UINT LinkID, CEasyNetLink * pLink)
{

	if (pLink->NeedReallocConnectionID())
	{
		//需要重新分配ID
		int * Stay;
		UINT ID=m_LinkIDPool.NewObject(&Stay);
		if(ID==0)
		{
			PrintNetLog(_T("CEasyNetLinkManager::AcceptLink 无法给连接[%s]重新分配一个ID。"),
				CClassifiedID(LinkID).ToStr());
			return false;
		}
		CClassifiedID ConnectionID = LinkID;
		ConnectionID.BigIndex() = ID;
		LinkID = ConnectionID;
	}
	
	CEasyNetLink * pExistConnection = GetLink(LinkID);
	if (pExistConnection)
	{
		if (pExistConnection == pLink)
		{
			OnLinkStart(pLink);
			pLink->OnLinkStart();
			return true;
		}
		PrintNetLog(_T("CEasyNetLinkManager::AcceptLink 连接[%s]已经存在。"),
			CClassifiedID(LinkID).ToStr());
		return false;
	}	

	//重新创建对象
	CEasyNetLink * pNewLink = CreateLink(LinkID);
	if (pNewLink)
	{
		if (pNewLink->Init(pLink))
		{
			DeleteLink(pLink);
			OnLinkStart(pNewLink);
			pNewLink->OnLinkStart();
			return true;
		}
		else
		{
			PrintNetLog( _T("CEasyNetLinkManager::AcceptLink 连接初始化失败。"));
		}
		DeleteLink(pNewLink);
	}
	else
	{
		PrintNetLog(_T("CEasyNetLinkManager::AcceptLink 无法给连接[%s]创建合适的对象。"),
			CClassifiedID(LinkID).ToStr());
	}	

	return false;

}

void CEasyNetLinkManager::DeleteLink(CEasyNetLink * pLink)
{
	if (pLink->NeedReallocConnectionID())
	{
		CClassifiedID ConnectionID = pLink->GetID();
		m_LinkIDPool.DeleteObject(ConnectionID.BigIndex());
	}

	m_LinkMap.Delete(pLink->GetID());
	for(size_t i=0;i<m_LinkList.GetCount();i++)
	{
		if (m_LinkList[i] == pLink)
		{
			m_LinkList.Delete(i);
		}
	}
	SAFE_RELEASE(pLink);
}



CEasyNetLinkService * CEasyNetLinkManager::CreateLinkService(UINT ID)
{
	if(GetService(ID))
	{
		PrintNetLog(_T("CEasyNetLinkManager::CreateService 服务[%u]已经存在。"),ID);
		return NULL;
	}

	CEasyNetLinkService * pService=OnCreateService(ID);
	if(pService)
	{
		pService->SetID(ID);
		m_ServiceMap[ID]=pService;
		m_ServiceList.Add(pService);
	}
	return pService;
}

bool CEasyNetLinkManager::DeleteLinkService(CEasyNetLinkService * pService)
{
	if(m_ServiceMap.Delete(pService->GetID()))
	{
		for(size_t i=0;i<m_ServiceList.GetCount();i++)
		{
			if(m_ServiceList[i]==pService)
			{
				m_ServiceList.Delete(i);
				break;
			}
		}
		return true;
	}
	return false;
}

void CEasyNetLinkManager::OnLinkStart(CEasyNetLink * pLink)
{
}

void CEasyNetLinkManager::OnLinkEnd(CEasyNetLink * pLink)
{
}

void CEasyNetLinkManager::PrintLinkInfo(UINT LogChannel)
{
	CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,_T("Service List:"));
	for(size_t i=0;i<m_ServiceList.GetCount();i++)
	{
		m_ServiceList[i]->PrintInfo(LogChannel);
	}
	CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,_T("Link List:"));
	for(int i=(int)m_LinkList.GetCount()-1;i>=0;i--)
	{
		m_LinkList[i]->PrintInfo(LogChannel);
	}
}
