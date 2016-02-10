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
#include "SystemConfig.h"

CSystemConfig::CSystemConfig(void)
{
	m_MainThreadProcessLimit=DEFAULT_PROCESS_LIMIT;
	m_UDPControlAddress.SetPort(0);
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
		if(Config.moveto_child("System"))
		{

			xml_node MainThread=Config;
			if(MainThread.moveto_child("MainThread"))
			{
				if(MainThread.has_attribute("ProcessLimit"))
					m_MainThreadProcessLimit=MainThread.attribute("ProcessLimit");

			}

			xml_node UDPControlAddress=Config;
			if(UDPControlAddress.moveto_child("UDPControlAddress"))
			{
				if(UDPControlAddress.has_attribute("IP"))
					m_UDPControlAddress.SetIP(UDPControlAddress.attribute("IP").getvalue());

				if(UDPControlAddress.has_attribute("Port"))
					m_UDPControlAddress.SetPort(UDPControlAddress.attribute("Port"));

			}

			xml_node LogConfig=Config;
			if(LogConfig.moveto_child("LogConfig"))
			{
				if(LogConfig.has_attribute("LogServerObjectUse"))
					m_LogServerObjectUse=(bool)LogConfig.attribute("LogServerObjectUse");

				if(LogConfig.has_attribute("PreLoadModuleSym"))
					m_PreLoadModuleSym=(bool)LogConfig.attribute("PreLoadModuleSym");

				if(LogConfig.has_attribute("MakeFullDump"))
					m_MakeFullDump=(bool)LogConfig.attribute("MakeFullDump");

				if (LogConfig.has_attribute("LogModuleSymStatus"))
					m_LogModuleSymStatus = (bool)LogConfig.attribute("LogModuleSymStatus");



				if(LogConfig.has_attribute("LogLevel"))
					m_LogLevel=LogConfig.attribute("LogLevel");

				if(LogConfig.has_attribute("ConsoleLogLevel"))
					m_ConsoleLogLevel=LogConfig.attribute("ConsoleLogLevel");
			}

			xml_node GuardThread=Config;
			if(GuardThread.moveto_child("GuardThread"))
			{
				if(GuardThread.has_attribute("KeepAliveTime"))
					m_GuardThreadKeepAliveTime=GuardThread.attribute("KeepAliveTime");

				if(GuardThread.has_attribute("KeepAliveCount"))
					m_GuardThreadKeepAliveCount=GuardThread.attribute("KeepAliveCount");
			}


			PrintImportantLog(0,"系统配置已载入");
			PrintImportantLog(0,"主线程处理限制:%u",m_MainThreadProcessLimit);
			PrintImportantLog(0,"UDP控制接口:%s:%u",m_UDPControlAddress.GetIPString(),m_UDPControlAddress.GetPort());
			PrintImportantLog(0,"是否记录OverLapped对象使用状态:%s",m_LogServerObjectUse?"是":"否");
			PrintImportantLog(0,"Log输出级别:%u",m_LogLevel);
			PrintImportantLog(0,"控制台Log输出级别:%u",m_ConsoleLogLevel);
			PrintImportantLog(0,"主线程死锁判定时间:%u",m_GuardThreadKeepAliveTime);
			PrintImportantLog(0,"主线程死锁判定次数:%u",m_GuardThreadKeepAliveCount);

		}
		else
		{
			PrintImportantLog(0,"配置文件[%s]不合法",ConfigFileName);
			return false;
		}

	}
	else
	{
		PrintImportantLog(0,"无法打开配置文件[%s]",ConfigFileName);
		return false;
	}
	return true;
}
