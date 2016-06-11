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


