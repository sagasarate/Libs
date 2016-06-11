#pragma once



class CMainConfig :
	public CStaticObject<CMainConfig>
{
public:
	enum DB_TYPE
	{
		DB_TYPE_MSSQL,
		DB_TYPE_MYSQL,
	};
	struct DB_CONNECT_CONFIG
	{
		int				DBType;
		CEasyString		DBConnectString;
		UINT			ThreadCount;
		UINT			QueueSize;
		UINT			RunLimit;
		bool			IsLogPerformance;
	};
protected:
	bool					m_IsPlayerCharDataDBEnable;
	DB_CONNECT_CONFIG		m_PlayerCharDataDBConfig;
public:
	CMainConfig(void);
	~CMainConfig(void);

	bool LoadConfig(LPCTSTR FileName);

	bool IsPlayerCharDataDBEnable()
	{
		return m_IsPlayerCharDataDBEnable;
	}
	DB_CONNECT_CONFIG& GetPlayerCharDataDBConfig()
	{
		return m_PlayerCharDataDBConfig;
	}
	
};
