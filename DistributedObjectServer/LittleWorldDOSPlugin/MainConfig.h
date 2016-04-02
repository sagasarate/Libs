#pragma once



class CMainConfig :
	public CStaticObject<CMainConfig>
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
			GrowSize=0;
			GrowLimit=0;
		}
	};
	struct MAP_AREA_CONFIG
	{
		bool		Enable;
		float		MapStartX;
		float		MapStartY;
		float		MapWidth;
		float		MapHeight;
		float		CollisionGridSize;
		float		EventGridSize;
		float		VisibleRange;
		POOL_CONFIG	MapObjectPool;
		POOL_CONFIG	ObjectPoolInEventGrid;
		MAP_AREA_CONFIG()
		{
			Enable=true;
			MapStartX=0;
			MapStartY=0;
			MapWidth=0;
			MapHeight=0;
			CollisionGridSize=1;
			EventGridSize=8;
			VisibleRange=32;
		}
	};

	struct PLAYER_MANAGER_CONFIG
	{
		bool		Enable;
		POOL_CONFIG	PlayerPool;
		PLAYER_MANAGER_CONFIG()
		{
			Enable=true;			
		}
	};

	struct NPC_MANAGER_CONFIG
	{
		bool		Enable;
		POOL_CONFIG	NPCPool;
		NPC_MANAGER_CONFIG()
		{
			Enable=true;			
		}
	};

	struct CHAR_QUERY_CENTER_CONFIG
	{
		bool		Enable;
		POOL_CONFIG	QueryPool;
		CHAR_QUERY_CENTER_CONFIG()
		{
			Enable=true;
		}
	};

	struct BORN_POINT
	{
		UINT64		MapID;
		float		PosX;
		float		PosY;
		float		Height;
		float		Dir;
	};
protected:
	MAP_AREA_CONFIG				m_MapAreaConfig;
	PLAYER_MANAGER_CONFIG		m_PlayerManagerConfig;
	NPC_MANAGER_CONFIG			m_NPCManagerConfig;
	CHAR_QUERY_CENTER_CONFIG	m_CharQueryCenterConfig;
	CEasyArray<BORN_POINT>		m_PlayerBormPoints;
public:
	CMainConfig(void);
	~CMainConfig(void);

	bool LoadConfig(LPCTSTR FileName);

	const MAP_AREA_CONFIG& GetMapAreaConfig()
	{
		return m_MapAreaConfig;
	}
	const PLAYER_MANAGER_CONFIG& GetPlayerManagerConfig()
	{
		return m_PlayerManagerConfig;
	}
	const NPC_MANAGER_CONFIG& GetNPCManagerConfig()
	{
		return m_NPCManagerConfig;
	}
	const CHAR_QUERY_CENTER_CONFIG& GetCharQueryCenterConfig()
	{
		return m_CharQueryCenterConfig;
	}
	bool GetRandomBornPoint(BORN_POINT& Point)
	{
		if(m_PlayerBormPoints.GetCount())
		{
			UINT Index = GetRand((UINT)0, (UINT)m_PlayerBormPoints.GetCount() - 1);
			Point=m_PlayerBormPoints[Index];
			return true;
		}
		return false;
	}
protected:
	bool ReadPoolConfig(xml_node& XMLContent,POOL_CONFIG& Config);
};
