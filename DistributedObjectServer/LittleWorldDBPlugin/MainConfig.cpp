#include "StdAfx.h"

CMainConfig::CMainConfig(void)
{
	//FUNCTION_BEGIN;

	m_IsPlayerCharDataDBEnable=false;
	m_PlayerCharDataDBConfig.DBType=DB_TYPE_MYSQL;
	m_PlayerCharDataDBConfig.ThreadCount=2;
	m_PlayerCharDataDBConfig.QueueSize=128;
	m_PlayerCharDataDBConfig.RunLimit=16;
	m_PlayerCharDataDBConfig.IsLogPerformance=true;

	//FUNCTION_END;
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
		xml_node Config=Parser.document();
		if(Config.moveto_child("Main"))
		{

			m_IsPlayerCharDataDBEnable=Config.attribute("PlayerCharDataDBEnable");

			xml_node PlayerCharDataDB=Config;
			if(PlayerCharDataDB.moveto_child("PlayerCharDataDB"))
			{
				CEasyString DBType=PlayerCharDataDB.attribute("DBType").getvalue();
				DBType.Trim();
				DBType.MakeUpper();
				if(DBType=="MSSQL")
					m_PlayerCharDataDBConfig.DBType=DB_TYPE_MSSQL;
				else
					m_PlayerCharDataDBConfig.DBType=DB_TYPE_MYSQL;

				m_PlayerCharDataDBConfig.DBConnectString=PlayerCharDataDB.attribute("DBConnectString").getvalue();

				if(PlayerCharDataDB.has_attribute("ThreadCount"))
					m_PlayerCharDataDBConfig.ThreadCount=PlayerCharDataDB.attribute("ThreadCount");
				if(PlayerCharDataDB.has_attribute("QueueSize"))
					m_PlayerCharDataDBConfig.QueueSize=PlayerCharDataDB.attribute("QueueSize");
				if(PlayerCharDataDB.has_attribute("IsLogPerformance"))
					m_PlayerCharDataDBConfig.IsLogPerformance=PlayerCharDataDB.attribute("IsLogPerformance");
			}

		}
		else
		{
			Log("配置文件[%s]不合法",FileName);
			return false;
		}

	}
	else
	{
		Log("无法打开配置文件[%s]",FileName);
		return false;
	}
	return true;
	FUNCTION_END;
	return false;
}


