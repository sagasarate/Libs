/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemConfig.cpp                                         */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CSystemConfig::CSystemConfig(void)
{
	m_MainThreadProcessLimit=DEFAULT_PROCESS_LIMIT;
	m_UDPControlAddress.SetPort(0);
	m_ControlPipeAssembleBufferSize = 0;
	m_LogServerObjectUse=false;
	m_PreLoadModuleSym=false;
	m_MakeFullDump=false;
	m_LogModuleSymStatus=false;
#ifdef _DEBUG
	m_LogLevel=ILogPrinter::LOG_LEVEL_NORMAL|ILogPrinter::LOG_LEVEL_DEBUG;
#else
	m_LogLevel=ILogPrinter::LOG_LEVEL_NORMAL;
#endif
	m_ConsoleLogLevel=m_LogLevel;
	m_LogCacheSize = 65536;
	m_EnableGuardThread = false;
	m_GuardThreadKeepAliveTime=20*1000;
	m_GuardThreadKeepAliveCount=5;

}

CSystemConfig::~CSystemConfig(void)
{
}

bool CSystemConfig::LoadConfig(LPCTSTR ConfigFileName)
{
	xml_parser Parser;

	if(Parser.parse_file(ConfigFileName,pug::parse_trim_attribute))
	{
		xml_node Config=Parser.document();
		if (Config.moveto_child("Main"))
		{
			if (Config.moveto_child("System"))
			{

				xml_node MainThread = Config;
				if (MainThread.moveto_child("MainThread"))
				{
					if (MainThread.has_attribute("ProcessLimit"))
						m_MainThreadProcessLimit = MainThread.attribute("ProcessLimit");

				}

				xml_node UDPControlAddress = Config;
				if (UDPControlAddress.moveto_child("UDPControlAddress"))
				{
					if (UDPControlAddress.has_attribute("IP"))
						m_UDPControlAddress.SetIP(UDPControlAddress.attribute("IP").getvalue());

					if (UDPControlAddress.has_attribute("Port"))
						m_UDPControlAddress.SetPort(UDPControlAddress.attribute("Port"));

				}

				xml_node LogConfig = Config;
				if (LogConfig.moveto_child("LogConfig"))
				{
					if (LogConfig.has_attribute("LogServerObjectUse"))
						m_LogServerObjectUse = (bool)LogConfig.attribute("LogServerObjectUse");

					if (LogConfig.has_attribute("PreLoadModuleSym"))
						m_PreLoadModuleSym = (bool)LogConfig.attribute("PreLoadModuleSym");

					if (LogConfig.has_attribute("MakeFullDump"))
						m_MakeFullDump = (bool)LogConfig.attribute("MakeFullDump");

					if (LogConfig.has_attribute("LogModuleSymStatus"))
						m_LogModuleSymStatus = (bool)LogConfig.attribute("LogModuleSymStatus");



					if (LogConfig.has_attribute("LogLevel"))
						m_LogLevel = LogConfig.attribute("LogLevel");

					if (LogConfig.has_attribute("ConsoleLogLevel"))
						m_ConsoleLogLevel = LogConfig.attribute("ConsoleLogLevel");

					if (LogConfig.has_attribute("LogCacheSize"))
						m_LogCacheSize = LogConfig.attribute("LogCacheSize");
					
				}

				xml_node GuardThread = Config;
				if (GuardThread.moveto_child("GuardThread"))
				{
					if (GuardThread.has_attribute("Enable"))
						m_EnableGuardThread = GuardThread.attribute("Enable");

					if (GuardThread.has_attribute("KeepAliveTime"))
						m_GuardThreadKeepAliveTime = GuardThread.attribute("KeepAliveTime");

					if (GuardThread.has_attribute("KeepAliveCount"))
						m_GuardThreadKeepAliveCount = GuardThread.attribute("KeepAliveCount");
				}

				xml_node ControlPipe = Config;
				if (ControlPipe.moveto_child("ControlPipe"))
				{
					if (ControlPipe.has_attribute("Name"))
						m_ControlPipeName = ControlPipe.attribute("Name").getvalue();

					if (ControlPipe.has_attribute("AssembleBufferSize"))
						m_ControlPipeAssembleBufferSize = ControlPipe.attribute("AssembleBufferSize");
				}

				xml_node MallocConfig = Config;
				if (MallocConfig.moveto_child("MallocConfig"))
				{
					if (MallocConfig.has_attribute("TrimThreshold"))
					{
						m_MallocConfig.bSetTrimThreshold = true;
						m_MallocConfig.TrimThreshold = MallocConfig.attribute("TrimThreshold");
					}
						

					if (MallocConfig.has_attribute("MMapThreshold"))
					{
						m_MallocConfig.bSetMMapThreshold = true;
						m_MallocConfig.MMapThreshold = MallocConfig.attribute("MMapThreshold");
					}
						
					if (MallocConfig.has_attribute("MMapMax"))
					{
						m_MallocConfig.bSetMMapMax = true;
						m_MallocConfig.MMapMax = MallocConfig.attribute("MMapMax");
					}
				}

				PrintImportantLog("系统配置已载入");
				PrintImportantLog("主线程处理限制:%u", m_MainThreadProcessLimit);
				PrintImportantLog("UDP控制接口:%s:%u", m_UDPControlAddress.GetIPString(), m_UDPControlAddress.GetPort());
				PrintImportantLog("控制管道名称:%s", (LPCTSTR)m_ControlPipeName);
				PrintImportantLog("控制管道消息装配缓冲大小:%u", m_ControlPipeAssembleBufferSize);
				PrintImportantLog("是否记录OverLapped对象使用状态:%s", m_LogServerObjectUse ? "是" : "否");
				PrintImportantLog("Log输出级别:%u", m_LogLevel);
				PrintImportantLog("控制台Log输出级别:%u", m_ConsoleLogLevel);
				PrintImportantLog("Log缓冲大小:%u", m_LogCacheSize);
				PrintImportantLog("主线程死锁判定时间:%u", m_GuardThreadKeepAliveTime);
				PrintImportantLog("主线程死锁判定次数:%u", m_GuardThreadKeepAliveCount);
				PrintImportantLog("是否与加载模块符号:%s", m_PreLoadModuleSym ? "是" : "否");
				PrintImportantLog("是否输出符号加载日志:%s", m_LogModuleSymStatus ? "是" : "否");
				PrintImportantLog("异常时是否输出完整内存镜像:%s", m_MakeFullDump ? "是" : "否");
				if (m_MallocConfig.bSetTrimThreshold)
					PrintImportantLog("设置GLIBC内存堆收缩门槛:%d", m_MallocConfig.TrimThreshold);
				if (m_MallocConfig.bSetMMapThreshold)
					PrintImportantLog("设置GLIBC内存堆分配门槛:%d", m_MallocConfig.MMapThreshold);
				if (m_MallocConfig.bSetMMapMax)
					PrintImportantLog("设置GLIBC内存堆最大大小:%d", m_MallocConfig.MMapMax);
			}
			else
			{
				PrintImportantLog("配置文件[%s]不合法", ConfigFileName);
				return false;
			}
		}
		else
		{
			PrintImportantLog("配置文件[%s]不合法", ConfigFileName);
			return false;
		}
	}
	else
	{
		PrintImportantLog("无法打开配置文件[%s]",ConfigFileName);
		return false;
	}
	return true;
}
