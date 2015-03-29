#include "StdAfx.h"

CMainConfig::CMainConfig(void)
{
	FUNCTION_BEGIN;

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
		xml_node Config=Parser.document();
		if(Config.moveto_child("Main"))
		{
			xml_node MapArea=Config;
			if(MapArea.moveto_child("MapArea"))
			{
				if(MapArea.has_attribute("Enable"))
					m_MapAreaConfig.Enable=MapArea.attribute("Enable");
				if(!MapArea.has_attribute("MapStartX"))
				{
					Log("配置文件[%s]缺少MapStartX项",FileName);
					return false;
				}
				m_MapAreaConfig.MapStartX=MapArea.attribute("MapStartX");
				if(!MapArea.has_attribute("MapStartY"))
				{
					Log("配置文件[%s]缺少MapStartY项",FileName);
					return false;
				}
				m_MapAreaConfig.MapStartY=MapArea.attribute("MapStartY");
				if(!MapArea.has_attribute("MapWidth"))
				{
					Log("配置文件[%s]缺少MapWidth项",FileName);
					return false;
				}
				m_MapAreaConfig.MapWidth=MapArea.attribute("MapWidth");
				if(!MapArea.has_attribute("MapHeight"))
				{
					Log("配置文件[%s]缺少MapHeight项",FileName);
					return false;
				}
				m_MapAreaConfig.MapHeight=MapArea.attribute("MapHeight");

				if(MapArea.has_attribute("CollisionGridSize"))
					m_MapAreaConfig.CollisionGridSize=MapArea.attribute("CollisionGridSize");
				if(MapArea.has_attribute("EventGridSize"))
					m_MapAreaConfig.EventGridSize=MapArea.attribute("EventGridSize");
				if(MapArea.has_attribute("VisibleRange"))
					m_MapAreaConfig.VisibleRange=MapArea.attribute("VisibleRange");

				xml_node PoolConfig=MapArea;
				if(PoolConfig.moveto_child("MapObjectPool"))
				{
					ReadPoolConfig(PoolConfig,m_MapAreaConfig.MapObjectPool);
				}
				PoolConfig=MapArea;
				if(PoolConfig.moveto_child("ObjectPoolInEventGrid"))
				{
					ReadPoolConfig(PoolConfig,m_MapAreaConfig.ObjectPoolInEventGrid);
				}
			}

			xml_node BornPoints=Config;
			if(BornPoints.moveto_child("BornPoints"))
			{
				m_PlayerBormPoints.Clear();
				for(UINT i=0;i<BornPoints.children();i++)
				{
					xml_node BornPoint=BornPoints.child(i);

					if(BornPoint.has_name("BornPoint"))
					{
						BORN_POINT Point;

						Point.MapID=BornPoint.attribute("MapID");
						Point.PosX=BornPoint.attribute("PosX");
						Point.PosY=BornPoint.attribute("PosY");
						Point.Height=BornPoint.attribute("Height");
						Point.Dir=BornPoint.attribute("Dir");

						m_PlayerBormPoints.Add(Point);
					}
				}
			}

			xml_node PlayerManager=Config;
			if(PlayerManager.moveto_child("PlayerManager"))
			{
				if(PlayerManager.has_attribute("Enable"))
					m_PlayerManagerConfig.Enable=PlayerManager.attribute("Enable");
				xml_node PoolConfig=PlayerManager;
				if(PoolConfig.moveto_child("PlayerPool"))
				{
					ReadPoolConfig(PoolConfig,m_PlayerManagerConfig.PlayerPool);
				}
				
			}

			xml_node NPCManager=Config;
			if(NPCManager.moveto_child("NPCManager"))
			{
				if(NPCManager.has_attribute("Enable"))
					m_NPCManagerConfig.Enable=NPCManager.attribute("Enable");
				xml_node PoolConfig=NPCManager;
				if(PoolConfig.moveto_child("NPCPool"))
				{
					ReadPoolConfig(PoolConfig,m_NPCManagerConfig.NPCPool);
				}
			}
			xml_node CharQueryCenter=Config;
			if(CharQueryCenter.moveto_child("CharQueryCenter"))
			{
				if(CharQueryCenter.has_attribute("Enable"))
					m_CharQueryCenterConfig.Enable=CharQueryCenter.attribute("Enable");
				xml_node PoolConfig=CharQueryCenter;
				if(PoolConfig.moveto_child("QueryPool"))
				{
					ReadPoolConfig(PoolConfig,m_CharQueryCenterConfig.QueryPool);
				}
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